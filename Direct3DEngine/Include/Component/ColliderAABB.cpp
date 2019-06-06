#include "ColliderAABB.h"

ENGINE_USING

ColliderAABB::ColliderAABB()
{
	SetTag("Col_AABB");
	m_eCollType = COL_AABB;
}


ColliderAABB::ColliderAABB(const ColliderAABB & _Com) : Collider(_Com)
{
}

ColliderAABB::~ColliderAABB()
{
}

void ColliderAABB::Start()
{
}

bool ColliderAABB::Init()
{
	return false;
}

int ColliderAABB::Input(float _fTime)
{
	return 0;
}

int ColliderAABB::Update(float _fTime)
{
	return 0;
}

int ColliderAABB::LateUpdate(float _fTime)
{
	return 0;
}

int ColliderAABB::Collision(float _fTime)
{
	return 0;
}

bool ColliderAABB::Collision(Collider * _pDest)
{
	return false;
}

int ColliderAABB::PrevRender(float _fTime)
{
	return 0;
}

int ColliderAABB::Render(float _fTime)
{
	return 0;
}

ColliderAABB * ColliderAABB::Clone() const
{
	return nullptr;
}
