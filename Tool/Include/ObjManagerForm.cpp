// ObjManagerForm.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "ObjManagerForm.h"


// ObjManagerForm

IMPLEMENT_DYNCREATE(ObjManagerForm, CFormView)

ObjManagerForm::ObjManagerForm()
	: CFormView(IDD_OBJMANAGERFORM)
{

	EnableAutomation();
}

ObjManagerForm::~ObjManagerForm()
{
}

void ObjManagerForm::OnFinalRelease()
{
	// �ڵ�ȭ ��ü�� ���� ������ ������ �����Ǹ�
	// OnFinalRelease�� ȣ��˴ϴ�.  �⺻ Ŭ�������� �ڵ����� ��ü�� �����մϴ�.
	// �⺻ Ŭ������ ȣ���ϱ� ���� ��ü�� �ʿ��� �߰� ���� �۾���
	// �߰��Ͻʽÿ�.

	CFormView::OnFinalRelease();
}

void ObjManagerForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(ObjManagerForm, CFormView)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(ObjManagerForm, CFormView)
END_DISPATCH_MAP()

// ����: IID_IObjManagerForm�� ���� ������ �߰��Ͽ�
//  VBA���� ���� ���� ���ε��� �����մϴ�. 
//  �� IID�� .IDL ���Ͽ� �ִ� dispinterface�� GUID�� ��ġ�ؾ� �մϴ�.

// {6716058F-3E98-4361-8C43-24314C2BF440}
static const IID IID_IObjManagerForm =
{ 0x6716058F, 0x3E98, 0x4361, { 0x8C, 0x43, 0x24, 0x31, 0x4C, 0x2B, 0xF4, 0x40 } };

BEGIN_INTERFACE_MAP(ObjManagerForm, CFormView)
	INTERFACE_PART(ObjManagerForm, IID_IObjManagerForm, Dispatch)
END_INTERFACE_MAP()


// ObjManagerForm �����Դϴ�.

#ifdef _DEBUG
void ObjManagerForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void ObjManagerForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// ObjManagerForm �޽��� ó�����Դϴ�.
