#include "Player.h"
#include "Scene\Scene.h"
#include "Scene\Layer.h"
#include "Time.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Bullet.h"
#include "Component\ColliderSphere.h"
#include "Component\ColliderOBB.h"
#include "Component\Navigation.h"
#include "Component\Camera.h"
#include "Component\TPCamera.h"
#include "Render\RenderManager.h"
#include "Navigation\NavigationManager.h"
#include "Navigation\NavigationMesh.h"
#include "PlayerHead.h"
#include "Weapon.h"


void Player::ReNavLateUpdate(float _fTime)
{
	Navigation* pNav = m_pGameObject->FindComponentFromType<Navigation>(CT_NAVIGATION);
	pNav->LateUpdate(_fTime);
	SAFE_RELEASE(pNav);
}

bool Player::GetMoveAni()
{
	return m_bMoveAni;
}

Player::Player()
	: m_fSpeed(10.0f),
	m_pBodyTransform(nullptr),
	m_pBodyRenderer(nullptr),
	m_pBodyMaterial(nullptr),
	m_pBodyAnimation(nullptr),
	m_pMouseRay(nullptr),
	m_pHeadObject(nullptr),
	m_pWeaponObject(nullptr),
	m_pHeadScript(nullptr),
	m_pWeaponScript(nullptr),
	m_pMoveAniChildObj(nullptr),
	m_pMoveAniChildTR(nullptr),
	m_pMoveAniChildScript(nullptr) ,
	m_bMoveAni(false)
{
	m_ePlayerState = BaseState::IDLE;

	m_tPlayerUIState.m_iLevel = 1;
	m_tPlayerUIState.m_iHP = 10000;
	m_tPlayerUIState.m_iMP = 500;
	m_tPlayerUIState.m_iCurEXP = 0;
	m_tPlayerUIState.m_iMaxEXP = 1000;

	//스킬

	m_bComputeCoolTime[0] = false;
	m_bComputeCoolTime[1] = false;
	m_bComputeCoolTime[2] = false;
	m_bComputeCoolTime[3] = false;
	m_bComputeCoolTime[4] = false;

	m_fArrSkillCollTime[0] = 8.0f;
	m_fArrSkillCollTime[1] = 10.0f;
	m_fArrSkillCollTime[2] = 1.0f;
	m_fArrSkillCollTime[3] = 9.0f;
	m_fArrSkillCollTime[4] = 10.0f;

	m_fArrCurCoolTime[0] = 0.0f;
	m_fArrCurCoolTime[1] = 0.0f;
	m_fArrCurCoolTime[2] = 0.0f;
	m_fArrCurCoolTime[3] = 0.0f;
	m_fArrCurCoolTime[4] = 0.0f;

	m_iCurSKillNumber = BaseState::NONE;
}

Player::Player(const Player & _Player) : Base(_Player)
{
}


Player::~Player()
{
	SAFE_RELEASE(m_pBodyAnimation);
	SAFE_RELEASE(m_pBodyTransform);

	SAFE_RELEASE(m_pHeadObject);
	SAFE_RELEASE(m_pWeaponObject);
	SAFE_RELEASE(m_pMoveAniChildObj);

}

void Player::Start()
{
	Collider*	pPickSphere = FindComponentFromTag<Collider>("PickSphere");

	pPickSphere->SetCallback<Player>(CCBS_STAY, this, &Player::HitPick);
	pPickSphere->SetCallback<Player>(CCBS_EXIT, this, &Player::HitPickLeave);

	SAFE_RELEASE(pPickSphere);
}

bool Player::Init()
{
	ComponentInit();		// 컴포넌트들 초기화
	InputInit();			// 입력키 초기화

	return true;
}

int Player::Input(float _fTime)
{
	return 0;
}

