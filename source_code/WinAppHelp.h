//----------------------------------------------------------------
#ifndef _WinAppHelp_h_
#define _WinAppHelp_h_
//----------------------------------------------------------------
#include "WinApp.h"
//----------------------------------------------------------------
bool WinAppHelp_OnPreCreateWinApp();
bool WinAppHelp_OnPostCreateWinApp();
bool WinAppHelp_OnPreCreateAppWindow();
bool WinAppHelp_OnPostCreateAppWindow();
bool WinAppHelp_OnPreShowCreatedAppWindow();
bool WinAppHelp_OnPostShowCreatedAppWindow();
void WinAppHelp_OnPreReleaseWinApp();
void WinAppHelp_OnPostReleaseWinApp();
void WinAppHelp_OnAppUpdate();
void WinAppHelp_OnAppRender();
//处理窗口消息。
//返回-1，表示函数内部没有处理消息；
//返回其他值，表示函数内部处理了消息。不同的消息ID，返回值也不同。
int WinAppHelp_WindowMsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
//有一个文件拖拽到本窗口内了。
void WinAppHelp_WindowMsg_OnDropFile(const char* szDropFile);
//----------------------------------------------------------------
#endif //_WinAppHelp_h_
//----------------------------------------------------------------
