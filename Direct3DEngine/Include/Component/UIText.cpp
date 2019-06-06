#include "UIText.h"
#include "../Device.h"
#include "../FontManager.h"
#include "../Scene/Scene.h"
#include "Camera.h"
#include "Transform.h"
#include "../GameObject.h"

ENGINE_USING

UIText::UIText() :
	m_vOffset(0.f, 0.f, 0.f),
	// ���� Ÿ��
	// RENDERTARGET2D�� GET_SINGLE(CDevice)->Get2DRenderTarget()�� ����
	m_2DTarget(RENDERTARGET2D),
	// �׸��� ����
	m_bShadow(false)
{
	m_eComponentType = CT_TEXT;
	SetTag("Text");

	// ��Ʈ�� ����
	m_pFont = new TCHAR[64];
	memset(m_pFont, 0, sizeof(TCHAR) * 64);
	lstrcpy(m_pFont, TEXT("�ü�ü"));

	// ��Ʈ ������
	m_fSize = 20.f;

	// ���� ����
	m_eAlignH = TAH_LEFT;

	// ���� ����
	m_eAlignV = TAV_MID;

	// �ؽ�Ʈ Ÿ�� (UI)
	m_eRenderType = TRT_UI;

	// ���� �� ����
	m_bAlpha = false;

	// �׸��� ���� �� ����
	m_bAlphaShadow = false;

	// ����
	m_fOpacity = 1.f;

	// �׸��� ����
	m_fShadowOpacity = 1.f;

	// ��Ʈ ����
	m_vColor = Vector4::White;

	// �׸��� ����
	m_vShadowColor = Vector4::Black;

	// �ؽ�Ʈ ����
	m_pTextFormat = GET_SINGLETON(FontManager)->FindTextFormat("�ü�ü");

	// �ؽ�Ʈ ���̾ƿ�
	m_pTextLayout = GET_SINGLETON(FontManager)->CreateTextLayout(TEXT(""),
		m_pTextFormat, 100.f, 100.f);

	// ��Ʈ �÷� ����
	m_pColor = GET_SINGLETON(FontManager)->CreateColor(m_vColor);
	// ��Ʈ �׸��� ����
	m_pShadowColor = GET_SINGLETON(FontManager)->CreateColor(m_vShadowColor);

	// �ؽ�Ʈ ���ڼ� 256��
	m_iMaxCount = 256;

	m_pText = new TCHAR[m_iMaxCount];

	memset(m_pText, 0, sizeof(TCHAR) * m_iMaxCount);

	// �⺻ 
	lstrcpy(m_pText, TEXT("Text"));

	// �ʱ�ȭ (�ؽ�Ʈ ������Ʈ ������ ����)
	m_tRenderArea = {};
}

UIText::UIText(const UIText & _Com) : Component(_Com)
{
	*this = _Com;
	m_iReferenceCount = 1;

	// �ؽ�Ʈ�� ������ TCHAR ���� �ʱ�ȭ
	m_pText = new TCHAR[m_iMaxCount];
	memcpy(m_pText, _Com.m_pText, sizeof(TCHAR) * m_iMaxCount);

	m_pFont = new TCHAR[64];
	memset(m_pFont, 0, sizeof(TCHAR) * 64);
	lstrcpy(m_pFont, _Com.m_pFont);
	m_pTextLayout = nullptr;
	CreateTextLayout();

	// �ʱ�ȭ (�ؽ�Ʈ ������Ʈ ������ ����)
	m_tRenderArea = {};
}

UIText::~UIText()
{

	SAFE_RELEASE(m_pTextLayout);
	SAFE_DELETE_ARRAY(m_pText);
	SAFE_DELETE_ARRAY(m_pFont);
}

void UIText::SetRenderType(TEXT_RENDER_TYPE _eType)
{
	m_eRenderType = _eType;
}

