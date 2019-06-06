#pragma once
#include "afxwin.h"

#include "GameObject.h"
#include "Component/Animation.h"
#include "Component/Renderer.h"
#include "Component/Transform.h"
#include "afxcmn.h"

ENGINE_USING
// EditorFormView �� ���Դϴ�.

class EditorFormView : public CFormView
{
	DECLARE_DYNCREATE(EditorFormView)

protected:
	EditorFormView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~EditorFormView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FORM_VIEW };
#endif

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

private:
	class GameObject*	m_pObject;
	class Animation*	m_pAnimation;
	class Renderer*		m_pRenderer;
	std::vector<PBONEKEYFRAME>	m_vecKeyFrame;

public:
	CString m_strMeshName;		// �޽� �̸�
	CString m_strClipName;		// Ŭ�� �̸�
	UINT m_iStartFrame;			// ���� ������
	UINT m_iEndFrame;			// ���� ������
	float m_fPlayTime;			// �÷��� �ð�
	CComboBox m_OptionCombo;	// �ɼ� �޺�
	CListBox m_ClipList;		// Ŭ�� ����Ʈ
	BOOL m_bKeyFrameCopy;		// Ű�����Ӻ��� üũ ����

	int m_AnimListNum;
	CListCtrl m_AnimListCtrl;		// �ִϸ��̼� Ŭ�� ����Ʈ ��Ʈ��

public:
	// Button
	afx_msg void OnBnClickedButtonAddclip();				// Ŭ�� �߰�
	afx_msg void OnBnClickedButtonModifyclip();				// Ŭ�� ����
	afx_msg void OnBnClickedButtonDeleteclip();				// Ŭ�� ����
	afx_msg void OnBnClickedButtonMeshload();				// �޽� �ε�
	afx_msg void OnBnClickedButtonMeshsave();				// �޽� ����
	afx_msg void OnBnClickedButtonFbxload();				// FBX �ε�
	afx_msg void OnBnClickedButtonClipload();				// Ŭ�� �ε�
	afx_msg void OnBnClickedButtonClipsave();				// Ŭ�� ����
	afx_msg void OnBnClickedCheckFramecopy();				// Ű�����Ӻ���

	// List Box
	afx_msg void OnLbnSelchangeListClip();					// Ŭ�� ����Ʈ
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);

	virtual void OnInitialUpdate();
};


