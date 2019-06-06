#include "ColliderOBB.h"
#include "ColliderSphere.h"
#include "ColliderRay.h"
#ifdef _DEBUG
#include "..\Resource\Mesh.h"
#include "..\Resource\ResourcesManager.h"
#include "..\Scene\Scene.h"
#include "Camera.h"
#endif // _DEBUG

#include "Transform.h"

ENGINE_USING

ColliderOBB::ColliderOBB()
{
	SetTag("Col_OBB");
	m_eCollType = COL_OBB;

#ifdef _DEBUG
	m_pMesh = GET_SINGLETON(ResourcesManager)->FindMesh("Collider_Box");
#endif // _DEBUG


}

ColliderOBB::ColliderOBB(const ColliderOBB & _Com) : Collider(_Com)
{
	m_tRelative = _Com.m_tRelative;
	m_tInfo = _Com.m_tInfo;
}


ColliderOBB::~ColliderOBB()
{
}

void ColliderOBB::SetOBBInfo(const Vector3 & _vCenter, const Vector3 & _vHalfLength)
{
	m_tRelative.vCenter = _vCenter;
	m_tRelative.vHalfLength = _vHalfLength;

	m_tInfo.vHalfLength = _vHalfLength;
}

OBBInfo ColliderOBB::GetInfo() const
{
	return m_tInfo;
}

void ColliderOBB::Start()
{
	Collider::Start();
}

bool ColliderOBB::Init()
{
	return true;
}

int ColliderOBB::Input(float _fTime)
{
	return 0;
}

int ColliderOBB::Update(float _fTime)
{
	return 0;
}

int ColliderOBB::LateUpdate(float _fTime)
{
	m_tInfo.vCenter = m_tRelative.vCenter + m_pTransform->GetWorldPosition();

	// �������� ȸ����Ų��.
	Matrix	matWorldRot = m_pTransform->GetWorldRotationMatrix();
	for (int i = 0; i < AXIS_END; ++i)
	{
		m_tInfo.vAxis[i] = Vector3::Axis[i].TransformNormal(matWorldRot);
		m_tInfo.vAxis[i].Normalize();
	}

	// 8���� ���������� ���ϰ�, ȸ���� �Ŀ� �װ� �� ���ϰ� Min, Max ���� ���� ����
	Vector3 vPos[8];

	vPos[0] = Vector3(-m_tInfo.vHalfLength.x, m_tInfo.vHalfLength.y, -m_tInfo.vHalfLength.z);
	vPos[1] = Vector3(m_tInfo.vHalfLength.x, m_tInfo.vHalfLength.y, -m_tInfo.vHalfLength.z);
	vPos[2] = Vector3(-m_tInfo.vHalfLength.x, -m_tInfo.vHalfLength.y, -m_tInfo.vHalfLength.z);
	vPos[3] = Vector3(m_tInfo.vHalfLength.x, -m_tInfo.vHalfLength.y, -m_tInfo.vHalfLength.z);

	vPos[4] = Vector3(-m_tInfo.vHalfLength.x, m_tInfo.vHalfLength.y, m_tInfo.vHalfLength.z);
	vPos[5] = Vector3(m_tInfo.vHalfLength.x, m_tInfo.vHalfLength.y, m_tInfo.vHalfLength.z);
	vPos[6] = Vector3(-m_tInfo.vHalfLength.x, -m_tInfo.vHalfLength.y, m_tInfo.vHalfLength.z);
	vPos[7] = Vector3(m_tInfo.vHalfLength.x, -m_tInfo.vHalfLength.y, m_tInfo.vHalfLength.z);

	// 8���� ���������� ȸ����Ű�� ������ ���ؼ� ���� �������� ��ȯ���ش�.
	for (int i = 0; i < 8; ++i)
	{
		vPos[i] = vPos[i].TransformCoord(matWorldRot);
		vPos[i] += m_tInfo.vCenter;
	}

	// �ּҰ��� �ִ밪�� �����ش�.
	m_vSectionMin = Vector3(100000.f, 100000.f, 100000.f);
	m_vSectionMax = Vector3(-100000.f, -100000.f, -100000.f);

	for (int i = 0; i < 8; ++i)
	{
		if (m_vSectionMin.x > vPos[i].x)
			m_vSectionMin.x = vPos[i].x;

		if (m_vSectionMin.y > vPos[i].y)
			m_vSectionMin.y = vPos[i].y;

		if (m_vSectionMin.z > vPos[i].z)
			m_vSectionMin.z = vPos[i].z;

		if (m_vSectionMax.x < vPos[i].x)
			m_vSectionMax.x = vPos[i].x;

		if (m_vSectionMax.y < vPos[i].y)
			m_vSectionMax.y = vPos[i].y;

		if (m_vSectionMax.z < vPos[i].z)
			m_vSectionMax.z = vPos[i].z;
	}

	return 0;
}

