#pragma once
#include "Engine.h"

ENGINE_BEGIN

class ENGINE_DLL TimerManager
{
private:
	std::unordered_map<std::string, class Timer*>	m_TimerMap;

public:
	bool Init();
	bool CreateTimer(const std::string& _strName);
	class Timer* FindTimer(const std::string& _strName);

public:
	DECLARE_SINGLETON(TimerManager)
};

ENGINE_END

