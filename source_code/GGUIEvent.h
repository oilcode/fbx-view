//------------------------------------------------------------
#ifndef _GGUIEvent_h_
#define _GGUIEvent_h_
//------------------------------------------------------------
enum GGUIEventType
{
	GGUIEvent_Invalid = -1,
	GGUIEvent_WindowRectEnter,
	GGUIEvent_WindowRectLeave,
	GGUIEvent_ButtonClick,
};
//------------------------------------------------------------
struct GGUIEventParam_WindowRectEnter
{
	const char* szWindowName;
	int nWindowID;
};
//------------------------------------------------------------
struct GGUIEventParam_WindowRectLeave
{
	const char* szWindowName;
	int nWindowID;
};
//------------------------------------------------------------
struct GGUIEventParam_ButtonClick
{
	const char* szWindowName;
	int nWindowID;
};
//------------------------------------------------------------
#endif //_GGUIEvent_h_
//------------------------------------------------------------
