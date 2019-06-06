#include "TitleScene.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Device.h"
#include "Scene/Layer.h"
#include "GameObject.h"
#include "Component/Renderer.h"
#include "Component/TPCamera.h"
#include "Component/Transform.h"
#include "Component/Material.h"
#include "Component/Particle.h"
#include "Component/AnimationFrame.h"
#include "Component/UIButton.h"
#include "Component/LandScape.h"
#include "Component/UIText.h"
#include "Component/AudioSound.h"
#include "../UserComponent/Player.h"
#include "../UserComponent/Monster.h"
#include "../UserComponent/Bullet.h"
#include "CollisionManager.h"
#include "Render/RenderManager.h"

#include "MainScene.h"


TitleScene::TitleScene()
{
}


TitleScene::~TitleScene()
{
}

bool TitleScene::Init()
{
	Layer* pDefaultLayer = m_pScene->FindLayer("Default");

	Layer*	pUILayer = m_pScene->FindLayer("UI");

	GameObject*	pStartButtonObj = GameObject::CreateObject("StartButton",
		pUILayer);

	Transform*	pTr = pStartButtonObj->GetTransform();

	//pTr->SetWorldPosition(_RESOLUTION.iWidth / 2.f - 100.f, _RESOLUTION.iHeight / 2.f - 50.f, 0.f);

	pTr->SetWorldPosition(_RESOLUTION.iWidth / 3.f - 100.f, _RESOLUTION.iHeight / 4.f - 50.f, 0.f);

	SAFE_RELEASE(pTr);

	UIButton*	pStartButton = pStartButtonObj->AddComponent<UIButton>("StartButton");

	pStartButton->SetStateTexture(BS_NORMAL, "StartButton", TEXT("Start.png"));
	pStartButton->SetCallBackFunc(BS_CLICK, this, &TitleScene::SceneChangeButton);

	SAFE_RELEASE(pStartButton);

	SAFE_RELEASE(pStartButtonObj);


	// 텍스트
	GameObject* pTextObject = GameObject::CreateObject("Text", pUILayer);
	Transform* pTextTransform = pTextObject->GetTransform();
	pTextTransform->SetWorldPosition(100.0f, 100.0f, 0.0f);

	SAFE_RELEASE(pTextTransform);

	UIText* pText = pTextObject->AddComponent<UIText>("Text");

	pText->SetText(TEXT("ㅇㅁㅇ 익명이는 귀여워"));
	pText->Shadow(true);
	pText->SetShadowColor(1.0f, 0.0f, 1.0f, 1.0f);
	pText->SetShadowOffset(Vector3(2.f, -2.f, 0.f));
	pText->SetShadowOpacity(1.f);
	pText->SetFont("Gungseo");

	pText->SetColor(1.f, 1.f, 0.f, 1.f);
	pText->SetOpacity(0.5f);
	pText->AlphaBlend(true);
	pText->SetRenderArea(0.f, 0.f, 300.f, 50.f);

	SAFE_RELEASE(pText);
	SAFE_RELEASE(pTextObject);

	// 배경음
	GameObject* pBGMObject = GameObject::CreateObject("BGM", pDefaultLayer);
	AudioSound* pBGM = pBGMObject->AddComponent<AudioSound>("BGM");
	pBGM->SetSound("StartBGM", m_pScene, true, "MainBgm.mp3");
	pBGM->Play();

	SAFE_RELEASE(pBGM);
	SAFE_RELEASE(pBGMObject);

	return true;
}

void TitleScene::CreatePrototype()
{
}

void TitleScene::SceneChangeButton(float _fTime)
{
	Scene* pScene = GET_SINGLETON(SceneManager)->CreateScene<MainScene>();
	GET_SINGLETON(SceneManager)->SetNextScene(pScene);
	GET_SINGLETON(RenderManager)->SetPostEffectEnable(true);
	GET_SINGLETON(RenderManager)->SetShadowCompute(true);
	GET_SINGLETON(RenderManager)->SetSSAOEnable(false);
}
