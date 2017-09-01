//----------------------------------------------------------------
#ifndef _SoD3DModelKK_h_
#define _SoD3DModelKK_h_
//----------------------------------------------------------------
#include "SoD3DModelBase.h"
//----------------------------------------------------------------
class SoD3DTexture;
class SoD3DShaderBase;
class StKKModel;
//----------------------------------------------------------------
struct SoD3DModelInitParam_KK
{
	const StKKModel* pKKModel;
};
//----------------------------------------------------------------
class SoD3DModelKK : public SoD3DModelBase
{
public:
	SoD3DModelKK();
	~SoD3DModelKK();
	//重写基类的函数
	bool InitModel(void* pInitParam);
	void ClearModel();
	void UpdateModel(float fDeltaTime);
	void RenderModel();

	void SetWorldMatrix(const XMFLOAT4X4* pMatWorld);
	void SetTexture(const char* szFileName);

private:
	bool CreateTexture(const char* szFileName);
	void ReleaseTexture();

private:
	const StKKModel* m_pKKModel;
	SoD3DShaderBase* m_pShader;
	SoD3DTexture* m_pTexture;
	//世界坐标系矩阵变换。
	XMFLOAT4X4 m_matWorld;
	float m_fAccTime;
	int m_nCurrentKeyFrameIndex;

};
//----------------------------------------------------------------
#endif //_SoD3DModelKK_h_
//----------------------------------------------------------------
