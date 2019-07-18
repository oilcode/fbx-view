//--------------------------------------------------------------------
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif //_CRT_SECURE_NO_WARNINGS
//----------------------------------------------------------------
#include "StKKFileKkbWrite.h"
#include "StFBXModel.h"
#include "SoSimpleLog.h"
#include <cstdio>
#include <string>
//----------------------------------------------------------------
StKKFileKkbWrite::StKKFileKkbWrite()
:m_pVertexStructBuff(0)
,m_pPosBuff(0)
,m_pNormalBuff(0)
,m_pUVBuff(0)
,m_pFbxModel(0)
{
	memset(&m_kFileHead, 0, sizeof(m_kFileHead));
}
//----------------------------------------------------------------
StKKFileKkbWrite::~StKKFileKkbWrite()
{
	ClearFileKkb();
}
//----------------------------------------------------------------
bool StKKFileKkbWrite::WriteKkb(const char* szFileName, StFBXModel* pModel)
{
	if (szFileName == 0 || szFileName[0] == 0)
	{
		return false;
	}
	if (pModel == 0)
	{
		return false;
	}

	SoLogDebug("StKKFileKkbWrite::WriteKkb : Start; FileName[%s]", szFileName);

	bool br = false;
	do 
	{
		m_pFbxModel = pModel;

		if (Write_CheckVertexType() == false)
		{
			break;
		}
		if (Write_FillFileHead() == false)
		{
			break;
		}
		if (Write_GenerateVertexStructBuff() == false)
		{
			break;
		}
		if (Write_GeneratePosBuff() == false)
		{
			break;
		}
		if (Write_GenerateNormalBuff() == false)
		{
			break;
		}
		if (Write_GenerateUVBuff() == false)
		{
			break;
		}
		if (Write_GenerateBoneWeight() == false)
		{
			break;
		}
		if (Write_WriteFile(szFileName) == false)
		{
			break;
		}

		br = true;

	} while (0);

	ClearFileKkb();
	SoLogDebug("StKKFileKkbWrite::WriteKkb : Finish; Success[%d]", br?1:0);
	return br;
}
//----------------------------------------------------------------
void StKKFileKkbWrite::ClearFileKkb()
{
	memset(&m_kFileHead, 0, sizeof(m_kFileHead));
	if (m_pVertexStructBuff)
	{
		free(m_pVertexStructBuff);
		m_pVertexStructBuff = 0;
	}
	if (m_pPosBuff)
	{
		free(m_pPosBuff);
		m_pPosBuff = 0;
	}
	if (m_pNormalBuff)
	{
		free(m_pNormalBuff);
		m_pNormalBuff = 0;
	}
	if (m_pUVBuff)
	{
		free(m_pUVBuff);
		m_pUVBuff = 0;
	}
	m_pFbxModel = 0;
}
//----------------------------------------------------------------
bool StKKFileKkbWrite::Write_CheckVertexType()
{
	StKKVertexType Result = StKKVertexType_Invalid;

	const StFBXMeshData* pMeshData = m_pFbxModel->GetMeshData();
	if (pMeshData->kVertexType.IsFlagExist(StFBXElement_Position)
		&& pMeshData->kVertexType.IsFlagExist(StFBXElement_Normal)
		&& pMeshData->kVertexType.IsFlagExist(StFBXElement_UV1))
	{
		if (m_pFbxModel->GetBoneGroup()->GetSize() == 0)
		{
			Result = StKKVertexType_Pos_Normal_UV;
		}
		else
		{
			Result = StKKVertexType_Pos_Normal_UV_Bone;
		}
	}

	if (Result != StKKVertexType_Invalid)
	{
		m_kFileHead.VertexType = Result;
		return true;
	}
	else
	{
		SoLogError("StKKFileKkbWrite::Write_CheckVertexType : VertexType is invalid");
		return false;
	}
}
//----------------------------------------------------------------
bool StKKFileKkbWrite::Write_FillFileHead()
{
	for (int i = 0; i < StKkbFileFlagSize; ++i)
	{
		m_kFileHead.FileFlag[i] = StKkbFileFlagString[i];
	}

	switch (m_kFileHead.VertexType)
	{
	case StKKVertexType_Pos_Normal_UV:
		{
			m_kFileHead.VertexSize = sizeof(StKKVertexStruct_Pos_Normal_UV);
		}
		break;
	case StKKVertexType_Pos_Normal_UV_Bone:
		{
			m_kFileHead.VertexSize = sizeof(StKKVertexStruct_Pos_Normal_UV_Bone);
		}
		break;
	default:
		{
			SoLogError("StKKFileKkbWrite::Write_FillFileHead : VertexType is invalid");
		}
		break;
	}
	
	const StFBXMeshData* pMeshData = m_pFbxModel->GetMeshData();
	m_kFileHead.VertexCount = pMeshData->nVertexCount;
	m_kFileHead.TriangleIndex_123 = StFBX_TriangleIndex_123;
	return true;
}
//----------------------------------------------------------------
bool StKKFileKkbWrite::Write_GenerateVertexStructBuff()
{
	const int nVertexStructBuffSize = m_kFileHead.VertexSize * m_kFileHead.VertexCount;
	m_pVertexStructBuff = (char*)malloc(nVertexStructBuffSize);
	if (m_pVertexStructBuff)
	{
		return true;
	}
	else
	{
		SoLogError("StKKFileKkbWrite::Write_GenerateVertexStructBuff : malloc fail! VertexStructBuffSize[%d]", nVertexStructBuffSize);
		return false;
	}
}
//----------------------------------------------------------------
bool StKKFileKkbWrite::Write_GeneratePosBuff()
{
	const StFBXMeshData* pMeshData = m_pFbxModel->GetMeshData();
	const float* pMeshVertexStruct = (const float*)(pMeshData->pVertexBuff);
	const int nFloatCountPerMeshVertexStruct = pMeshData->nSizeofVertexData / sizeof(float);

	const int nPosIndexCount = pMeshData->nVertexCount;
	int* pPosIndexFlagBuff = (int*)malloc(sizeof(int) * nPosIndexCount);
	for (int i = 0; i < nPosIndexCount; ++i)
	{
		pPosIndexFlagBuff[i] = -1;
	}

	int nPosIndexX = 0;
	int nPosIndexY = 0;
	int nPosIndexZ = 0;
	int nOffset = 0;
	switch (m_kFileHead.VertexType)
	{
	case StKKVertexType_Pos_Normal_UV:
	case StKKVertexType_Pos_Normal_UV_Bone:
		{
			nPosIndexX = 0;
			nPosIndexY = 1;
			nPosIndexZ = 2;
			nOffset = 0;
		}
		break;
	default:
		SoLogError("StKKFileKkbWrite::Write_GeneratePosBuff : VertexType is invalid");
		break;
	}

	//记录坐标值的个数，这些坐标值是各不相同的值。
	int nPosCount = 0;
	//记录有多少个坐标值是重复的。
	int nPosSameCount = 0;

	for (int i = 0; i < nPosIndexCount; ++i)
	{
		if (pPosIndexFlagBuff[i] != -1)
		{
			continue;
		}

		pPosIndexFlagBuff[i] = i;
		++nPosCount;

		const float* pStruct = pMeshVertexStruct + nFloatCountPerMeshVertexStruct * i;
		float fPosX = pStruct[nPosIndexX];
		float fPosY = pStruct[nPosIndexY];
		float fPosZ = pStruct[nPosIndexZ];

		for (int j = i+1; j < nPosIndexCount; ++j)
		{
			if (pPosIndexFlagBuff[j] != -1)
			{
				continue;
			}

			const float* pStruct_B = pMeshVertexStruct + nFloatCountPerMeshVertexStruct * j;
			float fPosX_B = pStruct_B[nPosIndexX];
			float fPosY_B = pStruct_B[nPosIndexY];
			float fPosZ_B = pStruct_B[nPosIndexZ];

			if (SoMath_IsFloatZero(fPosX - fPosX_B)
				&& SoMath_IsFloatZero(fPosY - fPosY_B)
				&& SoMath_IsFloatZero(fPosZ - fPosZ_B))
			{
				pPosIndexFlagBuff[j] = i;
				++nPosSameCount;
			}
		}
	}


	m_pPosBuff = (char*)malloc(StFBX_Sizeof_Vector3 * nPosCount);
	m_kFileHead.PosCount = nPosCount;
	SoMathFloat3* pPosFloat3 = (SoMathFloat3*)m_pPosBuff;
	int nAccPosCount = 0;
	for (int i = 0; i < nPosIndexCount; ++i)
	{
		if (pPosIndexFlagBuff[i] == i)
		{
			const float* pStruct = pMeshVertexStruct + nFloatCountPerMeshVertexStruct * i;
			pPosFloat3[nAccPosCount].x = pStruct[nPosIndexX];
			pPosFloat3[nAccPosCount].y = pStruct[nPosIndexY];
			pPosFloat3[nAccPosCount].z = pStruct[nPosIndexZ];
			++nAccPosCount;
			//
			pPosIndexFlagBuff[i] = -nAccPosCount;
		}
	}


	for (int i = 0; i < nPosIndexCount; ++i)
	{
		int* pPosIndex = (int*)(m_pVertexStructBuff + m_kFileHead.VertexSize * i + nOffset);
		if (pPosIndexFlagBuff[i] < 0)
		{
			*pPosIndex = -pPosIndexFlagBuff[i] - 1;
		}
		else
		{
			int nValue = pPosIndexFlagBuff[pPosIndexFlagBuff[i]];
			*pPosIndex = -nValue - 1;
		}
	}

	free(pPosIndexFlagBuff);
	return true;
}
//----------------------------------------------------------------
bool StKKFileKkbWrite::Write_GenerateNormalBuff()
{
	const StFBXMeshData* pMeshData = m_pFbxModel->GetMeshData();
	const float* pMeshVertexStruct = (const float*)(pMeshData->pVertexBuff);
	const int nFloatCountPerMeshVertexStruct = pMeshData->nSizeofVertexData / sizeof(float);

	const int nNormalIndexCount = pMeshData->nVertexCount;
	int* pNormalIndexFlagBuff = (int*)malloc(sizeof(int) * nNormalIndexCount);
	for (int i = 0; i < nNormalIndexCount; ++i)
	{
		pNormalIndexFlagBuff[i] = -1;
	}

	int nNormalIndexX = 0;
	int nNormalIndexY = 0;
	int nNormalIndexZ = 0;
	int nOffset = 0;
	switch (m_kFileHead.VertexType)
	{
	case StKKVertexType_Pos_Normal_UV:
	case StKKVertexType_Pos_Normal_UV_Bone:
		{
			nNormalIndexX = 3;
			nNormalIndexY = 4;
			nNormalIndexZ = 5;
			nOffset = sizeof(int);
		}
		break;
	default:
		SoLogError("StKKFileKkbWrite::Write_GenerateNormalBuff : VertexType is invalid");
		break;
	}

	//记录法线值的个数，这些法线值是各不相同的值。
	int nNormalCount = 0;
	//记录有多少个法线值是重复的。
	int nNormalSameCount = 0;

	for (int i = 0; i < nNormalIndexCount; ++i)
	{
		if (pNormalIndexFlagBuff[i] != -1)
		{
			continue;
		}

		pNormalIndexFlagBuff[i] = i;
		++nNormalCount;

		const float* pStruct = pMeshVertexStruct + nFloatCountPerMeshVertexStruct * i;
		float fNormalX = pStruct[nNormalIndexX];
		float fNormalY = pStruct[nNormalIndexY];
		float fNormalZ = pStruct[nNormalIndexZ];

		for (int j = i+1; j < nNormalIndexCount; ++j)
		{
			if (pNormalIndexFlagBuff[j] != -1)
			{
				continue;
			}

			const float* pStruct_B = pMeshVertexStruct + nFloatCountPerMeshVertexStruct * j;
			float fNormalX_B = pStruct_B[nNormalIndexX];
			float fNormalY_B = pStruct_B[nNormalIndexY];
			float fNormalZ_B = pStruct_B[nNormalIndexZ];

			if (SoMath_IsFloatZero(fNormalX - fNormalX_B)
				&& SoMath_IsFloatZero(fNormalY - fNormalY_B)
				&& SoMath_IsFloatZero(fNormalZ - fNormalZ_B))
			{
				pNormalIndexFlagBuff[j] = i;
				++nNormalSameCount;
			}
		}
	}


	m_pNormalBuff = (char*)malloc(StFBX_Sizeof_Vector3 * nNormalCount);
	m_kFileHead.NormalCount = nNormalCount;
	SoMathFloat3* pNormalFloat3 = (SoMathFloat3*)m_pNormalBuff;
	int nAccNormalCount = 0;
	for (int i = 0; i < nNormalIndexCount; ++i)
	{
		if (pNormalIndexFlagBuff[i] == i)
		{
			const float* pStruct = pMeshVertexStruct + nFloatCountPerMeshVertexStruct * i;
			pNormalFloat3[nAccNormalCount].x = pStruct[nNormalIndexX];
			pNormalFloat3[nAccNormalCount].y = pStruct[nNormalIndexY];
			pNormalFloat3[nAccNormalCount].z = pStruct[nNormalIndexZ];
			++nAccNormalCount;
			//
			pNormalIndexFlagBuff[i] = -nAccNormalCount;
		}
	}


	for (int i = 0; i < nNormalIndexCount; ++i)
	{
		int* pNormalIndex = (int*)(m_pVertexStructBuff + m_kFileHead.VertexSize * i + nOffset);
		if (pNormalIndexFlagBuff[i] < 0)
		{
			*pNormalIndex = -pNormalIndexFlagBuff[i] - 1;
		}
		else
		{
			int nValue = pNormalIndexFlagBuff[pNormalIndexFlagBuff[i]];
			*pNormalIndex = -nValue - 1;
		}
	}

	free(pNormalIndexFlagBuff);
	return true;
}
//----------------------------------------------------------------
bool StKKFileKkbWrite::Write_GenerateUVBuff()
{
	const StFBXMeshData* pMeshData = m_pFbxModel->GetMeshData();
	const float* pMeshVertexStruct = (const float*)(pMeshData->pVertexBuff);
	const int nFloatCountPerMeshVertexStruct = pMeshData->nSizeofVertexData / sizeof(float);

	const int nUVIndexCount = pMeshData->nVertexCount;
	int* pUVIndexFlagBuff = (int*)malloc(sizeof(int) * nUVIndexCount);
	for (int i = 0; i < nUVIndexCount; ++i)
	{
		pUVIndexFlagBuff[i] = -1;
	}

	int nUVIndexX = 0;
	int nUVIndexY = 0;
	int nOffset = 0;
	switch (m_kFileHead.VertexType)
	{
	case StKKVertexType_Pos_Normal_UV:
	case StKKVertexType_Pos_Normal_UV_Bone:
		{
			nUVIndexX = 6;
			nUVIndexY = 7;
			nOffset = sizeof(int) + sizeof(int);
		}
		break;
	default:
		SoLogError("StKKFileKkbWrite::Write_GenerateUVBuff : VertexType is invalid");
		break;
	}

	//记录UV值的个数，这些UV值是各不相同的值。
	int nUVCount = 0;
	//记录有多少个UV值是重复的。
	int nUVSameCount = 0;

	for (int i = 0; i < nUVIndexCount; ++i)
	{
		if (pUVIndexFlagBuff[i] != -1)
		{
			continue;
		}

		pUVIndexFlagBuff[i] = i;
		++nUVCount;

		const float* pStruct = pMeshVertexStruct + nFloatCountPerMeshVertexStruct * i;
		float fUVX = pStruct[nUVIndexX];
		float fUVY = pStruct[nUVIndexY];

		for (int j = i+1; j < nUVIndexCount; ++j)
		{
			if (pUVIndexFlagBuff[j] != -1)
			{
				continue;
			}

			const float* pStruct_B = pMeshVertexStruct + nFloatCountPerMeshVertexStruct * j;
			float fUVX_B = pStruct_B[nUVIndexX];
			float fUVY_B = pStruct_B[nUVIndexY];

			if (SoMath_IsFloatZero(fUVX - fUVX_B)
				&& SoMath_IsFloatZero(fUVY - fUVY_B))
			{
				pUVIndexFlagBuff[j] = i;
				++nUVSameCount;
			}
		}
	}


	m_pUVBuff = (char*)malloc(StFBX_Sizeof_UV * nUVCount);
	m_kFileHead.UVCount = nUVCount;
	SoMathFloat2* pUVFloat2 = (SoMathFloat2*)m_pUVBuff;
	int nAccUVCount = 0;
	for (int i = 0; i < nUVIndexCount; ++i)
	{
		if (pUVIndexFlagBuff[i] == i)
		{
			const float* pStruct = pMeshVertexStruct + nFloatCountPerMeshVertexStruct * i;
			pUVFloat2[nAccUVCount].x = pStruct[nUVIndexX];
			pUVFloat2[nAccUVCount].y = pStruct[nUVIndexY];
			++nAccUVCount;
			//
			pUVIndexFlagBuff[i] = -nAccUVCount;
		}
	}


	for (int i = 0; i < nUVIndexCount; ++i)
	{
		int* pUVIndex = (int*)(m_pVertexStructBuff + m_kFileHead.VertexSize * i + nOffset);
		if (pUVIndexFlagBuff[i] < 0)
		{
			*pUVIndex = -pUVIndexFlagBuff[i] - 1;
		}
		else
		{
			int nValue = pUVIndexFlagBuff[pUVIndexFlagBuff[i]];
			*pUVIndex = -nValue - 1;
		}
	}

	free(pUVIndexFlagBuff);
	return true;
}
//----------------------------------------------------------------
bool StKKFileKkbWrite::Write_GenerateBoneWeight()
{
	const StFBXMeshData* pMeshData = m_pFbxModel->GetMeshData();
	const int* pVertex2CP = pMeshData->pVertexIndex2ControlPointIndex;
	const StFBXControlPointGroup* pControlPointGroup = m_pFbxModel->GetControlPointGroup();

	int nOffset = 0;
	switch (m_kFileHead.VertexType)
	{
	case StKKVertexType_Pos_Normal_UV:
		{
			//没有骨骼数据。
			return true;
		}
		break;
	case StKKVertexType_Pos_Normal_UV_Bone:
		{
			nOffset = sizeof(int) + sizeof(int) + sizeof(int);
		}
		break;
	default:
		SoLogError("StKKFileKkbWrite::Write_GenerateBoneWeight : VertexType is invalid");
		break;
	}

	const int nCount = pMeshData->nVertexCount;
	for (int i = 0; i < nCount; ++i)
	{
		const int nCPIndex = pVertex2CP[i];
		StFBXControlPoint* pCP = pControlPointGroup->GetAt(nCPIndex);
		if (pCP)
		{
			int* pBoneIndex = (int*)(m_pVertexStructBuff + m_kFileHead.VertexSize * i + nOffset);
			float* pBoneWeight = (float*)(m_pVertexStructBuff + m_kFileHead.VertexSize * i + nOffset + sizeof(int) * StKK_MaxBonePerVertex);
			for (int j = 0; j < StKK_MaxBonePerVertex; ++j)
			{
				if (j < StFBX_MaxCount_BoneIndexSkinWeightPerControlPoint)
				{
					pBoneIndex[j] = pCP->kPairList[j].nBoneIndex;
					pBoneWeight[j] = pCP->kPairList[j].fSkinWeight;
				}
				else
				{
					pBoneIndex[j] = -1;
					pBoneWeight[j] = 0.0f;
				}
			}
		}
	}

	return true;
}
//----------------------------------------------------------------
bool StKKFileKkbWrite::Write_WriteFile(const char* szFileName)
{
	FILE* fp = fopen(szFileName, "wb+");
	if (fp == 0)
	{
		return false;
	}

	fwrite(&m_kFileHead, 1, sizeof(m_kFileHead), fp);
	fwrite(m_pVertexStructBuff, 1, m_kFileHead.VertexSize * m_kFileHead.VertexCount, fp);
	fwrite(m_pPosBuff, 1, StFBX_Sizeof_Vector3 * m_kFileHead.PosCount, fp);
	fwrite(m_pNormalBuff, 1, StFBX_Sizeof_Vector3 * m_kFileHead.NormalCount, fp);
	fwrite(m_pUVBuff, 1, StFBX_Sizeof_UV * m_kFileHead.UVCount, fp);

	fclose(fp);
	return true;
}
//----------------------------------------------------------------
