//----------------------------------------------------------------
#ifndef _WinApp_h_
#define _WinApp_h_
//----------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
//----------------------------------------------------------------
class WinApp
{
public:
	static void CreateWinApp();
	static void ReleaseWinApp();
	static WinApp* Get();

	//创建Windows窗口
	bool CreateAppWindow(HINSTANCE hInstance);
	//销毁Windows窗口
	void ReleaseAppWindow();

	HWND GetHWND();
	int GetClientWidth();
	int GetClientHeight();

	void SetClientWidthHeight(int nWidth, int nHeight);
	bool GetWindowSizeAble();

	void SetWindowActive(bool bActive);
	bool GetWindowActive();

	void SetWindowMinimize(bool bMini);
	bool GetWindowMinimize();

	bool GetBlockMsgLoopWhenDragWindow();

private:
	WinApp();
	~WinApp();
	//如果本窗口m_hWND属于高权限进程，那么本窗口无法收到来自低权限进程的Windows消息。
	//下面这个函数修改过滤规则，使得本窗口能够收到来自低权限进程的Windows消息。
	void myChangeWindowMessageFilter();

private:
	static WinApp* ms_pInstance;
	HINSTANCE m_hInstance; //进程句柄
	HWND m_hWND; //窗口句柄
	const char* m_szWindowTitle; //窗口标题栏字符串
	const char* m_szWCName; //窗口类的名字
	int m_nClientWidth; //窗口客户区的宽
	int m_nClientHeight; //窗口客户区的高
	DWORD m_dwBgColor; //背景画刷的颜色
	bool m_bCreateBrush; //是否创建背景画刷。有些情况下不需要创建背景画刷，例如在D3D或者OpenGL程序里面。
	bool m_bSizeAble; //是否可以调整窗口宽高
	bool m_bAcceptFiles; //窗口是否响应“有文件拖放到窗口内”的消息
	bool m_bActive; //窗口当前是否处于激活状态
	bool m_bMinimize; //窗口当前是否处于最小化状态
	bool m_bBlockMsgLoopWhenDragWindow; //当鼠标拖动窗口时，是否阻塞消息循环。如果阻塞消息循环，则游戏画面不能更新。
};
//----------------------------------------------------------------
inline WinApp* WinApp::Get()
{
	return ms_pInstance;
}
//----------------------------------------------------------------
inline HWND WinApp::GetHWND()
{
	return m_hWND;
}
//----------------------------------------------------------------
inline int WinApp::GetClientWidth()
{
	return m_nClientWidth;
}
//----------------------------------------------------------------
inline int WinApp::GetClientHeight()
{
	return m_nClientHeight;
}
//----------------------------------------------------------------
inline void WinApp::SetClientWidthHeight(int nWidth, int nHeight)
{
	m_nClientWidth = nWidth;
	m_nClientHeight = nHeight;
}
//----------------------------------------------------------------
inline bool WinApp::GetWindowSizeAble()
{
	return m_bSizeAble;
}
//----------------------------------------------------------------
inline void WinApp::SetWindowActive(bool bActive)
{
	m_bActive = bActive;
}
//----------------------------------------------------------------
inline bool WinApp::GetWindowActive()
{
	return m_bActive;
}
//----------------------------------------------------------------
inline void WinApp::SetWindowMinimize(bool bMini)
{
	m_bMinimize = bMini;
}
//----------------------------------------------------------------
inline bool WinApp::GetWindowMinimize()
{
	return m_bMinimize;
}
//----------------------------------------------------------------
inline bool WinApp::GetBlockMsgLoopWhenDragWindow()
{
	return m_bBlockMsgLoopWhenDragWindow;
}
//----------------------------------------------------------------
#endif //_WinApp_h_
//----------------------------------------------------------------
