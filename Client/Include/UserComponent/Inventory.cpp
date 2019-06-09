#include "Inventory.h"



Inventory::Inventory()
{
}

Inventory::Inventory(const Inventory & _Inventory) : Base(_Inventory)
{
	*this = _Inventory;
	m_iReferenceCount = 1;
}


Inventory::~Inventory()
{
}

void Inventory::Start()
{
}

bool Inventory::Init()
{
	return false;
}

int Inventory::Input(float _fTime)
{
	return 0;
}

int Inventory::Update(float _fTime)
{
	return 0;
}

int Inventory::LateUpdate(float _fTime)
{
	return 0;
}

int Inventory::Collision(float _fTime)
{
	return 0;
}

int Inventory::PrevRender(float _fTime)
{
	return 0;
}

int Inventory::Render(float _fTime)
{
	return 0;
}

Inventory * Inventory::Clone() const
{
	return nullptr;
}
