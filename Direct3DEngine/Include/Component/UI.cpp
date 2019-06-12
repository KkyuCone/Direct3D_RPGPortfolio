#include "UI.h"
#include "Transform.h"
#include "..\GameObject.h"

ENGINE_USING

UI::UI()
{
	m_eComponentType = CT_UI;
	m_eUIType = UI_TYPE::UI_NONE;
}

UI::UI(const UI & _Com) : Component(_Com)
{
	*this = _Com;
	m_iReferenceCount = 1;
	m_eComponentType = _Com.m_eComponentType;
	m_eUIType = _Com.m_eUIType;
}


UI::~UI()
{
}

void UI::Start()
{
}

bool UI::Init()
{
	m_pTransform->SetUIEnable(true);
	m_pGameObject->SetRenderGroup(RG_UI);		// ���� �׷� ����

	return true;
}

int UI::Input(float _fTime)
{
	return 0;
}

int UI::Update(float _fTime)
{
	return 0;
}

int UI::LateUpdate(float _fTime)
{
	return 0;
}

int UI::Collision(float _fTime)
{
	return 0;
}

int UI::PrevRender(float _fTime)
{
	return 0;
}
int UI::Render(float _fTime)
{
	return 0;
}

UI * UI::Clone() const
{
	return nullptr;
}
