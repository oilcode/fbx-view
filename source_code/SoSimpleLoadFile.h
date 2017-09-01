//----------------------------------------------------------------
#ifndef _SoSimpleLoadFile_h_
#define _SoSimpleLoadFile_h_
//----------------------------------------------------------------
class SoSimpleLoadFile
{
public:
	SoSimpleLoadFile();
	~SoSimpleLoadFile();
	bool LoadFile(const char* szFileName, bool bTextMode);
	void Clear();
	bool IsLoadSuccess() const;
	char* GetData() const;
	int GetSize() const;

private:
	char* m_pData;
	int m_nSize;
};
//----------------------------------------------------------------
inline bool SoSimpleLoadFile::IsLoadSuccess() const
{
	return (m_nSize != -1);
}
//----------------------------------------------------------------
inline char* SoSimpleLoadFile::GetData() const
{
	return m_pData;
}
//----------------------------------------------------------------
inline int SoSimpleLoadFile::GetSize() const
{
	return m_nSize;
}
//----------------------------------------------------------------
#endif
//----------------------------------------------------------------
