//-----------------------------------------------------------------------------
#ifndef _SoTinyString_h_
#define _SoTinyString_h_
//-----------------------------------------------------------------------------
//本对象中最多有多少个字符（包括结束符）。
//本对象的sizeof值是 (MaxCharCount_TinyString + 1) 。
#define MaxCharCount_TinyString 55
//-----------------------------------------------------------------------------
class SoTinyString
{
public:
	SoTinyString();
	SoTinyString(const char* pszString);
	SoTinyString(const SoTinyString& other);
	~SoTinyString();

	//注意，pszString字符串的size（包括结束符）不能大于MaxCharCount_TinyString；
	//如果大于的话，会被截断。
	//如果pszString值为NULL，则m_szBuffer被置为空字符串""。
	void SetValue(const char* pszString);
	const char* GetValue() const;
	char* GetBuffer();
	//获取有效字符的个数，不包括结束符。
	int GetLength() const;
	void Clear();

	//用等号赋值操作
	void operator = (const char* pszString);
	void operator = (const SoTinyString& other);

	//比较运算
	//STL容器在排序时会使用到<运算符
	friend bool operator < (const SoTinyString& left, const SoTinyString& right);

private:
	char m_szBuffer[MaxCharCount_TinyString];
	unsigned char m_nLength;
};
//-----------------------------------------------------------------------------
inline SoTinyString::SoTinyString()
{
	Clear();
}
//-----------------------------------------------------------------------------
inline SoTinyString::SoTinyString(const char* pszString)
{
	SetValue(pszString);
}
//-----------------------------------------------------------------------------
inline SoTinyString::SoTinyString(const SoTinyString& other)
{
	SetValue(other.GetValue());
}
//-----------------------------------------------------------------------------
inline SoTinyString::~SoTinyString()
{

}
//-----------------------------------------------------------------------------
inline const char* SoTinyString::GetValue() const
{
	return m_szBuffer;
}
//-----------------------------------------------------------------------------
inline char* SoTinyString::GetBuffer()
{
	return m_szBuffer;
}
//-----------------------------------------------------------------------------
inline int SoTinyString::GetLength() const
{
	return m_nLength;
}
//-----------------------------------------------------------------------------
inline void SoTinyString::Clear()
{
	m_szBuffer[0] = 0;
	m_nLength = 0;
}
//-----------------------------------------------------------------------------
inline void SoTinyString::operator = (const char* pszString)
{
	if (pszString != 0 && (&(m_szBuffer[0]) == &(pszString[0])))
	{
		//不能把自己赋值给自己
		return;
	}
	else
	{
		SetValue(pszString);
	}
}
//-----------------------------------------------------------------------------
inline void SoTinyString::operator = (const SoTinyString& other)
{
	if (this == &other)
	{
		//不能把自己赋值给自己
		return;
	}
	else
	{
		SetValue(other.GetValue());
	}
}
//-----------------------------------------------------------------------------
#endif //_SoTinyString_h_
//-----------------------------------------------------------------------------
