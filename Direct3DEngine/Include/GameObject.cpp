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
	// ��� ���ӿ�����Ʈ���� Transform ������Ʈ�� �ʼ������� �����ִ�.
	// �� Transform�� ����� ��ȯ�ϴ����� ���� ����ϰų� ������� �ʰų��� ������. -> m_bStatic ���� ����
	m_pTransform = AddComponent<Transform>("Transform");
	m_SoketBoneName = "";
}

GameObject::GameObject(const GameObject & _Obj)
{
	// ���������
	*this = _Obj;	
	m_bInstancing = false;
	m_iReferenceCount = 1;				// �ٸ� ��ü�� ���������� ī���͸� 1�� �ٲ�����Ѵ�.
	m_pScene = nullptr;
	m_pLayer = nullptr;
	m_eRenderGroup = _Obj.m_eRenderGroup;
	// �̹� ������ ������ �ָ� �־����.  
	// �ٵ� ����뵵�ϱ� ������Ʈ ����Ʈ�� �����ش�. (Ȥ�ø𸣴ϱ� ->���۷���ī���͋���) 
	// �׸��� ������Ʈ�� �״�� �����ؼ� �־��ش�.
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
			// Transform�� ��쿡�� ������Ʈ�� ���۷��� ī���͸� ������Ų��.
			pComponent->AddReference();
			m_pTransform = (Transform*)pComponent;
		}
		else if (CT_COLLIDER == pComponent->GetComponentType())
		{
			// �ݶ��̴��� ��� �ݶ��̴� ����Ʈ�� ���� �־��ش�.
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

// static�Լ�
GameObject* GameObject::CreateObject(const std::string & _NameTag, Layer * _pLayer)
{
	GameObject* pObject = new GameObject;

	// ���ӿ�����Ʈ�� ����� Init()�Լ��� ȣ���ϱ�
	if (false == pObject->Init())
	{
		SAFE_RELEASE(pObject);
		return nullptr;
	}

	pObject->SetTag(_NameTag.c_str());

	if (nullptr != _pLayer)
	{
		_pLayer->AddGameObject(pObject);			// ���̾ ������ ������Ʈ�� �߰��Ѵ�.
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

	// ���̾��ٸ� ������� ���������Ѵ�.
	if (nullptr == _pScene)
	{
		_pScene = GET_SINGLETON(SceneManager)->GetScene();
	}

	// �ش� ���� ������Ÿ���� �����ִ��� üũ
	std::unordered_map<Scene*, std::unordered_map<std::string, GameObject*>>::iterator FIter = m_PrototypeMap.find(_pScene);

	// ������Ÿ���� ���� �ִ� ���� ���ٸ� �ش���� �߰��ϱ�(�� ���� ó������ ������Ÿ���� ���鶧)
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

// ������Ÿ�Կ��� �����ؼ� �����ϴ� Ŭ���Լ�
GameObject * GameObject::CreateClone(const std::string & _NameTag, Scene * _pScene, Layer * _pLayer)
{
	GameObject* pProtoType = FindPrototype(_NameTag, _pScene);

	if (nullptr == pProtoType)
	{
		return nullptr;
	}

	GameObject* pCloneObject = pProtoType->Clone();

	// ���̾ ������ -> ��ġ�Ѵ�. ���������� �ش� �����̿��� ������Ʈ�� �����ؾ� �����������(�������̰���)
	if (nullptr != _pLayer)
	{
		_pLayer->AddGameObject(pCloneObject);
	}

	return pCloneObject;
}

void GameObject::RemoveProtoType(Scene * _pScene)
{
	// �ش���� ��������  -> �ش���� ��� ������Ÿ�� �����
	// �ش�� ã��
	std::unordered_map<Scene*, std::unordered_map<std::string, GameObject*>>::iterator FIter = m_PrototypeMap.find(_pScene);

	// �ش���� �������� ���� ���
	if (FIter == m_PrototypeMap.end())
	{
		return;
	}

	// �ش���� ������Ÿ�� �����
	std::unordered_map<std::string, GameObject*>::iterator StartIter = FIter->second.begin();
	std::unordered_map<std::string, GameObject*>::iterator EndIter = FIter->second.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		SAFE_RELEASE(StartIter->second);
	}

	FIter->second.clear();				// ���� �����
	m_PrototypeMap.erase(FIter);		// ������Ÿ�� �����ϴ� map���� �����
}

void GameObject::RemoveProtoType(Scene* _pScene, const std::string & _strProtoType)
{
	// �ش���� ���� ���ϴ� ������Ÿ�� �����
	std::unordered_map<Scene*, std::unordered_map<std::string, GameObject*>>::iterator FIter = m_PrototypeMap.find(_pScene);

	// �ش�� ��ü�� ������ return
	if (FIter == m_PrototypeMap.end())
	{
		return;
	}

	std::unordered_map<std::string, GameObject*>::iterator FProtoType = FIter->second.find(_strProtoType);

	// �ش� ������Ÿ���� ������
	if (FProtoType == FIter->second.end())
	{
		return;
	}

	// ������ �����
	SAFE_RELEASE(FProtoType->second);
	FIter->second.erase(FProtoType);
	
}

GameObject * GameObject::FindPrototype(const std::string & _NameTag, Scene * _pScene)
{
	// �ش���� ������Ÿ���� �����ִ��� ã��
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
	// ���� ���ӿ�����Ʈ�� ���� �����ϰ�
	m_pScene = _pScene;

	// ���� ���ӿ�����Ʈ�� ��� ������Ʈ���� ������� �˷��ش�.
	std::list<Component*>::iterator StartIter = m_ComponentList.begin();
	std::list<Component*>::iterator EndIter = m_ComponentList.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		(*StartIter)->m_pScene = _pScene;
	}
}

void GameObject::SetLayer(Layer * _pLayer)
{
	// ���� ���ӿ�����Ʈ�� ���̾ �����ϰ�
	m_pLayer = _pLayer;

	// ���� ���ӿ�����Ʈ�� ��� ������Ʈ�� �ش� ���ӿ�����Ʈ�� ���̾ ��� �����ִ����� �˷��ش�.
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

	// �̹� �θ� �ִ� ��� ó��
	if (true == m_bParent)
	{
		m_pParent->DeleteChild(this);
	}

	// �ڱ� �ڽ��� �θ� �� �� ����.
	std::string ParaObjTag = _Object->m_strTag;
	if (ParaObjTag == m_strTag)
	{
		assert(false);
		return;
	}


	// ���� ���̾��� ��쿡�� �θ�, �ڽİ��� �����ϰ� �ϱ�
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
	// �ڱ� �ڽ��� �θ� �� �� ����.
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
	// �ڱ� �ڽ��� �ڽ��� �� �� ����.
	if (_Object == this)
		return;

	// ���̾���� �갡 �ִ����� �Ǵ��Ѵ�. 
	// �׸��� �ߺ����� �ʰ� �ѹ� �� �˻��Ѵ�.
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

	// �ڱ� �ڽ��� �ڽ��� �� �� ����.
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

	// �ش� �θ� �� ã��
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

	// Start�� ����� ��ġ�ɶ� ȣ�����ִ� ���̴�. ������ 
	// ���̾ �˰��������� ��ġ�Ȱ���
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

	// �ڽ� ���� ��� �Ŀ� �Լ� ȣ��
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

	// ���ӿ�����Ʈ ������Ÿ�� �߰�
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

		// Ʈ�������� ��� ����� �ʿ��ϸ� �ϱ� -> Transfrom�� ������Ʈ�� �Ǵ��� �ȵǴ��� üũ
		if (m_pTransform == *StartIter &&  true == m_pTransform->GetIsUpdate())
		{
			(*StartIter)->Update(_fTime);
		}
		else
		{
			// Ʈ�������� �ƴ� �ٸ�������Ʈ
			(*StartIter)->Update(_fTime);
		}
		++StartIter;
	}

	// �Ź� Ŭ���̾�Ʈ���� LookAt�Լ� ȣ���ϱ� �����Ƽ� ������ ���⼭ ȣ����
	m_pTransform->RotationLookAt();

	// �ڽ� ���� ��� �Ŀ� �Լ� ȣ��
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

		// Ʈ�������� ��� m_bUpdate�� true�ϰ�� 
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

	// �ڽ� ���� ��� �Ŀ� �Լ� ȣ��
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

	// �ڽ� ���� ��� �Ŀ� �Լ� ȣ��
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

	// PrevRender() ȣ�� -> ���������� PrevRender()�Լ��� ȣ���Ѵ�.
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
			// �ݶ��̴��� ��� �����귻�������ָ� �ȵ� 
			// CollisionManager���� �浹ü���� ��Ƽ� ���� ������ ���ٰ���
			++StartIter;
			continue;
		}

		(*StartIter)->PrevRender(_fTime);			// �����Լ� ȣ�� ������ ȣ��
		++StartIter;
	}



	return 0;
}

