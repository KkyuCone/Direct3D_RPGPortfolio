
// UNITYMAKER.h : UNITYMAKER 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.
#include <list>

// CUNITYMAKERApp:
// 이 클래스의 구현에 대해서는 UNITYMAKER.cpp을 참조하십시오.
//

class CUNITYMAKERApp : public CWinApp
{
public:
	CUNITYMAKERApp();


// 재정의입니다.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

public:
	void CreateUnityCpp(const wchar_t* _RootPath, const wchar_t* _FolderName, const wchar_t* _FileName);

// 구현입니다.

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
