#include "Monster.h"
#include "Scene\Scene.h"
#include "Scene\Layer.h"
#include "Bullet.h"

Monster::Monster() : 
	pTransform(nullptr),
	m_pRenderer(nullptr), 
	m_pSphereCollider(nullptr), 
	m_pTraceCollider(nullptr),
	m_pMaterial(nullptr),
	m_pOBBCollider(nullptr),
	m_pAnimation(nullptr),
	m_pNavigation(nullptr),
	m_BulletReTime(3.0f), m_BulletCurTime(0.0f)
{
}

Monster::Monster(const Monster & _Monster) : UserComponent(_Monster)
{
	m_pAnimation = nullptr;
	m_pNavigation = nullptr;
}


Monster::~Monster()
{
	SAFE_RELEASE(m_pMaterial);
	SAFE_RELEASE(m_pRenderer);

	SAFE_RELEASE(m_pPlayerTrnasform);
	SAFE_RELEASE(pTransform);

	SAFE_RELEASE(m_pNavigation);
	SAFE_RELEASE(m_pAnimation);
}

void Monster::Start()
{
	// 애니메이션이랑 네비게이션 셋팅
	SAFE_RELEASE(m_pAnimation);
	m_pAnimation = m_pGameObject->FindComponentFromType<Animation>(CT_ANIMATION);

	if (nullptr != m_pAnimation)
	{
		m_pAnimation->ChangeClip("Walk");
	}

	SAFE_RELEASE(m_pNavigation);
	m_pNavigation = m_pGameObject->FindComponentFromType<Navigation>(CT_NAVIGATION);

	if (nullptr != m_pNavigation)
	{
		ColliderSphere* pTrace = m_pGameObject->FindComponentFromTag<ColliderSphere>("Trace");

		pTrace->SetCallback<Monster>(CCBS_ENTER, this, &Monster::Trace);
		pTrace->SetCallback<Monster>(CCBS_EXIT, this, &Monster::TraceExit);
		pTrace->SetScaleEnable(false);

		SAFE_RELEASE(pTrace);
	}
}

bool Monster::Init()
{
	pTransform = m_pGameObject->GetTransform();
	pTransform->SetWorldScale(0.05f, 0.05f, 0.05f);
	m_pTransform->SetLocalRotation(0.0f, -90.0f, 0.0f);

	m_pRenderer = m_pGameObject->AddComponent<Renderer>("MonsterRenderer");
	//m_pRenderer->SetMesh("Pyramid");
	m_pRenderer->SetMesh("Monster", TEXT("EnchantedDoll.msh"), Vector3::Axis[AXIS_Z], PATH_MESH);


	/*m_pMaterial = m_pGameObject->AddComponent<Material>("MonsterMaterial");
	m_pMaterial->SetColor(Vector4::White, Vector4::White, Vector4::Black, Vector4::Black, 1.0f);*/
	m_pMaterial = m_pGameObject->FindComponentFromType<Material>(CT_MATERIAL);
	

#pragma region Collision
	//m_pSphereCollider = m_pGameObject->AddComponent<ColliderSphere>("Body");
	//m_pSphereCollider->SetProfile("Enemy");
	//m_pSphereCollider->SetChannel("Enemy");
	//m_pSphereCollider->SetSphereInfo(Vector3::Zero, 1.0f);
	//m_pSphereCollider->PickEnable(true);
	//SAFE_RELEASE(m_pSphereCollider);


	m_pTraceCollider = m_pGameObject->AddComponent<ColliderSphere>("Trace");
	m_pTraceCollider->SetProfile("Trace");
	m_pTraceCollider->SetChannel("Trace");
	m_pTraceCollider->SetSphereInfo(Vector3::Zero, 15.0f);
	m_pTraceCollider->SetScaleEnable(false);
	m_pTraceCollider->PickEnable(false);
	SAFE_RELEASE(m_pTraceCollider);




	m_pOBBCollider = m_pGameObject->AddComponent<ColliderOBB>("Body");
	m_pOBBCollider->SetProfile("Enemy");
	m_pOBBCollider->SetChannel("Enemy");
	m_pOBBCollider->SetOBBInfo(Vector3::Zero, Vector3(0.5f, 0.5f, 0.5f));
	m_pOBBCollider->PickEnable(true);

	SAFE_RELEASE(m_pOBBCollider);
#pragma endregion


	m_pAnimation = m_pGameObject->FindComponentFromType<Animation>(CT_ANIMATION);

	if (nullptr == m_pAnimation)
	{
		m_pAnimation = m_pGameObject->AddComponent<Animation>("MonsterAni");
		m_pAnimation->LoadBone("EnchantedDoll.bne");
		m_pAnimation->Load("EnchantedDoll.anm");
	}

	Navigation*	pNav = m_pGameObject->AddComponent<Navigation>("Nav");
	//m_pNavigation = m_pGameObject->AddComponent<Navigation>("Nav");
	SAFE_RELEASE(pNav);

	return true;
}

