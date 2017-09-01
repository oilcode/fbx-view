//----------------------------------------------------------------
#ifndef _GGUIRichTextHelp_h_
#define _GGUIRichTextHelp_h_
//----------------------------------------------------------------
#include "GGUIDefine.h"
//----------------------------------------------------------------
#define GGUIRichText_Debug
//----------------------------------------------------------------
#define GGUIRichText_Default_Font "宋体"
#define GGUIRichText_Default_Color 0xFFFFFFFF
#define GGUIRichText_Default_AnchorX GGUIRichTextAnchor_Left
#define GGUIRichText_Default_AnchorY GGUIRichTextAnchor_Bottom
#define GGUIRichText_Default_LineHeight 14.0f
//----------------------------------------------------------------
//enum GGUIRichTextKeyType
//{
//	GGUIRichTextKey_Invalid = -1,
//	GGUIRichTextKey_Font,
//	GGUIRichTextKey_Color,
//	GGUIRichTextKey_AnchorX,
//	GGUIRichTextKey_AnchorY,
//	GGUIRichTextKey_br, //换行
//};
enum GGUIRichTextAnchor
{
	GGUIRichTextAnchor_Left,
	GGUIRichTextAnchor_Right,
	GGUIRichTextAnchor_Top,
	GGUIRichTextAnchor_Bottom,
	GGUIRichTextAnchor_XCenter,
	GGUIRichTextAnchor_YCenter,
};
//----------------------------------------------------------------
struct GGUIRichTextStyle
{
	//<font=myfont1>, Font="myfont1"
#ifdef GGUIRichText_Debug
	SoTinyString Font;
#endif
	soint32 FontImagesetID;
	//<color=ff0000ff>, Color=0xff0000ff
	souint32 Color;
	//<anchor=xcenter>, AnchorX=GGUIRichTextAnchor_XCenter
	GGUIRichTextAnchor AnchorX;
	//<anchor=top>, AnchorY=GGUIRichTextAnchor_Top
	GGUIRichTextAnchor AnchorY;
	//<br>
	bool bBR;
	//
	GGUIRichTextStyle();
};
//----------------------------------------------------------------
struct GGUITextChunk;
class GGUIComponentText;
//----------------------------------------------------------------
//解析一个RichText字符串，把解析之后的结果存储到pCompText中。
//函数内部不会把pCompText中已有的TextChunk清除，而是把新TextChunk附加在列表尾部。
void GGUIRichText_Pause(const wchar_t* wText, GGUIComponentText* pCompText);
//解析一个以'<''>'为起止符号的RichTextStyle字符串，把解析之后的结果存储到pStyle中。
//返回“解析成功了多少个RichTextKey”。
int GGUIRichText_PauseStyle(const wchar_t* StyleText, const int TextCount, GGUIRichTextStyle* pStyle);
bool GGUIRichText_GenerateTextChunk(const GGUIRichTextStyle* pStyle, GGUITextChunk* pNewTextChunk, GGUIComponentText* pCompText);
//解析RichTextStyle字符串中的一个键值对，把解析之后的结果存储到pStyle中。
bool GGUIRichText_PauseKey(const wchar_t* KeyText, const int TextCount, GGUIRichTextStyle* pStyle);
bool GGUIRichText_PauseKey_anchor(const wchar_t* KeyText, const int TextCount, GGUIRichTextStyle* pStyle);
bool GGUIRichText_PauseKey_br(const wchar_t* KeyText, const int TextCount, GGUIRichTextStyle* pStyle);
bool GGUIRichText_PauseKey_color(const wchar_t* KeyText, const int TextCount, GGUIRichTextStyle* pStyle);
bool GGUIRichText_PauseKey_font(const wchar_t* KeyText, const int TextCount, GGUIRichTextStyle* pStyle);
//----------------------------------------------------------------
#endif //_GGUIRichTextHelp_h_
//----------------------------------------------------------------
