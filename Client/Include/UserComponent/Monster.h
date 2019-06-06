#pragma once
#include "GameObject.h"
#include "Component\UserComponent.h"
#include "Component\Renderer.h"
#include "Component\Transform.h"
#include "Component\Collider.h"
#include "Component\ColliderSphere.h"
#include "Component\ColliderOBB.h"
#include "Component\Material.h"
#include "Component\Animation.h"
#include "Component\Navigation.h"

ENGINE_USING

class Monster : public UserComponent
{
private:
	float m_BulletReTime;
	float m_BulletCurTime;

private:
	Transform* pTransform;
	Renderer* m_pRenderer;
	ColliderSphere* m_pSphereCollider;
	ColliderSphere* m_pTraceCollider;
	ColliderOBB*	m_pOBBCollider;
	Material* m_pMaterial;

	Animation* m_pAnimation;
	Navigation* m_pNavigation;

private:
	// 다른 오브젝트
	Transform* m_pPlayerTrnasform;

public:
	Monster();
	Monster(const Monster& _Monster);
	~Monster();

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출, 즉 레이어에서 호출해준다.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// 렌더를 하기 직전에 호출해주는 함수ㅔㄱㅁ
	virtual int Render(float _fTime);
	virtual Monster* Clone()	const;

public:
	void Trace(Collider* _pSrc, Collider* _pDest, float _fTime);
	void TraceExit(Collider* _pSrc, Collider* _pDest, float _fTime);

private:
	void Fire(Vector3 _Dir);
};

