#include "ColliderPoint.h"

ENGINE_USING

ColliderPoint::ColliderPoint()
{
	SetTag("Col_Point");
	m_eCollType = COL_POINT;
}

ColliderPoint::ColliderPoint(ColliderPoint & _Com) : Collider(_Com)
{
}


ColliderPoint::~ColliderPoint()
{
}

void ColliderPoint::Start()
{
}

bool ColliderPoint::Init()
{
	return false;
}

int ColliderPoint::Input(float _fTime)
{
	return 0;
}

int ColliderPoint::Update(float _fTime)
{
	return 0;
}

int ColliderPoint::LateUpdate(float _fTime)
{
	return 0;
}

int ColliderPoint::Collision(float _fTime)
{
	return 0;
}

bool ColliderPoint::Collision(Collider * _pDest)
{
	return false;
}

int ColliderPoint::PrevRender(float _fTime)
{
	return 0;
}

int ColliderPoint::Render(float _fTime)
{
	return 0;
}

ColliderPoint * ColliderPoint::Clone() const
{
	return nullptr;
}
