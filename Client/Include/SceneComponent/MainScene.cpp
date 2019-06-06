#include "MainScene.h"

MainScene::MainScene() :
	pDefaultLayer(nullptr),
	pPlayerObject(nullptr),
	pPlayerHeadObject(nullptr),
	pPlayerCameraPivotObject(nullptr),
	pMonsterObject(nullptr),
	pParticleObject(nullptr),
	pPlayerScript(nullptr),
	pMainCameraObject(nullptr),
	pHPBarUIObj(nullptr),
	pMPBarUIObj(nullptr),
	pHpBarScript(nullptr),
	pMPBarScript(nullptr),
	pPlayerWeaponObject(nullptr),
	pPlayerWeaponScript(nullptr),
	pPlayerCameraPivotScript(nullptr)
{
	pHPText = nullptr;
	pMPText = nullptr;
}


MainScene::~MainScene()
{
	SAFE_RELEASE(pPlayerScript);
	SAFE_RELEASE(pPlayerObject);
	SAFE_RELEASE(pPlayerFaceScript);
	SAFE_RELEASE(pPlayerHeadObject);
	SAFE_RELEASE(pPlayerWeaponObject);
	SAFE_RELEASE(pPlayerCameraPivotObject);
	SAFE_RELEASE(pPlayerWeaponScript);

	SAFE_RELEASE(pMonsterObject);
	SAFE_RELEASE(pParticleObject);
	SAFE_RELEASE(pLandScapeObject);

	SAFE_RELEASE(pMainCameraObject);

	SAFE_RELEASE(pPlayerCameraPivotScript);


	// UI
	SAFE_RELEASE(pHPText);
	//SAFE_RELEASE(pMPText);

	SAFE_RELEASE(pHpBarScript);
	SAFE_RELEASE(pMPBarScript);

	SAFE_RELEASE(pHPBarUIObj);
	SAFE_RELEASE(pMPBarUIObj);
}

bool MainScene::Init()
{
	// Collision - Channel
	CollisionChannelInit();

	// Collision - Profile
	CollisionProfileInit();

	CollisionProfileChannelStateInit();


	// 레이어
	LayerInit();

	// 프로토타입
	ProtoTypeInit();

	// 플레이어
	PlayerInit();

	// 몬스터
	MonsterInit();

	// 파티클
	ParticleInit();

	// 지형
	LandScapeInit();

	// 3인칭 카메라
	TPCameraInit();

	// 인스턴싱 테스트
	InstancingTestInit();

	UIInit();

	return true;
}

void MainScene::CollisionChannelInit()
{
	// Channel 생성
	GET_SINGLETON(CollisionManager)->AddChannel("Player");
	GET_SINGLETON(CollisionManager)->AddChannel("Enemy");
	GET_SINGLETON(CollisionManager)->AddChannel("PlayerAttack");
	GET_SINGLETON(CollisionManager)->AddChannel("EnemyAttack");
	GET_SINGLETON(CollisionManager)->AddChannel("PickSphere");
	GET_SINGLETON(CollisionManager)->AddChannel("Trace");
}

void MainScene::CollisionProfileInit()
{
	//  Profile 생성
	GET_SINGLETON(CollisionManager)->CreateProfile("Player");
	GET_SINGLETON(CollisionManager)->CreateProfile("Enemy");
	GET_SINGLETON(CollisionManager)->CreateProfile("Trace");
}

