#include "SceneManager.h"
#include "Scene.h"
#include "..\InputManager.h"

ENGINE_USING

DEFINITION_SINGLETON(SceneManager)


SceneManager::SceneManager()
	: m_pCurScene(nullptr)
	, m_pNextScene(nullptr)
{
}


SceneManager::~SceneManager()
{
	SAFE_DELETE(m_pCurScene);
	SAFE_DELETE(m_pNextScene);
}

Scene * SceneManager::GetScene() const
{
	return m_pCurScene;
}

Matrix SceneManager::GetViewMaterix() const
{
	return m_pCurScene->GetViewMatrix();
}

Matrix SceneManager::GetProjMatrix() const
{
	return m_pCurScene->GetProjMatrix();
}

Vector3 SceneManager::GetMainCameraPos() const
{
	return m_pCurScene->GetMainCameraPos();
}

void SceneManager::SetNextScene(Scene * _pScene)
{
	m_pNextScene = _pScene;
}

bool SceneManager::Init()
{
	m_pCurScene = CreateSceneEmpty();

	// 현재씬에만 마우스 충돌하게끔 설정해준다.
	GET_SINGLETON(InputManager)->ChangeMouseScene(m_pCurScene);
	return true;
}

int SceneManager::Input(float _fTime)
{
	m_pCurScene->Input(_fTime);

	return ChangeScene();
}

int SceneManager::Update(float _fTime)
{
	m_pCurScene->Update(_fTime);
	return ChangeScene();
}

int SceneManager::LateUpdate(float _fTime)
{
	m_pCurScene->LateUpdate(_fTime);
	return ChangeScene();
}

int SceneManager::Collision(float _fTime)
{
	m_pCurScene->Collision(_fTime);
	return ChangeScene();
}

int SceneManager::Render(float _fTime)
{
	m_pCurScene->Render(_fTime);
	return ChangeScene();
}

int SceneManager::ChangeScene()
{
	if (nullptr != m_pNextScene)
	{
		SAFE_DELETE(m_pCurScene);
		m_pCurScene = m_pNextScene;
		m_pNextScene = nullptr;

		GET_SINGLETON(InputManager)->ChangeMouseScene(m_pCurScene);
		return SC_NEXT;
	}

	return SC_NONE;
}

Scene * SceneManager::CreateSceneEmpty()
{
	Scene* pScene = new Scene;

	if (false == pScene->Init())
	{
		SAFE_DELETE(pScene);
		return nullptr;
	}

	return pScene;
}
