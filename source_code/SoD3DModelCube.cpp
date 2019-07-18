//----------------------------------------------------------------
//1，ArchorPoint锚点位于Cube的中心点。
//2，FootPoint足点位于Cube底面的中心点。
//----------------------------------------------------------------
#include "SoD3DModelCube.h"
#include "SoD3DSystem.h"
#include "SoD3DTextureManager.h"
#include "SoD3DShaderManager.h"
#include "SoD3DCamera.h"
//----------------------------------------------------------------
#define SoD3DModelCube_VertexCount 24
#define SoD3DModelCube_IndexCount 36
//----------------------------------------------------------------
SoD3DModelCube::SoD3DModelCube()
:m_pVertexBuffer(NULL)
,m_pIndexBuffer(NULL)
,m_pShader(NULL)
,m_pTexture(NULL)
{

}
//----------------------------------------------------------------
SoD3DModelCube::~SoD3DModelCube()
{
	
}
//----------------------------------------------------------------
bool SoD3DModelCube::InitModel(void* pInitParam)
{
	const SoD3DModelInitParam_Cube* pCubeParam = (const SoD3DModelInitParam_Cube*)pInitParam;
	bool bSuccess = false;
	do 
	{
		if (CreateVertexBuffer(pCubeParam) == false)
		{
			break;
		}
		if (CreateIndexBuffer() == false)
		{
			break;
		}
		if (CreateTexture(pCubeParam->szTextureName) == false)
		{
			break;
		}
		m_pShader = SoD3DShaderManager::Get()->GetShader(SoD3DShaderType_ModelCube);
		if (m_pShader == 0)
		{
			break;
		}
		XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());
		//
		bSuccess = true;
	} while (0);

	if (bSuccess == false)
	{
		ClearModel();
	}
	return bSuccess;
}
//----------------------------------------------------------------
void SoD3DModelCube::ClearModel()
{
	ReleaseTexture();
	ReleaseIndexBuffer();
	ReleaseVertexBuffer();
	m_pShader = 0;
}
//----------------------------------------------------------------
void SoD3DModelCube::UpdateModel(float fDeltaTime)
{

}
//----------------------------------------------------------------
void SoD3DModelCube::RenderModel()
{
	XMMATRIX kWorld = XMLoadFloat4x4(&m_matWorld);
	//
	stShaderModelCubeParam kParam;
	kParam.pVB = m_pVertexBuffer;
	kParam.pIB = m_pIndexBuffer;
	kParam.pSRV = m_pTexture->GetTextureSRV();
	kParam.pMatWorld = &kWorld;
	kParam.uiIndexCount = SoD3DModelCube_IndexCount;
	kParam.uiSizeofVertexType = sizeof(stVertexType);
	m_pShader->ProcessRender(&kParam);
}
//----------------------------------------------------------------
void SoD3DModelCube::SetWorldMatrix(const XMFLOAT4X4* pMatWorld)
{
	m_matWorld = *pMatWorld;
}
//----------------------------------------------------------------
XMMATRIX SoD3DModelCube::GetWorldMatrix() const
{
	return XMLoadFloat4x4(&m_matWorld);
}
//----------------------------------------------------------------
bool SoD3DModelCube::CreateVertexBuffer(const SoD3DModelInitParam_Cube* pParam)
{
	stVertexType* pVertexArray = new stVertexType[SoD3DModelCube_VertexCount];
	if (pVertexArray == NULL)
	{
		return false;
	}

	const float width = pParam->fWidth * 0.5f;
	const float depth = pParam->fDepth * 0.5f;
	const float height = pParam->fHeight * 0.5f;
	const XMFLOAT4 color(pParam->fColorR, pParam->fColorG, pParam->fColorB, pParam->fColorA);

	//Front
	//此面与Y轴垂直，位于Y轴负方向处。
	//1---2
	//| / |
	//0---3
	int curIndex = 0;
	pVertexArray[curIndex].kPosition = XMFLOAT3(-width, -depth, -height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(0.0f, 1.0f);

	++curIndex;
	pVertexArray[curIndex].kPosition = XMFLOAT3(-width, -depth, height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(0.0f, 0.0f);

	++curIndex;
	pVertexArray[curIndex].kPosition = XMFLOAT3(width, -depth, height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(1.0f, 0.0f);

	++curIndex;
	pVertexArray[curIndex].kPosition = XMFLOAT3(width, -depth, -height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(1.0f, 1.0f);

	// Back
	//此面与Y轴垂直，位于Y轴正方向处。由于此面位于背面，所以顶点顺序有变化。
	//3---2
	//| / |
	//0---1
	++curIndex;
	pVertexArray[curIndex].kPosition = XMFLOAT3(-width, depth, -height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(0.0f, 1.0f);

	++curIndex;
	pVertexArray[curIndex].kPosition = XMFLOAT3(width, depth, -height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(1.0f, 1.0f);

	++curIndex;
	pVertexArray[curIndex].kPosition = XMFLOAT3(width, depth, height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(1.0f, 0.0f);

	++curIndex;
	pVertexArray[curIndex].kPosition = XMFLOAT3(-width, depth, height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(0.0f, 0.0f);

	// Left
	//此面与X轴垂直，位于X轴负方向处。
	//1---2
	//| / |
	//0---3
	++curIndex;
	pVertexArray[curIndex].kPosition = XMFLOAT3(-width, depth, -height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(0.0f, 1.0f);

	++curIndex;
	pVertexArray[curIndex].kPosition = XMFLOAT3(-width, depth, height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(0.0f, 0.0f);

	++curIndex;
	pVertexArray[curIndex].kPosition = XMFLOAT3(-width, -depth, height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(1.0f, 0.0f);

	++curIndex;
	pVertexArray[curIndex].kPosition = XMFLOAT3(-width, -depth, -height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(1.0f, 1.0f);

	// Right
	//此面与X轴垂直，位于X轴正方向处。
	//1---2
	//| / |
	//0---3
	++curIndex;
	pVertexArray[curIndex].kPosition = XMFLOAT3(width, -depth, -height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(0.0f, 1.0f);

	++curIndex;
	pVertexArray[curIndex].kPosition = XMFLOAT3(width, -depth, height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(0.0f, 0.0f);

	++curIndex;
	pVertexArray[curIndex].kPosition = XMFLOAT3(width, depth, height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(1.0f, 0.0f);

	++curIndex;
	pVertexArray[curIndex].kPosition = XMFLOAT3(width, depth, -height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(1.0f, 1.0f);

	// Top
	//此面与Z轴垂直，位于Z轴正方向处。
	//0---1
	//| \ |
	//3---2
	++curIndex;
	pVertexArray[curIndex].kPosition = XMFLOAT3(-width, depth, height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(0.0f, 0.0f);

	++curIndex;
	pVertexArray[curIndex].kPosition = XMFLOAT3(width, depth, height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(1.0f, 0.0f);

	++curIndex;
	pVertexArray[curIndex].kPosition = XMFLOAT3(width, -depth, height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(1.0f, 1.0f);

	++curIndex;
	pVertexArray[curIndex].kPosition = XMFLOAT3(-width, -depth, height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(0.0f, 1.0f);

	// Bottom
	//此面与Z轴垂直，位于Z轴负方向处。
	//0---1
	//| \ |
	//3---2
	++curIndex;
	pVertexArray[curIndex].kPosition = XMFLOAT3(-width, -depth, -height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(0.0f, 0.0f);

	++curIndex;
	pVertexArray[curIndex].kPosition = XMFLOAT3(width, -depth, -height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(1.0f, 0.0f);

	++curIndex;
	pVertexArray[curIndex].kPosition = XMFLOAT3(width, depth, -height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(1.0f, 1.0f);

	++curIndex;
	pVertexArray[curIndex].kPosition = XMFLOAT3(-width, depth, -height);
	pVertexArray[curIndex].kColor = color;
	pVertexArray[curIndex].kUV = XMFLOAT2(0.0f, 1.0f);

	//
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(stVertexType) * SoD3DModelCube_VertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = pVertexArray;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	ID3D11Device* pD3DDevice = SoD3DSystem::Get()->GetD3DDevice();
	HRESULT hr = pD3DDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer);

	delete [] pVertexArray;
	pVertexArray = 0;

	return SUCCEEDED(hr);
}
//----------------------------------------------------------------
void SoD3DModelCube::ReleaseVertexBuffer()
{
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = 0;
	}
}
//----------------------------------------------------------------
bool SoD3DModelCube::CreateIndexBuffer()
{
	unsigned int* pIndexArray = new unsigned int[SoD3DModelCube_IndexCount];
	if (pIndexArray == NULL)
	{
		return false;
	}

	for (int i=0; i<6; ++i)
	{
		int nIndex = i * 6;
		pIndexArray[nIndex+0] = i*4+0;
		pIndexArray[nIndex+1] = i*4+1;
		pIndexArray[nIndex+2] = i*4+2;
		pIndexArray[nIndex+3] = i*4+0;
		pIndexArray[nIndex+4] = i*4+2;
		pIndexArray[nIndex+5] = i*4+3;
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * SoD3DModelCube_IndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = pIndexArray;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	ID3D11Device* pD3DDevice = SoD3DSystem::Get()->GetD3DDevice();
	HRESULT hr = pD3DDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer);

	delete [] pIndexArray;
	pIndexArray = 0;

	return SUCCEEDED(hr);
}
//----------------------------------------------------------------
void SoD3DModelCube::ReleaseIndexBuffer()
{
	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = 0;
	}
}
//----------------------------------------------------------------
bool SoD3DModelCube::CreateTexture(const char* szTextureName)
{
	if (szTextureName == 0 || szTextureName[0] == 0)
	{
		return false;
	}

	m_pTexture = SoD3DTextureManager::Get()->CreateTextureFromFile(szTextureName);
	if (m_pTexture)
	{
		m_pTexture->AddRefTexture();
	}
	return (m_pTexture != NULL);
}
//----------------------------------------------------------------
void SoD3DModelCube::ReleaseTexture()
{
	if (m_pTexture)
	{
		m_pTexture->RemoveRefTexture();
		m_pTexture = NULL;
	}
}
//----------------------------------------------------------------
