//------------------------------------------------------------
#include <Windows.h>
#include "SoLanguageRule.h"
//------------------------------------------------------------
char g_szBuff[SoLanguageRule_MaxSize_FileLine] = {0};
//------------------------------------------------------------
bool SoLanguageRule::Parse(stItemKeyValue* pItem, const char* pString, const int nStrLen)
{
	if (pItem == 0)
	{
		return false;
	}
	if (pString == 0)
	{
		return false;
	}
	if (nStrLen >= SoLanguageRule_MaxSize_FileLine)
	{
		return false;
	}
	if (nStrLen < 3)
	{
		return false;
	}
	//
	strcpy_s(g_szBuff, SoLanguageRule_MaxSize_FileLine, pString);
	bool bFind = false;
	int nIndex = 0;
	for (; nIndex < nStrLen; ++nIndex)
	{
		if (g_szBuff[nIndex] == '=')
		{
			g_szBuff[nIndex] = 0;
			bFind = true;
			break;
		}
	}
	//
	if (bFind)
	{
		pItem->pKey = g_szBuff;
		pItem->pValue = g_szBuff + nIndex + 1;
		pItem->nKeyLen = nIndex;
		pItem->nValueLen = nStrLen - nIndex - 1;
	}
	return bFind;
}
//------------------------------------------------------------