int Player::Update(float _fTime)
{
	if (0 > m_tPlayerUIState.m_iHP)
	{
		m_ePlayerState = BaseState::DEATH;
	}

	if (true == m_bComputeCoolTime[0]
		|| true == m_bComputeCoolTime[1]
		|| true == m_bComputeCoolTime[2]
		|| true == m_bComputeCoolTime[3]
		|| true == m_bComputeCoolTime[4])
	{
		ComputeSkillCoolTime(_fTime);
	}

	switch (m_ePlayerState)
	{
	case IDLE:
		Idle();
		break;
	case RUN:
		Run();
		break;
	case WAIT:
		Wait();
		break;
	case DEATH:
		Death();
		break;
	case DEATHWAIT:
		DeathWait();
		break;
	case KNOCKBACK:
		NockBack();
		break;
	case GROGGY:
		Groggy();
		break;
	case TUMBLING:
		Tumbling();
		break;
	case SKILL01:
		Skill01();
		break;
	case SKILL02:
		Skill02();
		break;
	case SKILL03:
		Skill03();
		break;
	case SKILL04:
		Skill04();
		break;
	case SKILL05:
		Skill05();
		break;
	case COMBO1:
	case COMBO1R:
		Combo1();
		break;
	case COMBO2:
	case COMBO2R:
		Combo2();
		break;
	case COMBO3:
	case COMBO3R:
		Combo3();
		break;
	case COMBO4:
		Combo4();
		break;
	default:
		break;
	}

	return 0;
}

int Player::LateUpdate(float _fTime)
{
	return 0;
}

int Player::Collision(float _fTime)
{
	return 0;
}

int Player::PrevRender(float _fTime)
{
	return 0;
}

int Player::Render(float _fTime)
{
	m_bMoveAni = false;
	return 0;
}

Player * Player::Clone() const
{
	return new Player(*this);
}

void Player::SetHeadObject(GameObject * _Object)
{
	if (nullptr != m_pHeadObject)
	{
		SAFE_RELEASE(m_pHeadObject);
	}

	if (nullptr != _Object)
	{
		_Object->AddReference();
	}
	m_pHeadObject = _Object;
}

void Player::SetWeaponObject(GameObject * _Object)
{
	if (nullptr != m_pWeaponObject)
	{
		SAFE_RELEASE(m_pWeaponObject);
	}

	if (nullptr != _Object)
	{
		_Object->AddReference();
	}
	m_pWeaponObject = _Object;
}

void Player::SetHeadScript(PlayerHead * _Script)
{
	m_pHeadScript = _Script;
}

void Player::SetWeaponScript(Weapon * _Script)
{
	m_pWeaponScript = _Script;
}

void Player::SetMoveAniChildObject(GameObject * _Object)
{
	if (nullptr != m_pMoveAniChildObj)
	{
		SAFE_RELEASE(m_pMoveAniChildObj);
	}

	if (nullptr != _Object)
	{
		_Object->AddReference();
	}
	m_pMoveAniChildObj = _Object;
	SAFE_RELEASE(_Object);
}

void Player::SetMoveAniChildScript(PlayerCameraPivot * _Script)
{
	m_pMoveAniChildScript = _Script;
}

BaseState Player::GetCurrentState()
{
	return m_ePlayerState;
}

