#pragma once
#include "Collider.h"

ENGINE_BEGIN

class ENGINE_DLL ColliderSphere :
	public Collider
{
private:
	friend class GameObject;

protected:
	ColliderSphere();
	ColliderSphere(const ColliderSphere& _Com);
	virtual ~ColliderSphere();

private:
	SphereInfo	m_tRelative;			// 상대적인 OffSet
	SphereInfo	m_tInfo;				// 구 충돌체 정보
	bool		m_bScaleEnable;			// 구 정보가 스케일 영향을 받을건지에 대한 bool값

public:
	SphereInfo GetInfo()	const;

public:
	void SetSphereInfo(const Vector3& _vCenter, float _fRadius);
	void SetScaleEnable(bool _bEnable);

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출, 즉 레이어에서 호출해준다.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual bool Collision(Collider* _pDest);
	virtual int PrevRender(float _fTime);	// 렌더를 하기 직전에 호출해주는 함수
	virtual int Render(float _fTime);
	virtual ColliderSphere* Clone()	const;
};

ENGINE_END