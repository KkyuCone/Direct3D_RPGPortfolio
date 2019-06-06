// EditorFormView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Editor.h"
#include "EditorFormView.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/Layer.h"
#include "Resource/ResourcesManager.h"



// EditorFormView

IMPLEMENT_DYNCREATE(EditorFormView, CFormView)

EditorFormView::EditorFormView()
	: CFormView(IDD_DIALOG_FORM_VIEW)
	, m_strMeshName(_T(""))
	, m_strClipName(_T(""))
	, m_iStartFrame(0)
	, m_iEndFrame(0)
	, m_fPlayTime(0)
	, m_bKeyFrameCopy(FALSE)
	, m_pObject(nullptr)
	, m_pAnimation(nullptr)
	, m_pRenderer(nullptr)
	, m_AnimListNum(0)
{
}

EditorFormView::~EditorFormView()
{
	Safe_Delete_VectorList(m_vecKeyFrame);
	SAFE_RELEASE(m_pObject);
	SAFE_RELEASE(m_pAnimation);
	SAFE_RELEASE(m_pRenderer);
}


void EditorFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MESHNAME, m_strMeshName);
	DDX_Text(pDX, IDC_EDIT_CLIPNAME, m_strClipName);
	DDX_Text(pDX, IDC_EDIT_STARTFRAME, m_iStartFrame);
	DDX_Text(pDX, IDC_EDIT_ENDFRAME, m_iEndFrame);
	DDX_Text(pDX, IDC_EDIT_PLAYTIME, m_fPlayTime);
	DDX_Control(pDX, IDC_COMBO_OP, m_OptionCombo);
	DDX_Control(pDX, IDC_LIST_CLIP, m_ClipList);
	DDX_Check(pDX, IDC_CHECK_FRAMECOPY, m_bKeyFrameCopy);
	DDX_Control(pDX, IDC_LIST1, m_AnimListCtrl);
}

BEGIN_MESSAGE_MAP(EditorFormView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_ADDCLIP, &EditorFormView::OnBnClickedButtonAddclip)
	ON_BN_CLICKED(IDC_BUTTON_MODIFYCLIP, &EditorFormView::OnBnClickedButtonModifyclip)
	ON_BN_CLICKED(IDC_BUTTON_DELETECLIP, &EditorFormView::OnBnClickedButtonDeleteclip)
	ON_BN_CLICKED(IDC_BUTTON_MESHLOAD, &EditorFormView::OnBnClickedButtonMeshload)
	ON_BN_CLICKED(IDC_BUTTON_MESHSAVE, &EditorFormView::OnBnClickedButtonMeshsave)
	ON_BN_CLICKED(IDC_BUTTON_FBXLOAD, &EditorFormView::OnBnClickedButtonFbxload)
	ON_BN_CLICKED(IDC_BUTTON_CLIPLOAD, &EditorFormView::OnBnClickedButtonClipload)
	ON_BN_CLICKED(IDC_BUTTON_CLIPSAVE, &EditorFormView::OnBnClickedButtonClipsave)
	ON_BN_CLICKED(IDC_CHECK_FRAMECOPY, &EditorFormView::OnBnClickedCheckFramecopy)
	ON_LBN_SELCHANGE(IDC_LIST_CLIP, &EditorFormView::OnLbnSelchangeListClip)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &EditorFormView::OnLvnItemchangedList1)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()



// EditorFormView �����Դϴ�.

#ifdef _DEBUG
void EditorFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void EditorFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// EditorFormView �޽��� ó�����Դϴ�.

// Ŭ�� �߰�
void EditorFormView::OnBnClickedButtonAddclip()
{
	if (!m_pAnimation)
		return;

	UpdateData(TRUE);

	char	strName[256] = {};

	WideCharToMultiByte(CP_ACP, 0, m_strClipName.GetString(), -1,
		strName, m_strClipName.GetLength(), 0, 0);

	//  Ŭ�� �̸��� �����Ѵ�.
	if (!m_vecKeyFrame.empty())
	{
		m_pAnimation->AddClip(strName, (ANIMATION_OPTION)m_OptionCombo.GetCurSel(),
			m_iStartFrame, m_iEndFrame, m_fPlayTime, m_vecKeyFrame);

		m_ClipList.AddString(m_strClipName);


		// List Control���� �߰����ֱ�
		int Row = m_AnimListNum;
		const char* cRow;
		//itoa(Row, cRow, 10);
	
		std::string strListCtrl  = std::to_string(Row);
		cRow = strListCtrl.c_str();
		m_AnimListCtrl.InsertItem(m_AnimListNum, _T("asdfasdfs"));														// (0, 0)
		m_AnimListCtrl.SetItem(0, 1, LVIF_TEXT, TEXT("����������"), NULL, NULL, NULL, NULL, NULL);	// (0, 1)

		++m_AnimListNum;
	}
}