void MainScene::CollisionProfileChannelStateInit()
{
	// SetProfileChannelState(프로파일명, 채널명, 채널상태)
	GET_SINGLETON(CollisionManager)->SetProfileChannelState("Player", "Player", CCS_BLOCK);
	GET_SINGLETON(CollisionManager)->SetProfileChannelState("Player", "WorldStatic", CCS_BLOCK);
	GET_SINGLETON(CollisionManager)->SetProfileChannelState("Player", "WorldDynamic", CCS_BLOCK);
	GET_SINGLETON(CollisionManager)->SetProfileChannelState("Player", "Enemy", CCS_BLOCK);
	GET_SINGLETON(CollisionManager)->SetProfileChannelState("Player", "EneymyAttack", CCS_BLOCK);
	GET_SINGLETON(CollisionManager)->SetProfileChannelState("Player", "PickSphere", CCS_IGNORE);
	GET_SINGLETON(CollisionManager)->SetProfileChannelState("Player", "Trace", CCS_BLOCK);

	GET_SINGLETON(CollisionManager)->SetProfileChannelState("Enemy", "Enemy", CCS_BLOCK);
	GET_SINGLETON(CollisionManager)->SetProfileChannelState("Enemy", "WorldStatic", CCS_BLOCK);
	GET_SINGLETON(CollisionManager)->SetProfileChannelState("Enemy", "WorldDynamic", CCS_BLOCK);
	GET_SINGLETON(CollisionManager)->SetProfileChannelState("Enemy", "Player", CCS_BLOCK);
	GET_SINGLETON(CollisionManager)->SetProfileChannelState("Enemy", "PlayerAttack", CCS_BLOCK);
	GET_SINGLETON(CollisionManager)->SetProfileChannelState("Enemy", "PickSphere", CCS_IGNORE);

	GET_SINGLETON(CollisionManager)->SetProfileChannelState("Trace", "Player", CCS_BLOCK);
}

bool MainScene::LayerInit()
{
	pDefaultLayer = m_pScene->FindLayer("Default");
	return true;
}

