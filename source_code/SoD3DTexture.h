//----------------------------------------------------------------
#ifndef _SoD3DTexture_h_
#define _SoD3DTexture_h_
//----------------------------------------------------------------
#include "SoD3DInclude.h"
//----------------------------------------------------------------
class SoD3DTexture
{
public:
	bool SetDynamicTexturePixelData(void* pPixelData, int nSizeOfPixelData);
	bool GetTexturePixelData(void* pBuff, int nSizeOfBuff);

	ID3D11Texture2D* GetTexture2D() const;
	ID3D11ShaderResourceView* GetTextureSRV() const;
	int GetTextureID() const;
	int GetTextureWidth() const;
	int GetTextureHeight() const;
	
	void AddRefTexture();
	void RemoveRefTexture();
	int GetRefTexture() const;

protected:
	friend class SoD3DTextureManager;

	struct stInitParamFromFile
	{
		const char* szFileName;
		D3DX11_IMAGE_LOAD_INFO kLoadInfo;
	};
	struct stInitParamDynamic
	{
		D3D11_TEXTURE2D_DESC kTexture2DDesc;
		D3D11_SUBRESOURCE_DATA kResourceData;
	};

protected:
	SoD3DTexture();
	~SoD3DTexture();
	bool InitD3DTextureFromFile(stInitParamFromFile& kParam);
	bool InitD3DTextureDynamic(stInitParamDynamic& kParam);
	void SetTextureID(int nID);
	void ClearD3DTexture();

private:
	ID3D11Texture2D* m_pTexture2D;
	ID3D11ShaderResourceView* m_pTextureSRV;
	int m_nTextureID;
	int m_nTextureWidth;
	int m_nTextureHeight;
	int m_nRefCount;
};
//----------------------------------------------------------------
inline ID3D11Texture2D* SoD3DTexture::GetTexture2D() const
{
	return m_pTexture2D;
}
//----------------------------------------------------------------
inline ID3D11ShaderResourceView* SoD3DTexture::GetTextureSRV() const
{
	return m_pTextureSRV;
}
//----------------------------------------------------------------
inline void SoD3DTexture::SetTextureID(int nID)
{
	m_nTextureID = nID;
}
//----------------------------------------------------------------
inline int SoD3DTexture::GetTextureID() const
{
	return m_nTextureID;
}
//----------------------------------------------------------------
inline int SoD3DTexture::GetTextureWidth() const
{
	return m_nTextureWidth;
}
//----------------------------------------------------------------
inline int SoD3DTexture::GetTextureHeight() const
{
	return m_nTextureHeight;
}
//----------------------------------------------------------------
inline void SoD3DTexture::AddRefTexture()
{
	++m_nRefCount;
}
//----------------------------------------------------------------
inline void SoD3DTexture::RemoveRefTexture()
{
	--m_nRefCount;
}
//----------------------------------------------------------------
inline int SoD3DTexture::GetRefTexture() const
{
	return m_nRefCount;
}
//----------------------------------------------------------------
#endif //_SoD3DTexture_h_
//----------------------------------------------------------------
