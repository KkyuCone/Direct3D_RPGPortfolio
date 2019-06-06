#pragma once
#include "Component/UserComponent.h"
#include "Component/UIBar.h"
#include "Component/UIText.h"

ENGINE_USING

struct State
{
	int m_iCur;			// 현재 수치값
	int m_iMin;
	int m_iMax;			// 최대값
};

class StateBar : public UserComponent
{
private:
	float m_fRatio;		// 비율
	State m_State;

	class UIBar* m_StateBar;
	class UIText* m_StateText;

public:
	StateBar();
	StateBar(const StateBar& _StateBar);
	~StateBar();

public:
	UIBar* GetUIBar();
	UIText* GetStateText();

public:
	void SetMin(int _iMin);
	void SetMax(int _iMax);

	void SetUIBar(UIBar* _Bar);
	void SetUIText(UIText* _Text);

	void SetColor(Vector4 _vColor);
	void SetChangeBarShader(std::string& _strKey);
	void SetStateBarTexture(const std::string& _strName, const TCHAR* _pFileName, const std::string& _strPathName = PATH_UI_BAR);

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출, 즉 레이어에서 호출해준다.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// 렌더를 하기 직전에 호출해주는 함수ㅔㄱㅁ
	virtual int Render(float _fTime);
	virtual StateBar* Clone()	const;

private:
	void CheckRatio();			// 비율체크
	void ShowStateText();		// 텍스트 바꾸기

public:
	void Heal(int _Value);
	void Damage(int _Value);
};

