#include "FontManager.h"
#include "Device.h"

ENGINE_USING

DEFINITION_SINGLETON(FontManager)

FontManager::FontManager() :
	m_pWriteFactory(nullptr)
{
}

FontManager::~FontManager()
{
	Safe_Release_Map(m_mapFont);
	Safe_Release_Map(m_mapColor);
	SAFE_RELEASE(m_pWriteFactory);
}

bool FontManager::Init()
{
	// __uuidof: �Ű������� ����� GUID�� ã�� �Լ�
	if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(m_pWriteFactory), (IUnknown**)&m_pWriteFactory)))
	{
		return false;
	}

	CreateTextFormat("Gungseo", TEXT("�ü�ü"),
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_ULTRA_EXPANDED,
		20.f, TEXT("ko"));

	CreateTextFormat("FreshGothic", TEXT("���� ���"),
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_ULTRA_EXPANDED,
		20.f, TEXT("ko"));

	return true;
}

bool FontManager::CreateTextFormat(const std::string & strName,
	const TCHAR * pFontName, int iWeight, int iStyle,
	int iStretch, float fSize, const TCHAR * pLocalName)
{
	IDWriteTextFormat*	pFont = FindTextFormat(strName);

	if (pFont)
	{
		return false;
	}

	// Text Format�� �����Ѵ�.
	// 1������ : ��Ʈ �̸��� �Է��Ѵ�. 
	// 2������ : Ư�� ��Ʈ�� �÷����� ������ �ִ�. �� Arial �� Arial Black �̷���
	// 3������ : ��Ʈ ���� 4������ : ����
	// 5������ : �ڰ� 6������ : ��Ʈ ũ��
	// 7������ : ��� ���� �̸��� �����Ѵ�. �ѱ��� ko - KR �̱��� en - us 
	// 8������ : �ؽ�Ʈ �������̽�
	if (FAILED(m_pWriteFactory->CreateTextFormat(pFontName,
		nullptr,
		(DWRITE_FONT_WEIGHT)iWeight,
		(DWRITE_FONT_STYLE)iStyle,
		(DWRITE_FONT_STRETCH)iStretch,
		fSize, pLocalName, &pFont)))
		return false;

	m_mapFont.insert(make_pair(strName, pFont));

	return true;
}

ID2D1SolidColorBrush* FontManager::CreateColor(float r, float g, float b, float a)
{
	// RGB �� ����
	ID2D1SolidColorBrush* pColor = FindColor(r, g, b, a);

	if (pColor)
	{
		return pColor;
	}

	// 0 ~ 1 ������ ���� �������� �̿��ؾ� �Ѵ�.
	if (FAILED(RENDERTARGET2D->CreateSolidColorBrush(
		D2D1::ColorF(r, g, b, a), &pColor)))
	{
		return nullptr;
	}
	m_mapColor.insert(std::make_pair(CreateColorKey(r, g, b, a), pColor));

	return pColor;
}

ID2D1SolidColorBrush* FontManager::CreateColor(unsigned char r, unsigned char g,
	unsigned char b, unsigned char a)
{
	// �÷����� �޾ƿ´�.
	ID2D1SolidColorBrush* pColor = FindColor(r, g, b, a);

	// �÷����� �����ϴ� ���
	if (pColor)
	{
		return pColor;
	}

	// 0 ~ 1 ������ ���� �������� �̿��ؾ� �Ѵ�.
	if (FAILED(RENDERTARGET2D->CreateSolidColorBrush(
		D2D1::ColorF(r / 255.f, g / 255.f, b / 255.f, a / 255.f),
		&pColor)))
	{
		return nullptr;
	}

	m_mapColor.insert(std::make_pair(CreateColorKey(r, g, b, a), pColor));

	return pColor;
}

ID2D1SolidColorBrush* FontManager::CreateColor(const Vector4 & vColor)
{
	ID2D1SolidColorBrush* pColor = FindColor(vColor);

	if (pColor)
	{
		return pColor;
	}

	// 0 ~ 1 ������ ���� �������� �̿��ؾ� �Ѵ�.
	if (FAILED(RENDERTARGET2D->CreateSolidColorBrush(
		D2D1::ColorF(vColor.r, vColor.g, vColor.b, vColor.a),
		&pColor)))
	{
		return nullptr;
	}

	m_mapColor.insert(std::make_pair(CreateColorKey(vColor), pColor));

	return pColor;
}

ID2D1SolidColorBrush* FontManager::CreateColor(unsigned int iColor)
{
	ID2D1SolidColorBrush* pColor = FindColor(iColor);

	if (pColor)
	{
		return pColor;
	}

	float	r, g, b, a;

	// 8��Ʈ������ int �� / 255.f (0 ~ 1 ������ ��)
	b = (iColor & 0x000000ff) / 255.f;
	g = ((iColor >> 8) & 0x000000ff) / 255.f;
	r = ((iColor >> 16) & 0x000000ff) / 255.f;
	a = ((iColor >> 24) & 0x000000ff) / 255.f;

	// 0 ~ 1 ������ ���� �������� �̿��ؾ� �Ѵ�.
	if (FAILED(RENDERTARGET2D->CreateSolidColorBrush(D2D1::ColorF(r, g, b, a), &pColor)))
	{
		return nullptr;
	}

	m_mapColor.insert(std::make_pair(iColor, pColor));

	return pColor;
}

