//----------------------------------------------------------------
#ifndef _WinInputMsg_h_
#define _WinInputMsg_h_
//----------------------------------------------------------------
//按键事件的枚举值
enum eInputEventType
{
	InputEvent_Invalid = -1, //无效值
	InputEvent_Down, //按钮被按下了
	InputEvent_Up, //按钮被抬起了
	InputEvent_MouseMove, //鼠标移动了
	InputEvent_MouseWheel, //鼠标滚轮滚动了
	InputEvent_MouseMove_CameraOp, //专门为摄像机旋转操作而服务的事件
	InputEvent_Max, //最大值
};
//----------------------------------------------------------------
//键盘和鼠标的按钮的枚举值
enum eInputKeyType
{
	InputKey_Invalid = -1, //无效值
	InputKey_A,
	InputKey_B,
	InputKey_C,
	InputKey_D,
	InputKey_E,
	InputKey_F,
	InputKey_G,
	InputKey_H,
	InputKey_I,
	InputKey_J,
	InputKey_K,
	InputKey_L,
	InputKey_M,
	InputKey_N,
	InputKey_O,
	InputKey_P,
	InputKey_Q,
	InputKey_R,
	InputKey_S,
	InputKey_T,
	InputKey_U,
	InputKey_V,
	InputKey_W,
	InputKey_X,
	InputKey_Y,
	InputKey_Z,
	InputKey_F1,
	InputKey_F2,
	InputKey_F3,
	InputKey_Space,
	InputKey_Delete,
	InputKey_Shift,
	InputKey_Ctrl,
	InputKey_Alt,
	InputKey_LMouse, //鼠标左键
	InputKey_MMouse, //鼠标中键
	InputKey_RMouse, //鼠标右键
	InputKey_Max,
};
//----------------------------------------------------------------
struct stInputEvent
{
	eInputEventType theEvent;
	eInputKeyType theKey;
	int nParam1;
	int nParam2;
	bool bIsJustDown; //如果是InputEvent_Down事件，该键位是否刚刚被按下
	bool bIsJustUp; //如果是InputEvent_Up事件，该键位是否刚刚被抬起
	bool bShiftKeyDown; //此时Shift键是否处于按下状态
	bool bCtrlKeyDown; //此时Ctrl键是否处于按下状态
	bool bAltKeyDown; //此时Alt键是否处于按下状态
	bool bSwallowed; //值为true，表示本消息已经被吞噬了，排在响应队列后面的模块将不能收到本消息。

	stInputEvent()
	:theEvent(InputEvent_Invalid), theKey(InputKey_Invalid), nParam1(0), nParam2(0)
	,bIsJustDown(false), bIsJustUp(false)
	,bShiftKeyDown(false), bCtrlKeyDown(false), bAltKeyDown(false), bSwallowed(false)
	{
	}
	void Swallow()
	{
		bSwallowed = true;
	}
};
//----------------------------------------------------------------
#endif //_WinInputMsg_h_
//----------------------------------------------------------------
