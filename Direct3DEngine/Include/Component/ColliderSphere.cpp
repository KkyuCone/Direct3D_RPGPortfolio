#include "ColliderSphere.h"
#include "ColliderOBB.h"
#include "ColliderRay.h"

#ifdef _DEBUG
#include "..\Resource\Mesh.h"
#include "..\Resource\ResourcesManager.h"
#include "..\Scene\Scene.h"
#include "Camera.h"
#endif // _DEBUG

#include "Transform.h"

ENGINE_USING

ColliderSphere::ColliderSphere()
{
	SetTag("Col_Sphere");
	m_eCollType = COL_SPHERE;
	m_bScaleEnable = true;

#ifdef _DEBUG
	m_pMesh = GET_SINGLETON(ResourcesManager)->FindMesh("Sphere");
#endif // _DEBUG

}

ColliderSphere::ColliderSphere(const ColliderSphere & _Com) : Collider(_Com)
{
	m_tRelative = _Com.m_tRelative;
	m_tInfo = _Com.m_tInfo;;
	m_bScaleEnable = _Com.m_bScaleEnable;
}


ColliderSphere::~ColliderSphere()
{
}

SphereInfo ColliderSphere::GetInfo() const
{
	return m_tInfo;
}

// �� �浹ü�� ������ Set()
// _vCenter�߽���.. ���� ĳ���� �߽����� ��������Vector4::Zero�� �־���, ������ �浹ü�� ������Ʈ�� ���� �Ǻ�����
void ColliderSphere::SetSphereInfo(const Vector3 & _vCenter, float _fRadius)
{
	m_tRelative.vCenter = _vCenter;
	m_tRelative.fRadius = _fRadius;
	m_tInfo.fRadius = _fRadius;
}

void ColliderSphere::SetScaleEnable(bool _bEnable)
{
	m_bScaleEnable = _bEnable;
}

void ColliderSphere::Start()
{
	Collider::Start();
}

bool ColliderSphere::Init()
{
	return true;
}

int ColliderSphere::Input(float _fTime)
{
	return 0;
}

int ColliderSphere::Update(float _fTime)
{
	return 0;
}

int ColliderSphere::LateUpdate(float _fTime)
{
	if (true == m_bScaleEnable)
	{
		// ���� ũ�⸦ �����Ѵ�.
		Vector3 vScale = m_pTransform->GetWorldScale();

		// �浹ü�� ���� ��ġ���
		m_tInfo.vCenter = m_tRelative.vCenter * vScale + m_pTransform->GetWorldPosition();

		// ���� ūũ�� �������� �� ũ�⸦ �����ؾ��Ѵ�.
		float fScale = vScale.x;
		fScale = fScale < vScale.y ? vScale.y : fScale;
		fScale = fScale < vScale.z ? vScale.z : fScale;

		m_tInfo.fRadius = m_tRelative.fRadius * fScale;
	}
	else
	{
		// ����ũ�� ������
		m_tInfo.vCenter = m_tRelative.vCenter + m_pTransform->GetWorldPosition();
		m_tInfo.fRadius = m_tRelative.fRadius;
	}

	// �� �浹ü�̱� ������ ������ ������ �̿��ؼ�
	// �ڽ��� ���� ������ �˾Ƴ��� ���� �ڽ��� ������ �ּڰ��� �ִ��� �����Ѵ�.
	m_vSectionMin = m_tInfo.vCenter - m_tInfo.fRadius;
	m_vSectionMax = m_tInfo.vCenter + m_tInfo.fRadius;
	return 0;
}

int ColliderSphere::Collision(float _fTime)
{
	return 0;
}

bool ColliderSphere::Collision(Collider * _pDest)
{
	switch (_pDest->GetColliderType())
	{
	case COLLIDER_TYPE::COL_SPHERE:
		return CollisionSphereToSphere(m_tInfo, ((ColliderSphere*)_pDest)->GetInfo());
	case COLLIDER_TYPE::COL_CAPSULE:
		//return CollisionSphereToOBB(m_tInfo, ((ColliderOBB*)_pDest)->GetInfo());
	case COLLIDER_TYPE::COL_OBB:
		return CollisionSphereToOBB(m_tInfo, ((ColliderOBB*)_pDest)->GetInfo());
	case COLLIDER_TYPE::COL_POINT:
		return false;
	case COLLIDER_TYPE::COL_RAY:
		return CollisionSphereToRay(m_tInfo, ((ColliderRay*)_pDest)->GetInfo());
	case COLLIDER_TYPE::COL_AABB:
		return false;
	}

	return false;
}

int ColliderSphere::PrevRender(float _fTime)
{
	return 0;
}

int ColliderSphere::Render(float _fTime)
{
#ifdef _DEBUG
	Camera*	pCamera = m_pScene->GetMainCamera();

	Matrix	matScale, matTranslate, matWorld;

	matScale.Scaling(m_tInfo.fRadius, m_tInfo.fRadius, m_tInfo.fRadius);			// ũ�� ����
	matTranslate.Translation(m_tInfo.vCenter);

	matWorld = matScale * matTranslate;

	m_tCBuffer.WorldRotationMatrix.Identity();
	m_tCBuffer.WorldViewRotationMatrix = m_tCBuffer.WorldRotationMatrix * pCamera->GetViewMatrix();
	m_tCBuffer.WorldMatrix = matWorld;
	m_tCBuffer.ViewMatrix = pCamera->GetViewMatrix();
	m_tCBuffer.ProjectionMatrix = pCamera->GetProjectionMatrix();
	m_tCBuffer.InversProjectionMatrix = m_tCBuffer.ProjectionMatrix;
	m_tCBuffer.InversProjectionMatrix.Inverse();
	m_tCBuffer.WorldViewMatrix = m_tCBuffer.WorldMatrix * m_tCBuffer.ViewMatrix;
	m_tCBuffer.WorldViewProjectionMatrix = m_tCBuffer.WorldViewMatrix * m_tCBuffer.ProjectionMatrix;

	m_tCBuffer.WorldRotationMatrix.Transpose();
	m_tCBuffer.WorldViewRotationMatrix.Transpose();
	m_tCBuffer.WorldMatrix.Transpose();
	m_tCBuffer.ViewMatrix.Transpose();
	m_tCBuffer.ProjectionMatrix.Transpose();
	m_tCBuffer.InversProjectionMatrix.Transpose();
	m_tCBuffer.WorldViewMatrix.Transpose();
	m_tCBuffer.WorldViewProjectionMatrix.Transpose();

	SAFE_RELEASE(pCamera);
#endif // _DEBUG


	Collider::Render(_fTime);			// ���⼭ ������� �������ش�. �������� ���� �κ�.. (���̴� + ��ǲ���̾ƿ� ��)
	return 0;
}

ColliderSphere * ColliderSphere::Clone() const
{
	return new ColliderSphere(*this);
}
