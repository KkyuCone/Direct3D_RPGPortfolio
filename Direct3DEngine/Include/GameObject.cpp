#include "GameObject.h"
#include "Component\Component.h"
#include "Component\Transform.h"
#include "Scene\Layer.h"
#include "Scene\SceneManager.h"
#include "Scene\Scene.h"
#include "CollisionManager.h"
#include "Render\Shader.h"
#include "Component\Animation.h"

ENGINE_USING

std::unordered_map<class Scene*, std::unordered_map<std::string, GameObject*>> GameObject::m_PrototypeMap;

GameObject::GameObject() : 
	m_pScene(nullptr), 
	m_pLayer(nullptr), 
	m_pTransform(nullptr),
	m_eRenderGroup(RG_DEFAULT),
	m_fLifeTime(-1.0f),
	m_bInstancing(false),
	m_pParent(nullptr),
	m_bParent(false),
	m_bSoket(false),
	m_SoketBoneNum(-1)
{
	// 모든 게임오브젝트들은 Transform 컴포넌트를 필수적으로 갖고있다.
	// 이 Transform이 행렬이 변환하는지에 따라 계산하거나 계산하지 않거나로 따진다. -> m_bStatic 변수 여부
	m_pTransform = AddComponent<Transform>("Transform");
	m_SoketBoneName = "";
}

GameObject::GameObject(const GameObject & _Obj)
{
	// 복사생성자
	*this = _Obj;	
	m_bInstancing = false;
	m_iReferenceCount = 1;				// 다른 객체를 복사했으니 카운터를 1로 바꿔줘야한다.
	m_pScene = nullptr;
	m_pLayer = nullptr;
	m_eRenderGroup = _Obj.m_eRenderGroup;
	// 이미 위에서 참조할 애를 넣어줬다.  
	// 근데 복사용도니까 컴포넌트 리스트를 지워준다. (혹시모르니깡 ->레퍼런스카운터떔에) 
	// 그리고 컴포넌트를 그대로 복사해서 넣어준다.
	m_ComponentList.clear();
	m_ColliderList.clear();
	m_StartList.clear();

	std::list<Component*>::const_iterator StartIter = _Obj.m_ComponentList.begin();
	std::list<Component*>::const_iterator EndIter = _Obj.m_ComponentList.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		Component*	pComponent = (*StartIter)->Clone();

		if (CT_TRANSFORM == pComponent->GetComponentType())
		{
			// Transform일 경우에는 컴포넌트의 레퍼런스 카운터를 증가시킨다.
			pComponent->AddReference();
			m_pTransform = (Transform*)pComponent;
		}
		else if (CT_COLLIDER == pComponent->GetComponentType())
		{
			// 콜라이더의 경우 콜라이더 리스트에 따로 넣어준다.
			m_ColliderList.push_back((Collider*)pComponent);
		}

		pComponent->SetGameObject(this);
		pComponent->m_pTransform = m_pTransform;
		pComponent->AddReference();

		m_ComponentList.push_back(pComponent);
		m_StartList.push_back(pComponent);
	}
	
}


GameObject::~GameObject()
{
	SAFE_RELEASE(m_pTransform);
	//Safe_Release_VectorList(m_ColliderList);
	Safe_Release_VectorList(m_ComponentList);
	// m_StartList
	Safe_Release_VectorList(m_StartList);

}

// static함수
GameObject* GameObject::CreateObject(const std::string & _NameTag, Layer * _pLayer)
{
	GameObject* pObject = new GameObject;

	// 게임오브젝트를 만들고 Init()함수를 호출하기
	if (false == pObject->Init())
	{
		SAFE_RELEASE(pObject);
		return nullptr;
	}

	pObject->SetTag(_NameTag.c_str());

	if (nullptr != _pLayer)
	{
		_pLayer->AddGameObject(pObject);			// 레이어에 생성한 오브젝트를 추가한다.
	}

	return pObject;
}

