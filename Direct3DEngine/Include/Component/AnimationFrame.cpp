#include "AnimationFrame.h"
#include "..\Resource\Texture.h"
#include "..\Resource\ResourcesManager.h"
#include "Transform.h"
#include "Material.h"
#include "..\GameObject.h"
#include "..\Render\ShaderManager.h"

ENGINE_USING

AnimationFrame::AnimationFrame() :
	m_pCurClip(nullptr),
	m_pDefaultClip(nullptr),
	m_pMaterial(nullptr)
{
	m_eComponentType = CT_ANIMATIONFRAME;
}

AnimationFrame::AnimationFrame(const AnimationFrame & _Com)
{
	m_eComponentType = CT_ANIMATIONFRAME;

	std::unordered_map<std::string, PAnimationFramClip>::const_iterator StartIter = _Com.m_mapClip.begin();
	std::unordered_map<std::string, PAnimationFramClip>::const_iterator EndIter = _Com.m_mapClip.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		PAnimationFramClip pClip = new AnimationFrameClip;
		*pClip = *(StartIter->second);

		// �ؽ�ó�� ������ ���۷���ī���� ����
		if (nullptr != pClip->pTexture)
		{
			pClip->pTexture->AddReference();
		}

		// ���� Ŭ�� ����
		if (StartIter->first == _Com.m_pCurClip->strName)
		{
			m_pCurClip = pClip;
		}

		// ����Ʈ Ŭ������
		if (StartIter->first == _Com.m_pDefaultClip->strName)
		{
			m_pDefaultClip = pClip;
		}

		m_mapClip.insert(std::make_pair(StartIter->first, pClip));
	}

	m_pMaterial = nullptr;
}


AnimationFrame::~AnimationFrame()
{
	SAFE_RELEASE(m_pMaterial);

	std::unordered_map<std::string, PAnimationFramClip>::iterator StartIter = m_mapClip.begin();
	std::unordered_map<std::string, PAnimationFramClip>::iterator EndIter = m_mapClip.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		SAFE_RELEASE(StartIter->second->pTexture);
		SAFE_DELETE(StartIter->second);
	}

	m_mapClip.clear();
}

// ��Ʋ��
bool AnimationFrame::CreateClip(const std::string & _strName,
	ANIMATION_OPTION _eOption, 
	const Vector2 & _vTextureSize, 
	const std::vector<TextureCoord>& _vecCoord,
	float _fLimitTime, 
	int _iTextureRegister,
	const std::string & _strTextureName,
	const TCHAR * _pFileName, 
	const std::string & _strPathName)
{
	PAnimationFramClip pClip = FindClip(_strName);

	// �̹� �ִ� ���
	if (nullptr != pClip)
	{
		return false;
	}

	pClip = new AnimationFrameClip;
	pClip->strName = _strName;
	pClip->eType = AFT_ATLAS;
	pClip->eOption = _eOption;
	pClip->iFrame = 0;
	pClip->fFrameTime = _fLimitTime / _vecCoord.size();
	pClip->fPlayTime = 0.0f;
	pClip->fPlayLimitTime = _fLimitTime;
	pClip->vTextureSize = _vTextureSize;
	pClip->vecCoord = _vecCoord;
	pClip->iRegister = _iTextureRegister;

	GET_SINGLETON(ResourcesManager)->LoadTexture(_strTextureName, _pFileName, _strPathName);

	pClip->pTexture = GET_SINGLETON(ResourcesManager)->FindTexture(_strTextureName);

	// ���࿡ �ش� �ؽ�ó�� ���°��..
	if (nullptr == pClip->pTexture)
	{
		SAFE_DELETE(pClip);
		return false;
	}

	// ���ʷ� ������� ���� Ŭ���� �ƹ��͵� �����ϱ� �ӽ÷� �ֱ�
	if (nullptr == m_pCurClip)
	{
		m_pCurClip = pClip;
	}

	if (nullptr == m_pDefaultClip)
	{
		m_pDefaultClip = pClip;
	}

	m_mapClip.insert(std::make_pair(_strName, pClip));

	return true;
}

// ������ �ؽ�ó(�ؽ�ó ������)
bool AnimationFrame::CreateClip(const std::string & _strName,
	ANIMATION_OPTION _eOption, 
	const Vector2 & _vTextureSize, 
	const std::vector<TextureCoord>& _vecCoord,
	float _fLimitTime, 
	int _iTextureRegister,
	const std::string & _strTextureName, 
	const std::vector<TCHAR*>& _vecFileName,
	const std::string & _strPathName)
{
	PAnimationFramClip pClip = FindClip(_strName);

	// �̹� �ִ� ���
	if (nullptr != pClip)
	{
		return false;
	}

	pClip = new AnimationFrameClip;
	pClip->strName = _strName;
	pClip->eType = AFT_FRAME;
	pClip->eOption = _eOption;
	pClip->iFrame = 0;
	pClip->fFrameTime = _fLimitTime / _vecCoord.size();
	pClip->fPlayTime = 0.0f;
	pClip->fPlayLimitTime = _fLimitTime;
	pClip->vTextureSize = _vTextureSize;
	pClip->vecCoord = _vecCoord;
	pClip->iRegister = _iTextureRegister;

	GET_SINGLETON(ResourcesManager)->LoadTexture(_strTextureName, _vecFileName, _strPathName);

	pClip->pTexture = GET_SINGLETON(ResourcesManager)->FindTexture(_strTextureName);

	// ���࿡ �ش� �ؽ�ó�� ���°��..
	if (nullptr == pClip->pTexture)
	{
		SAFE_DELETE(pClip);
		return false;
	}

	// ���ʷ� ������� ���� Ŭ���� �ƹ��͵� �����ϱ� �ӽ÷� �ֱ�
	if (nullptr == m_pCurClip)
	{
		m_pCurClip = pClip;
	}

	if (nullptr == m_pDefaultClip)
	{
		m_pDefaultClip = pClip;
	}

	m_mapClip.insert(std::make_pair(_strName, pClip));

	return true;
}

