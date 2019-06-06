#pragma once
#include "Component.h"

ENGINE_BEGIN

// 뷰공간(View Space)은 뷰행렬과 투영핼행렬로 이뤄져있다.
// 원근투영에는 종횡비, 시야각, 근평면거리, 먼평면거리가 필요하다.
class ENGINE_DLL __declspec(align(16)) Camera : public Component
{
private:
	friend class GameObject;

private:
	Camera();
	Camera(const Camera& _Camera);
	~Camera();

private:
	Matrix	m_ViewMatrix;				// 뷰행렬
	Matrix	m_ProjectionMatrix;			// 투영행렬
	CAMERA_TYPE m_eCameraType;
	UINT	m_iWidth;
	UINT	m_iHeight;
	float	m_fAngle;					// 시야각
	float	m_fNear;
	float	m_fFar;

	bool	m_bUpdate;					// 업데이트 필요 여부

	class Frustum* m_pFrustum;			// 프러스텀 컬링을 위해서 ㅇㅅㅇ

public:
	void SetCameraInfo(CAMERA_TYPE _eCameraType, UINT _iWidth, UINT _iHeight,
		float _fAngle, float _fNear, float _fFar);
	void SetCameraType(CAMERA_TYPE _eCameraType);

public:
	bool GetUpdate()	const;
	Matrix GetViewMatrix()	const;
	Matrix GetProjectionMatrix()	const;
	float GetNear() const;
	float GetFar() const;

public:
	// 컬링할 오브젝트를 계산할거임
	bool FrustumInPoint(const Vector3& _vPos);
	bool FrustumInSphere(const Vector3& _vCenter, float _fRadius);

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출, 즉 레이어에서 호출해준다.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// 렌더를 하기 직전에 호출해주는 함수ㅔㄱㅁ
	virtual int Render(float _fTime);
	virtual Camera* Clone()	const;

};

ENGINE_END

