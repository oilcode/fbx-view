//----------------------------------------------------------------
#include "SoD3DSystem.h"
//----------------------------------------------------------------
SoD3DSystem* SoD3DSystem::ms_pInstance = 0;
//----------------------------------------------------------------
bool SoD3DSystem::CreateD3DSystem()
{
	bool br = true;
	if (ms_pInstance == 0)
	{
		ms_pInstance = new SoD3DSystem;
		if (ms_pInstance && ms_pInstance->InitD3DSystem())
		{
			br = true;
		}
		else
		{
			ReleaseD3DSystem();
			br = false;
		}
	}
	return br;
}
//----------------------------------------------------------------
void SoD3DSystem::ReleaseD3DSystem()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = 0;
	}
}
//----------------------------------------------------------------
SoD3DSystem::SoD3DSystem()
:m_pD3DDevice(0)
,m_pImmediateContext(0)
,m_pSwapChain(0)
,m_pRenderTargetView(0)
,m_pDepthStencilBuffer(0)
,m_pDepthStencilView(0)
{

}
//----------------------------------------------------------------
SoD3DSystem::~SoD3DSystem()
{
	ClearD3DSystem();
}
//----------------------------------------------------------------
bool SoD3DSystem::InitD3DSystem()
{
	const UINT width = WinApp::Get()->GetClientWidth();
	const UINT height = WinApp::Get()->GetClientHeight();
	const HWND hWnd = WinApp::Get()->GetHWND();
	//multisampling mode.
	//if you wanted to change the multisampling settings at runtime, 
	//you would have to destory and recreate the swap chain.
	const bool bEnable4xMsaa = true;

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//Create D3DDevice
	IDXGIAdapter* pAdapter = NULL; //use the primary display adapter.
	const UINT numDriverTypes = 3;
	D3D_DRIVER_TYPE driverTypes[3] = {D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE,};
	HMODULE Software = NULL; //we always use hardware for rendering.
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	const UINT numFeatureLevels = 3;
	D3D_FEATURE_LEVEL featureLevels[3] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,};
	D3D_FEATURE_LEVEL resultFeatureLevel = D3D_FEATURE_LEVEL_11_0; //记录D3D创建成功后，使用的是哪个FeatureLevel。
	HRESULT hr = S_OK;
	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex)
	{
		hr = D3D11CreateDevice(pAdapter, driverTypes[driverTypeIndex], Software, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &m_pD3DDevice, &resultFeatureLevel, &m_pImmediateContext);
		if (SUCCEEDED(hr))
		{
			break;
		}
	}
	if (FAILED(hr))
	{
		return false;
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//Create SwapChain
	const DXGI_FORMAT Format = DXGI_FORMAT_R8G8B8A8_UNORM; //Each element has four 8-bit unsigned components mapped to the [0,1] range.
	UINT uiMultisampleCount = 1;
	UINT ui4xMsaaQuality = 0;
	if (bEnable4xMsaa)
	{
		//Check 4X MSAA quality support, [0, ui4xMsaaQuality)
		uiMultisampleCount = 4;
		m_pD3DDevice->CheckMultisampleQualityLevels(Format, uiMultisampleCount, &ui4xMsaaQuality);
		if (ui4xMsaaQuality > 0)
		{
			ui4xMsaaQuality -= 1;
		}
	}

	DXGI_SWAP_CHAIN_DESC kSwapChainDesc;
	memset(&kSwapChainDesc, 0, sizeof(kSwapChainDesc));
	kSwapChainDesc.BufferDesc.Width = width;
	kSwapChainDesc.BufferDesc.Height = height;
	kSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	kSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	kSwapChainDesc.BufferDesc.Format = Format;
	kSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	kSwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	kSwapChainDesc.SampleDesc.Count = uiMultisampleCount;
	kSwapChainDesc.SampleDesc.Quality = ui4xMsaaQuality;
	kSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	kSwapChainDesc.BufferCount = 1;
	kSwapChainDesc.OutputWindow = hWnd;
	kSwapChainDesc.Windowed = TRUE;
	kSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	kSwapChainDesc.Flags = 0;

	IDXGIDevice* dxgiDevice = NULL;
	IDXGIAdapter* dxgiAdapter = NULL;
	IDXGIFactory* dxgiFactory = NULL;
	hr = m_pD3DDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	if (SUCCEEDED(hr))
	{
		hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
		if (SUCCEEDED(hr))
		{
			hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
			if (SUCCEEDED(hr))
			{
				hr = dxgiFactory->CreateSwapChain(m_pD3DDevice, &kSwapChainDesc, &m_pSwapChain);
			}
		}
	}
	if (dxgiDevice)
	{
		dxgiDevice->Release();
		dxgiDevice = NULL;
	}
	if (dxgiAdapter)
	{
		dxgiAdapter->Release();
		dxgiAdapter = NULL;
	}
	if (dxgiFactory)
	{
		dxgiFactory->Release();
		dxgiFactory = NULL;
	}
	if (FAILED(hr))
	{
		return false;
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//Create RenderTargetView
	//获取后台缓冲区
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	if (FAILED(hr))
	{
		return false;
	}
	//创建一个与后台缓冲区绑定的RenderTargetView
	hr = m_pD3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
	pBackBuffer->Release();
	pBackBuffer = NULL;
	if (FAILED(hr))
	{
		return false;
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//Create Depth/Stencil buffer and view
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = uiMultisampleCount;
	depthStencilDesc.SampleDesc.Quality = ui4xMsaaQuality;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	hr = m_pD3DDevice->CreateTexture2D(&depthStencilDesc, 0, &m_pDepthStencilBuffer);
	if (FAILED(hr))
	{
		return false;
	}
	hr = m_pD3DDevice->CreateDepthStencilView(m_pDepthStencilBuffer, 0, &m_pDepthStencilView);
	if (FAILED(hr))
	{
		return false;
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
	// Setup the viewport
	D3D11_VIEWPORT kViewPort;
	kViewPort.TopLeftX = 0.0f;
	kViewPort.TopLeftY = 0.0f;
	kViewPort.Width = (FLOAT)width;
	kViewPort.Height = (FLOAT)height;
	kViewPort.MinDepth = 0.0f;
	kViewPort.MaxDepth = 1.0f;
	m_pImmediateContext->RSSetViewports(1, &kViewPort);

	return true;
}
//----------------------------------------------------------------
void SoD3DSystem::ClearD3DSystem()
{
	if (m_pImmediateContext)
	{
		m_pImmediateContext->ClearState();
	}
	if (m_pSwapChain)
	{
		m_pSwapChain->SetFullscreenState(FALSE, NULL);
	}
	//
	if (m_pDepthStencilView)
	{
		m_pDepthStencilView->Release();
		m_pDepthStencilView = 0;
	}
	if (m_pDepthStencilBuffer)
	{
		m_pDepthStencilBuffer->Release();
		m_pDepthStencilBuffer = 0;
	}
	if (m_pRenderTargetView)
	{
		m_pRenderTargetView->Release();
		m_pRenderTargetView = 0;
	}
	if (m_pImmediateContext)
	{
		m_pImmediateContext->Release();
		m_pImmediateContext = 0;
	}
	if (m_pD3DDevice)
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = 0;
	}
	if (m_pSwapChain)
	{
		m_pSwapChain->Release();
		m_pSwapChain = 0;
	}
}
//----------------------------------------------------------------
void SoD3DSystem::BeginRender()
{
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, SoD3D_BG_ClearColor);
	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, SoD3D_DepthValue_Max, 0);
}
//----------------------------------------------------------------
void SoD3DSystem::EndRender()
{
	m_pSwapChain->Present(0, 0);
}
//----------------------------------------------------------------
void SoD3DSystem::ClearDepthStencil()
{
	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, SoD3D_DepthValue_Max, 0);
}
//----------------------------------------------------------------
