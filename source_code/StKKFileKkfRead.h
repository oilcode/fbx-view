//----------------------------------------------------------------
#ifndef _StKKFileKkfRead_h_
#define _StKKFileKkfRead_h_
//----------------------------------------------------------------
#include "StKKDefine.h"
//----------------------------------------------------------------
class StKKFileKkfRead
{
public:
	static bool ReadKkf(const char* szFileName, StKKModelAnimation* pModelAnim);

private:
	static bool LoadFileData(const char* szFileName, StKkfFileHead** ppFileHead, char** ppTextureData, int* pTextureWidth, int* pTextureHeight);
	static bool CreateAnimTexture(int nWidth, int nHeight, const char* pSrcBuff, SoD3DTexture** ppTexture);
	static void GenerateWidthHeight(int nPixelCount, int* pWidth, int* pHeight);
};
//----------------------------------------------------------------
#endif //_StKKFileKkfRead_h_
//----------------------------------------------------------------
