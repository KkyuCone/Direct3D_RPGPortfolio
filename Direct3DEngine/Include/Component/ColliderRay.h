#pragma once
#include "Collider.h"

ENGINE_BEGIN

class ENGINE_DLL ColliderRay :
	public Collider
{
private:
	friend class GameObject;

protected:
	ColliderRay();
	ColliderRay(const ColliderRay& _Com);
	virtual ~ColliderRay();

private:
	RayInfo m_tRelative;
	RayInfo m_tInfo;
	bool m_bMouse;						// 마우스 기준의 레이인지 아닌지 여부

public:
	void MouseEnable(bool _Value);
	RayInfo GetInfo() const;

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
	virtual ColliderRay* Clone()	const;
};

ENGINE_END