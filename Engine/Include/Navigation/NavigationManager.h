#pragma once
#include "..\Engine.h"

ENGINE_BEGIN

// 네비게이션 매니저는
// 네비메쉬들을 관리해주는 관리자 역할을 한다.
// 각 씬별로 여러개의 네비메쉬가 존재할 수 있다고 가정해서 아래와 같이 만들어졌지만
// 보통 씬당 한개의 네비메쉬를 갖는다.

// 그리고 싱글톤으로 만들어졌다. 
// 코어쪽에서 생성및 제거를해주고, 
// 지형을 생성할때 CreateNavMesh함수를 호출해서 네비메쉬를 생성하고 여기 관리자에서 갖고있게한다.
class ENGINE_DLL NavigationManager
{
private:
	std::unordered_map<class Scene*, std::unordered_map<std::string, class NavigationMesh*>>	m_mapNavMesh;

public:
	bool Init();

public:
	class NavigationMesh* CreateNavMesh(class Scene* _pScene, const std::string& _strName);
	class NavigationMesh* CreateNavMesh(class Scene* _pScene, const std::string& _strName, 
		const char* _pFileName, const std::string& _strPathName = PATH_DATA);
	
	class NavigationMesh* FindNavMesh(class Scene* _pScene, const std::string& _strName);
	class NavigationMesh* FindNavMesh(class Scene* _pScene, const Vector3& _vPos);

	bool EraseNavMesh(class Scene* _pScene, const std::string& _strName);
	bool EraseNavMesh(class Scene* _pScene, const Vector3& _vPos);
	bool EraseNavMesh(class Scene* _pScene);

public:
	int MouseNavSectionCount(class Scene* _pScene, RayInfo _RayInfo);
	Vector3 MouseNavPosition(class Scene* _pSCene, RayInfo _RayInfo);

	DECLARE_SINGLETON(NavigationManager)
};

ENGINE_END
