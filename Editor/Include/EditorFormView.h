#pragma once
#include "afxwin.h"

#include "GameObject.h"
#include "Component/Animation.h"
#include "Component/Renderer.h"
#include "Component/Transform.h"
#include "afxcmn.h"

ENGINE_USING
// EditorFormView 폼 뷰입니다.

class EditorFormView : public CFormView
{
	DECLARE_DYNCREATE(EditorFormView)

protected:
	EditorFormView();           // 동적 만들기에 사용되는 protected 생성자입니다.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	class GameObject*	m_pObject;
	class Animation*	m_pAnimation;
	class Renderer*		m_pRenderer;
	std::vector<PBONEKEYFRAME>	m_vecKeyFrame;

public:
	CString m_strMeshName;		// 메쉬 이름
	CString m_strClipName;		// 클립 이름
	UINT m_iStartFrame;			// 시작 프레임
	UINT m_iEndFrame;			// 종료 프레임
	float m_fPlayTime;			// 플레이 시간
	CComboBox m_OptionCombo;	// 옵션 콤보
	CListBox m_ClipList;		// 클립 리스트
	BOOL m_bKeyFrameCopy;		// 키프레임복사 체크 유무

	int m_AnimListNum;
	CListCtrl m_AnimListCtrl;		// 애니메이션 클립 리스트 컨트롤

public:
	// Button
	afx_msg void OnBnClickedButtonAddclip();				// 클립 추가
	afx_msg void OnBnClickedButtonModifyclip();				// 클립 수정
	afx_msg void OnBnClickedButtonDeleteclip();				// 클립 삭제
	afx_msg void OnBnClickedButtonMeshload();				// 메쉬 로드
	afx_msg void OnBnClickedButtonMeshsave();				// 메쉬 저장
	afx_msg void OnBnClickedButtonFbxload();				// FBX 로드
	afx_msg void OnBnClickedButtonClipload();				// 클립 로드
	afx_msg void OnBnClickedButtonClipsave();				// 클립 저장
	afx_msg void OnBnClickedCheckFramecopy();				// 키프레임복사

	// List Box
	afx_msg void OnLbnSelchangeListClip();					// 클립 리스트
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);

	virtual void OnInitialUpdate();
};


