#pragma once
#include "Collider.h"

ENGINE_BEGIN

class ENGINE_DLL ColliderOBB :
	public Collider
{
private:
	friend class GameObject;

protected:
	ColliderOBB();
	ColliderOBB(const ColliderOBB& _Com);
	virtual ~ColliderOBB();
	
private:
	OBBInfo m_tRelative;
	OBBInfo m_tInfo;

public:
	void SetOBBInfo(const Vector3& _vCenter, const Vector3& _vHalfLength);
	OBBInfo GetInfo()	const;

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
	virtual ColliderOBB* Clone()	const;

};

ENGINE_END
