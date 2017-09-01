//-----------------------------------------------------------------------------
//1，如果键值对结束的标识符为空格，那么下面这个字符串就会解析错误：
//   "x=  y=9"
//   解析结果：x的值为"y=9"。所以，不要把空格作为键值对结束的标识符。
//-----------------------------------------------------------------------------
#include "SoCmdLineHelp.h"
#include "SoStringHelp.h"
//-----------------------------------------------------------------------------
#define SoCmdLineHelp_GlobalBuffSize 2048
//-----------------------------------------------------------------------------
//公共缓存区，用于存储返回值
char g_CmdLineBuff[SoCmdLineHelp_GlobalBuffSize] = { 0 };
wchar_t* g_WCmdLineBuff = (wchar_t*)g_CmdLineBuff;
//键与值之间的间隔符号有哪些
//空格，等号，冒号
const wchar_t* g_Signal = L" =:";
//键值对结束的标识符有哪些
//空格，逗号，分号，大括号
const wchar_t* g_EndFlag = L" ,;{}";
//-----------------------------------------------------------------------------
bool CheckIsSignal(const wchar_t theChar)
{
	bool br = false;
	int nIndex = 0;
	while (g_Signal[nIndex] != 0)
	{
		if (g_Signal[nIndex] == theChar)
		{
			br = true;
			break;
		}
		else
		{
			++nIndex;
		}
	}
	return br;
}
//-----------------------------------------------------------------------------
bool CheckIsEndFlag(const wchar_t theChar)
{
	bool br = false;
	int nIndex = 0;
	while (g_EndFlag[nIndex] != 0)
	{
		if (g_EndFlag[nIndex] == theChar)
		{
			br = true;
			break;
		}
		else
		{
			++nIndex;
		}
	}
	return br;
}
//-----------------------------------------------------------------------------
char* SoCmdLineHelp::GetValueByKey(const char* szCmdLine, const int nCmdLength, const char* szKey)
{
	//清空公共缓存区
	g_CmdLineBuff[0] = 0;

	if (szCmdLine == 0 || szCmdLine[0] == 0)
	{
		return g_CmdLineBuff;
	}
	if (szKey == 0 || szKey[0] == 0)
	{
		return g_CmdLineBuff;
	}

	//判断字符串长度，其长度不能是无限大。
	const int nRealCmdLen = (nCmdLength == -1) ? 999999 : nCmdLength;
	const int nKeyLen = SoStrLen(szKey);

	//第一步，在szCmdLine中查找key所在的位置
	int nCurIndex = 0;
	bool bFindKey = false;
	//记录前一个字符是否为“键值对结束的标识符”
	bool bLastKeyIsEndFlag = true;
	while (szCmdLine[nCurIndex] != 0 && nCurIndex < nRealCmdLen)
	{
		const bool bCurKeyIsEndFlag = CheckIsEndFlag(szCmdLine[nCurIndex]);
		if (bLastKeyIsEndFlag == true && bCurKeyIsEndFlag == false)
		{
			//“键值对结束的标识符”的后面遇到非结束符，则开始判断是否为Key字符串
			if (szCmdLine[nCurIndex] == szKey[0])
			{
				//命令行中有个字符与Key的第一个字符相同，判断Key字符串是否出现
				bool bAppear = true;
				for (int i = 0; i < nKeyLen; ++i)
				{
					if (szCmdLine[nCurIndex+i] == szKey[i])
					{
						//相同，继续比较，什么都不做
					}
					else
					{
						//不相同
						bAppear = false;
						break;
					}
				}
				if (bAppear == true)
				{
					//Key字符串出现了，判断接下来的字符是否为“键与值之间的间隔符号”，
					//如果不是“键与值之间的间隔符号”，说明并没有找到真正的Key字符串。
					if (CheckIsSignal(szCmdLine[nCurIndex+nKeyLen]) == true)
					{
						bFindKey = true;
					}
				}
			}
		}
		//
		if (bFindKey)
		{
			//此时，szCmdLine[nCurIndex]就是szKey[0]
			break;
		}
		else
		{
			bLastKeyIsEndFlag = bCurKeyIsEndFlag;
			++nCurIndex;
		}
	}

	if (bFindKey == false)
	{
		//命令行中没有找到Key字符串
		return g_CmdLineBuff;
	}

	//已经找到Key字符串
	//让下标序号跳过Key字符串
	nCurIndex += nKeyLen;
	//此时szCmdLine[nCurIndex]就是“键与值之间的间隔符号”
	//跳过间隔符号和空格，找到值字符串的第一个字符
	bool bFindValue = false;
	while (szCmdLine[nCurIndex] != 0 && nCurIndex < nRealCmdLen)
	{
		if (CheckIsSignal(szCmdLine[nCurIndex]) == true)
		{
			++nCurIndex;
		}
		else
		{
			bFindValue = true;
			break;
		}
	}
	if (bFindValue == false)
	{
		//没有找到Value字符串
		return g_CmdLineBuff;
	}

	//把Value字符串拷贝到g_Buff中，直到遇到“键值对结束的标识符”
	int nValueLen = 0;
	while (szCmdLine[nCurIndex] != 0 && nCurIndex < nRealCmdLen)
	{
		if (CheckIsEndFlag(szCmdLine[nCurIndex]) == true)
		{
			g_CmdLineBuff[nValueLen] = 0;
			break;
		}
		else
		{
			g_CmdLineBuff[nValueLen] = szCmdLine[nCurIndex];
			++nValueLen;
			++nCurIndex;
		}
	}

	//再次设置结束符。
	//如果while循环的结束不是从break跳出，而是遇到了szCmdLine[nCurIndex]值为0，
	//则g_Buff不是以0结尾的。所以需要再次设置结束符。
	g_CmdLineBuff[nValueLen] = 0;
	return g_CmdLineBuff;
}
//-----------------------------------------------------------------------------
int SoCmdLineHelp::GetIntByKey(const char* szCmdLine, const int nCmdLength, const char* szKey, const int nDefaultValue)
{
	const char* szValue = GetValueByKey(szCmdLine, nCmdLength, szKey);
	if (szValue[0] != 0)
	{
		return Soatoi(szValue);
	}
	else
	{
		return nDefaultValue;
	}
}
//-----------------------------------------------------------------------------
float SoCmdLineHelp::GetFloatByKey(const char* szCmdLine, const int nCmdLength, const char* szKey, const float fDefaultValue)
{
	const char* szValue = GetValueByKey(szCmdLine, nCmdLength, szKey);
	if (szValue[0] != 0)
	{
		return (float)Soatof(szValue);
	}
	else
	{
		return fDefaultValue;
	}
}
//-----------------------------------------------------------------------------
wchar_t* SoCmdLineHelp::GetValueByKey_W(const wchar_t* szCmdLine, const int nCmdLength, const wchar_t* szKey)
{
	//清空公共缓存区
	g_WCmdLineBuff[0] = 0;

	if (szCmdLine == 0 || szCmdLine[0] == 0)
	{
		return g_WCmdLineBuff;
	}
	if (szKey == 0 || szKey[0] == 0)
	{
		return g_WCmdLineBuff;
	}

	const int nRealCmdLen = (nCmdLength == -1) ? 999999 : nCmdLength;
	const int nKeyLen = SoWStrLen(szKey);

	//第一步，在szCmdLine中查找key所在的位置
	int nCurIndex = 0;
	bool bFindKey = false;
	//记录前一个字符是否为“键值对结束的标识符”
	bool bLastKeyIsEndFlag = true;
	while (szCmdLine[nCurIndex] != 0 && nCurIndex < nRealCmdLen)
	{
		const bool bCurKeyIsEndFlag = CheckIsEndFlag(szCmdLine[nCurIndex]);
		if (bLastKeyIsEndFlag == true && bCurKeyIsEndFlag == false)
		{
			//“键值对结束的标识符”的后面遇到非结束符，则开始判断是否为Key字符串
			if (szCmdLine[nCurIndex] == szKey[0])
			{
				//命令行中有个字符与Key的第一个字符相同，判断Key字符串是否出现
				bool bAppear = true;
				for (int i = 0; i < nKeyLen; ++i)
				{
					if (szCmdLine[nCurIndex+i] == szKey[i])
					{
						//相同，继续比较，什么都不做
					}
					else
					{
						//不相同
						bAppear = false;
						break;
					}
				}
				if (bAppear == true)
				{
					//Key字符串出现了，判断接下来的字符是否为“键与值之间的间隔符号”，
					//如果不是“键与值之间的间隔符号”，说明并没有找到真正的Key字符串。
					if (CheckIsSignal(szCmdLine[nCurIndex+nKeyLen]) == true)
					{
						bFindKey = true;
					}
				}
			}
		}
		//
		if (bFindKey)
		{
			//此时，szCmdLine[nCurIndex]就是szKey[0]
			break;
		}
		else
		{
			bLastKeyIsEndFlag = bCurKeyIsEndFlag;
			++nCurIndex;
		}
	}

	if (bFindKey == false)
	{
		//命令行中没有找到Key字符串
		return g_WCmdLineBuff;
	}

	//已经找到Key字符串
	//让下标序号跳过Key字符串
	nCurIndex += nKeyLen;
	//此时szCmdLine[nCurIndex]就是“键与值之间的间隔符号”
	//跳过间隔符号和空格，找到值字符串的第一个字符
	bool bFindValue = false;
	while (szCmdLine[nCurIndex] != 0 && nCurIndex < nRealCmdLen)
	{
		if (CheckIsSignal(szCmdLine[nCurIndex]) == true)
		{
			++nCurIndex;
		}
		else
		{
			bFindValue = true;
			break;
		}
	}
	if (bFindValue == false)
	{
		//没有找到Value字符串
		return g_WCmdLineBuff;
	}

	//把Value字符串拷贝到g_Buff中，直到遇到“键值对结束的标识符”
	int nValueLen = 0;
	while (szCmdLine[nCurIndex] != 0 && nCurIndex < nRealCmdLen)
	{
		if (CheckIsEndFlag(szCmdLine[nCurIndex]) == true)
		{
			g_WCmdLineBuff[nValueLen] = 0;
			break;
		}
		else
		{
			g_WCmdLineBuff[nValueLen] = szCmdLine[nCurIndex];
			++nValueLen;
			++nCurIndex;
		}
	}

	//再次设置结束符。
	//如果while循环的结束不是从break跳出，而是遇到了szCmdLine[nCurIndex]值为0，
	//则g_Buff不是以0结尾的。所以需要再次设置结束符。
	g_WCmdLineBuff[nValueLen] = 0;
	return g_WCmdLineBuff;
}
//-----------------------------------------------------------------------------
int SoCmdLineHelp::GetIntByKey_W(const wchar_t* szCmdLine, const int nCmdLength, const wchar_t* szKey, const int nDefaultValue)
{
	const wchar_t* szValue = GetValueByKey_W(szCmdLine, nCmdLength, szKey);
	if (szValue[0] != 0)
	{
		return SoWatoi(szValue);
	}
	else
	{
		return nDefaultValue;
	}
}
//-----------------------------------------------------------------------------
float SoCmdLineHelp::GetFloatByKey_W(const wchar_t* szCmdLine, const int nCmdLength, const wchar_t* szKey, const float fDefaultValue)
{
	const wchar_t* szValue = GetValueByKey_W(szCmdLine, nCmdLength, szKey);
	if (szValue[0] != 0)
	{
		return (float)SoWatof(szValue);
	}
	else
	{
		return fDefaultValue;
	}
}
//-----------------------------------------------------------------------------
