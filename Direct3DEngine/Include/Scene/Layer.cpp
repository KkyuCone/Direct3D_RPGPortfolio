#include "Layer.h"
#include "Scene.h"
#include "..\GameObject.h"
#include "..\Render\RenderManager.h"

ENGINE_USING

// 실제로 오브젝트가 배치되는 곳은 레이어임
// 포토샵의 레이어처럼 생각하면됨
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
		//	// 부모가 존재한다면 부모 오브젝트에서 함수들을 업데이트 해줄거임
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
			// 부모가 존재한다면 부모 오브젝트에서 함수들을 업데이트 해줄거임
			// UI의 경우는 제외
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
			// 부모가 존재한다면 부모 오브젝트에서 함수들을 업데이트 해줄거임
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
			// 부모가 존재한다면 부모 오브젝트에서 함수들을 업데이트 해줄거임
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

	// 렌더하기 전에 라이트 컴포넌트를 들고 있는 애들은 조명효과를 줘야함 ㅇㅅㅇ
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
	//_GameObject->Start();			// 배치되니까 Start()함수 호출
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
