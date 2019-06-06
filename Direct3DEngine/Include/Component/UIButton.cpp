#include "UIButton.h"
#include "Renderer.h"
#include "Material.h"
#include "Transform.h"
#include "..\GameObject.h"
#include "..\Resource\Texture.h"
#include "..\Resource\ResourcesManager.h"
#include "..\Render\ShaderManager.h"
#include "..\InputManager.h"

ENGINE_USING

UIButton::UIButton()
{
	m_tButtonCBuffer.vColor = Vector4::Black;
	m_pRenderer = nullptr;

	m_pMaterial = nullptr;
	m_eState = BS_NORMAL;
	m_ePrevState = BS_NORMAL;

	m_pTexture[BS_DISABLE] = nullptr;
	m_pTexture[BS_NORMAL] = nullptr;
	m_pTexture[BS_MOUSEON] = nullptr;
	m_pTexture[BS_CLICK] = nullptr;

	m_vColor[BS_DISABLE] = Vector4(0.2f, 0.2f, 0.2f, 1.0f);			//
	m_vColor[BS_NORMAL] = Vector4(0.8f, 0.8f, 0.8f, 1.0f);			// 기본상태
	m_vColor[BS_MOUSEON] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);			// 마우스와 충돌
	m_vColor[BS_CLICK] = Vector4(1.0f, 1.0f, 0.0f, 1.0f);;			// 눌렀을때
}

UIButton::UIButton(const UIButton & _Com) : UI(_Com)
{
	m_eState = _Com.m_eState;
	m_ePrevState = _Com.m_ePrevState;

	for (int i = 0; i < BS_END; ++i)
	{
		m_CallBackFunc[i] = nullptr;
		m_pTexture[i] = _Com.m_pTexture[i];

		if (nullptr != m_pTexture[i])
		{
			m_pTexture[i]->AddReference();
		}

		m_vColor[i] = _Com.m_vColor[i];
	}

	m_pMaterial = nullptr;
	//m_bStartTextureChange = true;
}


UIButton::~UIButton()
{
	for (int i = 0; i < BS_END; ++i)
	{
		SAFE_RELEASE(m_pTexture[i]);
	}

	SAFE_RELEASE(m_pMaterial);
	SAFE_RELEASE(m_pRenderer);
}

void UIButton::SetStateColor(BUTTON_STATE _eState, const Vector4 & _vColor)
{
	m_vColor[_eState] = _vColor;
}

void UIButton::SetStateTexture(BUTTON_STATE _eState, const std::string & _strName,
	const TCHAR * _pFileName, const std::string & _strPathName)
{
	SAFE_RELEASE(m_pTexture[_eState]);

	if (false == GET_SINGLETON(ResourcesManager)->LoadTexture(_strName,
		_pFileName, _strPathName))
	{
		return;
	}

	m_pTexture[_eState] = GET_SINGLETON(ResourcesManager)->FindTexture(_strName);

	if (_eState == m_eState && nullptr != m_pMaterial)
	{
		m_pMaterial->DeleteTextureSet(0, 0);
		m_pMaterial->AddTextureSet(0, 0, 0, m_pTexture[m_eState]);
	}
}

void UIButton::Start()
{
	SAFE_RELEASE(m_pMaterial);
	m_pMaterial = FindComponentFromType<Material>(CT_MATERIAL);

	if (nullptr != m_pTexture[m_eState])
	{
		m_pMaterial->DeleteTextureSet(0, 0);
		m_pMaterial->AddTextureSet(0, 0, 0, m_pTexture[m_eState]);
	}
}

bool UIButton::Init()
{
	if (false == UI::Init())
	{
		return false;
	}

	m_pTransform->SetWorldScale(200.0f, 100.0f, 1.0f);

	m_pRenderer = AddComponent<Renderer>("ButtonRenderer");
	m_pRenderer->SetMesh("Rectangle");
	m_pRenderer->SetRenderState("AlphaBlend");
	m_pRenderer->SetShader(SHADER_UI_BUTTON);

	m_pMaterial = AddComponent<Material>("ButtonMaterial");
	m_pMaterial->AddTextureSet(0, 0, 0, "ButtonDefaultNormal", TEXT("UIDefault.png"), PATH_UI_BUTTON);

	return true;
}

int UIButton::Input(float _fTime)
{
	return 0;
}

int UIButton::Update(float _fTime)
{
	if (BS_DISABLE != m_eState)
	{
		m_ePrevState = m_eState;

		Vector2 vMousePos = GET_SINGLETON(InputManager)->GetUIMouseViewportPos();
		Vector3 vPos = m_pTransform->GetWorldPosition();
		Vector3 vScale = m_pTransform->GetWorldScale();

		if (vPos.x <= vMousePos.x && vMousePos.x <= vPos.x + vScale.x &&
			vPos.y <= vMousePos.y && vMousePos.y <= vPos.y + vScale.y)
		{
			if (true == GET_SINGLETON(InputManager)->MousePress(MB_LBUTTON))
			{
				m_eState = BS_CLICK;
			}
			else if (true == GET_SINGLETON(InputManager)->MousePush(MB_LBUTTON))
			{
				if (m_eState != BS_CLICK)
				{
					m_eState = BS_MOUSEON;
				}
			}
			else if (true == GET_SINGLETON(InputManager)->MouseRelease(MB_LBUTTON))
			{
				m_CallBackFunc[BS_CLICK](_fTime);
			}
			else
			{
				m_eState = BS_MOUSEON;                                                                                                                                               
			}
		}
		else
		{
			m_eState = BS_NORMAL;
		}
	}

	if (nullptr != m_CallBackFunc[m_eState] && BS_CLICK != m_eState)
	{
		m_CallBackFunc[m_eState](_fTime);
	}

	return 0;
}

int UIButton::LateUpdate(float _fTime)
{
	return 0;
}

int UIButton::Collision(float _fTime)
{
	return 0;
}

int UIButton::PrevRender(float _fTime)
{
	m_tButtonCBuffer.vColor = m_vColor[m_eState];

	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("UI_Button", &m_tButtonCBuffer);

	if (m_ePrevState != m_eState && nullptr != m_pTexture[m_eState])
	{
		m_pMaterial->DeleteTextureSet(0, 0);
		m_pMaterial->AddTextureSet(0, 0, 0, m_pTexture[m_eState]);
	}

	return 0;
}

int UIButton::Render(float _fTime)
{
	return 0;
}

UIButton * UIButton::Clone() const
{
	return new UIButton(*this);
}

void UIButton::SetCallBackFunc(BUTTON_STATE _eState, void(*_pFunc)(float))
{
	m_CallBackFunc[_eState] = std::bind(_pFunc, std::placeholders::_1);
}
