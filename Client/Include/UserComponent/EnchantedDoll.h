#pragma once
#include "Component\UserComponent.h"
#include "Component\Renderer.h"
#include "Component\Transform.h"
#include "Component\Material.h"
#include "Component\Animation.h"
#include "Component\Collider.h"
#include "Component\ColliderRay.h"
#include "Component\ColliderSphere.h"
#include "Component\Navigation.h"
#include "Base.h"

ENGINE_USING

class EnchantedDoll : public Base
{
private:
	BaseState		m_eState;				// 애니메이션 등 상태 관련
	BaseUIState		m_tUIState;				// HP, MP, EXP 관련
	float			m_fSpeed;
	float			m_fCurWalkTime;
	float			m_fWalkTime;				// Walk상태일때 걷는 시간

private:
	GameObject* m_pPivotObj;

private:
	Renderer* m_pDollRenderer;
	Material* m_pDollMaterial;
	Animation*	m_pDollAnimation;

	ColliderSphere* m_pSphereCollider;
	ColliderSphere* m_pTraceCollider;

	Navigation* m_pNavigation;

public:
	EnchantedDoll();
	EnchantedDoll(const EnchantedDoll& _EnchantedDoll);
	~EnchantedDoll();

public:
	void SetMonsterPivotObject(GameObject* _pObject);

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);
	virtual int Render(float _fTime);
	virtual EnchantedDoll* Clone()	const;

private:
	void Idle(float _fTime);
	void Walk(float _fTime);
	void Run(float _fTime);				// 추적상태일때만 Run
	void Wait(float _fTime);
	void Talk(float _fTime);
	void Dance_Q(float _fTime);
	void Death(float _fTime);
	void DeathWait(float _fTime);
	void Attack01(float _fTime);
	void Attack02(float _fTime);

private:
	void Trace(Collider * _pSrc, Collider * _pDest, float _fTime);
	void TraceExit(Collider * _pSrc, Collider * _pDest, float _fTime);

private:
	void ChangeIdleState();
	void ChangeAnimation(std::string _strName);

	void MovePivotPos(float _fTime);
	void SetWalkStateDir();

	void RandomAttackState(float _fTime);

private:
	void HPCheck();
};

