#pragma once
#include "Scene\SceneComponent.h"
#include "GameObject.h"
#include "Scene\Scene.h"
#include "Scene\Layer.h"
#include "Component\Renderer.h"
#include "Component\TPCamera.h"
#include "Component\Particle.h"
#include "Component\AnimationFrame.h"
#include "Component\LandScape.h"
#include "..\UserComponent\Player.h"
#include "..\UserComponent\PlayerHead.h"
#include "..\UserComponent\Weapon.h"
#include "..\UserComponent\Monster.h"
#include "..\UserComponent\Bullet.h"
#include "..\UserComponent\StateBar.h"
#include "..\UserComponent\PlayerCameraPivot.h"
#include "CollisionManager.h"

ENGINE_USING

class MainScene : public SceneComponent
{
private:
	Layer* pDefaultLayer;

private:
	GameObject* pPlayerObject;
	GameObject* pPlayerHeadObject;
	GameObject*	pPlayerWeaponObject;
	GameObject* pPlayerCameraPivotObject;

	GameObject* pMonsterObject;
	GameObject* pParticleObject;
	GameObject* pLandScapeObject;
	GameObject* pMainCameraObject;

	// UI
	GameObject* pHPBarUIObj;
	GameObject* pMPBarUIObj;

	GameObject* pHPText;
	GameObject* pMPText;

private:
	// 스크립트
	Player*				pPlayerScript;
	PlayerHead*			pPlayerFaceScript;
	Weapon*				pPlayerWeaponScript;
	PlayerCameraPivot*	pPlayerCameraPivotScript;

	StateBar* pHpBarScript;
	StateBar* pMPBarScript;

public:
	MainScene();
	~MainScene();

public:
	virtual bool Init();

private:
	void CollisionChannelInit();
	void CollisionProfileInit();
	void CollisionProfileChannelStateInit();

	bool LayerInit();
	bool ProtoTypeInit();
	bool PlayerInit();
	bool MonsterInit();
	bool ParticleInit();
	bool LandScapeInit();

	bool TPCameraInit();

	bool UIInit();

	void InstancingTestInit();

};

