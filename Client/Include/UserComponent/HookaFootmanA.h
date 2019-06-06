#pragma once

#include "Component\UserComponent.h"
#include "Component\Renderer.h"
#include "Component\Transform.h"
#include "Component\Material.h"
#include "Component\Animation.h"
#include "Component\Collider.h"
#include "Component\ColliderRay.h"
#include "Base.h"

ENGINE_USING

class HookaFootmanA : public Base
{
private:
	BaseState		m_eState;				// �ִϸ��̼� �� ���� ����
	BaseUIState		m_tUIState;				// HP, MP, EXP ����
	float			m_fSpeed;

private:
	Renderer* m_pHookaRenderer;
	Material* m_pHookaMaterial;
	Animation*	m_pHookaAnimation;

	ColliderSphere* m_pSphereCollider;
	ColliderSphere* m_pTraceCollider;

	Navigation* m_pNavigation;

private:
	// �ٸ� ������Ʈ
	Transform* m_pPlayerTrnasform;

public:
	HookaFootmanA();
	HookaFootmanA(const HookaFootmanA& _HookaFootmanA);
	~HookaFootmanA();

public:
	virtual void Start();				// ��ġ�ɷ��� ���̾ ���Եɶ� (��ġ�ɶ�) ȣ��, �� ���̾�� ȣ�����ش�.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// ������ �ϱ� ������ ȣ�����ִ� �Լ��Ĥ���
	virtual int Render(float _fTime);
	virtual HookaFootmanA* Clone()	const;

private:
	void Idle();
	void Walk();
	void Run();				// ���������϶��� Run
	void Wait();
	void Talk();
	void Dance_Q();
	void Death();
	void DeathWait();
	void Attack01();
	void Attack02();

private:
	void Trace(Collider * _pSrc, Collider * _pDest, float _fTime);
	void TraceExit(Collider * _pSrc, Collider * _pDest, float _fTime);

private:
	void ChangeIdleState();
	void ChangeAnimation(std::string _strName);

private:
	void HPCheck();
};

