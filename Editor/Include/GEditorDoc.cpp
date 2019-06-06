// GEditorDoc.cpp : ���� �����Դϴ�.
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
	// �ڵ�ȭ ��ü�� ���� ������ ������ �����Ǹ�
	// OnFinalRelease�� ȣ��˴ϴ�.  �⺻ Ŭ�������� �ڵ����� ��ü�� �����մϴ�.
	// �⺻ Ŭ������ ȣ���ϱ� ���� ��ü�� �ʿ��� �߰� ���� �۾���
	// �߰��Ͻʽÿ�.

	CDocument::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(GEditorDoc, CDocument)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(GEditorDoc, CDocument)
END_DISPATCH_MAP()

// ����: IID_IGEditorDoc�� ���� ������ �߰��Ͽ�
//  VBA���� ���� ���� ���ε��� �����մϴ�. 
//  �� IID�� .IDL ���Ͽ� �ִ� dispinterface�� GUID�� ��ġ�ؾ� �մϴ�.

// {5FD07BAF-4128-44A5-A8B1-4B87E61B90C0}
static const IID IID_IGEditorDoc =
{ 0x5FD07BAF, 0x4128, 0x44A5, { 0xA8, 0xB1, 0x4B, 0x87, 0xE6, 0x1B, 0x90, 0xC0 } };

BEGIN_INTERFACE_MAP(GEditorDoc, CDocument)
	INTERFACE_PART(GEditorDoc, IID_IGEditorDoc, Dispatch)
END_INTERFACE_MAP()


// GEditorDoc �����Դϴ�.

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
// GEditorDoc serialization�Դϴ�.

void GEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}
#endif


// GEditorDoc ����Դϴ�.
