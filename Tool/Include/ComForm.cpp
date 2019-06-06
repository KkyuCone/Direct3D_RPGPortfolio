// ComForm.cpp : 구현 파일입니다.
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
	// 자동화 개체에 대한 마지막 참조가 해제되면
	// OnFinalRelease가 호출됩니다.  기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

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

// 참고: IID_IomForm에 대한 지원을 추가하여
//  VBA에서 형식 안전 바인딩을 지원합니다. 
//  이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {39080CE4-6ADD-4A55-A097-F7146244DA67}
static const IID IID_IomForm =
{ 0x39080CE4, 0x6ADD, 0x4A55, { 0xA0, 0x97, 0xF7, 0x14, 0x62, 0x44, 0xDA, 0x67 } };

BEGIN_INTERFACE_MAP(ComForm, CFormView)
	INTERFACE_PART(ComForm, IID_IomForm, Dispatch)
END_INTERFACE_MAP()


// ComForm 진단입니다.

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


// ComForm 메시지 처리기입니다.
