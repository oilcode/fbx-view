//--------------------------------------------------------------------
//1，致力于跨平台，跨编译器。
//2，非线程安全，使用了全局的字符串缓存。
//   SoxxxSelf 系列的函数是线程安全函数，它要求用户提供字符串缓存。
//--------------------------------------------------------------------
//去除编译warning
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif //_CRT_SECURE_NO_WARNINGS
//--------------------------------------------------------------------
#include "SoStringHelp.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
//--------------------------------------------------------------------
#define SoStringHelp_MaxStringSize 0x3FFFFFFF
//本模块使用的缓存
char g_StrHelpBuff[SoStringHelp_GlobalBuffSize] = {0};
wchar_t* g_WStrHelpBuff = (wchar_t*)g_StrHelpBuff;
//--------------------------------------------------------------------
int SoStrLen(const char* szString)
{
	int nLen = 0;
	for (nLen = 0; nLen < SoStringHelp_MaxStringSize; ++nLen)
	{
		if (*szString == 0)
		{
			break;
		}
		else
		{
			++szString;
		}
	}
	return nLen;
}
//--------------------------------------------------------------------
int SoWStrLen(const wchar_t* szString)
{
	int nLen = 0;
	for (nLen = 0; nLen < SoStringHelp_MaxStringSize; ++nLen)
	{
		if (*szString == 0)
		{
			break;
		}
		else
		{
			++szString;
		}
	}
	return nLen;
}
//--------------------------------------------------------------------
int SoStrCpy(char* DestBuff, const int nDestBuffSize, const char* szSourceString)
{
	int nLen = 0;
	const int nMaxLen = nDestBuffSize - 1;
	for (nLen = 0; nLen < nMaxLen; ++nLen)
	{
		if (*szSourceString == 0)
		{
			break;
		}
		else
		{
			*DestBuff = *szSourceString;
			++DestBuff;
			++szSourceString;
		}
	}
	*DestBuff = 0;
	return nLen;
}
//--------------------------------------------------------------------
int SoWStrCpy(wchar_t* DestBuff, const int nDestBuffSize, const wchar_t* szSourceString)
{
	int nLen = 0;
	const int nMaxLen = nDestBuffSize - 1;
	for (nLen = 0; nLen < nMaxLen; ++nLen)
	{
		if (*szSourceString == 0)
		{
			break;
		}
		else
		{
			*DestBuff = *szSourceString;
			++DestBuff;
			++szSourceString;
		}
	}
	*DestBuff = 0;
	return nLen;
}
//--------------------------------------------------------------------
char* SoStrFmt(const char* szFormat, ...)
{
	va_list klist;
	va_start(klist, szFormat);
	vsnprintf(g_StrHelpBuff, SoStringHelp_GlobalBuffSize, szFormat, klist);
	va_end(klist);
	return g_StrHelpBuff;
}
//--------------------------------------------------------------------
void SoStrFmtSelf(char* Buff, const int nBuffSize, const char* szFormat, ...)
{
	va_list klist;
	va_start(klist, szFormat);
	vsnprintf(Buff, nBuffSize, szFormat, klist);
	va_end(klist);
}
//--------------------------------------------------------------------
char* SoStrFmtByVaList(const char* szFormat, const void* pValist)
{
	va_list* pList = (va_list*)pValist;
	vsnprintf(g_StrHelpBuff, SoStringHelp_GlobalBuffSize, szFormat, *pList);
	return g_StrHelpBuff;
}
//--------------------------------------------------------------------
void SoStrFmtSelfByVaList(char* Buff, const int nBuffSize, const char* szFormat, const void* pValist)
{
	va_list* pList = (va_list*)pValist;
	vsnprintf(Buff, nBuffSize, szFormat, *pList);
}
//--------------------------------------------------------------------
int SoStrCmp(const char* s1, const char* s2)
{
	const unsigned char* us1 = (const unsigned char*)s1;
	const unsigned char* us2 = (const unsigned char*)s2;

	while (*us1 == *us2)
	{
		if (*us1 == 0)
		{
			return 0;
		}
		else
		{
			++us1;
			++us2;
		}
	}
	return (*us1 - *us2);
}
//--------------------------------------------------------------------
int SoWStrCmp(const wchar_t* s1, const wchar_t* s2)
{
	const wchar_t* us1 = (const wchar_t*)s1;
	const wchar_t* us2 = (const wchar_t*)s2;

	while (*us1 == *us2)
	{
		if (*us1 == 0)
		{
			return 0;
		}
		else
		{
			++us1;
			++us2;
		}
	}
	return (*us1 - *us2);
}
//--------------------------------------------------------------------
int SoStrCmpNoCase(const char* s1, const char* s2)
{
	const unsigned char* us1 = (const unsigned char*)s1;
	const unsigned char* us2 = (const unsigned char*)s2;
	unsigned char tolower1 = 1;
	unsigned char tolower2 = 1;

	while (tolower1 == tolower2)
	{
		if (tolower1 == 0)
		{
			return 0;
		}
		else
		{
			tolower1 = *us1++;
			tolower2 = *us2++;
			if (tolower1 >= 65 && tolower1 <= 90)
			{
				tolower1 += 32;
			}
			if (tolower2 >= 65 && tolower2 <= 90)
			{
				tolower2 += 32;
			}
		}
	}
	return (tolower1 - tolower2);
}
//--------------------------------------------------------------------
int SoWStrCmpNoCase(const wchar_t* s1, const wchar_t* s2)
{
	const wchar_t* us1 = (const wchar_t*)s1;
	const wchar_t* us2 = (const wchar_t*)s2;
	wchar_t tolower1 = 1;
	wchar_t tolower2 = 1;

	while (tolower1 == tolower2)
	{
		if (tolower1 == 0)
		{
			return 0;
		}
		else
		{
			tolower1 = *us1++;
			tolower2 = *us2++;
			if (tolower1 >= 65 && tolower1 <= 90)
			{
				tolower1 += 32;
			}
			if (tolower2 >= 65 && tolower2 <= 90)
			{
				tolower2 += 32;
			}
		}
	}
	return (tolower1 - tolower2);
}
//--------------------------------------------------------------------
int SoStrChr(const char* str, const char c, int nStrLen)
{
	int nPos = -1;
	if (nStrLen == -1)
	{
		nStrLen = SoStrLen(str);
	}
	//
	for (int i = 0; i < nStrLen; ++i)
	{
		if (*str == c)
		{
			nPos = i;
			break;
		}
		else
		{
			++str;
		}
	}
	return nPos;
}
//--------------------------------------------------------------------
int SoWStrChr(const wchar_t* str, const wchar_t c, int nStrLen)
{
	int nPos = -1;
	if (nStrLen == -1)
	{
		nStrLen = SoWStrLen(str);
	}
	//
	for (int i = 0; i < nStrLen; ++i)
	{
		if (*str == c)
		{
			nPos = i;
			break;
		}
		else
		{
			++str;
		}
	}
	return nPos;
}
//--------------------------------------------------------------------
int SoStrRChr(const char* str, const char c, int nStrLen)
{
	int nPos = -1;
	if (nStrLen == -1)
	{
		nStrLen = SoStrLen(str);
	}
	//
	const char* r_str = str + nStrLen - 1;
	for (int i = nStrLen - 1; i >= 0; --i)
	{
		if (*r_str == c)
		{
			nPos = i;
			break;
		}
		else
		{
			--r_str;
		}
	}
	return nPos;
}
//--------------------------------------------------------------------
int SoWStrRChr(const wchar_t* str, const wchar_t c, int nStrLen)
{
	int nPos = -1;
	if (nStrLen == -1)
	{
		nStrLen = SoWStrLen(str);
	}
	//
	const wchar_t* r_str = str + nStrLen - 1;
	for (int i = nStrLen - 1; i >= 0; --i)
	{
		if (*r_str == c)
		{
			nPos = i;
			break;
		}
		else
		{
			--r_str;
		}
	}
	return nPos;
}
//--------------------------------------------------------------------
char* SoSubStr(const char* str, int nStartIndex, int nEndIndex)
{
	int nSubStrLen = 0;
	for (int i = 0; i < SoStringHelp_MaxStringSize; ++i)
	{
		if (str[i] == 0)
		{
			break;
		}
		if (i < nStartIndex)
		{
			continue;
		}
		else if (i >= nStartIndex && i <= nEndIndex)
		{
			g_StrHelpBuff[nSubStrLen] = str[i];
			++nSubStrLen;
			if (nSubStrLen >= SoStringHelp_GlobalBuffSize - 1)
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	//
	g_StrHelpBuff[nSubStrLen] = 0;
	return g_StrHelpBuff;
}
//--------------------------------------------------------------------
char* SoStrSlim(const char* str, int nStrLen)
{
	g_StrHelpBuff[0] = 0;
	if (nStrLen == -1)
	{
		nStrLen = SoStrLen(str);
	}
	//
	int nStartIndex = -1;
	for (int i = 0; i < nStrLen; ++i)
	{
		if (str[i] != ' ' && str[i] != '\t')
		{
			nStartIndex = i;
			break;
		}
	}
	if (nStartIndex == -1)
	{
		return g_StrHelpBuff;
	}
	//
	int nEndIndex = -1;
	for (int i = nStrLen-1; i >= 0; --i)
	{
		if (str[i] != ' ' && str[i] != '\t')
		{
			nEndIndex = i;
			break;
		}
	}
	if (nEndIndex == -1)
	{
		return g_StrHelpBuff;
	}
	//
	if (nStartIndex > nEndIndex)
	{
		return g_StrHelpBuff;
	}
	int nValidCount = nEndIndex - nStartIndex + 1;
	if (nValidCount > SoStringHelp_GlobalBuffSize - 1)
	{
		nValidCount = SoStringHelp_GlobalBuffSize - 1;
	}
	for (int i = 0; i < nValidCount; ++i)
	{
		g_StrHelpBuff[i] = str[i + nStartIndex];
	}
	g_StrHelpBuff[nValidCount] = 0;
	return g_StrHelpBuff;
}
//--------------------------------------------------------------------
char* SoStrToUpper(const char* szString)
{
	int nCount = 0;
	while (szString[nCount] != 0)
	{
		char theC = szString[nCount];
		if (theC >= 'a' && theC <= 'z')
		{
			theC -= 32;
		}
		g_StrHelpBuff[nCount] = theC;
		//
		++nCount;
		if (nCount >= SoStringHelp_GlobalBuffSize)
		{
			g_StrHelpBuff[SoStringHelp_GlobalBuffSize-1] = 0;
			break;
		}
	}
	if (nCount < SoStringHelp_GlobalBuffSize)
	{
		g_StrHelpBuff[nCount] = 0;
	}
	return g_StrHelpBuff;
}
//--------------------------------------------------------------------
void SoStrToUpperSelf(char* szString)
{
	int nCount = 0;
	while (szString[nCount] != 0)
	{
		char& theC = szString[nCount];
		if (theC >= 'a' && theC <= 'z')
		{
			theC -= 32;
		}
		//
		++nCount;
	}
}
//--------------------------------------------------------------------
char* SoStrToLower(const char* szString)
{
	int nCount = 0;
	while (szString[nCount] != 0)
	{
		char theC = szString[nCount];
		if (theC >= 'A' && theC <= 'Z')
		{
			theC += 32;
		}
		g_StrHelpBuff[nCount] = theC;
		//
		++nCount;
		if (nCount >= SoStringHelp_GlobalBuffSize)
		{
			g_StrHelpBuff[SoStringHelp_GlobalBuffSize-1] = 0;
			break;
		}
	}
	if (nCount < SoStringHelp_GlobalBuffSize)
	{
		g_StrHelpBuff[nCount] = 0;
	}
	return g_StrHelpBuff;
}
//--------------------------------------------------------------------
void SoStrToLowerSelf(char* szString)
{
	int nCount = 0;
	while (szString[nCount] != 0)
	{
		char& theC = szString[nCount];
		if (theC >= 'A' && theC <= 'Z')
		{
			theC += 32;
		}
		//
		++nCount;
	}
}
//--------------------------------------------------------------------
char* SoPathNormalize(const char* szPath, bool bXieGang)
{
	g_StrHelpBuff[0] = 0;

	if (szPath == 0 || szPath[0] == 0)
	{
		return g_StrHelpBuff;
	}

	//去除头部的某些字符。
	while (*szPath != 0)
	{
		if (*szPath == '.' || *szPath == '/' || *szPath == '\\' || *szPath == ' ' || *szPath == '\t')
		{
			++szPath;
		}
		else
		{
			break;
		}
	}
	if (szPath[0] == 0)
	{
		return g_StrHelpBuff;
	}

	//去除尾部的某些字符
	int nLength = SoStrLen(szPath);
	while (nLength > 0)
	{
		if (szPath[nLength-1] == '.' || szPath[nLength-1] == ' ' || szPath[nLength-1] == '\t')
		{
			--nLength;
		}
		else
		{
			break;
		}
	}
	if (nLength == 0)
	{
		return g_StrHelpBuff;
	}

	//遍历每个字符
	int nCount = 0;
	for (int i = 0; i < nLength; ++i)
	{
		char theC = szPath[nCount];
		if (theC >= 'A' && theC <= 'Z')
		{
			//大写字母转换为小写字母
			theC += 32;
		}
		else if (theC == '\\')
		{
			theC = '/';
		}
		g_StrHelpBuff[nCount] = theC;
		//
		++nCount;
		if (nCount >= SoStringHelp_GlobalBuffSize)
		{
			g_StrHelpBuff[SoStringHelp_GlobalBuffSize-1] = 0;
			break;
		}
	}

	if (nCount < SoStringHelp_GlobalBuffSize)
	{
		g_StrHelpBuff[nCount] = 0;
		//
		if (bXieGang)
		{
			//路径末尾必须是'/'
			if (g_StrHelpBuff[nCount-1] != '/')
			{
				if (nCount+1 < SoStringHelp_GlobalBuffSize)
				{
					g_StrHelpBuff[nCount] = '/';
					g_StrHelpBuff[nCount+1] = 0;
				}
			}
		}
	}
	return g_StrHelpBuff;
}
//--------------------------------------------------------------------
char* SoGetOneCharacterFromUtf8String(const char* pszUtf8Str, const int nUtf8StrLen, const int nStartIndex, int* pByteCountOfTheCharacter)
{
	g_StrHelpBuff[0] = 0;
	int nByteCountOfTheCharacter = 0;
	if (pByteCountOfTheCharacter)
	{
		*pByteCountOfTheCharacter = 0;
	}
	//
	if (nStartIndex >= nUtf8StrLen)
	{
		return g_StrHelpBuff;
	}
	//
	int nCurrentIndex = nStartIndex;
	g_StrHelpBuff[nByteCountOfTheCharacter] = pszUtf8Str[nCurrentIndex];
	++nByteCountOfTheCharacter;
	++nCurrentIndex;
	//
	for (; nCurrentIndex<nUtf8StrLen; ++nCurrentIndex)
	{
		unsigned char theChar = pszUtf8Str[nCurrentIndex];
		if (!(theChar&0x80) || (theChar&0xC0) == 0xC0)
		{
			break;
		}
		else
		{
			g_StrHelpBuff[nByteCountOfTheCharacter] = pszUtf8Str[nCurrentIndex];
			++nByteCountOfTheCharacter;
		}
	}
	//
	g_StrHelpBuff[nByteCountOfTheCharacter] = 0;
	if (pByteCountOfTheCharacter)
	{
		*pByteCountOfTheCharacter = nByteCountOfTheCharacter;
	}
	return g_StrHelpBuff;
}
//--------------------------------------------------------------------
bool SoCheckUtf8WellFormed(const unsigned char *str, int len)
{
    const unsigned char* pos = str;
    while (*pos != '\0' && pos - str < len)
    {
		unsigned char c = *pos;
        int charlen = (c <= 0x7f ? 1 : (c & 0xe0) == 0xc0 ? 2: (c & 0xf0) == 0xe0 ? 3: (c & 0xf8) == 0xf0 ? 4: 0);
        if (pos + charlen > str + len)
        {
        	return false;
        }

        switch (charlen)
        {
        case 0:
        	return false;
        case 1:
        	//always ok
        	break;
        case 2:
        	/*
        	 * 2字节合法取值范围：
        	 * [C2-DF][80-BF]
        	 */
        	if (!(pos[0] >= 0xC2 && pos[0] <= 0xDF && pos[1] >= 0x80 && pos[1] <= 0xBF))
        	{
        		return false;
        	}
        	break;
        case 3:
        	/*
        	 * 3字节合法取值范围
        	 * [E0-E0][A0-BF][80-BF]
        	 * [E1-EC][80-BF][80-BF]
        	 * [ED-ED][80-9F][80-BF]
        	 * [EE-EF][80-BF][80-BF]
        	 */
        	if (pos[2] < 0x80 || pos[2] > 0xBF)
        	{
        		return false;
        	}
        	if (!(pos[0] == 0xE0 && pos[1] >= 0xA0 && pos[1] <= 0xBF ||
        		pos[0] >= 0xE1 && pos[0] <= 0xEC && pos[1] >= 0x80 && pos[1] <= 0xBF ||
        		pos[0] == 0xED && pos[1] >= 0x80 && pos[1] <= 0x9F ||
        		pos[0] >= 0xEE && pos[0] <= 0xEF && pos[1] >= 0x80 && pos[1] <= 0xBF ))
        	{
        		return false;
        	}
        	break;
        case 4:
        	/*
        	 * 4字节合法取值范围
        	 * [F0-F0][90-BF][80-BF][80-BF]
        	 * [F1-F3][80-BF][80-BF][80-BF]
        	 * [F4-F4][80-8F][80-BF][80-BF]
        	 */
        	if (pos[2] < 0x80 || pos[2] > 0xBF || pos[3] < 0x80 || pos[3] > 0xBF)
        	{
        		return false;
        	}
        	if (!(pos[0] == 0xF0 && pos[1] >= 0x90 && pos[1] <= 0xBF ||
        		pos[0] >= 0xF1 && pos[0] <= 0xF3 && pos[1] >= 0x80 && pos[1] <= 0xBF ||
        		pos[0] == 0xF4 && pos[1] >= 0x80 && pos[1] <= 0x8F ))
        	{
        		return false;
        	}
        	break;
        }

        pos += charlen;
    }
    return true;
}
//--------------------------------------------------------------------
double Soatof(const char* szString)
{
	if (szString == 0)
	{
		return 0.0;
	}

	const int nStrLen = SoStrCpy(g_StrHelpBuff, SoStringHelp_GlobalBuffSize, szString);
	// strip string, only remain 7 numbers after '.'
	int nPos = SoStrChr(g_StrHelpBuff, '.', nStrLen);
	if (nPos != -1 && nPos + 8 < SoStringHelp_GlobalBuffSize)
	{
		g_StrHelpBuff[nPos + 8] = 0;
	}

	return atof(g_StrHelpBuff);
}
//--------------------------------------------------------------------
double SoWatof(const wchar_t* szString)
{
	if (szString == 0)
	{
		return 0.0;
	}

	const int nStrLen = SoWStrCpy(g_WStrHelpBuff, SoStringHelp_GlobalBuffSize, szString);
	// strip string, only remain 7 numbers after '.'
	int nPos = SoWStrChr(g_WStrHelpBuff, '.', nStrLen);
	if (nPos != -1 && nPos + 8 < SoStringHelp_GlobalBuffSize)
	{
		g_WStrHelpBuff[nPos + 8] = 0;
	}

	return _wtof(g_WStrHelpBuff);
}
//--------------------------------------------------------------------
