//----------------------------------------------------------------
#ifndef _WinInputMsgManager_h_
#define _WinInputMsgManager_h_
//----------------------------------------------------------------
#include "SoArray.h"
#include "WinInputMsg.h"
//----------------------------------------------------------------
class WinInputMsgManager
{
public:
	static bool CreateInputMsgManager();
	static void ReleaseInputMsgManager();
	static WinInputMsgManager* Get();

	//获取某个键位当前是否处于按下状态。
	bool IsKeyDown(eInputKeyType theKey);
	//获取某个键位是否刚刚按下，即上一帧没有按下，当前帧按下了。
	bool IsKeyJustDown(eInputKeyType theKey);
	//获取某个键位是否刚刚抬起，即上一帧处于按下，当前帧抬起了。
	bool IsKeyJustUp(eInputKeyType theKey);
	//在一帧开始时，把当前帧的键位状态保存到上一帧的键位状态中。
	void SaveKeyDownState();
	//接收并缓存Windows消息中的Input消息。
	int ReceiveInputMsg(unsigned int uiMsg, unsigned int wParam, unsigned int lParam);
	//把Input消息分发给游戏逻辑。
	void DispatchInputMsg();
	//处理Windows窗口激活消息与反激活消息。
	void ProcessWindowActivateChangeMsg();

	//特殊逻辑处理。
	void SetMouseMoveForCameraOp(bool bFlag);

private:
	WinInputMsgManager();
	~WinInputMsgManager();
	bool InitInputMsgManager();
	void ClearInputMsgManager();

	//把所有的按下状态都清除掉。
	void ClearKeyDownState();
	eInputKeyType ConvertWinVirtualKeyCode2InputKeyType(unsigned int uiVirtualKeyCode);
	int ConvertInputKeyType2WinVirtualKeyCode(eInputKeyType theKey);

private:
	static WinInputMsgManager* ms_pInstance;
	SoArray m_kMsgArray1;
	SoArray m_kMsgArray2;
	//当前负责接收消息的数组。
	SoArray* m_pReceive;
	//当前负责分发消息的数组。
	SoArray* m_pDispatch;
	//记录上一帧的键位状态，是否处于按下状态。
	bool m_bKeyDownState_PreFrame[InputKey_Max];
	//记录当前帧的键位状态，是否处于按下状态。
	bool m_bKeyDownState[InputKey_Max];
	//特殊逻辑处理。
	bool m_bMouseMove_CameraOp;
};
//----------------------------------------------------------------
inline WinInputMsgManager* WinInputMsgManager::Get()
{
	return ms_pInstance;
}
//----------------------------------------------------------------
inline bool WinInputMsgManager::IsKeyDown(eInputKeyType theKey)
{
	return m_bKeyDownState[theKey];
}
//----------------------------------------------------------------
inline bool WinInputMsgManager::IsKeyJustDown(eInputKeyType theKey)
{
	return (m_bKeyDownState_PreFrame[theKey] == false && m_bKeyDownState[theKey] == true);
}
//----------------------------------------------------------------
inline bool WinInputMsgManager::IsKeyJustUp(eInputKeyType theKey)
{
	return (m_bKeyDownState_PreFrame[theKey] == true && m_bKeyDownState[theKey] == false);
}
//----------------------------------------------------------------
inline void WinInputMsgManager::SetMouseMoveForCameraOp(bool bFlag)
{
	m_bMouseMove_CameraOp = bFlag;
}
//----------------------------------------------------------------
#endif //_WinInputMsgManager_h_
//----------------------------------------------------------------
