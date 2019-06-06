#pragma once

// ToolDoc �����Դϴ�.

class ToolDoc : public CDocument
{
protected:
	ToolDoc();
	DECLARE_DYNCREATE(ToolDoc)

public:
	virtual ~ToolDoc();

public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);   // ���� ��/����� ���� �����ǵǾ����ϴ�.
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
