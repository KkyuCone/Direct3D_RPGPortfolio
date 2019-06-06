// ObjManagerForm.cpp : 구현 파일입니다.
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
	// 자동화 개체에 대한 마지막 참조가 해제되면
	// OnFinalRelease가 호출됩니다.  기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

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

// 참고: IID_IObjManagerForm에 대한 지원을 추가하여
//  VBA에서 형식 안전 바인딩을 지원합니다. 
//  이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {6716058F-3E98-4361-8C43-24314C2BF440}
static const IID IID_IObjManagerForm =
{ 0x6716058F, 0x3E98, 0x4361, { 0x8C, 0x43, 0x24, 0x31, 0x4C, 0x2B, 0xF4, 0x40 } };

BEGIN_INTERFACE_MAP(ObjManagerForm, CFormView)
	INTERFACE_PART(ObjManagerForm, IID_IObjManagerForm, Dispatch)
END_INTERFACE_MAP()


// ObjManagerForm 진단입니다.

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


// ObjManagerForm 메시지 처리기입니다.
