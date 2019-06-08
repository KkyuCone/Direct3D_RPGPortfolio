#include "PlayerCameraPivot.h"
#include "Scene\Scene.h"
#include "Scene\Layer.h"
#include "Time.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Base.h"
#include "Player.h"


PlayerCameraPivot::PlayerCameraPivot() :
	m_pParentObj(nullptr)
	, m_pParentAni(nullptr)
	, m_pParentScript(nullptr)
	, m_pTPCamera(nullptr)
	, m_pTPCamTR(nullptr)
{
}

PlayerCameraPivot::PlayerCameraPivot(const PlayerCameraPivot & _PlayerCameraPivot)
{
}


PlayerCameraPivot::~PlayerCameraPivot()
{
	SAFE_RELEASE(m_pParentAni);
	SAFE_RELEASE(m_pParentScript);

	SAFE_RELEASE(m_pTPCamTR);
	SAFE_RELEASE(m_pTPCamera);
}

void PlayerCameraPivot::Start()
{
}

bool PlayerCameraPivot::Init()
{
	// 부모 설정
	m_pParentObj = m_pGameObject->GetParent();
	m_pParentAni = m_pParentObj->FindComponentFromType<Animation>(CT_ANIMATION);
	m_pParentScript = m_pParentObj->FindComponentFromType<Player>(CT_USERCOMPONENT);
	//m_pGameObject->SetBoneSoket("Bip01-Spine3");
	m_pGameObject->SetBoneSoket("Bip01-Head");

	GameObject* pMainCam = m_pGameObject->GetScene()->GetMainCameraObject();
	m_pTPCamera = pMainCam->FindComponentFromType<TPCamera>(CT_TPCAMERA);
	m_pTPCamTR = pMainCam->GetTransform();
	SAFE_RELEASE(pMainCam);
	return true;
}

int PlayerCameraPivot::Input(float _fTime)
{
	return 0;
}

int PlayerCameraPivot::Update(float _fTime)
{
	return 0;
}

int PlayerCameraPivot::LateUpdate(float _fTime)
{
	if (true == m_pParentScript->GetMoveAni())
	{
		Transform* pParentTR = m_pParentObj->GetTransform();
		pParentTR->SetWorldPosition(m_pTransform->GetWorldPositionAtMatrix());
		SAFE_RELEASE(pParentTR);

		m_pParentScript->ReNavLateUpdate(_fTime);
	}

	return 0;
}

int PlayerCameraPivot::Collision(float _fTime)
{
	return 0;
}

int PlayerCameraPivot::PrevRender(float _fTime)
{
	return 0;
}

int PlayerCameraPivot::Render(float _fTime)
{

	return 0;
}

PlayerCameraPivot * PlayerCameraPivot::Clone() const
{
	return new PlayerCameraPivot(*this);
}
