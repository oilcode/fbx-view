//----------------------------------------------------------------
#ifndef _SoD3DShaderModelKKNoBone_h_
#define _SoD3DShaderModelKKNoBone_h_
//----------------------------------------------------------------
#include "SoD3DShaderBase.h"
//----------------------------------------------------------------
//这里不定义一个新的结构体，而是使用 stShaderModelKKParam
//struct stShaderModelKKNoBoneParam
//----------------------------------------------------------------
class SoD3DShaderModelKKNoBone : public SoD3DShaderBase
{
public:
	SoD3DShaderModelKKNoBone();
	~SoD3DShaderModelKKNoBone();
	bool InitShaderModelKKNoBone();
	void ClearShaderModelKKNoBone();
	//重写基类的函数
	void ProcessRender(void* pParam);

private:
	bool CreateInputLayout();

private:
	static SoD3DShaderModelKKNoBone* ms_pInstance;
	ID3D11InputLayout* m_pInputLayout;
	ID3DX11Effect* m_pFxEffect;
	ID3DX11EffectTechnique* m_pFxTech;
	ID3DX11EffectMatrixVariable* m_pFxWorldMatrix;
	ID3DX11EffectMatrixVariable* m_pFxViewProjMatrix;
	ID3DX11EffectMatrixVariable* m_pFxWorldInvTransposeMatrix;
	ID3DX11EffectScalarVariable* m_pFxVertexTextureWidth;
	ID3DX11EffectScalarVariable* m_pFxVertexTexturePosCount;
	ID3DX11EffectScalarVariable* m_pFxVertexTextureNormalCount;
	ID3DX11EffectScalarVariable* m_pFxVertexTextureUVCount;
	ID3DX11EffectShaderResourceVariable* m_pFxTexture;
	ID3DX11EffectShaderResourceVariable* m_pFxVertexTexture;
};
//----------------------------------------------------------------
#endif //_SoD3DShaderModelKKNoBone_h_
//----------------------------------------------------------------
