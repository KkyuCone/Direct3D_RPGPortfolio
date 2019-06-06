
// UNITYMAKER.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
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
	// TODO: �Ʒ� ���� ���α׷� ID ���ڿ��� ���� ID ���ڿ��� �ٲٽʽÿ�(����).
	// ���ڿ��� ���� ����: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("UNITYMAKER.AppID.NoVersion"));

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}

// ������ CUNITYMAKERApp ��ü�Դϴ�.

CUNITYMAKERApp theApp;


// CUNITYMAKERApp �ʱ�ȭ

BOOL CUNITYMAKERApp::InitInstance()
{
	CWinApp::InitInstance();


	EnableTaskbarInteraction(FALSE);

	// RichEdit ��Ʈ���� ����Ϸ���  AfxInitRichEdit2()�� �־�� �մϴ�.	
	// AfxInitRichEdit2();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));

	// �ð��뺰�� 
	// ����Ƽ ���带 ����µ� �Ѱ���

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
	//TODO: �߰��� �߰� ���ҽ��� ó���մϴ�.
	return CWinApp::ExitInstance();
}

// CUNITYMAKERApp �޽��� ó����
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

	// ���� -> ���ο� cpp
	// ����Ƽ cpp -> cpp�� ��δ� ��Ƴ��� ����
	// ������ �����ð� > ����Ƽ cpp �� �����ð����� ũ��
	// �׶��� cpp�� �����.

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
			//cpp Ȯ��
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
