
// MainFrm.h : CMainFrame Ŭ������ �������̽�
//

#pragma once

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Ư���Դϴ�.
public:

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// �����Դϴ�.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CToolBar      m_wndToolBar;
	CSplitterWnd	m_SplitWnd;

// ������ �޽��� �� �Լ�
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
};