bool MainScene::ProtoTypeInit()
{
	// 몬스터 프로토타입
	GameObject* pMonsterProtoType = GameObject::CreatePrototype("Monster", m_pScene);
	Monster* pMonsterProtoTypeScript = pMonsterProtoType->AddComponent<Monster>("Monster");

	SAFE_RELEASE(pMonsterProtoTypeScript);
	SAFE_RELEASE(pMonsterProtoType);

	// 총알 프로토타입
	GameObject* pBulletProtoType = GameObject::CreatePrototype("Bullet", m_pScene);
	Bullet* pBulletScriptProtoType = pBulletProtoType->AddComponent<Bullet>("Bullet");
	
	SAFE_RELEASE(pBulletScriptProtoType);
	SAFE_RELEASE(pBulletProtoType);

	// 파티클 프로토타입
	GameObject* pExplosionProtoType = GameObject::CreatePrototype("Explosion", m_pScene);

	Transform* pExplosionTransform = pExplosionProtoType->GetTransform();
	pExplosionTransform->SetWorldScale(3.0f, 3.0f, 3.0f);

	Renderer*	pExplosionRenderer = pExplosionProtoType->AddComponent<Renderer>("ExplosionRenderer");
	pExplosionRenderer->SetMesh("Particle");
	pExplosionRenderer->SetRenderState("AlphaBlend");

	Particle*	pExplosionParticle = pExplosionProtoType->AddComponent<Particle>("Explosion");

	std::vector<TCHAR*>	vecParticleFileName;				// 파티클 2D애니메이션 하려구 ㅇㅅㅇ 각 텍스처들 불러오기
	vecParticleFileName.reserve(89);

	for (size_t i = 1; i <= 89; ++i)
	{
		TCHAR*	pFileName = new TCHAR[MAX_PATH];
		memset(pFileName, 0, sizeof(TCHAR) * MAX_PATH);

		wsprintf(pFileName, TEXT("Explosion/Explosion%d.png"), i);

		vecParticleFileName.push_back(pFileName);
	}

	pExplosionParticle->LoadTextureSet("Explosion", vecParticleFileName);

	AnimationFrame* pExplosionFrame = pExplosionProtoType->AddComponent<AnimationFrame>("ExplosionAnimation");

	std::vector<TextureCoord> vecCoord;
	vecCoord.reserve(89);

	for (int i = 0; i < 89; ++i)
	{
		TextureCoord tCoord = {};
		tCoord.vStart = Vector2(0.0f, 0.0f);
		tCoord.vEnd = Vector2(320.0f, 240.0f);
		vecCoord.push_back(tCoord);
	}

	pExplosionFrame->CreateClip("Idle", AO_LOOP, Vector2(320.0f, 240.0f),
		vecCoord, 1.0f, 10, "Explosion", vecParticleFileName);

	SAFE_RELEASE(pExplosionFrame);
	SAFE_RELEASE(pExplosionParticle);
	SAFE_RELEASE(pExplosionRenderer);
	SAFE_RELEASE(pExplosionTransform);
	SAFE_RELEASE(pExplosionProtoType);
	Safe_Delete_Array_VectorList(vecParticleFileName);

	// 빗방울 파티클
	GameObject* pRainParticleProtoType = GameObject::CreatePrototype("Rain", m_pScene);

	Transform* pRainParticleTransform = pRainParticleProtoType->GetTransform();
	pRainParticleTransform->SetWorldScale(3.0f, 3.0f, 3.0f);

	Renderer*	pRainRenderer = pRainParticleProtoType->AddComponent<Renderer>("RainRenderer");
	pRainRenderer->SetMesh("Particle");
	pRainRenderer->SetRenderState("AlphaBlend");

	Particle*	pRainParticle = pRainParticleProtoType->AddComponent<Particle>("Rain");

	std::vector<TCHAR*>	vecRainParticleFileName;				// 파티클 2D애니메이션 하려구 ㅇㅅㅇ 각 텍스처들 불러오기
	vecRainParticleFileName.reserve(89);

	for (size_t i = 1; i <= 1; ++i)
	{
		TCHAR*	pFileName = new TCHAR[MAX_PATH];
		memset(pFileName, 0, sizeof(TCHAR) * MAX_PATH);

		wsprintf(pFileName, TEXT("RainStreak.dds"), i);

		vecRainParticleFileName.push_back(pFileName);
	}

	pRainParticle->LoadTextureSet("Rain", vecRainParticleFileName);

	AnimationFrame* pRainFrame = pRainParticleProtoType->AddComponent<AnimationFrame>("RainAnimation");

	std::vector<TextureCoord> vecRainCoord;
	vecRainCoord.reserve(1);

	for (int i = 0; i < 1; ++i)
	{
		TextureCoord tCoord = {};
		tCoord.vStart = Vector2(0.0f, 0.0f);
		tCoord.vEnd = Vector2(320.0f, 240.0f);
		vecRainCoord.push_back(tCoord);
	}

	pRainFrame->CreateClip("Idle", AO_LOOP, Vector2(320.0f, 240.0f),
		vecRainCoord, 1.0f, 10, "Rain", vecRainParticleFileName);

	SAFE_RELEASE(pRainFrame);
	SAFE_RELEASE(pRainParticle);
	SAFE_RELEASE(pRainRenderer);
	SAFE_RELEASE(pRainParticleTransform);
	SAFE_RELEASE(pRainParticleProtoType);
	Safe_Delete_Array_VectorList(vecRainParticleFileName);


	// 의자
	GameObject*	pAnjangPrototype = GameObject::CreatePrototype("Anjang", m_pScene);
	pAnjangPrototype->SetRenderGroup(RG_STATIC);
	Renderer*	pAnjangRenderer = pAnjangPrototype->AddComponent<Renderer>("AnjangRenderer");

	pAnjangRenderer->SetMesh("Anjang", TEXT("chair.msh"));

	SAFE_RELEASE(pAnjangRenderer);

	SAFE_RELEASE(pAnjangPrototype);

	return true;
}

bool MainScene::PlayerInit()
{
	pPlayerObject = GameObject::CreateObject("Player", pDefaultLayer);
	pPlayerScript = pPlayerObject->AddComponent<Player>("Player");

	pPlayerHeadObject = GameObject::CreateObject("PlayerFace", pDefaultLayer);
	pPlayerHeadObject->SetParent(pPlayerObject);
	pPlayerFaceScript = pPlayerHeadObject->AddComponent<PlayerHead>("PlayerFace");

	pPlayerScript->SetHeadObject(pPlayerHeadObject);
	pPlayerScript->SetHeadScript(pPlayerFaceScript);

	pPlayerWeaponObject = GameObject::CreateObject("PlayerWeapon", pDefaultLayer);
	pPlayerWeaponObject->SetParent(pPlayerObject);
	pPlayerWeaponScript = pPlayerWeaponObject->AddComponent<Weapon>("PlayerWeapon");

	return true;
}

