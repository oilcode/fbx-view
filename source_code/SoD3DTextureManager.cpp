//----------------------------------------------------------------
//1，本管理器把贴图分成了两类，一类是UI界面使用的贴图，另一类是各类模型使用的贴图，
//   查找UI贴图就从UI类型里面去查找，查找模型贴图就从模型类型里面去查找，效率会高一点。
//2，外界使用一个贴图对象，就增加该贴图对象的引用计数，不再使用时就释放引用计数。
//   本管理器会把引用计数为0的贴图删除掉。
//----------------------------------------------------------------
#include "SoD3DTextureManager.h"
//----------------------------------------------------------------
SoD3DTextureManager* SoD3DTextureManager::ms_pInstance = NULL;
//----------------------------------------------------------------
bool SoD3DTextureManager::CreateD3DTextureManager()
{
	bool br = true;
	if (ms_pInstance == NULL)
	{
		ms_pInstance = new SoD3DTextureManager;
		if (ms_pInstance && ms_pInstance->InitD3DTextureManager())
		{
			br = true;
		}
		else
		{
			ReleaseD3DTextureManager();
			br = false;
		}
	}
	return br;
}
//----------------------------------------------------------------
void SoD3DTextureManager::ReleaseD3DTextureManager()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = NULL;
	}
}
//----------------------------------------------------------------
SoD3DTextureManager::SoD3DTextureManager()
:m_fAccTimeForGC(0.0f)
{

}
//----------------------------------------------------------------
SoD3DTextureManager::~SoD3DTextureManager()
{
	ClearD3DTextureManager();
}
//----------------------------------------------------------------
bool SoD3DTextureManager::InitD3DTextureManager()
{
	if (m_kUITextureArray.InitArray(sizeof(SoD3DTexture*), 10, 10) == false)
	{
		return false;
	}
	if (m_kModelTextureArray.InitArray(sizeof(SoD3DTexture*), 10, 10) == false)
	{
		return false;
	}
	return true;
}
//----------------------------------------------------------------
void SoD3DTextureManager::ClearD3DTextureManager()
{
	const int nUITexCount = m_kUITextureArray.GetCapacity();
	SoD3DTexture* pTexture = NULL;
	for (int i = 0; i < nUITexCount; ++i)
	{
		pTexture = GetUITextureByID(i);
		if (pTexture)
		{
			delete pTexture;
		}
	}
	m_kUITextureArray.ClearArray();
	//
	const int nModelTexCount = m_kModelTextureArray.GetCapacity();
	pTexture = NULL;
	for (int i = 0; i < nModelTexCount; ++i)
	{
		pTexture = GetTextureByID(i);
		if (pTexture)
		{
			delete pTexture;
		}
	}
	m_kModelTextureArray.ClearArray();
	m_kModelTexName2Index.clear();
}
//----------------------------------------------------------------
void SoD3DTextureManager::UpdateD3DTextureManager(float fDeltaTime)
{
	m_fAccTimeForGC += fDeltaTime;
	if (m_fAccTimeForGC > 1.0f)
	{
		m_fAccTimeForGC = 0.0f;
		ProcessGC();
	}
}
//----------------------------------------------------------------
void SoD3DTextureManager::ProcessGC()
{
	//未完待续
}
//----------------------------------------------------------------
SoD3DTexture* SoD3DTextureManager::CreateUITextureFromFile(const char* szTexName)
{
	if (szTexName == 0 || szTexName[0] == 0)
	{
		return NULL;
	}

	SoD3DTexture::stInitParamFromFile kParam;
	kParam.szFileName = szTexName;
	kParam.kLoadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	kParam.kLoadInfo.Format = DXGI_FORMAT_BC1_UNORM; //DXGI_FORMAT_UNKNOWN; //DXGI_FORMAT_BC3_UNORM; //DXGI_FORMAT_R8G8B8A8_TYPELESS; //DXGI_FORMAT_BC3_TYPELESS; //
	kParam.kLoadInfo.MipLevels = 1;
	kParam.kLoadInfo.MipFilter = D3DX11_FILTER_LINEAR;

	SoD3DTexture* pTexture = new SoD3DTexture;
	if (pTexture == NULL)
	{
		return NULL;
	}

	if (pTexture->InitD3DTextureFromFile(kParam) == false)
	{
		delete pTexture;
		pTexture = NULL;
		return NULL;
	}

	const int nTextureID = m_kUITextureArray.FillAt(-1, &pTexture);
	pTexture->SetTextureID(nTextureID);
	return pTexture;
}
//----------------------------------------------------------------
SoD3DTexture* SoD3DTextureManager::CreateUITextureDynamic(int nWidth, int nHeight, void* pPixelData)
{
	SoD3DTexture::stInitParamDynamic kParam;

	kParam.kTexture2DDesc.Width = nWidth;
	kParam.kTexture2DDesc.Height = nHeight;
	kParam.kTexture2DDesc.MipLevels = 1;
	kParam.kTexture2DDesc.ArraySize = 1;
	kParam.kTexture2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //DXGI_FORMAT_R8G8B8A8_UINT; //DXGI_FORMAT_R16G16_FLOAT
	kParam.kTexture2DDesc.SampleDesc.Count = 1;
	kParam.kTexture2DDesc.SampleDesc.Quality = 0;
	kParam.kTexture2DDesc.Usage = D3D11_USAGE_DYNAMIC;
	kParam.kTexture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	kParam.kTexture2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	kParam.kTexture2DDesc.MiscFlags = 0;

	const int nBytePerPixel = 4;
	kParam.kResourceData.pSysMem = pPixelData;
	kParam.kResourceData.SysMemPitch = nWidth * nBytePerPixel;
	kParam.kResourceData.SysMemSlicePitch = 0;

	SoD3DTexture* pTexture = new SoD3DTexture;
	if (pTexture == NULL)
	{
		return NULL;
	}

	if (pTexture->InitD3DTextureDynamic(kParam) == false)
	{
		delete pTexture;
		pTexture = NULL;
		return NULL;
	}

	const int nTextureID = m_kUITextureArray.FillAt(-1, &pTexture);
	pTexture->SetTextureID(nTextureID);
	return pTexture;
}
//----------------------------------------------------------------
SoD3DTexture* SoD3DTextureManager::GetUITextureByID(int nTextureID)
{
	void* pElement = m_kUITextureArray.GetAt(nTextureID);
	if (pElement)
	{
		return (*((SoD3DTexture**)pElement));
	}
	else
	{
		return NULL;
	}
}
//----------------------------------------------------------------
SoD3DTexture* SoD3DTextureManager::CreateTextureFromFile(const std::string& strTexName)
{
	if (strTexName.empty() == true)
	{
		return NULL;
	}

	const int nExistTexID = GetTextureIDByName(strTexName);
	if (nExistTexID != -1)
	{
		return GetTextureByID(nExistTexID);
	}

	SoD3DTexture::stInitParamFromFile kParam;
	kParam.szFileName = strTexName.c_str();
	kParam.kLoadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	kParam.kLoadInfo.Format = DXGI_FORMAT_BC1_UNORM; //DXGI_FORMAT_UNKNOWN; //DXGI_FORMAT_BC3_UNORM; //DXGI_FORMAT_R8G8B8A8_TYPELESS; //DXGI_FORMAT_BC3_TYPELESS; //
	kParam.kLoadInfo.MipLevels = 0;
	kParam.kLoadInfo.MipFilter = D3DX11_FILTER_LINEAR;

	SoD3DTexture* pTexture = new SoD3DTexture;
	if (pTexture == NULL)
	{
		return NULL;
	}

	if (pTexture->InitD3DTextureFromFile(kParam) == false)
	{
		delete pTexture;
		pTexture = NULL;
		return NULL;
	}

	const int nTextureID = m_kModelTextureArray.FillAt(-1, &pTexture);
	pTexture->SetTextureID(nTextureID);
	m_kModelTexName2Index.insert(std::make_pair(strTexName, nTextureID));
	return pTexture;
}
//----------------------------------------------------------------
SoD3DTexture* SoD3DTextureManager::CreateAnimDataTextureDynamic(int nWidth, int nHeight, void* pPixelData)
{
	const int nBytePerPixel = 4;

	SoD3DTexture::stInitParamDynamic kParam;

	kParam.kTexture2DDesc.Width = nWidth;
	kParam.kTexture2DDesc.Height = nHeight;
	kParam.kTexture2DDesc.MipLevels = 1;
	kParam.kTexture2DDesc.ArraySize = 1;
	kParam.kTexture2DDesc.Format = DXGI_FORMAT_R32_FLOAT; //每个像素是一个32位的float值
	kParam.kTexture2DDesc.SampleDesc.Count = 1;
	kParam.kTexture2DDesc.SampleDesc.Quality = 0;
	kParam.kTexture2DDesc.Usage = D3D11_USAGE_DYNAMIC;
	kParam.kTexture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	kParam.kTexture2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	kParam.kTexture2DDesc.MiscFlags = 0;


	kParam.kResourceData.pSysMem = pPixelData;
	kParam.kResourceData.SysMemPitch = nWidth * nBytePerPixel;
	kParam.kResourceData.SysMemSlicePitch = 0;

	SoD3DTexture* pTexture = new SoD3DTexture;
	if (pTexture == NULL)
	{
		return NULL;
	}

	if (pTexture->InitD3DTextureDynamic(kParam) == false)
	{
		delete pTexture;
		pTexture = NULL;
		return NULL;
	}

	const int nTextureID = m_kUITextureArray.FillAt(-1, &pTexture);
	pTexture->SetTextureID(nTextureID);
	return pTexture;
}
//----------------------------------------------------------------
SoD3DTexture* SoD3DTextureManager::GetTextureByID(int nTextureID)
{
	void* pElement = m_kModelTextureArray.GetAt(nTextureID);
	if (pElement)
	{
		return (*((SoD3DTexture**)pElement));
	}
	else
	{
		return NULL;
	}
}
//----------------------------------------------------------------
int SoD3DTextureManager::GetTextureIDByName(const std::string& strTexName)
{
	mapName2Index::iterator it = m_kModelTexName2Index.find(strTexName);
	if (it != m_kModelTexName2Index.end())
	{
		return it->second;
	}
	else
	{
		return -1;
	}
}
//----------------------------------------------------------------