GameObject * GameObject::CreatePrototype(const std::string & _NameTag, Scene * _pScene)
{
	GameObject* pObject = new GameObject;

	if (false == pObject->Init())
	{
		SAFE_RELEASE(pObject);
		return nullptr;
	}

	pObject->SetTag(_NameTag.c_str());

	// 씬이없다면 현재씬을 기준으로한다.
	if (nullptr == _pScene)
	{
		_pScene = GET_SINGLETON(SceneManager)->GetScene();
	}

	// 해당 씬이 프로토타입을 갖고있는지 체크
	std::unordered_map<Scene*, std::unordered_map<std::string, GameObject*>>::iterator FIter = m_PrototypeMap.find(_pScene);

	// 프로토타입을 갖고 있는 씬이 없다면 해당씬을 추가하기(그 씬에 처음으로 프로토타입을 만들때)
	if (FIter == m_PrototypeMap.end())
	{
		std::unordered_map<std::string, GameObject*> map;
		m_PrototypeMap.insert(std::make_pair(_pScene, map));
		FIter = m_PrototypeMap.find(_pScene);
	}

	pObject->m_pScene = _pScene;
	pObject->AddReference();
	FIter->second.insert(std::make_pair(_NameTag, pObject));

	return pObject;
}

// 프로토타입에서 복사해서 생성하는 클론함수
GameObject * GameObject::CreateClone(const std::string & _NameTag, Scene * _pScene, Layer * _pLayer)
{
	GameObject* pProtoType = FindPrototype(_NameTag, _pScene);

	if (nullptr == pProtoType)
	{
		return nullptr;
	}

	GameObject* pCloneObject = pProtoType->Clone();

	// 레이어가 있으면 -> 배치한다. 실질적으로 해다 ㅇ레이에어 오브젝트를 생성해야 만들어진거임(눈에보이게찌)
	if (nullptr != _pLayer)
	{
		_pLayer->AddGameObject(pCloneObject);
	}

	return pCloneObject;
}

void GameObject::RemoveProtoType(Scene * _pScene)
{
	// 해당씬이 지워질때  -> 해당씬의 모든 프로토타입 지우기
	// 해당씬 찾기
	std::unordered_map<Scene*, std::unordered_map<std::string, GameObject*>>::iterator FIter = m_PrototypeMap.find(_pScene);

	// 해당씬이 존재하지 않을 경우
	if (FIter == m_PrototypeMap.end())
	{
		return;
	}

	// 해당씬의 프로토타입 지우기
	std::unordered_map<std::string, GameObject*>::iterator StartIter = FIter->second.begin();
	std::unordered_map<std::string, GameObject*>::iterator EndIter = FIter->second.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		SAFE_RELEASE(StartIter->second);
	}

	FIter->second.clear();				// 내용 지우고
	m_PrototypeMap.erase(FIter);		// 프로토타입 관리하는 map에서 지우기
}

void GameObject::RemoveProtoType(Scene* _pScene, const std::string & _strProtoType)
{
	// 해당씬의 내가 원하는 프로토타입 지우기
	std::unordered_map<Scene*, std::unordered_map<std::string, GameObject*>>::iterator FIter = m_PrototypeMap.find(_pScene);

	// 해당씬 자체가 없으면 return
	if (FIter == m_PrototypeMap.end())
	{
		return;
	}

	std::unordered_map<std::string, GameObject*>::iterator FProtoType = FIter->second.find(_strProtoType);

	// 해당 프로토타입이 없으면
	if (FProtoType == FIter->second.end())
	{
		return;
	}

	// 있으면 지우기
	SAFE_RELEASE(FProtoType->second);
	FIter->second.erase(FProtoType);
	
}

GameObject * GameObject::FindPrototype(const std::string & _NameTag, Scene * _pScene)
{
	// 해당씬이 프로토타입을 갖고있는지 찾기
	std::unordered_map<Scene*, std::unordered_map<std::string, GameObject*>>::iterator FIter = m_PrototypeMap.find(_pScene);

	if (FIter == m_PrototypeMap.end())
	{
		return nullptr;
	}

	std::unordered_map<std::string, GameObject*>::iterator FPrototypeIter = FIter->second.find(_NameTag);

	if (FPrototypeIter == FIter->second.end())
	{
		return nullptr;
	}

	return FPrototypeIter->second;
}

