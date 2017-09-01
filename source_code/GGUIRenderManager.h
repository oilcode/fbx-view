//----------------------------------------------------------------
#ifndef _GGUIRenderManager_h_
#define _GGUIRenderManager_h_
//----------------------------------------------------------------
#include "SoD3DInclude.h"
//----------------------------------------------------------------
class SoD3DShaderBase;
//----------------------------------------------------------------
struct stUIRenderUnit
{
	float fRectLeft;
	float fRectTop;
	float fRectWidth;
	float fRectHeight;
	float fTexCoordLeft;
	float fTexCoordTop;
	float fTexCoordWidth;
	float fTexCoordHeight;
	void* pTexture; //其值为ID3D11ShaderResourceView*类型。
	float fColorR;
	float fColorG;
	float fColorB;
	float fColorA;
};
//----------------------------------------------------------------
class GGUIRenderManager
{
public:
	static bool CreateUIRenderManager();
	static void ReleaseUIRenderManager();
	static GGUIRenderManager* Get();

	void AddRnederUnit(const stUIRenderUnit* pUIRenderUnit);
	void RenderUIRenderManager();

private:
	GGUIRenderManager();
	~GGUIRenderManager();
	bool InitUIRenderManager();
	void ClearUIRenderManager();
	//
	bool CreateVertexList();
	void ReleaseVertexList();
	bool CreateVertexBuffer();
	void ReleaseVertexBuffer();
	bool CreateIndexBuffer();
	void ReleaseIndexBuffer();
	bool CreateSRVList();
	void ReleaseSRVList();

private:
	struct stVertexType
	{
		XMFLOAT3 kPosition;
		XMFLOAT3 kTexCoordAndTexIndex;
		XMFLOAT4 kColorRGBA;
	};

private:
	static GGUIRenderManager* ms_pInstance;
	stVertexType* m_pVertexList;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	ID3D11ShaderResourceView** m_pSRVList;
	SoD3DShaderBase* m_pShader;
	//记录 m_pVertexList 中的元素个数
	int m_nMaxWindowCount;
	int m_nCurWindowCount;
	//记录 m_pSRVList 中的元素个数
	int m_nMaxSRVCount;
	int m_nCurSRVCount;
	//最近一次执行AddRnederUnit时，贴图序号是多少。
	//本次执行AddRnederUnit时，贴图序号极有可能与上次的贴图序号相同。
	//这样做的目的是提高命中率，减少遍历。
	int m_nLastTextureIndex;
	//绘制当前的 stUIRenderUnit 时，使用的ZValue的值是多少。
	float m_fCurrentRenderOrder;
};
//----------------------------------------------------------------
inline GGUIRenderManager* GGUIRenderManager::Get()
{
	return ms_pInstance;
}
//----------------------------------------------------------------
#endif //_GGUIRenderManager_h_
//----------------------------------------------------------------
