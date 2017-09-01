//-----------------------------------------------------------------------------
#include "SoCursor.h"
#include "WinApp.h"
//-----------------------------------------------------------------------------
SoCursor* SoCursor::ms_pInstance = NULL;
//-----------------------------------------------------------------------------
void SoCursor::CreateSoCursor()
{
	if (ms_pInstance == NULL)
	{
		ms_pInstance = new SoCursor;
	}
}
//-----------------------------------------------------------------------------
void SoCursor::ReleaseSoCursor()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = NULL;
	}
}
//-----------------------------------------------------------------------------
SoCursor::SoCursor()
:m_CurrentCursor(NULL)
,m_szCurrentCursor(NULL)
{

}
//-----------------------------------------------------------------------------
SoCursor::~SoCursor()
{
	m_CurrentCursor = NULL;
	m_szCurrentCursor = NULL;
	m_mapName2Cursor.clear();
}
//-----------------------------------------------------------------------------
bool SoCursor::LoadCursorAndApply(const char* szFileName, bool bApply)
{
	if (szFileName == NULL)
	{
		return false;
	}
	if (szFileName[0] == 0)
	{
		//空字符串
		return false;
	}
	HCURSOR theCursor = NULL;
	const char* theName = "";
	std::string strFileName(szFileName);
	//判断光标文件是否已经加载
	mapName2Cursor::iterator it = m_mapName2Cursor.find(strFileName);
	if (it == m_mapName2Cursor.end())
	{
		//尚未加载过，尝试加载
		HCURSOR newCursor = ::LoadCursorFromFile(szFileName);
		if (newCursor != NULL)
		{
			m_mapName2Cursor[strFileName] = newCursor;
			it = m_mapName2Cursor.find(strFileName);
		}
	}
	if (it != m_mapName2Cursor.end())
	{
		theCursor = it->second;
		theName = (it->first).c_str();
	}
	//是否要立即应用该光标。
	if (bApply && theCursor)
	{
		::SetCursor(theCursor);
		m_CurrentCursor = theCursor;
		m_szCurrentCursor = theName;
	}
	return (theCursor != NULL);
}
//-----------------------------------------------------------------------------
bool SoCursor::ResetCurrentCursor()
{
	if (m_CurrentCursor)
	{
		::SetCursor(m_CurrentCursor);
		return true;
	}
	return false;
}
//-----------------------------------------------------------------------------
void SoCursor::SetCursorVisible(bool bVisible)
{
	::ShowCursor(bVisible ? TRUE : FALSE);
}
//-----------------------------------------------------------------------------
void SoCursor::SetCursorCapture(bool bCapture)
{
	if (bCapture)
	{
		::SetCapture(WinApp::Get()->GetHWND());
	}
	else
	{
		::ReleaseCapture();
	}
}
//-----------------------------------------------------------------------------
void SoCursor::SetCursorPosition(int nPosX, int nPosY)
{
	::SetCursorPos(nPosX, nPosY);
}
//-----------------------------------------------------------------------------
void SoCursor::GetCursorPosition(int& nPosX, int& nPosY)
{
	POINT kPoint;
	::GetCursorPos(&kPoint);
	nPosX = kPoint.x;
	nPosY = kPoint.y;
}
//-----------------------------------------------------------------------------
