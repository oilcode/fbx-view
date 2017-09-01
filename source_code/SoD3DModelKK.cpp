//----------------------------------------------------------------
#include "SoD3DModelKK.h"
#include "SoD3DSystem.h"
#include "SoD3DShaderManager.h"
#include "SoD3DTextureManager.h"
#include "StKKModel.h"
//----------------------------------------------------------------
SoD3DModelKK::SoD3DModelKK()
:m_pKKModel(0)
,m_pShader(0)
,m_pTexture(0)
,m_fAccTime(0.0f)
,m_nCurrentKeyFrameIndex(0)
{

}
//----------------------------------------------------------------
SoD3DModelKK::~SoD3DModelKK()
{

}
//----------------------------------------------------------------
bool SoD3DModelKK::InitModel(void* pInitParam)
{
	SoD3DModelInitParam_KK* pKKParam = (SoD3DModelInitParam_KK*)pInitParam;

	bool bSuccess = false;
	do 
	{
		if (pKKParam->pKKModel->IsBoneDataExist())
		{
			m_pShader = SoD3DShaderManager::Get()->GetShader(SoD3DShaderType_ModelKK);
		}
		else
		{
			m_pShader = SoD3DShaderManager::Get()->GetShader(SoD3DShaderType_ModelKKNoBone);
		}
		if (m_pShader == 0)
		{
			break;
		}
		if (CreateTexture(NULL) == false)
		{
			break;
		}

		XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());

		m_pKKModel = pKKParam->pKKModel;
		//
		bSuccess = true;
	} while(0);

	if (bSuccess == false)
	{
		ClearModel();
	}
	return bSuccess;
}
//----------------------------------------------------------------
void SoD3DModelKK::ClearModel()
{
	ReleaseTexture();
	m_pShader = 0;
	m_pKKModel = 0;
	m_fAccTime = 0.0f;
	m_nCurrentKeyFrameIndex = 0;
}
//----------------------------------------------------------------
void SoD3DModelKK::UpdateModel(float fDeltaTime)
{
	m_fAccTime += fDeltaTime;
	if (m_pKKModel)
	{
		m_nCurrentKeyFrameIndex = m_pKKModel->GetKeyFrameIndexByID(1, m_fAccTime);
	}
}
//----------------------------------------------------------------
void SoD3DModelKK::RenderModel()
{
	XMMATRIX kWorld = XMLoadFloat4x4(&m_matWorld);
	//
	const StKKModelData* pModelData = m_pKKModel->GetModelData();
	const StKKModelAnimation* pModelAnim = m_pKKModel->GetAnimByID(1);
	//
	stShaderModelKKParam kParam;
	kParam.pVB = pModelData->pVertexStructBuffer;
	kParam.pIB = pModelData->pIndexBuffer;
	kParam.uiSizeofVertex = pModelData->nSizeofVertexStruct;
	kParam.uiIndexCount = pModelData->nIndexCount;
	kParam.pTextureSRV = m_pTexture->GetTextureSRV();
	kParam.pMatWorld = &kWorld;
	kParam.pPosSRV = pModelData->pVertexValueTexture->GetTextureSRV();
	kParam.nPosSRVWidth = pModelData->nTextureWidth;
	kParam.nPosCount = pModelData->nPosCount;
	kParam.nNormalCount = pModelData->nNormalCount;
	kParam.nUVCount = pModelData->nUVCount;
	if (pModelAnim)
	{
		kParam.pAnimSRV = pModelAnim->pAnimationTexture->GetTextureSRV();
		kParam.nAnimSRVWidth = pModelAnim->nTextureWidth;
		kParam.nBoneCount = pModelAnim->nBoneCount;
		kParam.nKeyFrameIndex = m_nCurrentKeyFrameIndex;
	}
	else
	{
		kParam.pAnimSRV = NULL;
		kParam.nAnimSRVWidth = 0;
		kParam.nBoneCount = 0;
		kParam.nKeyFrameIndex = -1;
	}
	//
	m_pShader->ProcessRender(&kParam);
}
//----------------------------------------------------------------
void SoD3DModelKK::SetWorldMatrix(const XMFLOAT4X4* pMatWorld)
{
	m_matWorld = *pMatWorld;
}
//----------------------------------------------------------------
void SoD3DModelKK::SetTexture(const char* szFileName)
{
	CreateTexture(szFileName);
}
//----------------------------------------------------------------
bool SoD3DModelKK::CreateTexture(const char* szFileName)
{
	ReleaseTexture();
	//
	const char* szRealName = szFileName;
	if (szRealName == 0 || szRealName[0] == 0)
	{
		szRealName = "texture/defaultTex.dds";
	}
	//
	m_pTexture = SoD3DTextureManager::Get()->CreateTextureFromFile(szRealName);
	if (m_pTexture)
	{
		m_pTexture->AddRefTexture();
	}
	return (m_pTexture != NULL);
}
//----------------------------------------------------------------
void SoD3DModelKK::ReleaseTexture()
{
	if (m_pTexture)
	{
		m_pTexture->RemoveRefTexture();
		m_pTexture = NULL;
	}
}
//----------------------------------------------------------------
