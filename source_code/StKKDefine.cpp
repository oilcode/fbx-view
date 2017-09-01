//----------------------------------------------------------------
#include "StKKDefine.h"
//----------------------------------------------------------------
const char* StKkbFileFlagString = "stkkb";
const char* StKkfFileFlagString = "stkkf";
//----------------------------------------------------------------
StKKModelData::StKKModelData()
:pVertexStructBuffer(0)
,pIndexBuffer(0)
,pVertexValueTexture(0)
,nVertexType(StKKVertexType_Invalid)
,nVertexCount(0)
,nIndexCount(0)
,nSizeofVertexStruct(0)
,nTextureWidth(0)
,nTextureHeight(0)
,nPosCount(0)
,nNormalCount(0)
,nUVCount(0)
{

}
//----------------------------------------------------------------
StKKModelData::~StKKModelData()
{
	if (pVertexStructBuffer)
	{
		pVertexStructBuffer->Release();
		pVertexStructBuffer = 0;
	}
	if (pIndexBuffer)
	{
		pIndexBuffer->Release();
		pIndexBuffer = 0;
	}
	if (pVertexValueTexture)
	{
		pVertexValueTexture->RemoveRefTexture();
		pVertexValueTexture = 0;
	}
}
//----------------------------------------------------------------
StKKModelAnimation::StKKModelAnimation()
:pAnimationTexture(0)
,nAnimID(-1)
,nTextureWidth(0)
,nTextureHeight(0)
,nTimeLength(0)
,nKeyFrameCount(0)
,nBoneCount(0)
{

}
//----------------------------------------------------------------
StKKModelAnimation::~StKKModelAnimation()
{
	if (pAnimationTexture)
	{
		pAnimationTexture->RemoveRefTexture();
		pAnimationTexture = 0;
	}
}
//----------------------------------------------------------------
