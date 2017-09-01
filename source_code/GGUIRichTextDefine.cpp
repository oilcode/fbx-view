//----------------------------------------------------------------
#include "GGUIRichTextDefine.h"
#include "GGUIComponentText.h"
#include "GGUIFuncHelp.h"
#include "GGUIImagesetManager.h"
#include "GGUIImageset.h"
#include "SoD3DTextureSystemFont.h"
#include "SoStringConvert.h"
//----------------------------------------------------------------
//字符'<'
#define GGUIRichText_FlagBegin 60
//字符'>'
#define GGUIRichText_FlagEnd 62
//字符空格
#define GGUIRichText_FlagSpace 32
//字符Tab
#define GGUIRichText_FlagTab 9
//字符分号';'
#define GGUIRichText_FlagSplit 59
//----------------------------------------------------------------
const wchar_t* GGUIRichTextKeyString_anchor = L"anchor";
const wchar_t* GGUIRichTextKeyString_br = L"br";
const wchar_t* GGUIRichTextKeyString_color = L"color";
const wchar_t* GGUIRichTextKeyString_font = L"font";
//----------------------------------------------------------------
const wchar_t* GGUIRichTextValueString_left = L"left";
const wchar_t* GGUIRichTextValueString_right = L"right";
const wchar_t* GGUIRichTextValueString_top = L"top";
const wchar_t* GGUIRichTextValueString_bottom = L"bottom";
const wchar_t* GGUIRichTextValueString_xcenter = L"xcenter";
const wchar_t* GGUIRichTextValueString_ycenter = L"ycenter";
//----------------------------------------------------------------
GGUIRichTextStyle::GGUIRichTextStyle()
:FontImagesetID(-1)
#ifdef GGUIRichText_Debug
,Font(GGUIRichText_Default_Font)
#endif
,Color(GGUIRichText_Default_Color)
,AnchorX(GGUIRichText_Default_AnchorX)
,AnchorY(GGUIRichText_Default_AnchorY)
,bBR(false)
{
#ifdef GGUIRichText_Debug
	FontImagesetID = GGUIImagesetManager::Get()->GetImagesetIndex(Font);
#else
	FontImagesetID = GGUIImagesetManager::Get()->GetImagesetIndex(SoTinyString(GGUIRichText_Default_Font));
#endif
}
//----------------------------------------------------------------
void GGUIRichText_Pause(const wchar_t* wText, GGUIComponentText* pCompText)
{
	if (wText == 0 || wText[0] == 0)
	{
		return;
	}
	if (pCompText == 0)
	{
		return;
	}

	//这里不执行 pCompText->ClearTextChunk() ，在已有的TextChunk列表中续加。
	const int nAlreadyTextChunkCount = pCompText->GetTextChunkCount();
	//最好能够预先判断需要新增多少个Chunk，但是没办法预先判断。
	pCompText->ResizeTextChunkArray(nAlreadyTextChunkCount + GGUI_ComponentText_NewChunkCountPrePause);
	//记录新Chunk的某些属性。
	GGUITextChunk kTextChunk;
	if (nAlreadyTextChunkCount > 0)
	{
		const GGUITextChunk* pLastChunk = pCompText->GetTextChunk(nAlreadyTextChunkCount - 1);
		if (pLastChunk)
		{
			kTextChunk.kRect.x = pLastChunk->kRect.x + pLastChunk->kRect.w;
			kTextChunk.kRect.y = pLastChunk->kRect.y;
			kTextChunk.kRect.w = 0.0f;
			kTextChunk.kRect.h = pLastChunk->kRect.h;
			kTextChunk.LineIndex = pLastChunk->LineIndex;
		}
	}
	if (kTextChunk.kRect.h < GGUIRichText_Default_LineHeight)
	{
		kTextChunk.kRect.h = GGUIRichText_Default_LineHeight;
	}

	//记录上一次<>对的起止位置索引。
	int nLastFlagBegin = -1;
	int nLastFlagEnd = -1;
	GGUIRichTextStyle kLastTextStyle;
	//记录当前这一次<>对的起止位置索引。
	int nFlagBegin = -1;
	int nFlagEnd = -1;
	GGUIRichTextStyle kTextStyle;
	//
	const wchar_t* tempChar = wText;
	int nCount = 0;
	while (*tempChar != 0)
	{
		if (*tempChar == GGUIRichText_FlagBegin)
		{
			nFlagBegin = nCount;
		}
		else if (*tempChar == GGUIRichText_FlagEnd)
		{
			if (nFlagBegin != -1)
			{
				nFlagEnd = nCount;
			}
		}
		//
		if (nFlagEnd != -1)
		{
			//此时， nFlagBegin 和 nFlagEnd 都不等于-1，发现了<>对。
			//子字符串 [nFlagBegin+1, nFlagEnd-1] 即为StyleText。
			//接下来解析StyleText。
			const int nStyleTextCount = nFlagEnd - nFlagBegin - 1;
			const int nKeyCount = GGUIRichText_PauseStyle(wText+nFlagBegin+1, nStyleTextCount, &kTextStyle);
			if (nKeyCount > 0)
			{
				//下面这段代码不需要执行，可以作为一段注释说明文字。
				//if (nLastFlagBegin == -1 && nLastFlagEnd == -1)
				//{
				//	//kLastTextStyle影响范围是 [0, nFlagBegin-1] 。
				//	nLastFlagEnd = -1;
				//}
				//kLastTextStyle影响范围是 [nLastFlagEnd+1, nFlagBegin-1] 。
				const int nValidCharCount = nFlagBegin - nLastFlagEnd - 1;
				if (nValidCharCount > 0)
				{
					kTextChunk.szText = wText + nLastFlagEnd + 1;
					kTextChunk.TextCount = nValidCharCount;
					if (GGUIRichText_GenerateTextChunk(&kLastTextStyle, &kTextChunk, pCompText) == true)
					{
						kTextChunk.kRect.x = kTextChunk.kRect.x + kTextChunk.kRect.w;
						//kTextChunk.kRect.y 不变
						kTextChunk.kRect.w = 0.0f;
						//kTextChunk.kRect.h 不变
						//kTextChunk.LineIndex 不变
					}
				}
				//
				if (kTextStyle.bBR == true) //<>对是换行标志
				{
					kTextStyle.bBR = false;
					//另起一行
					kTextChunk.kRect.x = 0.0f;
					kTextChunk.kRect.y += kTextChunk.kRect.h;
					kTextChunk.kRect.w = 0.0f;
					kTextChunk.kRect.h = GGUIRichText_Default_LineHeight;
					++(kTextChunk.LineIndex);
				}
				//
				nLastFlagBegin = nFlagBegin;
				nLastFlagEnd = nFlagEnd;
				kLastTextStyle = kTextStyle;
			}
			else
			{
				//没有解析出键值对，说明是无效的<>对。
			}
			//
			nFlagBegin = -1;
			nFlagEnd = -1;
		}
		//
		++tempChar;
		++nCount;
	}

	//下面这段代码不需要执行，可以作为一段注释说明文字。
	//if (nLastFlagBegin == -1 && nLastFlagEnd == -1)
	//{
	//	//kLastTextStyle影响范围是 [0, nFlagBegin-1] 。
	//	nLastFlagEnd = -1;
	//}
	//kLastTextStyle影响范围是 [nLastFlagEnd+1, nCount-1] 。
	const int nValidCharCount = nCount - nLastFlagEnd - 1;
	if (nValidCharCount > 0)
	{
		kTextChunk.szText = wText + nLastFlagEnd + 1;
		kTextChunk.TextCount = nValidCharCount;
		GGUIRichText_GenerateTextChunk(&kLastTextStyle, &kTextChunk, pCompText);
	}
}
//----------------------------------------------------------------
int GGUIRichText_PauseStyle(const wchar_t* StyleText, const int TextCount, GGUIRichTextStyle* pStyle)
{
	//解析成功了多少个RichTextKey。
	int nKeyCount = 0;
	//
	int nKeyBegin = -1;
	int nKeyEnd = -1;
	//
	const wchar_t* tempChar = StyleText;
	int nCount = 0;
	while (nCount < TextCount)
	{
		if (*tempChar == GGUIRichText_FlagSpace)
		{
			++tempChar;
			++nCount;
			continue;
		}
		if (*tempChar == GGUIRichText_FlagTab)
		{
			++tempChar;
			++nCount;
			continue;
		}
		//
		if (*tempChar != GGUIRichText_FlagSplit)
		{
			if (nKeyBegin == -1)
			{
				nKeyBegin = nCount;
			}
		}
		else
		{
			nKeyEnd = nCount - 1;
			if (nKeyBegin != -1 && nKeyBegin < nKeyEnd)
			{
				const int nTextCount = nKeyEnd - nKeyBegin + 1;
				if (GGUIRichText_PauseKey(StyleText + nKeyBegin, nTextCount, pStyle) == true)
				{
					++nKeyCount;
				}
			}
			nKeyBegin = -1;
			nKeyEnd = -1;
		}
		//
		++tempChar;
		++nCount;
	}
	//
	nKeyEnd = nCount - 1;
	if (nKeyBegin != -1 && nKeyBegin < nKeyEnd)
	{
		const int nTextCount = nKeyEnd - nKeyBegin + 1;
		if (GGUIRichText_PauseKey(StyleText + nKeyBegin, nTextCount, pStyle) == true)
		{
			++nKeyCount;
		}
	}
	//
	return nKeyCount;
}
//----------------------------------------------------------------
bool GGUIRichText_GenerateTextChunk(const GGUIRichTextStyle* pStyle, GGUITextChunk* pNewTextChunk, GGUIComponentText* pCompText)
{
	if (pStyle->FontImagesetID == -1)
	{
		return false;
	}
	GGUIImageset* pImageset = GGUIImagesetManager::Get()->GetImagesetByID(pStyle->FontImagesetID);
	if (pImageset == 0)
	{
		return false;
	}

	//pNewTextChunk->kRect.x 为有效值
	//pNewTextChunk->kRect.y 为有效值
	pNewTextChunk->kRect.w = 0.0f;
	//pNewTextChunk->kRect.h 为有效值
	GGUIFunc_UIntConvertToGGUIColor(pStyle->Color, &(pNewTextChunk->kColor));
	//pNewTextChunk->szText 为有效值
	pNewTextChunk->FontImagesetID = pStyle->FontImagesetID;
	//pNewTextChunk->usCount 为有效值
	//pNewTextChunk->LineIndex 为有效值

	const wchar_t* wSrcText = pNewTextChunk->szText;
	const int nSrcCharCount = pNewTextChunk->TextCount;
	//预先把字形填充到贴图上。
	SoD3DTextureSystemFont::Get()->DrawString(wSrcText, nSrcCharCount);
	//
	const float fMaxWidth = pCompText->GetMaxWidth();
	//const float fMaxHeight = pCompText->GetMaxHeight();
	const float fImagesetWidth = pImageset->GetTextureWidth();
	const float fImagesetHeight = pImageset->GetTextureHeight();
	//
	SoTinyString kRectName;
	wchar_t wRectName[2] = {0};
	int nAlreadyCharCount = 0;
	for (int i = 0; i < nSrcCharCount; ++i)
	{
		wRectName[0] = wSrcText[i];
		kRectName = (char*)wRectName;
		const int nRectIndex = pImageset->GetRectID(kRectName);
		if (nRectIndex == -1)
		{
			continue;
		}
		//
		const GGUIRect& imageRect = pImageset->GetRect(nRectIndex);
		const float imageWidth = imageRect.w * fImagesetWidth;
		const float imageHeight = imageRect.h * fImagesetHeight;
		if (pNewTextChunk->kRect.x + pNewTextChunk->kRect.w + imageWidth >= fMaxWidth)
		{
			//水平方向上超出框了，换行
			const int nValidCharCount = i - nAlreadyCharCount;
			if (nValidCharCount > 0)
			{
				pNewTextChunk->szText = wSrcText + nAlreadyCharCount;
				pNewTextChunk->TextCount = nValidCharCount;
				pCompText->AddTextChunk(pNewTextChunk);
			}
			//另起一行
			pNewTextChunk->kRect.x = 0.0f;
			pNewTextChunk->kRect.y += pNewTextChunk->kRect.h;
			pNewTextChunk->kRect.w = 0.0f;
			pNewTextChunk->kRect.h = GGUIRichText_Default_LineHeight;
			++(pNewTextChunk->LineIndex);
			nAlreadyCharCount = i;
		}
		//
		pNewTextChunk->kRect.w += imageWidth;
		if (pNewTextChunk->kRect.h < imageHeight)
		{
			pNewTextChunk->kRect.h = imageHeight;
		}
	}
	//
	const int nValidCharCount = nSrcCharCount - nAlreadyCharCount;
	if (nValidCharCount > 0)
	{
		pNewTextChunk->szText = wSrcText + nAlreadyCharCount;
		pNewTextChunk->TextCount = nValidCharCount;
		pCompText->AddTextChunk(pNewTextChunk);
	}
	return true;
}
//----------------------------------------------------------------
bool GGUIRichText_PauseKey(const wchar_t* KeyText, const int TextCount, GGUIRichTextStyle* pStyle)
{
	bool bPauseSuccess = false;
	switch (KeyText[0])
	{
	case L'a':
		{
			bPauseSuccess = GGUIRichText_PauseKey_anchor(KeyText, TextCount, pStyle);
		} break;
	case L'b':
		{
			bPauseSuccess = GGUIRichText_PauseKey_br(KeyText, TextCount, pStyle);
		} break;
	case L'c':
		{
			bPauseSuccess = GGUIRichText_PauseKey_color(KeyText, TextCount, pStyle);
		} break;
	case L'f':
		{
			bPauseSuccess = GGUIRichText_PauseKey_font(KeyText, TextCount, pStyle);
		} break;
	case GGUIRichText_FlagSpace: //空格
	case GGUIRichText_FlagTab: //Tab
		{
			//什么都不做
		} break;
	default:
		{
			//无法识别的字符，说明该字符串不是Flag字符串
			//什么都不做
		}
		break;
	}
	return bPauseSuccess;
}
//----------------------------------------------------------------
bool GGUIRichText_PauseKey_anchor(const wchar_t* KeyText, const int TextCount, GGUIRichTextStyle* pStyle)
{
	bool br = false;
	const wchar_t* wValue = SoCmdLineHelp::GetValueByKey_W(KeyText, TextCount, GGUIRichTextKeyString_anchor);
	if (wValue[0] != 0)
	{
		switch (wValue[0])
		{
		case L'l': //left
			{
				pStyle->AnchorX = GGUIRichTextAnchor_Left;
				br = true;
			} break;
		case L'r': //right
			{
				pStyle->AnchorX = GGUIRichTextAnchor_Right;
				br = true;
			} break;
		case L't': //top
			{
				pStyle->AnchorY = GGUIRichTextAnchor_Top;
				br = true;
			} break;
		case L'b': //bottom
			{
				pStyle->AnchorY = GGUIRichTextAnchor_Bottom;
				br = true;
			} break;
		case L'x': //xcenter
			{
				pStyle->AnchorX = GGUIRichTextAnchor_XCenter;
				br = true;
			} break;
		case L'y': //ycenter
			{
				pStyle->AnchorY = GGUIRichTextAnchor_YCenter;
				br = true;
			} break;
		default:
			break;
		}
	}
	return br;
}
//----------------------------------------------------------------
bool GGUIRichText_PauseKey_br(const wchar_t* KeyText, const int TextCount, GGUIRichTextStyle* pStyle)
{
	bool br = false;
	if (TextCount == 2)
	{
		if (KeyText[0] == GGUIRichTextKeyString_br[0] && KeyText[1] == GGUIRichTextKeyString_br[1])
		{
			pStyle->bBR = true;
			br = true;
		}
	}
	return br;
}
//----------------------------------------------------------------
bool GGUIRichText_PauseKey_color(const wchar_t* KeyText, const int TextCount, GGUIRichTextStyle* pStyle)
{
	bool br = false;
	const wchar_t* wValue = SoCmdLineHelp::GetValueByKey_W(KeyText, TextCount, GGUIRichTextKeyString_color);
	if (wValue[0] != 0)
	{
		pStyle->Color = SoWstrtoul(wValue, NULL, 16);
		br = true;
	}
	return br;
}
//----------------------------------------------------------------
bool GGUIRichText_PauseKey_font(const wchar_t* KeyText, const int TextCount, GGUIRichTextStyle* pStyle)
{
	bool br = false;
	const wchar_t* wValue = SoCmdLineHelp::GetValueByKey_W(KeyText, TextCount, GGUIRichTextKeyString_font);
	if (wValue[0] != 0)
	{
		const char* szFont = SoUnicodeToAnsi(wValue);
		if (szFont[0] != 0)
		{
			int newFontImagesetID = -1;
#ifdef GGUIRichText_Debug
			pStyle->Font = szFont;
			newFontImagesetID = GGUIImagesetManager::Get()->GetImagesetIndex(pStyle->Font);
#else
			newFontImagesetID = GGUIImagesetManager::Get()->GetImagesetIndex(SoTinyString(szFont));
#endif
			if (newFontImagesetID != -1)
			{
				pStyle->FontImagesetID = newFontImagesetID;
				br = true;
			}
		}
	}
	return br;
}
//----------------------------------------------------------------
