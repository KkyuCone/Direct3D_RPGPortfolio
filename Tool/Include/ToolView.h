#pragma once
#include "Engine.h"
#include "ToolDoc.h"

// ToolView ���Դϴ�.
ENGINE_USING

class ToolView : public CView
{
protected:
	ToolView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	DECLARE_DYNCREATE(ToolView)

public:
	virtual ~ToolView();

public:
	ToolDoc* GetDocument() const;

public:
	virtual void OnFinalRelease();
	virtual void OnDraw(CDC* pDC);      // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};


#ifdef _DEBUG  // EditorView.cpp�� ����� ����
inline ToolDoc* ToolView::GetDocument() const
{
	return reinterpret_cast<ToolDoc*>(m_pDocument);
}
#endif

