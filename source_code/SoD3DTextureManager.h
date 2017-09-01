//----------------------------------------------------------------
#ifndef _SoD3DTextureManager_h_
#define _SoD3DTextureManager_h_
//----------------------------------------------------------------
#include "SoD3DTexture.h"
//----------------------------------------------------------------
class SoD3DTextureManager
{
public:
	static bool CreateD3DTextureManager();
	static void ReleaseD3DTextureManager();
	static SoD3DTextureManager* Get();

	void UpdateD3DTextureManager(float fDeltaTime);

	SoD3DTexture* CreateUITextureFromFile(const char* szTexName);
	SoD3DTexture* CreateUITextureDynamic(int nWidth, int nHeight, void* pPixelData);
	SoD3DTexture* GetUITextureByID(int nTextureID);

	SoD3DTexture* CreateTextureFromFile(const std::string& strTexName);
	SoD3DTexture* CreateAnimDataTextureDynamic(int nWidth, int nHeight, void* pPixelData);
	SoD3DTexture* GetTextureByID(int nTextureID);
	int GetTextureIDByName(const std::string& strTexName);

private:
	SoD3DTextureManager();
	~SoD3DTextureManager();
	bool InitD3DTextureManager();
	void ClearD3DTextureManager();
	void ProcessGC();

private:
	typedef std::map<std::string, int> mapName2Index;

private:
	static SoD3DTextureManager* ms_pInstance;
	SoArrayUID m_kUITextureArray;
	SoArrayUID m_kModelTextureArray;
	mapName2Index m_kModelTexName2Index;

	float m_fAccTimeForGC;
};
//----------------------------------------------------------------
inline SoD3DTextureManager* SoD3DTextureManager::Get()
{
	return ms_pInstance;
}
//----------------------------------------------------------------
#endif //_SoD3DTextureManager_h_
//----------------------------------------------------------------
