#include "ColliderCapsule.h"

ENGINE_USING

ColliderCapsule::ColliderCapsule()
{
	SetTag("Col_Capsule");
	m_eCollType = COL_CAPSULE;
}

ColliderCapsule::ColliderCapsule(const ColliderCapsule & _Com) : Collider(_Com)
{
}


ColliderCapsule::~ColliderCapsule()
{
}

void ColliderCapsule::Start()
{
}

bool ColliderCapsule::Init()
{
	return true;
}

int ColliderCapsule::Input(float _fTime)
{
	return 0;
}

int ColliderCapsule::Update(float _fTime)
{
	return 0;
}

int ColliderCapsule::LateUpdate(float _fTime)
{
	return 0;
}

int ColliderCapsule::Collision(float _fTime)
{
	return 0;
}

bool ColliderCapsule::Collision(Collider * _pDest)
{
	return false;
}

int ColliderCapsule::PrevRender(float _fTime)
{
	return 0;
}

int ColliderCapsule::Render(float _fTime)
{
	return 0;
}

ColliderCapsule * ColliderCapsule::Clone() const
{
	return nullptr;
}
