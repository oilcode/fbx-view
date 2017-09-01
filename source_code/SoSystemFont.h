//----------------------------------------------------------------
#ifndef _SoSystemFont_h_
#define _SoSystemFont_h_
//----------------------------------------------------------------
#include <Windows.h>
#include "SoTinyString.h"
//----------------------------------------------------------------
#define SoSystemFont_MaxFontSize 64
//颜色位深，32位真彩色
#define SoSystemFont_BitCount 32
//每个像素占用几个字节
#define SoSystemFont_BytePerPixel 4
//在ascii字符表中，从第32个符号才开始有有效的字形
#define SoSystemFont_ASCII_MinValidCharGlyph 32
//----------------------------------------------------------------
struct stSystemFontParam
{
	SoTinyString kFontName;
	int nFontSize; //字号
	int nFontWeight; //字体粗细，The weight of the font in the range 0 through 1000. For example, 400 is normal and 700 is bold.
	bool bItalic; //是否斜体
	bool bUnderline; //是否加下划线
	bool bStrikeOut; //是否加删除线

	stSystemFontParam()
	{
		Clear();
	}
	void Clear()
	{
		kFontName.Clear();
		nFontSize = SoSystemFont_MaxFontSize;
		nFontWeight = FW_NORMAL;
		bItalic = false;
		bUnderline = false;
		bStrikeOut = false;
	}
};
//----------------------------------------------------------------
struct stSystemFontCharGlyphBitData
{
	wchar_t Char;
	int nGlyphWidth;
	int nGlyphHeight;
	//数据存储格式是从左到右，从上到下。
	char* pGlyphBitData;

	stSystemFontCharGlyphBitData()
	{
		Clear();
	}
	void Clear()
	{
		Char = 0;
		nGlyphWidth = 0;
		nGlyphHeight = 0;
		pGlyphBitData = 0;
	}
};
//----------------------------------------------------------------
class SoSystemFont
{
public:
	SoSystemFont();
	~SoSystemFont();

	bool InitSystemFont(const stSystemFontParam* pParam);
	void ClearSystemFont();

	const stSystemFontCharGlyphBitData* GetCharGlyphBitData(wchar_t Char);
	const void* GetBitData();
	const stSystemFontParam& GetFontParam() const;

private:
	bool InitDevice();
	void ClearDevice();

private:
	//
	HDC m_hDrawDC;
	HFONT m_hDrawFont;
	HFONT m_hOldFont;
	HBITMAP m_hDrawBitmap;
	HBITMAP m_hOldBitmap;
	//m_pBitData指向一个存储Bitmap数据的内存块，该内存块由GDI系统分配和回收释放。
	//该Bitmap数据的宽高都是SoSystemFont_MaxFontSize像素。
	//每次绘制新的字形时，该内存块会被GDI系统清零，然后再填充新的字形的数据。
	//Bitmap的数据存储格式是从左到右，从下到上；
	//由于做了特殊处理，已经把m_pBitData的数据存储格式变成从左到右，从上到下。
	char* m_pBitData;
	RECT m_kDrawRect;
	//保存字体参数
	stSystemFontParam m_kFontParam;
	//存储最近一次读取的字形数据
	stSystemFontCharGlyphBitData m_kCurrentCharGlyph;
};
//----------------------------------------------------------------
#endif //_SoSystemFont_h_
//----------------------------------------------------------------
