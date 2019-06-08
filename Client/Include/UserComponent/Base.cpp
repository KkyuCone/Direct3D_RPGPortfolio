#include "Base.h"



Base::Base()
{
}

Base::Base(const Base & _Base): UserComponent(_Base)
{
	*this = _Base;
	m_iReferenceCount = 1;
}


Base::~Base()
{
}

void Base::Start()
{
}

bool Base::Init()
{
	return false;
}

int Base::Input(float _fTime)
{
	return 0;
}

int Base::Update(float _fTime)
{
	return 0;
}

int Base::LateUpdate(float _fTime)
{
	return 0;
}

int Base::Collision(float _fTime)
{
	return 0;
}

int Base::PrevRender(float _fTime)
{
	return 0;
}

int Base::Render(float _fTime)
{
	return 0;
}

Base * Base::Clone() const
{
	return nullptr;
}
