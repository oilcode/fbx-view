//----------------------------------------------------------------
#ifndef _SoD3DModelCube_h_
#define _SoD3DModelCube_h_
//----------------------------------------------------------------
#include "SoD3DModelBase.h"
//----------------------------------------------------------------
class SoD3DTexture;
class SoD3DShaderBase;
//----------------------------------------------------------------
struct SoD3DModelInitParam_Cube
{
	float fWidth;
	float fHeight;
	float fDepth;
	float fColorR;
	float fColorG;
	float fColorB;
	float fColorA;
	const char* szTextureName;
};
//----------------------------------------------------------------
class SoD3DModelCube : public SoD3DModelBase
{
public:
	SoD3DModelCube();
	~SoD3DModelCube();
	//重写基类的函数
	bool InitModel(void* pInitParam);
	void ClearModel();
	void UpdateModel(float fDeltaTime);
	void RenderModel();

	void SetWorldMatrix(const XMFLOAT4X4* pMatWorld);
	XMMATRIX GetWorldMatrix() const;

private:
	bool CreateVertexBuffer(const SoD3DModelInitParam_Cube* pParam);
	void ReleaseVertexBuffer();
	bool CreateIndexBuffer();
	void ReleaseIndexBuffer();
	bool CreateTexture(const char* szTextureName);
	void ReleaseTexture();

private:
	struct stVertexType
	{
		XMFLOAT3 kPosition;
		XMFLOAT4 kColor;
		XMFLOAT2 kUV;
	};

private:
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	SoD3DShaderBase* m_pShader;
	SoD3DTexture* m_pTexture;
	//世界坐标系矩阵变换。
	XMFLOAT4X4 m_matWorld;
};
//----------------------------------------------------------------
#endif //_SoD3DModelCube_h_
//----------------------------------------------------------------
