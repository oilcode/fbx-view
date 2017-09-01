//------------------------------------------------------------
#ifndef _SoLanguageRule_h_
#define _SoLanguageRule_h_
//------------------------------------------------------------
#define SoLanguageRule_MaxSize_FileLine 1024000
//------------------------------------------------------------
struct stItemKeyValue
{
	const char* pKey;
	const char* pValue;
	int nKeyLen;
	int nValueLen;

	stItemKeyValue() : pKey(0), pValue(0), nKeyLen(0), nValueLen(0)
	{
	}
};
//------------------------------------------------------------
class SoLanguageRule
{
public:
	//--pItem [out]
	//--pString [in]
	//--nStrLen [in]
	static bool Parse(stItemKeyValue* pItem, const char* pString, const int nStrLen);
};
//------------------------------------------------------------
#endif //_SoLanguageRule_h_
//------------------------------------------------------------
