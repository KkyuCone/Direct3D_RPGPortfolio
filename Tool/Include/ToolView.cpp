// ToolView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "ToolView.h"

#include "ToolDoc.h"
#include "Core.h"


ENGINE_USING

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ToolView

IMPLEMENT_DYNCREATE(ToolView, CView)

ToolView::ToolView()
{

	EnableAutomation();
}

ToolView::~ToolView()
{
}

void ToolView::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면
	// OnFinalRelease가 호출됩니다.  기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CView::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(ToolView, CView)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(ToolView, CView)
END_DISPATCH_MAP()

// 참고: IID_IToolView에 대한 지원을 추가하여
//  VBA에서 형식 안전 바인딩을 지원합니다. 
//  이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {2DA0F8DD-5AEA-48E6-915E-2BFBD5AE065F}
static const IID IID_IToolView =
{ 0x2DA0F8DD, 0x5AEA, 0x48E6, { 0x91, 0x5E, 0x2B, 0xFB, 0xD5, 0xAE, 0x6, 0x5F } };

BEGIN_INTERFACE_MAP(ToolView, CView)
	INTERFACE_PART(ToolView, IID_IToolView, Dispatch)
END_INTERFACE_MAP()


// ToolView 그리기입니다.

void ToolView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}

BOOL ToolView::PreCreateWindow(CREATESTRUCT & cs)
{
	return CView::PreCreateWindow(cs);
}

BOOL ToolView::OnPreparePrinting(CPrintInfo * pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void ToolView::OnBeginPrinting(CDC * pDC, CPrintInfo * pInfo)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void ToolView::OnEndPrinting(CDC * pDC, CPrintInfo * pInfo)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// ToolView 진단입니다.

#ifdef _DEBUG
void ToolView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void ToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// ToolView 메시지 처리기입니다.
