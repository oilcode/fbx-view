//----------------------------------------------------------------
#include "GGUIWindowBase.h"
//----------------------------------------------------------------
GGUIWindowBase::GGUIWindowBase()
:m_nID(-1)
,m_nParentID(-1)
,m_eType(GGUIWindow_Invalid)
,m_bCursorInWindowRect(false)
{
	
}
//----------------------------------------------------------------
GGUIWindowBase::~GGUIWindowBase()
{

}
//----------------------------------------------------------------
void GGUIWindowBase::ClearWindow()
{
	//被Factory回收的Window，其ParentID都是-1；
	//只要ParentID为-1，则该Window处于被回收待利用状态。
	//
	//m_nID = -1;
	//m_eType = GGUIWindow_Invalid;
	m_nParentID = -1;
	m_kName.Clear();
	m_kFullRect = GGUIFullRect_Empty;
	m_kRectInAbsCoord = GGUIRect_Empty;
	m_kParentRectInAbsCoord = GGUIRect_Empty;
	m_kUnvisibleReason.Clear();
	m_kInputDisableReason.Clear();
}
//----------------------------------------------------------------
void GGUIWindowBase::SetFullRect(const GGUIFullRect& kRect)
{
	m_kFullRect = kRect;
	CalculateRectInAbsCoord();
}
//----------------------------------------------------------------
void GGUIWindowBase::SetScalePos(float fScaleX, float fScaleY)
{
	m_kFullRect.fScaleX = fScaleX;
	m_kFullRect.fScaleY = fScaleY;
	CalculateRectInAbsCoord();
}
//----------------------------------------------------------------
void GGUIWindowBase::SetDeltaPos(float fDeltaX, float fDeltaY)
{
	m_kFullRect.fDeltaX = fDeltaX;
	m_kFullRect.fDeltaY = fDeltaY;
	CalculateRectInAbsCoord();
}
//----------------------------------------------------------------
void GGUIWindowBase::SetAbsDeltaPos(float fAbsDeltaX, float fAbsDeltaY)
{
	float fOldAbsDeltaX = m_kRectInAbsCoord.x - m_kParentRectInAbsCoord.x;
	float fOldAbsDeltaY = m_kRectInAbsCoord.y - m_kParentRectInAbsCoord.y;
	m_kFullRect.fDeltaX += fAbsDeltaX - fOldAbsDeltaX;
	m_kFullRect.fDeltaY += fAbsDeltaY - fOldAbsDeltaY;
	CalculateRectInAbsCoord();
}
//----------------------------------------------------------------
void GGUIWindowBase::MoveDelta(float fDeltaX, float fDeltaY)
{
	m_kFullRect.fDeltaX += fDeltaX;
	m_kFullRect.fDeltaY += fDeltaY;
	CalculateRectInAbsCoord();
}
//----------------------------------------------------------------
void GGUIWindowBase::OnParentRectChanged(const GGUIRect& kParentRectInAbsCoord)
{
	m_kParentRectInAbsCoord = kParentRectInAbsCoord;
	CalculateRectInAbsCoord();
}
//----------------------------------------------------------------
void GGUIWindowBase::SetVisibleByReason(souint32 uiReason, bool bVisible)
{
	if (bVisible)
	{
		m_kUnvisibleReason.RemoveFlag(uiReason);
	}
	else
	{
		m_kUnvisibleReason.AddFlag(uiReason);
	}
}
//----------------------------------------------------------------
void GGUIWindowBase::SetInputEnableByReason(souint32 uiReason, bool bEnable)
{
	if (bEnable)
	{
		m_kInputDisableReason.RemoveFlag(uiReason);
	}
	else
	{
		m_kInputDisableReason.AddFlag(uiReason);
	}
}
//----------------------------------------------------------------
void GGUIWindowBase::CalculateRectInAbsCoord()
{
	m_kRectInAbsCoord.x = m_kParentRectInAbsCoord.x + m_kParentRectInAbsCoord.w * m_kFullRect.fScaleX + m_kFullRect.fDeltaX;
	m_kRectInAbsCoord.y = m_kParentRectInAbsCoord.y + m_kParentRectInAbsCoord.h * m_kFullRect.fScaleY + m_kFullRect.fDeltaY;
	m_kRectInAbsCoord.w = m_kParentRectInAbsCoord.w * m_kFullRect.fScaleW + m_kFullRect.fDeltaW;
	m_kRectInAbsCoord.h = m_kParentRectInAbsCoord.h * m_kFullRect.fScaleH + m_kFullRect.fDeltaH;
}
//----------------------------------------------------------------
