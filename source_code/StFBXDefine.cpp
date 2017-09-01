//----------------------------------------------------------------
#include "StFBXDefine.h"
#include "SoStringHelp.h"
#include "SoSimpleLog.h"
//----------------------------------------------------------------
StFBXMeshData::StFBXMeshData()
{
	ResetInit();
}
//----------------------------------------------------------------
StFBXMeshData::~StFBXMeshData()
{
	if (pVertexBuff)
	{
		free(pVertexBuff);
		pVertexBuff = 0;
	}
	if (pIndexBuff)
	{
		free(pIndexBuff);
		pIndexBuff = 0;
	}
	if (pVertexIndex2ControlPointIndex)
	{
		free(pVertexIndex2ControlPointIndex);
		pVertexIndex2ControlPointIndex = 0;
	}
}
//----------------------------------------------------------------
void StFBXMeshData::ResetInit()
{
	pVertexBuff = 0;
	nVertexBuffSize = 0;
	nVertexCount = 0;
	pIndexBuff = 0;
	nIndexBuffSize = 0;
	nIndexCount = 0;
	pVertexIndex2ControlPointIndex = 0;
	kVertexType.Clear();
	nSizeofVertexData = 0;
}
//----------------------------------------------------------------
void StFBXMeshData::ReserveVertexCount(const int nVertexTotalCount)
{
	nVertexCount = nVertexTotalCount;
	nVertexBuffSize = nVertexCount * nSizeofVertexData;
	nIndexCount = nVertexTotalCount;
	nIndexBuffSize = nIndexCount * sizeof(unsigned int);
	pVertexBuff = (char*)malloc(nVertexBuffSize);
	pIndexBuff = (char*)malloc(nIndexBuffSize);
	pVertexIndex2ControlPointIndex = (int*)malloc(nVertexTotalCount * sizeof(int));
}
//----------------------------------------------------------------
StFBXBoneIndexSkinWeight::StFBXBoneIndexSkinWeight()
:nBoneIndex(-1)
,fSkinWeight(0.0f)
{

}
//----------------------------------------------------------------
StFBXControlPoint::StFBXControlPoint()
{

}
//----------------------------------------------------------------
void StFBXControlPoint::ResetInit()
{
	kVertex = SoMathFloat3_Zero;
	for (int i = 0; i < StFBX_MaxCount_BoneIndexSkinWeightPerControlPoint; ++i)
	{
		kPairList[i].nBoneIndex = -1;
		kPairList[i].fSkinWeight = 0.0f;
	}
}
//----------------------------------------------------------------
void StFBXControlPoint::AddBoneIndexSkinWeight(int nBoneIndex, float fSkinWeight)
{
	bool bSuccess = false;

	for (int i = 0; i < StFBX_MaxCount_BoneIndexSkinWeightPerControlPoint; ++i)
	{
		if (kPairList[i].nBoneIndex == -1)
		{
			kPairList[i].nBoneIndex = nBoneIndex;
			kPairList[i].fSkinWeight = fSkinWeight;
			bSuccess = true;
			break;
		}
	}

	if (bSuccess == false)
	{
		SoLogError("StFBXControlPoint::AddBoneIndexSkinWeight : kPairList is full");
	}
}
//----------------------------------------------------------------
StFBXControlPointGroup::StFBXControlPointGroup()
:kControlPointArray(NULL)
,nControlPointValidCount(0)
,nControlPointMaxCount(0)
{

}
//----------------------------------------------------------------
StFBXControlPointGroup::~StFBXControlPointGroup()
{
	if (kControlPointArray)
	{
		delete[] kControlPointArray;
		kControlPointArray = NULL;
	}
}
//----------------------------------------------------------------
void StFBXControlPointGroup::ReserveControlPointCount(int nCount)
{
	kControlPointArray = new StFBXControlPoint[nCount];
	nControlPointValidCount = 0;
	nControlPointMaxCount = nCount;
}
//----------------------------------------------------------------
StFBXControlPoint* StFBXControlPointGroup::TakeNew()
{
	if (nControlPointValidCount < nControlPointMaxCount)
	{
		++nControlPointValidCount;
		return kControlPointArray + (nControlPointValidCount - 1);
	}
	else
	{
		SoLogError("StFBXControlPointGroup::TakeNew : kControlPointArray is full");
		return NULL;
	}
}
//----------------------------------------------------------------
StFBXControlPoint* StFBXControlPointGroup::GetAt(int nIndex) const
{
	if (nIndex >= 0 && nIndex < nControlPointValidCount)
	{
		return kControlPointArray + nIndex;
	}
	else
	{
		return NULL;
	}
}
//----------------------------------------------------------------
int StFBXControlPointGroup::GetSize() const
{
	return nControlPointValidCount;
}
////----------------------------------------------------------------
//void StFBXControlPointGroup::CheckBoneSkinWeight()
//{
//	const int nCount = kControlPointArray.GetSize();
//	for (int i = 0; i < nCount; ++i)
//	{
//		StFBXControlPoint* pCP = (StFBXControlPoint*)kControlPointArray.GetAt(i);
//		if (pCP)
//		{
//			float fWeightSum = 0.0f;
//			for (int m = 0; m < StFBX_MaxCount_BoneIndexSkinWeightPerControlPoint; ++m)
//			{
//				if (pCP->kPairList[m].nBoneIndex != -1)
//				{
//					fWeightSum += pCP->kPairList[m].fSkinWeight;
//				}
//				else
//				{
//					break;
//				}
//			}
//
//			if (SoMath_IsFloatZero(fWeightSum) == true)
//			{
//				//本控制点不受骨骼影响。
//				//什么都不做
//			}
//			else
//			{
//				float fDelta = 1.0f - fWeightSum;
//				if (SoMath_IsFloatZero(fDelta) == false)
//				{
//					SoLogDebug("StFBXControlPointGroup::CheckBoneSkinWeight : ControlPointIndex[%d] fDelta[%.3f]", i, fDelta);
//					//调整，使得总和是1。
//					pCP->kPairList[0].fSkinWeight += fDelta;
//				}
//			}
//		}
//	}
//}
//----------------------------------------------------------------
StFBXBone::StFBXBone()
:nParentIndex(-1)
,kMatFromBoneSpaceToModelSpace(true)
{
	for (int i = 0; i < StFBX_MaxCount_ChildBone; ++i)
	{
		kChildIndexList[i] = -1;
	}
}
//----------------------------------------------------------------
StFBXBone::~StFBXBone()
{

}
//----------------------------------------------------------------
void StFBXBone::ResetInit()
{
	kBoneName.Clear();
	nParentIndex = -1;
	for (int i = 0; i < StFBX_MaxCount_ChildBone; ++i)
	{
		kChildIndexList[i] = -1;
	}
	SoMath_MatrixIdentity(&kMatFromBoneSpaceToModelSpace);
}
//----------------------------------------------------------------
StFBXBoneGroup::StFBXBoneGroup()
:kBoneArray(NULL)
,nBoneValidCount(0)
,nBoneMaxCount(0)
{

}
//----------------------------------------------------------------
StFBXBoneGroup::~StFBXBoneGroup()
{
	if (kBoneArray)
	{
		delete[] kBoneArray;
		kBoneArray = NULL;
	}
}
//----------------------------------------------------------------
void StFBXBoneGroup::ReserveBoneCount(int nCount)
{
	kBoneArray = new StFBXBone[nCount];
	nBoneValidCount = 0;
	nBoneMaxCount = nCount;
}
//----------------------------------------------------------------
void StFBXBoneGroup::GenerateChildren()
{
	for (int i = 0; i < nBoneValidCount; ++i)
	{
		StFBXBone* pBone = kBoneArray + i;
		int nChildCount = 0;
		//
		for (int j = 0; j < nBoneValidCount; ++j)
		{
			StFBXBone* pChild = kBoneArray + j;
			if (pChild->nParentIndex == i)
			{
				pBone->kChildIndexList[nChildCount] = j;
				++nChildCount;
				if (nChildCount >= StFBX_MaxCount_ChildBone)
				{
					//目前，子骨骼列表并没有发挥作用，所以这里不需要告警或报错。
					break;
				}
			}
		}
	}
}
//----------------------------------------------------------------
int StFBXBoneGroup::GetBoneIndexByBoneName(const char* szBoneName) const
{
	int nIndex = -1;
	if (szBoneName != 0 && szBoneName[0] != 0)
	{
		for (int i = 0; i < nBoneValidCount; ++i)
		{
			StFBXBone* pBone = kBoneArray + i;
			if (SoStrCmpNoCase(szBoneName, pBone->kBoneName.GetValue()) == 0)
			{
				nIndex = i;
				break;
			}
		}
	}
	return nIndex;
}
//----------------------------------------------------------------
StFBXBone* StFBXBoneGroup::TakeNew(int* pIndex)
{
	if (nBoneValidCount < nBoneMaxCount)
	{
		if (pIndex)
		{
			*pIndex = nBoneValidCount;
		}
		++nBoneValidCount;
		return kBoneArray + (nBoneValidCount - 1);
	}
	else
	{
		SoLogError("StFBXBoneGroup::TakeNew : kBoneArray is full");
		return NULL;
	}
}
//----------------------------------------------------------------
StFBXBone* StFBXBoneGroup::GetAt(int nIndex) const
{
	if (nIndex >= 0 && nIndex < nBoneValidCount)
	{
		return kBoneArray + nIndex;
	}
	else
	{
		return NULL;
	}
}
//----------------------------------------------------------------
int StFBXBoneGroup::GetSize() const
{
	return nBoneValidCount;
}
//----------------------------------------------------------------
StFBXKeyFrame::StFBXKeyFrame()
:fKeyTime(-1.0f)
,matKeyTransform(true)
{

}
//----------------------------------------------------------------
void StFBXKeyFrame::ResetInit()
{
	fKeyTime = -1.0f;
	SoMath_MatrixIdentity(&matKeyTransform);
}
//----------------------------------------------------------------
StFBXBoneAnimation::StFBXBoneAnimation()
:kKeyFrameArray(NULL)
,nFrameValidCount(0)
,nFrameMaxCount(0)
,nBoneIndex(-1)
{

}
//----------------------------------------------------------------
StFBXBoneAnimation::~StFBXBoneAnimation()
{
	if (kKeyFrameArray)
	{
		delete[] kKeyFrameArray;
		kKeyFrameArray = NULL;
	}
}
//----------------------------------------------------------------
void StFBXBoneAnimation::ResetInit()
{
	nBoneIndex = -1;
	nFrameValidCount = 0;
}
//----------------------------------------------------------------
void StFBXBoneAnimation::ReserveKeyFrameCount(int nCount)
{
	kKeyFrameArray = new StFBXKeyFrame[nCount];
	nFrameValidCount = 0;
	nFrameMaxCount = nCount;
}
//----------------------------------------------------------------
StFBXKeyFrame* StFBXBoneAnimation::TakeNew()
{
	if (nFrameValidCount < nFrameMaxCount)
	{
		++nFrameValidCount;
		return kKeyFrameArray + (nFrameValidCount - 1);
	}
	else
	{
		SoLogError("StFBXBoneAnimation::TakeNew : kKeyFrameArray is full");
		return NULL;
	}
}
//----------------------------------------------------------------
StFBXKeyFrame* StFBXBoneAnimation::GetAt(int nIndex) const
{
	if (nIndex >= 0 && nIndex < nFrameValidCount)
	{
		return kKeyFrameArray + nIndex;
	}
	else
	{
		return NULL;
	}
}
//----------------------------------------------------------------
int StFBXBoneAnimation::GetSize() const
{
	return nFrameValidCount;
}
//----------------------------------------------------------------
StFBXModelAnimation::StFBXModelAnimation()
:kBoneAnimationArray(NULL)
,nAnimValidCount(0)
,nAnimMaxCount(0)
,fAnimLength(0.0f)
{

}
//----------------------------------------------------------------
StFBXModelAnimation::~StFBXModelAnimation()
{
	if (kBoneAnimationArray)
	{
		delete[] kBoneAnimationArray;
		kBoneAnimationArray = NULL;
	}
}
//----------------------------------------------------------------
void StFBXModelAnimation::ReserveBoneCount(int nCount)
{
	kBoneAnimationArray = new StFBXBoneAnimation[nCount];
	nAnimValidCount = 0;
	nAnimMaxCount = nCount;
}
//----------------------------------------------------------------
StFBXBoneAnimation* StFBXModelAnimation::TakeNew()
{
	if (nAnimValidCount < nAnimMaxCount)
	{
		++nAnimValidCount;
		return kBoneAnimationArray + (nAnimValidCount - 1);
	}
	else
	{
		SoLogError("StFBXModelAnimation::TakeNew : kBoneAnimationArray is full");
		return NULL;
	}
}
//----------------------------------------------------------------
StFBXBoneAnimation* StFBXModelAnimation::GetAt(int nIndex) const
{
	if (nIndex >= 0 && nIndex < nAnimValidCount)
	{
		return kBoneAnimationArray + nIndex;
	}
	else
	{
		return NULL;
	}
}
//----------------------------------------------------------------
int StFBXModelAnimation::GetSize() const
{
	return nAnimValidCount;
}
//----------------------------------------------------------------
StFBXBoneAnimation* StFBXModelAnimation::GetBoneAnimation(int nBoneIndex) const
{
	StFBXBoneAnimation* pResultBoneAnim = 0;
	if (nBoneIndex >= 0 && nBoneIndex < nAnimValidCount)
	{
		pResultBoneAnim = kBoneAnimationArray + nBoneIndex;
		if (pResultBoneAnim->nBoneIndex == nBoneIndex)
		{
			return pResultBoneAnim;
		}
	}

	pResultBoneAnim = 0;
	for (int i = 0; i < nAnimValidCount; ++i)
	{
		StFBXBoneAnimation* pBoneAnim = kBoneAnimationArray + i;
		if (pBoneAnim->nBoneIndex == nBoneIndex)
		{
			pResultBoneAnim = pBoneAnim;
			break;
		}
	}
	return pResultBoneAnim;
}
//----------------------------------------------------------------
