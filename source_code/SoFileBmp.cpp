//--------------------------------------------------------------------
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif //_CRT_SECURE_NO_WARNINGS
//----------------------------------------------------------------
#include "SoFileBmp.h"
#include <cstdio>
#include <string>
//----------------------------------------------------------------
#pragma pack(1)
struct stBmpFileHeader
{
	unsigned short usType;
	unsigned int uiSize;
	unsigned short usReserved1;
	unsigned short usReserved2;
	unsigned int uiOffBits;
};
struct stBmpInfoHeader
{
	unsigned int uiSize;
	int iWidth;
	int iHeight;
	unsigned short usPlanes;
	unsigned short usBitCount;
	unsigned int uiCompression;
	unsigned int uiSizeImage;
	int iXPelsPerMeter;
	int iYPelsPerMeter;
	unsigned int uiClrUsed;
	unsigned int uiClrImportant;
};
#pragma pack()
//----------------------------------------------------------------
bool SoFileBmp::ReadBmpFile(const char* szFile, stFileBmpBaseInfo* pInfo)
{
	return true;
}
//----------------------------------------------------------------
bool SoFileBmp::WriteBmpFile(const char* szFile, const stFileBmpBaseInfo* pInfo)
{
	if (szFile == 0 || szFile[0] == 0)
	{
		return false;
	}
	if (pInfo == 0)
	{
		return false;
	}

	stBmpFileHeader kFileHeader;
	kFileHeader.usType = 0x4D42;
	kFileHeader.uiSize = sizeof(stBmpFileHeader) + sizeof(stBmpInfoHeader) + pInfo->nDataSize;
	kFileHeader.usReserved1 = 0;
	kFileHeader.usReserved2 = 0;
	kFileHeader.uiOffBits = sizeof(stBmpFileHeader) + sizeof(stBmpInfoHeader);

	stBmpInfoHeader kInfoHeader;
	memset(&kInfoHeader, 0, sizeof(stBmpInfoHeader));
	kInfoHeader.uiSize = sizeof(stBmpInfoHeader);
	kInfoHeader.iWidth = pInfo->nPictureWidth;
	if (pInfo->eCoordSysType == FileBmpDataCoordSystem_Left2Right_Down2Up)
	{
		kInfoHeader.iHeight = pInfo->nPictureHeight;
	}
	else
	{
		kInfoHeader.iHeight = -(pInfo->nPictureHeight);
	}
	kInfoHeader.usPlanes = 1;
	kInfoHeader.usBitCount = 24;
	kInfoHeader.uiCompression = 0; //0==BI_RGB
	
	//
	FILE* fp = fopen(szFile, "wb+");
	if (fp == 0)
	{
		return false;
	}

	fwrite(&kFileHeader, sizeof(kFileHeader), 1, fp);
	fwrite(&kInfoHeader, sizeof(kInfoHeader), 1, fp);

	if (pInfo->ePixelFormat == FileBmpPixelFormat_RGB)
	{
		fwrite(pInfo->pData, 1, pInfo->nDataSize, fp);
	}
	else if (pInfo->ePixelFormat == FileBmpPixelFormat_ARGB)
	{
		const int nBytePitch = pInfo->nPictureWidth * 4;
		const char* pSource = NULL;
		char tempBuff[3];
		for (int h = 0; h < pInfo->nPictureHeight; ++h)
		{
			pSource = pInfo->pData + h * nBytePitch;
			for (int w = 0; w < pInfo->nPictureWidth; ++w)
			{
				tempBuff[0] = *pSource;
				++pSource;
				tempBuff[1] = *pSource;
				++pSource;
				tempBuff[2] = *pSource;
				++pSource;
				++pSource;
				fwrite(tempBuff, 1, 3, fp);
			}
		}
	}
	else
	{
		const int nBytePitch = pInfo->nPictureWidth * 4;
		const char* pSource = NULL;
		char tempBuff[3];
		for (int h = 0; h < pInfo->nPictureHeight; ++h)
		{
			pSource = pInfo->pData + h * nBytePitch;
			for (int w = 0; w < pInfo->nPictureWidth; ++w)
			{
				++pSource;
				tempBuff[0] = *pSource;
				++pSource;
				tempBuff[1] = *pSource;
				++pSource;
				tempBuff[2] = *pSource;
				++pSource;
				fwrite(tempBuff, 1, 3, fp);
			}
		}
	}

	fclose(fp);
	return true;
}
//----------------------------------------------------------------
