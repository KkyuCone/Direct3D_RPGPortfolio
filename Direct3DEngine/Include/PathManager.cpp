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
	// �����Ҵ����ֱ� -> ��θ� ���鶧 ��Ƽ����Ʈ/�����ڵ�����.. �ʿ��Ҷ����� �������ؼ�
	TCHAR* pPath = new TCHAR[MAX_PATH];

	memset(pPath, 0, sizeof(TCHAR) * MAX_PATH);

	GetModuleFileName(nullptr, pPath, MAX_PATH);			// ��� ���� �о����

	size_t iLength = lstrlen(pPath) - 1;

	for (size_t i = iLength; i >= 0; --i)
	{
		// ���������� �ΰ� �������� �ϳ��� �ʿ���
		if (pPath[i] == '\\' || pPath[i] == '/')
		{
			// �� ���� �ִ� �������� ��α��� �о���⶧����
			// �ʿ���� �������ϰ�θ� ������� ����
			// �ڿ������� �˻��ؼ� ó���κп� �������� �Ǵ� �������� �ִ� �κ��� ��α����� �������� ��
			memset(&pPath[i + 1], 0, sizeof(TCHAR) * (iLength - i));
			break;
		}

	}

	// �⺻ ��� ���� (ROOT ���)
	m_PathMap.insert(std::make_pair(PATH_ROOT, pPath));

	// ���̴� ��� ����� ( �� ���̳ʸ������� �⺻��ΰ� �� �ȿ� �ִ� ������ ���̴� ���� ��θ� ������ش�.)
	CreatePath(PATH_SHADER, TEXT("Shader\\"));

	// �ؽ��� ���
	CreatePath(PATH_TEXTURE, TEXT("Texture\\"));
	CreatePath(PATH_LANDSCAPE, TEXT("Texture\\LandScape\\"));

	// UI ���
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

	// �޽� ���
	CreatePath(PATH_MESH, TEXT("Mesh\\")); 
	CreatePath(PATH_MESH_HOUSE, TEXT("Mesh\\House\\"));
	CreatePath(PATH_MESH_HUMAN, TEXT("Mesh\\Human\\"));
	CreatePath(PATH_MESH_MONSTER, TEXT("Mesh\\Monster\\"));
	CreatePath(PATH_MESH_ROCK, TEXT("Mesh\\Rock\\"));
	CreatePath(PATH_MESH_TREE, TEXT("Mesh\\Tree\\"));
	CreatePath(PATH_MESH_WALL, TEXT("Mesh\\Wall\\"));
	CreatePath(PATH_MESH_WEAPON, TEXT("Mesh\\Weapon\\"));

	// ������ ���
	CreatePath(PATH_DATA, TEXT("Data\\"));
	CreatePath(PATH_EXCEL, TEXT("Data\\Excel\\"));

	// �׽�Ʈ�� �ϳ� �� �߰�
	CreatePath(PATH_TEST_TEXTURE, TEXT("TestTexture\\"));

	// ���� ���
	CreatePath(PATH_SOUND, TEXT("Sound\\"));

	// ���� ���� ���
	CreatePath(PATH_RAIN, TEXT("Texture\\Rain\\"));

	return true;
}

bool PathManager::CreatePath(const std::string& _strKey, const TCHAR* _pAddPath,
	const std::string& _strBaseKey /*= PATH_ROOT*/)
{
	//�̹� �ִ� ��δ� �����ʿ䰡����
	if (FindPath(_strKey))
	{
		return false;
	}

	TCHAR* pPath = new TCHAR[MAX_PATH];
	memset(pPath, 0, sizeof(TCHAR) * MAX_PATH);

	// BaseKey�� ����Ʈ���� �⺻ �н���. (ROOT �н�)
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

	//�����ڵ��϶��� ��Ƽ����Ʈ�϶��� �����ϱ�
#ifdef UNICODE
	memset(m_strChange, 0, MAX_PATH);

	// ���̵����Ʈ -> ��Ƽ����Ʈ�� ��ȯ
	WideCharToMultiByte(CP_UTF8, 0, FindPathIter->second, -1,
		m_strChange, lstrlen(FindPathIter->second), nullptr, nullptr);

	return m_strChange;
#else
	return FindPathIter->second;
#endif // UNICODE

}

ENGINE_END
