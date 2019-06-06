#include "Timer.h"
#include "Core.h"

ENGINE_USING

Timer::Timer()
{
}


Timer::~Timer()
{
}

float Timer::GetTime() const
{
	return m_fDeltaTime;
}

float Timer::GetFPS() const
{
	return m_fFPS;
}

bool Timer::Init()
{
	QueryPerformanceFrequency(&m_tSecond);
	QueryPerformanceCounter(&m_tTick);

	m_fDeltaTime = 0.0f;
	m_fFPS = 0.0f;
	m_fFPSTime = 0.0f;
	m_iTick = 0;

	return true;
}

void Timer::Update()
{
	LARGE_INTEGER	tTick;

	QueryPerformanceCounter(&tTick);

	m_fDeltaTime = (tTick.QuadPart - m_tTick.QuadPart) / (float)(m_tSecond.QuadPart);

	m_tTick = tTick;

	m_fFPSTime += m_fDeltaTime;
	++m_iTick;

	if (60 <= m_iTick)
	{
		m_fFPS = m_iTick / m_fFPSTime;
		m_fFPSTime = 0.0f;
		m_iTick = 0;

		// 윈도우 타이틀바에 FPS 띄우기
		char strFPS[256] = {};

		sprintf_s(strFPS, "FPS : %.5f", m_fFPS);
		SetWindowTextA(WINDOWHANDLE, strFPS);

		return;
	}
}
