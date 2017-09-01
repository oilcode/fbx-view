//----------------------------------------------------------------
#ifndef _SoFileBmp_h_
#define _SoFileBmp_h_
//----------------------------------------------------------------
//BMP图片不支持透明通道。
//当生成BMP图片时，源数据可以是FileBmpPixelFormat_RGBA或者FileBmpPixelFormat_ARGB。
enum eFileBmpPixelFormat
{
	FileBmpPixelFormat_RGB, //内存地址 0xBBGGRR
	FileBmpPixelFormat_RGBA, //内存地址 0xAABBGGRR
	FileBmpPixelFormat_ARGB, //内存地址 0xBBGGRRAA
};
//----------------------------------------------------------------
//像素数据的坐标系类型。
//对于一张BMP图片，它的像素数据坐标系是FileBmpDataCoordSystem_Left2Right_Down2Up。
enum eFileBmpDataCoordSystemType
{
	FileBmpDataCoordSystem_Left2Right_Up2Down,
	FileBmpDataCoordSystem_Left2Right_Down2Up,
};
//----------------------------------------------------------------
struct stFileBmpBaseInfo
{
	int nPictureWidth;
	int nPictureHeight;
	eFileBmpPixelFormat ePixelFormat;
	eFileBmpDataCoordSystemType eCoordSysType;
	int nDataSize;
	char* pData;

	stFileBmpBaseInfo()
	{
		Clear();
	}
	void Clear()
	{
		nPictureWidth = 0;
		nPictureHeight = 0;
		ePixelFormat = FileBmpPixelFormat_RGB;
		eCoordSysType = FileBmpDataCoordSystem_Left2Right_Down2Up;
		nDataSize = 0;
		pData = 0;
	}
};
//----------------------------------------------------------------
class SoFileBmp
{
public:
	static bool ReadBmpFile(const char* szFile, stFileBmpBaseInfo* pInfo);
	static bool WriteBmpFile(const char* szFile, const stFileBmpBaseInfo* pInfo);
};
//----------------------------------------------------------------
#endif //_SoFileBmp_h_
//----------------------------------------------------------------
