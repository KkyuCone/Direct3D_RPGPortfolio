#include "Layer.h"
#include "Scene.h"
#include "..\GameObject.h"
#include "..\Render\RenderManager.h"

ENGINE_USING

// ������ ������Ʈ�� ��ġ�Ǵ� ���� ���̾���
// ���伥�� ���̾�ó�� �����ϸ��
Layer::Layer() : m_iOrder(0)
{
}


Layer::~Layer()
{
	Safe_Release_VectorList(m_StartList);
	Safe_Release_VectorList(m_GameObjectList);
}

//

int Layer::GetOrder() const
{
	return m_iOrder;
}

void Layer::SetOrder(int _Order)
{
	m_iOrder = _Order;
	m_pScene->SortLayer();
}

void Layer::Start()
{
	if (true == m_StartList.empty())
	{
		return;
	}

	std::list<GameObject*>::iterator StartIter = m_StartList.begin();
	std::list<GameObject*>::iterator EndIter = m_StartList.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		if (true == (*StartIter)->ParentEnable())
		{
			continue;
		}
		(*StartIter)->Start();
	}

	Safe_Release_VectorList(m_StartList);
}


void Layer::SetScene(Scene* _pScene)
{
	m_pScene = _pScene;
}

Scene* Layer::GetScene()
{
	return m_pScene;
}

//

bool Layer::Init()
{
	return true;
}

int Layer::Input(float _fTime)
{
	std::list<GameObject*>::iterator	StartIter	= m_GameObjectList.begin();
	std::list<GameObject*>::iterator	EndIter = m_GameObjectList.end();

	for (; StartIter != EndIter;)
	{
		if (false == (*StartIter)->IsActive())
		{
			SAFE_RELEASE((*StartIter));
			StartIter = m_GameObjectList.erase(StartIter);
			continue;
		}
		else if (false == (*StartIter)->IsEnable())
		{
			++StartIter;
			continue;
		}
		//else if (true == (*StartIter)->ParentEnable())
		//{
		//	// �θ� �����Ѵٸ� �θ� ������Ʈ���� �Լ����� ������Ʈ ���ٰ���
		//	++StartIter;
		//	continue;
		//}

		(*StartIter)->Input(_fTime);
		++StartIter;
	}

	return 0;
}

int Layer::Update(float _fTime)
{
	Start();

	std::list<GameObject*>::iterator	StartIter = m_GameObjectList.begin();
	std::list<GameObject*>::iterator	EndIter = m_GameObjectList.end();

	for (; StartIter != EndIter;)
	{
		if (false == (*StartIter)->IsActive())
		{
			SAFE_RELEASE((*StartIter));
			StartIter = m_GameObjectList.erase(StartIter);
			continue;
		}
		else if (false == (*StartIter)->IsEnable())
		{
			++StartIter;
			continue;
		}
		else if (true == (*StartIter)->ParentEnable())
		{
			// �θ� �����Ѵٸ� �θ� ������Ʈ���� �Լ����� ������Ʈ ���ٰ���
			// UI�� ���� ����
			if ((*StartIter)->GetLayer()->GetTag() == "UI")
			{
				(*StartIter)->Update(_fTime);
			}
			++StartIter;
			continue;
		}

		(*StartIter)->Update(_fTime);
		++StartIter;
	}
	return 0;
}

int Layer::LateUpdate(float _fTime)
{
	Start();

	std::list<GameObject*>::iterator	StartIter = m_GameObjectList.begin();
	std::list<GameObject*>::iterator	EndIter = m_GameObjectList.end();

	for (; StartIter != EndIter;)
	{
		if (false == (*StartIter)->IsActive())
		{
			SAFE_RELEASE((*StartIter));
			StartIter = m_GameObjectList.erase(StartIter);
			continue;
		}
		else if (false == (*StartIter)->IsEnable())
		{
			++StartIter;
			continue;
		}
		else if (true == (*StartIter)->ParentEnable())
		{
			// �θ� �����Ѵٸ� �θ� ������Ʈ���� �Լ����� ������Ʈ ���ٰ���
			++StartIter;
			continue;
		}

		(*StartIter)->LateUpdate(_fTime);
		++StartIter;
	}
	return 0;
}

int Layer::Collision(float _fTime)
{
	Start();

	std::list<GameObject*>::iterator	StartIter = m_GameObjectList.begin();
	std::list<GameObject*>::iterator	EndIter = m_GameObjectList.end();

	for (; StartIter != EndIter;)
	{
		if (false == (*StartIter)->IsActive())
		{
			SAFE_RELEASE((*StartIter));
			StartIter = m_GameObjectList.erase(StartIter);
			continue;
		}
		else if (false == (*StartIter)->IsEnable())
		{
			++StartIter;
			continue;
		}
		else if (true == (*StartIter)->ParentEnable())
		{
			// �θ� �����Ѵٸ� �θ� ������Ʈ���� �Լ����� ������Ʈ ���ٰ���
			++StartIter;
			continue;
		}

		(*StartIter)->Collision(_fTime);
		++StartIter;
	}
	return 0;
}

int Layer::Render(float _fTime)
{
	Start();

	std::list<GameObject*>::iterator	StartIter = m_GameObjectList.begin();
	std::list<GameObject*>::iterator	EndIter = m_GameObjectList.end();

	// �����ϱ� ���� ����Ʈ ������Ʈ�� ��� �ִ� �ֵ��� ����ȿ���� ����� ������
	for (; StartIter != EndIter;)
	{
		if (false == (*StartIter)->IsActive())
		{
			SAFE_RELEASE((*StartIter));
			StartIter = m_GameObjectList.erase(StartIter);
			continue;
		}
		else if (false == (*StartIter)->IsEnable())
		{
			++StartIter;
			continue;
		}

		GET_SINGLETON(RenderManager)->AddRenderObject(*StartIter);
		++StartIter;
	}
	return 0;
}

void Layer::AddGameObject(GameObject * _GameObject)
{
	_GameObject->SetScene(m_pScene);
	_GameObject->SetLayer(this);
	_GameObject->AddReference();
	_GameObject->AddReference();
	//_GameObject->Start();			// ��ġ�Ǵϱ� Start()�Լ� ȣ��
	m_StartList.push_back(_GameObject);
	m_GameObjectList.push_back(_GameObject);
}

GameObject * Layer::FindGameObject(std::string _Tag)
{
	std::list<GameObject*>::iterator	StartIter = m_GameObjectList.begin();
	std::list<GameObject*>::iterator	EndIter = m_GameObjectList.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		std::string ObjectTag = (*StartIter)->GetTag();

		if (_Tag == ObjectTag)
		{
			return (*StartIter);
		}
	}
	return nullptr;
}

void Layer::DeleteGameObject(GameObject * _Object)
{
	if (nullptr == _Object)
	{
		assert(false);
		return;
	}

	std::string strName = _Object->GetTag();
	DeleteGameObject(strName);
}

void Layer::DeleteGameObject(std::string _Tag)
{
	if ("" == _Tag)
	{
		assert(false);
		return;
	}

	GameObject* pFObject = nullptr;

	std::list<GameObject*>::iterator	StartIter = m_GameObjectList.begin();
	std::list<GameObject*>::iterator	EndIter = m_GameObjectList.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		std::string ObjectTag = (*StartIter)->GetTag();

		if (_Tag == ObjectTag)
		{
			pFObject = (*StartIter);
			m_GameObjectList.erase(StartIter);
			SAFE_RELEASE(pFObject);
		}
	}
}
