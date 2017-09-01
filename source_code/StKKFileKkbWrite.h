//----------------------------------------------------------------
#ifndef _StKKFileKkbWrite_h_
#define _StKKFileKkbWrite_h_
//----------------------------------------------------------------
#include "StKKDefine.h"
//----------------------------------------------------------------
class StFBXModel;
//----------------------------------------------------------------
class StKKFileKkbWrite
{
public:
	StKKFileKkbWrite();
	~StKKFileKkbWrite();

	bool WriteKkb(const char* szFileName, StFBXModel* pModel);

private:
	void ClearFileKkb();
	bool Write_CheckVertexType();
	bool Write_FillFileHead();
	bool Write_GenerateVertexStructBuff();
	bool Write_GeneratePosBuff();
	bool Write_GenerateNormalBuff();
	bool Write_GenerateUVBuff();
	bool Write_GenerateBoneWeight();
	bool Write_WriteFile(const char* szFileName);

private:
	StKkbFileHead m_kFileHead;
	char* m_pVertexStructBuff;
	char* m_pPosBuff;
	char* m_pNormalBuff;
	char* m_pUVBuff;
	StFBXModel* m_pFbxModel;
};
//----------------------------------------------------------------
#endif //_StKKFileKkbWrite_h_
//----------------------------------------------------------------
