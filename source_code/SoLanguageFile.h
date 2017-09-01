//------------------------------------------------------------
#ifndef _SoLanguageFile_h_
#define _SoLanguageFile_h_
//------------------------------------------------------------
class SoLanguageFile
{
public:
	SoLanguageFile();
	~SoLanguageFile();

	bool InitLanguageFile(const char* pszFileName);

	const char* GetValue(const char* pszKey);

private:
	void ClearAll();
	bool LoadFile(const char* pszFileName);
	//获取文件中一共有多少行
	int GetLineCount();
	bool ParseFile();
	char* GetNextLine();
	bool ParseFileLine(char* pFileLine);
	void SlimString(char* pBuff, int nValidSize, int& nAdjustStartIndex);
	bool MakeLanguageItem(char* pszKey, char* pszValue);

private:
	struct stLanguageItem
	{
		unsigned int uiHashKeyA;
		unsigned int uiHashKeyB;
		const char* pszKey;
		const char* pszValue;
	};

private:
	//申请的内存块，用于存储整个Language文件。
	char* m_pFileBuff;
	int m_nFileBuffSize;
	//数组
	stLanguageItem* m_pItemList;
	//数组元素的个数
	int m_nItemCount;
	//解析文件过程中使用的变量。
	int m_nReadPos;
};
//------------------------------------------------------------
#endif //_SoLanguageFile_h_
//------------------------------------------------------------
