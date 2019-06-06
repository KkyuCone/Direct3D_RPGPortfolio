// ToolDoc.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "ToolDoc.h"
#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ToolDoc

IMPLEMENT_DYNCREATE(ToolDoc, CDocument)

ToolDoc::ToolDoc()
{
}

BOOL ToolDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

ToolDoc::~ToolDoc()
{
}

void ToolDoc::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면
	// OnFinalRelease가 호출됩니다.  기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDocument::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(ToolDoc, CDocument)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(ToolDoc, CDocument)
END_DISPATCH_MAP()

// 참고: IID_IToolDoc에 대한 지원을 추가하여
//  VBA에서 형식 안전 바인딩을 지원합니다. 
//  이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {55E39528-A341-4FAA-8CA9-69A9D2CD23A4}
static const IID IID_IToolDoc =
{ 0x55E39528, 0xA341, 0x4FAA, { 0x8C, 0xA9, 0x69, 0xA9, 0xD2, 0xCD, 0x23, 0xA4 } };

BEGIN_INTERFACE_MAP(ToolDoc, CDocument)
	INTERFACE_PART(ToolDoc, IID_IToolDoc, Dispatch)
END_INTERFACE_MAP()


// ToolDoc 진단입니다.

#ifdef _DEBUG
void ToolDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void ToolDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

// ToolDoc serialization입니다.

void ToolDoc::Serialize(CArchive& ar)
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


// ToolDoc 명령입니다.
