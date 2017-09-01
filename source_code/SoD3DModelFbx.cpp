//----------------------------------------------------------------
#include "SoD3DModelFbx.h"
#include "SoD3DSystem.h"
#include "SoD3DShaderManager.h"
#include "SoD3DTextureManager.h"
#include "StFBXModel.h"
//----------------------------------------------------------------
SoD3DModelFbx::SoD3DModelFbx()
:m_pVertexBuffer(0)
,m_pIndexBuffer(0)
,m_pShader(0)
,m_pTexture(0)
,m_nVertexCount(0)
,m_nIndexCount(0)
,m_fAccTime(0.0f)
{

}
//----------------------------------------------------------------
SoD3DModelFbx::~SoD3DModelFbx()
{

}
//----------------------------------------------------------------
bool SoD3DModelFbx::InitModel(void* pInitParam)
{
	SoD3DModelInitParam_Fbx* pFbxParam = (SoD3DModelInitParam_Fbx*)pInitParam;
	bool bSuccess = false;
	do 
	{
		if (CreateVertexBuffer(pFbxParam->pMeshData) == false)
		{
			break;
		}
		if (CreateIndexBuffer(pFbxParam->pMeshData) == false)
		{
			break;
		}
		if (CreateTexture(NULL) == false)
		{
			break;
		}
		m_pShader = SoD3DShaderManager::Get()->GetShader(SoD3DShaderType_ModelFbx);
		if (m_pShader == 0)
		{
			break;
		}
		XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());
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
void SoD3DModelFbx::ClearModel()
{
	ReleaseVertexBuffer();
	ReleaseIndexBuffer();
	ReleaseTexture();
	m_pShader = 0;
	m_nVertexCount = 0;
	m_nIndexCount = 0;
}
//----------------------------------------------------------------
void SoD3DModelFbx::UpdateModel(float fDeltaTime, StFBXModel* pAnimatedModel)
{
	if (pAnimatedModel->GetKeyFrameCount() > 0)
	{
		m_fAccTime += fDeltaTime;
		const StFBXMeshData* pMeshData = pAnimatedModel->GetAnimationMeshData(m_fAccTime);

		ID3D11DeviceContext* pD3DDeviceContext = SoD3DSystem::Get()->GetD3DDeviceContext();
		////<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		//把顶点数据从CPU内存拷贝到GPU内存中
		D3D11_MAPPED_SUBRESOURCE kMappedResource;
		memset(&kMappedResource, 0, sizeof(kMappedResource));
		HRESULT hr = pD3DDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &kMappedResource);
		if (SUCCEEDED(hr))
		{
			memcpy(kMappedResource.pData, pMeshData->pVertexBuff, pMeshData->nVertexBuffSize);
			pD3DDeviceContext->Unmap(m_pVertexBuffer, 0);
		}
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	}
}
//----------------------------------------------------------------
void SoD3DModelFbx::RenderModel()
{
	XMMATRIX kWorld = XMLoadFloat4x4(&m_matWorld);
	//
	stShaderModelFbxParam kParam;
	kParam.pVB = m_pVertexBuffer;
	kParam.pIB = m_pIndexBuffer;
	kParam.pSRV = m_pTexture->GetTextureSRV();
	kParam.pMatWorld = &kWorld;
	kParam.uiIndexCount = m_nIndexCount;
	kParam.uiSizeofVertexType = sizeof(stVertexType);
	m_pShader->ProcessRender(&kParam);
}
//----------------------------------------------------------------
void SoD3DModelFbx::SetWorldMatrix(const XMFLOAT4X4* pMatWorld)
{
	m_matWorld = *pMatWorld;
}
//----------------------------------------------------------------
void SoD3DModelFbx::SetTexture(const char* szFileName)
{
	CreateTexture(szFileName);
}
//----------------------------------------------------------------
bool SoD3DModelFbx::CreateVertexBuffer(const StFBXMeshData* pMeshData)
{
	if (pMeshData->pVertexBuff == 0 || pMeshData->nVertexCount == 0)
	{
		return false;
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(stVertexType) * pMeshData->nVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = pMeshData->pVertexBuff;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	ID3D11Device* pD3DDevice = SoD3DSystem::Get()->GetD3DDevice();
	HRESULT hr = pD3DDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer);

	m_nVertexCount = pMeshData->nVertexCount;
	return SUCCEEDED(hr);
}
//----------------------------------------------------------------
void SoD3DModelFbx::ReleaseVertexBuffer()
{
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = 0;
	}
}
//----------------------------------------------------------------
bool SoD3DModelFbx::CreateIndexBuffer(const StFBXMeshData* pMeshData)
{
	if (pMeshData->pIndexBuff == 0 || pMeshData->nIndexCount == 0)
	{
		return false;
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * pMeshData->nIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = pMeshData->pIndexBuff;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	ID3D11Device* pD3DDevice = SoD3DSystem::Get()->GetD3DDevice();
	HRESULT hr = pD3DDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer);

	m_nIndexCount = pMeshData->nIndexCount;
	return SUCCEEDED(hr);
}
//----------------------------------------------------------------
void SoD3DModelFbx::ReleaseIndexBuffer()
{
	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = 0;
	}
}
//----------------------------------------------------------------
bool SoD3DModelFbx::CreateTexture(const char* szFileName)
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
void SoD3DModelFbx::ReleaseTexture()
{
	if (m_pTexture)
	{
		m_pTexture->RemoveRefTexture();
		m_pTexture = NULL;
	}
}
//----------------------------------------------------------------
