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
		// ��������
		// XMMatrixPerspectiveFovLH(�þ߰�, ��Ⱦ��, �����Ÿ�, �����Ÿ�)
		m_ProjectionMatrix = XMMatrixPerspectiveFovLH(DegreeToRadian(m_fAngle), m_iWidth / (float)m_iHeight, m_fNear, m_fFar);
		break;
	case Engine::CAM_ORTHOGONAL:
		// ��������
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
	// ī�޶󿡼� ����� ����ؼ� �������� (��İ�� X �� ���� �׳� ����ؼ� ����, �̰� �� ����)
	// �׸��� ����� ����� ����� �ٲ������(�̵�, ȸ�� �� ��ȯ�Ǿ�����) ����ϵ����Ѵ�. �׿ܿ��� ����ϸ� ������
	// UI�� �ƴ� ��쿡 �ش�
	if (true == m_pTransform->GetIsUpdate() && false == m_pTransform->GetUIEnable())
	{
		m_bUpdate = true;
		// ������� �����
		m_ViewMatrix.Identity();

		// 11, 12, 13   = X��
		// 21, 22, 23	= Y��
		// 31, 32, 33	= Z��
		for (int i = 0; i < AXIS_END; ++i)
		{
			// ���� ȸ���� �־��ֱ� (12����Ʈ��ŭ(x,y,z))
			memcpy(&m_ViewMatrix[i][0], &m_pTransform->GetWorldAxis((AXIS)i), sizeof(Vector3));
		}

		// ��ġ�� �Ѵ�.
		m_ViewMatrix.Transpose();

		// �׸��� �̵����� �־��ش�. ( ������ ��ĺκ� )
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


// �������� �ø� -  ���
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
