#pragma once
#include "Component.h"

ENGINE_BEGIN

enum UI_TYPE
{
	UI_NONE = 0,
	UI_BUTTON,
	UI_IMAGE,
	UI_BAR,
	UI_TEXT,
};

class ENGINE_DLL UI : public Component
{
private:
	friend class GameObject;

protected:
	UI();
	UI(const UI& _Com);
	virtual ~UI() = 0;

public:
	UI_TYPE m_eUIType;

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출, 즉 레이어에서 호출해준다.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// 렌더를 하기 직전에 호출해주는 함수ㅔㄱㅁ
	virtual int Render(float _fTime);
	virtual UI* Clone()	const;
};

ENGINE_END