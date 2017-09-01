//----------------------------------------------------------------
#include "GGUIWindowRichText.h"
#include "SoStringConvert.h"
//----------------------------------------------------------------
GGUIWindowRichText::GGUIWindowRichText()
:m_kCompText()
,m_bNeedPause(false)
{

}
//----------------------------------------------------------------
GGUIWindowRichText::~GGUIWindowRichText()
{

}
//----------------------------------------------------------------
void GGUIWindowRichText::ClearWindow()
{

}
//----------------------------------------------------------------
void GGUIWindowRichText::SetRichText(const char* szText)
{
	if (szText == 0 || szText[0] == 0)
	{
		ClearRichText();
		return;
	}

	wchar_t* wString = SoAnsiToUnicode(szText);
	if (wString[0] == 0)
	{
		ClearRichText();
		return;
	}

	m_wstrText = wString;
	m_bNeedPause = true;
}
//----------------------------------------------------------------
void GGUIWindowRichText::ClearRichText()
{
	m_kCompText.ClearComponentText();
	m_wstrText.clear();
	m_bNeedPause = false;
}
//----------------------------------------------------------------