void Player::ComponentInit()
{
	m_pBodyTransform = m_pGameObject->GetTransform();
	m_pBodyTransform->SetWorldScale(0.05f, 0.05f, 0.05f);
	m_pBodyTransform->SetLocalRotation(0.0f, -95.0f, 0.0f);

	m_pBodyRenderer = m_pGameObject->AddComponent<Renderer>("PlayerRenderer");
	//Popori_F_R31_Body
	m_pBodyRenderer->SetMesh("Player", TEXT("Popori_F_R31_Body.msh"), Vector3::Axis[AXIS_Z], PATH_MESH);

	m_pBodyMaterial = m_pGameObject->FindComponentFromType<Material>(CT_MATERIAL);
	//m_pMaterial->SetEmissiveColor(1.0f);

#pragma region Collision
	/*ColliderSphere*		pBodyCol = m_pGameObject->AddComponent<ColliderSphere>("Body");*/
	//pBodyCol->SetProfile("Player");
	//pBodyCol->SetChannel("Player");
	//pBodyCol->SetSphereInfo(Vector3::Zero, 1.0f);
	//pBodyCol->SetCallback<Player>(CCBS_ENTER, this, &Player::TestHitEnter);
	//pBodyCol->SetCallback<Player>(CCBS_STAY, this, &Player::TestHitStay);
	//pBodyCol->SetCallback<Player>(CCBS_EXIT, this, &Player::TestHitExit);
	//SAFE_RELEASE(pBodyCol);

	ColliderSphere*		pPlayerBodyCol = m_pGameObject->AddComponent<ColliderSphere>("PlayerBody");
	pPlayerBodyCol->SetProfile("Player");
	pPlayerBodyCol->SetChannel("Player");
	pPlayerBodyCol->SetSphereInfo(Vector3::Zero, 1.0f);
	pPlayerBodyCol->SetCallback<Player>(CCBS_ENTER, this, &Player::TestHitEnter);
	pPlayerBodyCol->SetCallback<Player>(CCBS_STAY, this, &Player::TestHitStay);
	pPlayerBodyCol->SetCallback<Player>(CCBS_EXIT, this, &Player::TestHitExit);
	SAFE_RELEASE(pPlayerBodyCol);


	ColliderOBB*		pBodyCol = m_pGameObject->AddComponent<ColliderOBB>("Body");
	pBodyCol->SetProfile("Player");
	pBodyCol->SetChannel("Player");
	pBodyCol->SetOBBInfo(Vector3(0.f, 0.5f, 0.f), Vector3(1.f, 0.5f, 0.5f));
	pBodyCol->SetCallback<Player>(CCBS_ENTER, this, &Player::TestHitEnter);
	pBodyCol->SetCallback<Player>(CCBS_STAY, this, &Player::TestHitStay);
	pBodyCol->SetCallback<Player>(CCBS_EXIT, this, &Player::TestHitExit);

	SAFE_RELEASE(pBodyCol);


#pragma endregion


	m_pBodyAnimation = m_pGameObject->FindComponentFromType<Animation>(CT_ANIMATION);

	if (nullptr == m_pBodyAnimation)
	{
		m_pBodyAnimation = m_pGameObject->AddComponent<Animation>("PalyerAnimation");
		m_pBodyAnimation->LoadBone("Popori_F_R31_Body.bne");			// 본정보 가져오기
		m_pBodyAnimation->Load("Popori_F_R31_Body.anm");				// 애니메이션 정보가져오기

		m_pBodyAnimation->ChangeClip("Combo4");

		//Bip01-Spine3, Bip01-Spine2
		m_pBodyAnimation->SetChangePivotBone("Bip01-Spine3");
	}

	Navigation* pNav = m_pGameObject->AddComponent<Navigation>("Nav");
	//pNav->SetAIFindPath(true);
	pNav->SetMouseMove(true);
	SAFE_RELEASE(pNav);


#pragma region 기존 코드

	//m_pRenderer->SetMesh("Pyramid");

	//m_pRenderer->SetMesh("Cone");
	//m_pRenderer->SetRenderState("WireFrame_CullNone");
	//m_pRenderer->SetRenderState("LessEqual");

	//Material* m_pMaterial = m_pGameObject->AddComponent<Material>("SkyMaterial");
	//m_pMaterial->SetDiffuseTexture(0, 0, SAMPLER_LINEAR, 0, 10, "EngineSky", TEXT("Sky.dds"));

	//m_pGameObject->Start();

	//SAFE_RELEASE(m_pMaterial);
#pragma endregion
	SAFE_RELEASE(m_pBodyMaterial);
	SAFE_RELEASE(m_pBodyRenderer);
	SAFE_RELEASE(m_pBodyTransform);
}

