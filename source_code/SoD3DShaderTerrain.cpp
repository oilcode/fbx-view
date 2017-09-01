//----------------------------------------------------------------
#include "SoD3DShaderTerrain.h"
#include "SoD3DSystem.h"
#include "SoD3DCamera.h"
#include "SoSimpleLog.h"
//----------------------------------------------------------------
#define ShaderTerrain_File "shader/terrain.fxo"
//----------------------------------------------------------------
SoD3DShaderTerrain::SoD3DShaderTerrain()
:m_pInputLayout(0)
,m_pFxEffect(0)
,m_pFxTech(0)
,m_pFxWorldViewProj(0)
,m_pFxTexture1(0)
,m_pFxTexture2(0)
{

}
//----------------------------------------------------------------
SoD3DShaderTerrain::~SoD3DShaderTerrain()
{
	ClearD3DShaderTerrain();
}
//----------------------------------------------------------------
bool SoD3DShaderTerrain::InitD3DShaderTerrain()
{
	if (SoD3DSystem::Get() == 0)
	{
		return false;
	}
	ID3D11Device* pD3DDevice = SoD3DSystem::Get()->GetD3DDevice();
	if (pD3DDevice == 0)
	{
		return false;
	}

	SoSimpleLoadFile kShaderFile;
	if (kShaderFile.LoadFile(ShaderTerrain_File, false) == false)
	{
		return false;
	}

	HRESULT hr = D3DX11CreateEffectFromMemory(kShaderFile.GetData(), kShaderFile.GetSize(), 0, pD3DDevice, &m_pFxEffect);
	if (FAILED(hr))
	{
		return false;
	}

	m_pFxTech = m_pFxEffect->GetTechniqueByName("ShaderTec");
	m_pFxWorldViewProj = m_pFxEffect->GetVariableByName("g_matWorldViewProj")->AsMatrix();
	m_pFxTexture1 = m_pFxEffect->GetVariableByName("g_Texture1")->AsShaderResource();
	m_pFxTexture2 = m_pFxEffect->GetVariableByName("g_Texture2")->AsShaderResource();

	if (CreateInputLayout() == false)
	{
		return false;
	}

	return true;
}
//----------------------------------------------------------------
void SoD3DShaderTerrain::ClearD3DShaderTerrain()
{
	if (m_pInputLayout)
	{
		m_pInputLayout->Release();
		m_pInputLayout = 0;
	}
	if (m_pFxEffect)
	{
		m_pFxEffect->Release();
		m_pFxEffect = 0;
	}
	m_pFxTech = 0;
	m_pFxWorldViewProj = 0;
	m_pFxTexture1 = 0;
	m_pFxTexture2 = 0;
}
//----------------------------------------------------------------
void SoD3DShaderTerrain::ProcessRender(void* pParam)
{
	stShaderTerrainParam* pTerrainParam = (stShaderTerrainParam*)pParam;
	ID3D11DeviceContext* pD3DDeviceContext = SoD3DSystem::Get()->GetD3DDeviceContext();
	pD3DDeviceContext->IASetInputLayout(m_pInputLayout);
	pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pFxTexture1->SetResource(pTerrainParam->pSRV);
	m_pFxTexture2->SetResource(pTerrainParam->pSRV2);

	UINT uiStride = pTerrainParam->uiSizeofVertexType;
	UINT uiOffset = 0;
	XMMATRIX worldViewProj = SoD3DCamera::Get()->GetMatViewProj();
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pFxTech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		pD3DDeviceContext->IASetVertexBuffers(0, 1, &(pTerrainParam->pVB), &uiStride, &uiOffset);
		pD3DDeviceContext->IASetIndexBuffer(pTerrainParam->pIB, DXGI_FORMAT_R32_UINT, 0);

		m_pFxWorldViewProj->SetMatrix(reinterpret_cast<const float*>(&worldViewProj));

		m_pFxTech->GetPassByIndex(p)->Apply(0, pD3DDeviceContext);
		pD3DDeviceContext->DrawIndexed(pTerrainParam->uiIndexCount, 0, 0);
	}
}
//----------------------------------------------------------------
bool SoD3DShaderTerrain::CreateInputLayout()
{
	if (m_pFxTech == 0)
	{
		return false;
	}

	ID3D11Device* pD3DDevice = SoD3DSystem::Get()->GetD3DDevice();
	HRESULT hr = S_OK;

	const int nDescCount = 5;
	D3D11_INPUT_ELEMENT_DESC kDesc[5];
	kDesc[0].SemanticName = "POSITION";
	kDesc[0].SemanticIndex = 0;
	kDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	kDesc[0].InputSlot = 0;
	kDesc[0].AlignedByteOffset = 0;
	kDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	kDesc[0].InstanceDataStepRate = 0;
	//
	kDesc[1].SemanticName = "NORMAL";
	kDesc[1].SemanticIndex = 0;
	kDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	kDesc[1].InputSlot = 0;
	kDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	kDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	kDesc[1].InstanceDataStepRate = 0;
	//
	kDesc[2].SemanticName = "COLOR";
	kDesc[2].SemanticIndex = 0;
	kDesc[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	kDesc[2].InputSlot = 0;
	kDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	kDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	kDesc[2].InstanceDataStepRate = 0;
	//
	kDesc[3].SemanticName = "TEXCOORD";
	kDesc[3].SemanticIndex = 0;
	kDesc[3].Format = DXGI_FORMAT_R32G32_FLOAT;
	kDesc[3].InputSlot = 0;
	kDesc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	kDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	kDesc[3].InstanceDataStepRate = 0;
	//
	kDesc[4].SemanticName = "TEXCOORD";
	kDesc[4].SemanticIndex = 1;
	kDesc[4].Format = DXGI_FORMAT_R32G32_FLOAT;
	kDesc[4].InputSlot = 0;
	kDesc[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	kDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	kDesc[4].InstanceDataStepRate = 0;

	D3DX11_PASS_DESC passDesc;
	m_pFxTech->GetPassByIndex(0)->GetDesc(&passDesc);
	hr = pD3DDevice->CreateInputLayout(kDesc, nDescCount, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pInputLayout);
	return SUCCEEDED(hr);
}
//----------------------------------------------------------------
