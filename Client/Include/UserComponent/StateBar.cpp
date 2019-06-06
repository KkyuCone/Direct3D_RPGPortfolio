#include "StateBar.h"

StateBar::StateBar() :
	m_fRatio(1.0f),
	m_StateBar(nullptr),
	m_StateText(nullptr)
{
	m_State.m_iCur = 0;
	m_State.m_iMax = 0;
	m_State.m_iMin = 0;
}

StateBar::StateBar(const StateBar & _StateBar)
{
	m_fRatio = 1.0f;
	m_State.m_iCur = _StateBar.m_State.m_iCur;
	m_State.m_iMax = _StateBar.m_State.m_iMax;
	m_State.m_iMin = _StateBar.m_State.m_iMin;

	m_StateBar = nullptr;
	m_StateText = nullptr;
}


StateBar::~StateBar()
{
	SAFE_RELEASE(m_StateBar);
	SAFE_RELEASE(m_StateText);
}

UIBar * StateBar::GetUIBar()
{
	return m_StateBar;
}

UIText * StateBar::GetStateText()
{
	return m_StateText;
}

void StateBar::SetMin(int _iMin)
{
	m_State.m_iMin = _iMin;
}

void StateBar::SetMax(int _iMax)
{
	m_State.m_iMax = _iMax;
}

void StateBar::SetUIBar(UIBar * _Bar)
{
	m_StateBar = _Bar;
}

void StateBar::SetUIText(UIText * _Text)
{
	m_StateText = _Text;
}

void StateBar::SetColor(Vector4 _vColor)
{
	m_StateBar->SetColor(_vColor);
}

void StateBar::SetChangeBarShader(std::string & _strKey)
{
	m_StateBar->SetShader(_strKey);
}

void StateBar::SetStateBarTexture(const std::string & _strName, const TCHAR * _pFileName, const std::string & _strPathName)
{
	m_StateBar->SetTexture(_strName, _pFileName, _strPathName);
}

void StateBar::Start()
{
}

bool StateBar::Init()
{
	return true;
}

int StateBar::Input(float _fTime)
{
	return 0;
}

int StateBar::Update(float _fTime)
{
	return 0;
}

int StateBar::LateUpdate(float _fTime)
{
	CheckRatio();			// 비율 체크
	ShowStateText();
	return 0;
}

int StateBar::Collision(float _fTime)
{
	return 0;
}

int StateBar::PrevRender(float _fTime)
{
	return 0;
}

int StateBar::Render(float _fTime)
{
	return 0;
}

StateBar * StateBar::Clone() const
{
	return nullptr;
}

void StateBar::CheckRatio()
{
	float fRatio = (float)(m_State.m_iCur / m_State.m_iMax);
	m_fRatio = fRatio;

	if (nullptr != m_StateBar)
	{
		m_StateBar->SetRatio(fRatio);
	}
}

void StateBar::ShowStateText()
{
	if (nullptr == m_StateText)
		return;

	// 여기에 텍스트 체크된거 표시해주기
	std::string CurValue = std::to_string(m_State.m_iCur);
	std::string MaxValue = std::to_string(m_State.m_iMax);

	std::string CurState = CurValue + " / " + MaxValue;

	std::wstring Statews = StringToWstring(CurState);
	m_StateText->SetText(Statews.c_str());

}

void StateBar::Heal(int _Value)
{
	m_State.m_iCur += _Value;

	if (m_State.m_iMax < m_State.m_iCur)
	{
		m_State.m_iCur = m_State.m_iMax;
	}
}

void StateBar::Damage(int _Value)
{
	m_State.m_iCur -= _Value;

	if (m_State.m_iMin > m_State.m_iCur)
	{
		m_State.m_iCur = m_State.m_iMin;
	}
}
