#include "Frustum.h"

ENGINE_USING

Frustum::Frustum()
{
	m_vPos[0] = Vector3(-1.0f, 1.0f, 0.0f);
	m_vPos[1] = Vector3(1.0f, 1.0f, 0.0f);
	m_vPos[2] = Vector3(-1.0f, -1.0f, 0.0f);
	m_vPos[3] = Vector3(1.0f, -1.0f, 0.0f);

	m_vPos[4] = Vector3(-1.0f, 1.0f, 1.0f);
	m_vPos[5] = Vector3(1.0f, 1.0f, 1.0f);
	m_vPos[6] = Vector3(-1.0f, -1.0f, 1.0f);
	m_vPos[7] = Vector3(1.0f, -1.0f, 1.0f);
}

Frustum::~Frustum()
{
}

void Frustum::Update(const Matrix & _matInvVP)
{
	Vector3 vPos[8];

	for (int i = 0; i < 8; ++i)
	{
		vPos[i] = m_vPos[i].TransformCoord(_matInvVP);		// 투영공간에서 월드 공간으로 변환
	}

	// 절두체의 면 만들어주기 
	// Left
	m_vPlane[FD_LEFT] = XMPlaneFromPoints(vPos[4].Convert(),
		vPos[0].Convert(), vPos[2].Convert());

	// Right
	m_vPlane[FD_RIGHT] = XMPlaneFromPoints(vPos[1].Convert(),
		vPos[5].Convert(), vPos[7].Convert());

	// Top
	m_vPlane[FD_TOP] = XMPlaneFromPoints(vPos[4].Convert(),
		vPos[5].Convert(), vPos[1].Convert());

	// Bottom
	m_vPlane[FD_BOTTOM] = XMPlaneFromPoints(vPos[2].Convert(),
		vPos[3].Convert(), vPos[7].Convert());

	// Near
	m_vPlane[FD_NEAR] = XMPlaneFromPoints(vPos[0].Convert(),
		vPos[1].Convert(), vPos[3].Convert());

	// Far
	m_vPlane[FD_FAR] = XMPlaneFromPoints(vPos[5].Convert(),
		vPos[4].Convert(), vPos[6].Convert());
}

bool Frustum::FrustumInPoint(const Vector3 & _vPos)
{
	for (int i = 0; i < FD_END; ++i)
	{
		float fDist = XMVectorGetX(XMPlaneDotCoord(m_vPlane[i].Convert(),
			_vPos.Convert()));

		if (fDist > 0.f)
			return false;
	}

	return true;
}

bool Frustum::FrustumInSphere(const Vector3 & _vCenter, float _fRadius)
{
	for (int i = 0; i < FD_END; ++i)
	{
		float fDist = XMVectorGetX(XMPlaneDotCoord(m_vPlane[i].Convert(),
			_vCenter.Convert()));

		if (fDist - _fRadius > 0.0f)
			return false;
	}

	return true;
}
