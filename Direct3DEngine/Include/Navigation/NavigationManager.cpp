#include "NavigationManager.h"
#include "NavigationMesh.h"
#include "../Scene/Scene.h"

ENGINE_USING

DEFINITION_SINGLETON(NavigationManager)

NavigationManager::NavigationManager()
{
}


NavigationManager::~NavigationManager()
{
	std::unordered_map<class Scene*, std::unordered_map<std::string, class NavigationMesh*>>::iterator StartIter = m_mapNavMesh.begin();
	std::unordered_map<class Scene*, std::unordered_map<std::string, class NavigationMesh*>>::iterator EndIter = m_mapNavMesh.end();

	for (; StartIter != EndIter; ++StartIter)
	{

		Safe_Release_Map(StartIter->second);
		StartIter->second.clear();
	}
	m_mapNavMesh.clear();
}

bool NavigationManager::Init()
{
	return true;
}

// ��, �̸�
NavigationMesh * NavigationManager::CreateNavMesh(Scene * _pScene, 
	const std::string & _strName)
{
	// �ϴ� �׺�޽��� �ִ� ���� �ִ��� ã��(�ߺ����� �ȳ־��ٷ��� ������)
	std::unordered_map<class Scene*, std::unordered_map<std::string, class NavigationMesh*>>::iterator FindIter = m_mapNavMesh.find(_pScene);

	std::unordered_map<std::string, class NavigationMesh*>* pMap;

	// ������(�׺�޽��� �ִ� ����..) ���ٸ� �־��ֱ�
	if (FindIter == m_mapNavMesh.end())
	{
		std::unordered_map<std::string, class NavigationMesh*> map;
		m_mapNavMesh.insert(std::make_pair(_pScene, map));

		pMap = &m_mapNavMesh.begin()->second;
	}
	else
	{
		// ������ �����ϴϱ�
		// �̹� �ٸ� �׺�޽���..�ϴ� ������ �װ� �����ͼ� �߰��� �־��ָ�Ǵϱ� �����´�.
		pMap = &FindIter->second;
	}

	NavigationMesh* pMesh = new NavigationMesh;
	pMesh->SetTag(_strName);
	pMesh->Init();

	pMap->insert(std::make_pair(_strName, pMesh));


	return pMesh;			// ���� ������ �׺�޽� ��ȯ, LandScape���� ���� �־��ش�.(���� ������)
}

NavigationMesh * NavigationManager::CreateNavMesh(Scene * _pScene, 
	const std::string & _strName,
	const char * _pFileName,
	const std::string & _strPathName /*= PATH_DATA*/)
{
	std::unordered_map<class Scene*, std::unordered_map<std::string, class NavigationMesh*>>::iterator FindIter = m_mapNavMesh.find(_pScene);

	std::unordered_map<std::string, class NavigationMesh*>* pMap;

	if (FindIter == m_mapNavMesh.end())
	{
		// ������ ��� ���� �߰�
		std::unordered_map<std::string, class NavigationMesh*> map;
		m_mapNavMesh.insert(std::make_pair(_pScene, map));

		pMap = &m_mapNavMesh.begin()->second;
	}
	else
	{
		pMap = &FindIter->second;
	}

	NavigationMesh* pMesh = new NavigationMesh;
	pMesh->SetTag(_strName);
	pMesh->Init();
	pMesh->Load(_pFileName, _strPathName);

	pMap->insert(std::make_pair(_strName, pMesh));			//������ map�� �־��ֱ�

	return pMesh;
}

NavigationMesh * NavigationManager::FindNavMesh(Scene * _pScene, const std::string & _strName)
{
	std::unordered_map<class Scene*, std::unordered_map<std::string, class NavigationMesh*>>::iterator FindIter;
	FindIter = m_mapNavMesh.find(_pScene);

	if (FindIter == m_mapNavMesh.end())
	{
		return nullptr;
	}

	std::unordered_map<std::string, class NavigationMesh*>::iterator StartIter = FindIter->second.find(_strName);

	if (StartIter == FindIter->second.end())
	{
		return nullptr;
	}

	StartIter->second->AddReference();

	return StartIter->second;
}