//

Scene * GameObject::GetScene() const
{
	return m_pScene;
}

Layer * GameObject::GetLayer() const
{
	return m_pLayer;
}

Transform * GameObject::GetTransform() const
{
	m_pTransform->AddReference();
	return m_pTransform;
}

RENDER_GROUP GameObject::GetRenderGroup() const
{
	return m_eRenderGroup;
}

float GameObject::GetLifeTime() const
{
	return m_fLifeTime;
}

bool GameObject::IsIntancing() const
{
	return m_bInstancing;
}

bool GameObject::IsFrustumCulling() const
{
	return m_bFrustum;
}

void GameObject::SetScene(Scene * _pScene)
{
	// 현재 게임오브젝트의 씬을 설정하고
	m_pScene = _pScene;

	// 현재 게임오브젝트의 모든 컴포넌트에도 현재씬을 알려준다.
	std::list<Component*>::iterator StartIter = m_ComponentList.begin();
	std::list<Component*>::iterator EndIter = m_ComponentList.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		(*StartIter)->m_pScene = _pScene;
	}
}

void GameObject::SetLayer(Layer * _pLayer)
{
	// 현재 게임오브젝트의 레이어를 셋팅하고
	m_pLayer = _pLayer;

	// 현재 게임오브젝트의 모든 컴포넌트에 해당 게임오브젝트의 레이어가 어디에 속해있는지도 알려준다.
	std::list<Component*>::iterator StartIter = m_ComponentList.begin();
	std::list<Component*>::iterator EndIter = m_ComponentList.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		(*StartIter)->m_pLayer = _pLayer;
	}
}

void GameObject::SetRenderGroup(RENDER_GROUP _RenderGroup)
{
	m_eRenderGroup = _RenderGroup;
}

void GameObject::SetLifeTime(float _fTime)
{
	m_fLifeTime = _fTime;
}

void GameObject::SetInstancingEnable(bool _bEnable)
{
	m_bInstancing = _bEnable;
}

void GameObject::SetFrustumCulling(bool _bCulling)
{
	m_bFrustum = _bCulling;
}

const std::list<class Collider*>* GameObject::GetColliderList() const
{
	return &m_ColliderList;
}

void GameObject::SetParent(GameObject * _Object)
{
	if (nullptr == _Object)
		return;

	// 이미 부모가 있는 경우 처리
	if (true == m_bParent)
	{
		m_pParent->DeleteChild(this);
	}

	// 자기 자신은 부모가 될 수 없다.
	std::string ParaObjTag = _Object->m_strTag;
	if (ParaObjTag == m_strTag)
	{
		assert(false);
		return;
	}


	// 같은 레이어의 경우에만 부모, 자식관계 성립하게 하기
	Layer* pParentLayer = _Object->m_pLayer;
	if (pParentLayer != m_pLayer)
	{
		return;
	}

	m_pParent = _Object;
	m_bParent = true;

	_Object->AddChild(this);
}

void GameObject::SetParent(std::string _Tag)
{
	// 자기 자신은 부모가 될 수 없다.
	if (_Tag == m_strTag)
	{
		assert(false);
		return;
	}

	GameObject* pFObject = m_pLayer->FindGameObject(_Tag);

	if (nullptr == pFObject)
	{
		assert(false);
		return;
	}
	m_pParent = pFObject;
	m_bParent = true;

	pFObject->AddChild(this);
}

void GameObject::DeleteParent()
{
	if (nullptr == m_pParent)
		return;

	m_pParent = nullptr;
	m_bParent = false;
}

void GameObject::AddChild(GameObject * _Object)
{
	// 자기 자신은 자식이 될 수 없다.
	if (_Object == this)
		return;

	// 레이어에서도 얘가 있는지를 판단한다. 
	// 그리고 중복되지 않게 한번 더 검사한다.
	std::string strName = _Object->GetTag();
	GameObject* pAddCildObject = m_pLayer->FindGameObject(strName);

	if (nullptr == _Object)
		return;
	else if (nullptr == pAddCildObject)
		return;

	m_pChildList.push_back(pAddCildObject);

	if (false == pAddCildObject->ParentEnable())
	{
		pAddCildObject->SetParent(this);
	}
}

