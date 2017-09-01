//--------------------------------------------------------------------
#include <Windows.h>
#include <strsafe.h>
#include "SoStringConvert.h"
//--------------------------------------------------------------------
//字符缓冲区，存储字符串转换后的结果。
//注意，缓冲区的大小是固定的，外界调用函数时要小心，转换后的字符个数不能太多。
const int MaxBufferSize = 2048;
char theCharBuffer[MaxBufferSize];
//--------------------------------------------------------------------
int SoUnicodeCountFromAnsi(const char* pString)
{
	return ::MultiByteToWideChar(CP_ACP, 0, pString, -1, NULL, 0);
}
//--------------------------------------------------------------------
int SoAnsiCountFromUnicode(const wchar_t* pString)
{
	return ::WideCharToMultiByte(CP_ACP, 0, pString, -1, NULL, 0, NULL, NULL);
}
//--------------------------------------------------------------------
int SoUnicodeCountFromUtf8(const char* pString)
{
	return ::MultiByteToWideChar(CP_UTF8, 0, pString, -1, NULL, 0);
}
//--------------------------------------------------------------------
int SoUtf8CountFromUnicode(const wchar_t* pString)
{
	return ::WideCharToMultiByte(CP_UTF8, 0, pString, -1, NULL, 0, NULL, NULL);
}
//--------------------------------------------------------------------
wchar_t* SoAnsiToUnicode(const char* pSrcString) 
{
	wchar_t* pResult = (wchar_t*)theCharBuffer;
	pResult[0] = 0;
	//
	if (pSrcString == NULL)
	{
		return pResult;
	}
	int nWCharCount = SoUnicodeCountFromAnsi(pSrcString);
	if (nWCharCount == 0)
	{
		//出现错误。
		return pResult;
	}
	if (nWCharCount*2 >= MaxBufferSize)
	{
		//字符个数太多，超过缓冲区大小。
		return pResult;
	}
	::MultiByteToWideChar(CP_ACP, 0, pSrcString, -1, pResult, nWCharCount);
	return pResult;
}
//--------------------------------------------------------------------
char* SoUnicodeToAnsi(const wchar_t* pSrcString)
{
	char* pResult = theCharBuffer;
	pResult[0] = 0;
	//
	if (pSrcString == NULL)
	{
		return pResult;
	}
	int nByteCount = SoAnsiCountFromUnicode(pSrcString);
	if (nByteCount == 0)
	{
		//出现错误。
		return pResult;
	}
	if (nByteCount >= MaxBufferSize)
	{
		//字符个数太多，超过缓冲区大小。
		return pResult;
	}
	::WideCharToMultiByte(CP_ACP, 0, pSrcString, -1, pResult, nByteCount, NULL, NULL);
	return pResult;
}
//--------------------------------------------------------------------
wchar_t* SoUtf8ToUnicode(const char* pSrcString)
{
	wchar_t* pResult = (wchar_t*)theCharBuffer;
	pResult[0] = 0;
	//
	if (pSrcString == NULL)
	{
		return pResult;
	}
	int nWCharCount = SoUnicodeCountFromUtf8(pSrcString);
	if (nWCharCount == 0)
	{
		//出现错误。
		return pResult;
	}
	if (nWCharCount*2 >= MaxBufferSize)
	{
		//字符个数太多，超过缓冲区大小。
		return pResult;
	}
	::MultiByteToWideChar(CP_UTF8, 0, pSrcString, -1, pResult, nWCharCount);
	return pResult;
}
//--------------------------------------------------------------------
char* SoUnicodeToUtf8(const wchar_t* pSrcString)
{
	char* pResult = theCharBuffer;
	pResult[0] = 0;
	//
	if (pSrcString == NULL)
	{
		return pResult;
	}
	int nByteCount = SoUtf8CountFromUnicode(pSrcString);
	if (nByteCount == 0)
	{
		//出现错误。
		return pResult;
	}
	if (nByteCount >= MaxBufferSize)
	{
		//字符个数太多，超过缓冲区大小。
		return pResult;
	}
	::WideCharToMultiByte(CP_UTF8, 0, pSrcString, -1, pResult, nByteCount, NULL, NULL);
	return pResult;
}
//--------------------------------------------------------------------
char* SoAnsiToUtf8(const char* pSrcString)
{
	//首先把Ansi字符串转换成Unicode格式。
	wchar_t* pUnicodeString = SoAnsiToUnicode(pSrcString);
	if (pUnicodeString[0] == 0)
	{
		return theCharBuffer;
	}
	//
	wchar_t tempBuffer[MaxBufferSize];
	StringCbCopyW(tempBuffer, sizeof(tempBuffer), pUnicodeString);
	//再把Unicode字符串转换成Utf8格式。
	return SoUnicodeToUtf8(tempBuffer);
}
//--------------------------------------------------------------------
char* SoUtf8ToAnsi(const char* pSrcString)
{
	//首先把Utf8字符串转换成Unicode格式。
	wchar_t* pUnicodeString = SoUtf8ToUnicode(pSrcString);
	if (pUnicodeString[0] == 0)
	{
		return theCharBuffer;
	}
	//
	wchar_t tempBuffer[MaxBufferSize];
	StringCbCopyW(tempBuffer, sizeof(tempBuffer), pUnicodeString);
	//再把Unicode字符串转换成Ansi格式。
	return SoUnicodeToAnsi(tempBuffer);
}
//--------------------------------------------------------------------
