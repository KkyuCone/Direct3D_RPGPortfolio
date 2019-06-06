#include "UserComponent.h"

ENGINE_USING

UserComponent::UserComponent()
{
	m_eComponentType = CT_USERCOMPONENT;
}

UserComponent::UserComponent(const UserComponent & _Component) : Component(_Component)
{
	// 복사생성자로.. Clone역할을 맡는데
	// 복사본이니까 ReferenceCount를 1로 고정시킨다.
	// 그렇지 않으면 원본에 이어 증가되기 때문에 프로그램 종료시 소멸자에서 삭제되지 않아 릭이 발생한다.
	m_iReferenceCount = 1;
}

UserComponent::~UserComponent()
{

}

void UserComponent::Start()
{
}

bool UserComponent::Init()
{
	return true;
}

int UserComponent::Input(float _fTime)
{
	return 0;
}

int UserComponent::Update(float _fTime)
{
	return 0;
}

int UserComponent::LateUpdate(float _fTime)
{
	return 0;
}

int UserComponent::Collision(float _fTime)
{
	return 0;
}

int UserComponent::PrevRender(float _fTime)
{
	return 0;
}

int UserComponent::Render(float _fTime)
{
	return 0;
}

Component * UserComponent::Clone() const
{
	return nullptr;
}

