#include "Light.h"
#include "..\Render\ShaderManager.h"
#include "Transform.h"
#include "Camera.h"
#include "..\Scene\Scene.h"
#include "..\Device.h"

ENGINE_USING

Light::Light()
{
	m_fInAngle = 0.0f;
	m_fOutAngle = 0.0f;
	m_eComponentType = CT_LIGHT;
}

Light::Light(const Light & _Component) : Component(_Component)
{
	m_tInfo = _Component.m_tInfo;
}


Light::~Light()
{
}

#pragma region Set, Get �Լ�

LightInfo Light::GetLightInfo() const
{
	return m_tInfo;
}

float Light::GetInAngle() const
{
	return m_fInAngle;
}

float Light::GetOutAngle() const
{
	return m_fOutAngle;
}

Vector4 Light::GetDiffuseColor() const
{
	return m_tInfo.vDiffuse;
}

Vector4 Light::GetAmbientColor() const
{
	return m_tInfo.vAmbient;
}

void Light::SetLightType(LIGHT_TYPE _eType)
{
	// Dir, Point, Spot
	m_tInfo.iType = _eType;
}

void Light::SetLightColor(const Vector4 & _vDiffuse, const Vector4 & _vAmbient, const Vector4 & _vSpecular)
{
	// ����Ʈ ���� ����
	m_tInfo.vDiffuse = _vDiffuse;
	m_tInfo.vAmbient = _vAmbient;
	m_tInfo.vSpecular = _vSpecular;
}

void Light::SetLightDistance(float _fDistance)
{
	// ����Ʈ �Ÿ� (�� �Ÿ�)
	m_tInfo.fDistance = _fDistance;
}

void Light::SetLightAngle(float _fInAngle, float _fOutAngle)
{
	// ����Ʈ ���� (����, �ٱ���)
	m_fInAngle = _fInAngle;
	m_fOutAngle = _fOutAngle;

	m_tInfo.fInAngle = cosf(DegreeToRadian(_fInAngle) * 0.5f);			// cos��Ÿ * 1/2������ �־�����Ѵ�. (�ݰ�)
	m_tInfo.fOutAngle = cosf(DegreeToRadian(_fOutAngle) * 0.5f);

}

void Light::SetLightVP(Matrix _VP)
{
	m_tInfo.matShadowLightVP = _VP;
	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("Light", &m_tInfo);
}

#pragma endregion


void Light::Start()
{
}

bool Light::Init()
{
	return true;
}

int Light::Input(float _fTime)
{
	return 0;
}

int Light::Update(float _fTime)
{
	switch (m_tInfo.iType)
	{
	case LT_POINT:
		// ����Ʈ����Ʈ ũ��� �Ÿ�(������)���� �Ѵ�.
		m_pTransform->SetWorldScale(m_tInfo.fDistance,
			m_tInfo.fDistance, m_tInfo.fDistance);
		break;
	case LT_SPOT:
		break;
	default:
		break;
	}

	return 0;
}

int Light::LateUpdate(float _fTime)
{
	// ���� ���� ��ġ�� (��������Ʈ��ġ) ������
	m_tInfo.vPosition = m_pTransform->GetWorldPosition();
	m_tInfo.vDirection = m_pTransform->GetWorldAxis(AXIS_Z);

	// ���� ��ġ�� ��������� ��ȯ�Ѵ�.
	// -> ī�޶� ���� ������� �ֱ⶧����..! 
	// �׸��� ���̴����� ��������� ����Ʈ ó���ϱ�� ����

	Camera*	pCamera = m_pScene->GetMainCamera();

	m_tInfo.vPosition = m_tInfo.vPosition.TransformCoord(pCamera->GetViewMatrix());
	m_tInfo.vDirection = m_tInfo.vDirection.TransformNormal(pCamera->GetViewMatrix());

	// Directional Light�� ������־��ش�.
	if (LT_DIR == m_tInfo.iType)
	{
		m_tInfo.vDirection *= -1.0f;
	}

	m_tInfo.vDirection.Normalize();


	// �׸���..����Ʈ������VP
	// �����츦 ���� ..
	Transform* pLightTR = GetTransform();
	Matrix matLightView = {};
	Matrix matLightProj = {};

	matLightView.Identity();
	matLightProj.Identity();

	for (int i = 0; i < AXIS_END; ++i)
	{
		// ���� ȸ���� �־��ֱ� (12����Ʈ��ŭ(x,y,z))
		memcpy(&matLightView[i][0], &pLightTR->GetWorldAxis((AXIS)i), sizeof(Vector3));
	}

	matLightView.Transpose();

	for (int j = 0; j < AXIS_END; ++j)
	{
		matLightView[3][j] = -pLightTR->GetWorldPosition().Dot(pLightTR->GetWorldAxis((AXIS)j));
	}
	matLightProj = XMMatrixPerspectiveFovLH(DegreeToRadian(90.0f), (float)(_RESOLUTION.iWidth) / (float)(_RESOLUTION.iHeight), 0.3f, 1000.0f);

	m_tInfo.matShadowLightVP = matLightView * matLightProj;
	m_tInfo.matShadowLightVP.Transpose();

	SAFE_RELEASE(pLightTR);


	m_tInfo.matInvCamView = pCamera->GetViewMatrix();
	m_tInfo.matInvCamView.Inverse();
	m_tInfo.matInvCamView.Transpose();

	SAFE_RELEASE(pCamera);

	return 0;
}

int Light::Collision(float _fTime)
{
	return 0;
}

int Light::PrevRender(float _fTime)
{
	return 0;
}
int Light::Render(float _fTime)
{
	return 0;
}


Light * Light::Clone() const
{
	return new Light(*this);
}

void Light::SetShader()
{
	//
	m_pTransform->PrevRender(0.0f);

	// ����Ʈ�� ���̴� �������ֱ� (������� ������Ʈ)
	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("Light", &m_tInfo);
}


