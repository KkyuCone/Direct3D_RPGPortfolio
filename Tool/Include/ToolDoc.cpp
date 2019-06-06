// ToolDoc.cpp : ���� �����Դϴ�.
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
	// �ڵ�ȭ ��ü�� ���� ������ ������ �����Ǹ�
	// OnFinalRelease�� ȣ��˴ϴ�.  �⺻ Ŭ�������� �ڵ����� ��ü�� �����մϴ�.
	// �⺻ Ŭ������ ȣ���ϱ� ���� ��ü�� �ʿ��� �߰� ���� �۾���
	// �߰��Ͻʽÿ�.

	CDocument::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(ToolDoc, CDocument)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(ToolDoc, CDocument)
END_DISPATCH_MAP()

// ����: IID_IToolDoc�� ���� ������ �߰��Ͽ�
//  VBA���� ���� ���� ���ε��� �����մϴ�. 
//  �� IID�� .IDL ���Ͽ� �ִ� dispinterface�� GUID�� ��ġ�ؾ� �մϴ�.

// {55E39528-A341-4FAA-8CA9-69A9D2CD23A4}
static const IID IID_IToolDoc =
{ 0x55E39528, 0xA341, 0x4FAA, { 0x8C, 0xA9, 0x69, 0xA9, 0xD2, 0xCD, 0x23, 0xA4 } };

BEGIN_INTERFACE_MAP(ToolDoc, CDocument)
	INTERFACE_PART(ToolDoc, IID_IToolDoc, Dispatch)
END_INTERFACE_MAP()


// ToolDoc �����Դϴ�.

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

// ToolDoc serialization�Դϴ�.

void ToolDoc::Serialize(CArchive& ar)
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


// ToolDoc ����Դϴ�.
