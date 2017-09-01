//----------------------------------------------------------------
#ifndef _GGUIWindowRichText_h_
#define _GGUIWindowRichText_h_
//----------------------------------------------------------------
#include "GGUIWindowBase.h"
#include "GGUIComponentText.h"
//----------------------------------------------------------------
class GGUIWindowRichText : public GGUIWindowBase
{
public:
	void SetRichText(const char* szText);
	void ClearRichText();
	bool IsNeedPause() const;

protected:
	friend class GGUIWindowFactory;
	GGUIWindowRichText();
	~GGUIWindowRichText();
	//re-write parent function
	void ClearWindow();

private:
	GGUIComponentText m_kCompText;
	std::wstring m_wstrText;
	bool m_bNeedPause;
};
//----------------------------------------------------------------
bool GGUIWindowRichText::IsNeedPause() const
{
	return m_bNeedPause;
}
//----------------------------------------------------------------
#endif //_GGUIWindowRichText_h_
//----------------------------------------------------------------
