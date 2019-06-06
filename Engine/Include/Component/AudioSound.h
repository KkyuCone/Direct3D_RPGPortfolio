#pragma once
#include "Component.h"
#include "..\SoundManager.h"

ENGINE_BEGIN

class ENGINE_DLL AudioSound : public Component
{
private:
	friend class GameObject;

public:
	AudioSound();
	AudioSound(const AudioSound& _Com);
	virtual ~AudioSound();

private:
	std::string					m_strFileName;
	struct _tagSoundInfo*		m_pSoundInfo;
	Channel*		m_pChannel;
	bool			m_bBGM;
	bool			m_bLoop;
	bool			m_bPlay;

public:
	void SetSound(const std::string& _strName);
	void SetSound(const std::string& _strName, class Scene* _pScene,
		bool _bLoop, const char* _pFileName,
		const std::string& _strPathName = PATH_SOUND);

	void EnableBGM(bool _Enable);

	void Play();
	void Stop();
	void Pause();

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출, 즉 레이어에서 호출해준다.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// 렌더를 하기 직전에 호출해주는 함수ㅔㄱㅁ
	virtual int Render(float _fTime);
	virtual AudioSound* Clone()	const;

};

ENGINE_END