int GameObject::PrevShadowRender(Matrix _matLightView, Matrix _matLightProj, float _fTime)
{
	Start();

	std::list<Component*>::iterator StartIter = m_ComponentList.begin();
	std::list<Component*>::iterator EndIter = m_ComponentList.end();

	// PrevRender() ȣ�� -> ���������� PrevRender()�Լ��� ȣ���Ѵ�.
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
		//	// �ݶ��̴��� ��� �����귻�������ָ� �ȵ� 
		//	// CollisionManager���� �浹ü���� ��Ƽ� ���� ������ ���ٰ���
		//	++StartIter;
		//	continue;
		//}
		(*StartIter)->PrevShadowRender(_matLightView, _matLightProj, _fTime);			// �����Լ� ȣ�� ������ ȣ��
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


	// Render () ȣ��
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
			// �ݶ��̴��� ��� �������ָ� �ȵ�
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


	// Render () ȣ��
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
	// ��ü ��������� ȣ��
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
		// �ش� ������Ʈ�� �̸��� ������ ������ ����
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
	_pComponent->AddReference();			// m_StartList���� �־��ַ��� �߰�

	// �ݶ��̴��� ��� ���� �ݶ��̴� ����Ʈ���� �־�д�.
	if (CT_COLLIDER == _pComponent->GetComponentType())
	{
		m_ColliderList.push_back((Collider*)_pComponent);
	}

	m_StartList.push_back(_pComponent);	
	m_ComponentList.push_back(_pComponent);

	return _pComponent;
}
