#pragma once
#include "Reference.h"

ENGINE_BEGIN

class ENGINE_DLL GameObject : public Reference
{
private:
	GameObject();
	GameObject(const GameObject& _Obj);
	~GameObject();

private:
	// 프로토타입 관리방법은 현재 우리 프레임워크에선
	// 씬에 프로토타입을 만들어서 그걸 복사해서 (Clon생성) 생성해서 사용한다.  ( 원본객체를 복사해서 사용, 메모리에 미리 넣어두고 그걸 빼서 복사함 ) 
	// 프로토타입을 저장하기 위해 map을 사용했는데 이건 탐색에 유리하기떄문에 사용한것임 ㅇㅅㅇ
	// 복사객체를 생성하는함수 -> CreateClone()
	static std::unordered_map<class Scene*, std::unordered_map<std::string, GameObject*>>	m_PrototypeMap;		// 프로토타입

public:
	bool m_bUpdateInstancing;					// 인스턴싱 그대로 넣을지 뺄지 여부 결정

public:
	void SetUpdateInstancing(bool _Enable);
	bool GetUpdateInstancing();

public:
	static GameObject* CreateObject(const std::string& _NameTag = "", class Layer* _pLayer = nullptr);
	static GameObject* CreatePrototype(const std::string& _NameTag = "", class Scene* _pScene = nullptr);
	static GameObject* CreateClone(const std::string& _NameTag, class Scene* _pScene, class Layer* _pLayer);

	// 씬이 지워질때나 아니면 해당씬에서 해당 프로토타입만 지우고싶을때 .. 경우를 2가지 나눠서 지운다.
	static void RemoveProtoType(class Scene* _pScene);
	static void RemoveProtoType(class Scene* _pScene, const std::string& _strProtoType);

private:
	static GameObject* FindPrototype(const std::string& _NameTag, class Scene* _pScene = nullptr);

private:
	class Scene* m_pScene;						// 해당 오브젝트가 어떤씬에 있는지 알고있음
	class Layer* m_pLayer;						// 해당 오브젝트가 어떤씬의 레이어에 있는지 알고있음
	class Transform* m_pTransform;				// 게임오브젝트에 트랜스폼은 필수다.
	RENDER_GROUP	m_eRenderGroup;				//  렌더그룹 별로 렌더링 해줄려고 추가함.( 파티클, UI 같은 경우는 조명을 받을 필요가 없어서 렌더그룹을 따로 설정한 후에 렌더매니저에서 돌려줄거임)
	float			m_fLifeTime;
	bool			m_bInstancing;				// 인스턴싱 여부
	bool			m_bFrustum;					// 프러스텀 컬링 당할지 여부

public:
	class Scene* GetScene()	const;
	class Layer* GetLayer()	const;
	class Transform* GetTransform() const;
	RENDER_GROUP	GetRenderGroup() const;
	float			GetLifeTime()	const;
	bool			 IsIntancing() const;
	bool			IsFrustumCulling() const;

	void SetScene(class Scene* _pScene);
	void SetLayer(class Layer* _pLayer);
	void SetRenderGroup(RENDER_GROUP _RenderGroup);
	void SetLifeTime(float _fTime);
	void SetInstancingEnable(bool _bEnable);
	void SetFrustumCulling(bool _bCulling);

private:
	std::list<class Component*>	m_ComponentList;		// 해당 오브젝트는 자신이 갖고있는 모든 컴포넌트들을 알고있다.
	std::list<class Collider*>	m_ColliderList;			// 콜라이더 리스트들
	std::list<class Component*>	m_StartList;			// Start()를 호출이필요한 컴포넌트들의 모음, 현재는 Add시 모든 컴포넌트들을 가지게 하고 있지만 추후에 내가 필요한애들만 따로 분리해도됨

public:
	const std::list<class Collider*>* GetColliderList() const;		// Get Collider List

private:
	// 부모, 자식 구조 추가
	bool m_bParent;
	GameObject* m_pParent;
	std::list<GameObject*> m_pChildList;					// 자식은 여러개가 될 수있게

public:
	int GetChildCount();
	bool ParentEnable() const;

public:
	void SetParent(GameObject* _Object);
	void SetParent(std::string _Tag);				// 태그로 추가(같은 레이어야함)

	void DeleteParent();

	void AddChild(GameObject* _Object);
	void AddChild(std::string _Tag);

	void DeleteChild(GameObject* _Object);
	void DeleteChild(std::string _Tag);

	GameObject* GetParent();
	std::list<GameObject*> GetChildren();
	GameObject* FindChild(std::string _strName);

private:
	// 무기같은 소켓의 경우 (애니메이션 존재 X 하지만 부모오브젝트의 본행렬이 필요한 경우
	bool m_bSoket;
	std::string m_SoketBoneName;
	int m_SoketBoneNum;

public:
	void SetBoneSoket(std::string _strBoneName);
	void DeleteBoneSoket();

	bool GetSoketEnable();
	int GetSoketBoneNum();

public:
	void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출
	bool Init();
	int Input(float _fTime);
	int Update(float _fTime);
	int LateUpdate(float _fTime);
	int Collision(float _fTime);
	int PrevRender(float _fTime);
	int PrevShadowRender(Matrix _matLightView, Matrix _matLightProj, float _fTime);
	int Render(float _fTime);
	int RenderShadow(class Shader* _pShader, Matrix _matLightView, Matrix _matLightProj, float _fTime);
	GameObject* Clone()	const;

public:
	// 해당 컴포넌트를 갖고있는지의 여부
	bool CheckComponentFromTag(const std::string& _strTag);
	bool CheckComponentFromType(COMPONENT_TYPE _eType);

public:
	// 컴포넌트 삭제
	void EraseComponentFromTag(const std::string& _strTag);
	void EraseComponentFromType(COMPONENT_TYPE _eType);
	void EraseComponent(class Component* _pComponent);

public:
	// 컴포넌트 추가
	Component* AddComponent(class Component* _pComponent);

	template <typename T>
	T* AddComponent(const std::string& _strTag = "")
	{
		T* pComponent = new T;

		pComponent->SetTag(_strTag.c_str());
		pComponent->SetScene(m_pScene);
		pComponent->SetLayer(m_pLayer);
		pComponent->SetGameObject(this);
		pComponent->m_pTransform = m_pTransform;

		if (false == pComponent->Init())
		{
			SAFE_RELEASE(pComponent);
			return nullptr;
		}

		return (T*)AddComponent(pComponent);
	}

public:
	// 컴포넌트 - 태그로 찾기
	template <typename T>
	T* FindComponentFromTag(const std::string& _strTag)
	{
		std::list<Component*>::iterator StartIter = m_ComponentList.begin();
		std::list<Component*>::iterator EndIter = m_ComponentList.end();

		for (; StartIter != EndIter; ++StartIter)
		{
			if ((*StartIter)->GetTag() == _strTag)
			{
				(*StartIter)->AddReference();
				return (T*)*StartIter;
			}
		}
		return nullptr;
	}

	// 컴포넌트 - 컴포넌트타입으로 찾기
	template <typename T>
	T* FindComponentFromType(COMPONENT_TYPE _eType)
	{
		std::list<Component*>::iterator StartIter = m_ComponentList.begin();
		std::list<Component*>::iterator EndIter = m_ComponentList.end();

		for (; StartIter != EndIter; ++StartIter)
		{
			if ((*StartIter)->GetComponentType() == _eType)
			{
				(*StartIter)->AddReference();
				return (T*)*StartIter;
			}
		}

		return nullptr;
	}
};

ENGINE_END

