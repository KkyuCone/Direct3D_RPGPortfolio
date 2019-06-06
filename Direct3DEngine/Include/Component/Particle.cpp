#include "Particle.h"
#include "Transform.h"
#include "..\Scene\Scene.h"
#include "..\Render\ShaderManager.h"
#include "..\GameObject.h"
#include "Material.h"

ENGINE_USING

Particle::Particle() :
	m_pMaterial(nullptr)
{
	m_strTextureName = "";
	m_eComponentType = CT_PARTICLE;
}

Particle::Particle(const Particle & _Com) : Component(_Com)
{
	m_pMaterial = nullptr;
}


Particle::~Particle()
{
	SAFE_RELEASE(m_pMaterial);
}

bool Particle::LoadTextureSet(const std::string & _strTextureName,
	const TCHAR * _pFileName, 
	const std::string & _strPathName)
{
	// 머티리얼이 없는경우 생성해준다.
	if (nullptr == m_pMaterial)
	{
		m_pMaterial = m_pGameObject->AddComponent<Material>("ParticleMaterial");
	}

	// 텍스처가 이미 있다면 바꿔주고 없다면 추가해준다.
	if (true == m_strTextureName.empty())
	{
		// 텍스처가 없는경우 Add
		m_pMaterial->AddTextureSet(0, 0, 0, _strTextureName, _pFileName, _strPathName);
	}
	else
	{
		// 텍스처가 있는 경우 Change
		m_pMaterial->ChangeTextureSet(0, 0, 0, m_strTextureName, _strTextureName, _pFileName, _strPathName);
	}

	m_strTextureName = _strTextureName;

	return true;
}

bool Particle::LoadTextureSetFromFullPath(const std::string & _strTextureName, const TCHAR * _pFullPath)
{
	if (nullptr == m_pMaterial)
	{
		m_pMaterial = m_pGameObject->AddComponent<Material>("ParticleMaterial");
	}
	// 텍스처가 이미 있다면 바꿔주고 없다면 추가해준다.
	if (true == m_strTextureName.empty())
	{
		// 텍스처가 없는경우 Add
		m_pMaterial->AddTextureSetFromFullPath(0, 0, 0, _strTextureName, _pFullPath);
	}
	else
	{
		// 텍스처가 있는 경우 Change
		m_pMaterial->ChangeTextureSet(0, 0, 0, m_strTextureName, _strTextureName, _pFullPath);
	}

	m_strTextureName = _strTextureName;

	return true;
}

bool Particle::LoadTextureSet(const std::string & _strTextureName, 
	const std::vector<TCHAR*>& _vecFileName, 
	const std::string & _strPathKey)
{
	if (nullptr == m_pMaterial)
	{
		m_pMaterial = m_pGameObject->AddComponent<Material>("ParticleMaterial");
	}

	if (true == m_strTextureName.empty())
	{
		m_pMaterial->AddTextureSetArray(0, 0, 10, _strTextureName, &_vecFileName, _strPathKey);
	}
	else
	{
		m_pMaterial->ChangeTextureSetArray(0, 0, 10, m_strTextureName, _strTextureName, &_vecFileName, _strPathKey);
	}

	m_strTextureName = _strTextureName;

	return true;
}

bool Particle::LoadTextureSetFromFullPath(const std::string & _strTextureName, 
	const std::vector<TCHAR*>& _vecFullPath)
{
	if (nullptr == m_pMaterial)
	{
		m_pMaterial = m_pGameObject->AddComponent<Material>("ParticleMaterial");
	}

	if (true == m_strTextureName.empty())
	{
		m_pMaterial->AddTextureSetArrayFromFullPath(0, 0, 10, _strTextureName, &_vecFullPath);
	}
	else
	{
		m_pMaterial->ChangeTextureSetArrayFromFullPath(0, 0, 10, m_strTextureName, _strTextureName, &_vecFullPath);
	}

	m_strTextureName = _strTextureName;

	return true;
}

void Particle::Start()
{
	SAFE_RELEASE(m_pMaterial);
	m_pMaterial = m_pGameObject->FindComponentFromType<Material>(CT_MATERIAL);
}

bool Particle::Init()
{
	// 렌더그룹을 따로 속성을 지정해주는 부분은 각 컴포넌트의 Init()함수 부분이다.
	m_pGameObject->SetRenderGroup(RG_ALPHA);
	return true;
}

int Particle::Input(float _fTime)
{
	return 0;
}

int Particle::Update(float _fTime)
{

	return 0;
}

int Particle::LateUpdate(float _fTime)
{
	m_tCBuffer.vParticlePos = m_pTransform->GetWorldPosition();
	m_tCBuffer.fSizeX = m_pTransform->GetWorldScale().x;
	m_tCBuffer.fSizeY = m_pTransform->GetWorldScale().y;

	Transform* pMainCamTrans = m_pScene->GetMainCameraTransform();
	m_tCBuffer.vAxisX = pMainCamTrans->GetWorldAxis(AXIS_X);
	m_tCBuffer.vAxisY = pMainCamTrans->GetWorldAxis(AXIS_Y);

	SAFE_RELEASE(pMainCamTrans);

	return 0;
}

int Particle::Collision(float _fTime)
{
	return 0;
}

int Particle::PrevRender(float _fTime)
{
	// 상수버퍼 업데이트 해주기
	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("Particle", &m_tCBuffer);
	return 0;
}

int Particle::Render(float _fTime)
{
	return 0;
}

Particle * Particle::Clone() const
{
	return new Particle(*this);
}
