#include "AudioSound.h"

ENGINE_USING

AudioSound::AudioSound() :
	m_pSoundInfo(nullptr),
	m_bBGM(false),
	m_bLoop(false),
	m_bPlay(false),
	m_pChannel(nullptr)
{
	m_strFileName = "";
	m_eComponentType = CT_SOUND;
}

AudioSound::AudioSound(const AudioSound & _Com) : Component(_Com)
{
	*this = _Com;
	m_iReferenceCount = 1;
}

AudioSound::~AudioSound()
{
}

void AudioSound::SetSound(const std::string & _strName)
{
	m_pSoundInfo = GET_SINGLETON(SoundManager)->FindSound(_strName);

	m_strFileName = _strName;
}

// ¾ê´Â ·Îµå ÈÄ ¼ÂÆÃ
void AudioSound::SetSound(const std::string & _strName, 
	Scene * _pScene, 
	bool _bLoop, 
	const char * _pFileName, const std::string & _strPathName)
{
	if (false == GET_SINGLETON(SoundManager)->LoadSound(_strName, _pScene, _bLoop, _pFileName, _strPathName))
	{
		return;
	}

	m_pSoundInfo = GET_SINGLETON(SoundManager)->FindSound(_strName);
	m_strFileName = _strName;
}

void AudioSound::EnableBGM(bool _Enable)
{
	m_bBGM = _Enable;
}

void AudioSound::Play()
{
	if (false == m_bPlay)
	{
		m_pChannel->stop();

		std::list<Channel*>::iterator Iter = m_pSoundInfo->ChannelList.begin();
		std::list<Channel*>::iterator EndIter = m_pSoundInfo->ChannelList.end();

		for (; Iter != EndIter; ++Iter)
		{
			if (*Iter == m_pChannel)
			{
				m_pSoundInfo->ChannelList.erase(Iter);
				break;
			}
		}

		m_pChannel = nullptr;
	}

	m_bPlay = true;
	GET_SINGLETON(SoundManager)->Play(m_pSoundInfo);

	m_pChannel = m_pSoundInfo->ChannelList.back();
}

void AudioSound::Stop()
{
	if (true == m_bPlay)
	{
		m_pChannel->stop();

		std::list<Channel*>::iterator Iter = m_pSoundInfo->ChannelList.begin();
		std::list<Channel*>::iterator EndIter = m_pSoundInfo->ChannelList.end();

		for (; Iter != EndIter; ++Iter)
		{
			if (*Iter == m_pChannel)
			{
				m_pSoundInfo->ChannelList.erase(Iter);
				break;
			}
		}
	}
}

// Àá±ñ ¸ØÃß´Â°Å
void AudioSound::Pause()
{
}

////////////////////////////////////////////////////////

void AudioSound::Start()
{
	if (true == m_bPlay)
	{
		if (nullptr != m_pChannel)
		{
			m_pChannel->stop();

			std::list<Channel*>::iterator Iter = m_pSoundInfo->ChannelList.begin();
			std::list<Channel*>::iterator EndIter = m_pSoundInfo->ChannelList.end();

			for (; Iter != EndIter; ++Iter)
			{
				if (*Iter == m_pChannel)
				{
					m_pSoundInfo->ChannelList.erase(Iter);
					break;
				}
			}

			m_pChannel = nullptr;
		}

		GET_SINGLETON(SoundManager)->Play(m_pSoundInfo);

		m_pChannel = m_pSoundInfo->ChannelList.back();
	}
}

bool AudioSound::Init()
{
	return true;
}

int AudioSound::Input(float _fTime)
{
	return 0;
}

int AudioSound::Update(float _fTime)
{
	if (true == m_bPlay)
	{
		bool bIsPlay = false;
		m_pChannel->isPlaying(&bIsPlay);

		if (false == bIsPlay)
		{
			m_bPlay = false;
			m_pChannel = nullptr;
		}

	}

	return 0;
}

int AudioSound::LateUpdate(float _fTime)
{
	return 0;
}

int AudioSound::Collision(float _fTime)
{
	return 0;
}

int AudioSound::PrevRender(float _fTime)
{
	return 0;
}

int AudioSound::Render(float _fTime)
{
	return 0;
}

AudioSound * AudioSound::Clone() const
{
	return new AudioSound(*this);
}
