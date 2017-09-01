//----------------------------------------------------------------
#ifndef _SoD3DShaderModelCube_h_
#define _SoD3DShaderModelCube_h_
//----------------------------------------------------------------
#include "SoD3DShaderBase.h"
//----------------------------------------------------------------
struct stShaderModelCubeParam
{
	ID3D11Buffer* pVB;
	ID3D11Buffer* pIB;
	ID3D11ShaderResourceView* pSRV;
	XMMATRIX* pMatWorld;
	unsigned int uiIndexCount;
	unsigned int uiSizeofVertexType;
};
//----------------------------------------------------------------
class SoD3DShaderModelCube : public SoD3DShaderBase
{
public:
	SoD3DShaderModelCube();
	~SoD3DShaderModelCube();
	bool InitShaderModelCube();
	void ClearShaderModelCube();
	//重写基类的函数
	void ProcessRender(void* pParam);

private:
	bool CreateInputLayout();

private:
	static SoD3DShaderModelCube* ms_pInstance;
	ID3D11InputLayout* m_pInputLayout;
	ID3DX11Effect* m_pFxEffect;
	ID3DX11EffectTechnique* m_pFxTech;
	ID3DX11EffectMatrixVariable* m_pFxWorldViewProj;
	ID3DX11EffectShaderResourceVariable* m_pFxTexture1;
};
//----------------------------------------------------------------
#endif //_SoD3DShaderModelCube_h_
//----------------------------------------------------------------
