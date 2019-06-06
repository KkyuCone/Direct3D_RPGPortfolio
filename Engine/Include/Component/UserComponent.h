#pragma once
#include "Component.h"

ENGINE_BEGIN
// UserComponent는 게임오브젝트의 스크립트?역할을 한다고 생각하면 될듯
class ENGINE_DLL UserComponent : public Component
{
private:
	friend class GameObject;

protected:
	UserComponent();
	UserComponent(const UserComponent& _Component);
	virtual ~UserComponent() = 0;

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출, 즉 레이어에서 호출해준다.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// 렌더를 하기 직전에 호출해주는 함수ㅔㄱㅁ
	virtual int Render(float _fTime);
	virtual Component* Clone()	const;
};

ENGINE_END

