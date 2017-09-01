//----------------------------------------------------------------
#include "SoD3DTextureSystemFont.h"
#include "SoD3DSystem.h"
#include "SoD3DTextureManager.h"
#include "SoSystemFont.h"
#include "SoStringConvert.h"
#include "SoStringHelp.h"
#include "GGUIImagesetManager.h"
#include "GGUIImageset.h"
#include "SoFileBmp.h"
//----------------------------------------------------------------
SoD3DTextureSystemFont* SoD3DTextureSystemFont::ms_pInstance = NULL;
//----------------------------------------------------------------
bool SoD3DTextureSystemFont::CreateD3DTextureSystemFont()
{
	bool br = true;
	if (ms_pInstance == NULL)
	{
		stSystemFontParam kSystemFontParam;
		kSystemFontParam.kFontName = "宋体";
		kSystemFontParam.nFontSize = 16;
		kSystemFontParam.nFontWeight = 400; //FW_BOLD;
		kSystemFontParam.bItalic = false;
		//
		ms_pInstance = new SoD3DTextureSystemFont;
		if (ms_pInstance && ms_pInstance->InitD3DTextureSystemFont(&kSystemFontParam, 100))
		{
			br = true;
		}
		else
		{
			br = false;
		}
	}
	return br;
}
//----------------------------------------------------------------
void SoD3DTextureSystemFont::ReleaseD3DTextureSystemFont()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = NULL;
	}
}
//----------------------------------------------------------------
SoD3DTextureSystemFont::SoD3DTextureSystemFont()
:m_pTexture(NULL)
,m_pSystemFont(NULL)
,m_pBitData(NULL)
,m_nSizeOfBitData(0)
,m_nTextureWidth(0)
,m_nTextureHeight(0)
,m_nGlyphCountInWidth(0)
,m_nGlyphCountInHeight(0)
,m_nImagesetIndex(-1)
,m_bPixelDataChanged(false)
{

}
//----------------------------------------------------------------
SoD3DTextureSystemFont::~SoD3DTextureSystemFont()
{
	ClearD3DTextureSystemFont();
}
//----------------------------------------------------------------
bool SoD3DTextureSystemFont::InitD3DTextureSystemFont(const stSystemFontParam* pParam, const int nMaxGlyphCount)
{
	if (pParam == NULL)
	{
		return false;
	}
	if (SoD3DSystem::Get() == NULL)
	{
		return false;
	}
	ID3D11Device* pD3DDevice = SoD3DSystem::Get()->GetD3DDevice();
	if (pD3DDevice == NULL)
	{
		return false;
	}

	//尝试删除旧的对象。
	ClearD3DTextureSystemFont();

	//创建字体对象。
	m_pSystemFont = new SoSystemFont;
	if (m_pSystemFont->InitSystemFont(pParam) == false)
	{
		ClearD3DTextureSystemFont();
		return false;
	}

	//计算贴图的宽高，宽高必须是2的整数幂。
	GenerateTextureResolution(pParam->nFontSize, nMaxGlyphCount);

	//创建像素数据内存块。
	m_nSizeOfBitData = m_nTextureWidth * m_nTextureHeight * SoSystemFont_BytePerPixel;
	m_pBitData = new char[m_nSizeOfBitData];
	if (m_pBitData == NULL)
	{
		ClearD3DTextureSystemFont();
		return false;
	}
	memset(m_pBitData, 0, m_nSizeOfBitData);

	if (DoCreateTexture() == false)
	{
		ClearD3DTextureSystemFont();
		return false;
	}

	if (DoCreateImageset() == false)
	{
		ClearD3DTextureSystemFont();
		return false;
	}

	return true;
}
//----------------------------------------------------------------
void SoD3DTextureSystemFont::ClearD3DTextureSystemFont()
{
	DoReleaseTexture();
	if (m_pSystemFont)
	{
		delete m_pSystemFont;
		m_pSystemFont = NULL;
	}
	if (m_pBitData)
	{
		delete [] m_pBitData;
		m_pBitData = 0;
	}
	m_nSizeOfBitData = 0;
	m_nTextureWidth = 0;
	m_nTextureHeight = 0;
	m_nGlyphCountInWidth = 0;
	m_nGlyphCountInHeight = 0;
	DoReleaseImageset();
	m_bPixelDataChanged = false;
}
//----------------------------------------------------------------
void SoD3DTextureSystemFont::PreRenderD3DTextureSystemFont()
{
	if (m_bPixelDataChanged)
	{
		m_bPixelDataChanged = false;
		//
		if (m_pTexture && m_pBitData)
		{
			m_pTexture->SetDynamicTexturePixelData(m_pBitData, m_nSizeOfBitData);
			//
			//stFileBmpBaseInfo kInfo;
			//kInfo.nPictureWidth = m_nTextureWidth;
			//kInfo.nPictureHeight = m_nTextureHeight;
			//kInfo.ePixelFormat = FileBmpPixelFormat_ARGB;
			//kInfo.eCoordSysType = FileBmpDataCoordSystem_Left2Right_Up2Down;
			//kInfo.nDataSize = m_nSizeOfBitData;
			//kInfo.pData = m_pBitData;
			//SoFileBmp kBmp;
			//kBmp.WriteBmpFile("TextureSystemFontnew.bmp", &kInfo);
		}
	}
}
//----------------------------------------------------------------
void SoD3DTextureSystemFont::DrawString(const wchar_t* szString, int nLen)
{
	if (szString == 0 || szString[0] == 0)
	{
		return;
	}
	if (m_pTexture == 0 || m_pSystemFont == 0)
	{
		return;
	}
	if (GGUIImagesetManager::Get() == 0)
	{
		return;
	}
	GGUIImageset* pImageset = GGUIImagesetManager::Get()->GetImagesetByID(m_nImagesetIndex);
	if (pImageset == 0)
	{
		return;
	}
	//
	int nWCharCount = nLen;
	if (nLen <= 0)
	{
		nWCharCount = SoWStrLen(szString);
	}
	//
	SoTinyString kRectName;
	wchar_t wRectName[2] = {0};
	for (int i = 0; i < nWCharCount; ++i)
	{
		wRectName[0] = szString[i];
		kRectName = (char*)wRectName;
		const int nRectIndex = pImageset->GetRectID(kRectName);
		if (nRectIndex != -1)
		{
			continue;
		}
		const stSystemFontCharGlyphBitData* pNewData = m_pSystemFont->GetCharGlyphBitData(szString[i]);
		if (pNewData == NULL)
		{
			continue;
		}
		if (pNewData->nGlyphWidth <= 0 || pNewData->nGlyphHeight <= 0)
		{
			continue;
		}
		//
		CopyGlyphData(pNewData, pImageset, kRectName);
		m_bPixelDataChanged = true;
	}
}
//----------------------------------------------------------------
bool SoD3DTextureSystemFont::DoCreateTexture()
{
	if (SoD3DTextureManager::Get() == NULL)
	{
		return false;
	}

	m_pTexture = SoD3DTextureManager::Get()->CreateUITextureDynamic(m_nTextureWidth, m_nTextureHeight, m_pBitData);
	if (m_pTexture == NULL)
	{
		return false;
	}

	m_pTexture->AddRefTexture();
	return true;
}
//----------------------------------------------------------------
void SoD3DTextureSystemFont::DoReleaseTexture()
{
	if (m_pTexture)
	{
		m_pTexture->RemoveRefTexture();
		m_pTexture = NULL;
	}
}
//----------------------------------------------------------------
bool SoD3DTextureSystemFont::DoCreateImageset()
{
	if (GGUIImagesetManager::Get() == NULL)
	{
		return false;
	}

	GenerateImagesetName();
	int nImagesetIndex = GGUIImagesetManager::Get()->GetImagesetIndex(m_kImagesetName);
	if (nImagesetIndex != -1)
	{
		return false;
	}

	stImagesetParam kParam;
	kParam.kName = m_kImagesetName;
	kParam.nInitRectCount = m_nGlyphCountInWidth * m_nGlyphCountInHeight;
	kParam.pD3DTexture = m_pTexture;
	m_nImagesetIndex = GGUIImagesetManager::Get()->CreateImageset(kParam);
	if (m_nImagesetIndex == -1)
	{
		return false;
	}

	return true;
}
//----------------------------------------------------------------
void SoD3DTextureSystemFont::DoReleaseImageset()
{
	//这里没有对Imageset对象进行释放。目前不需要释放。
	m_kImagesetName.Clear();
	m_nImagesetIndex = -1;
}
//----------------------------------------------------------------
void SoD3DTextureSystemFont::GenerateImagesetName()
{
	if (m_pSystemFont == NULL)
	{
		return;
	}

	const stSystemFontParam& kFontParam = m_pSystemFont->GetFontParam();
	//SoStrFmtSelf(m_kImagesetName.GetBuffer(), MaxCharCount_TinyString, "%s%d%d%d%d%d",
	//	kFontParam.kFontName.GetValue(), kFontParam.nFontSize, kFontParam.nFontWeight, 
	//	kFontParam.bItalic, kFontParam.bUnderline, kFontParam.bStrikeOut);
	SoStrFmtSelf(m_kImagesetName.GetBuffer(), MaxCharCount_TinyString, "%s", kFontParam.kFontName.GetValue());
}
//----------------------------------------------------------------
void SoD3DTextureSystemFont::GenerateTextureResolution(const int nFontSize, const int nMaxGlyphCount)
{
	//贴图分辨率列表。宽高必须是2的整数幂。
	const int TextureResolutionList[] = 
	{
		256, 256,
		256, 512,
		512, 512,
		512, 1024,
		1024, 1024,
		1024, 2048,
	};

	const int nResolutionCount = sizeof(TextureResolutionList) / sizeof(int) / 2;
	const int nGlyphWidthWithEdge = nFontSize + 1; //字形之间的间隙是1像素
	const int nGlyphHeightWithEdge = nFontSize + 1; //字形之间的间隙是1像素
	int nTextureWidth = 0;
	int nTextureHeight = 0;
	int nGlyphCountInWidth = 0;
	int nGlyphCountInHeight = 0;
	for (int i = 0; i < nResolutionCount; ++i)
	{
		nTextureWidth = TextureResolutionList[i*2];
		nTextureHeight = TextureResolutionList[i*2+1];
		nGlyphCountInWidth = nTextureWidth / nGlyphWidthWithEdge;
		nGlyphCountInHeight = nTextureHeight / nGlyphHeightWithEdge;
		if (nGlyphCountInWidth * nGlyphCountInHeight >= nMaxGlyphCount)
		{
			break;
		}
	}
	m_nTextureWidth = nTextureWidth;
	m_nTextureHeight = nTextureHeight;
	m_nGlyphCountInWidth = nGlyphCountInWidth;
	m_nGlyphCountInHeight = nGlyphCountInHeight;
}
//----------------------------------------------------------------
void SoD3DTextureSystemFont::CopyGlyphData(const stSystemFontCharGlyphBitData* pGlyphData, GGUIImageset* pImageset, const SoTinyString& kRectName)
{
	//已有的字形，已经填充到了第几行第几列。
	const int nRectCount = pImageset->GetRectCount();
	const int nIndexX = nRectCount % m_nGlyphCountInWidth;
	const int nIndexY = nRectCount / m_nGlyphCountInWidth;
	//计算新字形填充的位置。
	const int nFontSize = m_pSystemFont->GetFontParam().nFontSize;
	const int nBytePitch = m_nTextureWidth * SoSystemFont_BytePerPixel;
	const int nPixelOffset_X = nIndexX * (nFontSize + 1);
	const int nPixelOffset_Y = nIndexY * (nFontSize + 1);
	const int nByteOffset = nPixelOffset_Y * nBytePitch + nPixelOffset_X * SoSystemFont_BytePerPixel;
	const int nGlyphBufferBytePitch = SoSystemFont_MaxFontSize * SoSystemFont_BytePerPixel;
	const int nGlyphBytePitch = pGlyphData->nGlyphWidth * SoSystemFont_BytePerPixel;
	//
	char* pDestChar = m_pBitData + nByteOffset;
	char* pSrcChar = pGlyphData->pGlyphBitData;
	for (int y = 0; y < pGlyphData->nGlyphHeight; ++y)
	{
		for (int x = 0; x < nGlyphBytePitch; ++x)
		{
			pDestChar[x] = pSrcChar[x];
		}
		pDestChar += nBytePitch;
		pSrcChar += nGlyphBufferBytePitch;
	}
	//
	GGUIRect kRect;
	kRect.x = (float)nPixelOffset_X / (float)m_nTextureWidth;
	kRect.y = (float)nPixelOffset_Y / (float)m_nTextureHeight;
	kRect.w = (float)pGlyphData->nGlyphWidth / (float)m_nTextureWidth;
	kRect.h = (float)pGlyphData->nGlyphHeight / (float)m_nTextureHeight;
	pImageset->AddRect(kRectName, kRect);
}
//----------------------------------------------------------------