void Player::InputInit()
{
	// Axis - 입력키등록
	//GET_SINGLETON(InputManager)->AddAxisKey("MoveFront", DIK_W, 1.0f);
	//GET_SINGLETON(InputManager)->AddAxisKey("MoveBack", DIK_S, -1.0f);
	//GET_SINGLETON(InputManager)->AddAxisKey("MoveFront", DIK_UP, 1.0f);
	//GET_SINGLETON(InputManager)->AddAxisKey("MoveBack", DIK_DOWN, -1.0f);

	GET_SINGLETON(InputManager)->AddAxisKey("RotationLeft", DIK_A, -1.0f);
	GET_SINGLETON(InputManager)->AddAxisKey("RotationRight", DIK_D, 1.0f);
	GET_SINGLETON(InputManager)->AddAxisKey("RotationLeft", DIK_LEFT, -1.0f);
	GET_SINGLETON(InputManager)->AddAxisKey("RotationRight", DIK_RIGHT, 1.0f);

	GET_SINGLETON(InputManager)->AddAxisKey("HDR_ON", DIK_Z, 1.0f);				// HDR On
	GET_SINGLETON(InputManager)->AddAxisKey("HDR_OFF", DIK_X, 1.0f);			// HDR OFF

	GET_SINGLETON(InputManager)->AddAxisKey("SSAO_ON", DIK_C, 1.0f);			// SSAO_ON
	GET_SINGLETON(InputManager)->AddAxisKey("SSAO_OFF", DIK_V, 1.0f);			// SSAO_OFF

	GET_SINGLETON(InputManager)->AddAxisKey("DepthFog_ON", DIK_B, 1.0f);		// DepthFog_ON
	GET_SINGLETON(InputManager)->AddAxisKey("DepthFog_OFF", DIK_N, 1.0f);		// DepthFog_OFF

	GET_SINGLETON(InputManager)->AddAxisKey("Rain_ON", DIK_F, 1.0f);		// Rain_ON
	GET_SINGLETON(InputManager)->AddAxisKey("Rain_OFF", DIK_G, 1.0f);		// Rain_OFF

	// Axis - 등록된 입력키에 함수 연결
	//GET_SINGLETON(InputManager)->BindAxis("MoveFront", this, &Player::MoveFront);
	//GET_SINGLETON(InputManager)->BindAxis("MoveBack", this, &Player::MoveFront);
	GET_SINGLETON(InputManager)->BindAxis("RotationLeft", this, &Player::Rotate);
	GET_SINGLETON(InputManager)->BindAxis("RotationRight", this, &Player::Rotate);

	GET_SINGLETON(InputManager)->BindAxis("HDR_ON", this, &Player::HDR_ON);
	GET_SINGLETON(InputManager)->BindAxis("HDR_OFF", this, &Player::HDR_OFF);

	GET_SINGLETON(InputManager)->BindAxis("SSAO_ON", this, &Player::SSAO_ON);
	GET_SINGLETON(InputManager)->BindAxis("SSAO_OFF", this, &Player::SSAO_OFF);

	GET_SINGLETON(InputManager)->BindAxis("DepthFog_ON", this, &Player::DepthFog_ON);
	GET_SINGLETON(InputManager)->BindAxis("DepthFog_OFF", this, &Player::DepthFog_OFF);

	GET_SINGLETON(InputManager)->BindAxis("Rain_ON", this, &Player::Rain_ON);
	GET_SINGLETON(InputManager)->BindAxis("Rain_OFF", this, &Player::Rain_OFF);


	// Action - 조합키등록
	GET_SINGLETON(InputManager)->AddActionKey("MoveFront", DIK_W);
	GET_SINGLETON(InputManager)->AddActionKey("MoveBack", DIK_S);
	GET_SINGLETON(InputManager)->AddActionKey("MoveFront", DIK_UP);
	GET_SINGLETON(InputManager)->AddActionKey("MoveBack", DIK_DOWN);
	GET_SINGLETON(InputManager)->AddActionKey("HPDec", DIK_O);

	GET_SINGLETON(InputManager)->AddActionKey("Fire", DIK_SPACE, FK_CTRL);
	GET_SINGLETON(InputManager)->AddActionKey("Test", DIK_SPACE, FK_SHIFT);

	GET_SINGLETON(InputManager)->AddActionKey("Tumbling", DIK_SPACE);	// Tumbling	- Space
	GET_SINGLETON(InputManager)->AddActionKey("Skill_01", DIK_1);		// Skill_01
	GET_SINGLETON(InputManager)->AddActionKey("Skill_02", DIK_2);		// Skill_02
	GET_SINGLETON(InputManager)->AddActionKey("Skill_03", DIK_3);		// Skill_03
	GET_SINGLETON(InputManager)->AddActionKey("Skill_04", DIK_4);		// Skill_04
	GET_SINGLETON(InputManager)->AddActionKey("Skill_05", DIK_5);		// Skill_05 

	// Action - 조합키에 사용될 함수와 호출될 경우 등록
	GET_SINGLETON(InputManager)->BindAction("MoveFront", KS_PUSH, this, &Player::MoveFront);
	GET_SINGLETON(InputManager)->BindAction("MoveBack", KS_PUSH, this, &Player::MoveBack);
	GET_SINGLETON(InputManager)->BindAction("MoveFront", KS_RELEASE, this, &Player::MoveNone);
	GET_SINGLETON(InputManager)->BindAction("MoveBack", KS_RELEASE, this, &Player::MoveNone);

	GET_SINGLETON(InputManager)->BindAction("Fire", KS_PRESS, this, &Player::Fire);
	GET_SINGLETON(InputManager)->BindAction("Test", KS_PRESS, this, &Player::Test);

	GET_SINGLETON(InputManager)->BindAction("Tumbling", KS_PRESS, this, &Player::StateChangeTumbling);
	GET_SINGLETON(InputManager)->BindAction("Skill_01", KS_PRESS, this, &Player::StateChangeSkill01);
	GET_SINGLETON(InputManager)->BindAction("Skill_02", KS_PRESS, this, &Player::StateChangeSkill02);
	GET_SINGLETON(InputManager)->BindAction("Skill_03", KS_PRESS, this, &Player::StateChangeSkill03);
	GET_SINGLETON(InputManager)->BindAction("Skill_04", KS_PRESS, this, &Player::StateChangeSkill04);
	GET_SINGLETON(InputManager)->BindAction("Skill_05", KS_PRESS, this, &Player::StateChangeSkill05);

	GET_SINGLETON(InputManager)->BindAction("HPDec", KS_PRESS, this, &Player::DecHP);		// Skill_05 	// DecHP
}

