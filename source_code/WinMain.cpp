//----------------------------------------------------------------
#include "WinApp.h"
#include "WinAppHelp.h"
//----------------------------------------------------------------
//程序入口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	do 
	{
		bool bEventResult = true;

		bEventResult = WinAppHelp_OnPreCreateWinApp();
		if (bEventResult == false)
		{
			//某些模块初始化失败
			break;
		}
		WinApp::CreateWinApp();
		bEventResult = WinAppHelp_OnPostCreateWinApp();
		if (bEventResult == false)
		{
			//某些模块初始化失败
			break;
		}

		bEventResult = WinAppHelp_OnPreCreateAppWindow();
		if (bEventResult == false)
		{
			//某些模块初始化失败
			break;
		}
		bool bCreateAppWindow = WinApp::Get()->CreateAppWindow(hInstance);
		if (bCreateAppWindow == false)
		{
			break;
		}
		bEventResult = WinAppHelp_OnPostCreateAppWindow();
		if (bEventResult == false)
		{
			//某些模块初始化失败
			break;
		}

		bEventResult = WinAppHelp_OnPreShowCreatedAppWindow();
		if (bEventResult == false)
		{
			//某些模块初始化失败
			break;
		}
		HWND mainHWND = WinApp::Get()->GetHWND();
		ShowWindow(mainHWND, SW_SHOW);
		SetForegroundWindow(mainHWND); //稍微提高优先级
		SetFocus(mainHWND); //设置键盘的焦点至此窗口
		bEventResult = WinAppHelp_OnPostShowCreatedAppWindow();
		if (bEventResult == false)
		{
			//某些模块初始化失败
			break;
		}

		//消息主循环
		bool bDone = false;
		MSG msg;
		while (bDone == false)
		{
			//我们要做的第一件事是检查是否有消息在等待。
			//使用PeekMessage()可以在不锁住我们的程序的前提下对消息进行检查。
			//许多程序使用GetMessage()，也可以很好的工作。
			//但使用GetMessage()，程序在收到paint消息或其他别的什么窗口消息之前不会做任何事。
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				//检查是否出现退出消息。
				//如果当前的消息是由PostQuitMessage(0)引起的WM_QUIT，则退出主循环。
				if (msg.message == WM_QUIT)
				{
					bDone = true;
				}
				else
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			else
			{
				//没有Windows消息，更新和绘制我们的游戏场景。
				WinAppHelp_OnAppUpdate();
				WinAppHelp_OnAppRender();
			}
		} //消息主循环

	} while (0);

	//程序退出，释放流程
	WinAppHelp_OnPreReleaseWinApp();
	WinApp::ReleaseWinApp();
	WinAppHelp_OnPostReleaseWinApp();
	return 0;
}
//----------------------------------------------------------------
//窗口消息处理
LRESULT CALLBACK myWindowMsgProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//本函数的返回值，取决于窗口消息类型。不同的消息ID，要求的返回值不同。
	//不需要处理 WM_DESTROY 消息。
	switch (uMsg)
	{
	case WM_CLOSE:
		{
			//Windows窗口将被关闭。这里抛出退出消息，主循环将被中断，然后执行退出流程。
			PostQuitMessage(0);
			return 0;
		}
		break;
	}

	const int nResult = WinAppHelp_WindowMsgProc(uMsg, wParam, lParam);
	if (nResult == -1)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	else
	{
		return nResult;
	}
}
//------------------------------------------------------------
