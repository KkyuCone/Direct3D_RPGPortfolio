#pragma once
#include "Engine.h"

ENGINE_BEGIN

class ENGINE_DLL PathManager
{
private:
	std::unordered_map<std::string, TCHAR*>		m_PathMap;
	char	m_strChange[MAX_PATH];					// 전환할때 쓰려고 (와이드->멀티)만든 변수 ㅇㅅㅇ

public:
	bool Init();
	bool CreatePath(const std::string& _strKey, const TCHAR* _pAddPath,
		const std::string& _strBaseKey = PATH_ROOT);
	const TCHAR* FindPath(const std::string & _strKey);
	const char* FindPathMultibyte(const std::string& _strKey);

public:
	DECLARE_SINGLETON(PathManager)
};

ENGINE_END