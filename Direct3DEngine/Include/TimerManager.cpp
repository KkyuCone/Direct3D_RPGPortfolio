#include "TimerManager.h"
#include "Timer.h"

ENGINE_USING

DEFINITION_SINGLETON(TimerManager)


TimerManager::TimerManager()
{
}


TimerManager::~TimerManager()
{
	std::unordered_map<std::string, Timer*>::iterator StartIter = m_TimerMap.begin();
	std::unordered_map<std::string, Timer*>::iterator EndIter = m_TimerMap.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		SAFE_DELETE(StartIter->second);
	}

	m_TimerMap.clear();
}

bool TimerManager::Init()
{
	// 멀티스레드를 사용할때는 각 스레드마다 타이머를 따로 만들어야 한다. (그래서 타이머 관리자가 필요)
	// 현재는 메인스레드 한개
	CreateTimer("MainThread");

	return true;
}

bool TimerManager::CreateTimer(const std::string & _strName)
{
	Timer* pTimer = FindTimer(_strName);

	if (nullptr != pTimer)
	{
		return false;
	}

	pTimer = new Timer;

	if (false == pTimer->Init())
	{
		SAFE_DELETE(pTimer);
		return false;
	}

	m_TimerMap.insert(std::make_pair(_strName, pTimer));

	return true;
}

Timer * TimerManager::FindTimer(const std::string & _strName)
{
	std::unordered_map<std::string, class Timer*>::iterator FIter = m_TimerMap.find(_strName);

	if (FIter == m_TimerMap.end())
	{
		return nullptr;
	}

	return FIter->second;
}
