#pragma once
#include "Component.h"

ENGINE_BEGIN

class ENGINE_DLL Navigation : public Component
{
private:
	friend class GameObject;

protected:
	Navigation();
	Navigation(const Navigation& _Com);
	~Navigation();

private:
	Vector3 m_vMousePos;				// 마우스랑 충돌한 지형 위치
	bool m_bMousePosCheck;
	bool m_bddddddddddddddd;
	class NavigationMesh* m_pNavMesh;


private:
	bool m_bOnCellYPosition;			// 해당 네비게이션 메쉬의 셀의 Y포지션
	bool m_bMouseMove;					// 마우스 이동여부 (마우스로 이동할거 생각해서)

	bool m_bAI;							// AI 설정여부
	std::list<Vector3> m_PathList;		// 경로 (길찾으려고 찾은 경로들을 모아둔다.)
	class Transform* m_pTarget;			// AI일 경우 따라다닐 타겟 지정
	Vector3 m_vTargetPos;				// 타겟 위치
	Vector3 m_vWay;						// 갈 경로
	float m_fTargetDetectTime;			// 타겟 위치를 체크하는 시간
	float m_fTargetCheckTime;			// 얘가 n초 체크하는애

public:
	Vector3 GetMousePos() const;		// 지형과 충돌한 지점

public:
	void SetCellYPosition(bool _bEnable);
	void SetMouseMove(bool _bEnable);
	void SetAIFindPath(bool _bEnable);
	void SetTarget(class GameObject* _pTarget);
	void SetTarget(class Component* _pTarget);
	void SetTargetDetectTime(float _fTime);

private:
	void CheckMousePos();				// 마우스 위치판별 함수

	void CheckAIPos(Vector3 _vPos, float _fTime);
	void MoveAI(float _fTime);
	bool FindPath();

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출, 즉 레이어에서 호출해준다.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);
	virtual int Render(float _fTime);
	virtual Navigation* Clone()	const;
};

ENGINE_END