void GameObject::AddChild(std::string _Tag)
{
	GameObject* pAddCildObject = m_pLayer->FindGameObject(_Tag);

	// 자기 자신은 자식이 될 수 없다.
	if (nullptr == pAddCildObject || (pAddCildObject == this))
		return;

	m_pChildList.push_back(pAddCildObject);

	if (false == pAddCildObject->ParentEnable())
	{
		pAddCildObject->SetParent(this);
	}
}

void GameObject::DeleteChild(GameObject * _Object)
{
	//m_pChildList
	if (nullptr == _Object)
		return;

	std::string DeleteTag = _Object->m_strTag;

	std::list<GameObject*>::iterator StartIter = m_pChildList.begin();
	std::list<GameObject*>::iterator EndIter = m_pChildList.end();

	for (; StartIter != EndIter;)
	{
		std::string IterObjTag = (*StartIter)->m_strTag;

		if (DeleteTag == IterObjTag)
		{
			m_pChildList.erase(StartIter);

			if (true == _Object->ParentEnable())
			{
				_Object->DeleteParent();
			}
			return;
		}
		else
		{
			++StartIter;
		}
	}
}

void GameObject::DeleteChild(std::string _Tag)
{
	std::string DeleteTag = _Tag;

	std::list<GameObject*>::iterator StartIter = m_pChildList.begin();
	std::list<GameObject*>::iterator EndIter = m_pChildList.end();

	for (; StartIter != EndIter;)
	{
		std::string IterObjTag = (*StartIter)->m_strTag;

		if (DeleteTag == IterObjTag)
		{
			if (true == (*StartIter)->ParentEnable())
			{
				(*StartIter)->DeleteParent();
			}

			m_pChildList.erase(StartIter);
			return;
		}
		else
		{
			++StartIter;
		}
	}
}

GameObject * GameObject::GetParent()
{
	if (nullptr != m_pParent)
	{
		int a = 0;
	}

	return m_pParent;
}

std::list<GameObject*> GameObject::GetChildren()
{
	return m_pChildList;
}

GameObject * GameObject::FindChild(std::string _strName)
{
	std::list<GameObject*>::iterator StartIter = m_pChildList.begin();
	std::list<GameObject*>::iterator EndIter = m_pChildList.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		if (_strName == (*StartIter)->GetTag())
		{
			return (*StartIter);
		}
	}

	return nullptr;
}

int GameObject::GetChildCount()
{
	return (int)(m_pChildList.size());
}

bool GameObject::ParentEnable() const
{
	return m_bParent;
}

void GameObject::SetBoneSoket(std::string _strBoneName)
{
	if (false == m_bParent)
		return;

	// 해당 부모 본 찾기
	Animation* pParentAni = m_pParent->FindComponentFromType<Animation>(CT_ANIMATION);

	if (nullptr == pParentAni || NULL == pParentAni->FindBone(_strBoneName))
	{
		SAFE_RELEASE(pParentAni);
		return;
	}

	m_SoketBoneName = _strBoneName;
	m_SoketBoneNum = pParentAni->FindBoneIndex(_strBoneName);
	m_bSoket = true;

	SAFE_RELEASE(pParentAni);
}

void GameObject::DeleteBoneSoket()
{
	m_SoketBoneName = "";
	m_SoketBoneNum = -1;
	m_bSoket = false;
}

bool GameObject::GetSoketEnable()
{
	return m_bSoket;
}

int GameObject::GetSoketBoneNum()
{
	return m_SoketBoneNum;
}

void GameObject::Start()
{
	if (true == m_StartList.empty())
	{
		return;
	}

	// Start는 월드상 배치될때 호출해주는 것이다. ㅇㅅㅇ 
	// 레이어를 알고있을때가 배치된것임
	std::list<Component*>::iterator StartIter = m_StartList.begin();
	std::list<Component*>::iterator EndIter = m_StartList.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		(*StartIter)->Start();
	}

	Safe_Release_VectorList(m_StartList);
	//GET_SINGLE(CCollisionManager)->AddCollider(this);
}

