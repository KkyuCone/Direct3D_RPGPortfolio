#pragma once



// ComForm �� ���Դϴ�.

class ComForm : public CFormView
{
	DECLARE_DYNCREATE(ComForm)

protected:
	ComForm();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~ComForm();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COMFORM };
#endif

	virtual void OnFinalRelease();
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};


