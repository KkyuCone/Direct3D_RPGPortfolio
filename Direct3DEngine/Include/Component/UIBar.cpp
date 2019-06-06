#include "UIBar.h"
#include "Renderer.h"
#include "Material.h"
#include "Transform.h"
#include "..\GameObject.h"
#include "..\Resource\Texture.h"
#include "..\Resource\ResourcesManager.h"
#include "..\Render\ShaderManager.h"

ENGINE_USING

UIBar::UIBar() :
	m_fRatio(1.0f),
	m_vColor(Vector4::White),
	m_pTexture(nullptr),
	m_pMaterial(nullptr),
	m_pRenderer(nullptr)
{
	m_tBarCBuffer.fRatio = 1.0f;
	m_tBarCBuffer.vColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	m_tBarCBuffer.vEmpty = Vector3(0.0f, 0.0f, 0.0f);
}


UIBar::UIBar(const UIBar & _Com) : UI(_Com)
{
	m_vColor = _Com.m_vColor;

	if (nullptr != _Com.m_pTexture)
	{
		m_pTexture = _Com.m_pTexture;
		m_pTexture->AddReference();
	}

	m_tBarCBuffer.fRatio = 1.0f;
	m_tBarCBuffer.vColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	m_tBarCBuffer.vEmpty = Vector3(0.0f, 0.0f, 0.0f);

	m_pMaterial = nullptr;
}

UIBar::~UIBar()
{
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pMaterial);
	SAFE_RELEASE(m_pRenderer);
}

void UIBar::SetColor(const Vector4 & _vColor)
{
	m_vColor = _vColor;
}

void UIBar::SetRatio(const float & _fRatio)
{
	m_fRatio = _fRatio;
}

void UIBar::SetTexture(const std::string &_strName, const TCHAR * _pFileName, const std::string & _strPathName)
{
	SAFE_RELEASE(m_pTexture);

	if (false == GET_SINGLETON(ResourcesManager)->LoadTexture(_strName,
		_pFileName, _strPathName))
	{
		return;
	}

	m_pTexture = GET_SINGLETON(ResourcesManager)->FindTexture(_strName);

	if (nullptr != m_pMaterial)
	{
		m_pMaterial->DeleteTextureSet(0, 0);
		m_pMaterial->AddTextureSet(0, 0, 0, m_pTexture);
	}
}

void UIBar::SetShader(const std::string & _strKey)
{
	m_pRenderer->SetShader(_strKey);
}

void UIBar::Start()
{
	SAFE_RELEASE(m_pMaterial);
	m_pMaterial = FindComponentFromType<Material>(CT_MATERIAL);

	if (nullptr != m_pTexture)
	{
		m_pMaterial->DeleteTextureSet(0, 0);
		m_pMaterial->AddTextureSet(0, 0, 0, m_pTexture);
	}
}

bool UIBar::Init()
{
	if (false == UI::Init())
	{
		return false;
	}

	DefaultBarInit();
	return true;
}

int UIBar::Input(float _fTime)
{
	return 0;
}

int UIBar::Update(float _fTime)
{
	return 0;
}

int UIBar::LateUpdate(float _fTime)
{
	return 0;
}

int UIBar::Collision(float _fTime)
{
	return 0;
}

int UIBar::PrevRender(float _fTime)
{
	m_tBarCBuffer.vColor = m_vColor;
	m_tBarCBuffer.fRatio = m_fRatio;

	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("UI_Bar", &m_tBarCBuffer);

	if (nullptr != m_pTexture)
	{
		m_pMaterial->DeleteTextureSet(0, 0);
		m_pMaterial->AddTextureSet(0, 0, 0, m_pTexture);
	}

	return 0;
}

int UIBar::Render(float _fTime)
{
	return 0;
}

UIBar * UIBar::Clone() const
{
	return new UIBar(*this);
}

void UIBar::DefaultBarInit()
{
	m_pTransform->SetWorldScale(500.0f, 100.0f, 1.0f);

	m_pRenderer = AddComponent<Renderer>("BarRenderer");
	m_pRenderer->SetMesh("Rectangle");
	m_pRenderer->SetRenderState("AlphaBlend");
	m_pRenderer->SetShader(SHADER_UI_BAR);

	m_pMaterial = AddComponent<Material>("BarMaterial");
	m_pMaterial->AddTextureSet(0, 0, 0, "BarDefault", TEXT("HP.png"), PATH_UI_BAR);
}
