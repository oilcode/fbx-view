//--------------------------------------------------------------------
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif //_CRT_SECURE_NO_WARNINGS
//------------------------------------------------------------
#include "SoReadFile.h"
#include <stdio.h>
#include <Windows.h>
//------------------------------------------------------------
#define MaxSize_FileName 300
#define MaxSize_FileBuff 5000000
#define MaxSize_FileLine 4096
//------------------------------------------------------------
SoReadFile::SoReadFile()
{
	m_pFileName = new char[MaxSize_FileName];
	m_pFileBuff = new char[MaxSize_FileBuff];
	m_pLine = new char[MaxSize_FileLine];
	ClearFile();
}
//------------------------------------------------------------
SoReadFile::~SoReadFile()
{
	if (m_pFileName != 0)
	{
		delete [] m_pFileName;
		m_pFileName = 0;
	}
	if (m_pFileBuff != 0)
	{
		delete [] m_pFileBuff;
		m_pFileBuff = 0;
	}
	if (m_pLine != 0)
	{
		delete [] m_pLine;
		m_pLine = 0;
	}
}
//------------------------------------------------------------
bool SoReadFile::LoadFile(const char* pFileName)
{
	if (pFileName == 0 || pFileName[0] == 0)
	{
		return false;
	}
	//
	FILE* pFile = fopen(pFileName, "r"); //must is "r", not "rb"
	if (pFile == 0)
	{
		//Error happen
		return false;
	}
	//
	fseek(pFile, 0, SEEK_END);
	int nFileLen = ftell(pFile);
	if (nFileLen >= MaxSize_FileBuff) //must is ">="
	{
		//Too large
		return false;
	}
	//
	ClearFile();
	strcpy_s(m_pFileName, MaxSize_FileName, pFileName);
	//
	fseek(pFile, 0, SEEK_SET);
	int nReadLen = (int)fread_s(m_pFileBuff, MaxSize_FileBuff, 1, nFileLen, pFile);
	fclose(pFile);
	m_nFileLen = nReadLen;
	m_nReadPos = 0;
	//
	return true;
}
//------------------------------------------------------------
void SoReadFile::ClearFile()
{
	m_pFileName[0] = 0;
	m_nFileLen = 0;
	m_nLineLen = 0;
	m_nReadPos = 0;
}
//------------------------------------------------------------
const char* SoReadFile::GetFirstLine()
{
	m_nReadPos = 0;
	return GetNextLine();
}
//------------------------------------------------------------
const char* SoReadFile::GetNextLine()
{
	if (CopyNextLine() == true)
	{
		return m_pLine;
	}
	else
	{
		return NULL;
	}
}
//------------------------------------------------------------
bool SoReadFile::CopyNextLine()
{
	memset(m_pLine, 0, MaxSize_FileLine);
	m_nLineLen = 0;
	//
	if (m_nReadPos >= m_nFileLen)
	{
		//Arrive file end
		return false;
	}
	//
	bool bLineBuffFull = false;
	bool bArriveFileEnd = false;
	bool bArriveLineEnd = false;
	while (1)
	{
		if (m_nLineLen >= MaxSize_FileLine)
		{
			bLineBuffFull = true;
			break;
		}
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
		m_pLine[m_nLineLen] = Character;
		++m_nLineLen;
		++m_nReadPos;
	}
	//
	if (bLineBuffFull)
	{
		m_pLine[MaxSize_FileLine-1] = 0;
		--m_nLineLen;
		--m_nReadPos;
	}
	if (bArriveLineEnd)
	{
		//jump '\n' character
		++m_nReadPos;
	}
	//
	if (bArriveFileEnd && m_nLineLen == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}
//------------------------------------------------------------
int SoReadFile::GetLineLen() const
{
	return m_nLineLen;
}
//------------------------------------------------------------