int Monster::Input(float _fTime)
{
	return 0;
}

int Monster::Update(float _fTime)
{
	return 0;

#pragma region 숙제
	//if (nullptr != m_pPlayer)
	//{
	//	Vector3 CurMonsterPos = m_pTransform->GetWorldPosition();
	//	Vector3 CurPlayerPos = m_pPlayerTrnasform->GetWorldPosition();

	//	// 항상 방향 플레이어를 바라보게하는 코드 넣기
	//	// 1. 방향벡터구하기  -> 노말라이즈하기
	//	Vector3 DirVector = m_pPlayerTrnasform->GetWorldPosition() - m_pTransform->GetWorldPosition();
	//	DirVector.z = 0.0f;
	//	DirVector.Normalize();

	//	if (m_BulletCurTime >= m_BulletReTime)
	//	{
	//		// 3초마다 총알생성
	//		m_BulletCurTime = 0.0f;
	//		Fire(DirVector);
	//	}
	//	else
	//	{
	//		m_BulletCurTime += _fTime;
	//	}

	//	//DirectX::XMMatrixDecompose(,,,)
	//	Vector3 qqqq = Vector3(0.0f, 1.0f, 0.0f);

	//	float CTheta = acosf(DirVector.Dot(qqqq));

	//	CTheta = CTheta / ENGINE_PI * 180.0f;


	//	float aaaaa = (qqqq.Cross(DirVector)).z;

	//	//float AngleValue = acosf(DirVector.x) / ENGINE_PI * 180.0f;

	//	float ChangeTheta = CTheta;
	//	

	//	if (aaaaa > 0)
	//	{
	//		// 180도 보다 작음
	//		// 180
	//		//ChangeTheta = 360.0f - ChangeTheta;
	//		m_pTransform->SetWorldRotateZ(ChangeTheta);
	//	}
	//	else
	//	{
	//		m_pTransform->SetWorldRotateZ(-ChangeTheta);
	//	}


	//	//float dddd = cosf(DirVector.Dot(m_pPlayerTrnasform->GetWorldPosition()));

	//	//m_pTransform->SetWorldRotateZ(dddd);
	//}
#pragma endregion

}

int Monster::LateUpdate(float _fTime)
{
	return 0;
}

int Monster::Collision(float _fTime)
{
	return 0;
}

int Monster::PrevRender(float _fTime)
{
	return 0;
}

int Monster::Render(float _fTime)
{
	return 0;
}

Monster * Monster::Clone() const
{
	return new Monster(*this);
}

void Monster::Trace(Collider * _pSrc, Collider * _pDest, float _fTime)
{
	if ("PlayerBody" == _pDest->GetTag())
	{
		m_pNavigation->SetAIFindPath(true);
		m_pNavigation->SetTargetDetectTime(1.0f);
		m_pNavigation->SetTarget((Component*)_pDest);
	}
}

void Monster::TraceExit(Collider * _pSrc, Collider * _pDest, float _fTime)
{
	if ("PlayerBody" == _pDest->GetTag())
	{
		m_pNavigation->SetAIFindPath(false);
		m_pNavigation->SetTargetDetectTime(0.0f);
		m_pNavigation->SetTarget((Component*)nullptr);
	}
}

void Monster::Fire(Vector3 _Dir)
{
	//GameObject* NewBulletObj = GameObject::CreateObject("Bullet", m_pLayer);
	//Transform* BulletTransform = NewBulletObj->GetTransform();
	//BulletTransform->SetWorldPosition(m_pTransform->GetWorldPosition());
	//Bullet* BulletScript = NewBulletObj->AddComponent<Bullet>("BulletScript");
	//BulletScript->SetDir(_Dir);

	//SAFE_RELEASE(BulletTransform);
	//SAFE_RELEASE(BulletScript);
	//SAFE_RELEASE(NewBulletObj);
}
