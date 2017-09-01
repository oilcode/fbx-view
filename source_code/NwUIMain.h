//----------------------------------------------------------------
#ifndef _NwUIMain_h_
#define _NwUIMain_h_
//----------------------------------------------------------------
#include "NwUIDefine.h"
//----------------------------------------------------------------
class NwUIMain : public GGUIPanel
{
public:
	static bool CreateUIMain();
	static void ReleaseUIMain();
	static NwUIMain* Get();

	void UpdateUIMain(float fDeltaTime);
	void SetLog(const char* szLog);
	void SetVertexCount(int nCount);
	void SetControlPointCount(int nCount);
	void SetAnimLength(float fLen);

private:
	NwUIMain();
	~NwUIMain();
	bool InitUIMain();
	void ClearUIMain();
	void ProcessUIEvent(int nEventType, void* pParam);

private:
	static NwUIMain* ms_pInstance;
	GGUIWindowText* m_pTextLog;
	GGUIWindowText* m_pUITextFPS;
	GGUIWindowText* m_pTextVertexCount;
	GGUIWindowText* m_pTextControlPointCount;
	GGUIWindowText* m_pTextAnimLength;
	GGUIWindowButton* m_pBtnConvert;
	float m_fAccTimeForFPS;
};
//----------------------------------------------------------------
inline NwUIMain* NwUIMain::Get()
{
	return ms_pInstance;
}
//----------------------------------------------------------------
#endif //_NwUIMain_h_
//----------------------------------------------------------------
