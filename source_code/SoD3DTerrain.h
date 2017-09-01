//-----------------------------------------------------------------------------
#ifndef _SoD3DTerrain_h_
#define _SoD3DTerrain_h_
//-----------------------------------------------------------------------------
#include "SoD3DInclude.h"
//-----------------------------------------------------------------------------
class SoD3DTexture;
class SoD3DShaderBase;
//-----------------------------------------------------------------------------
class SoD3DTerrain
{
public:
	struct stVertexType
	{
		XMFLOAT3 kPosition;
		XMFLOAT3 kNormal;
		XMFLOAT4 kColor;
		XMFLOAT2 kTexUV0;
		XMFLOAT2 kTexUV1;
	};

public:
	static bool CreateD3DTerrain();
	static void ReleaseD3DTerrain();
	static SoD3DTerrain* Get();

	void RenderD3DTerrain();

	const stVertexType* GetVertexArray() const;
	const unsigned int* GetIndexArray() const;
	int GetVertexArraySize() const;
	int GetIndexArraySize() const;

private:
	SoD3DTerrain();
	~SoD3DTerrain();
	bool InitD3DTerrain();
	void ClearD3DTerrain();
	//
	bool CreateVertexBuffer();
	void ReleaseVertexBuffer();
	//
	bool CreateIndexBuffer();
	void ReleaseIndexBuffer();
	//
	bool CreateTexture();
	void ReleaseTexture();

private:
	static SoD3DTerrain* ms_pInstance;
	stVertexType* m_pVertexArray;
	unsigned int* m_pIndexArray;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	SoD3DShaderBase* m_pShader;
	SoD3DTexture* m_pTexture;
	SoD3DTexture* m_pTexture2;
	int m_nVertexCount;
	int m_nIndexCount;
};
//-----------------------------------------------------------------------------
inline SoD3DTerrain* SoD3DTerrain::Get()
{
	return ms_pInstance;
}
//-----------------------------------------------------------------------------
inline const SoD3DTerrain::stVertexType* SoD3DTerrain::GetVertexArray() const
{
	return m_pVertexArray;
}
//-----------------------------------------------------------------------------
inline const unsigned int* SoD3DTerrain::GetIndexArray() const
{
	return m_pIndexArray;
}
//-----------------------------------------------------------------------------
inline int SoD3DTerrain::GetVertexArraySize() const
{
	return m_nVertexCount;
}
//-----------------------------------------------------------------------------
inline int SoD3DTerrain::GetIndexArraySize() const
{
	return m_nIndexCount;
}
//-----------------------------------------------------------------------------
#endif //_SoD3DTerrain_h_
//-----------------------------------------------------------------------------
