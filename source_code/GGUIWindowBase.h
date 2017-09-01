//----------------------------------------------------------------
#ifndef _GGUIWindowBase_h_
#define _GGUIWindowBase_h_
//----------------------------------------------------------------
#include "GGUIDefine.h"
//----------------------------------------------------------------
enum GGUIWindowType
{
	GGUIWindow_Invalid = -1,
	GGUIWindow_Container,
	GGUIWindow_Text,
	GGUIWindow_Image,
	GGUIWindow_Button,
	GGUIWindow_Max,
};
enum GGUIReasonUnvisible
{
	GGUIReasonUnvisible_Self = 0x00000001,
	GGUIReasonUnvisible_Parent = 0x00000002,
	GGUIReasonUnvisible_3 = 0x00000004,
	GGUIReasonUnvisible_4 = 0x00000008,
};
enum GGUIReasonInputDisable
{
	GGUIReasonInputDisable_Self = 0x00000001,
	GGUIReasonInputDisable_2 = 0x00000002,
	GGUIReasonInputDisable_3 = 0x00000004,
	GGUIReasonInputDisable_4 = 0x00000008,
};
//----------------------------------------------------------------
class GGUIWindowBase
{
public:
	int GetID() const;
	GGUIWindowType GetType() const;
	void SetParentID(int nID);
	int GetParentID() const;
	void SetName(const char* szName);
	const SoTinyString& GetName() const;

	virtual void SetFullRect(const GGUIFullRect& kRect);
	virtual void SetScalePos(float fScaleX, float fScaleY);
	virtual void SetDeltaPos(float fDeltaX, float fDeltaY);
	virtual void SetAbsDeltaPos(float fAbsDeltaX, float fAbsDeltaY);
	virtual void MoveDelta(float fDeltaX, float fDeltaY);
	virtual void OnParentRectChanged(const GGUIRect& kParentRectInAbsCoord);
	const GGUIFullRect& GetFullRect() const;
	const GGUIRect& GetRectInAbsCoord() const;
	const GGUIRect& GetParentRectInAbsCoord() const;

	void SetVisible(bool bVisible);
	virtual void SetVisibleByReason(souint32 uiReason, bool bVisible);
	bool GetVisible() const;

	void SetInputEnable(bool bEnable);
	void SetInputEnableByReason(souint32 uiReason, bool bEnable);
	bool GetInputEnable() const;

	void SetCursorInWindowRect(bool bIn);
	bool GetCursorInWindowRect() const;

protected:
	friend class GGUIWindowFactory;
	GGUIWindowBase();
	virtual ~GGUIWindowBase();
	virtual void ClearWindow();
	void SetID(int nID);
	void CalculateRectInAbsCoord();

protected:
	int m_nID;
	int m_nParentID;
	GGUIWindowType m_eType;
	SoTinyString m_kName;
	GGUIFullRect m_kFullRect;
	GGUIRect m_kRectInAbsCoord;
	GGUIRect m_kParentRectInAbsCoord;
	SoBitFlag m_kUnvisibleReason;
	SoBitFlag m_kInputDisableReason;
	bool m_bCursorInWindowRect;
};
//----------------------------------------------------------------
inline void GGUIWindowBase::SetID(int nID)
{
	m_nID = nID;
}
//----------------------------------------------------------------
inline int GGUIWindowBase::GetID() const
{
	return m_nID;
}
//----------------------------------------------------------------
inline void GGUIWindowBase::SetParentID(int nID)
{
	m_nParentID = nID;
}
//----------------------------------------------------------------
inline int GGUIWindowBase::GetParentID() const
{
	return m_nParentID;
}
//----------------------------------------------------------------
inline GGUIWindowType GGUIWindowBase::GetType() const
{
	return m_eType;
}
//----------------------------------------------------------------
inline void GGUIWindowBase::SetName(const char* szName)
{
	m_kName = szName;
}
//----------------------------------------------------------------
inline const SoTinyString& GGUIWindowBase::GetName() const
{
	return m_kName;
}
//----------------------------------------------------------------
inline const GGUIFullRect& GGUIWindowBase::GetFullRect() const
{
	return m_kFullRect;
}
//----------------------------------------------------------------
inline const GGUIRect& GGUIWindowBase::GetRectInAbsCoord() const
{
	return m_kRectInAbsCoord;
}
//----------------------------------------------------------------
inline const GGUIRect& GGUIWindowBase::GetParentRectInAbsCoord() const
{
	return m_kParentRectInAbsCoord;
}
//----------------------------------------------------------------
inline void GGUIWindowBase::SetVisible(bool bVisible)
{
	SetVisibleByReason(GGUIReasonUnvisible_Self, bVisible);
}
//----------------------------------------------------------------
inline bool GGUIWindowBase::GetVisible() const
{
	return m_kUnvisibleReason.IsZero();
}
//----------------------------------------------------------------
inline void GGUIWindowBase::SetInputEnable(bool bEnable)
{
	SetInputEnableByReason(GGUIReasonInputDisable_Self, bEnable);
}
//----------------------------------------------------------------
inline bool GGUIWindowBase::GetInputEnable() const
{
	return m_kInputDisableReason.IsZero();
}
//----------------------------------------------------------------
inline void GGUIWindowBase::SetCursorInWindowRect(bool bIn)
{
	m_bCursorInWindowRect = bIn;
}
//----------------------------------------------------------------
inline bool GGUIWindowBase::GetCursorInWindowRect() const
{
	return m_bCursorInWindowRect;
}
//----------------------------------------------------------------
#endif //_GGUIWindowBase_h_
//----------------------------------------------------------------
