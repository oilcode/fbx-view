//----------------------------------------------------------------
#ifndef _SoD3DSystem_h_
#define _SoD3DSystem_h_
//----------------------------------------------------------------
#include "SoD3DInclude.h"
//----------------------------------------------------------------
class SoD3DSystem
{
public:
	static bool CreateD3DSystem();
	static void ReleaseD3DSystem();
	static SoD3DSystem* Get();

	void BeginRender();
	void EndRender();
	void ClearDepthStencil();

	ID3D11Device* GetD3DDevice();
	ID3D11DeviceContext* GetD3DDeviceContext();

private:
	SoD3DSystem();
	~SoD3DSystem();
	bool InitD3DSystem();
	void ClearD3DSystem();

private:
	static SoD3DSystem* ms_pInstance;
	ID3D11Device* m_pD3DDevice;
	ID3D11DeviceContext* m_pImmediateContext;
	IDXGISwapChain* m_pSwapChain;
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11Texture2D* m_pDepthStencilBuffer;
	ID3D11DepthStencilView* m_pDepthStencilView;
};
//----------------------------------------------------------------
inline SoD3DSystem* SoD3DSystem::Get()
{
	return ms_pInstance;
}
//----------------------------------------------------------------
inline ID3D11Device* SoD3DSystem::GetD3DDevice()
{
	return m_pD3DDevice;
}
//----------------------------------------------------------------
inline ID3D11DeviceContext* SoD3DSystem::GetD3DDeviceContext()
{
	return m_pImmediateContext;
}
//----------------------------------------------------------------
#endif //_SoD3DSystem_h_
//----------------------------------------------------------------