// Ŭ�� ����
void EditorFormView::OnBnClickedButtonModifyclip()
{
	if (!m_pAnimation)
		return;

	int	iListIndex = m_ClipList.GetCurSel();

	if (iListIndex == -1)
		return;

	CString	strListName;
	m_ClipList.GetText(iListIndex, strListName);

	char	strOriginName[256] = {};

	WideCharToMultiByte(CP_ACP, 0, strListName.GetString(), -1,
		strOriginName, strListName.GetLength(), 0, 0);

	UpdateData(TRUE);

	char	strName[256] = {};

	WideCharToMultiByte(CP_ACP, 0, m_strClipName.GetString(), -1,
		strName, m_strClipName.GetLength(), 0, 0);

	m_ClipList.DeleteString(iListIndex);
	m_ClipList.AddString(m_strClipName);

	//  Ŭ�� �̸��� �����Ѵ�.
	if (m_vecKeyFrame.empty())
	{
		m_pAnimation->ChangeClipKey(strOriginName, strName);
	}

	else
	{
		m_pAnimation->ModifyClip(strOriginName, strName,
			(ANIMATION_OPTION)m_OptionCombo.GetCurSel(),
			m_iStartFrame, m_iEndFrame, m_fPlayTime, m_vecKeyFrame);
	}
}


void EditorFormView::OnBnClickedButtonDeleteclip()
{
	if (!m_pAnimation)
		return;

	else if (m_ClipList.GetCount() == 1)
		return;

	int	iListIndex = m_ClipList.GetCurSel();

	if (iListIndex == -1)
		return;

	CString	strListName;
	m_ClipList.GetText(iListIndex, strListName);

	char	strOriginName[256] = {};

	WideCharToMultiByte(CP_ACP, 0, strListName.GetString(), -1,
		strOriginName, strListName.GetLength(), 0, 0);

	char	strName[256] = {};

	m_ClipList.DeleteString(iListIndex);

	m_pAnimation->DeleteClip(strOriginName);

	PANIMATIONCLIP	pClip = m_pAnimation->GetCurrentClip();

	m_strClipName = CA2CT(pClip->strName.c_str());
	m_iStartFrame = pClip->iStartFrame;
	m_iEndFrame = pClip->iEndFrame;
	m_fPlayTime = pClip->fPlayTime;
	m_OptionCombo.SetCurSel(pClip->eOption);

	UpdateData(FALSE);
}

// �޽� �ε�
void EditorFormView::OnBnClickedButtonMeshload()
{
	const TCHAR* pFilter = TEXT("Mesh File(*.msh)|*.msh|�������(*.*)|*.*||");
	CFileDialog	dlg(TRUE, TEXT(".msh"), nullptr, OFN_HIDEREADONLY,
		pFilter);

	if (dlg.DoModal() == IDOK)
	{
		CString	strPath = dlg.GetPathName();

		if (!m_pObject)
		{
			Scene*	pScene = GET_SINGLETON(SceneManager)->GetScene();
			Layer*	pLayer = pScene->FindLayer("Default");

			m_pObject = GameObject::CreateObject("Obj", pLayer);

			Transform*	pTransform = m_pObject->GetTransform();

			pTransform->SetWorldScale(1.0f, 1.0f, 1.0f);

			SAFE_RELEASE(pTransform);
		}

		if (!m_pRenderer)
		{
			m_pRenderer = m_pObject->AddComponent<Renderer>("Renderer");

			m_pRenderer->SetMeshFromFullPath("EditMesh", strPath.GetString());
		}

		else
		{
			GET_SINGLETON(ResourcesManager)->DeleteMesh("EditMesh");
			m_pRenderer->SetMeshFromFullPath("EditMesh", strPath.GetString());
		}

		m_strMeshName = TEXT("EditMesh");

		UpdateData(FALSE);
	}
}


void EditorFormView::OnBnClickedButtonMeshsave()
{

}

