//---------------------------------------------------------------
#include "SoGameTime.h"
#include <Windows.h>
//---------------------------------------------------------------
SoGameTime* SoGameTime::ms_pInstance = 0;
//---------------------------------------------------------------
void SoGameTime::CreateGameTime()
{
	if (ms_pInstance == 0)
	{
		ms_pInstance = new SoGameTime;
		ms_pInstance->InitGameTime();
	}
}
//---------------------------------------------------------------
void SoGameTime::ReleaseGameTime()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = 0;
	}
}
//---------------------------------------------------------------
SoGameTime::SoGameTime()
{
	//什么都不做。
}
//---------------------------------------------------------------
SoGameTime::~SoGameTime()
{
	ClearGameTime();
}
//---------------------------------------------------------------
void SoGameTime::InitGameTime()
{
	LARGE_INTEGER kCounter;
	QueryPerformanceCounter(&kCounter);

	m_GameStartTime = kCounter.QuadPart;
	m_GameAccTime = 0;
	m_LastFrameEndTime = kCounter.QuadPart;
	m_CurrentFrameEndTime = 0;

	m_nMaxFPS = 60;
	m_nCurrentFPS = 0;
	m_nAccFrame = 0;
	m_fFrameDeltaTime = 0.0f;

	QueryPerformanceFrequency(&kCounter);
	m_TimerFrequence = kCounter.QuadPart;
	m_CurrentAccFrequence = 0;
	m_FrameIntervalAtMaxFPS = m_TimerFrequence / m_nMaxFPS;
}
//---------------------------------------------------------------
void SoGameTime::ClearGameTime()
{
	//什么都不做。
}
//---------------------------------------------------------------
float SoGameTime::FrameBegin()
{
	return m_fFrameDeltaTime;
}
//---------------------------------------------------------------
void SoGameTime::FrameEnd()
{
	LARGE_INTEGER kCounter;
	__int64 deltaTime = 0;
	while (1)
	{
		QueryPerformanceCounter(&kCounter);
		m_CurrentFrameEndTime = kCounter.QuadPart;
		deltaTime = m_CurrentFrameEndTime - m_LastFrameEndTime;
		if (deltaTime < m_FrameIntervalAtMaxFPS)
		{
			Sleep(0);
		}
		else
		{
			break;
		}
	}

	m_GameAccTime = m_CurrentFrameEndTime - m_GameStartTime;
	m_LastFrameEndTime = m_CurrentFrameEndTime;
	m_CurrentFrameEndTime = 0;
	
	m_fFrameDeltaTime = (float)(((double)deltaTime) / ((double)m_TimerFrequence));

	++m_nAccFrame;
	m_CurrentAccFrequence += deltaTime;

	if (m_CurrentAccFrequence >= m_TimerFrequence)
	{
		//一秒钟过去了
		while (m_CurrentAccFrequence >= m_TimerFrequence)
		{
			m_CurrentAccFrequence -= m_TimerFrequence;
		}

		m_nCurrentFPS = m_nAccFrame;
		m_nAccFrame = 0;
	}
}
//---------------------------------------------------------------
void SoGameTime::SetMaxFPS(int nMaxFPS)
{
	if (nMaxFPS > 0)
	{
		m_nMaxFPS = nMaxFPS;
		m_FrameIntervalAtMaxFPS = m_TimerFrequence / m_nMaxFPS;
	}
}
//---------------------------------------------------------------
int SoGameTime::GetCurrentFPS()
{
	return m_nCurrentFPS;
}
//---------------------------------------------------------------
