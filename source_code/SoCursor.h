//-----------------------------------------------------------------------------
#ifndef _SoCursor_h_
#define _SoCursor_h_
//-----------------------------------------------------------------------------
#include <string>
#include <map>
#include <Windows.h>
//-----------------------------------------------------------------------------
class SoCursor
{
public:
	static void CreateSoCursor();
	static void ReleaseSoCursor();
	static SoCursor* Get();

	//从磁盘上加载指定的光标，并维护起来。如果需要的话，就立即应用该光标。
	//函数内部会判断该光标是否已经加载过了，不会重复加载。
	//--szFileName 必须是".cur"文件或者".ani"文件。
	//--bApply 是否立即应用该光标。
	bool LoadCursorAndApply(const char* szFileName, bool bApply);

	//重新设置一下当前的光标。
	bool ResetCurrentCursor();

	//是否显示光标。
	void SetCursorVisible(bool bVisible);
	//是否捕获光标。
	//在属于当前线程的指定窗口里设置鼠标捕获。一旦窗口捕获了鼠标，所有鼠标输入都针对该窗口，无论光标是否在窗口的边界内。
	void SetCursorCapture(bool bCapture);

	//光标位置。
	void SetCursorPosition(int nPosX, int nPosY);
	void GetCursorPosition(int& nPosX, int& nPosY);

private:
	SoCursor();
	~SoCursor();
	typedef std::map<std::string, HCURSOR> mapName2Cursor;
	
private:
	static SoCursor* ms_pInstance;
	//这里只加载光标文件，中途不会释放光标。
	//据MSDN描述，LoadCursorFromFile得到的光标是共享资源，不允许释放掉。
	//当程序退出时会自动释放光标资源。
	mapName2Cursor m_mapName2Cursor;
	//记录当前正在使用的光标。
	HCURSOR m_CurrentCursor;
	const char* m_szCurrentCursor;
};
//-----------------------------------------------------------------------------
inline SoCursor* SoCursor::Get()
{
	return ms_pInstance;
}
//-----------------------------------------------------------------------------
#endif //_SoCursor_h_
//-----------------------------------------------------------------------------
