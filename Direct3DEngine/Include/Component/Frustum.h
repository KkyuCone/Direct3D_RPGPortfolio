#pragma once
#include "..\Engine.h"

ENGINE_BEGIN

// 절두체 방향
enum FRUSTUM_DIR
{
	FD_LEFT,
	FD_RIGHT,
	FD_TOP,
	FD_BOTTOM,
	FD_NEAR,
	FD_FAR,
	FD_END
};

// 프러스텀 컬링을 위한 클래스
// 월드 행렬 기준으로 컬링을 할 것임, 평면의 방정식 이용
class ENGINE_DLL Frustum
{
private:
	friend class Camera;

public:
	Frustum();
	~Frustum();

private:
	Vector3 m_vPos[8];
	Vector4 m_vPlane[FD_END];

public:
	void Update(const Matrix& _matInvVP);
	bool FrustumInPoint(const Vector3& _vPos);
	bool FrustumInSphere(const Vector3& _vCenter, float _fRadius);
};

ENGINE_END