int ColliderOBB::Collision(float _fTime)
{
	return 0;
}

bool ColliderOBB::Collision(Collider * _pDest)
{
	switch (_pDest->GetColliderType())
	{
	case COLLIDER_TYPE::COL_SPHERE:
		return CollisionSphereToOBB(((ColliderSphere*)_pDest)->GetInfo(), m_tInfo);
	case COLLIDER_TYPE::COL_CAPSULE:
		return false;
	case COLLIDER_TYPE::COL_OBB:
		return CollisionOBBToOBB(m_tInfo, ((ColliderOBB*)_pDest)->GetInfo());
	case COLLIDER_TYPE::COL_POINT:
		return false;
	case COLLIDER_TYPE::COL_RAY:
		return CollisionRayToOBB(((ColliderRay*)_pDest)->GetInfo(), m_tInfo);
	case COLLIDER_TYPE::COL_AABB:
		return false;
	}

	return false;
}

int ColliderOBB::PrevRender(float _fTime)
{
	return 0;
}

int ColliderOBB::Render(float _fTime)
{
#ifdef _DEBUG

	Camera*	pCamera = m_pScene->GetMainCamera();

	Matrix matScale, matRot, matTranslate, matWorld;

	// �������� �ִ´�.
	for (int i = 0; i < AXIS_END; ++i)
	{
		memcpy(&matRot[i][0], &m_tInfo.vAxis[i], sizeof(Vector3));
	}

	matScale.Scaling(m_tInfo.vHalfLength);
	matTranslate.Translation(m_tInfo.vCenter);

	matWorld = matScale * matRot * matTranslate;			// ��������� �����ش�. SRT

	// ������۸� �����ϱ� ���ؼ� ���� �־��ش�.
	m_tCBuffer.WorldRotationMatrix = matRot;
	m_tCBuffer.WorldViewMatrix = m_tCBuffer.WorldRotationMatrix * pCamera->GetViewMatrix();
	m_tCBuffer.WorldMatrix = matWorld;
	m_tCBuffer.ViewMatrix = pCamera->GetViewMatrix();
	m_tCBuffer.ProjectionMatrix = pCamera->GetProjectionMatrix();
	m_tCBuffer.InversProjectionMatrix = m_tCBuffer.ProjectionMatrix;
	m_tCBuffer.InversProjectionMatrix.Inverse();
	m_tCBuffer.WorldViewMatrix = m_tCBuffer.WorldMatrix * m_tCBuffer.ViewMatrix;
	m_tCBuffer.WorldViewProjectionMatrix = m_tCBuffer.WorldViewMatrix * m_tCBuffer.ProjectionMatrix;

	m_tCBuffer.WorldRotationMatrix.Transpose();
	m_tCBuffer.WorldViewMatrix.Transpose();
	m_tCBuffer.WorldMatrix.Transpose();
	m_tCBuffer.ViewMatrix.Transpose();
	m_tCBuffer.ProjectionMatrix.Transpose();
	m_tCBuffer.InversProjectionMatrix.Transpose();
	m_tCBuffer.WorldViewMatrix.Transpose();
	m_tCBuffer.WorldViewProjectionMatrix.Transpose();

	SAFE_RELEASE(pCamera);

#endif // _DEBUG

	Collider::Render(_fTime);

	return 0;
}

ColliderOBB * ColliderOBB::Clone() const
{
	return new ColliderOBB(*this);
}