bool GameObject::Init()
{
	return true;
}

int GameObject::Input(float _fTime)
{
	std::list<Component*>::iterator StartIter = m_ComponentList.begin();
	std::list<Component*>::iterator EndIter = m_ComponentList.end();

	for (; StartIter != EndIter;)
	{
		if (false == (*StartIter)->IsActive())
		{
			SAFE_RELEASE((*StartIter));
			StartIter = m_ComponentList.erase(StartIter);
			continue;
		}
		else if (false == (*StartIter)->IsEnable())
		{
			++StartIter;
			continue;
		}

		(*StartIter)->Input(_fTime);
		++StartIter;
	}

	// 자식 있을 경우 후에 함수 호출
	std::list<GameObject*>::iterator StartChildIter = m_pChildList.begin();
	std::list<GameObject*>::iterator EndChildIter = m_pChildList.end();

	for (; StartChildIter != EndChildIter; ++StartChildIter)
	{
		(*StartChildIter)->Input(_fTime);
	}

	return 0;
}

int GameObject::Update(float _fTime)
{
	Start();

	m_bInstancing = false;

	// 게임오브젝트 라이프타임 추가
	if (m_fLifeTime > 0)
	{
		m_fLifeTime -= _fTime;

		if (m_fLifeTime <= 0.0f)
		{
			Active(false);
			return 0;
		}
	}

	std::list<Component*>::iterator StartIter = m_ComponentList.begin();
	std::list<Component*>::iterator EndIter = m_ComponentList.end();

	for (; StartIter != EndIter;)
	{
		if (false == (*StartIter)->IsActive())
		{
			SAFE_RELEASE((*StartIter));
			StartIter = m_ComponentList.erase(StartIter);
			continue;
		}
		else if (false == (*StartIter)->IsEnable())
		{
			++StartIter;
			continue;
		}

		// 트랜스폼이 행렬 계산이 필요하면 하기 -> Transfrom이 업데이트가 되는지 안되는지 체크
		if (m_pTransform == *StartIter &&  true == m_pTransform->GetIsUpdate())
		{
			(*StartIter)->Update(_fTime);
		}
		else
		{
			// 트랜스폼이 아닌 다른컴포넌트
			(*StartIter)->Update(_fTime);
		}
		++StartIter;
	}

	// 매번 클라이언트에서 LookAt함수 호출하기 귀찮아서 ㅇㅅㅇ 여기서 호출함
	m_pTransform->RotationLookAt();

	// 자식 있을 경우 후에 함수 호출
	std::list<GameObject*>::iterator StartChildIter = m_pChildList.begin();
	std::list<GameObject*>::iterator EndChildIter = m_pChildList.end();

	for (; StartChildIter != EndChildIter; ++StartChildIter)
	{
		(*StartChildIter)->Update(_fTime);
	}

	return 0;
}

int GameObject::LateUpdate(float _fTime)
{
	Start();

	std::list<Component*>::iterator StartIter = m_ComponentList.begin();
	std::list<Component*>::iterator EndIter = m_ComponentList.end();

	for (; StartIter != EndIter;)
	{
		if (false == (*StartIter)->IsActive())
		{
			SAFE_RELEASE((*StartIter));
			StartIter = m_ComponentList.erase(StartIter);
			continue;
		}
		else if (false == (*StartIter)->IsEnable())
		{
			++StartIter;
			continue;
		}

		// 트랜스폼일 경우 m_bUpdate가 true일경우 
		if (m_pTransform == *StartIter && true == m_pTransform->GetIsUpdate())
		{
			(*StartIter)->LateUpdate(_fTime);
		}
		else
		{
			(*StartIter)->LateUpdate(_fTime);
		}
		++StartIter;
	}

	// 자식 있을 경우 후에 함수 호출
	std::list<GameObject*>::iterator StartChildIter = m_pChildList.begin();
	std::list<GameObject*>::iterator EndChildIter = m_pChildList.end();

	for (; StartChildIter != EndChildIter; ++StartChildIter)
	{
		(*StartChildIter)->LateUpdate(_fTime);
	}

	return 0;
}

