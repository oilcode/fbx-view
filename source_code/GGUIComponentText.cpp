//----------------------------------------------------------------
#include "GGUIComponentText.h"
//----------------------------------------------------------------
GGUIComponentText::GGUIComponentText()
:m_fMaxWidth(0.0f)
,m_fMaxHeight(0.0f)
{
	//初始时没有为 m_kTextChunkArray 分配内存，外界要主动调用 ResizeTextChunkArray() 。
	m_kTextChunkArray.InitArray(sizeof(GGUITextChunk), 0, GGUI_ComponentText_TextChunkAddCount);
}
//----------------------------------------------------------------
GGUIComponentText::~GGUIComponentText()
{
	ClearComponentText();
}
//----------------------------------------------------------------
void GGUIComponentText::ClearComponentText()
{
	m_kTextChunkArray.ClearArray();
}
//----------------------------------------------------------------
void GGUIComponentText::ResizeTextChunkArray(int nCount)
{
	m_kTextChunkArray.ResizeArray(nCount);
}
//----------------------------------------------------------------
void GGUIComponentText::AddTextChunk(const GGUITextChunk* pChunk)
{
	m_kTextChunkArray.PushBack(pChunk);
}
//----------------------------------------------------------------
void GGUIComponentText::ClearTextChunk()
{
	m_kTextChunkArray.ClearArray();
}
//----------------------------------------------------------------
int GGUIComponentText::GetTextChunkCount() const
{
	return m_kTextChunkArray.GetSize();
}
//----------------------------------------------------------------
const GGUITextChunk* GGUIComponentText::GetTextChunk(int nIndex) const
{
	return (GGUITextChunk*)(m_kTextChunkArray.GetAt(nIndex));
}
//----------------------------------------------------------------
