//----------------------------------------------------------------
#include "SoD3DShaderModelCube.h"
#include "SoD3DSystem.h"
#include "SoD3DCamera.h"
//----------------------------------------------------------------
#define ShaderModelCube_File "shader/modelcube.fxo"
//----------------------------------------------------------------
SoD3DShaderModelCube::SoD3DShaderModelCube()
:m_pInputLayout(0)
,m_pFxEffect(0)
,m_pFxTech(0)
,m_pFxWorldViewProj(0)
,m_pFxTexture1(0)
{

}
//----------------------------------------------------------------
SoD3DShaderModelCube::~SoD3DShaderModelCube()
{
	ClearShaderModelCube();
}
//----------------------------------------------------------------
bool SoD3DShaderModelCube::InitShaderModelCube()
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
	if (kShaderFile.LoadFile(ShaderModelCube_File, false) == false)
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
	ID3DX11EffectVariable* pFxTexture1 = m_pFxEffect->GetVariableByName("g_Texture1");
	m_pFxTexture1 = pFxTexture1->GetElement(0)->AsShaderResource();

	if (CreateInputLayout() == false)
	{
		return false;
	}

	return true;
}
//----------------------------------------------------------------
void SoD3DShaderModelCube::ClearShaderModelCube()
{
	if (m_pInputLayout)
	{
		m_pInputLayout->Release();
		m_pInputLayout = 0;
	}
	m_pFxTech = 0;
	m_pFxWorldViewProj = 0;
	m_pFxTexture1 = 0;
	if (m_pFxEffect)
	{
		m_pFxEffect->Release();
		m_pFxEffect = 0;
	}
}
//----------------------------------------------------------------
void SoD3DShaderModelCube::ProcessRender(void* pParam)
{
	stShaderModelCubeParam* pModelParam = (stShaderModelCubeParam*)pParam;
	ID3D11DeviceContext* pD3DDeviceContext = SoD3DSystem::Get()->GetD3DDeviceContext();
	pD3DDeviceContext->IASetInputLayout(m_pInputLayout);
	pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pFxTexture1->SetResource(pModelParam->pSRV);

	UINT uiStride = pModelParam->uiSizeofVertexType;
	UINT uiOffset = 0;
	XMMATRIX worldViewProj = XMMatrixMultiply(*(pModelParam->pMatWorld), SoD3DCamera::Get()->GetMatViewProj());
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pFxTech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		pD3DDeviceContext->IASetVertexBuffers(0, 1, &(pModelParam->pVB), &uiStride, &uiOffset);
		pD3DDeviceContext->IASetIndexBuffer(pModelParam->pIB, DXGI_FORMAT_R32_UINT, 0);

		m_pFxWorldViewProj->SetMatrix(reinterpret_cast<const float*>(&worldViewProj));

		m_pFxTech->GetPassByIndex(p)->Apply(0, pD3DDeviceContext);
		pD3DDeviceContext->DrawIndexed(pModelParam->uiIndexCount, 0, 0);
	}
}
//----------------------------------------------------------------
bool SoD3DShaderModelCube::CreateInputLayout()
{
	if (m_pFxTech == 0)
	{
		return false;
	}

	ID3D11Device* pD3DDevice = SoD3DSystem::Get()->GetD3DDevice();
	HRESULT hr = S_OK;

	const int nDescCount = 3;
	D3D11_INPUT_ELEMENT_DESC kDesc[3];
	kDesc[0].SemanticName = "POSITION";
	kDesc[0].SemanticIndex = 0;
	kDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	kDesc[0].InputSlot = 0;
	kDesc[0].AlignedByteOffset = 0;
	kDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	kDesc[0].InstanceDataStepRate = 0;
	//
	kDesc[1].SemanticName = "COLOR";
	kDesc[1].SemanticIndex = 0;
	kDesc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	kDesc[1].InputSlot = 0;
	kDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	kDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	kDesc[1].InstanceDataStepRate = 0;
	//
	kDesc[2].SemanticName = "TEXCOORD";
	kDesc[2].SemanticIndex = 0;
	kDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	kDesc[2].InputSlot = 0;
	kDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	kDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	kDesc[2].InstanceDataStepRate = 0;

	D3DX11_PASS_DESC passDesc;
	m_pFxTech->GetPassByIndex(0)->GetDesc(&passDesc);
	hr = pD3DDevice->CreateInputLayout(kDesc, nDescCount, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pInputLayout);
	return SUCCEEDED(hr);
}
//----------------------------------------------------------------
