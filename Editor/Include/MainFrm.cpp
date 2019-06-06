
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "Editor.h"

#include "MainFrm.h"

#include "EditorView.h"
#include "EditorFormView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

//const int  iMaxUserToolbars = 10;
//const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
//const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

//static UINT indicators[] =
//{
//	ID_SEPARATOR,           // 상태 줄 표시기
//	ID_INDICATOR_CAPS,
//	ID_INDICATOR_NUM,
//	ID_INDICATOR_SCRL,
//};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
	int a = 0;
}

// 모든 프레임 윈도우에 공통으로 필요한 초기화 작업을 수행하는 함수
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

#pragma region 메인 프레임 윈도우에서 사용할 상태바 윈도우를 만드는 작업
	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	//{
	//	TRACE0("도구 모음을 만들지 못했습니다.\n");
	//	return -1;      // 만들지 못했습니다.
	//}

	//// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndToolBar);
#pragma endregion


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	//cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
	//	 | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
	cs.cx = 1880;
	cs.cy = 720;
	//cs.cx = 1930;
	//cs.cy = 1000;

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	
	m_SplitWnd.CreateStatic(this, 1, 2);

	m_SplitWnd.CreateView(0, 0, RUNTIME_CLASS(CEditorView), CSize(1280, 720),
		pContext);
	m_SplitWnd.CreateView(0, 1, RUNTIME_CLASS(EditorFormView), CSize(500, 720),
		pContext);

	//m_SplitWnd.CreateView(1, 0, RUNTIME_CLASS(GameView), CSize(1280, 500),
	//	pContext);
	//m_SplitWnd.CreateView(1, 1, RUNTIME_CLASS(EditorFormView), CSize(650, 500),
	//	pContext);

	m_pView = (CEditorView*)m_SplitWnd.GetPane(0, 0);
	m_pForm = (CEditorForm*)m_SplitWnd.GetPane(0, 1);
	//m_pGameView = (GameView*)m_SplitWnd.GetPane(1, 0);

	//m_SplitWnd.CreateStatic(this, 2, 2);

	//m_SplitWnd.CreateView(0, 0, RUNTIME_CLASS(CEditorView), CSize(1280, 720),
	//	pContext);
	//m_SplitWnd.CreateView(0, 1, RUNTIME_CLASS(EditorFormView), CSize(650, 1000),
	//	pContext);
	//m_SplitWnd.CreateView(1, 0, RUNTIME_CLASS(GameView), CSize(1280, 280),
	//	pContext);
	//m_SplitWnd.CreateView(1, 1, RUNTIME_CLASS(EditorFormView), CSize(0, 0),
	//	pContext);

	//m_pView = (CEditorView*)m_SplitWnd.GetPane(0, 0);
	//m_pForm = (CEditorForm*)m_SplitWnd.GetPane(0, 1);
	//m_pGameView = (GameView*)m_SplitWnd.GetPane(1, 0);

	return TRUE;
	//return CFrameWnd::OnCreateClient(lpcs, pContext);
}


