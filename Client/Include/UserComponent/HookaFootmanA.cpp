#include "HookaFootmanA.h"
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


HookaFootmanA::HookaFootmanA() :
	m_fSpeed(10.0f)
	, m_pHookaRenderer(nullptr)
	, m_pHookaMaterial(nullptr)
	, m_pHookaAnimation(nullptr)
	, m_pSphereCollider(nullptr)
	, m_pTraceCollider(nullptr)
	, m_pPlayerTrnasform(nullptr)
	, m_pNavigation(nullptr)
{
	m_eState = BaseState::IDLE;

	m_tUIState.m_iLevel = 20;
	m_tUIState.m_iHP = 20000;
	m_tUIState.m_iMP = 1;
	m_tUIState.m_iCurEXP = 100;
	m_tUIState.m_iMaxEXP = 100;
}

HookaFootmanA::HookaFootmanA(const HookaFootmanA & _HookaFootmanA)
{
}


HookaFootmanA::~HookaFootmanA()
{
	SAFE_RELEASE(m_pHookaMaterial);
	SAFE_RELEASE(m_pHookaRenderer);

	SAFE_RELEASE(m_pPlayerTrnasform);

	SAFE_RELEASE(m_pNavigation);
	SAFE_RELEASE(m_pHookaAnimation);

	SAFE_RELEASE(m_pTraceCollider);
	SAFE_RELEASE(m_pSphereCollider);

	SAFE_RELEASE(m_pNavigation);
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void HookaFootmanA::Start()
{
}

bool HookaFootmanA::Init()
{
	m_pTransform->SetWorldScale(1.0f, 1.0f, 1.0f);

	m_pHookaRenderer = m_pGameObject->AddComponent<Renderer>("HookaRenderer");
	m_pHookaRenderer->SetMesh("MonsterHooka", TEXT("HookaFootman_A.msh"), Vector3::Axis[AXIS_Z], PATH_MESH);

	m_pHookaMaterial = m_pGameObject->FindComponentFromType<Material>(CT_MATERIAL);

	// ������ �ݶ��̴�
	m_pTraceCollider = m_pGameObject->AddComponent<ColliderSphere>("Trace");
	m_pTraceCollider->SetProfile("Trace");
	m_pTraceCollider->SetChannel("Trace");
	m_pTraceCollider->SetSphereInfo(Vector3::Zero, 10.0f);
	m_pTraceCollider->SetScaleEnable(false);
	m_pTraceCollider->PickEnable(false);

	// �Ϲ� �ݶ��̴� (�ǰ� ����)
	m_pSphereCollider = m_pGameObject->AddComponent<ColliderSphere>("HookaCol");
	m_pSphereCollider->SetProfile("Enemy");
	m_pSphereCollider->SetChannel("Enemy");
	m_pSphereCollider->SetSphereInfo(Vector3::Zero, 1.0f);
	m_pSphereCollider->PickEnable(false);

	// �ִϸ��̼� ����
	m_pHookaAnimation = m_pGameObject->FindComponentFromType<Animation>(CT_ANIMATION);

	if (nullptr == m_pHookaAnimation)
	{
		m_pHookaAnimation = m_pGameObject->AddComponent<Animation>("HookaAni");
		m_pHookaAnimation->LoadBone("HookaFootman_A.bne");
		m_pHookaAnimation->Load("HookaFootman_A.anm");
		m_pHookaAnimation->ChangeClip("Idle");
	}

	m_pNavigation = m_pGameObject->AddComponent<Navigation>("Nav");

	return true;
}

int HookaFootmanA::Input(float _fTime)
{
	return 0;
}

int HookaFootmanA::Update(float _fTime)
{
	HPCheck();

	switch (m_eState)
	{
	case IDLE:
		Idle();
		break;
	case WALK:
		Walk();
		break;
	case RUN:
		Run();
		break;
	case WAIT:
		Wait();
		break;
	case TALK:
		Talk();
		break;
	case DANCE_Q:
		Dance_Q();
		break;
	case DEATH:
		Death();
		break;
	case DEATHWAIT:
		DeathWait();
		break;
	case ATK01:
		Attack01();
		break;
	case ATK02:
		Attack02();
		break;
	default:
		break;
	}
	return 0;
}

int HookaFootmanA::LateUpdate(float _fTime)
{
	return 0;
}

int HookaFootmanA::Collision(float _fTime)
{
	return 0;
}

int HookaFootmanA::PrevRender(float _fTime)
{
	return 0;
}

int HookaFootmanA::Render(float _fTime)
{
	return 0;
}

HookaFootmanA * HookaFootmanA::Clone() const
{
	return new HookaFootmanA(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void HookaFootmanA::Idle()
{
	// Idle -> Run, Walk(���� �̵�), Wait, Talk 
	ChangeAnimation("Idle");

	if (true == m_pHookaAnimation->IsAnimationEnd())
	{
		// ���� - Walk, Wait, Talk �� ���º���
		int RandomNum = rand() % 3;

		switch (RandomNum)
		{
		case 0:
			m_eState = BaseState::WALK;
			break;
		case 1:
			m_eState = BaseState::WAIT;
			break;
		case 2:
			m_eState = BaseState::TALK;
			break;
		default:
			break;
		}
	}
}

void HookaFootmanA::Walk()
{
	// Walk -> Walk(�ٸ����� or ��������), Idle, Run
	ChangeAnimation("Walk");

	// Walk�ִϸ��̼ǿ��� ���� �������� �� 60% Ȯ���� �� �Ȱ� �Ѵ�.
	if (true == m_pHookaAnimation->IsAnimationEnd())
	{
		int RandomNum = rand() % 3;

		switch (RandomNum)
		{
		case 0:
			m_eState = BaseState::WALK;
			break;
		case 1:
			m_eState = BaseState::WALK;
			break;
		case 2:
			m_eState = BaseState::IDLE;
			break;
		default:
			break;
		}
	}

	// ��ȸ�Ҷ��� ������ ��ġ�� �����ؼ� �����ϰԲ� �Ѵ�.
	// ȸ���� Y ȸ���� ���������Ѵ�.
	float fWanderRadius = 2.0f;					// ���� ��ġ�κ��� ��ǥ ���������� �Ÿ�
	int iWanderYRot = 0;
	int iWanderYRotMin = 0;
	int iWanderYRotMax = (rand() % 180) + 180;	// 180 ~ 360

	// ���� ȸ�� ���� ���� (Y��)
	std::uniform_int_distribution<int> YRotRange(iWanderYRotMin, iWanderYRotMax);
	YRotRange(iWanderYRot);


}

void HookaFootmanA::Run()
{
	// ���� ����϶��� Run
	ChangeAnimation("Run");
}

void HookaFootmanA::Wait()
{
	ChangeAnimation("Wait");
	ChangeIdleState();
}

void HookaFootmanA::Talk()
{
	ChangeAnimation("Talk");
	ChangeIdleState();
}

void HookaFootmanA::Dance_Q()
{
	ChangeAnimation("DanceQ");
	ChangeIdleState();
}

void HookaFootmanA::Death()
{
	ChangeAnimation("Death");
}

void HookaFootmanA::DeathWait()
{
	ChangeAnimation("DeathWait");
}

void HookaFootmanA::Attack01()
{
	ChangeAnimation("Atk01");
	ChangeIdleState();
}

void HookaFootmanA::Attack02()
{
	ChangeAnimation("Atk02");
	ChangeIdleState();
}

// ����
void HookaFootmanA::Trace(Collider * _pSrc, Collider * _pDest, float _fTime)
{
	if ("PlayerBody" == _pDest->GetTag())
	{
		// ���ݸ���� ��쿡�� ������ ��� �����.
		if (m_eState == BaseState::ATK01 || m_eState == BaseState::ATK02)
		{
			return;
		}

		m_eState = BaseState::RUN;
		m_pNavigation->SetAIFindPath(true);
		m_pNavigation->SetTargetDetectTime(1.0f);
		m_pNavigation->SetTarget((Component*)_pDest);
	}
}

void HookaFootmanA::TraceExit(Collider * _pSrc, Collider * _pDest, float _fTime)
{
	if ("PlayerBody" == _pDest->GetTag())
	{
		m_eState = BaseState::IDLE;
		m_pNavigation->SetAIFindPath(false);
		m_pNavigation->SetTargetDetectTime(0.0f);
		m_pNavigation->SetTarget((Component*)nullptr);
	}
}

void HookaFootmanA::ChangeIdleState()
{
	// �ش� �ִϸ��̼� ������ ������ �ٷ� Idle��� ����
	if (true == m_pHookaAnimation->IsAnimationEnd())
	{
		m_eState = BaseState::IDLE;
	}
}

void HookaFootmanA::ChangeAnimation(std::string _strName)
{
	// �ִϸ��̼� �ٲٱ�
	PANIMATIONCLIP pCurAniClip = m_pHookaAnimation->GetCurrentClip();

	if (pCurAniClip->strName != _strName)
	{
		m_pHookaAnimation->ChangeClip(_strName);
	}
}

void HookaFootmanA::HPCheck()
{
	if (m_tUIState.m_iHP < 0)
	{
		m_eState = BaseState::DEATH;
	}
}