void Player::MoveFront(float _fTime)
{
	if (true == UseSkill())
	{
		return;
	}

	m_ePlayerState = BaseState::RUN;
	m_pTransform->Move(AXIS_Z, 5.0f * 1.0f, _fTime);
}

void Player::MoveBack(float _fTime)
{
	if (true == UseSkill())
	{
		return;
	}

	m_ePlayerState = BaseState::RUN;
	m_pTransform->Move(AXIS_Z, 5.0f * -1.0f, _fTime);
}

void Player::MoveNone(float _fTime)
{
	m_ePlayerState = BaseState::IDLE;
}

void Player::MoveRight(float _fScale, float _fTime)
{
	if (true == UseSkill())
	{
		return;
	}

	m_pTransform->Move(AXIS_X, _fScale, _fTime);
}

void Player::Rotate(float _fScale, float _fTime)
{
	if (true == UseSkill())
	{
		return;
	}

	m_pTransform->RotateY(180.0f * _fScale, _fTime);
}

void Player::Fire(float _fTime)
{
	// 총알 발사하기
	OutputDebugString(TEXT("Fire\n"));
	GameObject* NewBulletObj = GameObject::CreateClone("Bullet", m_pScene, m_pLayer);
	Vector3 vPos = m_pTransform->GetWorldPosition();
	Vector3 vRotation = m_pTransform->GetWorldRotation();
	Vector3 vView = m_pTransform->GetWorldAxis(AXIS_Z);

	vPos += vView;

	Transform* BulletTransform = NewBulletObj->GetTransform();
	BulletTransform->SetWorldPosition(vPos);
	BulletTransform->SetWorldRotation(vRotation);

	SAFE_RELEASE(BulletTransform);
	SAFE_RELEASE(NewBulletObj);
}

