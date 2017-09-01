//----------------------------------------------------------------
#include "SoD3DShaderGGUI.h"
#include "SoD3DSystem.h"
#include "SoD3DCameraUI.h"
//----------------------------------------------------------------
#define ShaderGGUI_File "shader/ggui.fxo"
//----------------------------------------------------------------
SoD3DShaderGGUI::SoD3DShaderGGUI()
:m_pInputLayout(0)
,m_pBlendState(0)
,m_pFxEffect(0)
,m_pFxTech(0)
,m_pFxMatViewProj(0)
{
	for (int i = 0; i < SoD3DShaderGGUI_TexListSize; ++i)
	{
		m_pFxTextureList[i] = 0;
	}
}
//----------------------------------------------------------------
SoD3DShaderGGUI::~SoD3DShaderGGUI()
{
	ClearD3DShaderGGUI();
}
//----------------------------------------------------------------
bool SoD3DShaderGGUI::InitD3DShaderGGUI()
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
	if (kShaderFile.LoadFile(ShaderGGUI_File, false) == false)
	{
		return false;
	}

	HRESULT hr = D3DX11CreateEffectFromMemory(kShaderFile.GetData(), kShaderFile.GetSize(), 0, pD3DDevice, &m_pFxEffect);
	if (FAILED(hr))
	{
		return false;
	}

	m_pFxTech = m_pFxEffect->GetTechniqueByName("ShaderTec");
	m_pFxMatViewProj = m_pFxEffect->GetVariableByName("g_MatrixViewProj")->AsMatrix();
	ID3DX11EffectVariable* pFxTextureList = m_pFxEffect->GetVariableByName("g_TextureList");
	for (int i = 0; i < SoD3DShaderGGUI_TexListSize; ++i)
	{
		m_pFxTextureList[i] = pFxTextureList->GetElement(i)->AsShaderResource();
	}

	if (CreateInputLayout() == false)
	{
		return false;
	}
	if (CreateBlendState() == false)
	{
		return false;
	}

	return true;
}
//----------------------------------------------------------------
void SoD3DShaderGGUI::ClearD3DShaderGGUI()
{
	if (m_pInputLayout)
	{
		m_pInputLayout->Release();
		m_pInputLayout = 0;
	}
	if (m_pBlendState)
	{
		m_pBlendState->Release();
		m_pBlendState = 0;
	}
	m_pFxTech = 0;
	m_pFxMatViewProj = 0;
	for (int i = 0; i < SoD3DShaderGGUI_TexListSize; ++i)
	{
		m_pFxTextureList[i] = 0;
	}
	if (m_pFxEffect)
	{
		m_pFxEffect->Release();
		m_pFxEffect = 0;
	}
}
//----------------------------------------------------------------
void SoD3DShaderGGUI::ProcessRender(void* pParam)
{
	stShaderGGUIParam* pGGUIParam = (stShaderGGUIParam*)pParam;
	ID3D11DeviceContext* pD3DDeviceContext = SoD3DSystem::Get()->GetD3DDeviceContext();
	pD3DDeviceContext->IASetInputLayout(m_pInputLayout);
	pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//ÉèÖÃblendÒò×Ó
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	pD3DDeviceContext->OMSetBlendState(m_pBlendState, blendFactor, 0xFFFFFFFF);

	for (int i = 0; i < SoD3DShaderGGUI_TexListSize; ++i)
	{
		if (i < pGGUIParam->nSRVCount)
		{
			m_pFxTextureList[i]->SetResource(pGGUIParam->pSRVList[i]);
		}
		else
		{
			m_pFxTextureList[i]->SetResource(0);
		}
	}

	UINT uiStride = pGGUIParam->uiSizeofVertexType;
	UINT uiOffset = 0;
	XMMATRIX worldViewProj = SoD3DCameraUI::Get()->GetMatViewProj();
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pFxTech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		pD3DDeviceContext->IASetVertexBuffers(0, 1, &(pGGUIParam->pVB), &uiStride, &uiOffset);
		pD3DDeviceContext->IASetIndexBuffer(pGGUIParam->pIB, DXGI_FORMAT_R32_UINT, 0);

		m_pFxMatViewProj->SetMatrix(reinterpret_cast<const float*>(&worldViewProj));

		m_pFxTech->GetPassByIndex(p)->Apply(0, pD3DDeviceContext);
		pD3DDeviceContext->DrawIndexed(pGGUIParam->uiIndexCount, 0, 0);
	}
}
//----------------------------------------------------------------
bool SoD3DShaderGGUI::CreateInputLayout()
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
	kDesc[1].SemanticName = "TEXCOORD";
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

	D3DX11_PASS_DESC passDesc;
	m_pFxTech->GetPassByIndex(0)->GetDesc(&passDesc);
	hr = pD3DDevice->CreateInputLayout(kDesc, nDescCount, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pInputLayout);
	return SUCCEEDED(hr);
}
//----------------------------------------------------------------
bool SoD3DShaderGGUI::CreateBlendState()
{
	ID3D11Device* pD3DDevice = SoD3DSystem::Get()->GetD3DDevice();
	HRESULT hr = S_OK;

	D3D11_BLEND_DESC kDesc;
	kDesc.AlphaToCoverageEnable = FALSE;
	kDesc.IndependentBlendEnable = FALSE;
	kDesc.RenderTarget[0].BlendEnable = TRUE;
	kDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	kDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	kDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	kDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	kDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	kDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	kDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = pD3DDevice->CreateBlendState(&kDesc, &m_pBlendState);
	return SUCCEEDED(hr);
}
//----------------------------------------------------------------
