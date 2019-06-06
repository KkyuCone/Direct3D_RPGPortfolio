#include "ColliderRay.h"
#include "ColliderSphere.h"
#include "ColliderOBB.h"
#include "..\InputManager.h"
#include "..\Device.h"
#include "..\Scene\Scene.h"
#include "Camera.h"

ENGINE_USING

ColliderRay::ColliderRay()
{
	SetTag("Col_Ray");
	m_eCollType = COL_RAY;
	m_bMouse = false;
}

ColliderRay::ColliderRay(const ColliderRay & _Com) :  Collider(_Com)
{
	m_tRelative = _Com.m_tRelative;
	m_tInfo = _Com.m_tInfo;
	m_bMouse = _Com.m_bMouse;
}


ColliderRay::~ColliderRay()
{
}

void ColliderRay::MouseEnable(bool _Value)
{
	m_bMouse = _Value;
}

RayInfo ColliderRay::GetInfo() const
{
	return m_tInfo;
}

void ColliderRay::Start()
{
	Collider::Start();
}

bool ColliderRay::Init()
{
	return true;
}

int ColliderRay::Input(float _fTime)
{
	return 0;
}

int ColliderRay::Update(float _fTime)
{
	return 0;
}

int ColliderRay::LateUpdate(float _fTime)
{
	// 레이는 마우스인 경우와 아닌경우를 생각한다. ( 아닌경우(일반오브젝트기준)는 알아서 만들기 )
	if (true == m_bMouse)
	{
		Vector2 vMousePos = GET_SINGLETON(InputManager)->GetMouseViewportPos();		// 뷰포트 상의 마우스 위치 구하기
		Resolution tRS = _RESOLUTION;					// 해상도

		Camera* pCamera = m_pScene->GetMainCamera();		// 메인카메라 가져와서..

		Matrix matProj = pCamera->GetProjectionMatrix();
		Matrix matView = pCamera->GetViewMatrix();

		SAFE_RELEASE(pCamera);

		matView = matView.Inverse();		// 뷰의 역행렬.. (월드상으로 돌려야하니깜)

		// 여기서 동차좌표사용함
		// 동차좌표 : 2차원 공간의 좌푤르 표기하기 위해 3차원 벡터를 사용하는것.
		// 마우스 위치를 3차원 공간으로 변환한다. (투영 -> 뷰공간으로 바꾸기)
		m_tInfo.vDir.x = (vMousePos.x / (tRS.iWidth / 2.0f) - 1.0f) / matProj._11;
		m_tInfo.vDir.y = (vMousePos.y / (tRS.iHeight / -2.0f) + 1.0f) / matProj._22;
		m_tInfo.vDir.z = 1.f;

		//m_tInfo.vDir.Normalize();			// 방향만 나오게 정규화한다.
		m_tInfo.vOrigin = Vector3::Zero;	// 뷰공간이니까 카메라 위치는 (0, 0, 0)이다. 

		// 뷰의 역행렬을 곱해서 월드 공간으로 변환한다.
		m_tInfo.vDir = m_tInfo.vDir.TransformNormal(matView);
		m_tInfo.vDir.Normalize();

		m_tInfo.vOrigin = m_tInfo.vOrigin.TransformCoord(matView);		// 동차좌표로..이동

		Vector3	vEnd = m_tInfo.vOrigin + m_tInfo.vDir * 1000.0f;

		m_vSectionMin.x = m_tInfo.vOrigin.x < vEnd.x ? m_tInfo.vOrigin.x : vEnd.x;
		m_vSectionMin.y = m_tInfo.vOrigin.y < vEnd.y ? m_tInfo.vOrigin.y : vEnd.y;
		m_vSectionMin.z = m_tInfo.vOrigin.z < vEnd.z ? m_tInfo.vOrigin.z : vEnd.z;

		m_vSectionMax.x = m_tInfo.vOrigin.x > vEnd.x ? m_tInfo.vOrigin.x : vEnd.x;
		m_vSectionMax.y = m_tInfo.vOrigin.y > vEnd.y ? m_tInfo.vOrigin.y : vEnd.y;
		m_vSectionMax.z = m_tInfo.vOrigin.z > vEnd.z ? m_tInfo.vOrigin.z : vEnd.z;

	}
	else
	{

	}

	return 0;
}

int ColliderRay::Collision(float _fTime)
{
	return 0;
}

bool ColliderRay::Collision(Collider * _pDest)
{
	switch (_pDest->GetColliderType())
	{
	case COLLIDER_TYPE::COL_SPHERE:
		return CollisionSphereToRay(((ColliderSphere*)_pDest)->GetInfo(), m_tInfo);
	case COLLIDER_TYPE::COL_CAPSULE:
		return false;
	case COLLIDER_TYPE::COL_OBB:
		return CollisionRayToOBB(m_tInfo, ((ColliderOBB*)_pDest)->GetInfo());
	case COLLIDER_TYPE::COL_POINT:
		return false;
	case COLLIDER_TYPE::COL_RAY:
		return false;
	case COLLIDER_TYPE::COL_AABB:
		return false;
	}

	return false;
}

int ColliderRay::PrevRender(float _fTime)
{
	return 0;
}

int ColliderRay::Render(float _fTime)
{
	return 0;
}

ColliderRay * ColliderRay::Clone() const
{
	return new ColliderRay(*this);
}
