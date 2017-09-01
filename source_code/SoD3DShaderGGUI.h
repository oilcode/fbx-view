//----------------------------------------------------------------
#ifndef _SoD3DShaderGGUI_h_
#define _SoD3DShaderGGUI_h_
//----------------------------------------------------------------
#include "SoD3DShaderBase.h"
//----------------------------------------------------------------
#define SoD3DShaderGGUI_TexListSize 4
//----------------------------------------------------------------
struct stShaderGGUIParam
{
	ID3D11Buffer* pVB;
	ID3D11Buffer* pIB;
	unsigned int uiIndexCount;
	unsigned int uiSizeofVertexType;
	ID3D11ShaderResourceView** pSRVList;
	int nSRVCount;
};
//----------------------------------------------------------------
class SoD3DShaderGGUI : public SoD3DShaderBase
{
public:
	SoD3DShaderGGUI();
	~SoD3DShaderGGUI();
	bool InitD3DShaderGGUI();
	void ClearD3DShaderGGUI();
	//重写基类的函数
	void ProcessRender(void* pParam);

private:
	bool CreateInputLayout();
	bool CreateBlendState();

private:
	ID3D11InputLayout* m_pInputLayout;
	ID3D11BlendState* m_pBlendState;
	ID3DX11Effect* m_pFxEffect;
	ID3DX11EffectTechnique* m_pFxTech;
	ID3DX11EffectMatrixVariable* m_pFxMatViewProj;
	ID3DX11EffectShaderResourceVariable* m_pFxTextureList[SoD3DShaderGGUI_TexListSize];
};
//----------------------------------------------------------------
#endif //_SoD3DShaderGGUI_h_
//----------------------------------------------------------------
