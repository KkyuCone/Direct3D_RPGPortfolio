#include "PlayerHead.h"
#include "Scene\Scene.h"
#include "Scene\Layer.h"
#include "Time.h"
#include "GameObject.h"
#include "Component\Collider.h"
#include "Component\ColliderRay.h"
#include "Component\ColliderSphere.h"
#include "Component\ColliderOBB.h"
#include "Component\Navigation.h"
#include "Navigation\NavigationManager.h"
#include "Navigation\NavigationMesh.h"
#include "Render\RenderManager.h"



PlayerHead::PlayerHead() :
	m_pFaceTransform(nullptr)
	, m_pFaceRenderer(nullptr)
	, m_pFaceMaterial(nullptr)
	, m_pFaceAnimation(nullptr)
{
}

PlayerHead::PlayerHead(const PlayerHead & _Face)
{
}


PlayerHead::~PlayerHead()
{
	SAFE_RELEASE(m_pFaceTransform);
	SAFE_RELEASE(m_pFaceAnimation);
	SAFE_RELEASE(m_pFaceMaterial);
	SAFE_RELEASE(m_pFaceRenderer);
}

void PlayerHead::Start()
{
}

bool PlayerHead::Init()
{
	ComponentInit();
	return true;
}

int PlayerHead::Input(float _fTime)
{
	return 0;
}

int PlayerHead::Update(float _fTime)
{
	return 0;
}

int PlayerHead::LateUpdate(float _fTime)
{
	return 0;
}

int PlayerHead::Collision(float _fTime)
{
	return 0;
}

int PlayerHead::PrevRender(float _fTime)
{
	return 0;
}

int PlayerHead::Render(float _fTime)
{
	return 0;
}

PlayerHead * PlayerHead::Clone() const
{
	return nullptr;
}

void PlayerHead::ComponentInit()
{
	m_pFaceTransform = m_pGameObject->GetTransform();
	//m_pFaceTransform->SetLocalPosition(0.15f, 0.23f, 0.0f);
	//m_pFaceTransform->SetLocalScale(0.035f, 0.035f, 0.035f);
	m_pFaceTransform->SetLocalScale(1.0f, 1.0f, 1.0f);

	m_pFaceRenderer = m_pGameObject->AddComponent<Renderer>("PlayerHeadRenderer");
	m_pFaceRenderer->SetMesh("PlayerHead", TEXT("Popori_F_Head_Ani.msh"), Vector3::Axis[AXIS_Z], PATH_MESH);

	m_pFaceMaterial = m_pGameObject->FindComponentFromType<Material>(CT_MATERIAL);
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

	//ColliderSphere*		pPlayerBodyCol = m_pGameObject->AddComponent<ColliderSphere>("PlayerBody");
	//pPlayerBodyCol->SetProfile("Player");
	//pPlayerBodyCol->SetChannel("Player");
	//pPlayerBodyCol->SetSphereInfo(Vector3::Zero, 1.0f);
	//pPlayerBodyCol->SetCallback<Player>(CCBS_ENTER, this, &Player::TestHitEnter);
	//pPlayerBodyCol->SetCallback<Player>(CCBS_STAY, this, &Player::TestHitStay);
	//pPlayerBodyCol->SetCallback<Player>(CCBS_EXIT, this, &Player::TestHitExit);
	//SAFE_RELEASE(pPlayerBodyCol);


	//ColliderOBB*		pBodyCol = m_pGameObject->AddComponent<ColliderOBB>("Body");
	//pBodyCol->SetProfile("Player");
	//pBodyCol->SetChannel("Player");
	//pBodyCol->SetOBBInfo(Vector3(0.f, 0.5f, 0.f), Vector3(1.f, 0.5f, 0.5f));
	//pBodyCol->SetCallback<Player>(CCBS_ENTER, this, &Player::TestHitEnter);
	//pBodyCol->SetCallback<Player>(CCBS_STAY, this, &Player::TestHitStay);
	//pBodyCol->SetCallback<Player>(CCBS_EXIT, this, &Player::TestHitExit);

	//SAFE_RELEASE(pBodyCol);


#pragma endregion



	m_pFaceAnimation = m_pGameObject->FindComponentFromType<Animation>(CT_ANIMATION);

	if (nullptr == m_pFaceAnimation)
	{
		m_pFaceAnimation = m_pGameObject->AddComponent<Animation>("PalyerHeadAnimation");
		m_pFaceAnimation->LoadBone("Popori_F_Head_Ani.bne");			// 본정보 가져오기
		m_pFaceAnimation->Load("Popori_F_Head_Ani.anm");				// 애니메이션 정보가져오기

		m_pFaceAnimation->ChangeClip("CuttingSlash");
		m_pFaceAnimation->SetParentBoneName("Bip01-Head");
	}

	Navigation*	pNav = m_pGameObject->AddComponent<Navigation>("FaceNav");
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
	SAFE_RELEASE(m_pFaceTransform);
}

void PlayerHead::AnimationChange(std::string _strName)
{
	m_pFaceAnimation->ChangeClip(_strName);
}
