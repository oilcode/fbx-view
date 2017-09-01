//----------------------------------------------------------------
#include "WinAppHelp.h"
#include <ShellAPI.h>
#include "WinInputMsgManager.h"
#include "SoSimpleLog.h"
#include "SoGameTime.h"
#include "SoCursor.h"
#include "SoCoroutine.h"
#include "SoStringHelp.h"
#include "SoD3DLogicFlowHelp.h"
#include "GGUILogicFlowHelp.h"
#include "NwUILogicFlowHelp.h"
//----------------------------------------------------------------
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//为“当鼠标拖动窗口时，不要阻塞消息循环”逻辑服务的变量。
bool g_bNcLButtonDown_NoBlockLoop = false;
POINT g_kLastMousePos_NoBlockLoop;
POINT g_kCurMousePos_NoBlockLoop;
RECT g_kCurWindowRect_NoBlockLoop;
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//公共的临时内存块。
char g_TempBuff[512];
//----------------------------------------------------------------
bool WinAppHelp_OnPreCreateWinApp()
{
	if (SoSimpleLog::CreateSimpleLog() == false)
	{
		return false;
	}
	if (WinInputMsgManager::CreateInputMsgManager() == false)
	{
		return false;
	}
	SoGameTime::CreateGameTime();
	if (SoCoroutineManager::CreateCoroutineManager() == false)
	{
		return false;
	}
	return true;
}
//----------------------------------------------------------------
bool WinAppHelp_OnPostCreateWinApp()
{
	return true;
}
//----------------------------------------------------------------
bool WinAppHelp_OnPreCreateAppWindow()
{
	return true;
}
//----------------------------------------------------------------
bool WinAppHelp_OnPostCreateAppWindow()
{
	SoCursor::CreateSoCursor();
	SoCursor::Get()->LoadCursorAndApply("texture/attack.cur", true);
	if (SoD3DLogicFlowHelp_Create() == false)
	{
		return false;
	}
	if (GGUILogicFlowHelp_Create() == false)
	{
		return false;
	}
	if (NwUILogicFlowHelp_Create() == false)
	{
		return false;
	}
	return true;
}
//----------------------------------------------------------------
bool WinAppHelp_OnPreShowCreatedAppWindow()
{
	return true;
}
//----------------------------------------------------------------
bool WinAppHelp_OnPostShowCreatedAppWindow()
{
	return true;
}
//----------------------------------------------------------------
void WinAppHelp_OnPreReleaseWinApp()
{
	NwUILogicFlowHelp_Release();
	GGUILogicFlowHelp_Release();
	SoD3DLogicFlowHelp_Release();
	SoCursor::ReleaseSoCursor();
}
//----------------------------------------------------------------
void WinAppHelp_OnPostReleaseWinApp()
{
	SoCoroutineManager::ReleaseCoroutineManager();
	SoGameTime::ReleaseGameTime();
	WinInputMsgManager::ReleaseInputMsgManager();
	SoSimpleLog::ReleaseSimpleLog();
}
//----------------------------------------------------------------
void WinAppHelp_OnAppUpdate()
{
	float fDeltaTime = SoGameTime::Get()->FrameBegin();
	//在一帧的开始，把缓存起来的input消息分发出去。
	WinInputMsgManager::Get()->DispatchInputMsg();
	//
	if (SoCoroutineManager::Get())
	{
		SoCoroutineManager::Get()->UpdateCoroutineManager(fDeltaTime);
	}
	///////////////////////////////////////
	SoD3DLogicFlowHelp_Update(fDeltaTime);
	GGUILogicFlowHelp_Update(fDeltaTime);
	NwUILogicFlowHelp_Update(fDeltaTime);
	///////////////////////////////////////

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//“当鼠标拖动窗口时，不要阻塞消息循环”逻辑。
	if (g_bNcLButtonDown_NoBlockLoop)
	{
		//有时候客户端会卡顿，一旦不能收到窗口消息，g_bNcLButtonDown_NoBlockLoop可能会一直为true，导致bug。
		//解决办法是，这里总是判断鼠标左键是否按下。
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			::GetCursorPos(&g_kCurMousePos_NoBlockLoop);
			const int nDeltaX = g_kCurMousePos_NoBlockLoop.x - g_kLastMousePos_NoBlockLoop.x;
			const int nDeltaY = g_kCurMousePos_NoBlockLoop.y - g_kLastMousePos_NoBlockLoop.y;
			if (nDeltaX < -1 || nDeltaX > 1 || nDeltaY < -1 || nDeltaY > 1)
			{
				g_kCurWindowRect_NoBlockLoop.left += nDeltaX;
				g_kCurWindowRect_NoBlockLoop.top += nDeltaY;
				g_kLastMousePos_NoBlockLoop = g_kCurMousePos_NoBlockLoop;
				//执行移动操作的时候不要更改窗口size。
				::SetWindowPos(WinApp::Get()->GetHWND(), HWND_NOTOPMOST, g_kCurWindowRect_NoBlockLoop.left, g_kCurWindowRect_NoBlockLoop.top, g_kCurWindowRect_NoBlockLoop.right, g_kCurWindowRect_NoBlockLoop.bottom, SWP_NOSIZE);
			}
		}
		else
		{
			g_bNcLButtonDown_NoBlockLoop = false;
		}
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//在一帧的末尾，把当前帧的键位状态保存到上一帧的键位状态中。
	WinInputMsgManager::Get()->SaveKeyDownState();
	SoGameTime::Get()->FrameEnd();
}
//----------------------------------------------------------------
void WinAppHelp_OnAppRender()
{
	SoD3DLogicFlowHelp_PreRender();
	GGUILogicFlowHelp_PreRender();
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	if (SoD3DLogicFlowHelp_BeginRender() == false)
	{
		return;
	}
	GGUILogicFlowHelp_Render();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	SoD3DLogicFlowHelp_EndRender();
}
//----------------------------------------------------------------
int WinAppHelp_WindowMsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//处理Input消息
	bool bIsInputEvent = false;
	if (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST)
	{
		bIsInputEvent = true;
	}
	else if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST)
	{
		bIsInputEvent = true;
	}
	else if (uMsg == WM_IME_CHAR)
	{
		bIsInputEvent = true;
	}
	if (bIsInputEvent)
	{
		return WinInputMsgManager::Get()->ReceiveInputMsg(uMsg, (unsigned int)wParam, (unsigned int)lParam);
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	switch (uMsg)
	{
	case WM_ACTIVATE:
		{
			//窗口激活状态消息的处理
			bool bActive = true;
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				bActive = false;
			}
			WinApp::Get()->SetWindowActive(bActive);
			WinInputMsgManager::Get()->ProcessWindowActivateChangeMsg();
			return 0;
		}
	case WM_SIZE:
		{
			//窗口大小改变之后，就会得到这个消息
			if (WinApp::Get()->GetWindowSizeAble())
			{
				int nClientW = LOWORD(lParam);
				int nClientH = HIWORD(lParam);
				WinApp::Get()->SetClientWidthHeight(nClientW, nClientH);
				return 0;
			}
			break;
		}
	case WM_SYSCOMMAND:
		{
			switch (wParam & 0xfff0)
			{
			case SC_MINIMIZE: //最小化
				{
					WinApp::Get()->SetWindowMinimize(true);
				} break;
			case SC_RESTORE: //还原
				{
					WinApp::Get()->SetWindowMinimize(false);
					//下面这个逻辑，备用。
					////-获得窗体参数
					//WINDOWPLACEMENT kOldWndPlacement;
					//GetWindowPlacement(_hWnd, &kOldWndPlacement);
					////-如果是最大化还原,修改图形配置
					//if (kOldWndPlacement.showCmd == SW_SHOWMAXIMIZED)
					//{
					//	rItem = false;
					//}
				} break;
			case SC_KEYMENU:
			case SC_MOUSEMENU:
				{
					//SC_KEYMENU消息是按快捷键的时候产生的系统菜单，这时候消息循环也是停止的，
					//直到DefWindowProc处理完成返回。
					//SC_MOUSEMENU消息是鼠标左键点击左上角小图标是产生的系统菜单，同上。
					if (WinApp::Get()->GetBlockMsgLoopWhenDragWindow() == false)
					{
						//不要弹出系统菜单。
						return 0;
					}
				} break;
			case SC_MOVE:
				{
					//当鼠标在标题栏上按住不放时，系统会发送WM_SYSCOMMAND消息，当DefWindowProc
					//收到SC_MOVE后，会发送WM_ENTERSIZEMOVE，这个时候整个消息循环就会停止，直到
					//DefWindowProc处理完成返回，所以这个消息不能交给系统处理，直接返回。
					if (WinApp::Get()->GetBlockMsgLoopWhenDragWindow() == false)
					{
						if (g_bNcLButtonDown_NoBlockLoop == false)
						{
							g_bNcLButtonDown_NoBlockLoop = true;
							::GetCursorPos(&g_kLastMousePos_NoBlockLoop);
							::GetWindowRect(WinApp::Get()->GetHWND(), &g_kCurWindowRect_NoBlockLoop);
						}
						return 0;
					}
				} break;
			case SC_CLOSE:
				{
					/*
					//用户触发了应用程序关闭的事件，拦截这个消息，我们要让用户再次确认是否关闭游戏。
					//1，用户点击了标题栏右侧的关闭按钮；
					//2，用户双击了标题栏左侧的应用程序图标；
					//3，用户按下了 ctrl+F4 组合键；
					//4，用户在任务栏上右击应用程序，在弹出的系统菜单中选择了关闭按钮；
					const int nMsgResult = MessageBoxA(WinApp::Get()->GetHWND(), "要退出游戏吗？", "??", MB_YESNO);
					if (nMsgResult == IDOK || nMsgResult == IDYES)
					{
						//什么都不做，交给 DefWindowProc 去处理。
					}
					else
					{
						//不要退出游戏。
						return 0;
					}
					*/
				} break;
			default:
				break;
			}
			//返回-1，让窗口的默认消息处理函数继续处理。
		} break;
	case WM_SETCURSOR:
		{
			//从其他窗口切换到本窗口时会触发这个消息。
			//如果使用了自定义的光标，则必须重新设置一下自定义光标。
			if (SoCursor::Get())
			{
				if (SoCursor::Get()->ResetCurrentCursor() == true)
				{
					return 0;
				}
			}
		} break;
	case WM_NCLBUTTONUP:
		{
			//在窗口的非客户区（主要是标题栏），鼠标抬起了，表示不再拖动窗口。
			//返回-1，交给DefWindowProc去处理。
			g_bNcLButtonDown_NoBlockLoop = false;
		} break;
	case WM_NCRBUTTONDOWN:
		{
			//在标题栏上点击了右键。如果弹出系统菜单，会导致游戏循环卡住。
			if (WinApp::Get()->GetBlockMsgLoopWhenDragWindow() == false)
			{
				//不要弹出系统菜单。
				return 0;
			}
		} break;
	case WM_DROPFILES:
		{
			HDROP hDrop = (HDROP)wParam;
			DragQueryFile(hDrop, 0, g_TempBuff, 256);
			DragFinish(hDrop);
			WinAppHelp_WindowMsg_OnDropFile(g_TempBuff);
			return 0;
		} break;
	//case WM_PAINT:
	//	{
	//		//static PAINTSTRUCT ps;
	//		//BeginPaint(hWnd, &ps);
	//		//EndPaint(hWnd, &ps);
	//		myDrawText();
	//		return 0;
	//	}
	}
	
	return -1;
}
//----------------------------------------------------------------
void WinAppHelp_WindowMsg_OnDropFile(const char* szDropFile)
{
	if (szDropFile == 0 || szDropFile[0] == 0)
	{
		return;
	}

	const int nLen = SoStrLen(szDropFile);
	const int nPos = SoStrRChr(szDropFile, '.', nLen);
	if (nPos == -1)
	{
		return;
	}

	const char* pExtend = szDropFile + nPos + 1;
	if (SoStrCmpNoCase(pExtend, "fbx") == 0)
	{
		SoD3DLogicFlowHelp_OnDropFileFbx(szDropFile);
	}
	else if (SoStrCmpNoCase(pExtend, "kkb") == 0)
	{
		SoD3DLogicFlowHelp_OnDropFileKkb(szDropFile);
	}
	else if (SoStrCmpNoCase(pExtend, "kkf") == 0)
	{
		SoD3DLogicFlowHelp_OnDropFileKkf(szDropFile);
	}
	else if (SoStrCmpNoCase(pExtend, "dds") == 0
		|| SoStrCmpNoCase(pExtend, "png") == 0
		|| SoStrCmpNoCase(pExtend, "tga") == 0
		)
	{
		SoD3DLogicFlowHelp_OnDropFileImage(szDropFile);
	}
}
//----------------------------------------------------------------