void Player::Test(float _fTime)
{
	//OutputDebugString(TEXT("Test\n")); 
	MessageBox(nullptr, TEXT("Test"), TEXT("Test"), MB_OK);
}

void Player::AnimationChange(std::string _strName)
{
	PANIMATIONCLIP pCurClip = m_pBodyAnimation->GetCurrentClip();

	if (_strName == pCurClip->strName)
	{
		// 이미 돌리고 있는 애니메이션이라면
		return;
	}

	m_pBodyAnimation->ChangeClip(_strName);

	if (nullptr != m_pHeadScript)
	{
		m_pHeadScript->AnimationChange(_strName);
	}
}

void Player::HDR_ON(float _fScale, float _fTime)
{
	if (_fScale > 0.0f)
	{
		GET_SINGLETON(RenderManager)->SetPostEffectEnable(true);
	}
}

void Player::HDR_OFF(float _fScale, float _fTime)
{
	if (_fScale > 0.0f)
	{
		GET_SINGLETON(RenderManager)->SetPostEffectEnable(false);
	}
}

void Player::SSAO_ON(float _fScale, float _fTime)
{
	if (_fScale > 0.0f)
	{
		GET_SINGLETON(RenderManager)->SetSSAOEnable(true);
	}
}

void Player::SSAO_OFF(float _fScale, float _fTime)
{
	if (_fScale > 0.0f)
	{
		GET_SINGLETON(RenderManager)->SetSSAOEnable(false);
	}
}

void Player::DepthFog_ON(float _fScale, float _fTime)
{
	if (_fScale > 0.0f)
	{
		GET_SINGLETON(RenderManager)->SetDepthFogEnable(true);
	}
}

void Player::DepthFog_OFF(float _fScale, float _fTime)
{
	if (_fScale > 0.0f)
	{
		GET_SINGLETON(RenderManager)->SetDepthFogEnable(false);
	}
}

void Player::Rain_ON(float _fScale, float _fTime)
{
	if (_fScale > 0.0f)
	{
		GET_SINGLETON(RenderManager)->SetRainEnable(true);
	}
}

void Player::Rain_OFF(float _fScale, float _fTime)
{
	if (_fScale > 0.0f)
	{
		GET_SINGLETON(RenderManager)->SetRainEnable(false);
	}
}

void Player::TestHitEnter(Collider * _pSrc, Collider * _pDest, float _fTime)
{
	OutputDebugString(TEXT("충돌\n"));
}

void Player::TestHitStay(Collider * _pSrc, Collider * _pDest, float _fTime)
{
	//OutputDebugString(TEXT("충돌Stay\n"));
}

void Player::TestHitExit(Collider * _pSrc, Collider * _pDest, float _fTime)
{
	OutputDebugString(TEXT("충돌Leave\n"));
}

void Player::HitPick(Collider * _pSrc, Collider * _pDest, float _fTime)
{
	GameObject* pDestObj = _pDest->GetGameObject();
	if ("MouseRay" != _pDest->GetTag() || "Mouse" != pDestObj->GetTag())
	{
		SAFE_RELEASE(pDestObj);
		return;
	}

	Material*	pMaterial = FindComponentFromType<Material>(CT_MATERIAL);

	pMaterial->SetEmissiveColor(1.f);
	SAFE_RELEASE(pDestObj);
	SAFE_RELEASE(pMaterial);
}

