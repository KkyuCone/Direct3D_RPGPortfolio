#pragma once
#include "Component.h"

ENGINE_BEGIN

// 3인칭 카메라 컴포넌트지만
// 1인칭 처럼도 사용 가능하다. (거리 0으로 설정하면 됨)

class ENGINE_DLL TPCamera : public Component
{
private:
	friend class GameObject;

protected:
	TPCamera();
	TPCamera(const TPCamera& _Com);
	virtual ~TPCamera();

private:
	class Transform*	m_pTarget;				// 타겟의 위치
	Vector3				m_vDistance;			// 타겟을 바라보는 방향
	bool				m_bMouseEnable;			// 마우스 존재여부

	bool				m_bSoketMove;			// 소켓을 타겟으로 할지 여부
	bool				m_bBaseNoneMove;		// Idle상태일 경우


public:
	void SetTarget(class GameObject* _pTarget);
	void SetTarget(Component* _pTarget);
	void SetDistance(const Vector3& _vDistance);
	void SetMouseEnable(bool _bEnable);
	void SetSoketMoveEnable(bool _bEnable);
	void SetBaseNoneMoveEnable(bool _bEnable);

public:
	virtual void Start();			
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);
	virtual int Render(float _fTime);
	virtual int RenderShadow(float _fTime);
	virtual TPCamera* Clone()	const;

};

ENGINE_END