int GameObject::Collision(float _fTime)
{
	Start();

	std::list<Component*>::iterator StartIter = m_ComponentList.begin();
	std::list<Component*>::iterator EndIter = m_ComponentList.end();

	for (; StartIter != EndIter;)
	{
		if (false == (*StartIter)->IsActive())
		{
			SAFE_RELEASE((*StartIter));
			StartIter = m_ComponentList.erase(StartIter);
			continue;
		}
		else if (false == (*StartIter)->IsEnable())
		{
			++StartIter;
			continue;
		}

		(*StartIter)->Collision(_fTime);
		++StartIter;
	}

	// 자식 있을 경우 후에 함수 호출
	std::list<GameObject*>::iterator StartChildIter = m_pChildList.begin();
	std::list<GameObject*>::iterator EndChildIter = m_pChildList.end();

	for (; StartChildIter != EndChildIter; ++StartChildIter)
	{
		(*StartChildIter)->Collision(_fTime);
	}

	return 0;
}

int GameObject::PrevRender(float _fTime)
{
	Start();

	std::list<Component*>::iterator StartIter = m_ComponentList.begin();
	std::list<Component*>::iterator EndIter = m_ComponentList.end();

	// PrevRender() 호출 -> 렌더직전에 PrevRender()함수를 호출한다.
	for (; StartIter != EndIter;)
	{
		if (false == (*StartIter)->IsActive())
		{
			SAFE_RELEASE((*StartIter));
			StartIter = m_ComponentList.erase(StartIter);
			continue;
		}
		else if (false == (*StartIter)->IsEnable())
		{
			++StartIter;
			continue;
		}

		if (CT_COLLIDER == (*StartIter)->GetComponentType())
		{
			// 콜라이더의 경우 프리브렌더도해주면 안됨 
			// CollisionManager에서 충돌체들을 모아서 따로 렌더를 해줄거임
			++StartIter;
			continue;
		}

		(*StartIter)->PrevRender(_fTime);			// 렌더함수 호출 직전에 호출
		++StartIter;
	}



	return 0;
}

int GameObject::PrevShadowRender(Matrix _matLightView, Matrix _matLightProj, float _fTime)
{
	Start();

	std::list<Component*>::iterator StartIter = m_ComponentList.begin();
	std::list<Component*>::iterator EndIter = m_ComponentList.end();

	// PrevRender() 호출 -> 렌더직전에 PrevRender()함수를 호출한다.
	for (; StartIter != EndIter;)
	{
		if (false == (*StartIter)->IsActive())
		{
			SAFE_RELEASE((*StartIter));
			StartIter = m_ComponentList.erase(StartIter);
			continue;
		}
		else if (false == (*StartIter)->IsEnable())
		{
			++StartIter;
			continue;
		}

		//if (CT_TRANSFORM != (*StartIter)->GetComponentType())
		//{
		//	// 콜라이더의 경우 프리브렌더도해주면 안됨 
		//	// CollisionManager에서 충돌체들을 모아서 따로 렌더를 해줄거임
		//	++StartIter;
		//	continue;
		//}
		(*StartIter)->PrevShadowRender(_matLightView, _matLightProj, _fTime);			// 렌더함수 호출 직전에 호출
		++StartIter;
	}

	return 0;
}

int GameObject::Render(float _fTime)
{
	if (true == m_bInstancing)
		return 0;

	PrevRender(_fTime);

	std::list<Component*>::iterator StartIter = m_ComponentList.begin();
	std::list<Component*>::iterator EndIter = m_ComponentList.end();


	// Render () 호출
	for (; StartIter != EndIter;)
	{
		if (false == (*StartIter)->IsActive())
		{
			SAFE_RELEASE((*StartIter));
			StartIter = m_ComponentList.erase(StartIter);
			continue;
		}
		else if (false == (*StartIter)->IsEnable())
		{
			++StartIter;
			continue;
		}
		
		if (CT_COLLIDER == (*StartIter)->GetComponentType())
		{
			// 콜라이더의 경우 렌더해주면 안됨
			++StartIter;
			continue;
		}

		(*StartIter)->Render(_fTime);
		++StartIter;
	}

	return 0;
}