// FBX �ε�
void EditorFormView::OnBnClickedButtonFbxload()
{
	const TCHAR* pFilter = TEXT("FBX File(*.fbx)|*.fbx|�������(*.*)|*.*||");
	CFileDialog	dlg(TRUE, TEXT(".fbx"), nullptr, OFN_HIDEREADONLY,
		pFilter);

	if (dlg.DoModal() == IDOK)
	{
		CString	strPath = dlg.GetPathName();

		if (!m_pObject)
		{
			Scene*	pScene = GET_SINGLETON(SceneManager)->GetScene();
			Layer*	pLayer = pScene->FindLayer("Default");

			m_pObject = GameObject::CreateObject("Obj", pLayer);

			Transform*	pTransform = m_pObject->GetTransform();

			pTransform->SetWorldScale(1.0f, 1.0f, 1.0f);

			SAFE_RELEASE(pTransform);
		}

		if (!m_pRenderer)
		{
			m_pRenderer = m_pObject->AddComponent<Renderer>("Renderer");

			m_pRenderer->SetMeshFromFullPath("EditMesh", strPath.GetString());
		}

		else
		{
			GET_SINGLETON(ResourcesManager)->DeleteMesh("EditMesh");
			m_pRenderer->SetMeshFromFullPath("EditMesh", strPath.GetString());
		}

		SAFE_RELEASE(m_pAnimation);
		m_pAnimation = m_pObject->FindComponentFromType<Animation>(CT_ANIMATION);

		if (nullptr != m_pAnimation)
		{
			const std::list<std::string>*	pClips = m_pAnimation->GetAddClipNameList();

			std::list<std::string>::const_iterator	iter;
			std::list<std::string>::const_iterator	iterEnd = pClips->end();

			for (iter = pClips->begin(); iter != iterEnd; ++iter)
			{
				CString	strClipName = CA2CT((*iter).c_str());

				m_ClipList.AddString(strClipName);
			}

			m_ClipList.SetCurSel(0);

			PANIMATIONCLIP	pClip = m_pAnimation->GetCurrentClip();

			m_strClipName = CA2CT(pClip->strName.c_str());
			m_iStartFrame = pClip->iStartFrame;
			m_iEndFrame = pClip->iEndFrame;
			m_fPlayTime = pClip->fPlayTime;
			m_OptionCombo.SetCurSel(pClip->eOption);

			UpdateData(FALSE);
		}

	}
}

// Ŭ�� �ε�
void EditorFormView::OnBnClickedButtonClipload()
{
	const TCHAR* pFilter = TEXT("Animation File(*.anm)|*.anm|�������(*.*)|*.*||");
	CFileDialog	dlg(TRUE, TEXT(".anm"), nullptr, OFN_HIDEREADONLY,
		pFilter);

	if (dlg.DoModal() == IDOK)
	{
		CString	strPath = dlg.GetPathName();

		if (!m_pObject)
		{
			Scene*	pScene = GET_SINGLETON(SceneManager)->GetScene();
			Layer*	pLayer = pScene->FindLayer("Default");

			m_pObject = GameObject::CreateObject("Obj", pLayer);

			Transform*	pTransform = m_pObject->GetTransform();

			pTransform->SetWorldScale(1.0f, 1.0f, 1.0f);

			SAFE_RELEASE(pTransform);
		}

		SAFE_RELEASE(m_pAnimation);
		m_pObject->EraseComponentFromType(CT_ANIMATION);
		m_pAnimation = m_pObject->AddComponent<Animation>("Animation");

		TCHAR	strName[MAX_PATH] = {};

		lstrcpy(strName, strPath.GetString());

		TCHAR	strBoneName[MAX_PATH] = {};

		lstrcpy(strBoneName, strName);

		int	iLength = strPath.GetLength();

		strBoneName[iLength - 1] = 'e';
		strBoneName[iLength - 2] = 'n';
		strBoneName[iLength - 3] = 'b';

		m_pAnimation->LoadBoneFromFullPath(strBoneName);

		m_pAnimation->AddClip(strName);

		const std::list<std::string>*	pClips = m_pAnimation->GetAddClipNameList();

		std::list<std::string>::const_iterator	iter;
		std::list<std::string>::const_iterator	iterEnd = pClips->end();

		for (iter = pClips->begin(); iter != iterEnd; ++iter)
		{
			CString	strClipName = CA2CT((*iter).c_str());

			m_ClipList.AddString(strClipName);
		}

		m_ClipList.SetCurSel(0);

		PANIMATIONCLIP	pClip = m_pAnimation->GetCurrentClip();

		m_strClipName = CA2CT(pClip->strName.c_str());
		m_iStartFrame = pClip->iStartFrame;
		m_iEndFrame = pClip->iEndFrame;
		m_fPlayTime = pClip->fPlayTime;
		m_OptionCombo.SetCurSel(pClip->eOption);

		UpdateData(FALSE);
	}
}

