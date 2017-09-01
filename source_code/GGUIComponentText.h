//----------------------------------------------------------------
#ifndef _GGUIComponentText_h_
#define _GGUIComponentText_h_
//----------------------------------------------------------------
#include "GGUIDefine.h"
//----------------------------------------------------------------
struct GGUITextChunk
{
	GGUIRect kRect;
	GGUIColor kColor;
	const wchar_t* szText;
	//字体贴图的ImagesetID。
	int FontImagesetID;
	//本Chunk包含的字符是[0, TextCount-1]。
	unsigned short TextCount;
	//本Chunk位于第几行。0为有效值。
	unsigned char LineIndex;
	//
	GGUITextChunk() : szText(0), FontImagesetID(-1), TextCount(0), LineIndex(0) {}
};
//----------------------------------------------------------------
class GGUIComponentText
{
public:
	GGUIComponentText();
	~GGUIComponentText();
	void ClearComponentText();

	void ResizeTextChunkArray(int nCount);
	void AddTextChunk(const GGUITextChunk* pChunk);
	void ClearTextChunk();
	int GetTextChunkCount() const;
	const GGUITextChunk* GetTextChunk(int nIndex) const;

	void SetMaxWidth(float fw);
	void SetMaxHeight(float fh);
	float GetMaxWidth() const;
	float GetMaxHeight() const;

private:
	SoArray m_kTextChunkArray;
	float m_fMaxWidth;
	float m_fMaxHeight;
};
//----------------------------------------------------------------
inline void GGUIComponentText::SetMaxWidth(float fw)
{
	m_fMaxWidth = fw;
}
//----------------------------------------------------------------
inline void GGUIComponentText::SetMaxHeight(float fh)
{
	m_fMaxHeight = fh;
}
//----------------------------------------------------------------
inline float GGUIComponentText::GetMaxWidth() const
{
	return m_fMaxWidth;
}
//----------------------------------------------------------------
inline float GGUIComponentText::GetMaxHeight() const
{
	return m_fMaxHeight;
}
//----------------------------------------------------------------
#endif //_GGUIComponentText_h_
//----------------------------------------------------------------
