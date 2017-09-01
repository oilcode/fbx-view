//----------------------------------------------------------------
#ifndef _GGUIWindowFactory_h_
#define _GGUIWindowFactory_h_
//----------------------------------------------------------------
#include "GGUIWindowBase.h"
#include "GGUIPanel.h"
//----------------------------------------------------------------
class GGUIWindowFactory
{
public:
	static bool CreateUIWindowFactory();
	static void ReleaseUIWindowFactory();
	static GGUIWindowFactory* Get();

	GGUIWindowBase* CreateUIWindow(GGUIWindowType eType);
	void DeleteUIWindow(int nWindowID);
	GGUIWindowBase* GetUIWindow(int nWindowID);

	void UISpace_AddPanel(GGUIPanel* pPanel, int eSpaceType);
	void UISpace_RemovePanel(GGUIPanel* pPanel, int eSpaceType);
	void UISpace_SetRenderOrder(GGUIPanel* pPanel, int nIndex);
	void UISpace_SetVisible(int eSpaceType, bool bVisible);
	bool UISpace_GetVisible(int eSpaceType);
	int UISpace_GetPanelCount(int eSpaceType);
	GGUIPanel* UISpace_GetPanel(int eSpaceType, int nIndex);

private:
	GGUIWindowFactory();
	~GGUIWindowFactory();
	bool InitUIWindowFactory();
	void ClearUIWindowFactory();
	int FindUnusedWindow(GGUIWindowType eType);

private:
	static GGUIWindowFactory* ms_pInstance;
	//本类负责创建，删除，维护所有的GGUIWindowBase对象。
	SoArrayUID m_kWindowArray;
	int m_nUnusedObjectIndex[GGUIWindow_Max];
	//本类只维护GGUIPanel对象的指针，由外界负责创建和删除GGUIPanel对象。
	SoArray m_kUISpaceArray[GGUIPanelSpace_Max];
	bool m_bUISpaceVisible[GGUIPanelSpace_Max];
};
//----------------------------------------------------------------
inline GGUIWindowFactory* GGUIWindowFactory::Get()
{
	return ms_pInstance;
}
//----------------------------------------------------------------
#endif //_GGUIWindowFactory_h_
//----------------------------------------------------------------
