//----------------------------------------------------------------
#include "StFBXDefine.h"
#include "SoStringHelp.h"
#include "SoSimpleLog.h"
//----------------------------------------------------------------
StFBXMeshData::StFBXMeshData()
:pVertexBuff(NULL)
,nVertexBuffSize(0)
,nVertexCount(0)
,pIndexBuff(NULL)
,nIndexBuffSize(0)
,nIndexCount(0)
,pVertexIndex2ControlPointIndex(NULL)
,nSizeofVertexData(0)
{

}
//----------------------------------------------------------------
StFBXMeshData::~StFBXMeshData()
{
	if (pVertexBuff)
	{
		free(pVertexBuff);
		pVertexBuff = NULL;
	}
	if (pIndexBuff)
	{
		free(pIndexBuff);
		pIndexBuff = NULL;
	}
	if (pVertexIndex2ControlPointIndex)
	{
		free(pVertexIndex2ControlPointIndex);
		pVertexIndex2ControlPointIndex = NULL;
	}
}
//----------------------------------------------------------------
void StFBXMeshData::SetVertexType(const SoBitFlag& kType)
{
	kVertexType.SetValue(kType.GetValue());
	//计算顶点结构的sizeof值。
	//顶点坐标。
	nSizeofVertexData = StFBX_Sizeof_Vector3;
	//法线。
	if (kVertexType.IsFlagExist(StFBXElement_Normal))
	{
		nSizeofVertexData += StFBX_Sizeof_Vector3;
	}
	//切线。
	if (kVertexType.IsFlagExist(StFBXElement_Tangent))
	{
		nSizeofVertexData += StFBX_Sizeof_Vector3;
	}
	//顶点色。
	if (kVertexType.IsFlagExist(StFBXElement_Color))
	{
		nSizeofVertexData += StFBX_Sizeof_Color;
	}
	//UV1。
	if (kVertexType.IsFlagExist(StFBXElement_UV1))
	{
		nSizeofVertexData += StFBX_Sizeof_UV;
	}
	//UV2。
	if (kVertexType.IsFlagExist(StFBXElement_UV2))
	{
		nSizeofVertexData += StFBX_Sizeof_UV;
	}
}
//----------------------------------------------------------------
void StFBXMeshData::ReserveVertexCount(const int nVertexTotalCount)
{
	if (pVertexBuff)
	{
		free(pVertexBuff);
		pVertexBuff = NULL;
	}
	if (pIndexBuff)
	{
		free(pIndexBuff);
		pIndexBuff = NULL;
	}
	if (pVertexIndex2ControlPointIndex)
	{
		free(pVertexIndex2ControlPointIndex);
		pVertexIndex2ControlPointIndex = NULL;
	}

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
	kVertex = SoMathFloat3_Zero;
	for (int i = 0; i < StFBX_MaxCount_BoneIndexSkinWeightPerControlPoint; ++i)
	{
		kPairList[i].kBoneName.Clear();
		kPairList[i].nBoneIndex = -1;
		kPairList[i].fSkinWeight = 0.0f;
	}
}
//----------------------------------------------------------------
void StFBXControlPoint::AddBoneNameSkinWeight(const char* szBoneName, float fSkinWeight)
{
	if (szBoneName == NULL || szBoneName[0] == 0)
	{
		SoLogError("StFBXControlPoint::AddBoneNameSkinWeight : BoneName is Invalid");
		return;
	}

	bool bSuccess = false;
	for (int i = 0; i < StFBX_MaxCount_BoneIndexSkinWeightPerControlPoint; ++i)
	{
		if (kPairList[i].kBoneName.GetLength() == 0)
		{
			kPairList[i].kBoneName = szBoneName;
			kPairList[i].fSkinWeight = fSkinWeight;
			bSuccess = true;
			break;
		}
	}
	if (bSuccess == false)
	{
		SoLogError("StFBXControlPoint::AddBoneNameSkinWeight : kPairList is full");
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
	if (kControlPointArray)
	{
		delete[] kControlPointArray;
		kControlPointArray = NULL;
	}

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
//----------------------------------------------------------------
void StFBXControlPointGroup::MakeBoneIndexByBoneName(const StFBXBoneGroup* pBoneGroup)
{
	for (int i = 0; i < nControlPointValidCount; ++i)
	{
		StFBXBoneIndexSkinWeight* pPairList = kControlPointArray[i].kPairList;
		for (int j = 0; j < StFBX_MaxCount_BoneIndexSkinWeightPerControlPoint; ++j)
		{
			if (pPairList[j].kBoneName.GetLength() > 0)
			{
				const char* szBoneName = pPairList[j].kBoneName.GetValue();
				int nBoneIndex = pBoneGroup->GetBoneIndexByBoneName(szBoneName);
				pPairList[j].nBoneIndex = nBoneIndex;
				//
				if (nBoneIndex == -1)
				{
					SoLogError("StFBXControlPointGroup::MakeBoneIndexByBoneName : Can not find bone [%s]", szBoneName);
				}
			}
		}
	}
}
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
	if (kBoneArray)
	{
		delete[] kBoneArray;
		kBoneArray = NULL;
	}

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
void StFBXBoneAnimation::ReserveKeyFrameCount(int nCount)
{
	if (kKeyFrameArray)
	{
		delete[] kKeyFrameArray;
		kKeyFrameArray = NULL;
	}

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
,nFrameCount(0)
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
void StFBXModelAnimation::ReserveBoneCount(int nBoneCount)
{
	if (kBoneAnimationArray)
	{
		delete[] kBoneAnimationArray;
		kBoneAnimationArray = NULL;
	}

	kBoneAnimationArray = new StFBXBoneAnimation[nBoneCount];
	nAnimValidCount = 0;
	nAnimMaxCount = nBoneCount;
}
//----------------------------------------------------------------
int StFBXModelAnimation::GetSize() const
{
	return nAnimValidCount;
}
//----------------------------------------------------------------
int StFBXModelAnimation::GetFrameCount() const
{
	return nFrameCount;
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
int StFBXModelAnimation::GetKeyFrameIndexByTime(float fTime)
{
	int nKeyFrameIndex = -1;
	//找出一个帧数大于0的BoneAnimation对象。
	StFBXBoneAnimation* pBoneAnim = NULL;
	for (int boneIndex = 0; boneIndex < nAnimValidCount; ++boneIndex)
	{
		if (kBoneAnimationArray[boneIndex].GetSize() > 0)
		{
			pBoneAnim = kBoneAnimationArray + boneIndex;
			break;
		}
	}
	//有了BoneAnimation对象，判断给定的时间落在了哪一帧上。
	if (pBoneAnim)
	{
		for (int i = 0; i < pBoneAnim->nFrameValidCount; ++i)
		{
			StFBXKeyFrame* pKeyFrame = pBoneAnim->GetAt(i);
			if (fTime >= pKeyFrame->fKeyTime - SoMath_float_zero_critical)
			{
				nKeyFrameIndex = i;
			}
			else
			{
				break;
			}
		}
	}
	return nKeyFrameIndex;
}
//----------------------------------------------------------------
