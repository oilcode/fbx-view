//----------------------------------------------------------------
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif //_CRT_SECURE_NO_WARNINGS
//----------------------------------------------------------------
#include "StKKFileKkfWrite.h"
#include "StFBXModel.h"
#include "SoSimpleLog.h"
//----------------------------------------------------------------
StKKFileKkfWrite::StKKFileKkfWrite()
:m_pBoneTransformBuff(0)
,m_pFbxModel(0)
{
	memset(&m_kFileHead, 0, sizeof(StKkfFileHead));
}
//----------------------------------------------------------------
StKKFileKkfWrite::~StKKFileKkfWrite()
{
	ClearFileKkf();
}
//----------------------------------------------------------------
bool StKKFileKkfWrite::WriteKkf(const char* szFileName, StFBXModel* pModel)
{
	if (szFileName == 0 || szFileName[0] == 0)
	{
		return false;
	}
	if (pModel == 0)
	{
		return false;
	}

	SoLogDebug("StKKFileKkfWrite::WriteKkf : Start; FileName[%s]", szFileName);

	bool br = false;
	do 
	{
		m_pFbxModel = pModel;

		if (Write_FillFileHead() == false)
		{
			break;
		}

		if (Write_GenerateBoneTransformBuff() == false)
		{
			break;
		}

		if (Write_WriteFile(szFileName) == false)
		{
			break;
		}

		br = true;

	} while (0);

	ClearFileKkf();
	SoLogDebug("StKKFileKkfWrite::WriteKkf : Finish; Success[%d]", br?1:0);
	return br;
}
//----------------------------------------------------------------
void StKKFileKkfWrite::ClearFileKkf()
{
	memset(&m_kFileHead, 0, sizeof(StKkfFileHead));
	if (m_pBoneTransformBuff)
	{
		free(m_pBoneTransformBuff);
		m_pBoneTransformBuff = 0;
	}
	m_pFbxModel = 0;
}
//----------------------------------------------------------------
bool StKKFileKkfWrite::Write_FillFileHead()
{
	for (int i = 0; i < StKkfFileFlagSize; ++i)
	{
		m_kFileHead.FileFlag[i] = StKkfFileFlagString[i];
	}

	const StFBXModelAnimation* pAnim = m_pFbxModel->GetAnimation();
	const StFBXBoneAnimation* pBoneAnim = pAnim->GetAt(0);
	const StFBXBoneGroup* pBoneGroup = m_pFbxModel->GetBoneGroup();
	if (pBoneAnim == 0)
	{
		SoLogError("StKKFileKkfWrite::Write_FillFileHead : Can not find BoneAnim!");
		return false;
	}

	m_kFileHead.TimeLength = (int)(pAnim->fAnimLength * 1000.0f);
	m_kFileHead.KeyFrameCount = pBoneAnim->GetSize();
	//注意，下面这个值必须是骨骼总个数（pBoneGroup->kBoneArray.GetSize()的值），
	//而不是本动画中发挥作用的骨骼的个数（pAnim->kBoneAnimationArray.GetSize()的值）。
	//因为顶点结构体中顶点所引用的骨骼序号是相对于骨骼总个数的序号。
	m_kFileHead.BoneCount = pBoneGroup->GetSize();
	return true;
}
//----------------------------------------------------------------
bool StKKFileKkfWrite::Write_GenerateBoneTransformBuff()
{
	const int BuffSize = m_kFileHead.KeyFrameCount * m_kFileHead.BoneCount * sizeof(StKKBoneTransform);
	m_pBoneTransformBuff = (char*)malloc(BuffSize);
	if (m_pBoneTransformBuff == 0)
	{
		SoLogError("StKKFileKkfWrite::Write_GenerateBoneTransformBuff : malloc fail! size[%d]", BuffSize);
		return false;
	}

	const StFBXBoneGroup* pBoneGroup = m_pFbxModel->GetBoneGroup();
	const StFBXModelAnimation* pModelAnim = m_pFbxModel->GetAnimation();
	StKKBoneTransform* pBoneTransform = (StKKBoneTransform*)m_pBoneTransformBuff;
	int nAccCount = 0;

	for (int i = 0; i < m_kFileHead.KeyFrameCount; ++i)
	{
		for (int j = 0; j < m_kFileHead.BoneCount; ++j)
		{
			const StFBXBone* pBone = pBoneGroup->GetAt(j);
			const StFBXBoneAnimation* pBoneAnim = pModelAnim->GetBoneAnimation(j);
			SoMathMatrix3* pRotate = &(pBoneTransform[nAccCount].Rotate);
			SoMathFloat3* pTranslate = &(pBoneTransform[nAccCount].Translate);
			//float* pInvTrans = pBoneTransform[nAccCount].InverseTranspose;
			++nAccCount;

			if (pBoneAnim)
			{
				XMFLOAT4X4* pMatSpace = (XMFLOAT4X4*)(&(pBone->kMatFromBoneSpaceToModelSpace));
				XMMATRIX matSpace = XMLoadFloat4x4(pMatSpace);

				StFBXKeyFrame* pKeyFrame = pBoneAnim->GetAt(i);
				XMFLOAT4X4* pMatKeyFrame = (XMFLOAT4X4*)(&(pKeyFrame->matKeyTransform));
				XMMATRIX matKeyFrame = XMLoadFloat4x4(pMatKeyFrame);

				XMMATRIX result = XMMatrixMultiply(matSpace, matKeyFrame);
				XMFLOAT4X4 theResult;
				XMStoreFloat4x4(&theResult, result);

				pRotate->element[0][0] = theResult(0,0);
				pRotate->element[0][1] = theResult(0,1);
				pRotate->element[0][2] = theResult(0,2);
				pRotate->element[1][0] = theResult(1,0);
				pRotate->element[1][1] = theResult(1,1);
				pRotate->element[1][2] = theResult(1,2);
				pRotate->element[2][0] = theResult(2,0);
				pRotate->element[2][1] = theResult(2,1);
				pRotate->element[2][2] = theResult(2,2);

				pTranslate->x = theResult(3,0);
				pTranslate->y = theResult(3,1);
				pTranslate->z = theResult(3,2);

				//XMMATRIX InvTranspose = XMMatrixInverse(&XMMatrixDeterminant(result), result);
				//InvTranspose = XMMatrixTranspose(InvTranspose);
				//XMFLOAT4X4 theInvTrans;
				//XMStoreFloat4x4(&theInvTrans, InvTranspose);
				//pInvTrans[0] = theInvTrans(0,0);
				//pInvTrans[1] = theInvTrans(0,1);
				//pInvTrans[2] = theInvTrans(0,2);
				//pInvTrans[3] = theInvTrans(0,3);
				//pInvTrans[4] = theInvTrans(1,0);
				//pInvTrans[5] = theInvTrans(1,1);
				//pInvTrans[6] = theInvTrans(1,2);
				//pInvTrans[7] = theInvTrans(1,3);
				//pInvTrans[8] = theInvTrans(2,0);
				//pInvTrans[9] = theInvTrans(2,1);
				//pInvTrans[10] = theInvTrans(2,2);
				//pInvTrans[11] = theInvTrans(2,3);
			}
			else
			{
				pRotate->element[0][0] = 1.0f;
				pRotate->element[0][1] = 0.0f;
				pRotate->element[0][2] = 0.0f;
				pRotate->element[1][0] = 0.0f;
				pRotate->element[1][1] = 1.0f;
				pRotate->element[1][2] = 0.0f;
				pRotate->element[2][0] = 0.0f;
				pRotate->element[2][1] = 0.0f;
				pRotate->element[2][2] = 1.0f;

				pTranslate->x = 0.0f;
				pTranslate->y = 0.0f;
				pTranslate->z = 0.0f;

				//pInvTrans[0] = 1.0f;
				//pInvTrans[1] = 0.0f;
				//pInvTrans[2] = 0.0f;
				//pInvTrans[3] = 0.0f;
				//pInvTrans[4] = 0.0f;
				//pInvTrans[5] = 1.0f;
				//pInvTrans[6] = 0.0f;
				//pInvTrans[7] = 0.0f;
				//pInvTrans[8] = 0.0f;
				//pInvTrans[9] = 0.0f;
				//pInvTrans[10] = 1.0f;
				//pInvTrans[11] = 0.0f;
			}
		}
	}

	return true;
}
//----------------------------------------------------------------
bool StKKFileKkfWrite::Write_WriteFile(const char* szFileName)
{
	FILE* fp = fopen(szFileName, "wb+");
	if (fp == 0)
	{
		return false;
	}

	fwrite(&m_kFileHead, 1, sizeof(m_kFileHead), fp);
	const int BuffSize = m_kFileHead.KeyFrameCount * m_kFileHead.BoneCount * sizeof(StKKBoneTransform);
	fwrite(m_pBoneTransformBuff, 1, BuffSize, fp);

	fclose(fp);
	return true;
}
//----------------------------------------------------------------
