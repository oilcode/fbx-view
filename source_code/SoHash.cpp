//-----------------------------------------------------------------------------
// SoHash
// (C) oil
// 2013-10-05
//-----------------------------------------------------------------------------
#include "SoHash.h"
//-----------------------------------------------------------------------------
souint32 SoHash_PHP(const char *pszString)
{
	souint32 h = 0;
	souint32 g = 0;
	while (*pszString)
	{
		h = (h << 4) + *pszString;
		++pszString;
		if ((g = (h & 0xF0000000)))
		{
			h = h ^ (g >> 24);
			h = h ^ g;
		}
	}
	return h;
}
//-----------------------------------------------------------------------------
souint32 SoHash_BKDR(const char* pszString)
{
	souint32 seed = 131; // 31 131 1313 13131 131313 etc..
	souint32 hash = 0;
	while (*pszString)
	{
		hash = hash * seed + (*pszString++);
	}
	return (hash & 0x7FFFFFFF);
}
//-----------------------------------------------------------------------------
souint32 SoHash_Index(const char* pszString)
{
	souint32 nHash = 0;
	while (*pszString)
	{
		nHash = (nHash<<5) + nHash + *pszString;
		++pszString;
	}
	return nHash;
}
//-----------------------------------------------------------------------------
