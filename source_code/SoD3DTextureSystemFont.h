//----------------------------------------------------------------
#ifndef _SoD3DTextureSystemFont_h_
#define _SoD3DTextureSystemFont_h_
//----------------------------------------------------------------
#include "SoD3DInclude.h"
//----------------------------------------------------------------
class SoD3DTexture;
class SoSystemFont;
class GGUIImageset;
struct stSystemFontParam;
struct stSystemFontCharGlyphBitData;
//----------------------------------------------------------------
class SoD3DTextureSystemFont
{
public:
	static bool CreateD3DTextureSystemFont();
	static void ReleaseD3DTextureSystemFont();
	static SoD3DTextureSystemFont* Get();

	SoD3DTextureSystemFont();
	~SoD3DTextureSystemFont();

	bool InitD3DTextureSystemFont(const stSystemFontParam* pParam, const int nMaxGlyphCount);
	void ClearD3DTextureSystemFont();
	void PreRenderD3DTextureSystemFont();

	//绘制字形，并添加到Imageset中。
	//--szString 如果字符串没有以0结尾，则nLen必须为有效值。
	//--nLen 字符个数。可以为0或者-1等无效值；当为无效值时，szString必须为以0结尾的字符串。
	void DrawString(const wchar_t* szString, int nLen);

private:
	bool DoCreateTexture();
	void DoReleaseTexture();
	bool DoCreateImageset();
	void DoReleaseImageset();
	void GenerateImagesetName();
	void GenerateTextureResolution(const int nFontSize, const int nDestGlyphCount);
	void CopyGlyphData(const stSystemFontCharGlyphBitData* pGlyphData, GGUIImageset* pImageset, const SoTinyString& kRectName);

private:
	static SoD3DTextureSystemFont* ms_pInstance;
	SoD3DTexture* m_pTexture;
	SoSystemFont* m_pSystemFont;
	char* m_pBitData;
	int m_nSizeOfBitData;
	int m_nTextureWidth;
	int m_nTextureHeight;
	int m_nGlyphCountInWidth;
	int m_nGlyphCountInHeight;
	//字形贴图会被包装成Imageset，其名字保存在这里。
	SoTinyString m_kImagesetName;
	int m_nImagesetIndex;
	bool m_bPixelDataChanged;
};
//----------------------------------------------------------------
inline SoD3DTextureSystemFont* SoD3DTextureSystemFont::Get()
{
	return ms_pInstance;
}
//----------------------------------------------------------------
#endif //_SoD3DTextureSystemFont_h_
//----------------------------------------------------------------
