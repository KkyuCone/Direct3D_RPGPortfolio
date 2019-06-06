#include "Camera.h"
#include "Transform.h"
#include "Frustum.h"
#include "..\Device.h"

ENGINE_USING

Camera::Camera()
	: m_eCameraType(CAM_END), 
	m_iWidth(1280), 
	m_iHeight(720),
	m_fAngle(90.0f), 
	m_fNear(0.3f), 
	m_fFar(1000.0f),
	m_bUpdate(true),
	m_pFrustum(nullptr)
{
	m_eComponentType = CT_CAMERA;
	m_pFrustum = new Frustum;
}

Camera::Camera(const Camera & _Camera) : Component(_Camera)
{
	*this = _Camera;
	m_iReferenceCount = 1;
	m_bUpdate = true;
	m_pFrustum = new Frustum;
}


Camera::~Camera()
{
	SAFE_DELETE(m_pFrustum);
}

void Camera::SetCameraInfo(CAMERA_TYPE _eCameraType, UINT _iWidth, UINT _iHeight, float _fAngle, float _fNear, float _fFar)
{
	m_eCameraType = _eCameraType;
	m_iWidth = _iWidth;
	m_iHeight = _iHeight;
	m_fAngle = _fAngle;
	m_fNear = _fNear;
	m_fFar = _fFar;
	SetCameraType(_eCameraType);
}

void Camera::SetCameraType(CAMERA_TYPE _eCameraType)
{
	switch (_eCameraType)
	{
	case Engine::CAM_PERSPECTIVE:
		// 원근투영
		// XMMatrixPerspectiveFovLH(시야각, 종횡비, 근평면거리, 먼평면거리)
		m_ProjectionMatrix = XMMatrixPerspectiveFovLH(DegreeToRadian(m_fAngle), m_iWidth / (float)m_iHeight, m_fNear, m_fFar);
		break;
	case Engine::CAM_ORTHOGONAL:
		// 직교투영
		m_ProjectionMatrix = XMMatrixOrthographicOffCenterLH(0.0f, (float)m_iWidth, 0.0f, (float)m_iHeight, 0.0f, m_fFar);
		break;
	case Engine::CAM_END:
		break;
	default:
		break;
	}
}

void Camera::Start()
{
}

bool Camera::Init()
{
	return true;
}

int Camera::Input(float _fTime)
{
	return 0;
}

int Camera::Update(float _fTime)
{
	m_bUpdate = false;
	return 0;
}

int Camera::LateUpdate(float _fTime)
{

	return 0;
}

int Camera::Collision(float _fTime)
{
	return 0;
}

int Camera::PrevRender(float _fTime)
{
	// 카메라에서 뷰행렬 계산해서 넣을거임 (행렬계산 X 각 값을 그냥 계산해서 대입, 이게 더 빠름)
	// 그리고 뷰행렬 계산은 행렬이 바뀌었을때(이동, 회전 등 변환되었을때) 계산하도록한다. 그외에도 계산하면 낭비임
	// UI가 아닐 경우에 해당
	if (true == m_pTransform->GetIsUpdate() && false == m_pTransform->GetUIEnable())
	{
		m_bUpdate = true;
		// 단위행렬 만들기
		m_ViewMatrix.Identity();

		// 11, 12, 13   = X축
		// 21, 22, 23	= Y축
		// 31, 32, 33	= Z축
		for (int i = 0; i < AXIS_END; ++i)
		{
			// 월드 회전축 넣어주기 (12바이트만큼(x,y,z))
			memcpy(&m_ViewMatrix[i][0], &m_pTransform->GetWorldAxis((AXIS)i), sizeof(Vector3));
		}

		// 전치를 한다.
		m_ViewMatrix.Transpose();

		// 그리고 이동값을 넣어준다. ( 마지막 행렬부분 )
		for (int j = 0; j < AXIS_END; ++j)
		{
			m_ViewMatrix[3][j] = -m_pTransform->GetWorldPosition().Dot(m_pTransform->GetWorldAxis((AXIS)j));
		}
	}

	Matrix	matVP = m_ViewMatrix * m_ProjectionMatrix;
	matVP.Inverse();
	m_pFrustum->Update(matVP);

	return 0;
}

int Camera::Render(float _fTime)
{
	return 0;
}

Camera * Camera::Clone() const
{
	return new Camera(*this);
}

Matrix Camera::GetViewMatrix()	const
{
	return m_ViewMatrix;
}

Matrix Camera::GetProjectionMatrix() const
{
	return m_ProjectionMatrix;
}

bool Camera::GetUpdate()	const
{
	return m_bUpdate;
}


// 프러스텀 컬링 -  계산
bool Camera::FrustumInPoint(const Vector3& _vPos)
{
	return m_pFrustum->FrustumInPoint(_vPos);
}

bool Camera::FrustumInSphere(const Vector3& _vCenter, float _fRadius)
{
	return m_pFrustum->FrustumInSphere(_vCenter, _fRadius);
}

float Camera:: GetNear() const
{
	return m_fNear;
}

float Camera::GetFar() const
{
	return m_fFar;
}
