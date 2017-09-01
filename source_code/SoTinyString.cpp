//-----------------------------------------------------------------------------
#include "SoTinyString.h"
//-----------------------------------------------------------------------------
void SoTinyString::SetValue(const char* pszString)
{
	unsigned char i = 0;
	if (pszString)
	{
		for (; i<MaxCharCount_TinyString; ++i)
		{
			if (pszString[i] == 0)
			{
				break;
			}
			else
			{
				m_szBuffer[i] = pszString[i];
			}
		}
	}
	m_szBuffer[i] = 0;
	m_nLength = i;
}
//-----------------------------------------------------------------------------
bool operator < (const SoTinyString& left, const SoTinyString& right)
{
	for (int i=0; i<MaxCharCount_TinyString; ++i)
	{
		if (left.m_szBuffer[i] && right.m_szBuffer[i])
		{
			if (left.m_szBuffer[i] == right.m_szBuffer[i])
			{
				//继续比较；什么也不做。
			}
			else
			{
				return (left.m_szBuffer[i] < right.m_szBuffer[i]);
			}
		}
		else
		{
			//要么left.m_szBuffer[i]为0，要么right.m_szBuffer[i]为0，要么都为0
			if (left.m_szBuffer[i])
			{
				return false;
			}
			if (right.m_szBuffer[i])
			{
				return true;
			}
			return false;
		}
	}
	return false;
}
//-----------------------------------------------------------------------------