bool AnimationFrame::SetCurrentClip(const std::string & _strName)
{
	PAnimationFramClip pClip = FindClip(_strName);

	if (nullptr == pClip) 
	{
		return false;
	}
	m_pCurClip = pClip;
	m_pCurClip->iFrame = 0;
	m_pCurClip->fPlayTime = 0.0f;
	return true;
}

bool AnimationFrame::SetDefaultClip(const std::string & _strName)
{
	PAnimationFramClip pClip = FindClip(_strName);

	if (nullptr == pClip)
	{
		return false;
	}
	m_pDefaultClip = pClip;
	return true;
}

bool AnimationFrame::ChangeClip(const std::string & _strName)
{
	if (m_pCurClip->strName == _strName)
		return true;

	// �ٲٱ� ���� Ŭ�� �ؽ�ó
	std::string strPrevName = m_pCurClip->pTexture->GetTag();
	
	if (false == SetCurrentClip(_strName))
	{
		return false;
	}

	// �ٲٱ�
	m_pMaterial->ChangeTextureSet(0, 0, m_pCurClip->iRegister, strPrevName, m_pCurClip->pTexture);
	return true;
}

PAnimationFramClip AnimationFrame::GetCurClip()
{
	return m_pCurClip;
}

PAnimationFramClip AnimationFrame::FindClip(const std::string & _strName)
{
	std::unordered_map<std::string, PAnimationFramClip>::iterator FindIter = m_mapClip.find(_strName);

	if (FindIter == m_mapClip.end())
	{
		return nullptr;
	}

	return FindIter->second;
}

void AnimationFrame::Start()
{
	SAFE_RELEASE(m_pMaterial);
	m_pMaterial = FindComponentFromType<Material>(CT_MATERIAL);

	if (nullptr == m_pMaterial)
	{
		m_pMaterial = m_pGameObject->AddComponent<Material>("Material");
	}

	m_pMaterial->DeleteTextureSet(0, 0);

	// Current Cilp�� Texture�� Material�� �����Ѵ�.
	m_pMaterial->AddTextureSet(0, 0, m_pCurClip->iRegister, m_pCurClip->pTexture);
}

bool AnimationFrame::Init()
{
	return true;
}

int AnimationFrame::Input(float _fTime)
{
	return 0;
}

int AnimationFrame::Update(float _fTime)
{
	m_pCurClip->fPlayTime += _fTime;

	// ������ ��������� while ���
	while (m_pCurClip->fPlayTime >= m_pCurClip->fFrameTime)
	{
		m_pCurClip->fPlayTime -= m_pCurClip->fFrameTime;
		++m_pCurClip->iFrame;

		// �ִϸ��̼� �� �������� ������
		if (m_pCurClip->iFrame == m_pCurClip->vecCoord.size())
		{
			m_pCurClip->iFrame = 0;

			// �ɼǿ����� ó�� 
			switch (m_pCurClip->eOption)
			{
			case ANIMATION_OPTION::AO_LOOP:
				break;
			case ANIMATION_OPTION::AO_ONCE_DESTROY:
				m_pGameObject->Active(false);				// ���ó��
				break;
			default:
				break;
			}
		}
	}

	// ������� ���� ä���ش�. -> PrevRender���� ������۸� ���̴��� ����
	m_tCBuffer.iAnimationFrameType = m_pCurClip->eType;
	m_tCBuffer.iAnimationOption = m_pCurClip->eOption;
	m_tCBuffer.iFrame = m_pCurClip->iFrame;
	m_tCBuffer.vTextureSize = m_pCurClip->vTextureSize;
	m_tCBuffer.vStart = m_pCurClip->vecCoord[m_pCurClip->iFrame].vStart;
	m_tCBuffer.vEnd = m_pCurClip->vecCoord[m_pCurClip->iFrame].vEnd;

	return 0;
}

int AnimationFrame::LateUpdate(float _fTime)
{
	return 0;
}

int AnimationFrame::Collision(float _fTime)
{
	return 0;
}

int AnimationFrame::PrevRender(float _fTime)
{
	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("AnimationFrame", &m_tCBuffer);
	return 0;
}
int AnimationFrame::Render(float _fTime)
{
	return 0;
}

Component * AnimationFrame::Clone() const
{
	return new AnimationFrame(*this);
}
