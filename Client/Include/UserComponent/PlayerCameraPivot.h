#pragma once
#include "Component\UserComponent.h"
#include "Component\Transform.h"
#include "Component\Animation.h"
#include "Component\Camera.h"
#include "Component\TPCamera.h"

ENGINE_USING

class PlayerCameraPivot : public UserComponent
{
private:
	class GameObject* m_pParentObj;
	class Player* m_pParentScript;
	Animation* m_pParentAni;
	TPCamera* m_pTPCamera;
	Transform* m_pTPCamTR;

public:
	PlayerCameraPivot();
	PlayerCameraPivot(const PlayerCameraPivot& _PlayerCameraPivot);
	~PlayerCameraPivot();

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출, 즉 레이어에서 호출해준다.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// 렌더를 하기 직전에 호출해주는 함수ㅔㄱㅁ
	virtual int Render(float _fTime);
	virtual PlayerCameraPivot* Clone()	const;

};