// ���� ���� �ø��� �Լ�
void UIText::SetTextCount(int _iMaxCount)
{
	// ���� ���� ����Ǿ��µ�, ���� ���ں��� ª�� ���
	if (m_iMaxCount >= _iMaxCount)
	{
		return;
	}

	// ������ �Ҵ�
	TCHAR*	pText = new TCHAR[_iMaxCount];
	// �ʱ�ȭ
	memset(pText, 0, sizeof(TCHAR) * _iMaxCount);

	// ���� ���� ���� ������ ������ ����
	memcpy(pText, m_pText, _iMaxCount);
	// ī��Ʈ �� ����
	m_iMaxCount = _iMaxCount;

	// ���� �� ����
	SAFE_DELETE_ARRAY(m_pText);
	// ���� ���� ������ �� ����
	m_pText = pText;
}

void UIText::SetText(const TCHAR *_pText)
{
	int iCount = lstrlen(_pText);

	// ���� �ؽ�Ʈ���� �� �� �ؽ�Ʈ�� ���� ���
	if (m_iMaxCount < iCount)
	{
		m_iMaxCount = iCount;

		// ���� ���� ����
		SAFE_DELETE_ARRAY(m_pText);

		// ���̸�ŭ ������ �Ҵ�
		m_pText = new TCHAR[m_iMaxCount];
	}

	memset(m_pText, 0, sizeof(TCHAR) * m_iMaxCount);
	lstrcpy(m_pText, _pText);
}

// �ؽ�Ʈ �̾���̱�
void UIText::AddText(const TCHAR *_pText)
{
	int iCount = lstrlen(_pText);
	int iCurCount = lstrlen(m_pText);

	// �̾���� �ؽ�Ʈ ���̺��� ���� MaxCount�� �� �������
	if (m_iMaxCount < iCount + iCurCount)
	{
		// �̾���� �ؽ�Ʈ ���� * 2 ��ŭ �÷��ش�.
		int iMaxCount = (iCount + iCurCount) * 2;

		TCHAR* pNewText = new TCHAR[iMaxCount];
		memset(pNewText, 0, sizeof(TCHAR) * iMaxCount);

		lstrcpy(pNewText, m_pText);

		SAFE_DELETE_ARRAY(m_pText);


		m_pText = pNewText;

		m_iMaxCount = iMaxCount;
	}

	lstrcat(m_pText, _pText);
}

void UIText::SetFont(const std::string& strName, const TCHAR* pFontName,
	int iWeight, int iStyle, int iStretch, float fSize,
	const TCHAR* pLocalName)
{
	lstrcpy(m_pFont, pFontName);
	m_fSize = fSize;

	GET_SINGLETON(FontManager)->CreateTextFormat(strName, pFontName,
		iWeight, iStyle, iStretch, fSize, pLocalName);

	m_pTextFormat = GET_SINGLETON(FontManager)->FindTextFormat(strName);

	CreateTextLayout();
}
void UIText::SetFont(const std::string & strName)
{
	m_pTextFormat = GET_SINGLETON(FontManager)->FindTextFormat(strName);

	CreateTextLayout();
}

void UIText::SetSize(float fSize)
{
	m_fSize = fSize;

	CreateTextLayout();
}

void UIText::SetAlignH(TEXT_ALIGN_H eAlign)
{
	m_eAlignH = eAlign;

	if (!m_pTextLayout)
		CreateTextLayout();

	switch (eAlign)
	{
	case TAH_LEFT:
		m_pTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		break;
	case TAH_CENTER:
		m_pTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		break;
	case TAH_RIGHT:
		m_pTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		break;
	}
}

void UIText::SetAlignV(TEXT_ALIGN_V eAlign)
{
	m_eAlignV = eAlign;

	if (!m_pTextLayout)
		CreateTextLayout();

	switch (eAlign)
	{
	case TAV_TOP:
		m_pTextLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		break;
	case TAV_MID:
		m_pTextLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		break;
	case TAV_BOTTOM:
		m_pTextLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
		break;
	}
}

void UIText::Shadow(bool bShadow)
{
	m_bShadow = bShadow;
}

void UIText::SetOffset(const Vector3 & vOffset)
{
	m_vOffset = vOffset;
}

void UIText::SetShadowOffset(const Vector3 & vShadowOffset)
{
	m_vShadowOffset = vShadowOffset;
}

