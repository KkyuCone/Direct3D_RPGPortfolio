#pragma once
#include "..\Reference.h"

ENGINE_BEGIN

class ENGINE_DLL Component : public Reference
{
private:
	friend class GameObject;

protected:
	Component();
	Component(const Component& _Component);
	virtual ~Component() = 0;

protected:
	class Scene*	m_pScene;
	class Layer*	m_pLayer;
	class GameObject*	m_pGameObject;
	class Transform*	m_pTransform;

public:
	class Scene*	GetScene()	const;
	class Layer*	GetLayer()	const;
	class GameObject*	GetGameObject() const;
	class Transform*	GetTransform()	const;			// SetTransform은 필요없은 생성할때 생성자에서 만들어줄거임ㅇ ㅅㅇ


public:
	void SetScene(class Scene* _pScene);
	void SetLayer(class Layer* _pLayer);
	void SetGameObject(class GameObject* _pGameObject);
	void SetTransform(Transform* _Transform);

	bool IsObjectEnable()	const;
	bool IsObjectActive()	const;


protected:
	COMPONENT_TYPE m_eComponentType;

public:
	COMPONENT_TYPE GetComponentType() const;

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출, 즉 레이어에서 호출해준다.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// 렌더를 하기 직전에 호출해주는 함수ㅔㄱㅁ
	virtual int PrevShadowRender(Matrix _matLightView, Matrix _matLightProj, float _fTime);
	virtual int Render(float _fTime);
	virtual int RenderShadow(class Shader* _pShader, float _fTime);
	virtual Component* Clone()	const;

public:
	// 컴포넌트 지우기
	void EraseComponentFromTag(const std::string& _strTag);
	void EraseComponentFromType(COMPONENT_TYPE _eType);
	void EraseComponent(class Component* _pComponent);

public:
	template <typename T>
	T* FindComponentFromTag(const std::string& _strTag)
	{
		return m_pGameObject->FindComponentFromTag<T>(_strTag);
	}

	template <typename T>
	T* FindComponentFromType(COMPONENT_TYPE _eType)
	{
		return m_pGameObject->FindComponentFromType<T>(_eType);
	}

	template <typename T>
	T* AddComponent(const std::string& _strTag)
	{
		return m_pGameObject->AddComponent<T>(_strTag);
	}
};

ENGINE_END