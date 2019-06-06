#include "SoundManager.h"
#include "PathManager.h"

ENGINE_USING

DEFINITION_SINGLETON(SoundManager)

SoundManager::SoundManager() :
	m_pSystem(nullptr)
{
}


SoundManager::~SoundManager()
{
	std::unordered_map<std::string, PSoundInfo>::iterator Iter = m_mapSound.begin();
	std::unordered_map<std::string, PSoundInfo>::iterator EndIter = m_mapSound.end();

	for (; Iter != EndIter; ++Iter)
	{
		Iter->second->pSound->release();
		SAFE_DELETE(Iter->second);
	}

	m_mapSound.clear();

	if (nullptr != m_pSystem)
	{
		m_pSystem->close();
		m_pSystem->release();
	}
}


System * SoundManager::GetSoundSystem() const
{
	return m_pSystem;
}

bool SoundManager::Init()
{
	// System 생성
	System_Create(&m_pSystem);

	// 시스템 초기화 init(채널 갯수, 초가화 방식, 다른 드라이버)
	m_pSystem->init(FMOD_MAX_CHANNEL_WIDTH, FMOD_INIT_NORMAL, nullptr);

	return true;
}

// 사운드 로드
bool SoundManager::LoadSound(const std::string & _strName,
	Scene * _pScene,
	bool _bLoop, 
	const char * _pFileName, 
	const std::string & _strPathName /*= PATH_SOUND*/)
{
	PSoundInfo pInfo = FindSound(_strName);

	// 이미 해당 ㅇ사운드가 있다면 return false
	if (nullptr != pInfo)
	{
		return false;
	}

	pInfo = new SoundInfo;

	pInfo->bLoop = _bLoop;

	const char* pPath = GET_SINGLETON(PathManager)->FindPathMultibyte(_strPathName);

	std::string strPath = "";

	if (nullptr != pPath)
	{
		strPath = pPath;
	}

	strPath += _pFileName;

	FMOD_MODE eMode = FMOD_LOOP_NORMAL;			// 반복

	if (false == _bLoop)
	{
		eMode = FMOD_DEFAULT;					// 한번
	}

	// 사운드로드(파일명, 초기화방식, 음원정보, 사운드 음원포인터)
	if (FMOD_OK != m_pSystem->createSound(strPath.c_str(), eMode, nullptr, &pInfo->pSound))
	{
		SAFE_DELETE(pInfo);
		return false;
	}

	pInfo->pScene = _pScene;

	m_mapSound.insert(std::make_pair(_strName, pInfo));

	return true;
}

void SoundManager::Play(const std::string & _strName)
{
	PSoundInfo pSoundInfo = FindSound(_strName);

	// 없으면 return
	if (nullptr == pSoundInfo)
		return;

	Channel* pChannel = nullptr;
	m_pSystem->playSound(pSoundInfo->pSound, nullptr, false, &pChannel);

	pSoundInfo->ChannelList.push_back(pChannel);
}

void SoundManager::Play(PSoundInfo _pSound)
{
	Channel* pChannel = nullptr;
	m_pSystem->playSound(_pSound->pSound, nullptr, false, &pChannel);

	_pSound->ChannelList.push_back(pChannel);
}

void SoundManager::Stop(const std::string & _strName)
{
	PSoundInfo pSoundInfo = FindSound(_strName);

	if (nullptr == pSoundInfo)
	{
		return;
	}

	std::list<Channel*>::iterator Iter = pSoundInfo->ChannelList.begin();
	std::list<Channel*>::iterator EndIter = pSoundInfo->ChannelList.end();

	for (; Iter != EndIter; ++Iter)
	{
		(*Iter)->stop();
	}

	pSoundInfo->ChannelList.clear();
}

void SoundManager::Stop(PSoundInfo _pSound)
{
	if (nullptr == _pSound)
	{
		return;
	}

	std::list<Channel*>::iterator Iter = _pSound->ChannelList.begin();
	std::list<Channel*>::iterator EndIter = _pSound->ChannelList.end();

	for (; Iter != EndIter; ++Iter)
	{
		(*Iter)->stop();
	}

	_pSound->ChannelList.clear();
}

void SoundManager::Update(float _fTime)
{
	std::unordered_map<std::string, PSoundInfo>::iterator Iter = m_mapSound.begin();
	std::unordered_map<std::string, PSoundInfo>::iterator EndIter = m_mapSound.end();

	for (; Iter != EndIter; ++Iter)
	{
		std::list<Channel*>::iterator ChannelIter = Iter->second->ChannelList.begin();
		std::list<Channel*>::iterator ChannelEndIter = Iter->second->ChannelList.end();

		for (; ChannelIter != ChannelEndIter;)
		{
			bool bIsplay = false;
			(*ChannelIter)->isPlaying(&bIsplay);

			if (false == bIsplay)
			{
				ChannelIter = Iter->second->ChannelList.erase(ChannelIter);
				ChannelIter = Iter->second->ChannelList.end();
			}
			else
			{
				++ChannelIter;
			}
		}
	}
}

// 해당 씬에 존재하는 모든 사운드 지우기
void SoundManager::DeleteSound(Scene * _pScene)
{
	std::unordered_map<std::string, PSoundInfo>::iterator Iter = m_mapSound.begin();
	std::unordered_map<std::string, PSoundInfo>::iterator EndIter = m_mapSound.end();

	for (; Iter != EndIter;)
	{
		if (Iter->second->pScene == _pScene)
		{
			Iter->second->pSound->release();
			SAFE_DELETE(Iter->second);
			Iter = m_mapSound.erase(Iter);
		}
		else
		{
			++Iter;
		}
	}
}

PSoundInfo SoundManager::FindSound(const std::string & _strName)
{
	std::unordered_map<std::string, PSoundInfo>::iterator Iter = m_mapSound.find(_strName);

	if (Iter == m_mapSound.end())
	{
		return nullptr;
	}

	return Iter->second;
}