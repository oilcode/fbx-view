//------------------------------------------------------------
#ifndef _SoReadFile_h_
#define _SoReadFile_h_
//------------------------------------------------------------
class SoReadFile
{
public:
	SoReadFile();
	~SoReadFile();

	bool LoadFile(const char* pFileName);
	void ClearFile();

	//if reach the end of file, then return NULL
	const char* GetFirstLine();
	const char* GetNextLine();
	int GetLineLen() const;

private:
	bool CopyNextLine();

private:
	char* m_pFileName;
	char* m_pFileBuff;
	char* m_pLine;
	int m_nFileLen; //not include ZERO character
	int m_nLineLen; //not include ZERO character
	int m_nReadPos;
};
//------------------------------------------------------------
#endif //_SoReadFile_h_
//------------------------------------------------------------
