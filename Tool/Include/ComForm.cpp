// ComForm.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "ComForm.h"


// ComForm

IMPLEMENT_DYNCREATE(ComForm, CFormView)

ComForm::ComForm()
	: CFormView(IDD_COMFORM)
{

	EnableAutomation();
}

ComForm::~ComForm()
{
}

void ComForm::OnFinalRelease()
{
	// �ڵ�ȭ ��ü�� ���� ������ ������ �����Ǹ�
	// OnFinalRelease�� ȣ��˴ϴ�.  �⺻ Ŭ�������� �ڵ����� ��ü�� �����մϴ�.
	// �⺻ Ŭ������ ȣ���ϱ� ���� ��ü�� �ʿ��� �߰� ���� �۾���
	// �߰��Ͻʽÿ�.

	CFormView::OnFinalRelease();
}

void ComForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(ComForm, CFormView)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(ComForm, CFormView)
END_DISPATCH_MAP()

// ����: IID_IomForm�� ���� ������ �߰��Ͽ�
//  VBA���� ���� ���� ���ε��� �����մϴ�. 
//  �� IID�� .IDL ���Ͽ� �ִ� dispinterface�� GUID�� ��ġ�ؾ� �մϴ�.

// {39080CE4-6ADD-4A55-A097-F7146244DA67}
static const IID IID_IomForm =
{ 0x39080CE4, 0x6ADD, 0x4A55, { 0xA0, 0x97, 0xF7, 0x14, 0x62, 0x44, 0xDA, 0x67 } };

BEGIN_INTERFACE_MAP(ComForm, CFormView)
	INTERFACE_PART(ComForm, IID_IomForm, Dispatch)
END_INTERFACE_MAP()


// ComForm �����Դϴ�.

#ifdef _DEBUG
void ComForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void ComForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// ComForm �޽��� ó�����Դϴ�.
