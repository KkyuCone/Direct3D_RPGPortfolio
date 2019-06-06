#pragma once
#include "Collider.h"

ENGINE_BEGIN

class ENGINE_DLL ColliderPoint :
	public Collider
{
private:
	friend class GameObject;

protected:
	ColliderPoint();
	ColliderPoint(ColliderPoint& _Com);
	virtual ~ColliderPoint();

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출, 즉 레이어에서 호출해준다.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual bool Collision(Collider* _pDest);
	virtual int PrevRender(float _fTime);	// 렌더를 하기 직전에 호출해주는 함수ㅔㄱㅁ
	virtual int Render(float _fTime);
	virtual ColliderPoint* Clone()	const;
};

ENGINE_END
