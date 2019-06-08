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
	, m_pPivotObj(nullptr)
{
	m_eState = BaseState::IDLE;

	m_tUIState.m_iLevel = 20;
	m_tUIState.m_iHP = 20000;
	m_tUIState.m_iMP = 1;
	m_tUIState.m_iCurEXP = 100;
	m_tUIState.m_iMaxEXP = 100;

	m_fCurWalkTime = 0.0f;
	m_fWalkTime = 0.0f;
}

HookaFootmanA::HookaFootmanA(const HookaFootmanA & _HookaFootmanA) : Base(_HookaFootmanA)
{
	*this = _HookaFootmanA;
	m_iReferenceCount = 1;

	m_pHookaMaterial = nullptr;
	m_pHookaRenderer = nullptr;
	m_pHookaAnimation = nullptr;
	m_pNavigation = nullptr;
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

	//SAFE_RELEASE(m_pPivotObj);
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void HookaFootmanA::Start()
{
	SAFE_RELEASE(m_pHookaRenderer);
	m_pHookaRenderer = m_pGameObject->FindComponentFromType<Renderer>(CT_RENDERER);

	SAFE_RELEASE(m_pHookaMaterial);
	m_pHookaMaterial = m_pGameObject->FindComponentFromType<Material>(CT_MATERIAL);

	SAFE_RELEASE(m_pHookaAnimation);
	m_pHookaAnimation = m_pGameObject->FindComponentFromType<Animation>(CT_ANIMATION);

	SAFE_RELEASE(m_pNavigation);
	m_pNavigation = m_pGameObject->FindComponentFromType<Navigation>(CT_NAVIGATION);

	if (nullptr != m_pNavigation)
	{
		SAFE_RELEASE(m_pTraceCollider);
		m_pTraceCollider = m_pGameObject->FindComponentFromTag<ColliderSphere>("Trace");

		m_pTraceCollider->SetCallback<HookaFootmanA>(CCBS_ENTER, this, &HookaFootmanA::Trace);
		m_pTraceCollider->SetCallback<HookaFootmanA>(CCBS_EXIT, this, &HookaFootmanA::TraceExit);
		m_pTraceCollider->SetScaleEnable(false);
	}

	SAFE_RELEASE(m_pSphereCollider);
	m_pSphereCollider = m_pGameObject->FindComponentFromTag<ColliderSphere>("HookaCol");

	m_pTransform->SetLookAtActive(false);
}

bool HookaFootmanA::Init()
{
	m_pTransform->SetWorldScale(0.08f, 0.08f, 0.08f);

	m_pHookaRenderer = m_pGameObject->AddComponent<Renderer>("HookaRenderer");
	m_pHookaRenderer->SetMesh("MonsterHooka", TEXT("HookaFootman_A.msh"), Vector3::Axis[AXIS_X], PATH_MESH);

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
		Idle(_fTime);
		break;
	case WALK:
		Walk(_fTime);
		break;
	case RUN:
		Run(_fTime);
		break;
	case WAIT:
		Wait(_fTime);
		break;
	case TALK:
		Talk(_fTime);
		break;
	case DANCE_Q:
		Dance_Q(_fTime);
		break;
	case DEATH:
		Death(_fTime);
		break;
	case DEATHWAIT:
		DeathWait(_fTime);
		break;
	case ATK01:
		Attack01(_fTime);
		break;
	case ATK02:
		Attack02(_fTime);
		break;
	default:
		break;
	}
	return 0;
}

