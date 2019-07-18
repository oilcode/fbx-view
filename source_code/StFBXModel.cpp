//----------------------------------------------------------------
#include "StFBXModel.h"
#include "SoD3DInclude.h"
//----------------------------------------------------------------
StFBXModel::StFBXModel()
{

}
//----------------------------------------------------------------
StFBXModel::~StFBXModel()
{

}
//----------------------------------------------------------------
const StFBXMeshData* StFBXModel::GetMeshData() const
{
	return &m_kMeshData;
}
//----------------------------------------------------------------
const StFBXControlPointGroup* StFBXModel::GetControlPointGroup() const
{
	return &m_kControlPointGroup;
}
//----------------------------------------------------------------
const StFBXBoneGroup* StFBXModel::GetBoneGroup() const
{
	return &m_kBoneGroup;
}
//----------------------------------------------------------------
const StFBXModelAnimation* StFBXModel::GetAnimation() const
{
	return &m_kAnimation;
}
//----------------------------------------------------------------
const StFBXMeshData* StFBXModel::GetAnimationMeshData(float fTime)
{
	float fYuShu = fTime / m_kAnimation.fAnimLength;
	fYuShu = fYuShu - floor(fYuShu);
	float curTime = m_kAnimation.fAnimLength * fYuShu;
	if (curTime < 0.0f)
	{
		curTime = 0.0f;
	}

	int nCurKeyFrameIndex = m_kAnimation.GetKeyFrameIndexByTime(curTime);
	if (nCurKeyFrameIndex != -1)
	{
		CalculateMeshDataByKeyFrame(nCurKeyFrameIndex);
	}

	return &m_kMeshData;
}
//----------------------------------------------------------------
int StFBXModel::GetKeyFrameCount() const
{
	return m_kAnimation.nFrameCount;
}
//----------------------------------------------------------------
float StFBXModel::GetAnimTimeLength()
{
	return m_kAnimation.fAnimLength;
}
//----------------------------------------------------------------
void StFBXModel::CalculateMeshBoundingBox(SoMathFloat3* pMinPos, SoMathFloat3* pMaxPos)
{
	float fMinX = SoMath_FloatMax;
	float fMinY = SoMath_FloatMax;
	float fMinZ = SoMath_FloatMax;
	float fMaxX = SoMath_FloatMin;
	float fMaxY = SoMath_FloatMin;
	float fMaxZ = SoMath_FloatMin;

	const int nCPCount = m_kControlPointGroup.GetSize();
	for (int cpIndex = 0; cpIndex < nCPCount; ++cpIndex)
	{
		StFBXControlPoint* pCP = m_kControlPointGroup.GetAt(cpIndex);
		if (pCP == 0)
		{
			continue;
		}

		if (pCP->kVertex.x < fMinX)
		{
			fMinX = pCP->kVertex.x;
		}
		if (pCP->kVertex.x > fMaxX)
		{
			fMaxX = pCP->kVertex.x;
		}

		if (pCP->kVertex.y < fMinY)
		{
			fMinY = pCP->kVertex.y;
		}
		if (pCP->kVertex.y > fMaxY)
		{
			fMaxY = pCP->kVertex.y;
		}

		if (pCP->kVertex.z < fMinZ)
		{
			fMinZ = pCP->kVertex.z;
		}
		if (pCP->kVertex.z > fMaxZ)
		{
			fMaxZ = pCP->kVertex.z;
		}
	}

	pMinPos->x = fMinX;
	pMinPos->y = fMinY;
	pMinPos->z = fMinZ;
	pMaxPos->x = fMaxX;
	pMaxPos->y = fMaxY;
	pMaxPos->z = fMaxZ;
}
//----------------------------------------------------------------
StFBXControlPointGroup* StFBXModel::GetControlPointGroup_Modify()
{
	return &m_kControlPointGroup;
}
//----------------------------------------------------------------
StFBXBoneGroup* StFBXModel::GetBoneGroup_Modify()
{
	return &m_kBoneGroup;
}
//----------------------------------------------------------------
StFBXModelAnimation* StFBXModel::GetAnimation_Modify()
{
	return &m_kAnimation;
}
//----------------------------------------------------------------
StFBXMeshData* StFBXModel::GetMeshData_Modify()
{
	return &m_kMeshData;
}
//----------------------------------------------------------------
void StFBXModel::CalculateMeshDataByKeyFrame(const int nKeyFrameIndex)
{
	const XMFLOAT3 kVectorZero(0.0f, 0.0f, 0.0f);
	XMFLOAT3 kFinalCPVertex;
	XMVECTOR xmvecFinalCPVertex;

	const int nCPCount = m_kControlPointGroup.GetSize();
	for (int cpIndex = 0; cpIndex < nCPCount; ++cpIndex)
	{
		StFBXControlPoint* pCP = m_kControlPointGroup.GetAt(cpIndex);
		if (pCP == NULL)
		{
			continue;
		}
		if (pCP->kPairList[0].nBoneIndex == -1)
		{
			//本控制点不受任何骨骼的影响
			continue;
		}

		const XMFLOAT3* pCPVertex = (XMFLOAT3*)(&(pCP->kVertex));
		const XMVECTOR xmvecCPVertex = XMLoadFloat3(pCPVertex);
		xmvecFinalCPVertex = XMLoadFloat3(&kVectorZero);

		for (int pairIndex = 0; pairIndex < StFBX_MaxCount_BoneIndexSkinWeightPerControlPoint; ++pairIndex)
		{
			const int nBoneIndex = pCP->kPairList[pairIndex].nBoneIndex;
			if (nBoneIndex == -1)
			{
				break;
			}
			StFBXBone* pBone = m_kBoneGroup.GetAt(nBoneIndex);
			if (pBone == NULL)
			{
				continue;
			}
			StFBXBoneAnimation* pBoneAnim = m_kAnimation.GetBoneAnimation(nBoneIndex);
			if (pBoneAnim == NULL)
			{
				continue;
			}
			StFBXKeyFrame* pKeyFrame = pBoneAnim->GetAt(nKeyFrameIndex);
			if (pKeyFrame == NULL)
			{
				continue;
			}

			XMFLOAT4X4* pMatSpace = (XMFLOAT4X4*)(&(pBone->kMatFromBoneSpaceToModelSpace));
			XMMATRIX xmmatSpace = XMLoadFloat4x4(pMatSpace);

			XMFLOAT4X4* pMatKeyFrame = (XMFLOAT4X4*)(&(pKeyFrame->matKeyTransform));
			XMMATRIX xmmatKeyFrame = XMLoadFloat4x4(pMatKeyFrame);

			XMVECTOR result = XMVector3TransformCoord(xmvecCPVertex, xmmatSpace);
			result = XMVector3TransformCoord(result, xmmatKeyFrame);
			result *= pCP->kPairList[pairIndex].fSkinWeight;
			xmvecFinalCPVertex += result;
		}

		XMStoreFloat3(&kFinalCPVertex, xmvecFinalCPVertex);

		//遍历顶点列表，如果顶点所属的ControlPoint与cpIndex相同，则更新该顶点。
		//遍历过程中不能执行break操作。
		const int nMeshVertexCount = m_kMeshData.nVertexCount;
		const int* pVertexIndex2ControlPointIndex = m_kMeshData.pVertexIndex2ControlPointIndex;
		for (int vertexIndex = 0; vertexIndex < nMeshVertexCount; ++vertexIndex)
		{
			if (pVertexIndex2ControlPointIndex[vertexIndex] == cpIndex)
			{
				float* pVertexData = (float*)(m_kMeshData.pVertexBuff + m_kMeshData.nSizeofVertexData * vertexIndex);
				pVertexData[0] = kFinalCPVertex.x;
				pVertexData[1] = kFinalCPVertex.y;
				pVertexData[2] = kFinalCPVertex.z;
			}
		}
	}
}
//----------------------------------------------------------------
