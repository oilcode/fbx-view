//----------------------------------------------------------------
#ifndef _StKKFileKkfWrite_h_
#define _StKKFileKkfWrite_h_
//----------------------------------------------------------------
#include "StKKDefine.h"
//----------------------------------------------------------------
class StFBXModel;
//----------------------------------------------------------------
class StKKFileKkfWrite
{
public:
	StKKFileKkfWrite();
	~StKKFileKkfWrite();

	bool WriteKkf(const char* szFileName, StFBXModel* pModel);

private:
	void ClearFileKkf();
	bool Write_FillFileHead();
	bool Write_GenerateBoneTransformBuff();
	bool Write_WriteFile(const char* szFileName);

private:
	StKkfFileHead m_kFileHead;
	char* m_pBoneTransformBuff;
	StFBXModel* m_pFbxModel;
};
//----------------------------------------------------------------
#endif //_StKKFileKkfWrite_h_
//----------------------------------------------------------------
