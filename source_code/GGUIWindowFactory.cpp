//----------------------------------------------------------------
#include "GGUIWindowFactory.h"
#include "GGUIWindowContainer.h"
#include "GGUIWindowText.h"
#include "GGUIWindowImage.h"
#include "GGUIWindowButton.h"
//----------------------------------------------------------------
#define UnusedIndex_TryToFindUnused -1
#define UnusedIndex_CreateNewOne -2
//----------------------------------------------------------------
GGUIWindowFactory* GGUIWindowFactory::ms_pInstance = 0;
//----------------------------------------------------------------
bool GGUIWindowFactory::CreateUIWindowFactory()
{
	bool br = true;
	if (ms_pInstance == 0)
	{
		ms_pInstance = new GGUIWindowFactory;
		if (ms_pInstance && ms_pInstance->InitUIWindowFactory())
		{
			br = true;
		}
		else
		{
			ReleaseUIWindowFactory();
			br = false;
		}
	}
	return br;
}
//----------------------------------------------------------------
void GGUIWindowFactory::ReleaseUIWindowFactory()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = 0;
	}
}
//----------------------------------------------------------------
GGUIWindowFactory::GGUIWindowFactory()
{

}
//----------------------------------------------------------------
GGUIWindowFactory::~GGUIWindowFactory()
{
	ClearUIWindowFactory();
}
//----------------------------------------------------------------
bool GGUIWindowFactory::InitUIWindowFactory()
{
	if (m_kWindowArray.InitArray(sizeof(GGUIWindowBase*), 10, 10) == false)
	{
		return false;
	}
	//
	for (int i = 0; i < GGUIWindow_Max; ++i)
	{
		m_nUnusedObjectIndex[i] = UnusedIndex_CreateNewOne;
	}
	//
	for (int i = 0; i < GGUIPanelSpace_Max; ++i)
	{
		m_kUISpaceArray[i].InitArray(sizeof(GGUIPanel*), 10, 10);
		m_bUISpaceVisible[i] = true;
	}
	return true;
}
//----------------------------------------------------------------
void GGUIWindowFactory::ClearUIWindowFactory()
{
	GGUIWindowBase* pWindow = 0;
	const int nCount = m_kWindowArray.GetCapacity();
	for (int i = 0; i < nCount; ++i)
	{
		pWindow = GetUIWindow(i);
		if (pWindow)
		{
			delete pWindow;
		}
	}
	m_kWindowArray.ClearArray();
	//
	for (int i = 0; i < GGUIWindow_Max; ++i)
	{
		m_nUnusedObjectIndex[i] = UnusedIndex_CreateNewOne;
	}
	//
	for (int i = 0; i < GGUIPanelSpace_Max; ++i)
	{
		m_kUISpaceArray[i].ClearArray();
		m_bUISpaceVisible[i] = true;
	}
}
//----------------------------------------------------------------
int GGUIWindowFactory::FindUnusedWindow(GGUIWindowType eType)
{
	int nID = -1;
	GGUIWindowBase* pWindow = 0;
	const int nCount = m_kWindowArray.GetCapacity();
	for (int i = 0; i < nCount; ++i)
	{
		//被回收的Window，其ParentID都是-1；
		//只要ParentID为-1，则该Window处于被回收待利用状态。
		pWindow = GetUIWindow(i);
		if (pWindow && pWindow->GetParentID() == -1 && pWindow->GetType() == eType)
		{
			nID = i;
			break;
		}
	}
	return nID;
}
//----------------------------------------------------------------
GGUIWindowBase* GGUIWindowFactory::CreateUIWindow(GGUIWindowType eType)
{
	int nIndex = -1;
	//
	if (m_nUnusedObjectIndex[eType] >= 0)
	{
		nIndex = m_nUnusedObjectIndex[eType];
		m_nUnusedObjectIndex[eType] = UnusedIndex_TryToFindUnused;
		return GetUIWindow(nIndex);
	}
	//
	if (m_nUnusedObjectIndex[eType] == UnusedIndex_TryToFindUnused)
	{
		//try to find an unused object
		nIndex = FindUnusedWindow(eType);
		if (nIndex != -1)
		{
			return GetUIWindow(nIndex);
		}
	}
	//
	m_nUnusedObjectIndex[eType] = UnusedIndex_CreateNewOne;
	GGUIWindowBase* pWindow = 0;
	switch (eType)
	{
		case GGUIWindow_Container:
		{
			pWindow = new GGUIWindowContainer;
		} break;
		case GGUIWindow_Text:
		{
			pWindow = new GGUIWindowText;
		} break;
		case GGUIWindow_Image:
		{
			pWindow = new GGUIWindowImage;
		} break;
		case GGUIWindow_Button:
		{
			pWindow = new GGUIWindowButton;
		} break;
		default:
		{
			SoLogError("GGUIWindowFactory::CreateUIWindow : Unknown WindowType [%d]", eType);
		} break;
	}
	if (pWindow)
	{
		pWindow->SetID(m_kWindowArray.FillAt(-1, &pWindow));
	}
	return pWindow;
}
//----------------------------------------------------------------
void GGUIWindowFactory::DeleteUIWindow(int nWindowID)
{
	GGUIWindowBase* pWindow = GetUIWindow(nWindowID);
	if (pWindow == 0)
	{
		return;
	}
	//
	if (pWindow->GetParentID() == -1)
	{
		//被回收的Window，其ParentID都是-1；
		//只要ParentID为-1，则该Window处于被回收待利用状态。
		pWindow->ClearWindow();
		m_nUnusedObjectIndex[pWindow->GetType()] = nWindowID;
		return;
	}
	//
	GGUIWindowBase* pParentBase = GetUIWindow(pWindow->GetParentID());
	if (pParentBase == 0)
	{
		return;
	}
	if (pParentBase->GetType() != GGUIWindow_Container)
	{
		return;
	}
	GGUIWindowContainer* pParent = (GGUIWindowContainer*)pParentBase;
	pParent->RemoveChildByID(nWindowID);
}
//----------------------------------------------------------------
GGUIWindowBase* GGUIWindowFactory::GetUIWindow(int nWindowID)
{
	void* pElement = m_kWindowArray.GetAt(nWindowID);
	if (pElement)
	{
		return *((GGUIWindowBase**)pElement);
	}
	else
	{
		return 0;
	}
}
//----------------------------------------------------------------
void GGUIWindowFactory::UISpace_AddPanel(GGUIPanel* pPanel, int eSpaceType)
{
	if (eSpaceType >= 0 && eSpaceType < GGUIPanelSpace_Max)
	{
		if (pPanel)
		{
			m_kUISpaceArray[eSpaceType].PushBack(&pPanel);
		}
	}
}
//----------------------------------------------------------------
void GGUIWindowFactory::UISpace_RemovePanel(GGUIPanel* pPanel, int eSpaceType)
{
	if (eSpaceType >= 0 && eSpaceType < GGUIPanelSpace_Max)
	{
		if (pPanel)
		{
			m_kUISpaceArray[eSpaceType].Remove(&pPanel);
		}
	}
}
//----------------------------------------------------------------
void GGUIWindowFactory::UISpace_SetRenderOrder(GGUIPanel* pPanel, int nIndex)
{
	if (pPanel)
	{
		const int eSpaceType = pPanel->GetSpaceType();
		if (eSpaceType >= 0 && eSpaceType < GGUIPanelSpace_Max)
		{
			const int nCurIndex = m_kUISpaceArray[eSpaceType].GetIndex(&pPanel);
			if (nCurIndex != nIndex)
			{
				m_kUISpaceArray[eSpaceType].SwapAt(nCurIndex, nIndex);
			}
		}
	}
}
//----------------------------------------------------------------
void GGUIWindowFactory::UISpace_SetVisible(int eSpaceType, bool bVisible)
{
	if (eSpaceType >= 0 && eSpaceType < GGUIPanelSpace_Max)
	{
		m_bUISpaceVisible[eSpaceType] = bVisible;
	}
}
//----------------------------------------------------------------
bool GGUIWindowFactory::UISpace_GetVisible(int eSpaceType)
{
	if (eSpaceType >= 0 && eSpaceType < GGUIPanelSpace_Max)
	{
		return m_bUISpaceVisible[eSpaceType];
	}
	else
	{
		return false;
	}
}
//----------------------------------------------------------------
int GGUIWindowFactory::UISpace_GetPanelCount(int eSpaceType)
{
	int nCount = 0;
	if (eSpaceType >= 0 && eSpaceType < GGUIPanelSpace_Max)
	{
		nCount = m_kUISpaceArray[eSpaceType].GetSize();
	}
	return nCount;
}
//----------------------------------------------------------------
GGUIPanel* GGUIWindowFactory::UISpace_GetPanel(int eSpaceType, int nIndex)
{
	GGUIPanel* pPanel = 0;
	if (eSpaceType >= 0 && eSpaceType < GGUIPanelSpace_Max)
	{
		void* pElement = m_kUISpaceArray[eSpaceType].GetAt(nIndex);
		if (pElement)
		{
			pPanel = *(GGUIPanel**)pElement;
		}
	}
	return pPanel;
}
//----------------------------------------------------------------