// Ŭ�� ����
void EditorFormView::OnBnClickedButtonClipsave()
{
	if (!m_pAnimation)
		return;

	const TCHAR* pFilter = TEXT("Animation File(*.anm)|*.anm|�������(*.*)|*.*||");
	CFileDialog	dlg(FALSE, TEXT(".anm"), nullptr, OFN_OVERWRITEPROMPT,
		pFilter);

	if (dlg.DoModal() == IDOK)
	{
		CString	strPath = dlg.GetPathName();

		TCHAR	strName[MAX_PATH] = {};

		lstrcpy(strName, strPath.GetString());

		TCHAR	strBoneName[MAX_PATH] = {};

		lstrcpy(strBoneName, strName);

		int	iLength = strPath.GetLength();

		strBoneName[iLength - 1] = 'e';
		strBoneName[iLength - 2] = 'n';
		strBoneName[iLength - 3] = 'b';

		m_pAnimation->SaveBoneFromFullPath(strBoneName);

		m_pAnimation->SaveFromFullPath(strName);
	}
}


// üũ�ڽ�
void EditorFormView::OnBnClickedCheckFramecopy()
{
	if (!m_pAnimation)
	{
		m_bKeyFrameCopy = FALSE;
		UpdateData(FALSE);
		return;
	}

	UpdateData(TRUE);

	if (m_bKeyFrameCopy)
	{
		m_pAnimation->GetCurrentKeyFrame(m_vecKeyFrame);
	}

	else
	{
		Safe_Delete_VectorList(m_vecKeyFrame);
	}
}

// Ŭ������Ʈ ���� ����
void EditorFormView::OnLbnSelchangeListClip()
{
	if (!m_pAnimation)
		return;

	int	iIndex = m_ClipList.GetCurSel();

	if (iIndex == -1)
		return;

	CString	strClipName;
	m_ClipList.GetText(iIndex, strClipName);

	char	strName[256] = {};

	WideCharToMultiByte(CP_ACP, 0, strClipName.GetString(), -1,
		strName, strClipName.GetLength(), 0, 0);

	PANIMATIONCLIP	pClip = m_pAnimation->FindClip(strName);

	if (!pClip)
		return;

	m_strClipName = CA2CT(pClip->strName.c_str());
	m_iStartFrame = pClip->iStartFrame;
	m_iEndFrame = pClip->iEndFrame;
	m_fPlayTime = pClip->fPlayTime;

	m_OptionCombo.SetCurSel(pClip->eOption);

	UpdateData(FALSE);

	m_pAnimation->ChangeClip(pClip->strName);
}


void EditorFormView::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	int ScrPos = m_AnimListCtrl.GetScrollPos(SB_VERT);			// ���� ��ũ�� ��ġ

	// ������ �ִ� �ڵ� �߰�

	// ���⼭ �°�..����
	CRect ref;
	m_AnimListCtrl.GetItemRect(0, ref, LVIR_BOUNDS);			// ��ĭ ���� ����

	CSize size(0, ref.Height() * ScrPos);

	m_AnimListCtrl.Scroll(size);								// ��ũ�ѹ� ����ġ�� �ʱ�ȭ���ֱ�

}


void EditorFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	m_AnimListCtrl.DeleteAllItems();

	m_AnimListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES /*| LVS_EX_CHECKBOXES*/);

	// Ÿ��Ʋ ����
	m_AnimListCtrl.InsertColumn(0, _T("�޽� �̸�"), LVCFMT_CENTER, 100, -1);
	m_AnimListCtrl.InsertColumn(1, _T("Ŭ�� �̸�"), LVCFMT_CENTER, 100, -1);
	m_AnimListCtrl.InsertColumn(2, _T("���� ������"), LVCFMT_CENTER, 100, -1);
	m_AnimListCtrl.InsertColumn(3, _T("���� ������"), LVCFMT_CENTER, 100, -1);
	m_AnimListCtrl.InsertColumn(4, _T("�÷��� Ÿ��"), LVCFMT_CENTER, 100, -1);
	m_AnimListCtrl.InsertColumn(5, _T("�ɼ�"), LVCFMT_CENTER, 100, -1);


	// ä��� ������
	//int num = m_AnimListCtrl.GetItemCount();

	//CString str;
	//str.Format(_T("%d"), num);

	// ù ���� �����͸� �Է��Ҷ��� InsertItem()�Լ��� ����ϰ�
	// ������ ���� �����ʹ� SEtITem �Լ��� ����Ѵ�.
	//m_AnimListCtrl.InsertItem(0, _T("1"));														// (0, 0)
	//m_AnimListCtrl.SetItem(0, 1, LVIF_TEXT, TEXT("����������"), NULL, NULL, NULL, NULL, NULL);	// (0, 1)

	//m_AnimListCtrl.InsertItem(1, _T("2"));
	//m_AnimListCtrl.SetItem(2, 1, LVIF_TEXT, TEXT("����������"), NULL, NULL, NULL, NULL, NULL);

}
