//----------------------------------------------------------------
#ifndef _GGUIFileGGM_h_
#define _GGUIFileGGM_h_
//----------------------------------------------------------------
#include "GGUIDefine.h"
//----------------------------------------------------------------
class GGUIFileGGM
{
public:
	GGUIFileGGM();
	~GGUIFileGGM();

	bool ReadFcfFile(const char* szFileName);
	bool GetTextureName(std::string& strTextureName);
	bool GetNextImageRect(SoTinyString& kName, GGUIRect& kRect);
	int GetImageRectCount();

	void CloseFcfFile();

private:
	char* ReadNextFileLine();

private:
	FILE* m_pf;
	//存储整个文件的数据。
	char* m_pFileBuff;
	int m_nFileSize;
	//读操作的光标位置。
	int m_nReadPointer;
};
//----------------------------------------------------------------
#endif //_GGUIFileGGM_h_
//----------------------------------------------------------------
