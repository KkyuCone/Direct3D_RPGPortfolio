#include "MainScene.h"
#include "Device.h"

MainScene::MainScene() :
	pDefaultLayer(nullptr),
	pPlayerObject(nullptr),
	pPlayerHeadObject(nullptr),
	pPlayerCameraPivotObject(nullptr),
	pMonsterObject(nullptr),
	pHookaObject(nullptr),
	pHookaPivotObject(nullptr),
	pParticleObject(nullptr),
	pPlayerScript(nullptr),
	pMainCameraObject(nullptr),
	pHPBarUIObj(nullptr),
	pMPBarUIObj(nullptr),
	pEXPBarUIObj(nullptr),
	pHpBarScript(nullptr),
	pMPBarScript(nullptr),
	pEXPBarScript(nullptr),
	pPlayerWeaponObject(nullptr),
	pPlayerWeaponScript(nullptr),
	pPlayerCameraPivotScript(nullptr),
	pQuickSlotBGObj(nullptr),
	pQuickSlotScript(nullptr),
	pInventoryObj(nullptr),
	pInventoryScript(nullptr)
{
	pHPText = nullptr;
	pMPText = nullptr;
	pEXPText = nullptr;

	m_pHPUIBar = nullptr;
	m_pMPUIBar = nullptr;
	m_pEXPUIBar = nullptr;
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
	SAFE_RELEASE(pHookaObject);
	SAFE_RELEASE(pHookaPivotObject);
	SAFE_RELEASE(pParticleObject);
	SAFE_RELEASE(pLandScapeObject);

	SAFE_RELEASE(pMainCameraObject);

	SAFE_RELEASE(pPlayerCameraPivotScript);


	// UI
	SAFE_RELEASE(pHPText);
	SAFE_RELEASE(pMPText);
	SAFE_RELEASE(pEXPText);

	SAFE_RELEASE(pHpBarScript);
	SAFE_RELEASE(pMPBarScript);
	SAFE_RELEASE(pEXPBarScript);

	SAFE_RELEASE(m_pHPUIBar);
	SAFE_RELEASE(m_pMPUIBar);
	SAFE_RELEASE(m_pEXPUIBar);

	SAFE_RELEASE(pHPBarUIObj);
	SAFE_RELEASE(pMPBarUIObj);
	SAFE_RELEASE(pEXPBarUIObj);

	SAFE_RELEASE(pQuickSlotBGObj);
	SAFE_RELEASE(pQuickSlotScript);

	SAFE_RELEASE(pInventoryObj);
	SAFE_RELEASE(pInventoryScript);
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

	// UI 
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

	GameObject* pHookaProtoType = GameObject::CreatePrototype("Hooka", m_pScene);
	HookaFootmanA* pHookaProtoTypeScript = pHookaProtoType->AddComponent<HookaFootmanA>("Hooka");

	SAFE_RELEASE(pHookaProtoType);
	SAFE_RELEASE(pHookaProtoTypeScript);

	GameObject* pMonsterPivotProtoType = GameObject::CreatePrototype("MosnterPivot", m_pScene);
	SAFE_RELEASE(pMonsterPivotProtoType);

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

	// Hooka
	pHookaObject = GameObject::CreateClone("Hooka", m_pScene, pDefaultLayer);

	Transform* pHookaTransform = pHookaObject->GetTransform();
	pHookaTransform->SetWorldPosition(15.0f, 1.0f, 60.0f);
	pHookaTransform->LookAt(pPlayerObject);

	pHookaPivotObject = GameObject::CreateClone("MosnterPivot", m_pScene, pDefaultLayer);
	pHookaPivotObject->SetParent(pHookaObject);
	pHookaPivotObject->SetBoneSoket("Bone05");

	HookaFootmanA* pHookaObjectScript = pHookaObject->FindComponentFromType<HookaFootmanA>(CT_USERCOMPONENT);
	pHookaObjectScript->SetMonsterPivotObject(pHookaPivotObject);

	SAFE_RELEASE(pHookaObjectScript);
	SAFE_RELEASE(pHookaTransform);
	SAFE_RELEASE(pHookaObject);

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
	Layer*	pUILayer = m_pScene->FindLayer("UI");
	// UI - 상태바
	// HP,MP 배경 오브젝트
	GameObject* pHPBarBGObj = GameObject::CreateObject("StateBar_BG", pUILayer);
	UIBar* pHPBarBG = pHPBarBGObj->AddComponent<UIBar>("StateBar_BG");
	pHPBarBG->SetTexture("StateBar_BG", TEXT("ab2_bonus_frame02.png"), PATH_UI_GAGEBAR);
	pHPBarBG->SetShader(SHADER_UI_BAR);

	Transform* pHPBGTr = pHPBarBGObj->GetTransform();
	pHPBGTr->SetWorldScale(738.0f, 184.8f, 1.0f);
	pHPBGTr->SetWorldPosition(_RESOLUTION.iWidth / 2.f - 369.0f, _RESOLUTION.iHeight / 2.f - 410.f, 2.0f);

	SAFE_RELEASE(pHPBarBGObj);
	SAFE_RELEASE(pHPBarBG);
	SAFE_RELEASE(pHPBGTr);

	GameObject* pHPBarBGObj02 = GameObject::CreateObject("StateBar_BG02", pUILayer);
	UIBar* pHPBarBG02 = pHPBarBGObj02->AddComponent<UIBar>("StateBar_BG02");
	pHPBarBG02->SetTexture("StateBar_BG02", TEXT("ab2_bonus_frame_globe_borders.png"), PATH_UI_GAGEBAR);
	pHPBarBG02->SetShader(SHADER_UI_BAR);

	Transform* pHPBGTr02 = pHPBarBGObj02->GetTransform();
	pHPBGTr02->SetWorldScale(166.8f, 166.8f, 1.0f);
	pHPBGTr02->SetWorldPosition(_RESOLUTION.iWidth * 0.5f - 82.0f, _RESOLUTION.iHeight * 0.5f - 390.0f, 1.3f);

	SAFE_RELEASE(pHPBarBGObj02);
	SAFE_RELEASE(pHPBarBG02);
	SAFE_RELEASE(pHPBGTr02);

	// HP 오브젝트
	pHPBarUIObj = GameObject::CreateObject("HPBar", pUILayer);
	m_pHPUIBar = pHPBarUIObj->AddComponent<UIBar>("HPBar");
	m_pHPUIBar->SetTexture("HPBarGage", TEXT("ab2_bonus_frame_globe_fill_red.png"), PATH_UI_GAGEBAR);
	m_pHPUIBar->SetShader(SHADER_UI_BAR);
	pHpBarScript = pHPBarUIObj->AddComponent<StateBar>("UI_StateBar_HP");
	pHpBarScript->SetMin(0);
	pHpBarScript->SetMax(10000);
	pHpBarScript->SetUIBar(m_pHPUIBar);

	Transform* pHPTr = pHPBarUIObj->GetTransform();
	pHPTr->SetWorldScale(81.0f, 162.0f, 1.0f);
	pHPTr->SetWorldPosition(_RESOLUTION.iWidth * 0.5f - 80.0f, _RESOLUTION.iHeight * 0.5f - 389.f, 1.5f);

	SAFE_RELEASE(m_pHPUIBar);
	SAFE_RELEASE(pHpBarScript);
	SAFE_RELEASE(pHPTr);


	// MP
	pMPBarUIObj = GameObject::CreateObject("MPBar", pUILayer);
	m_pMPUIBar = pMPBarUIObj->AddComponent<UIBar>("MPBar");
	m_pMPUIBar->SetTexture("MPBarGage", TEXT("ab2_bonus_frame_globe_fill_blue.png"), PATH_UI_GAGEBAR);
	m_pMPUIBar->SetShader(SHADER_UI_BAR);
	pMPBarScript = pMPBarUIObj->AddComponent<StateBar>("UI_StateBar_MP");
	pMPBarScript->SetMin(0);
	pMPBarScript->SetMax(10000);
	pMPBarScript->SetUIBar(m_pMPUIBar);

	Transform* pMPTr = pMPBarUIObj->GetTransform();
	pMPTr->SetWorldScale(81.0f, 162.0f, 1.0f);
	pMPTr->SetWorldPosition(_RESOLUTION.iWidth * 0.5f - 1.0f, _RESOLUTION.iHeight * 0.5f - 389.f, 1.5f);

	SAFE_RELEASE(m_pMPUIBar);
	SAFE_RELEASE(pMPBarScript);
	SAFE_RELEASE(pMPTr);


	////  UI - 텍스트
	//// 상태바
	//pHPText = GameObject::CreateObject("HPText", pDefaultLayer);
	//Transform* pHPTextTr = pHPText->GetTransform();
	//pHPTextTr->SetWorldPosition(pHPTr->GetWorldPosition());

	//UIText* pHPUIText = pHPText->AddComponent<UIText>("Text");
	//pHPUIText->SetText(TEXT("HP"));
	//pHPUIText->Shadow(true);
	//pHPUIText->SetShadowColor(1.0f, 0.0f, 1.0f, 1.0f);
	//pHPUIText->SetShadowOffset(Vector3(2.f, -2.f, 0.f));
	//pHPUIText->SetShadowOpacity(1.f);
	//pHPUIText->SetFont("Gungseo");
	//pHPUIText->SetColor(1.f, 1.f, 0.f, 1.f);
	//pHPUIText->SetOpacity(0.5f);
	//pHPUIText->AlphaBlend(true);
	//pHPUIText->SetRenderArea(0.f, 0.f, 300.f, 50.f);

	//pHpBarScript->SetUIText(pHPUIText);			// 연결

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
	/*SAFE_RELEASE(pHPTr);
	SAFE_RELEASE(pMPTr);

	SAFE_RELEASE(pHPUIText);

	SAFE_RELEASE(pHPTextTr);*/

	//SAFE_RELEASE(pMPUIText);


	// EXP 바
	// HP,MP 배경 오브젝트
	GameObject* pEXPBarBGObj = GameObject::CreateObject("EXPBar_BG", pUILayer);
	UIBar* pEXPBarBG = pEXPBarBGObj->AddComponent<UIBar>("EXPBar_BG");
	pEXPBarBG->SetTexture("EXPBar_BG", TEXT("ab2_xp_empty_frame.png"), PATH_UI_GAGEBAR);
	pEXPBarBG->SetShader(SHADER_UI_BAR);

	Transform* pEXPBGTr = pEXPBarBGObj->GetTransform();
	pEXPBGTr->SetWorldScale(1711.2f, 43.2f, 1.0f);
	pEXPBGTr->SetWorldPosition(_RESOLUTION.iWidth * 0.5f - 855.0f, _RESOLUTION.iHeight * 0.5f - 540.f, 1.5f);

	SAFE_RELEASE(pEXPBarBGObj);
	SAFE_RELEASE(pEXPBarBG);
	SAFE_RELEASE(pEXPBGTr);

	// EXP 오브젝트
	pEXPBarUIObj = GameObject::CreateObject("EXPBar", pUILayer);
	m_pEXPUIBar = pEXPBarUIObj->AddComponent<UIBar>("EXPBar");
	m_pEXPUIBar->SetTexture("EXPBarGage", TEXT("ab2_xp_fill.png"), PATH_UI_GAGEBAR);
	m_pEXPUIBar->SetShader(SHADER_UI_BAR);
	pEXPBarScript = pEXPBarUIObj->AddComponent<StateBar>("UI_StateBar_EXP");
	pEXPBarScript->SetMin(0);
	pEXPBarScript->SetMax(10000);
	pEXPBarScript->SetUIBar(m_pEXPUIBar);

	Transform* pEXPTr = pEXPBarUIObj->GetTransform();
	pEXPTr->SetWorldScale(1678.4f, 21.6f, 1.0f);
	pEXPTr->SetWorldPosition(_RESOLUTION.iWidth * 0.5f - 841.5f, _RESOLUTION.iHeight * 0.5f - 533.f, 1.4f);

	SAFE_RELEASE(m_pEXPUIBar);
	SAFE_RELEASE(pHpBarScript);
	SAFE_RELEASE(pEXPTr);


	// 퀵슬롯
	pQuickSlotBGObj = GameObject::CreateObject("QuickSlot", pUILayer);
	pQuickSlotScript = pQuickSlotBGObj->AddComponent<QuickSlot>("QuickSlot");

	Transform* pQuickSlotTR = pQuickSlotBGObj->GetTransform();
	pQuickSlotTR->SetWorldScale(1162.2f, 139.8f, 1.0f);
	pQuickSlotTR->SetWorldPosition(_RESOLUTION.iWidth * 0.5f - 590.0f, _RESOLUTION.iHeight * 0.5f - 528.f, 1.4f);
	SAFE_RELEASE(pQuickSlotTR);


	// 인벤토리
	pInventoryObj = GameObject::CreateObject("Inventory", pUILayer);
	pInventoryScript = pInventoryObj->AddComponent<Inventory>("Inventory");

	Transform* pInventoryObjTR = pInventoryObj->GetTransform();
	pInventoryObjTR->SetWorldPosition(_RESOLUTION.iWidth * 0.5f, _RESOLUTION.iHeight * 0.5f, 1.0f);
	pInventoryObjTR->SetWorldScale(1.0f, 1.0f, 1.0f);

	SAFE_RELEASE(pInventoryObjTR);

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
