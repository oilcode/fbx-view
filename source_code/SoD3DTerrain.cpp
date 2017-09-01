//-----------------------------------------------------------------------------
//1，四个顶点构成一个方块，这个方块称作Tile。每个Tile渲染一张地表贴图。
//   如果地表贴图分辨率比较高，可以每4个Tile渲染一张地表贴图，等等。
//2，顶点的组织结构。下面是相邻的4个Tile。顶点非常浪费，有待优化。
//  8---9  12--13
//  | \ |  | \ |
// 11---10 15--14
//  0---1 4---5
//  | \ | | \ |
//  3---2 7---6
//-----------------------------------------------------------------------------
#include "SoD3DTerrain.h"
#include "SoD3DSystem.h"
#include "SoD3DTextureManager.h"
#include "SoD3DShaderManager.h"
//-----------------------------------------------------------------------------
#define SoD3DTerrain_Texture "texture/floor.dds"
#define SoD3DTerrain_Texture2 "texture/defaultTex.dds"
//-----------------------------------------------------------------------------
//每个方格子的宽度，单位是米
const float s_fTileWidth = 1.0f;
//地表沿X轴方向上有多少个格子
const int s_nTileCount_X = 10;
//地表沿Y轴方向上有多少个格子
const int s_nTileCount_Y = 10;
//-----------------------------------------------------------------------------
SoD3DTerrain* SoD3DTerrain::ms_pInstance = NULL;
//-----------------------------------------------------------------------------
bool SoD3DTerrain::CreateD3DTerrain()
{
	bool br = true;
	if (ms_pInstance == NULL)
	{
		ms_pInstance = new SoD3DTerrain;
		if (ms_pInstance && ms_pInstance->InitD3DTerrain())
		{
			//什么都不做
		}
		else
		{
			ReleaseD3DTerrain();
			br = false;
		}
	}
	return br;
}
//-----------------------------------------------------------------------------
void SoD3DTerrain::ReleaseD3DTerrain()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = NULL;
	}
}
//-----------------------------------------------------------------------------
SoD3DTerrain::SoD3DTerrain()
:m_pVertexArray(0)
,m_pIndexArray(0)
,m_pVertexBuffer(0)
,m_pIndexBuffer(0)
,m_pShader(0)
,m_pTexture(0)
,m_pTexture2(0)
,m_nVertexCount(0)
,m_nIndexCount(0)
{

}
//-----------------------------------------------------------------------------
SoD3DTerrain::~SoD3DTerrain()
{
	ClearD3DTerrain();
}
//-----------------------------------------------------------------------------
bool SoD3DTerrain::InitD3DTerrain()
{
	if (CreateVertexBuffer() == false)
	{
		return false;
	}
	if (CreateIndexBuffer() == false)
	{
		return false;
	}
	if (CreateTexture() == false)
	{
		return false;
	}
	m_pShader = SoD3DShaderManager::Get()->GetShader(SoD3DShaderType_Terrain);
	if (m_pShader == 0)
	{
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
void SoD3DTerrain::ClearD3DTerrain()
{
	ReleaseVertexBuffer();
	ReleaseIndexBuffer();
	ReleaseTexture();
	m_pShader = 0;
}
//-----------------------------------------------------------------------------
void SoD3DTerrain::RenderD3DTerrain()
{
	stShaderTerrainParam kParam;
	kParam.pVB = m_pVertexBuffer;
	kParam.pIB = m_pIndexBuffer;
	kParam.pSRV = m_pTexture->GetTextureSRV();
	kParam.pSRV2 = m_pTexture2->GetTextureSRV();
	kParam.uiIndexCount = m_nIndexCount;
	kParam.uiSizeofVertexType = sizeof(stVertexType);
	m_pShader->ProcessRender(&kParam);
}
//-----------------------------------------------------------------------------
bool SoD3DTerrain::CreateVertexBuffer()
{
	//计算有多少个顶点
	const int nVertexTotalCount = s_nTileCount_X * s_nTileCount_Y * 4;
	const int nVertexArraySizeof = sizeof(stVertexType) * nVertexTotalCount;
	stVertexType* pVertexArray = (stVertexType*)malloc(nVertexArraySizeof);
	if (pVertexArray == NULL)
	{
		return false;
	}

	for (int y=0; y<s_nTileCount_Y; ++y)
	{
		for (int x=0; x<s_nTileCount_X; ++x)
		{
			//方块的左上角顶点
			int nIndex = (y * s_nTileCount_X + x) * 4;
			pVertexArray[nIndex].kPosition = XMFLOAT3(x * s_fTileWidth, (y+1) * s_fTileWidth, 0.0f);
			pVertexArray[nIndex].kNormal = XMFLOAT3(0.0f, 0.0f, 1.0f);
			pVertexArray[nIndex].kColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			pVertexArray[nIndex].kTexUV0 = XMFLOAT2(0.0f, 0.0f);
			pVertexArray[nIndex].kTexUV1 = XMFLOAT2(0.0f, 1.0f);

			//方块的右上角顶点
			++nIndex;
			pVertexArray[nIndex].kPosition = XMFLOAT3((x+1) * s_fTileWidth, (y+1) * s_fTileWidth, 0.0f);
			pVertexArray[nIndex].kNormal = XMFLOAT3(0.0f, 0.0f, 1.0f);
			pVertexArray[nIndex].kColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			pVertexArray[nIndex].kTexUV0 = XMFLOAT2(1.0f, 0.0f);
			pVertexArray[nIndex].kTexUV1 = XMFLOAT2(1.0f, 1.0f);

			//方块的右下角顶点
			++nIndex;
			pVertexArray[nIndex].kPosition = XMFLOAT3((x+1) * s_fTileWidth, y * s_fTileWidth, 0.0f);
			pVertexArray[nIndex].kNormal = XMFLOAT3(0.0f, 0.0f, 1.0f);
			pVertexArray[nIndex].kColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			pVertexArray[nIndex].kTexUV0 = XMFLOAT2(1.0f, 1.0f);
			pVertexArray[nIndex].kTexUV1 = XMFLOAT2(1.0f, 0.0f);

			//方块的左下角顶点
			++nIndex;
			pVertexArray[nIndex].kPosition = XMFLOAT3(x * s_fTileWidth, y * s_fTileWidth, 0.0f);
			pVertexArray[nIndex].kNormal = XMFLOAT3(0.0f, 0.0f, 1.0f);
			pVertexArray[nIndex].kColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			pVertexArray[nIndex].kTexUV0 = XMFLOAT2(0.0f, 1.0f);
			pVertexArray[nIndex].kTexUV1 = XMFLOAT2(0.0f, 0.0f);
		}
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = nVertexArraySizeof;
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

	m_pVertexArray = pVertexArray;
	m_nVertexCount = nVertexTotalCount;
	return SUCCEEDED(hr);
}
//-----------------------------------------------------------------------------
void SoD3DTerrain::ReleaseVertexBuffer()
{
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = 0;
	}
	if (m_pVertexArray)
	{
		free(m_pVertexArray);
		m_pVertexArray = 0;
	}
	m_nVertexCount = 0;
}
//-----------------------------------------------------------------------------
bool SoD3DTerrain::CreateIndexBuffer()
{
	//计算有多少个方块
	const int nTileTotalCount = s_nTileCount_X * s_nTileCount_Y;
	//每个方块用6个索引值
	const int nIndexTotalCount = nTileTotalCount * 6;
	const int nIndexArraySizeof = sizeof(unsigned int) * nIndexTotalCount;

	unsigned int* pIndexArray = (unsigned int*)malloc(nIndexArraySizeof);
	if (pIndexArray == NULL)
	{
		return false;
	}

	for (int i=0; i<nTileTotalCount; ++i)
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
	indexBufferDesc.ByteWidth = nIndexArraySizeof;
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

	m_pIndexArray = pIndexArray;
	m_nIndexCount = nIndexTotalCount;
	return SUCCEEDED(hr);
}
//-----------------------------------------------------------------------------
void SoD3DTerrain::ReleaseIndexBuffer()
{
	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = 0;
	}
	if (m_pIndexArray)
	{
		free(m_pIndexArray);
		m_pIndexArray = 0;
	}
	m_nIndexCount = 0;
}
//-----------------------------------------------------------------------------
bool SoD3DTerrain::CreateTexture()
{
	m_pTexture = SoD3DTextureManager::Get()->CreateTextureFromFile(SoD3DTerrain_Texture);
	if (m_pTexture)
	{
		m_pTexture->AddRefTexture();
	}
	m_pTexture2 = SoD3DTextureManager::Get()->CreateTextureFromFile(SoD3DTerrain_Texture2);
	if (m_pTexture2)
	{
		m_pTexture2->AddRefTexture();
	}
	return (m_pTexture != NULL && m_pTexture2 != NULL);
}
//-----------------------------------------------------------------------------
void SoD3DTerrain::ReleaseTexture()
{
	if (m_pTexture)
	{
		m_pTexture->RemoveRefTexture();
		m_pTexture = 0;
	}
	if (m_pTexture2)
	{
		m_pTexture2->RemoveRefTexture();
		m_pTexture2 = 0;
	}
}
//-----------------------------------------------------------------------------
