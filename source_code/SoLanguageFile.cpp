//------------------------------------------------------------
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif //_CRT_SECURE_NO_WARNINGS
//------------------------------------------------------------
#include "SoLanguageFile.h"
#include "SoHash.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
//------------------------------------------------------------
SoLanguageFile::SoLanguageFile()
:m_pFileBuff(0)
,m_nFileBuffSize(0)
,m_pItemList(0)
,m_nItemCount(0)
,m_nReadPos(0)
{

}
//------------------------------------------------------------
SoLanguageFile::~SoLanguageFile()
{
	ClearAll();
}
//------------------------------------------------------------
bool SoLanguageFile::InitLanguageFile(const char* pszFileName)
{
	if (pszFileName == 0)
	{
		return false;
	}
	if (LoadFile(pszFileName) == false)
	{
		ClearAll();
		return false;
	}
	//
	m_nItemCount = GetLineCount();
	if (m_nItemCount <= 0)
	{
		ClearAll();
		return false;
	}
	m_pItemList = (stLanguageItem*)malloc(sizeof(stLanguageItem) * m_nItemCount);
	memset(m_pItemList, 0, sizeof(stLanguageItem) * m_nItemCount);
	//
	if (ParseFile() == false)
	{
		ClearAll();
		return false;
	}
	//
	return true;
}
//------------------------------------------------------------
const char* SoLanguageFile::GetValue(const char* pszKey)
{
	const char* pValue = "";
	if (pszKey == 0)
	{
		return pValue;
	}

	const unsigned int myHashKeyA = SoHash_Index(pszKey);
	int nIndex = (int)(myHashKeyA % (unsigned int)m_nItemCount);
	for (int i = 0; i < m_nItemCount; ++i)
	{
		if (m_pItemList[nIndex].uiHashKeyA == myHashKeyA)
		{
			if (m_pItemList[nIndex].uiHashKeyB == SoHash_PHP(pszKey))
			{
				pValue = m_pItemList[nIndex].pszValue;
				break;
			}
		}
		else
		{
			++nIndex;
			if (nIndex >= m_nItemCount)
			{
				nIndex = 0;
			}
		}
	}

	return pValue;
}
//------------------------------------------------------------
void SoLanguageFile::ClearAll()
{
	if (m_pFileBuff)
	{
		free(m_pFileBuff);
		m_pFileBuff = 0;
	}
	m_nFileBuffSize = 0;
	if (m_pItemList)
	{
		free(m_pItemList);
		m_pItemList = 0;
	}
	m_nItemCount = 0;
	m_nReadPos = 0;
}
//------------------------------------------------------------
bool SoLanguageFile::LoadFile(const char* pszFileName)
{
	if (pszFileName == 0)
	{
		return false;
	}

	FILE* pFile = fopen(pszFileName, "r"); //只读文本方式
	if (pFile == NULL)
	{
		return false;
	}

	//获取文件字节个数
	fseek(pFile, 0, SEEK_END);
	//多计算一个字节，目的是申请内存时多申请一个字节，作为结束符
	const int nFileLen = (int)ftell(pFile) + 1;
	if (nFileLen <= 1)
	{
		//文件大小异常
		fclose(pFile);
		return false;
	}

	m_pFileBuff = (char*)malloc(nFileLen);
	if (m_pFileBuff == 0)
	{
		//申请内存失败
		fclose(pFile);
		return false;
	}

	m_nFileBuffSize = nFileLen;
	memset(m_pFileBuff, 0, nFileLen);
	//读取文件内容
	fseek(pFile, 0, SEEK_SET);
	fread(m_pFileBuff, 1, nFileLen, pFile);
	fclose(pFile);
	return true;
}
//------------------------------------------------------------
int SoLanguageFile::GetLineCount()
{
	int nLineCount = 0;
	for (int i = 0; i < m_nFileBuffSize; ++i)
	{
		if (m_pFileBuff[i] == 0)
		{
			//到达文件末尾，也算作一行
			++nLineCount;
			break;
		}
		else if (m_pFileBuff[i] == '\n')
		{
			++nLineCount;
		}
	}
	return nLineCount;
}
//------------------------------------------------------------
bool SoLanguageFile::ParseFile()
{
	if (m_pFileBuff == 0 || m_nFileBuffSize == 0)
	{
		return false;
	}

	int m_nReadPos = 0;
	while (1)
	{
		char* pLine = GetNextLine();
		if (pLine == 0)
		{
			//文件内容解析完毕
			break;
		}
		else
		{
			//得到了文件中的一行
			ParseFileLine(pLine);
		}
	}

	return true;
}
//------------------------------------------------------------
char* SoLanguageFile::GetNextLine()
{
	if (m_nReadPos >= m_nFileBuffSize)
	{
		//Arrive file end
		return 0;
	}
	//
	const int nStartPos = m_nReadPos;
	bool bArriveFileEnd = false;
	bool bArriveLineEnd = false;
	while (1)
	{
		const char Character = m_pFileBuff[m_nReadPos];
		if (Character == 0)
		{
			bArriveFileEnd = true;
			break;
		}
		if (Character == '\n')
		{
			bArriveLineEnd = true;
			break;
		}
		//
		++m_nReadPos;
	}
	//
	if (bArriveLineEnd)
	{
		//把'\n'字符修改为字符串结束符0
		m_pFileBuff[m_nReadPos] = 0;
		//jump '\n' character
		++m_nReadPos;
	}
	//
	if (bArriveFileEnd && nStartPos == m_nReadPos)
	{
		//到达文件末尾，并且此次解析过程中没有遇到有效字符
		return 0;
	}
	else
	{
		return m_pFileBuff + nStartPos;
	}
}
//------------------------------------------------------------
bool SoLanguageFile::ParseFileLine(char* pFileLine)
{
	//遍历字符串，记录'='字符所在的下标位置
	int nIndex = 0;

	int nLineLen = 0;
	while (1)
	{
		if (pFileLine[nLineLen] == 0)
		{
			//字符串结束
			break;
		}
		else if (pFileLine[nLineLen] == '=')
		{
			nIndex = nLineLen;
			//这里不要break，因为要计算nLineLen的值。
		}
		//
		++nLineLen;
	}

	if (nIndex == 0)
	{
		//没有找到'='字符，或者第一个字符就是'='字符
		return false;
	}

	pFileLine[nIndex] = 0;
	//Key字符串
	char* pszKey = pFileLine;
	int nKeyLen = nIndex;
	//Value字符串
	char* pszValue = pFileLine + nIndex + 1;
	int nValueLen = nLineLen - nIndex - 1;
	//
	int nKeyStartIndex = 0;
	int nValueStartIndex = 0;
	SlimString(pszKey, nKeyLen, nKeyStartIndex);
	SlimString(pszValue, nValueLen, nValueStartIndex);
	pszKey = pszKey + nKeyStartIndex;
	pszValue = pszValue + nValueStartIndex;
	//
	return MakeLanguageItem(pszKey, pszValue);
}
//------------------------------------------------------------
void SoLanguageFile::SlimString(char* pBuff, int nValidSize, int& nAdjustStartIndex)
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
bool SoLanguageFile::MakeLanguageItem(char* pszKey, char* pszValue)
{
	//记录是否找到了一个空的元素
	bool bFind = false;
	unsigned int uiHashKeyA = SoHash_Index(pszKey);
	int nIndex = (int)(uiHashKeyA % (unsigned int)m_nItemCount);
	for (int i = 0; i < m_nItemCount; ++i)
	{
		if (m_pItemList[nIndex].uiHashKeyA == 0)
		{
			bFind = true;
			break;
		}
		else
		{
			++nIndex;
			if (nIndex >= m_nItemCount)
			{
				nIndex = 0;
			}
		}
	}
	if (bFind == false)
	{
		return false;
	}

	stLanguageItem* pItem = m_pItemList + nIndex;
	pItem->uiHashKeyA = uiHashKeyA;
	pItem->uiHashKeyB = SoHash_PHP(pszKey);
	pItem->pszKey = pszKey;
	pItem->pszValue = pszValue;
	return true;
}
//------------------------------------------------------------
