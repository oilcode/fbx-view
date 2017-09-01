//---------------------------------------------------------------
#ifndef _SoGameTime_h_
#define _SoGameTime_h_
//---------------------------------------------------------------
class SoGameTime
{
public:
	static void CreateGameTime();
	static void ReleaseGameTime();
	static SoGameTime* Get();

	float FrameBegin();
	void FrameEnd();

	//设置游戏帧率上限。
	void SetMaxFPS(int nMaxFPS);
	//获取当前的游戏帧率。
	int GetCurrentFPS();

private:
	SoGameTime();
	~SoGameTime();
	void InitGameTime();
	void ClearGameTime();

private:
	static SoGameTime* ms_pInstance;
	//游戏启动时的时刻。
	__int64 m_GameStartTime;
	//游戏运行时长。
	__int64 m_GameAccTime;
	//上一帧结束的时刻。也即本帧开始的时刻。
	__int64 m_LastFrameEndTime;
	//本帧结束的时刻。
	__int64 m_CurrentFrameEndTime;
	//计时器的频率，也即一秒钟走多少个数。
	//该值只需获取一次，它不会发生变化。
	__int64 m_TimerFrequence;
	//记录本秒钟内，已经走了多少个数。
	__int64 m_CurrentAccFrequence;
	//在帧率达到上限的情况下，每帧走多少个数。
	__int64 m_FrameIntervalAtMaxFPS;

	//游戏帧率上限
	int m_nMaxFPS;
	//当前的游戏帧率
	int m_nCurrentFPS;
	//记录本秒钟内，已经走了多少帧。
	int m_nAccFrame;

	float m_fFrameDeltaTime;
};
//---------------------------------------------------------------
inline SoGameTime* SoGameTime::Get()
{
	return ms_pInstance;
}
//---------------------------------------------------------------
#endif
//---------------------------------------------------------------
