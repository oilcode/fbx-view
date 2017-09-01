//------------------------------------------------------------
#include "GGUIPanel.h"
#include "GGUIWindowFactory.h"
//------------------------------------------------------------
GGUIPanel::GGUIPanel()
:m_pContainer(0)
,m_eSpaceType(GGUIPanelSpace_Invalid)
{

}
//------------------------------------------------------------
GGUIPanel::~GGUIPanel()
{

}
//------------------------------------------------------------
bool GGUIPanel::InitPanel(const char* szFile, GGUIPanelSpaceType eType)
{
	if (LoadPanel(szFile) == false)
	{
		return false;
	}

	m_kFileName = szFile;
	SetSpaceType(eType);
	return true;
}
//------------------------------------------------------------
bool GGUIPanel::InitPanelByUIContainer(GGUIWindowContainer* pContainer, GGUIPanelSpaceType eType)
{
	if (pContainer == 0)
	{
		return false;
	}

	m_pContainer = pContainer;
	SetSpaceType(eType);
	return true;
}
//------------------------------------------------------------
void GGUIPanel::ClearPanel()
{
	if (m_pContainer)
	{
		GGUIWindowFactory::Get()->DeleteUIWindow(m_pContainer->GetID());
		m_pContainer = 0;
	}
	m_kFileName.Clear();
	SetSpaceType(GGUIPanelSpace_Invalid);
}
//------------------------------------------------------------
void GGUIPanel::ShowPanel()
{
	if (m_pContainer)
	{
		m_pContainer->SetVisible(true);
		GGUIWindowFactory::Get()->UISpace_SetRenderOrder(this, 0);
	}
}
//------------------------------------------------------------
void GGUIPanel::HidePanel()
{
	if (m_pContainer)
	{
		m_pContainer->SetVisible(false);
	}
}
//------------------------------------------------------------
void GGUIPanel::ProcessUIEvent(int nEventType, void* pParam)
{

}
//------------------------------------------------------------
void GGUIPanel::OnResolutionChanged(float newWidth, float newHeight)
{

}
//------------------------------------------------------------
void GGUIPanel::SetSpaceType(GGUIPanelSpaceType eType)
{
	if (m_eSpaceType != GGUIPanelSpace_Invalid)
	{
		GGUIWindowFactory::Get()->UISpace_RemovePanel(this, m_eSpaceType);
	}
	m_eSpaceType = eType;
	if (m_eSpaceType != GGUIPanelSpace_Invalid)
	{
		GGUIWindowFactory::Get()->UISpace_AddPanel(this, m_eSpaceType);
	}
}
//------------------------------------------------------------
bool GGUIPanel::LoadPanel(const char* szFile)
{
	return true;
}
//------------------------------------------------------------
GGUIWindowBase* GGUIPanel::FindWindowByName(const char* szName) const
{
	if (m_pContainer)
	{
		return m_pContainer->FindWindowByName(szName);
	}
	else
	{
		return 0;
	}
}
//------------------------------------------------------------
GGUIWindowBase* GGUIPanel::FindWindowByID(int nWindowID) const
{
	if (m_pContainer)
	{
		return m_pContainer->FindWindowByID(nWindowID);
	}
	else
	{
		return 0;
	}
}
//------------------------------------------------------------
