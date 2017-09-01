//--------------------------------------------------------------------
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif //_CRT_SECURE_NO_WARNINGS
//----------------------------------------------------------------
#include "SoSimpleLoadFile.h"
#include <string>
#include <cstdio>
//----------------------------------------------------------------
SoSimpleLoadFile::SoSimpleLoadFile()
:m_pData(0)
,m_nSize(-1)
{

}
//----------------------------------------------------------------
SoSimpleLoadFile::~SoSimpleLoadFile()
{
	Clear();
}
//----------------------------------------------------------------
bool SoSimpleLoadFile::LoadFile(const char* szFileName, bool bTextMode)
{
	Clear();

	bool br = false;
	FILE* pf = 0;

	do 
	{
		if (szFileName == 0 || szFileName[0] == 0)
		{
			break;
		}

		pf = fopen(szFileName, (bTextMode ? "rt" : "rb"));
		if (pf == 0)
		{
			break;
		}

		fseek(pf, 0, SEEK_END);
		m_nSize = ftell(pf);
		fseek(pf, 0, SEEK_SET);
		if (m_nSize < 0)
		{
			break;
		}
		else if (m_nSize == 0)
		{
			br = true;
			break;
		}

		m_pData = (char*)calloc(1, m_nSize);
		if (m_pData == 0)
		{
			break;
		}

		fread(m_pData, 1, m_nSize, pf);
		br = true;

	} while (0);

	if (pf)
	{
		fclose(pf);
		pf = 0;
	}
	if (br == false)
	{
		Clear();
	}
	return br;
}
//----------------------------------------------------------------
void SoSimpleLoadFile::Clear()
{
	if (m_pData)
	{
		free(m_pData);
		m_pData = 0;
	}
	m_nSize = -1;
}
//----------------------------------------------------------------
