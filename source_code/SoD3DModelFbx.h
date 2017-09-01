//----------------------------------------------------------------
#ifndef _SoD3DModelFbx_h_
#define _SoD3DModelFbx_h_
//----------------------------------------------------------------
#include "SoD3DModelBase.h"
//----------------------------------------------------------------
class SoD3DTexture;
class SoD3DShaderBase;
struct StFBXMeshData;
class StFBXModel;
//----------------------------------------------------------------
struct SoD3DModelInitParam_Fbx
{
	const StFBXMeshData* pMeshData;
};
//----------------------------------------------------------------
class SoD3DModelFbx : public SoD3DModelBase
{
public:
	SoD3DModelFbx();
	~SoD3DModelFbx();
	//重写基类的函数
	bool InitModel(void* pInitParam);
	void ClearModel();
	void UpdateModel(float fDeltaTime, StFBXModel* pAnimatedModel);
	void RenderModel();

	void SetWorldMatrix(const XMFLOAT4X4* pMatWorld);
	void SetTexture(const char* szFileName);

private:
	bool CreateVertexBuffer(const StFBXMeshData* pMeshData);
	void ReleaseVertexBuffer();
	bool CreateIndexBuffer(const StFBXMeshData* pMeshData);
	void ReleaseIndexBuffer();
	bool CreateTexture(const char* szFileName);
	void ReleaseTexture();

private:
	struct stVertexType
	{
		XMFLOAT3 kPosition;
		XMFLOAT3 kNormal;
		XMFLOAT2 kUV;
	};

private:
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	SoD3DShaderBase* m_pShader;
	SoD3DTexture* m_pTexture;
	int m_nVertexCount;
	int m_nIndexCount;
	//世界坐标系矩阵变换。
	XMFLOAT4X4 m_matWorld;

	float m_fAccTime;
};
//----------------------------------------------------------------
#endif //_SoD3DModelFbx_h_
//----------------------------------------------------------------
