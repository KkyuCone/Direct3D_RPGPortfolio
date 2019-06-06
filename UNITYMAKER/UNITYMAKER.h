
// UNITYMAKER.h : UNITYMAKER ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.
#include <list>

// CUNITYMAKERApp:
// �� Ŭ������ ������ ���ؼ��� UNITYMAKER.cpp�� �����Ͻʽÿ�.
//

class CUNITYMAKERApp : public CWinApp
{
public:
	CUNITYMAKERApp();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

public:
	void CreateUnityCpp(const wchar_t* _RootPath, const wchar_t* _FolderName, const wchar_t* _FileName);

// �����Դϴ�.

public:
	DECLARE_MESSAGE_MAP()

public:
	std::list<std::string> CPPlist;
	std::list<std::string>::iterator listbegin;
	std::list<std::string>::iterator listend;

public:
	void SaveCPPlist(CString _SavePath);
	void PushCPPlist(CString _RootPath, CString _FolderName, CString _SavePath);
};

extern CUNITYMAKERApp theApp;
