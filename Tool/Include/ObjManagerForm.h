#pragma once



// ObjManagerForm �� ���Դϴ�.

class ObjManagerForm : public CFormView
{
	DECLARE_DYNCREATE(ObjManagerForm)

protected:
	ObjManagerForm();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~ObjManagerForm();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OBJMANAGERFORM };
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