bool MainScene::MonsterInit()
{
	pMonsterObject = GameObject::CreateClone("Monster", m_pScene, pDefaultLayer);

	Transform* pMonTransform = pMonsterObject->GetTransform();
	pMonTransform->SetWorldPosition(15.0f, 1.0f, 30.0f);
	pMonTransform->LookAt(pPlayerObject);

	SAFE_RELEASE(pMonTransform);
	SAFE_RELEASE(pMonsterObject);

	return true;
}

bool MainScene::ParticleInit()
{
	pParticleObject = GameObject::CreateClone("Explosion", m_pScene, pDefaultLayer);
	return true;
}

bool MainScene::LandScapeInit()
{
	pLandScapeObject = GameObject::CreateObject("LandScape", pDefaultLayer);
	pLandScapeObject->SetRenderGroup(RG_LANDSCAPE);
	LandScape*	pLandScape = pLandScapeObject->AddComponent<LandScape>("LandScape");

	pLandScape->CreateLandScape("LandScape", "LandScape/Height3.bmp");


	pLandScape->AddSplatTexture(TEXT("LandScape/BD_Terrain_Cliff05.dds"),
		TEXT("LandScape/BD_Terrain_Cliff05_NRM.bmp"),
		TEXT("LandScape/BD_Terrain_Cliff05_SPEC.bmp"),
		TEXT("LandScape/RoadAlpha.bmp"));
	pLandScape->AddSplatTexture(TEXT("LandScape/Terrain_Cliff_15_Large.dds"),
		TEXT("LandScape/Terrain_Cliff_15_Large_NRM.bmp"),
		TEXT("LandScape/Terrain_Cliff_15_Large_SPEC.bmp"),
		TEXT("LandScape/SandBaseAlpha.bmp"));
	pLandScape->AddSplatTexture(TEXT("LandScape/Terrain_Pebbles_01.dds"),
		TEXT("LandScape/Terrain_Pebbles_01_NRM.bmp"),
		TEXT("LandScape/Terrain_Pebbles_01_SPEC.bmp"),
		TEXT("LandScape/WaterBaseAlpha.bmp"));

	pLandScape->CreateSplatTexture();

	SAFE_RELEASE(pLandScape);

	return true;
}

bool MainScene::TPCameraInit()
{
	pMainCameraObject = m_pScene->GetMainCameraObject();

	// 플레이어
	pPlayerCameraPivotObject = GameObject::CreateObject("PlayerPivot", pDefaultLayer);
	pPlayerCameraPivotObject->SetParent(pPlayerObject);

	TPCamera* pTPCamera = pMainCameraObject->AddComponent<TPCamera>("ThirdPersonCamera");
	pTPCamera->SetMouseEnable(true);
	pTPCamera->SetTarget(pPlayerCameraPivotObject);

	pPlayerCameraPivotScript = pPlayerCameraPivotObject->AddComponent<PlayerCameraPivot>("PlayerPivot");
	pPlayerScript->SetMoveAniChildObject(pPlayerCameraPivotObject);
	pPlayerScript->SetMoveAniChildScript(pPlayerCameraPivotScript);
	SAFE_RELEASE(pTPCamera);

	return true;
}