IDWriteTextLayout * FontManager::CreateTextLayout(
	const TCHAR * pText, IDWriteTextFormat * pFormat,
	float fWidth, float fHeight)
{
	IDWriteTextLayout* pLayout = nullptr;

	// ���̾ƿ� ����
	if (FAILED(m_pWriteFactory->CreateTextLayout(pText, lstrlen(pText),
		pFormat, fWidth, fHeight, &pLayout)))
	{
		return nullptr;
	}
	return pLayout;
}

IDWriteTextLayout * FontManager::CreateTextLayout(
	const TCHAR * pText, const std::string & strFontKey,
	float fWidth, float fHeight)
{
	IDWriteTextFormat*	pFormat = FindTextFormat(strFontKey);

	if (!pFormat)
	{
		return nullptr;
	}

	IDWriteTextLayout*	pLayout = nullptr;

	if (FAILED(m_pWriteFactory->CreateTextLayout(pText, lstrlen(pText),
		pFormat, fWidth, fHeight, &pLayout)))
	{
		return nullptr;
	}

	return pLayout;
}

IDWriteTextFormat * FontManager::FindTextFormat(const std::string & strName)
{
	std::unordered_map<std::string, IDWriteTextFormat*>::iterator	iter = m_mapFont.find(strName);

	if (iter == m_mapFont.end())
		return nullptr;

	return iter->second;
}

ID2D1SolidColorBrush * FontManager::FindColor(float r, float g,
	float b, float a)
{
	unsigned int	iColor = 0;

	unsigned char	r1, g1, b1, a1;

	// 0 - 255 unsigned Char�� �̿�
	r1 = (unsigned char)(r * 255);
	g1 = (unsigned char)(g * 255);
	b1 = (unsigned char)(b * 255);
	a1 = (unsigned char)(a * 255);

	// 8 ��Ʈ �� ����Ʈ ������ ���� 
	// 4Byte Unsigned int�� ���� �����Ѵ�.
	iColor = a1;
	iColor = (iColor << 8) | r1;
	iColor = (iColor << 8) | g1;
	iColor = (iColor << 8) | b1;

	return FindColor(iColor);
}

ID2D1SolidColorBrush * FontManager::FindColor(unsigned char r,
	unsigned char g, unsigned char b, unsigned char a)
{
	unsigned int	iColor = 0;

	iColor = a;
	iColor = (iColor << 8) | r;
	iColor = (iColor << 8) | g;
	iColor = (iColor << 8) | b;

	return FindColor(iColor);
}

ID2D1SolidColorBrush * FontManager::FindColor(const Vector4 & vColor)
{
	unsigned int	iColor = 0;
	unsigned char	r, g, b, a;
	r = (unsigned char)(vColor.r * 255);
	g = (unsigned char)(vColor.g * 255);
	b = (unsigned char)(vColor.b * 255);
	a = (unsigned char)(vColor.a * 255);

	iColor = a;
	iColor = (iColor << 8) | r;
	iColor = (iColor << 8) | g;
	iColor = (iColor << 8) | b;

	return FindColor(iColor);
}

ID2D1SolidColorBrush * FontManager::FindColor(unsigned int iColor)
{
	std::unordered_map<unsigned int, ID2D1SolidColorBrush*>::iterator	iter = m_mapColor.find(iColor);

	if (iter == m_mapColor.end())
	{
		return nullptr;
	}

	return iter->second;
}

unsigned int FontManager::CreateColorKey(float r, float g,
	float b, float a)
{
	unsigned int	iColor = 0;
	unsigned char	r1, g1, b1, a1;
	r1 = (unsigned char)(r * 255);
	g1 = (unsigned char)(g * 255);
	b1 = (unsigned char)(b * 255);
	a1 = (unsigned char)(a * 255);

	iColor = a1;
	iColor = (iColor << 8) | r1;
	iColor = (iColor << 8) | g1;
	iColor = (iColor << 8) | b1;

	return iColor;
}

unsigned int FontManager::CreateColorKey(unsigned char r,
	unsigned char g, unsigned char b, unsigned char a)
{
	unsigned int	iColor = 0;

	iColor = a;
	iColor = (iColor << 8) | r;
	iColor = (iColor << 8) | g;
	iColor = (iColor << 8) | b;

	return iColor;
}

unsigned int FontManager::CreateColorKey(const Vector4 & vColor)
{
	unsigned int	iColor = 0;
	unsigned char	r, g, b, a;
	r = (unsigned char)(vColor.r * 255);
	g = (unsigned char)(vColor.g * 255);
	b = (unsigned char)(vColor.b * 255);
	a = (unsigned char)(vColor.a * 255);

	iColor = a;
	iColor = (iColor << 8) | r;
	iColor = (iColor << 8) | g;
	iColor = (iColor << 8) | b;

	return iColor;
}