void Player::HitPickLeave(Collider * _pSrc, Collider * _pDest, float _fTime)
{
	GameObject* pDestObj = _pDest->GetGameObject();
	if ("MouseRay" != _pDest->GetTag() || "Mouse" != pDestObj->GetTag())
	{
		SAFE_RELEASE(pDestObj);
		return;
	}

	Material*	pMaterial = FindComponentFromType<Material>(CT_MATERIAL);

	pMaterial->SetEmissiveColor(0.f);

	SAFE_RELEASE(pDestObj);
	SAFE_RELEASE(pMaterial);
}

bool Player::ChangeableSkill(float _fTime)
{
	// 후에 퀵슬롯 할시, 스킬 쿨타임에 맞춰서 해당 스킬이 현재 사용중인지 판단하는 코드 구현
	if (false == m_bComputeCoolTime[0]
		&& false == m_bComputeCoolTime[1]
		&& false == m_bComputeCoolTime[2]
		&& false == m_bComputeCoolTime[3]
		&& false == m_bComputeCoolTime[4])
	{
		return true;
	}
	else
	{
		return false;
	}

	// 스킬 애니메이션으로 바뀔수 있는 상태인지 검사
	if (BaseState::IDLE == m_ePlayerState || BaseState::RUN == m_ePlayerState
		|| BaseState::COMBO1 == m_ePlayerState || BaseState::COMBO1R == m_ePlayerState
		|| BaseState::COMBO2 == m_ePlayerState || BaseState::COMBO2R == m_ePlayerState
		|| BaseState::COMBO3 == m_ePlayerState || BaseState::COMBO3R == m_ePlayerState
		|| BaseState::COMBO4 == m_ePlayerState)
	{
		return true;
	}

	return false;
}

bool Player::ComputeSkillCoolTime(float _fTime)
{
	switch (m_iCurSKillNumber)
	{
	case SKILL01:
		if (m_fArrCurCoolTime[0] >= m_fArrSkillCollTime[0])
		{
			m_fArrCurCoolTime[0] = 0.0f;
			m_bComputeCoolTime[0] = false;
			m_iCurSKillNumber = BaseState::NONE;
			return true;
		}
		m_fArrCurCoolTime[0] += _fTime;
		break;
	case SKILL02:
		if (m_fArrCurCoolTime[1] >= m_fArrSkillCollTime[1])
		{
			m_fArrCurCoolTime[1] = 0.0f;
			m_bComputeCoolTime[1] = false;
			m_iCurSKillNumber = BaseState::NONE;
			return true;
		}
		m_fArrCurCoolTime[1] += _fTime;
		break;
	case SKILL03:
		if (m_fArrCurCoolTime[2] >= m_fArrSkillCollTime[2])
		{
			m_fArrCurCoolTime[2] = 0.0f;
			m_bComputeCoolTime[2] = false;
			m_iCurSKillNumber = BaseState::NONE;
			return true;
		}
		m_fArrCurCoolTime[2] += _fTime;
		break;
	case SKILL04:
		if (m_fArrCurCoolTime[3] >= m_fArrSkillCollTime[3])
		{
			m_fArrCurCoolTime[3] = 0.0f;
			m_bComputeCoolTime[3] = false;
			m_iCurSKillNumber = BaseState::NONE;
			return true;
		}
		m_fArrCurCoolTime[3] += _fTime;
		break;
	case SKILL05:
		if (m_fArrCurCoolTime[4] >= m_fArrSkillCollTime[4])
		{
			m_fArrCurCoolTime[4] = 0.0f;
			m_bComputeCoolTime[4] = false;
			m_iCurSKillNumber = BaseState::NONE;
			return true;
		}
		m_fArrCurCoolTime[4] += _fTime;
		break;
	default:
		break;
	}
	return false;
}

