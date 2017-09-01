//------------------------------------------------------------
#ifndef _GGUIPanel_h_
#define _GGUIPanel_h_
//------------------------------------------------------------
#include "GGUIDefine.h"
#include "GGUIWindowContainer.h"
#include "GGUIEvent.h"
//------------------------------------------------------------
enum GGUIPanelSpaceType
{
	GGUIPanelSpace_Invalid = -1,
	GGUIPanelSpace_1,
	GGUIPanelSpace_2,
	GGUIPanelSpace_3,
	GGUIPanelSpace_Max,
};
//------------------------------------------------------------
class GGUIPanel
{
public:
	GGUIPanel();
	~GGUIPanel();
	virtual bool InitPanel(const char* szFile, GGUIPanelSpaceType eType);
	virtual bool InitPanelByUIContainer(GGUIWindowContainer* pContainer, GGUIPanelSpaceType eType);
	virtual void ClearPanel();
	virtual void ShowPanel();
	virtual void HidePanel();
	virtual void ProcessUIEvent(int nEventType, void* pParam);
	virtual void OnResolutionChanged(float newWidth, float newHeight);

	const SoTinyString& GetFileName() const;
	const GGUIWindowContainer* GetUIContainer() const;
	void SetSpaceType(GGUIPanelSpaceType eType);
	GGUIPanelSpaceType GetSpaceType() const;
	bool IsPanelVisible() const;

	GGUIWindowBase* FindWindowByName(const char* szName) const;
	GGUIWindowBase* FindWindowByID(int nWindowID) const;

protected:
	bool LoadPanel(const char* szFile);

protected:
	GGUIWindowContainer* m_pContainer;
	SoTinyString m_kFileName;
	GGUIPanelSpaceType m_eSpaceType;
};
//------------------------------------------------------------
inline const SoTinyString& GGUIPanel::GetFileName() const
{
	return m_kFileName;
}
//------------------------------------------------------------
inline const GGUIWindowContainer* GGUIPanel::GetUIContainer() const
{
	return m_pContainer;
}
//------------------------------------------------------------
inline GGUIPanelSpaceType GGUIPanel::GetSpaceType() const
{
	return m_eSpaceType;
}
//------------------------------------------------------------
inline bool GGUIPanel::IsPanelVisible() const
{
	if (m_pContainer)
	{
		return m_pContainer->GetVisible();
	}
	else
	{
		return false;
	}
}
//------------------------------------------------------------
#endif //_GGUIPanel_h_
//------------------------------------------------------------
