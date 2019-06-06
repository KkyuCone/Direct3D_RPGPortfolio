#include "Bullet.h"
#include "Scene\Scene.h"
#include "Scene\Layer.h"
#include "GameObject.h"


Bullet::Bullet() :
	m_pRenderer(nullptr), m_pColliderSphere(nullptr),
	m_fSpeed(10.0f), m_fDist(5.0f)
{
}

Bullet::Bullet(const Bullet & _Bullet) : UserComponent(_Bullet)
{
	m_fSpeed = _Bullet.m_fSpeed;
	m_fDist = _Bullet.m_fDist;
}


Bullet::~Bullet()
{
	SAFE_RELEASE(m_pRenderer);
	SAFE_RELEASE(m_pColliderSphere);
}

void Bullet::Start()
{
}

bool Bullet::Init()
{
	//m_pTransform->SetWorldScale(0.5f, 0.5f, 1.0f);
	m_pRenderer = m_pGameObject->AddComponent<Renderer>("BulletRenderer");
	m_pRenderer->SetMesh("Pyramid");


	m_pColliderSphere = m_pGameObject->AddComponent<ColliderSphere>("EnemyAttack");
	m_pColliderSphere->SetChannel("EnemyAttack");
	m_pColliderSphere->PickEnable(false);
	m_pColliderSphere->SetSphereInfo(Vector3::Zero, 1.0f);

	return true;
}

int Bullet::Input(float _fTime)
{
	return 0;
}

int Bullet::Update(float _fTime)
{
	m_pTransform->Move(AXIS_Y, m_fSpeed, _fTime);

	m_fDist -= m_fSpeed * _fTime;

	if (m_fDist <= 0.0f)
	{
		m_pGameObject->Active(false);
	}

	return 0;
}

int Bullet::LateUpdate(float _fTime)
{
	return 0;
}

int Bullet::Collision(float _fTime)
{
	return 0;
}

int Bullet::PrevRender(float _fTime)
{
	return 0;
}

int Bullet::Render(float _fTime)
{
	return 0;
}

Bullet * Bullet::Clone() const
{
	return new Bullet(*this);
}

