#pragma once
#include "Engine.h"
#include "ToolDoc.h"

// ToolView 뷰입니다.
ENGINE_USING

class ToolView : public CView
{
protected:
	ToolView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	DECLARE_DYNCREATE(ToolView)

public:
	virtual ~ToolView();

public:
	ToolDoc* GetDocument() const;

public:
	virtual void OnFinalRelease();
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
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


#ifdef _DEBUG  // EditorView.cpp의 디버그 버전
inline ToolDoc* ToolView::GetDocument() const
{
	return reinterpret_cast<ToolDoc*>(m_pDocument);
}
#endif

