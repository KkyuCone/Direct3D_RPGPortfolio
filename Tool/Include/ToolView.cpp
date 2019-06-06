// ToolView.cpp : ���� �����Դϴ�.
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
	// �ڵ�ȭ ��ü�� ���� ������ ������ �����Ǹ�
	// OnFinalRelease�� ȣ��˴ϴ�.  �⺻ Ŭ�������� �ڵ����� ��ü�� �����մϴ�.
	// �⺻ Ŭ������ ȣ���ϱ� ���� ��ü�� �ʿ��� �߰� ���� �۾���
	// �߰��Ͻʽÿ�.

	CView::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(ToolView, CView)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(ToolView, CView)
END_DISPATCH_MAP()

// ����: IID_IToolView�� ���� ������ �߰��Ͽ�
//  VBA���� ���� ���� ���ε��� �����մϴ�. 
//  �� IID�� .IDL ���Ͽ� �ִ� dispinterface�� GUID�� ��ġ�ؾ� �մϴ�.

// {2DA0F8DD-5AEA-48E6-915E-2BFBD5AE065F}
static const IID IID_IToolView =
{ 0x2DA0F8DD, 0x5AEA, 0x48E6, { 0x91, 0x5E, 0x2B, 0xFB, 0xD5, 0xAE, 0x6, 0x5F } };

BEGIN_INTERFACE_MAP(ToolView, CView)
	INTERFACE_PART(ToolView, IID_IToolView, Dispatch)
END_INTERFACE_MAP()


// ToolView �׸����Դϴ�.

void ToolView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: ���⿡ �׸��� �ڵ带 �߰��մϴ�.
}

BOOL ToolView::PreCreateWindow(CREATESTRUCT & cs)
{
	return CView::PreCreateWindow(cs);
}

BOOL ToolView::OnPreparePrinting(CPrintInfo * pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void ToolView::OnBeginPrinting(CDC * pDC, CPrintInfo * pInfo)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void ToolView::OnEndPrinting(CDC * pDC, CPrintInfo * pInfo)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// ToolView �����Դϴ�.

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


// ToolView �޽��� ó�����Դϴ�.
