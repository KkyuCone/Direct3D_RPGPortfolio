#include "UIImage.h"
#include "Renderer.h"
#include "Material.h"
#include "Transform.h"
#include "..\GameObject.h"
#include "..\Resource\Texture.h"
#include "..\Resource\ResourcesManager.h"
#include "..\Render\ShaderManager.h"
#include "..\InputManager.h"

ENGINE_USING

UIImage::UIImage()
{
	m_tImageCBuffer.vColor = Vector4::White;
	m_pRenderer = nullptr;
	m_pMaterial = nullptr;
	m_pTexture = nullptr;
	m_CallBackFunc = nullptr;
}

UIImage::UIImage(const UIImage & _Com)
{
	m_CallBackFunc = nullptr;
	m_pTexture = _Com.m_pTexture;

	if (nullptr != m_pTexture)
	{
		m_pTexture->AddReference();
	}

	m_vColor = _Com.m_vColor;
	m_pMaterial = nullptr;
}


UIImage::~UIImage()
{
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pMaterial);
	SAFE_RELEASE(m_pRenderer);
}

void UIImage::SetColor(const Vector4 & _vColor)
{
	m_vColor = _vColor;
}

void UIImage::SetTexture(const std::string & _strName, const TCHAR * _pFileName, const std::string & _strPathName)
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

void UIImage::SetShader(const std::string & _strKey)
{
	m_pRenderer->SetShader(_strKey);
}

void UIImage::SetBlendState(const std::string & _strKey)
{
	m_pRenderer->SetRenderState("AlphaBlend");
}

void UIImage::Start()
{
	SAFE_RELEASE(m_pMaterial);
	m_pMaterial = FindComponentFromType<Material>(CT_MATERIAL);

	if (nullptr != m_pTexture)
	{
		m_pMaterial->DeleteTextureSet(0, 0);
		m_pMaterial->AddTextureSet(0, 0, 0, m_pTexture);
	}
}

bool UIImage::Init()
{
	if (false == UI::Init())
	{
		return false;
	}

	// 얘도 버튼처럼 디폴트값 주기
	DefaultInit();
	return true;
}

int UIImage::Input(float _fTime)
{
	return 0;
}

int UIImage::Update(float _fTime)
{
	if (nullptr != m_CallBackFunc)
	{
		m_CallBackFunc(_fTime);
	}

	return 0;
}

int UIImage::LateUpdate(float _fTime)
{
	return 0;
}

int UIImage::Collision(float _fTime)
{
	return 0;
}

int UIImage::PrevRender(float _fTime)
{
	// 상수버퍼애들 업뎃
	m_tImageCBuffer.vColor = m_vColor;
	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("UI_Image", &m_tImageCBuffer);
	return 0;
}
int UIImage::Render(float _fTime)
{
	return 0;
}

UIImage * UIImage::Clone() const
{
	return new UIImage(*this);
}

void UIImage::DefaultInit()
{
	m_pTransform->SetWorldScale(100.0f, 100.0f, 1.0f);

	m_pRenderer = AddComponent<Renderer>("ImageRenderer");
	m_pRenderer->SetMesh("Rectangle");
	m_pRenderer->SetRenderState("AlphaBlend");
	m_pRenderer->SetShader(SHADER_UI_IMAGE);

	m_pMaterial = AddComponent<Material>("ImageMaterial");
	m_pMaterial->AddTextureSet(0, 0, 0, "ImageDefault", TEXT("DefaultImage.png"), PATH_UI_IMAGE);
}

void UIImage::SetCallBackFunc(void(*_pFunc)(float))
{
	m_CallBackFunc = std::bind(_pFunc, std::placeholders::_1);
}
