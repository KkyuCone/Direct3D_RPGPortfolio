#include "Weapon.h"
#include "Scene\Scene.h"
#include "Scene\Layer.h"
#include "Time.h"
#include "GameObject.h"
#include "InputManager.h"


Weapon::Weapon() :
	m_pWeaponTransform(nullptr)
	, m_pWeaponRenderer(nullptr)
	, m_pWeaponMaterial(nullptr)
{
}

Weapon::Weapon(const Weapon & _Weapon)
{
}


Weapon::~Weapon()
{
	SAFE_RELEASE(m_pWeaponTransform);
	SAFE_RELEASE(m_pWeaponRenderer);
	SAFE_RELEASE(m_pWeaponMaterial);
}

void Weapon::Start()
{
}

bool Weapon::Init()
{
	ComponentInit();
	return true;
}

int Weapon::Input(float _fTime)
{
	return 0;
}

int Weapon::Update(float _fTime)
{
	return 0;
}

int Weapon::LateUpdate(float _fTime)
{
	return 0;
}

int Weapon::Collision(float _fTime)
{
	return 0;
}

int Weapon::PrevRender(float _fTime)
{
	return 0;
}

int Weapon::Render(float _fTime)
{
	return 0;
}

Weapon * Weapon::Clone() const
{
	return new Weapon(*this);
}

void Weapon::ComponentInit()
{
	m_pWeaponTransform = m_pGameObject->GetTransform();
	m_pWeaponTransform->SetLocalPosition(2.0f, 7.0f, 1.0f);
	m_pWeaponTransform->SetWorldScale(1.0f, 1.0f, 1.0f);

	m_pWeaponRenderer = m_pGameObject->AddComponent<Renderer>("PlayerWeaponRenderer");
	m_pWeaponRenderer->SetMesh("PlayerWeapon", TEXT("PC_Weapons_29.msh"), Vector3::Axis[AXIS_Z], PATH_MESH);
	m_pWeaponMaterial = m_pGameObject->FindComponentFromType<Material>(CT_MATERIAL);

	m_pGameObject->SetBoneSoket("Bip01-R-Hand");
	//m_pGameObject->SetBoneSoket("Bip01-Spine1");
	//m_pGameObject->SetBoneSoket("Bip01-R-Finger1");

	SAFE_RELEASE(m_pWeaponMaterial);
	SAFE_RELEASE(m_pWeaponRenderer);
	SAFE_RELEASE(m_pWeaponTransform);
}