bool MainScene::UIInit()
{
	// UI - 상태바
	// HP
	pHPBarUIObj = GameObject::CreateObject("HPBar", pDefaultLayer);
	pHpBarScript = pHPBarUIObj->AddComponent<StateBar>("UI_StateBar_HP");
	pHpBarScript->SetMin(0);
	pHpBarScript->SetMax(10000);

	Transform* pHPTr = pHPBarUIObj->GetTransform();
	pHPTr->SetWorldPosition(100.0f, 100.0f, 0.0f);


	// MP
	pMPBarUIObj = GameObject::CreateObject("MPBar", pDefaultLayer);
	pMPBarScript = pMPBarUIObj->AddComponent<StateBar>("UI_StateBar_MP");
	pMPBarScript->SetMin(0);
	pMPBarScript->SetMax(20000);

	Transform* pMPTr = pMPBarUIObj->GetTransform();
	pMPTr->SetWorldPosition(100.0f, 250.0f, 0.0f);


	//  UI - 텍스트
	// 상태바
	pHPText = GameObject::CreateObject("HPText", pDefaultLayer);
	Transform* pHPTextTr = pHPText->GetTransform();
	pHPTextTr->SetWorldPosition(pHPTr->GetWorldPosition());

	UIText* pHPUIText = pHPText->AddComponent<UIText>("Text");
	pHPUIText->SetText(TEXT("HP"));
	pHPUIText->Shadow(true);
	pHPUIText->SetShadowColor(1.0f, 0.0f, 1.0f, 1.0f);
	pHPUIText->SetShadowOffset(Vector3(2.f, -2.f, 0.f));
	pHPUIText->SetShadowOpacity(1.f);
	pHPUIText->SetFont("Gungseo");
	pHPUIText->SetColor(1.f, 1.f, 0.f, 1.f);
	pHPUIText->SetOpacity(0.5f);
	pHPUIText->AlphaBlend(true);
	pHPUIText->SetRenderArea(0.f, 0.f, 300.f, 50.f);

	pHpBarScript->SetUIText(pHPUIText);			// 연결

	//pMPText = GameObject::CreateObject("MPText", pDefaultLayer);
	//Transform* pMPTextTr = pMPText->GetTransform();
	//pMPTextTr->SetWorldPosition(pHPTr->GetWorldPosition());
	//UIText* pMPUIText = pMPText->AddComponent<UIText>("Text");
	//pMPUIText->SetText(TEXT("MP"));
	//pMPUIText->Shadow(true);
	//pMPUIText->SetShadowColor(1.0f, 0.0f, 1.0f, 1.0f);
	//pMPUIText->SetShadowOffset(Vector3(2.f, -2.f, 0.f));
	//pMPUIText->SetShadowOpacity(1.f);
	//pMPUIText->SetFont("Gungseo");
	//pMPUIText->SetColor(1.f, 1.f, 0.f, 1.f);
	//pMPUIText->SetOpacity(0.5f);
	//pMPUIText->AlphaBlend(true);
	//pMPUIText->SetRenderArea(0.f, 0.f, 300.f, 50.f);

	//pMPBarScript->SetUIText(pMPUIText);			// 연결

	// Release
	SAFE_RELEASE(pHPTr);
	SAFE_RELEASE(pMPTr);

	SAFE_RELEASE(pHPUIText);

	SAFE_RELEASE(pHPTextTr);

	//SAFE_RELEASE(pMPUIText);

	return true;
}

void MainScene::InstancingTestInit()
{
	return;
	for (int i = 0; i < 12; ++i)
	{
		for (int j = 0; j < 25; ++j)
		{
			// 스태틱 인스턴싱
			//GameObject*	pAnjangObj = GameObject::CreateClone("Anjang",
			//	m_pScene, pDefaultLayer);

			//Transform*	pAnjangTr = pAnjangObj->GetTransform();

			//pAnjangTr->SetWorldScale(0.02f, 0.02f, 0.02f);
			//pAnjangTr->SetWorldPosition(j * 2.f, 6.f, 7.f + i * 2.f);

			//SAFE_RELEASE(pAnjangTr);
			//SAFE_RELEASE(pAnjangObj);

			// 2D애니메이션 , 프레임 애니메이션 인스턴싱Rain
			GameObject* pParticleObj = GameObject::CreateClone("Explosion", m_pScene, pDefaultLayer);
			//GameObject* pParticleObj = GameObject::CreateClone("Rain", m_pScene, pDefaultLayer);
			Transform*	pParticleTr = pParticleObj->GetTransform();
			pParticleTr->SetWorldPosition(j * 2.f, 0.0f, i * 2.f);

			SAFE_RELEASE(pParticleTr);
			SAFE_RELEASE(pParticleObj);

		}
	}

}
