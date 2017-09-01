//----------------------------------------------------------------
//1，窗口创建完毕后，修改窗口title使用的函数是 SetWindowText ，
//   修改窗口风格 SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
//   修改消息响应函数 SetWindowLongW(m_hWnd, GWLP_WNDPROC, (LONG)DefWindowProc);
//
//2，如果本窗口m_hWND属于高权限进程，那么本窗口无法收到来自低权限进程的Windows消息。
//   下面这个函数修改过滤规则，使得本窗口能够收到来自低权限进程的Windows消息。
//   void myChangeWindowMessageFilter();
//
//3，为了收到像WM_LBUTTONDBLCLK、WM_MBUTTONDBLCLK、WM_RBUTTONDBLCLK等鼠标双击消息，需要
//   在Window的Style属性中添加CS_DBLCLKS，但这个时候发现，Windows省去了一些单击消息。
//   所以比较好的方式还是自己模拟鼠标双击消息。
//----------------------------------------------------------------
#include "WinApp.h"
//----------------------------------------------------------------
//窗口消息处理
extern LRESULT CALLBACK myWindowMsgProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//----------------------------------------------------------------
WinApp* WinApp::ms_pInstance = NULL;
//----------------------------------------------------------------
void WinApp::CreateWinApp()
{
	if (ms_pInstance == NULL)
	{
		ms_pInstance = new WinApp;
	}
}
//----------------------------------------------------------------
void WinApp::ReleaseWinApp()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = NULL;
	}
}
//----------------------------------------------------------------
WinApp::WinApp()
:m_hInstance(NULL)
,m_hWND(NULL)
,m_szWindowTitle("fbx-kfm-view")
,m_szWCName("my_dx11")
,m_nClientWidth(1024)
,m_nClientHeight(768)
,m_dwBgColor(0x999999)
,m_bCreateBrush(false)
,m_bSizeAble(false)
,m_bAcceptFiles(true)
,m_bActive(false)
,m_bMinimize(false)
,m_bBlockMsgLoopWhenDragWindow(false)
{

}
//----------------------------------------------------------------
WinApp::~WinApp()
{
	ReleaseAppWindow();
}
//----------------------------------------------------------------
bool WinApp::CreateAppWindow(HINSTANCE hInstance)
{
	//创建图标，使用默认的应用程序图标
	HICON hIcon = LoadIcon(NULL, IDI_APPLICATION);
	//创建光标，使用默认的光标
	HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW);
	//创建背景画刷
	HBRUSH hBrush = NULL;
	if (m_bCreateBrush)
	{
		hBrush = CreateSolidBrush(m_dwBgColor);
	}

	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; //移动时重画，并为窗口取得DC
	wc.lpfnWndProc = myWindowMsgProcess;
	wc.cbClsExtra = 0; //无额外窗口数据
	wc.cbWndExtra = 0; //无额外窗口数据
	wc.hInstance = hInstance;
	wc.hIcon = hIcon;
	wc.hCursor = hCursor;
	wc.hbrBackground = hBrush;
	wc.lpszMenuName = NULL; //不需要菜单
	wc.lpszClassName = m_szWCName;

	//注册窗口类
	if (RegisterClass(&wc) == 0)
	{
		return false;
	}

	//设置窗口风格
	DWORD dwWinStyle = WS_BORDER|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX; //|WS_MAXIMIZEBOX
	//根据m_bSizeAble的值,对窗口风格进行调整
	if (m_bSizeAble)
	{
		dwWinStyle |= WS_THICKFRAME;
	}
	DWORD dwWinStyleEx = 0;
	if (m_bAcceptFiles)
	{
		dwWinStyleEx |= WS_EX_ACCEPTFILES;
	}
	//窗口的初始位置
	int nPosX = 0;
	int nPosY = 0;
	//没有父窗口
	HWND hWndParent = NULL;
	//没有菜单
	HMENU hMenu = NULL;
	//没有需要传递的参数
	LPVOID lpParam = NULL;

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	RECT winRect;
	winRect.left = nPosX;
	winRect.top = nPosY;
	winRect.right = nPosX + m_nClientWidth;
	winRect.bottom = nPosY + m_nClientHeight;
	BOOL bHaveMenu = FALSE; //没有菜单栏
	BOOL bAdjustResult = AdjustWindowRectEx(&winRect, dwWinStyle, bHaveMenu, dwWinStyleEx);
	if (bAdjustResult == FALSE)
	{
		//出错了
	}
	const int nRealWidth = winRect.right-winRect.left;
	const int nRealHeight = winRect.bottom-winRect.top;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	HWND hWnd = CreateWindowEx(dwWinStyleEx, m_szWCName, m_szWindowTitle, dwWinStyle, 
		nPosX, nPosY, nRealWidth, nRealHeight,
		hWndParent, hMenu, hInstance, lpParam);
	if (hWnd == NULL) 
	{
		return false;
	}

	//前面的操作都成功了
	m_hInstance = hInstance;
	m_hWND = hWnd;
	//
	myChangeWindowMessageFilter();
	return true;
}
//----------------------------------------------------------------
void WinApp::ReleaseAppWindow()
{
	if (m_hWND)
	{
		//销毁窗口
		BOOL br = DestroyWindow(m_hWND);
		if (br == FALSE)
		{

		}
		m_hWND = NULL;
	}
	//反注册窗口类
	if (m_hInstance)
	{
		BOOL bResult = UnregisterClass(m_szWCName, m_hInstance);
		if (bResult == FALSE)
		{

		}
		m_hInstance = NULL;
	}
}
//----------------------------------------------------------------
void WinApp::myChangeWindowMessageFilter()
{
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	typedef struct tagCHANGEFILTERSTRUCT
	{
		DWORD cbSize;
		DWORD ExtStatus;
	} CHANGEFILTERSTRUCT, *PCHANGEFILTERSTRUCT;

#define MSGFLT_ALLOW 1
#define MSGFLT_DISALLOW 2
#define MSGFLT_RESET 0

	typedef BOOL (WINAPI *LPFUNC)(HWND, UINT, DWORD, PCHANGEFILTERSTRUCT);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	HMODULE hLib = LoadLibrary("user32.dll");
	if (hLib == NULL)
	{
		return;
	}
	LPFUNC dllFunc = (LPFUNC)GetProcAddress(hLib, "ChangeWindowMessageFilterEx");
	if (dllFunc == NULL)
	{
		FreeLibrary(hLib);
		return;
	}
	CHANGEFILTERSTRUCT chfit = { sizeof(CHANGEFILTERSTRUCT) };

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//用户在这里添加代码
	dllFunc(m_hWND, WM_COPYDATA, MSGFLT_ALLOW, &chfit);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	FreeLibrary(hLib);
}
//----------------------------------------------------------------
