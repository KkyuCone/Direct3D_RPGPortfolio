
// Editor.h : Editor ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CEditorApp:
// �� Ŭ������ ������ ���ؼ��� Editor.cpp�� �����Ͻʽÿ�.
//

class CEditorApp : public CWinApp
{
	CSingleDocTemplate* pDocTemplate;
public:
	CEditorApp();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};

extern CEditorApp theApp;
