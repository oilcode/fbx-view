//--------------------------------------------------------------------
//去除编译warning
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif //_CRT_SECURE_NO_WARNINGS
//----------------------------------------------------------------
#include "SoSystemFont.h"
//----------------------------------------------------------------
SoSystemFont::SoSystemFont()
:m_hDrawDC(NULL)
,m_hDrawFont(NULL)
,m_hOldFont(NULL)
,m_hDrawBitmap(NULL)
,m_hOldBitmap(NULL)
,m_pBitData(NULL)
{

}
//----------------------------------------------------------------
SoSystemFont::~SoSystemFont()
{
	ClearSystemFont();
}
//----------------------------------------------------------------
bool SoSystemFont::InitSystemFont(const stSystemFontParam* pParam)
{
	if (pParam == NULL)
	{
		return false;
	}
	if (pParam->nFontSize > SoSystemFont_MaxFontSize)
	{
		return false;
	}

	m_kFontParam = *pParam;
	if (InitDevice() == false)
	{
		ClearSystemFont();
		return false;
	}

	return true;
}
//----------------------------------------------------------------
void SoSystemFont::ClearSystemFont()
{
	m_kFontParam.Clear();
	m_kCurrentCharGlyph.Clear();
	ClearDevice();
}
//----------------------------------------------------------------
const stSystemFontCharGlyphBitData* SoSystemFont::GetCharGlyphBitData(wchar_t Char)
{
	if (m_hDrawDC == NULL)
	{
		return NULL;
	}
	if (Char < SoSystemFont_ASCII_MinValidCharGlyph)
	{
		return NULL;
	}
	if (Char == m_kCurrentCharGlyph.Char)
	{
		return &m_kCurrentCharGlyph;
	}

	wchar_t szDrawText[2];
	szDrawText[0]= Char;
	szDrawText[1]= 0;
	const int nDrawTextLen = 1;

	SIZE kGlyphSize;
	if (GetTextExtentPoint32W(m_hDrawDC, szDrawText, nDrawTextLen, &kGlyphSize) == 0)
	{
		return NULL;
	}

	int nGlyphWidth = kGlyphSize.cx;
	int nGlyphHeight = kGlyphSize.cy;

	//清除旧的字形数据信息
	m_kCurrentCharGlyph.Clear();

	//获取新的字形数据
	if (DrawTextW(m_hDrawDC, szDrawText, 1, &m_kDrawRect, DT_SINGLELINE | DT_NOPREFIX) == 0)
	{
		return NULL;
	}
	//字形数据中的Alpha值总是0，下面进行遍历，如果像素有颜色，则把Alpha值置为0xFF。
	DWORD* pTestBuffer = (DWORD*)m_pBitData;
	for (int y = 0; y < SoSystemFont_MaxFontSize; ++y)
	{
		for (int x = 0; x < SoSystemFont_MaxFontSize; ++x)
		{
			if ((*pTestBuffer) > 0)
			{
				//有像素数据
				*pTestBuffer |= 0xFF000000;
			}
			++pTestBuffer;
		}
	}

	if (m_kFontParam.bItalic)
	{
		const DWORD* pTestBuffer = (DWORD*)m_pBitData;
		int maxX = 0;
		int maxY = 0;
		for (int y = 0; y < SoSystemFont_MaxFontSize; ++y)
		{
			for (int x = 0; x < SoSystemFont_MaxFontSize; ++x)
			{
				if ((*pTestBuffer) > 0)
				{
					//有像素数据
					if (maxX < x)
					{
						maxX = x;
					}
					if (maxY < y)
					{
						maxY = y;
					}
				}
				++pTestBuffer;
			}
		}
		//
		if (nGlyphWidth < maxX)
		{
			nGlyphWidth = maxX;
		}
		if (nGlyphHeight < maxY)
		{
			nGlyphHeight = maxY;
		}
	}

	m_kCurrentCharGlyph.Char = Char;
	m_kCurrentCharGlyph.nGlyphWidth = nGlyphWidth;
	m_kCurrentCharGlyph.nGlyphHeight = nGlyphHeight;
	m_kCurrentCharGlyph.pGlyphBitData = m_pBitData;
	return &m_kCurrentCharGlyph;
}
//----------------------------------------------------------------
const void* SoSystemFont::GetBitData()
{
	return m_pBitData;
}
//----------------------------------------------------------------
const stSystemFontParam& SoSystemFont::GetFontParam() const
{
	return m_kFontParam;
}
//----------------------------------------------------------------
bool SoSystemFont::InitDevice()
{
	m_hDrawDC = CreateCompatibleDC(NULL);
	if (m_hDrawDC == NULL)
	{
		return false;
	}

	m_hDrawFont = CreateFont(m_kFontParam.nFontSize, 0, 0, 0, m_kFontParam.nFontWeight, m_kFontParam.bItalic, m_kFontParam.bUnderline, m_kFontParam.bStrikeOut,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, m_kFontParam.kFontName.GetValue());
	if (m_hDrawFont == NULL)
	{
		return false;
	}

	m_hOldFont = (HFONT)SelectObject(m_hDrawDC, m_hDrawFont);
	if (m_hOldFont == NULL)
	{
		return false;
	}

	BITMAPINFO kBitmapInfo;
	memset(&kBitmapInfo.bmiHeader, 0, sizeof(kBitmapInfo.bmiHeader));
	kBitmapInfo.bmiHeader.biSize = sizeof(kBitmapInfo.bmiHeader);
	kBitmapInfo.bmiHeader.biWidth = SoSystemFont_MaxFontSize;
	//Bitmap的数据存储格式是从左到右，从下到上；
	//这里把Height填写成负数，目的是把数据存储格式变成从左到右，从上到下。
	kBitmapInfo.bmiHeader.biHeight = -SoSystemFont_MaxFontSize;
	kBitmapInfo.bmiHeader.biPlanes = 1;
	kBitmapInfo.bmiHeader.biCompression = BI_RGB;
	kBitmapInfo.bmiHeader.biBitCount = SoSystemFont_BitCount;
	m_hDrawBitmap = CreateDIBSection(m_hDrawDC, &kBitmapInfo, DIB_RGB_COLORS, (void**)&m_pBitData, NULL, 0);
	if (m_hDrawBitmap == NULL)
	{
		return false;
	}

	m_hOldBitmap = (HBITMAP)SelectObject(m_hDrawDC, m_hDrawBitmap);
	if (m_hOldBitmap == NULL)
	{
		return false;
	}

	SetMapMode(m_hDrawDC, MM_TEXT);
	SetTextColor(m_hDrawDC, 0x00FFFFFF); //0xAABBGGRR，A的值必须是0
	SetBkColor(m_hDrawDC, 0x00000000);
	SetTextAlign(m_hDrawDC, TA_TOP);

	m_kDrawRect.left = 0;
	m_kDrawRect.top = 0;
	m_kDrawRect.right = SoSystemFont_MaxFontSize;
	m_kDrawRect.bottom = SoSystemFont_MaxFontSize;
	return true;
}
//----------------------------------------------------------------
void SoSystemFont::ClearDevice()
{
	m_pBitData = NULL;
	if (m_hOldBitmap && m_hDrawDC)
	{
		SelectObject(m_hDrawDC, m_hOldBitmap);
		m_hOldBitmap = NULL;
	}
	if (m_hOldFont && m_hDrawDC)
	{
		SelectObject(m_hDrawDC, m_hOldFont);
		m_hOldFont = NULL;
	}
	if (m_hDrawBitmap)
	{
		DeleteObject(m_hDrawBitmap);
		m_hDrawBitmap = NULL;
	}
	if (m_hDrawFont)
	{
		DeleteObject(m_hDrawFont);
		m_hDrawFont = NULL;
	}
	if (m_hDrawDC)
	{
		DeleteDC(m_hDrawDC);
		m_hDrawDC = NULL;
	}
}
//----------------------------------------------------------------
