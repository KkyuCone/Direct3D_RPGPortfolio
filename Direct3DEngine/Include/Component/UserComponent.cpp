#include "UserComponent.h"

ENGINE_USING

UserComponent::UserComponent()
{
	m_eComponentType = CT_USERCOMPONENT;
}

UserComponent::UserComponent(const UserComponent & _Component) : Component(_Component)
{
	// ��������ڷ�.. Clone������ �ôµ�
	// ���纻�̴ϱ� ReferenceCount�� 1�� ������Ų��.
	// �׷��� ������ ������ �̾� �����Ǳ� ������ ���α׷� ����� �Ҹ��ڿ��� �������� �ʾ� ���� �߻��Ѵ�.
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

