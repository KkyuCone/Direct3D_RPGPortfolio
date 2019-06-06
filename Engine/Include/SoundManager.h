#pragma once
#include "Engine.h"
#include "fmod.hpp"

using namespace FMOD;

ENGINE_BEGIN

typedef struct ENGINE_DLL _tagSoundInfo
{
	Sound*	pSound;
	bool	bLoop;
	class Scene* pScene;
	std::list<Channel*> ChannelList;

}SoundInfo, *PSoundInfo;


class ENGINE_DLL SoundManager
{
private:
	System* m_pSystem;
	std::unordered_map<std::string, PSoundInfo> m_mapSound;

public:
	System* GetSoundSystem() const;

public:
	bool Init();
	bool LoadSound(const std::string& _strName, class Scene* _pScene,
		bool _bLoop, const char* _pFileName,
		const std::string& _strPathName = PATH_SOUND);

	void Play(const std::string& _strName);
	void Play(PSoundInfo _pSound);

	void Stop(const std::string& _strName);
	void Stop(PSoundInfo _pSound);

	void Update(float _fTime);

	void DeleteSound(class Scene* _pScene);

	PSoundInfo FindSound(const std::string& _strName);

	DECLARE_SINGLETON(SoundManager);
};

ENGINE_END