int GameObject::RenderShadow(class Shader* _pShader ,Matrix _matLightView, Matrix _matLightProj, float _fTime)
{
	if (true == m_bInstancing)
		return 0;

	PrevShadowRender(_matLightView, _matLightProj, _fTime);

	std::list<Component*>::iterator StartIter = m_ComponentList.begin();
	std::list<Component*>::iterator EndIter = m_ComponentList.end();


	// Render () 호출
	for (; StartIter != EndIter;)
	{
		if (false == (*StartIter)->IsActive())
		{
			SAFE_RELEASE((*StartIter));
			StartIter = m_ComponentList.erase(StartIter);
			continue;
		}
		else if (false == (*StartIter)->IsEnable())
		{
			++StartIter;
			continue;
		}

		if (CT_RENDERER != (*StartIter)->GetComponentType())
		{
			++StartIter;
			continue;
		}

		(*StartIter)->RenderShadow(_pShader, _fTime);
		++StartIter;
	}

	return 0;
}

GameObject * GameObject::Clone() const
{
	// 객체 복사생성자 호출
	return new GameObject(*this);
}

bool GameObject::CheckComponentFromTag(const std::string & _strTag)
{
	std::list<Component*>::iterator StartIter = m_ComponentList.begin();
	std::list<Component*>::iterator EndIter = m_ComponentList.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		if ((*StartIter)->GetTag() == _strTag)
		{
			//(*StartIter)->AddReference();
			return true;
		}
	}
	return false;
}

bool GameObject::CheckComponentFromType(COMPONENT_TYPE _eType)
{
	std::list<Component*>::iterator StartIter = m_ComponentList.begin();
	std::list<Component*>::iterator EndIter = m_ComponentList.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		if ((*StartIter)->GetComponentType() == _eType)
		{
			//(*StartIter)->AddReference();
			return true;
		}
	}
	return false;
}

void GameObject::EraseComponentFromTag(const std::string & _strTag)
{
	std::list<Component*>::iterator StartIter = m_ComponentList.begin();
	std::list<Component*>::iterator EndIter = m_ComponentList.end();

	for (; StartIter != EndIter ; ++StartIter)
	{
		// 해당 컴포넌트의 이름을 가진게 있으면 삭제
		if ((*StartIter)->GetTag() == _strTag)
		{
			SAFE_RELEASE((*StartIter));
			m_ComponentList.erase(StartIter);
			return;
		}
	}
}

void GameObject::EraseComponentFromType(COMPONENT_TYPE _eType)
{
	std::list<Component*>::iterator StartIter = m_ComponentList.begin();
	std::list<Component*>::iterator EndIter = m_ComponentList.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		if ((*StartIter)->GetComponentType() == _eType)
		{
			SAFE_RELEASE((*StartIter));
			m_ComponentList.erase(StartIter);
			return;
		}
	}
}

void GameObject::EraseComponent(Component * _pComponent)
{
	std::list<Component*>::iterator StartIter = m_ComponentList.begin();
	std::list<Component*>::iterator EndIter = m_ComponentList.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		if ((*StartIter) == _pComponent)
		{
			SAFE_RELEASE((*StartIter));
			m_ComponentList.erase(StartIter);
			return;
		}
	}
}

Component * GameObject::AddComponent(Component * _pComponent)
{
	_pComponent->SetScene(m_pScene);
	_pComponent->SetLayer(m_pLayer);
	_pComponent->SetGameObject(this);
	_pComponent->m_pTransform = m_pTransform;
	_pComponent->AddReference();
	_pComponent->AddReference();			// m_StartList에도 넣어주려고 추가

	// 콜라이더의 경우 따로 콜라이더 리스트에도 넣어둔다.
	if (CT_COLLIDER == _pComponent->GetComponentType())
	{
		m_ColliderList.push_back((Collider*)_pComponent);
	}

	m_StartList.push_back(_pComponent);	
	m_ComponentList.push_back(_pComponent);

	return _pComponent;
}