void UIText::SetShadowColor(float r, float g, float b, float a)
{
	// ���ͷ� ������ �÷� �� (0 ~ 1)
	m_vShadowColor = Vector4(r, g, b, a);
	// �����ͷ� ������ �÷� ��
	m_pShadowColor = GET_SINGLETON(FontManager)->CreateColor(r, g, b, a);
}

void UIText::SetShadowColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	// ���ͷ� ������ �׸��� �÷� �� (0 ~ 1)
	m_vShadowColor = Vector4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);

	// �����ͷ� ������ �׸��� �÷� ��
	m_pShadowColor = GET_SINGLETON(FontManager)->CreateColor(r, g, b, a);
}
void UIText::SetShadowColor(const Vector4 & vColor)
{
	m_vShadowColor = vColor;

	m_pShadowColor = GET_SINGLETON(FontManager)->CreateColor(m_vShadowColor);
}
void UIText::SetShadowColor(unsigned int iColor)
{
	m_vShadowColor.b = (iColor & 0x000000ff) / 255.f;
	m_vShadowColor.g = ((iColor >> 8) & 0x000000ff) / 255.f;
	m_vShadowColor.r = ((iColor >> 16) & 0x000000ff) / 255.f;
	m_vShadowColor.a = ((iColor >> 24) & 0x000000ff) / 255.f;

	m_pShadowColor = GET_SINGLETON(FontManager)->CreateColor(iColor);
}

void UIText::AlphaBlend(bool bAlpha)
{
	m_bAlpha = bAlpha;
}

void UIText::ShadowAlphaBlend(bool bAlpha)
{
	m_bAlphaShadow = bAlpha;
}

void UIText::SetOpacity(float fOpacity)
{
	m_fOpacity = fOpacity;
}

void UIText::SetShadowOpacity(float fOpacity)
{
	m_fShadowOpacity = fOpacity;
}

void UIText::SetColor(float r, float g, float b, float a)
{
	m_vColor = Vector4(r, g, b, a);

	m_pColor = GET_SINGLETON(FontManager)->CreateColor(r, g, b, a);
}

void UIText::SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	m_vColor = Vector4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);

	m_pColor = GET_SINGLETON(FontManager)->CreateColor(r, g, b, a);
}

void UIText::SetColor(const Vector4 & vColor)
{
	m_vColor = vColor;

	m_pColor = GET_SINGLETON(FontManager)->CreateColor(m_vColor);
}

void UIText::SetColor(unsigned int iColor)
{
	m_vColor.b = (iColor & 0x000000ff) / 255.f;
	m_vColor.g = ((iColor >> 8) & 0x000000ff) / 255.f;
	m_vColor.r = ((iColor >> 16) & 0x000000ff) / 255.f;
	m_vColor.a = ((iColor >> 24) & 0x000000ff) / 255.f;

	m_pColor = GET_SINGLETON(FontManager)->CreateColor(iColor);
}

void UIText::SetRenderArea(float l, float t, float r, float b)
{
	// RESOLUTION => ���ϴ� 0, 0 / ���� 1, 1
	// RectF => ��, ��, ��, �� �Ű�����

	// D2D1 ������ y ��ǥ�� �ݴ��̹Ƿ� 2, 4��° �Ű������� �ݴ�� ����־��־�� �Ѵ�.
	m_tRenderArea = D2D1::RectF(l, _RESOLUTION.iHeight - b,
		r, _RESOLUTION.iHeight - t);

	CreateTextLayout();
}

void UIText::CreateTextLayout()
{
	// ���� ������ �ؽ�Ʈ ��Ʈ�� �������� ���� ���
	if (!m_pTextFormat)
	{
		return;
	}

	// ������ �ؽ�Ʈ ���̾ƿ� ����
	SAFE_RELEASE(m_pTextLayout);
	// �ؽ�Ʈ ���̾ƿ� ����
	// 1. �ؽ�Ʈ
	// 2. �ؽ�Ʈ ����
	// 3. ���� �ʺ�
	// 4. ���� �ʺ�
	m_pTextLayout = GET_SINGLETON(FontManager)->CreateTextLayout(
		m_pText, m_pTextFormat, m_tRenderArea.right - m_tRenderArea.left,
		m_tRenderArea.bottom - m_tRenderArea.top);

	// �ؽ�Ʈ ���̿� ���� ���� (0��°���� �ش� �ؽ�Ʈ ��ü ���̱���)
	DWRITE_TEXT_RANGE	tRange;
	tRange.startPosition = 0;
	tRange.length = lstrlen(m_pText);

	// ���̾ƿ��� �ؽ�Ʈ���� �� �ؽ�Ʈ ������ ����
	m_pTextLayout->SetFontSize(m_fSize, tRange);
}

