#pragma once

#include "Engine.h"

ENGINE_BEGIN

class ENGINE_DLL FontManager
{
private:
	// IDWriteFactory 인터페이스는 DirectWrite를 사용해
	// 텍스트 드로잉을 하기 위해 반드시 필요한 인터페이스이다.
	IDWriteFactory* m_pWriteFactory;
	std::unordered_map<std::string, IDWriteTextFormat*> m_mapFont;
	std::unordered_map<unsigned int, ID2D1SolidColorBrush*> m_mapColor;

public:
	bool Init();
	bool CreateTextFormat(const std::string& strName, const TCHAR* pFontName,
		int iWeight, int iStyle, int iStretch, float fSize,
		const TCHAR* pLocalName);
	ID2D1SolidColorBrush* CreateColor(float r, float g, float b, float a);
	ID2D1SolidColorBrush* CreateColor(unsigned char r, unsigned char g, unsigned char b,
		unsigned char a);
	ID2D1SolidColorBrush* CreateColor(const Vector4& vColor);
	ID2D1SolidColorBrush* CreateColor(unsigned int iColor);
	IDWriteTextLayout* CreateTextLayout(const TCHAR* pText,
		IDWriteTextFormat* pFormat, float fWidth, float fHeight);
	IDWriteTextLayout* CreateTextLayout(const TCHAR* pText,
		const std::string& strFontKey, float fWidth, float fHeight);

	IDWriteTextFormat* FindTextFormat(const std::string& strName);
	ID2D1SolidColorBrush* FindColor(float r, float g, float b, float a);
	ID2D1SolidColorBrush* FindColor(unsigned char r, unsigned char g, unsigned char b,
		unsigned char a);
	ID2D1SolidColorBrush* FindColor(const Vector4& vColor);
	ID2D1SolidColorBrush* FindColor(unsigned int iColor);
	unsigned int CreateColorKey(float r, float g, float b, float a);
	unsigned int CreateColorKey(unsigned char r, unsigned char g, unsigned char b,
		unsigned char a);
	unsigned int CreateColorKey(const Vector4& vColor);


	DECLARE_SINGLETON(FontManager)
};

ENGINE_END
