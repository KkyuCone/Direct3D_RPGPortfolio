// GEditorDoc.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Editor.h"
#include "GEditorDoc.h"


// GEditorDoc

IMPLEMENT_DYNCREATE(GEditorDoc, CDocument)

GEditorDoc::GEditorDoc()
{
	EnableAutomation();
}

BOOL GEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

GEditorDoc::~GEditorDoc()
{
}

void GEditorDoc::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면
	// OnFinalRelease가 호출됩니다.  기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDocument::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(GEditorDoc, CDocument)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(GEditorDoc, CDocument)
END_DISPATCH_MAP()

// 참고: IID_IGEditorDoc에 대한 지원을 추가하여
//  VBA에서 형식 안전 바인딩을 지원합니다. 
//  이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {5FD07BAF-4128-44A5-A8B1-4B87E61B90C0}
static const IID IID_IGEditorDoc =
{ 0x5FD07BAF, 0x4128, 0x44A5, { 0xA8, 0xB1, 0x4B, 0x87, 0xE6, 0x1B, 0x90, 0xC0 } };

BEGIN_INTERFACE_MAP(GEditorDoc, CDocument)
	INTERFACE_PART(GEditorDoc, IID_IGEditorDoc, Dispatch)
END_INTERFACE_MAP()


// GEditorDoc 진단입니다.

#ifdef _DEBUG
void GEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void GEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// GEditorDoc serialization입니다.

void GEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}
#endif


// GEditorDoc 명령입니다.
