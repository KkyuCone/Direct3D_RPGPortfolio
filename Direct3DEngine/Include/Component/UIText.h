#pragma once
#include "Component.h"

ENGINE_BEGIN

// 가로 정렬
enum TEXT_ALIGN_H
{
	TAH_LEFT,
	TAH_CENTER,
	TAH_RIGHT
};

// 세로 정렬
enum TEXT_ALIGN_V
{
	TAV_TOP,
	TAV_MID,
	TAV_BOTTOM
};

enum TEXT_RENDER_TYPE
{
	TRT_UI,
	TRT_2D,
	TRT_3D
};

class ENGINE_DLL UIText :
	public Component
{
private:
	friend class GameObject;

protected:
	UIText();
	UIText(const UIText& _Com);
	virtual ~UIText();

private:
	IDWriteTextFormat* m_pTextFormat;
	IDWriteTextLayout* m_pTextLayout;

	ID2D1SolidColorBrush* m_pColor;
	ID2D1SolidColorBrush* m_pShadowColor; // 그림자용

										  // 텍스트 오프셋 (YDJ)
	Vector3 m_vOffset;

	// 랜더 타겟
	ID2D1RenderTarget* m_2DTarget;

	// 그림자 사용여부
	bool	m_bShadow;

	// 텍스트
	TCHAR* m_pText;

	// 텍스트 총 길이
	int m_iMaxCount;

	// 폰트 명
	TCHAR* m_pFont;

	// 폰트 크기
	float m_fSize;

	// 가로 정렬 기준
	TEXT_ALIGN_H m_eAlignH;
	// 세로 정렬 기준
	TEXT_ALIGN_V m_eAlignV;

	// 랜더 타입(2D, 3D, UI) 
	TEXT_RENDER_TYPE m_eRenderType;

	// 알파 적용 여부
	bool m_bAlpha;
	// 그림자 알파 적용 여부
	bool m_bAlphaShadow;

	// 투명도
	float m_fOpacity;

	// 그림자 투명도
	float m_fShadowOpacity;

	// 폰트 색상
	Vector4 m_vColor;
	// 그림자 색상
	Vector4 m_vShadowColor;

	// 그림자 오프셋
	Vector3 m_vShadowOffset;

	// 폰트를 그려낼 영역
	D2D1_RECT_F m_tRenderArea;

public:
	void SetRenderType(TEXT_RENDER_TYPE _eType);
	void SetTextCount(int _iMaxCount);
	void SetText(const TCHAR* _pText);
	void AddText(const TCHAR* _pText);

	void SetFont(const std::string& _strName, const TCHAR* _pFontName,
		int _iWeight, int _iStyle, int _iStretch, float _fSize,
		const TCHAR* _pLocalName);

	void SetFont(const std::string& _strName);
	void SetSize(float _fSize);
	void SetAlignH(TEXT_ALIGN_H _eAlign);
	void SetAlignV(TEXT_ALIGN_V _eAlign);
	void Shadow(bool _bShadow);
	void SetOffset(const Vector3& _vOffset); // YDJ
	void SetShadowOffset(const Vector3& _vShadowOffset);
	void SetShadowColor(float _r, float _g, float _b, float _a);
	void SetShadowColor(unsigned char _r, unsigned char _g,
		unsigned char _b, unsigned char _a);
	void SetShadowColor(const Vector4& _vColor);
	void SetShadowColor(unsigned int _iColor);
	void AlphaBlend(bool _bAlpha);
	void ShadowAlphaBlend(bool _bAlpha);
	void SetOpacity(float _fOpacity);
	void SetShadowOpacity(float _fOpacity);
	void SetColor(float _r, float _g, float _b, float _a);
	void SetColor(unsigned char _r, unsigned char _g,
		unsigned char _b, unsigned char _a);
	void SetColor(const Vector4& _vColor);
	void SetColor(unsigned int _iColor);
	void SetRenderArea(float _l, float _t, float _r, float _b);

private:
	void CreateTextLayout();

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);
	virtual int Render(float _fTime);
	virtual UIText* Clone()	const;
};

ENGINE_END
