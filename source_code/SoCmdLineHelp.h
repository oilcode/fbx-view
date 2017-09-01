//-----------------------------------------------------------------------------
#ifndef _SoCmdLineHelp_h_
#define _SoCmdLineHelp_h_
//-----------------------------------------------------------------------------
class SoCmdLineHelp
{
public:
	//--nCmdLength 字符串长度。可以设置为-1，函数内部会妥善处理。
	static char* GetValueByKey(const char* szCmdLine, const int nCmdLength, const char* szKey);
	static int GetIntByKey(const char* szCmdLine, const int nCmdLength, const char* szKey, const int nDefaultValue);
	static float GetFloatByKey(const char* szCmdLine, const int nCmdLength, const char* szKey, const float fDefaultValue);
	//
	static wchar_t* GetValueByKey_W(const wchar_t* szCmdLine, const int nCmdLength, const wchar_t* szKey);
	static int GetIntByKey_W(const wchar_t* szCmdLine, const int nCmdLength, const wchar_t* szKey, const int nDefaultValue);
	static float GetFloatByKey_W(const wchar_t* szCmdLine, const int nCmdLength, const wchar_t* szKey, const float fDefaultValue);
};
//-----------------------------------------------------------------------------
#endif //_SoCmdLineHelp_h_
//-----------------------------------------------------------------------------
