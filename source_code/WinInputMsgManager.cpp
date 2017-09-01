//----------------------------------------------------------------
//1，为了收到像WM_LBUTTONDBLCLK、WM_MBUTTONDBLCLK、WM_RBUTTONDBLCLK等鼠标双击消息，需要
//   在Window的Style属性中添加CS_DBLCLKS，但这个时候发现，Windows省去了一些单击消息。
//   所以比较好的方式还是自己模拟鼠标双击消息。
//2，响应WM_ACTIVATE消息，游戏窗口失去活动状态，则通过发送ButtonUp来取消鼠标键“按住”的状态残留。
//3，响应WM_MOUSELEAVE消息，鼠标离开游戏界面，则通过发送ButtonUp来取消鼠标键“按住”的状态残留。
//----------------------------------------------------------------
#include "WinInputMsgManager.h"
#include "WinInputMsgDispatch.h"
#include "SoSimpleLog.h"
#include <Windows.h>
//----------------------------------------------------------------
WinInputMsgManager* WinInputMsgManager::ms_pInstance = 0;
//----------------------------------------------------------------
bool WinInputMsgManager::CreateInputMsgManager()
{
	bool br = true;
	if (ms_pInstance == 0)
	{
		ms_pInstance = new WinInputMsgManager;
		if (ms_pInstance && ms_pInstance->InitInputMsgManager())
		{
			br = true;
		}
		else
		{
			ReleaseInputMsgManager();
			br = false;
		}
	}
	return br;
}
//----------------------------------------------------------------
void WinInputMsgManager::ReleaseInputMsgManager()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = 0;
	}
}
//----------------------------------------------------------------
WinInputMsgManager::WinInputMsgManager()
:m_pReceive(0)
,m_pDispatch(0)
,m_bMouseMove_CameraOp(false)
{

}
//----------------------------------------------------------------
WinInputMsgManager::~WinInputMsgManager()
{
	ClearInputMsgManager();
}
//----------------------------------------------------------------
bool WinInputMsgManager::InitInputMsgManager()
{
	if (m_kMsgArray1.InitArray(sizeof(stInputEvent), 10, 10) == false)
	{
		return false;
	}
	if (m_kMsgArray2.InitArray(sizeof(stInputEvent), 10, 10) == false)
	{
		return false;
	}
	//
	m_pReceive = &m_kMsgArray1;
	m_pDispatch = &m_kMsgArray2;
	ClearKeyDownState();
	m_bMouseMove_CameraOp = false;
	return true;
}
//----------------------------------------------------------------
void WinInputMsgManager::ClearInputMsgManager()
{
	m_pReceive = 0;
	m_pDispatch = 0;
	m_kMsgArray1.ClearArray();
	m_kMsgArray2.ClearArray();
}
//----------------------------------------------------------------
void WinInputMsgManager::SaveKeyDownState()
{
	memcpy(m_bKeyDownState_PreFrame, m_bKeyDownState, sizeof(m_bKeyDownState_PreFrame));
}
//----------------------------------------------------------------
int WinInputMsgManager::ReceiveInputMsg(unsigned int uiMsg, unsigned int wParam, unsigned int lParam)
{
	stInputEvent kInputEvent;

	switch (uiMsg)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			kInputEvent.theEvent = InputEvent_Down;
			kInputEvent.theKey = ConvertWinVirtualKeyCode2InputKeyType(wParam);
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		{
			kInputEvent.theEvent = InputEvent_Up;
			kInputEvent.theKey = ConvertWinVirtualKeyCode2InputKeyType(wParam);
		}
		break;
	case WM_LBUTTONDOWN:
		{
			kInputEvent.theEvent = InputEvent_Down;
			kInputEvent.theKey = InputKey_LMouse;
			int xPos = (short)LOWORD(lParam);
			int yPos = (short)HIWORD(lParam);
			kInputEvent.nParam1 = xPos;
			kInputEvent.nParam2 = yPos;
		}
		break;
	case WM_LBUTTONUP:
		{
			kInputEvent.theEvent = InputEvent_Up;
			kInputEvent.theKey = InputKey_LMouse;
			int xPos = (short)LOWORD(lParam);
			int yPos = (short)HIWORD(lParam);
			kInputEvent.nParam1 = xPos;
			kInputEvent.nParam2 = yPos;
		}
		break;
	case WM_MBUTTONDOWN:
		{
			kInputEvent.theEvent = InputEvent_Down;
			kInputEvent.theKey = InputKey_MMouse;
			int xPos = (short)LOWORD(lParam);
			int yPos = (short)HIWORD(lParam);
			kInputEvent.nParam1 = xPos;
			kInputEvent.nParam2 = yPos;
		}
		break;
	case WM_MBUTTONUP:
		{
			kInputEvent.theEvent = InputEvent_Up;
			kInputEvent.theKey = InputKey_MMouse;
			int xPos = (short)LOWORD(lParam);
			int yPos = (short)HIWORD(lParam);
			kInputEvent.nParam1 = xPos;
			kInputEvent.nParam2 = yPos;
		}
		break;
	case WM_RBUTTONDOWN:
		{
			kInputEvent.theEvent = InputEvent_Down;
			kInputEvent.theKey = InputKey_RMouse;
			int xPos = (short)LOWORD(lParam);
			int yPos = (short)HIWORD(lParam);
			kInputEvent.nParam1 = xPos;
			kInputEvent.nParam2 = yPos;
		}
		break;
	case WM_RBUTTONUP:
		{
			kInputEvent.theEvent = InputEvent_Up;
			kInputEvent.theKey = InputKey_RMouse;
			int xPos = (short)LOWORD(lParam);
			int yPos = (short)HIWORD(lParam);
			kInputEvent.nParam1 = xPos;
			kInputEvent.nParam2 = yPos;
		}
		break;
	case WM_MOUSEMOVE:
		{
			eInputKeyType theKey = InputKey_Invalid;
			if (wParam & MK_LBUTTON)
			{
				//鼠标左键处于按下状态
				theKey = InputKey_LMouse;
			}
			else if (wParam & MK_RBUTTON)
			{
				//鼠标右键处于按下状态
				theKey = InputKey_RMouse;
			}
			else if (wParam & MK_MBUTTON)
			{
				//鼠标中键处于按下状态
				theKey = InputKey_MMouse;
			}
			int xPos = (short)LOWORD(lParam);
			int yPos = (short)HIWORD(lParam);
			//
			if (m_bMouseMove_CameraOp)
			{
				kInputEvent.theEvent = InputEvent_MouseMove_CameraOp;
			}
			else
			{
				kInputEvent.theEvent = InputEvent_MouseMove;
			}
			kInputEvent.theKey = theKey;
			kInputEvent.nParam1 = xPos;
			kInputEvent.nParam2 = yPos;
		}
		break;
	case WM_MOUSEWHEEL:
		{
			int nDelta = (short)HIWORD(wParam);
			kInputEvent.theEvent = InputEvent_MouseWheel;
			kInputEvent.theKey = InputKey_MMouse;
			kInputEvent.nParam1 = nDelta;
		}
		break;
	case WM_MOUSELEAVE:
		{
		}
		break;
	case WM_CHAR:
		{
		}
		break;
	case WM_IME_CHAR:
		{
		}
		break;
	}

	if (kInputEvent.theEvent == InputEvent_Invalid)
	{
		//没有处理消息，返回-1。
		return -1;
	}
	if (kInputEvent.theKey == InputKey_Invalid)
	{
		if (kInputEvent.theEvent == InputEvent_MouseMove || kInputEvent.theEvent == InputEvent_MouseMove_CameraOp)
		{
			//此情况下，key为无效值是正常情况，什么都不做。
		}
		else
		{
			//不能识别的key值。
			return -1;
		}
	}

	//更新键位状态
	if (kInputEvent.theEvent == InputEvent_Down)
	{
		m_bKeyDownState[kInputEvent.theKey] = true;
		kInputEvent.bIsJustDown = IsKeyJustDown(kInputEvent.theKey);
	}
	else if (kInputEvent.theEvent == InputEvent_Up)
	{
		m_bKeyDownState[kInputEvent.theKey] = false;
		kInputEvent.bIsJustUp = IsKeyJustUp(kInputEvent.theKey);
	}

	//判断辅助键位的状态
	kInputEvent.bShiftKeyDown = IsKeyDown(InputKey_Shift);
	kInputEvent.bCtrlKeyDown = IsKeyDown(InputKey_Ctrl);
	kInputEvent.bAltKeyDown = IsKeyDown(InputKey_Alt);

	//存储Input事件。
	if (m_pReceive)
	{
		m_pReceive->PushBack(&kInputEvent);
	}
	else
	{
		SoLogError("WinInputMsgManager::ReceiveInputMsg : m_pReceive == NULL");
	}
	//处理了消息，返回0。
	return 0;
}
//----------------------------------------------------------------
void WinInputMsgManager::DispatchInputMsg()
{
	if (m_pReceive == 0)
	{
		return;
	}
	if (m_pReceive->GetSize() == 0)
	{
		return;
	}

	if (m_pDispatch)
	{
		m_pDispatch->ClearArray();
	}
	SoArray* pTemp = m_pDispatch;
	m_pDispatch = m_pReceive;
	m_pReceive = pTemp;

	stInputEvent* pEvent = 0;
	const int nCount = m_pDispatch->GetSize();
	for (int i = 0; i < nCount; ++i)
	{
		pEvent = (stInputEvent*)m_pDispatch->GetAt(i);
		if (pEvent)
		{
			WinInputMsgDispatch_Process(pEvent);
		}
	}
}
//----------------------------------------------------------------
void WinInputMsgManager::ProcessWindowActivateChangeMsg()
{
	//把所有处于按下状态的键位，都处理成抬起状态。
	unsigned int uiMsg = 0;
	unsigned int wParam = 0;
	unsigned int lParam = 0;
	for (int i = 0; i < InputKey_Max; ++i)
	{
		if (m_bKeyDownState[i] == true)
		{
			if (i == InputKey_LMouse)
			{
				uiMsg = WM_LBUTTONUP;
			}
			else if (i == InputKey_MMouse)
			{
				uiMsg = WM_MBUTTONUP;
			}
			else if (i == InputKey_RMouse)
			{
				uiMsg = WM_RBUTTONUP;
			}
			else
			{
				uiMsg = WM_KEYUP;
				wParam = ConvertInputKeyType2WinVirtualKeyCode((eInputKeyType)i);
			}
			ReceiveInputMsg(uiMsg, wParam, lParam);
		}
	}
}
//----------------------------------------------------------------
void WinInputMsgManager::ClearKeyDownState()
{
	memset(m_bKeyDownState_PreFrame, 0, sizeof(m_bKeyDownState_PreFrame));
	memset(m_bKeyDownState, 0, sizeof(m_bKeyDownState));
}
//----------------------------------------------------------------
eInputKeyType WinInputMsgManager::ConvertWinVirtualKeyCode2InputKeyType(unsigned int uiVirtualKeyCode)
{
	switch (uiVirtualKeyCode)
	{
	case 'A': return InputKey_A;
	case 'B': return InputKey_B;
	case 'C': return InputKey_C;
	case 'D': return InputKey_D;
	case 'E': return InputKey_E;
	case 'F': return InputKey_F;
	case 'G': return InputKey_G;
	case 'H': return InputKey_H;
	case 'I': return InputKey_I;
	case 'J': return InputKey_J;
	case 'K': return InputKey_K;
	case 'L': return InputKey_L;
	case 'M': return InputKey_M;
	case 'N': return InputKey_N;
	case 'O': return InputKey_O;
	case 'P': return InputKey_P;
	case 'Q': return InputKey_Q;
	case 'R': return InputKey_R;
	case 'S': return InputKey_S;
	case 'T': return InputKey_T;
	case 'U': return InputKey_U;
	case 'V': return InputKey_V;
	case 'W': return InputKey_W;
	case 'X': return InputKey_X;
	case 'Y': return InputKey_Y;
	case 'Z': return InputKey_Z;
	case VK_F1:	return InputKey_F1;
	case VK_F2:	return InputKey_F2;
	case VK_F3:	return InputKey_F3;
	case VK_SPACE: return InputKey_Space;
	case VK_DELETE:	return InputKey_Delete;
	case VK_SHIFT: return InputKey_Shift;
	case VK_LSHIFT: return InputKey_Shift;
	case VK_RSHIFT: return InputKey_Shift;
	case VK_CONTROL: return InputKey_Ctrl;
	case VK_LCONTROL: return InputKey_Ctrl;
	case VK_RCONTROL: return InputKey_Ctrl;
	case VK_MENU: return InputKey_Alt;
	case VK_LMENU: return InputKey_Alt;
	case VK_RMENU: return InputKey_Alt;
	}
	SoLogError("WinInputMsgManager::ConvertWinVirtualKeyCode2InputKeyType : Unknown VirtualKeyCode[%d]", uiVirtualKeyCode);
	return InputKey_Invalid;
}
//----------------------------------------------------------------
int WinInputMsgManager::ConvertInputKeyType2WinVirtualKeyCode(eInputKeyType theKey)
{
	switch (theKey)
	{
	case InputKey_A: return 'A';
	case InputKey_B: return 'B';
	case InputKey_C: return 'C';
	case InputKey_D: return 'D';
	case InputKey_E: return 'E';
	case InputKey_F: return 'F';
	case InputKey_G: return 'G';
	case InputKey_H: return 'H';
	case InputKey_I: return 'I';
	case InputKey_J: return 'J';
	case InputKey_K: return 'K';
	case InputKey_L: return 'L';
	case InputKey_M: return 'M';
	case InputKey_N: return 'N';
	case InputKey_O: return 'O';
	case InputKey_P: return 'P';
	case InputKey_Q: return 'Q';
	case InputKey_R: return 'R';
	case InputKey_S: return 'S';
	case InputKey_T: return 'T';
	case InputKey_U: return 'U';
	case InputKey_V: return 'V';
	case InputKey_W: return 'W';
	case InputKey_X: return 'X';
	case InputKey_Y: return 'Y';
	case InputKey_Z: return 'Z';
	case InputKey_F1: return VK_F1;
	case InputKey_F2: return VK_F2;
	case InputKey_F3: return VK_F3;
	case InputKey_Space: return VK_SPACE;
	case InputKey_Delete: return VK_DELETE;
	case InputKey_Shift: return VK_SHIFT;
	case InputKey_Ctrl: return VK_CONTROL;
	case InputKey_Alt: return VK_MENU;
	}
	SoLogError("WinInputMsgManager::ConvertInputKeyType2WinVirtualKeyCode : Unknown InputKeyType[%d]", theKey);
	return 0;
}
//----------------------------------------------------------------
