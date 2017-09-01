//------------------------------------------------------------
#include "GGUIWindowContainer.h"
#include "GGUIWindowFactory.h"
//------------------------------------------------------------
GGUIWindowContainer::GGUIWindowContainer()
{
	m_eType = GGUIWindow_Container;
	m_kWindowArray.InitArray(sizeof(GGUIWindowBase*), 10, 10);
}
//------------------------------------------------------------
GGUIWindowContainer::~GGUIWindowContainer()
{
	
}
//------------------------------------------------------------
void GGUIWindowContainer::AddChild(GGUIWindowBase* pChild)
{
	if (pChild == 0)
	{
		return;
	}
	if (GetChildByID(pChild->GetID()) != 0)
	{
		//already exist
		return;
	}
	//
	m_kWindowArray.PushBack(&pChild);
	pChild->SetParentID(m_nID);
	pChild->OnParentRectChanged(m_kRectInAbsCoord);
	pChild->SetVisibleByReason(GGUIReasonUnvisible_Parent, GetVisible());
}
//------------------------------------------------------------
void GGUIWindowContainer::RemoveChildByIndex(int nIndex)
{
	GGUIWindowBase* pChild = GetChildByIndex(nIndex);
	if (pChild)
	{
		DoRemoveChild(pChild);
		m_kWindowArray.RemoveAt(nIndex);
	}
}
//------------------------------------------------------------
void GGUIWindowContainer::RemoveChildByID(int nWindowID)
{
	const int nIndex = GetIndexByID(nWindowID);
	if (nIndex != -1)
	{
		RemoveChildByIndex(nIndex);
	}
}
//------------------------------------------------------------
void GGUIWindowContainer::RemoveAllChild()
{
	GGUIWindowFactory* pFactory = GGUIWindowFactory::Get();
	const int nCount = m_kWindowArray.GetSize();
	for (int i = 0; i < nCount; ++i)
	{
		GGUIWindowBase* pChild = GetChildByIndex(i);
		if (pChild)
		{
			DoRemoveChild(pChild);
		}
	}
	m_kWindowArray.ClearArray();
}
//------------------------------------------------------------
void GGUIWindowContainer::DoRemoveChild(GGUIWindowBase* pChild)
{
	pChild->SetParentID(-1);
	GGUIWindowFactory::Get()->DeleteUIWindow(pChild->GetID());
}
//------------------------------------------------------------
void GGUIWindowContainer::SwapChild(int nIndexA, int nIndexB)
{
	m_kWindowArray.SwapAt(nIndexA, nIndexB);
}
//------------------------------------------------------------
GGUIWindowBase* GGUIWindowContainer::GetChildByIndex(int nIndex) const
{
	void* pElement = m_kWindowArray.GetAt(nIndex);
	if (pElement)
	{
		return *((GGUIWindowBase**)pElement);
	}
	else
	{
		return 0;
	}
}
//------------------------------------------------------------
GGUIWindowBase* GGUIWindowContainer::GetChildByID(int nWindowID) const
{
	GGUIWindowBase* pWindow = 0;
	GGUIWindowBase* tempWindow = 0;
	const int nCount = m_kWindowArray.GetSize();
	for (int i = 0; i < nCount; ++i)
	{
		tempWindow = GetChildByIndex(i);
		if (tempWindow)
		{
			if (tempWindow->GetID() == nWindowID)
			{
				pWindow = tempWindow;
				break;
			}
		}
	}
	return pWindow;
}
//------------------------------------------------------------
GGUIWindowBase* GGUIWindowContainer::GetChildByName(const char* szName) const
{
	GGUIWindowBase* pWindow = 0;
	GGUIWindowBase* tempWindow = 0;
	const int nCount = m_kWindowArray.GetSize();
	for (int i = 0; i < nCount; ++i)
	{
		tempWindow = GetChildByIndex(i);
		if (tempWindow)
		{
			if (SoStrCmp(tempWindow->GetName().GetValue(), szName) == 0)
			{
				pWindow = tempWindow;
				break;
			}
		}
	}
	return pWindow;
}
//------------------------------------------------------------
GGUIWindowBase* GGUIWindowContainer::FindWindowByID(int nWindowID) const
{
	if (GetID() == nWindowID)
	{
		return (GGUIWindowBase*)this;
	}
	//
	GGUIWindowBase* pWindow = 0;
	GGUIWindowBase* tempWindow = 0;
	const int nCount = m_kWindowArray.GetSize();
	for (int i = 0; i < nCount; ++i)
	{
		tempWindow = GetChildByIndex(i);
		if (tempWindow)
		{
			if (tempWindow->GetID() == nWindowID)
			{
				pWindow = tempWindow;
				break;
			}
			if (tempWindow->GetType() == GGUIWindow_Container)
			{
				GGUIWindowContainer* pContainer = (GGUIWindowContainer*)tempWindow;
				GGUIWindowBase* pChild = pContainer->FindWindowByID(nWindowID);
				if (pChild)
				{
					pWindow = pChild;
					break;
				}
			}
		}
	}
	return pWindow;
}
//------------------------------------------------------------
GGUIWindowBase* GGUIWindowContainer::FindWindowByName(const char* szName) const
{
	if (SoStrCmp(GetName().GetValue(), szName) == 0)
	{
		return (GGUIWindowBase*)this;
	}
	//
	GGUIWindowBase* pWindow = 0;
	GGUIWindowBase* tempWindow = 0;
	const int nCount = m_kWindowArray.GetSize();
	for (int i = 0; i < nCount; ++i)
	{
		tempWindow = GetChildByIndex(i);
		if (tempWindow)
		{
			if (SoStrCmp(tempWindow->GetName().GetValue(), szName) == 0)
			{
				pWindow = tempWindow;
				break;
			}
			if (tempWindow->GetType() == GGUIWindow_Container)
			{
				GGUIWindowContainer* pContainer = (GGUIWindowContainer*)tempWindow;
				GGUIWindowBase* pChild = pContainer->FindWindowByName(szName);
				if (pChild)
				{
					pWindow = pChild;
					break;
				}
			}
		}
	}
	return pWindow;
}
//----------------------------------------------------------------
int GGUIWindowContainer::GetIndexByID(int nWindowID) const
{
	int nIndex = -1;
	GGUIWindowBase* tempWindow = 0;
	const int nCount = m_kWindowArray.GetSize();
	for (int i = 0; i < nCount; ++i)
	{
		tempWindow = GetChildByIndex(i);
		if (tempWindow)
		{
			if (tempWindow->GetID() == nWindowID)
			{
				nIndex = i;
				break;
			}
		}
	}
	return nIndex;
}
//----------------------------------------------------------------
void GGUIWindowContainer::Notify_ParentRectChanged() const
{
	GGUIWindowBase* pChild = 0;
	const int nCount = m_kWindowArray.GetSize();
	for (int i = 0; i < nCount; ++i)
	{
		pChild = GetChildByIndex(i);
		if (pChild)
		{
			pChild->OnParentRectChanged(m_kRectInAbsCoord);
		}
	}
}
//----------------------------------------------------------------
void GGUIWindowContainer::Notify_ParentVisibleChanged() const
{
	GGUIWindowBase* pChild = 0;
	const bool bVisible = GetVisible();
	const int nCount = m_kWindowArray.GetSize();
	for (int i = 0; i < nCount; ++i)
	{
		pChild = GetChildByIndex(i);
		if (pChild)
		{
			pChild->SetVisibleByReason(GGUIReasonUnvisible_Parent, bVisible);
		}
	}
}
//----------------------------------------------------------------
void GGUIWindowContainer::ClearWindow()
{
	GGUIWindowBase::ClearWindow();
	RemoveAllChild();
}
//----------------------------------------------------------------
void GGUIWindowContainer::SetFullRect(const GGUIFullRect& kRect)
{
	GGUIWindowBase::SetFullRect(kRect);
	Notify_ParentRectChanged();
}
//----------------------------------------------------------------
void GGUIWindowContainer::SetScalePos(float fScaleX, float fScaleY)
{
	GGUIWindowBase::SetScalePos(fScaleX, fScaleY);
	Notify_ParentRectChanged();
}
//----------------------------------------------------------------
void GGUIWindowContainer::SetDeltaPos(float fDeltaX, float fDeltaY)
{
	GGUIWindowBase::SetDeltaPos(fDeltaX, fDeltaY);
	Notify_ParentRectChanged();
}
//----------------------------------------------------------------
void GGUIWindowContainer::SetAbsDeltaPos(float fAbsDeltaX, float fAbsDeltaY)
{
	GGUIWindowBase::SetAbsDeltaPos(fAbsDeltaX, fAbsDeltaY);
	Notify_ParentRectChanged();
}
//----------------------------------------------------------------
void GGUIWindowContainer::MoveDelta(float fDeltaX, float fDeltaY)
{
	GGUIWindowBase::MoveDelta(fDeltaX, fDeltaY);
	Notify_ParentRectChanged();
}
//----------------------------------------------------------------
void GGUIWindowContainer::OnParentRectChanged(const GGUIRect& kParentRectInAbsCoord)
{
	GGUIWindowBase::OnParentRectChanged(kParentRectInAbsCoord);
	Notify_ParentRectChanged();
}
//----------------------------------------------------------------
void GGUIWindowContainer::SetVisibleByReason(souint32 uiReason, bool bVisible)
{
	const bool oldValue = GetVisible();
	GGUIWindowBase::SetVisibleByReason(uiReason, bVisible);
	const bool newValue = GetVisible();
	if (oldValue != newValue)
	{
		Notify_ParentVisibleChanged();
	}
}
//------------------------------------------------------------