void Player::ChangeSkillToIdle()
{
	AnimationPivotMove();
	if (true == m_pBodyAnimation->IsAnimationEnd())
	{
		// 피봇 변경이 필요한 애니메이션의 경우 까지 처리
		m_bMoveAni = true;
		m_ePlayerState = BaseState::IDLE;
	}
}

bool Player::CheckChangeSkill(BaseState _State)
{
	// 같은 스킬 사용시 
	if (_State == m_iCurSKillNumber)
	{
		return false;
	}
	else
	{
		if (BaseState::IDLE == m_ePlayerState
			|| BaseState::RUN == m_ePlayerState)
		{
			m_iCurSKillNumber = _State;
			m_bComputeCoolTime[_State - BaseState::SKILL01] = true;
			m_ePlayerState = _State;
			return true;
		}
	}

	if (BaseState::SKILL01 == m_iCurSKillNumber
		|| BaseState::SKILL02 == m_iCurSKillNumber
		|| BaseState::SKILL03 == m_iCurSKillNumber
		|| BaseState::SKILL04 == m_iCurSKillNumber
		|| BaseState::SKILL05 == m_iCurSKillNumber)
	{
		return false;
	}

	// 이미 쿨타임 돌아가고 있으면 X, 그리고 스킬사용중이면 다른 스킬로 바꾸지 못한다.
	if (true == m_bComputeCoolTime[_State - BaseState::SKILL01])
	{
		return false;
	}

	m_iCurSKillNumber = _State;
	m_bComputeCoolTime[_State - BaseState::SKILL01] = true;
	m_ePlayerState = _State;
	return true;
}

bool Player::UseSkill()
{
	if (BaseState::SKILL01 == m_ePlayerState
		|| BaseState::SKILL02 == m_ePlayerState
		|| BaseState::SKILL03 == m_ePlayerState
		|| BaseState::SKILL04 == m_ePlayerState
		|| BaseState::SKILL05 == m_ePlayerState)
	{
		return true;
	}

	return false;
}


// 애니메이션 자체가 이동하는 거면 끝프레임에서 위치 이동 시켜주기
void Player::AnimationPivotMove()
{
	GameObject* pMainCam = m_pGameObject->GetScene()->GetMainCameraObject();
	TPCamera* pTPCamComponent = pMainCam->FindComponentFromType<TPCamera>(CT_TPCAMERA);

	switch (m_ePlayerState)
	{
	case NONE:
	case IDLE:
	case WAIT:
	{
		pTPCamComponent->SetSoketMoveEnable(false);
		pTPCamComponent->SetBaseNoneMoveEnable(true);
		break;
	}
	case WALK:
	case RUN:
	{
		pTPCamComponent->SetSoketMoveEnable(false);
		pTPCamComponent->SetBaseNoneMoveEnable(false);
		break;
	}
	case DEATH:
	case DEATHWAIT:
	case KNOCKBACK:
	case GROGGY:
	case TUMBLING:
	case SKILL01:
	case SKILL02:
	case SKILL03:
	case SKILL04:
	case SKILL05:
	case ATK01:
	case ATK02:
	case ATK03:
	case ATK04:
	case ATK05:
	case COMBO1:
	case COMBO1R:
	case COMBO2:
	case COMBO2R:
	case COMBO3:
	case COMBO3R:
	case COMBO4:
	{
		pTPCamComponent->SetSoketMoveEnable(true);
		pTPCamComponent->SetBaseNoneMoveEnable(false);
		break;
	}
	case MAX:
		break;
	default:
		break;
	}


	SAFE_RELEASE(pTPCamComponent);
	SAFE_RELEASE(pMainCam);
}


// 테스트용
void Player::DecHP(float _fTime)
{
	m_tPlayerUIState.m_iHP -= 2000;

	if (m_tPlayerUIState.m_iHP <= 0)
	{
		StateChangeDeath(_fTime);
	}
}
