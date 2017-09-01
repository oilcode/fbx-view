//----------------------------------------------------------------
#include "GGUIWindowText.h"
//----------------------------------------------------------------
GGUIWindowText::GGUIWindowText()
{
	m_eType = GGUIWindow_Text;
}
//----------------------------------------------------------------
GGUIWindowText::~GGUIWindowText()
{

}
//----------------------------------------------------------------
void GGUIWindowText::SetColor(const GGUIColor& kColor)
{
	m_kColor = kColor;
}
//----------------------------------------------------------------
void GGUIWindowText::SetFont(const char* szFont)
{
	m_kFont = szFont;
}
//----------------------------------------------------------------
void GGUIWindowText::SetText(const char* szText)
{
	if (szText)
	{
		m_strText = szText;
	}
	else
	{
		m_strText.clear();
	}
}
//----------------------------------------------------------------
void GGUIWindowText::ClearWindow()
{
	GGUIWindowBase::ClearWindow();
	m_kColor = GGUIColor_Black;
	m_kFont.Clear();
	m_strText.clear();
}
//----------------------------------------------------------------
