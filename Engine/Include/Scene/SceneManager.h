#pragma once
#include "../Engine.h"

ENGINE_BEGIN

class ENGINE_DLL SceneManager
{
private:
	class Scene*	m_pCurScene;
	class Scene*	m_pNextScene;

public:
	class Scene* GetScene()	const;
	Matrix GetViewMaterix() const;
	Matrix GetProjMatrix() const;
	Vector3 GetMainCameraPos() const;

public:
	void SetNextScene(class Scene* _pScene);

public:
	bool Init();
	int Input(float _fTime);
	int Update(float _fTime);
	int LateUpdate(float _fTime);
	int Collision(float _fTime);
	int Render(float _fTime);

private:
	int ChangeScene();

public:
	template <typename T>
	Scene* CreateScene()
	{
		Scene* pScene = new Scene;

		if (false == pScene->Init())
		{
			SAFE_DELETE(pScene);
			return nullptr;
		}

		pScene->AddSceneComponent<T>();

		return pScene;
	}

	template <typename T>
	bool AddSceneComponent(bool _bCur = true)
	{
		if (true == _bCur)
		{
			return m_pCurScene->AddSceneComponent<T>();
		}

		return m_pNextScene->AddSceneComponent<T>();
	}

	class Scene* CreateSceneEmpty();

	DECLARE_SINGLETON(SceneManager)
};

ENGINE_END

