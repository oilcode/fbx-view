//----------------------------------------------------------------
#include "StKKModel.h"
#include "StKKFileKkbRead.h"
#include "StKKFileKkfRead.h"
//----------------------------------------------------------------
StKKModel::StKKModel()
:m_pModelData(0)
,m_pAnimList(0)
,m_nAnimCount(0)
{

}
//----------------------------------------------------------------
StKKModel::~StKKModel()
{
	ClearKKModel();
}
//----------------------------------------------------------------
bool StKKModel::LoadKkbFile(const char* szFileName)
{
	m_pModelData = new StKKModelData;
	if (StKKFileKkbRead::ReadKkb(szFileName, m_pModelData) == false)
	{
		return false;
	}

	return true;
}
//----------------------------------------------------------------
bool StKKModel::LoadKkfFile(const char* szFileName)
{
	StKKModelAnimation* pAnim = new StKKModelAnimation;
	if (StKKFileKkfRead::ReadKkf(szFileName, pAnim) == false)
	{
		delete pAnim;
		return false;
	}

	pAnim->nAnimID = 1;
	ReserveAnimCount(3);
	AddAnimation(pAnim);
	return true;
}
//----------------------------------------------------------------
void StKKModel::ClearKKModel()
{
	if (m_pModelData)
	{
		delete m_pModelData;
		m_pModelData = 0;
	}
	ClearAllAnimation();
}
//----------------------------------------------------------------
void StKKModel::SetModelData(StKKModelData* pModelData)
{
	m_pModelData = pModelData;
}
//----------------------------------------------------------------
void StKKModel::ReserveAnimCount(int nAnimCount)
{
	if (m_nAnimCount >= nAnimCount)
	{
		return;
	}

	m_pAnimList = new StKKModelAnimation*[nAnimCount];
	memset(m_pAnimList, 0, sizeof(void*)*nAnimCount);
	m_nAnimCount = nAnimCount;
}
//----------------------------------------------------------------
void StKKModel::AddAnimation(StKKModelAnimation* pAnim)
{
	for (int i = 0; i < m_nAnimCount; ++i)
	{
		if (m_pAnimList[i] == 0)
		{
			m_pAnimList[i] = pAnim;
			break;
		}
	}
}
//----------------------------------------------------------------
void StKKModel::ClearAllAnimation()
{
	for (int i = 0; i < m_nAnimCount; ++i)
	{
		if (m_pAnimList[i])
		{
			delete m_pAnimList[i];
		}
	}
	if (m_pAnimList)
	{
		delete[] m_pAnimList;
		m_pAnimList = 0;
	}
	m_nAnimCount = 0;
}
//----------------------------------------------------------------
const StKKModelData* StKKModel::GetModelData() const
{
	return m_pModelData;
}
//----------------------------------------------------------------
bool StKKModel::IsBoneDataExist() const
{
	bool br = false;
	if (m_pModelData)
	{
		if (m_pModelData->nVertexType == StKKVertexType_Pos_Normal_UV_Bone)
		{
			br = true;
		}
	}
	return br;
}
//----------------------------------------------------------------
const StKKModelAnimation* StKKModel::GetAnimByID(int nAnimID) const
{
	StKKModelAnimation* pAnim = 0;
	for (int i = 0; i < m_nAnimCount; ++i)
	{
		if (m_pAnimList[i])
		{
			if (m_pAnimList[i]->nAnimID == nAnimID)
			{
				pAnim = m_pAnimList[i];
			}
		}
	}
	return pAnim;
}
//----------------------------------------------------------------
int StKKModel::GetKeyFrameIndexByID(int nAnimID, float fTime) const
{
	const StKKModelAnimation* pAnim = GetAnimByID(nAnimID);
	if (pAnim == 0)
	{
		return -1;
	}
	//
	int nTime = (int)(fTime * 1000.0f);
	int curTime = nTime % pAnim->nTimeLength;
	int nKeyFrameInterval = pAnim->nTimeLength / pAnim->nKeyFrameCount;
	int nIndex = curTime / nKeyFrameInterval;
	if (nIndex < 0)
	{
		nIndex = 0;
	}
	else if (nIndex >= pAnim->nKeyFrameCount)
	{
		nIndex = pAnim->nKeyFrameCount - 1;
	}
	return nIndex;
}
//----------------------------------------------------------------
void StKKModel::GetMeshBoundingBox(SoMathFloat3* pMinPos, SoMathFloat3* pMaxPos) const
{
	if (m_pModelData)
	{
		*pMinPos = m_pModelData->kPosMinValue;
		*pMaxPos = m_pModelData->kPosMaxValue;
	}
}
//----------------------------------------------------------------
