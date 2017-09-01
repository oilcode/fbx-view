//----------------------------------------------------------------
#ifndef _GGUIWindowText_h_
#define _GGUIWindowText_h_
//----------------------------------------------------------------
#include "GGUIWindowBase.h"
//----------------------------------------------------------------
class GGUIWindowText : public GGUIWindowBase
{
public:
	void SetColor(const GGUIColor& kColor);
	const GGUIColor& GetColor() const;
	void SetFont(const char* szFont);
	const SoTinyString& GetFont() const;
	void SetText(const char* szText);
	const std::string& GetText() const;

protected:
	friend class GGUIWindowFactory;
	GGUIWindowText();
	~GGUIWindowText();
	//re-write parent function
	void ClearWindow();

private:
	GGUIColor m_kColor;
	SoTinyString m_kFont;
	std::string m_strText;
};
//----------------------------------------------------------------
inline const GGUIColor& GGUIWindowText::GetColor() const
{
	return m_kColor;
}
//----------------------------------------------------------------
inline const SoTinyString& GGUIWindowText::GetFont() const
{
	return m_kFont;
}
//----------------------------------------------------------------
inline const std::string& GGUIWindowText::GetText() const
{
	return m_strText;
}
//----------------------------------------------------------------
#endif //_GGUIWindowText_h_
//----------------------------------------------------------------
