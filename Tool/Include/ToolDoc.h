#pragma once

// ToolDoc 문서입니다.

class ToolDoc : public CDocument
{
protected:
	ToolDoc();
	DECLARE_DYNCREATE(ToolDoc)

public:
	virtual ~ToolDoc();

public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);   // 문서 입/출력을 위해 재정의되었습니다.
	virtual void OnFinalRelease();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};
