
// UNITYMAKER.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "UNITYMAKER.h"
#include <string>
#include <unordered_map>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class GamePath
{
public:
	static std::wstring RootPath;
	static std::wstring BinRootPath;
	static void Init() 
	{
		wchar_t Arr[1000];
		GetCurrentDirectoryW(sizeof(wchar_t) * 1000, Arr);
		RootPath = Arr;
		BinRootPath = Arr;

		size_t CutCount = RootPath.find_last_of('\\', RootPath.size());
		RootPath.replace(CutCount, RootPath.size(), L"\\");
		BinRootPath.replace(CutCount, RootPath.size(), L"\\BIN\\");
	}

};

std::wstring GamePath::RootPath;
std::wstring GamePath::BinRootPath;

// CUNITYMAKERApp

BEGIN_MESSAGE_MAP(CUNITYMAKERApp, CWinApp)
END_MESSAGE_MAP()

CUNITYMAKERApp::CUNITYMAKERApp()
{
	// TODO: 아래 응용 프로그램 ID 문자열을 고유 ID 문자열로 바꾸십시오(권장).
	// 문자열에 대한 서식: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("UNITYMAKER.AppID.NoVersion"));

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

// 유일한 CUNITYMAKERApp 개체입니다.

CUNITYMAKERApp theApp;


// CUNITYMAKERApp 초기화

BOOL CUNITYMAKERApp::InitInstance()
{
	CWinApp::InitInstance();


	EnableTaskbarInteraction(FALSE);

	// RichEdit 컨트롤을 사용하려면  AfxInitRichEdit2()가 있어야 합니다.	
	// AfxInitRichEdit2();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));

	// 시간대별로 
	// 유니티 빌드를 만드는데 한가지

	GamePath::Init();

	//if (1 >= __argc)
	//{
		CreateUnityCpp(GamePath::RootPath.c_str(), L"Direct3DEngine/include/", L"Direct3DEngineUnity.cpp");
		CreateUnityCpp(GamePath::RootPath.c_str(), L"Client/include/", L"ClientUnity.cpp");
		CreateUnityCpp(GamePath::RootPath.c_str(), L"Editor/include/", L"EditorUnity.cpp");

	//}

	return TRUE;
}

int CUNITYMAKERApp::ExitInstance()
{
	//TODO: 추가한 추가 리소스를 처리합니다.
	return CWinApp::ExitInstance();
}

// CUNITYMAKERApp 메시지 처리기
void CUNITYMAKERApp::CreateUnityCpp(const wchar_t* _RootPath, const wchar_t* _FolderName, const wchar_t* _FileName)
{
	//CFileFind FileFind;
	CString RootPath = _RootPath;
	CString FolderName = _FolderName;

	CString SavePath = GamePath::BinRootPath.c_str();
	SavePath += L"Unity\\";
	SavePath += _FileName;

	PushCPPlist(RootPath, FolderName, SavePath);

	SaveCPPlist(SavePath);

}

void CUNITYMAKERApp::SaveCPPlist(CString _SavePath)
{
	FILE* P = nullptr;
	_wfopen_s(&P, _SavePath.GetString(), L"wt");

	listbegin = CPPlist.begin();
	listend = CPPlist.end();

	for (; listbegin != listend; listbegin++)
	{
		fwrite((*listbegin).c_str(), (*listbegin).size(), 1, P);
	}

	CPPlist.clear();

	if (P != nullptr)
	{
		fclose(P);
	}
}

void CUNITYMAKERApp::PushCPPlist(CString _RootPath, CString _FolderName, CString _SavePath)
{

	CFileFind FileFind;

	BOOL bFile = FileFind.FindFile(_RootPath + _FolderName + L"*.*");

	// 폴더 -> 내부에 cpp
	// 유니티 cpp -> cpp를 모두다 모아놓은 파일
	// 폴더의 수정시간 > 유니티 cpp 의 수정시간보다 크면
	// 그때만 cpp를 만든다.

	_WIN32_FILE_ATTRIBUTE_DATA FolderAttData;
	_WIN32_FILE_ATTRIBUTE_DATA FileAttData;
	LARGE_INTEGER FolderTime;
	LARGE_INTEGER FileTime;

	GetFileAttributesEx((_RootPath + _FolderName).GetString(), GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &FolderAttData);
	GetFileAttributesEx(_SavePath.GetString(), GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &FileAttData);
	memcpy_s(&FolderTime, sizeof(LARGE_INTEGER), &(FolderAttData.ftLastWriteTime), sizeof(LARGE_INTEGER));
	memcpy_s(&FileTime, sizeof(LARGE_INTEGER), &(FileAttData.ftLastWriteTime), sizeof(LARGE_INTEGER));

	//if (FolderTime.QuadPart < FileTime.QuadPart)
	//{
	//	return;
	//}

	while (bFile)
	{
		bFile = FileFind.FindNextFileW();

		// "."
		// ".."

		if (FileFind.IsDots())
		{
			continue;
		}
		else if (FileFind.IsDirectory())
		{
			CString NewFolderName = _FolderName;
			NewFolderName = NewFolderName + FileFind.GetFileName();
			NewFolderName += "/";
			PushCPPlist(_RootPath, NewFolderName, _SavePath);
		}
		else
		{
			// CString PathName = FileFind.GetFilePath();
			CString FileName = FileFind.GetFileName();
			//cpp 확인
			CString FileName2 = FileFind.GetFileName();
			CString FileName3 = FileFind.GetFileTitle();
			FileName3 += ".cpp";

			if (FileName2 == FileName3)
			{
				std::string TextName = "#include \"";
				TextName += "../../";
				TextName += CW2A(_FolderName);
				TextName += CW2A(FileName);
				TextName += "\"\n";

				CPPlist.push_back(TextName);
			}

		}
	}


	FileFind.Close();
}