int HookaFootmanA::LateUpdate(float _fTime)
{
	// �Ǻ��� �̵��Ǵ� �ִϸ��̼��� ��� �ش� ������Ʈ�� 
	// �� �����ӿ��� �̵������ش�.
	MovePivotPos(_fTime);
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

void HookaFootmanA::Idle(float _fTime)
{
	// Idle -> Run, Walk(���� �̵�), Wait, Talk 
	ChangeAnimation("Idle");

	if (true == m_pHookaAnimation->IsAnimationEnd())
	{
		// ���� - Walk, Wait, Talk �� ���º���
		int RandomNum = rand() % 5;

		switch (RandomNum)
		{
		case 0:
		case 1:
		case 2:
			m_eState = BaseState::WALK;
			SetWalkStateDir();
			break;
		case 3:
			m_eState = BaseState::WAIT;
			break;
		case 4:
			m_eState = BaseState::TALK;
			break;
		default:
			break;
		}
	}
}

void HookaFootmanA::Walk(float _fTime)
{
	// Walk -> Walk(�ٸ����� or ��������), Idle, Run
	ChangeAnimation("Walk");

	// Walk�ִϸ��̼ǿ��� ���� �������� �� 60% Ȯ���� �� �Ȱ� �Ѵ�.

	if (m_fCurWalkTime >= m_fWalkTime)
	{
		std::random_device seed;
		std::default_random_engine eng(seed());
		std::bernoulli_distribution Check(0.6f);
		bool bReWalk = Check(eng);

		if (true == bReWalk)
		{
			m_eState = BaseState::WALK;
			SetWalkStateDir();
		}
		else
		{
			m_fCurWalkTime = 0.0f;
			m_eState = BaseState::IDLE;
		}
	}
	else
	{
		m_fCurWalkTime += _fTime;
		m_pTransform->Move(AXIS_X, 1.0f, _fTime);
	}

}

void HookaFootmanA::Run(float _fTime)
{
	// ���� ����϶��� Run
	ChangeAnimation("Run");
}

void HookaFootmanA::Wait(float _fTime)
{
	ChangeAnimation("Wait");
	ChangeIdleState();
}

void HookaFootmanA::Talk(float _fTime)
{
	ChangeAnimation("Talk");
	ChangeIdleState();
}

void HookaFootmanA::Dance_Q(float _fTime)
{
	ChangeAnimation("DanceQ");
	ChangeIdleState();
}

void HookaFootmanA::Death(float _fTime)
{
	ChangeAnimation("Death");
}

void HookaFootmanA::DeathWait(float _fTime)
{
	ChangeAnimation("DeathWait");
}

void HookaFootmanA::Attack01(float _fTime)
{
	ChangeAnimation("Atk01");
	ChangeIdleState();
}

void HookaFootmanA::Attack02(float _fTime)
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

		// �Ÿ� ����
		Transform* pDestTR = _pDest->GetTransform();
		float fDistance = m_pTransform->GetWorldPositionAtMatrix().Distance(pDestTR->GetWorldPositionAtMatrix());

		if (5.0f > fDistance)
		{
			RandomAttackState(_fTime);
		}
		else
		{
			m_eState = BaseState::RUN;
			m_pTransform->SetLookAtActive(true);
			m_pNavigation->SetAIFindPath(true);
			m_pNavigation->SetTargetDetectTime(1.0f);
			m_pNavigation->SetTarget((Component*)_pDest);
		}

		SAFE_RELEASE(pDestTR);
	}
}

void HookaFootmanA::TraceExit(Collider * _pSrc, Collider * _pDest, float _fTime)
{
	if ("PlayerBody" == _pDest->GetTag())
	{
		m_eState = BaseState::IDLE;
		m_pTransform->SetLookAtActive(false);
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

void HookaFootmanA::MovePivotPos(float _fTime)
{
	if (nullptr == m_pPivotObj)
		return;

	PANIMATIONCLIP pCurAniClip = m_pHookaAnimation->GetCurrentClip();

	if ("Atk01" != pCurAniClip->strName && "Death" != pCurAniClip->strName)
	{
		return;
	}

	// �ش� �Ǻ� ��ġ�� �̵�
	if (true == m_pHookaAnimation->IsAnimationEnd())
	{
		Transform* pPivotTR = m_pPivotObj->GetTransform();
		m_pTransform->SetWorldPosition(pPivotTR->GetWorldPositionAtMatrix());
		SAFE_RELEASE(pPivotTR);
		
		m_pNavigation->LateUpdate(_fTime);
	}
}

void HookaFootmanA::SetWalkStateDir()
{
	// ��ȸ�Ҷ��� ������ ��ġ�� �����ؼ� �����ϰԲ� �Ѵ�.
	// ȸ���� Y ȸ���� ���������Ѵ�.
	float fWanderRadius = 2.0f;					// ���� ��ġ�κ��� ��ǥ ���������� �Ÿ�
	int iWanderYRot = 0;
	int iWanderYRotMin = 0;
	int iWanderYRotMax = (rand() % 180) + 180;	// 180 ~ 360

	// ���� ȸ�� ���� ���� (Y��)
	std::uniform_int_distribution<int> YRotRange(iWanderYRotMin, iWanderYRotMax);
	std::mt19937_64 rn;
	iWanderYRot = YRotRange(rn);

	m_pTransform->RotateY((float)iWanderYRot);

	int iWalkTime = rand() % 4;
	m_fWalkTime = (float)iWalkTime;

	m_fCurWalkTime = 0.0f;
}

void HookaFootmanA::RandomAttackState(float _fTime)
{
	int iRandom = rand() % 2;

	switch (iRandom)
	{
	case 0:
		m_eState = BaseState::ATK01;
		break;
	case 1:
		m_eState = BaseState::ATK02;
		break;
	default:
		break;
	}
}

void HookaFootmanA::HPCheck()
{
	if (m_tUIState.m_iHP < 0)
	{
		m_eState = BaseState::DEATH;
	}
}

void HookaFootmanA::SetMonsterPivotObject(GameObject * _pObject)
{
	m_pPivotObj = _pObject;
	//m_pPivotObj->SetParent(m_pGameObject);
	//m_pPivotObj->SetBoneSoket("Bip01");
}
