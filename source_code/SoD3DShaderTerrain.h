//----------------------------------------------------------------
#ifndef _SoD3DShaderTerrain_h_
#define _SoD3DShaderTerrain_h_
//----------------------------------------------------------------
#include "SoD3DShaderBase.h"
//----------------------------------------------------------------
struct stShaderTerrainParam
{
	ID3D11Buffer* pVB;
	ID3D11Buffer* pIB;
	ID3D11ShaderResourceView* pSRV;
	ID3D11ShaderResourceView* pSRV2;
	unsigned int uiIndexCount;
	unsigned int uiSizeofVertexType;
};
//----------------------------------------------------------------
class SoD3DShaderTerrain : public SoD3DShaderBase
{
public:
	SoD3DShaderTerrain();
	~SoD3DShaderTerrain();
	bool InitD3DShaderTerrain();
	void ClearD3DShaderTerrain();
	//重写基类的函数
	void ProcessRender(void* pParam);

private:
	bool CreateInputLayout();

private:
	static SoD3DShaderTerrain* ms_pInstance;
	ID3D11InputLayout* m_pInputLayout;
	ID3DX11Effect* m_pFxEffect;
	ID3DX11EffectTechnique* m_pFxTech;
	ID3DX11EffectMatrixVariable* m_pFxWorldViewProj;
	ID3DX11EffectShaderResourceVariable* m_pFxTexture1;
	ID3DX11EffectShaderResourceVariable* m_pFxTexture2;
};
//----------------------------------------------------------------
#endif //_SoD3DShaderTerrain_h_
//----------------------------------------------------------------
