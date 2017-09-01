//----------------------------------------------------------------
#include "GGUIWindowButton.h"
//----------------------------------------------------------------
GGUIWindowButton::GGUIWindowButton()
:m_nImagesetIndex(-1)
,m_nImageRectIndex(-1)
,m_eButtonState(ButtonState_Normal)
{
	m_eType = GGUIWindow_Button;
}
//----------------------------------------------------------------
GGUIWindowButton::~GGUIWindowButton()
{

}
//----------------------------------------------------------------
void GGUIWindowButton::ClearWindow()
{
	GGUIWindowBase::ClearWindow();
	m_nImagesetIndex = -1;
	m_nImageRectIndex = -1;
	m_eButtonState = ButtonState_Normal;
}
//----------------------------------------------------------------
void GGUIWindowButton::SetTexture(const char* szTexture)
{
	GGUIFunc_GetImagesetIndexRectIndex(szTexture, m_nImagesetIndex, m_nImageRectIndex);
}
//----------------------------------------------------------------
void GGUIWindowButton::SetText(const char* szText)
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
