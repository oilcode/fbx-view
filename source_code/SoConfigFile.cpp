//------------------------------------------------------------
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif //_CRT_SECURE_NO_WARNINGS
//------------------------------------------------------------
#include "SoConfigFile.h"
#include <stdio.h>
//------------------------------------------------------------
//一块一块的从磁盘上读取文件内容，块的大小为下面这个值。
#define SoConfigFile_BuffSize 8000
//文件中一行的最大字符个数，包括字符串结束符。
#define SoConfigFile_LineSize 1000
//------------------------------------------------------------
SoConfigFile::SoConfigFile()
{
	ClearConfig();
}
//------------------------------------------------------------
SoConfigFile::~SoConfigFile()
{
	WriteFile();
	ClearConfig();
}
//------------------------------------------------------------
void SoConfigFile::ReadFile(const char* pFileName)
{
	if (pFileName == 0)
	{
		return;
	}

	ClearConfig();
	m_strFileName = pFileName;

	FILE* pFile = fopen(pFileName, "r"); //必须是以文本方式去读文件，不能是以二进制方式。
	if (pFile != 0)
	{
		char* pBuff = (char*)malloc(SoConfigFile_BuffSize);
		m_pFileLineBuff = (char*)malloc(SoConfigFile_LineSize);
		m_nValidLineSize = 0;

		while (1)
		{
			const int nReadCount = (int)fread(pBuff, 1, SoConfigFile_BuffSize, pFile);
			const bool bEndOfFile = (nReadCount < SoConfigFile_BuffSize);

			if (nReadCount > 0)
			{
				ParseFileChunk(pBuff, nReadCount, bEndOfFile);
			}

			if (bEndOfFile)
			{
				break;
			}
		}

		fclose(pFile);
		free(pBuff);
		free(m_pFileLineBuff);
		m_pFileLineBuff = 0;
		m_nValidLineSize = 0;
	}
}
//------------------------------------------------------------
void SoConfigFile::WriteFile()
{
	if (m_bDirty == false)
	{
		return;
	}

	FILE* pFile = fopen(m_strFileName.c_str(), "w");
	if (pFile == NULL)
	{
		return;
	}

	m_pFileLineBuff = (char*)malloc(SoConfigFile_LineSize);
	m_nValidLineSize = 0;

	for (mapKey2Value::iterator it = m_mapKey2Value.begin();
		it != m_mapKey2Value.end();
		++it)
	{
		sprintf(m_pFileLineBuff, "%s=%s\n", it->first.c_str(), it->second.c_str());
		int nLen = it->first.length() + it->second.length() + 2;
		fwrite(m_pFileLineBuff, 1, nLen, pFile);
	}

	fclose(pFile);
	free(m_pFileLineBuff);
	m_pFileLineBuff = 0;
	m_nValidLineSize = 0;
}
//------------------------------------------------------------
int SoConfigFile::GetValue_int(const std::string& strKey, const int nDefaultValue)
{
	int nValue = nDefaultValue;
	mapKey2Value::iterator it = m_mapKey2Value.find(strKey);
	if (it != m_mapKey2Value.end())
	{
		const std::string& strValue = it->second;
		nValue = atoi(strValue.c_str());
	}
	return nValue;
}
//------------------------------------------------------------
float SoConfigFile::GetValue_float(const std::string& strKey, const float fDefaultValue)
{
	float fValue = fDefaultValue;
	mapKey2Value::iterator it = m_mapKey2Value.find(strKey);
	if (it != m_mapKey2Value.end())
	{
		const std::string& strValue = it->second;
		fValue = (float)atof(strValue.c_str());
	}
	return fValue;
}
//------------------------------------------------------------
const char* SoConfigFile::GetValue_string(const std::string& strKey, const char* pszDefaultValue)
{
	const char* pValue = pszDefaultValue;
	mapKey2Value::iterator it = m_mapKey2Value.find(strKey);
	if (it != m_mapKey2Value.end())
	{
		const std::string& strValue = it->second;
		pValue = strValue.c_str();
	}
	return pValue;
}
//------------------------------------------------------------
void SoConfigFile::SetValue_int(const std::string& strKey, const int nValue)
{
	char Buff[50] = {0};
	sprintf(Buff, "%d", nValue);
	m_mapKey2Value[strKey] = std::string(Buff);
	//
	m_bDirty = true;
}
//------------------------------------------------------------
void SoConfigFile::SetValue_float(const std::string& strKey, const float fValue)
{
	char Buff[50] = {0};
	sprintf(Buff, "%f", fValue);
	m_mapKey2Value[strKey] = std::string(Buff);
	//
	m_bDirty = true;
}
//------------------------------------------------------------
void SoConfigFile::SetValue_string(const std::string& strKey, const std::string& strValue)
{
	m_mapKey2Value[strKey] = strValue;
	//
	m_bDirty = true;
}
//------------------------------------------------------------
void SoConfigFile::ClearConfig()
{
	m_strFileName.clear();
	m_mapKey2Value.clear();
	m_pFileLineBuff = 0;
	m_nValidLineSize = 0;
	m_nKeyStartIndex = 0;
	m_nValueStartIndex = 0;
	m_bDirty = false;
}
//------------------------------------------------------------
void SoConfigFile::ParseFileChunk(const char* pBuff, const int nValidSize, const bool bEndOfFile)
{
	for (int i = 0; i < nValidSize; ++i)
	{
		const char theChar = pBuff[i];
		if (theChar == '\n')
		{
			m_pFileLineBuff[m_nValidLineSize] = 0;
			ParseFileLine();
		}
		else
		{
			m_pFileLineBuff[m_nValidLineSize] = theChar;
			++m_nValidLineSize;
		}
	}
	//
	if (bEndOfFile)
	{
		m_pFileLineBuff[m_nValidLineSize] = 0;
		ParseFileLine();
	}
}
//------------------------------------------------------------
void SoConfigFile::ParseFileLine()
{
	bool bFind = false;
	int nIndex = 0;
	//
	for (; nIndex < m_nValidLineSize; ++nIndex)
	{
		if (m_pFileLineBuff[nIndex] == '=')
		{
			m_pFileLineBuff[nIndex] = 0;
			bFind = true;
			break;
		}
	}
	//
	if (bFind)
	{
		m_nKeyStartIndex = 0;
		m_nValueStartIndex = nIndex + 1;
		const int nKeyLen = nIndex;
		const int nValueLen = m_nValidLineSize - nIndex - 1;
		//
		SlimString(m_pFileLineBuff+m_nKeyStartIndex, nKeyLen, m_nKeyStartIndex);
		SlimString(m_pFileLineBuff+m_nValueStartIndex, nValueLen, m_nValueStartIndex);
		m_mapKey2Value.insert(make_pair(std::string(m_pFileLineBuff+m_nKeyStartIndex), std::string(m_pFileLineBuff+m_nValueStartIndex)));
	}
	m_nValidLineSize = 0;
	m_nKeyStartIndex = 0;
	m_nValueStartIndex = 0;
}
//------------------------------------------------------------
void SoConfigFile::SlimString(char* pBuff, int nValidSize, int& nAdjustStartIndex)
{
	for (int i = 0; i < nValidSize; ++i)
	{
		if (pBuff[i] == ' ')
		{
			++nAdjustStartIndex;
		}
		else if (pBuff[i] == '\t')
		{
			++nAdjustStartIndex;
		}
		else
		{
			break;
		}
	}
	for (int i = nValidSize-1; i >= 0; --i)
	{
		if (pBuff[i] == ' ')
		{
			pBuff[i] = 0;
		}
		else if (pBuff[i] == '\t')
		{
			pBuff[i] = 0;
		}
		else
		{
			break;
		}
	}
}
//------------------------------------------------------------