void UIText::Start()
{
	if (m_eRenderType == TRT_UI)
		m_pGameObject->SetRenderGroup(RG_UI);
}

bool UIText::Init()
{
	return true;
}

int UIText::Input(float fTime)
{
	return 0;
}

int UIText::Update(float fTime)
{
	return 0;
}

int UIText::LateUpdate(float fTime)
{
	return 0;
}

int UIText::Collision(float fTime)
{
	return 0;
}

int UIText::PrevRender(float fTime)
{
	return 0;
}


int UIText::Render(float fTime)
{
	// �׸��� ȣ���� BeginDraw ȣ��� EndDraw ȣ�� ���̿����� ���� �� �� �ֽ��ϴ�.
	m_2DTarget->BeginDraw();

	Vector3 vPos = m_pTransform->GetWorldPosition();

	float fHeight = m_tRenderArea.bottom - m_tRenderArea.top;

	// UI�� �ƴ� ����ī�޶� ����޴� Ÿ���� ���
	// ���� ī�޶��� �������־�� �Ѵ�.
	if (m_eRenderType != TRT_UI)
	{
		// ���� ī�޶� Transform�� ���´�.
		Transform* pCameraTr = m_pScene->GetMainCameraTransform();

		// �ش� ������Ʈ�� ī�޶� ����ŭ ���� Ŭ���̾�Ʈ ���� ���� �����ϴ°�ó�� �����Ų��.
		vPos -= pCameraTr->GetWorldPosition();
		SAFE_RELEASE(pCameraTr);
	}

	// ��Ʈ�� �׸��ڰ� �����ϴ� ���
	if (m_bShadow)
	{
		// �׸��ڴ� ���� ���� ��Ʈ�� ������ ����ŭ ��������
		// ��ġ �׸���ó�� ���̰Բ� �ϴ°��̹Ƿ�...
		Vector3 vShadowPos = vPos + m_vShadowOffset;

		// �׸��ڿ� ���İ��� �����ϴ� ���
		if (m_bAlphaShadow)
		{
			// �ش� ���� ��ŭ �������ش� (0 ~ 1)
			m_pShadowColor->SetOpacity(m_fShadowOpacity);
		}
		else
		{
			// ������
			m_pShadowColor->SetOpacity(1.f);
		}

		// �׸��ڰ� ���� ��Ʈ���� ���� ������
		// �ش� ������ ������ ���� ��Ʈ���� �Ʒ��� �� ���̴�.
		vShadowPos.y = _RESOLUTION.iHeight - vShadowPos.y - fHeight;
		// �׸��ڸ� �׸���.
		// 1. �׸��� ��� ��ġ
		// 2. �׸��� ���̾ƿ�
		// 3. �׸��� �÷�
		m_2DTarget->DrawTextLayout(D2D1::Point2F(vShadowPos.x, vShadowPos.y),
			m_pTextLayout, m_pShadowColor);
	}

	// ��Ʈ�� ���İ� �����ϴ� ���
	if (m_bAlpha)
	{
		// ���İ� ����
		m_pColor->SetOpacity(m_fOpacity);
	}
	else
	{
		// ������
		m_pColor->SetOpacity(1.f);
	}
	vPos.y = _RESOLUTION.iHeight - vPos.y - fHeight;

	// �ؽ�Ʈ�� �׸���.
	// 1. �ؽ�Ʈ ��� ��ġ
	// 2. �ؽ�Ʈ ���̾ƿ�
	// 3. �ؽ�Ʈ �÷�
	m_2DTarget->DrawTextLayout(D2D1::Point2F(vPos.x, vPos.y),
		m_pTextLayout, m_pColor);

	m_2DTarget->EndDraw();

	return 0;
}

UIText * UIText::Clone() const
{
	return new UIText(*this);
}
