//----------------------------------------------------------------
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif //_CRT_SECURE_NO_WARNINGS
//----------------------------------------------------------------
#include "StKKFileKkfRead.h"
#include "StFBXDefine.h"
#include "SoD3DSystem.h"
#include "SoD3DTextureManager.h"
#include "SoSimpleLog.h"
//----------------------------------------------------------------
bool StKKFileKkfRead::ReadKkf(const char* szFileName, StKKModelAnimation* pModelAnim)
{
	if (szFileName == 0 || szFileName[0] == 0)
	{
		return false;
	}
	if (pModelAnim == 0)
	{
		return false;
	}

	StKkfFileHead* pFileHead = 0;
	char* pTextureData = 0;

	SoD3DTexture* theAnimTexture = 0;
	int nTextureWidth = 0;
	int nTextureHeight = 0;

	SoLogDebug("StKKFileKkfRead::ReadKkf : Start; FileName[%s]", szFileName);

	bool br = false;
	do 
	{
		if (LoadFileData(szFileName, &pFileHead, &pTextureData, &nTextureWidth, &nTextureHeight) == false)
		{
			break;
		}

		if (CreateAnimTexture(nTextureWidth, nTextureHeight, pTextureData, &theAnimTexture) == false)
		{
			break;
		}

		br = true;

	} while (0);


	if (br == true)
	{
		pModelAnim->pAnimationTexture = theAnimTexture;
		pModelAnim->nTextureWidth = nTextureWidth;
		pModelAnim->nTextureHeight = nTextureHeight;
		pModelAnim->nTimeLength = pFileHead->TimeLength;
		pModelAnim->nKeyFrameCount = pFileHead->KeyFrameCount;
		pModelAnim->nBoneCount = pFileHead->BoneCount;
	}
	else
	{
		if (theAnimTexture)
		{
			theAnimTexture->RemoveRefTexture();
			theAnimTexture = 0;
		}
	}

	//不管操作是否成功，都要删除下面的内存。
	if (pFileHead)
	{
		free(pFileHead);
		pFileHead = 0;
	}
	if (pTextureData)
	{
		free(pTextureData);
		pTextureData = 0;
	}

	SoLogDebug("StKKFileKkfRead::ReadKkf : Finish; Success[%d]", br?1:0);
	return br;
}
//----------------------------------------------------------------
bool StKKFileKkfRead::LoadFileData(const char* szFileName, StKkfFileHead** ppFileHead, char** ppTextureData, int* pTextureWidth, int* pTextureHeight)
{
	FILE* fp = fopen(szFileName, "rb");
	if (fp == 0)
	{
		SoLogError("StKKFileKkfRead::LoadFileData : fopen fail! [%s]", szFileName);
		return false;
	}

	StKkfFileHead* pFileHead = 0;
	char* pTextureData = 0;
	int nWidth = 0;
	int nHeight = 0;

	bool br = false;
	do 
	{
		//读取文件头。
		const unsigned int SizeofFileHead = sizeof(StKkfFileHead);
		pFileHead = (StKkfFileHead*)malloc(SizeofFileHead);
		if (pFileHead == 0)
		{
			SoLogError("StKKFileKkfRead::LoadFileData : malloc fail! size[%d]", SizeofFileHead);
			break;
		}

		unsigned int ReadCount = fread(pFileHead, 1, SizeofFileHead, fp);
		if (ReadCount != SizeofFileHead)
		{
			SoLogError("StKKFileKkfRead::LoadFileData : fread FileHead fail!");
			break;
		}

		//判断文件标记。
		bool bFileFlagOK = true;
		for (int i = 0; i < StKkfFileFlagSize; ++i)
		{
			if (pFileHead->FileFlag[i] != StKkfFileFlagString[i])
			{
				bFileFlagOK = false;
				break;
			}
		}
		if (bFileFlagOK == false)
		{
			SoLogError("StKKFileKkfRead::LoadFileData : FileFlag check fail!");
			break;
		}

		//读取顶点元素值的贴图数据。
		//为了创建贴图，必须把末尾行扩充成整行。
		const unsigned int TextureDataSize = pFileHead->KeyFrameCount * pFileHead->BoneCount * sizeof(StKKBoneTransform);
		//一个像素是4个字节
		const int BytePerPixel = 4;
		const int PixelCount = ((int)TextureDataSize) / BytePerPixel;
		GenerateWidthHeight(PixelCount, &nWidth, &nHeight);
		//计算扩充成整行之后的buff大小。
		const int SizeofNewSrcBuff = nWidth * nHeight * BytePerPixel;
		//
		pTextureData = (char*)malloc(SizeofNewSrcBuff);
		if (pTextureData == 0)
		{
			SoLogError("StKKFileKkfRead::LoadFileData : malloc fail! size[%d]", SizeofNewSrcBuff);
			break;
		}

		ReadCount = fread(pTextureData, 1, TextureDataSize, fp);
		if (ReadCount != TextureDataSize)
		{
			SoLogError("StKKFileKkfRead::LoadFileData : fread TextureData fail!");
			break;
		}

		br = true;

	} while (0);

	if (br == true)
	{
		*ppFileHead = pFileHead;
		*ppTextureData = pTextureData;
		*pTextureWidth = nWidth;
		*pTextureHeight = nHeight;
	}
	else
	{
		if (pFileHead)
		{
			free(pFileHead);
			pFileHead = 0;
		}
		if (pTextureData)
		{
			free(pTextureData);
			pTextureData = 0;
		}
	}

	fclose(fp);
	return br;
}
//----------------------------------------------------------------
bool StKKFileKkfRead::CreateAnimTexture(int nWidth, int nHeight, const char* pSrcBuff, SoD3DTexture** ppTexture)
{
	SoD3DTexture* pTex = SoD3DTextureManager::Get()->CreateAnimDataTextureDynamic(nWidth, nHeight, (void*)pSrcBuff);
	if (pTex)
	{
		pTex->AddRefTexture();
		*ppTexture = pTex;
		return true;
	}
	else
	{
		SoLogError("StKKFileKkfRead::CreateVertexValueTexture : CreateAnimDataTextureDynamic fail!");
		return false;
	}
}
//----------------------------------------------------------------
void StKKFileKkfRead::GenerateWidthHeight(int nPixelCount, int* pWidth, int* pHeight)
{
	const int TryWidth = 128;
	*pWidth = TryWidth;
	*pHeight = (nPixelCount + TryWidth - 1) / TryWidth;
}
//----------------------------------------------------------------
