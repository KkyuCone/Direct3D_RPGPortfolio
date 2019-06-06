#pragma once
#include "Component\UserComponent.h"
#include "Component\Renderer.h"
#include "Component\Transform.h"
#include "Component\ColliderSphere.h"

ENGINE_USING

class Bullet : public UserComponent
{
private:
	float m_fSpeed;
	float m_fDist;

private:
	Renderer* m_pRenderer;
	ColliderSphere* m_pColliderSphere;

public:
	Bullet();
	Bullet(const Bullet& _Bullet);
	~Bullet();

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출, 즉 레이어에서 호출해준다.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// 렌더를 하기 직전에 호출해주는 함수ㅔㄱㅁ
	virtual int Render(float _fTime);
	virtual Bullet* Clone()	const;

};