NavigationMesh * NavigationManager::FindNavMesh(Scene * _pScene, const Vector3 & _vPos)
{
	std::unordered_map<class Scene*, std::unordered_map<std::string, class NavigationMesh*>>::iterator FindIter = m_mapNavMesh.find(_pScene);

	if (FindIter == m_mapNavMesh.end())
	{
		return nullptr;
	}

	std::unordered_map<std::string, class NavigationMesh*>::iterator StartIter = FindIter->second.begin();
	std::unordered_map<std::string, class NavigationMesh*>::iterator EndIter = FindIter->second.end();

	for ( ; StartIter != EndIter ; ++StartIter)
	{
		NavigationMesh* pNavMesh = StartIter->second;

		Vector3	vMin = pNavMesh->GetMin();
		Vector3	vMax = pNavMesh->GetMax();

		if (vMin.x <= _vPos.x && _vPos.x <= vMax.x &&
			vMin.y <= _vPos.y && _vPos.y <= vMax.y &&
			vMin.z <= _vPos.z && _vPos.z <= vMax.z)
		{
			pNavMesh->AddReference();
			return pNavMesh;
		}
	}

	return nullptr;
}

bool NavigationManager::EraseNavMesh(Scene * _pScene, const std::string & _strName)
{
	std::unordered_map<class Scene*, std::unordered_map<std::string, class NavigationMesh*>>::iterator FindIter = m_mapNavMesh.find(_pScene);

	if (FindIter == m_mapNavMesh.end())
	{
		return false;
	}

	std::unordered_map<std::string, class NavigationMesh*>::iterator StartIter = FindIter->second.find(_strName);

	if (StartIter == FindIter->second.end())
	{
		return false;
	}

	SAFE_RELEASE(StartIter->second);
	FindIter->second.erase(StartIter);

	return true;

}

bool NavigationManager::EraseNavMesh(Scene * _pScene, const Vector3 & _vPos)
{
	std::unordered_map<class Scene*, std::unordered_map<std::string, class NavigationMesh*>>::iterator FindIter = m_mapNavMesh.find(_pScene);

	if (FindIter == m_mapNavMesh.end())
	{
		return false;
	}

	std::unordered_map<std::string, class NavigationMesh*>::iterator StartIter = FindIter->second.begin();
	std::unordered_map<std::string, class NavigationMesh*>::iterator EndIter = FindIter->second.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		NavigationMesh* pNavMesh = StartIter->second;

		Vector3	vMin = pNavMesh->GetMin();
		Vector3	vMax = pNavMesh->GetMax();

		if (vMin.x <= _vPos.x && _vPos.x <= vMax.x &&
			vMin.y <= _vPos.y && _vPos.y <= vMax.y &&
			vMin.z <= _vPos.z && _vPos.z <= vMax.z)
		{
			SAFE_RELEASE(StartIter->second);
			FindIter->second.erase(StartIter);
			return true;
		}
	}

	return false;
}

bool NavigationManager::EraseNavMesh(Scene * _pScene)
{
	std::unordered_map<class Scene*, std::unordered_map<std::string, class NavigationMesh*>>::iterator FindIter = m_mapNavMesh.find(_pScene);

	if (FindIter == m_mapNavMesh.end())
	{
		return false;
	}

	Safe_Release_Map(FindIter->second);
	m_mapNavMesh.erase(FindIter);

	return true;
}

// -_- �ƴϾ� ���� ������ ���� �Ѱ��� ��������!
int NavigationManager::MouseNavSectionCount(class Scene* _pScene, RayInfo _RayInfo)
{
	std::unordered_map<class Scene*, std::unordered_map<std::string, class NavigationMesh*>>::iterator FindIter = m_mapNavMesh.find(_pScene);

	if (FindIter == m_mapNavMesh.end())
	{
		return 0;
	}
	
	NavigationMesh* pNavMesh = FindIter->second.begin()->second;

	int SectionColCount = pNavMesh->MouseSectionColCount(_RayInfo);

	return SectionColCount;
}

Vector3 NavigationManager::MouseNavPosition(Scene * _pScene, RayInfo _RayInfo)
{
	std::unordered_map<class Scene*, std::unordered_map<std::string, class NavigationMesh*>>::iterator FindIter = m_mapNavMesh.find(_pScene);

	if (FindIter == m_mapNavMesh.end())
	{
		return 0;
	}

	NavigationMesh* pNavMesh = FindIter->second.begin()->second;
	Vector3 RetrunPos = pNavMesh->MouseNavCol(_RayInfo);
	pNavMesh->GetY(RetrunPos);
	return RetrunPos;
}
