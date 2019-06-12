#include "PathManager.h"

ENGINE_BEGIN

DEFINITION_SINGLETON(PathManager)

PathManager::PathManager()
{
}


PathManager::~PathManager()
{
	std::unordered_map<std::string, TCHAR*>::iterator StartIter = m_PathMap.begin();
	std::unordered_map<std::string, TCHAR*>::iterator EndIter = m_PathMap.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		SAFE_DELETE_ARRAY(StartIter->second);
	}

	m_PathMap.clear();
}

//

bool PathManager::Init()
{
	// 동적할당해주기 -> 경로를 만들때 멀티바이트/유니코드인지.. 필요할때마다 쓰기위해서
	TCHAR* pPath = new TCHAR[MAX_PATH];

	memset(pPath, 0, sizeof(TCHAR) * MAX_PATH);

	GetModuleFileName(nullptr, pPath, MAX_PATH);			// 경로 전부 읽어오기

	size_t iLength = lstrlen(pPath) - 1;

	for (size_t i = iLength; i >= 0; --i)
	{
		// 역슬래쉬는 두개 슬래쉬는 하나가 필요함
		if (pPath[i] == '\\' || pPath[i] == '/')
		{
			// 맨 끝에 있는 실행파일 경로까지 읽어오기때문에
			// 필요없는 실행파일경로만 지우려고 만듬
			// 뒤에서부터 검사해서 처음부분에 역슬래쉬 또는 슬래쉬가 있는 부분의 경로까지만 가져오게 함
			memset(&pPath[i + 1], 0, sizeof(TCHAR) * (iLength - i));
			break;
		}

	}

	// 기본 경로 설정 (ROOT 경로)
	m_PathMap.insert(std::make_pair(PATH_ROOT, pPath));

	// 셰이더 경로 만들기 ( 즉 바이너리폴더가 기본경로고 그 안에 있는 폴더인 셰이더 폴더 경로를 만들어준다.)
	CreatePath(PATH_SHADER, TEXT("Shader\\"));

	// 텍스쳐 경로
	CreatePath(PATH_TEXTURE, TEXT("Texture\\"));
	CreatePath(PATH_LANDSCAPE, TEXT("Texture\\LandScape\\"));

	// UI 경로
	CreatePath(PATH_UI, TEXT("Texture\\UI\\"));
	CreatePath(PATH_UI_IMAGE, TEXT("Texture\\UI\\Image\\"));
	CreatePath(PATH_UI_BUTTON, TEXT("Texture\\UI\\Button\\"));
	CreatePath(PATH_UI_BAR, TEXT("Texture\\UI\\Bar\\"));
	CreatePath(PATH_UI_GAGEBAR, TEXT("Texture\\UI\\GageBar\\"));
	CreatePath(PATH_UI_SKILL, TEXT("Texture\\UI\\Skill\\"));
	CreatePath(PATH_UI_SKILL_ICON, TEXT("Texture\\UI\\Skill\\Icon\\"));
	CreatePath(PATH_UI_SKILL_TERA_ICON, TEXT("Texture\\UI\\Skill\\Icon_Tera\\"));
	CreatePath(PATH_UI_QUEST, TEXT("Texture\\UI\\Quest\\"));
	CreatePath(PATH_UI_NUMBER, TEXT("Texture\\UI\\Number\\"));
	CreatePath(PATH_UI_MENU, TEXT("Texture\\UI\\Menu\\"));
	CreatePath(PATH_UI_INVENTORY, TEXT("Texture\\UI\\Inventory\\"));
	CreatePath(PATH_UI_SCROLLBAR, TEXT("Texture\\UI\\ScrollBar\\"));
	CreatePath(PATH_UI_QUICKSLOT, TEXT("Texture\\UI\\QuickSlot\\"));

	// 메쉬 경로
	CreatePath(PATH_MESH, TEXT("Mesh\\")); 
	CreatePath(PATH_MESH_HOUSE, TEXT("Mesh\\House\\"));
	CreatePath(PATH_MESH_HUMAN, TEXT("Mesh\\Human\\"));
	CreatePath(PATH_MESH_MONSTER, TEXT("Mesh\\Monster\\"));
	CreatePath(PATH_MESH_ROCK, TEXT("Mesh\\Rock\\"));
	CreatePath(PATH_MESH_TREE, TEXT("Mesh\\Tree\\"));
	CreatePath(PATH_MESH_WALL, TEXT("Mesh\\Wall\\"));
	CreatePath(PATH_MESH_WEAPON, TEXT("Mesh\\Weapon\\"));

	// 데이터 경로
	CreatePath(PATH_DATA, TEXT("Data\\"));
	CreatePath(PATH_EXCEL, TEXT("Data\\Excel\\"));

	// 테스트용 하나 더 추가
	CreatePath(PATH_TEST_TEXTURE, TEXT("TestTexture\\"));

	// 사운드 경로
	CreatePath(PATH_SOUND, TEXT("Sound\\"));

	// 강우 관련 경로
	CreatePath(PATH_RAIN, TEXT("Texture\\Rain\\"));

	return true;
}

bool PathManager::CreatePath(const std::string& _strKey, const TCHAR* _pAddPath,
	const std::string& _strBaseKey /*= PATH_ROOT*/)
{
	//이미 있는 경로는 만들필요가없음
	if (FindPath(_strKey))
	{
		return false;
	}

	TCHAR* pPath = new TCHAR[MAX_PATH];
	memset(pPath, 0, sizeof(TCHAR) * MAX_PATH);

	// BaseKey의 디폴트값은 기본 패스다. (ROOT 패스)
	const TCHAR* pBasePath = FindPath(_strBaseKey);

	if (nullptr != pBasePath)
	{
		lstrcat(pPath, pBasePath);
	}

	lstrcat(pPath, _pAddPath);

	m_PathMap.insert(std::make_pair(_strKey, pPath));

	return true;
}

const TCHAR* PathManager::FindPath(const std::string& _strKey)
{
	std::unordered_map<std::string, TCHAR*>::iterator FindPathIter = m_PathMap.find(_strKey);

	if (FindPathIter == m_PathMap.end())
	{
		return nullptr;
	}

	return FindPathIter->second;
}

const char* PathManager::FindPathMultibyte(const std::string& _strKey)
{
	std::unordered_map<std::string, TCHAR*>::iterator FindPathIter = m_PathMap.find(_strKey);

	if (FindPathIter == m_PathMap.end())
	{
		return nullptr;
	}

	//유니코드일때랑 멀티바이트일때랑 생각하기
#ifdef UNICODE
	memset(m_strChange, 0, MAX_PATH);

	// 와이드바이트 -> 멀티바이트로 전환
	WideCharToMultiByte(CP_UTF8, 0, FindPathIter->second, -1,
		m_strChange, lstrlen(FindPathIter->second), nullptr, nullptr);

	return m_strChange;
#else
	return FindPathIter->second;
#endif // UNICODE

}

ENGINE_END
