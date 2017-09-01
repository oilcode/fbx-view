//------------------------------------------------------------
#ifndef _GGUIWindowContainer_h_
#define _GGUIWindowContainer_h_
//------------------------------------------------------------
#include "GGUIWindowBase.h"
//------------------------------------------------------------
class GGUIWindowContainer : public GGUIWindowBase
{
public:
	void AddChild(GGUIWindowBase* pChild);
	void RemoveChildByIndex(int nIndex);
	void RemoveChildByID(int nWindowID);
	void RemoveAllChild();
	void SwapChild(int nIndexA, int nIndexB);

	int GetChildCount() const;
	GGUIWindowBase* GetChildByIndex(int nIndex) const;
	GGUIWindowBase* GetChildByID(int nWindowID) const;
	GGUIWindowBase* GetChildByName(const char* szName) const;
	GGUIWindowBase* FindWindowByID(int nWindowID) const;
	GGUIWindowBase* FindWindowByName(const char* szName) const;
	int GetIndexByID(int nWindowID) const;

	//re-write parent function
	void SetFullRect(const GGUIFullRect& kRect);
	void SetScalePos(float fScaleX, float fScaleY);
	void SetDeltaPos(float fDeltaX, float fDeltaY);
	void SetAbsDeltaPos(float fAbsDeltaX, float fAbsDeltaY);
	void MoveDelta(float fDeltaX, float fDeltaY);
	void OnParentRectChanged(const GGUIRect& kParentRectInAbsCoord);
	void SetVisibleByReason(souint32 uiReason, bool bVisible);

protected:
	friend class GGUIWindowFactory;
	GGUIWindowContainer();
	~GGUIWindowContainer();
	void DoRemoveChild(GGUIWindowBase* pChild);
	void Notify_ParentRectChanged() const;
	void Notify_ParentVisibleChanged() const;
	//re-write parent function
	void ClearWindow();

private:
	SoArray m_kWindowArray;
};
//----------------------------------------------------------------
inline int GGUIWindowContainer::GetChildCount() const
{
	return m_kWindowArray.GetSize();
}
//------------------------------------------------------------
#endif //_GGUIWindowContainer_h_
//------------------------------------------------------------
