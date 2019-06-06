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

#pragma region Set, Get 함수

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
	// 라이트 색상 셋팅
	m_tInfo.vDiffuse = _vDiffuse;
	m_tInfo.vAmbient = _vAmbient;
	m_tInfo.vSpecular = _vSpecular;
}

void Light::SetLightDistance(float _fDistance)
{
	// 라이트 거리 (총 거리)
	m_tInfo.fDistance = _fDistance;
}

void Light::SetLightAngle(float _fInAngle, float _fOutAngle)
{
	// 라이트 각도 (안쪽, 바깥쪽)
	m_fInAngle = _fInAngle;
	m_fOutAngle = _fOutAngle;

	m_tInfo.fInAngle = cosf(DegreeToRadian(_fInAngle) * 0.5f);			// cos세타 * 1/2값으로 넣어줘야한다. (반경)
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
		// 포인트라이트 크기는 거리(반지름)으로 한다.
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
	// 빛을 받을 위치랑 (현오브젝트위치) 빛방향
	m_tInfo.vPosition = m_pTransform->GetWorldPosition();
	m_tInfo.vDirection = m_pTransform->GetWorldAxis(AXIS_Z);

	// 조명 위치를 뷰공간으로 변환한다.
	// -> 카메라가 현재 뷰공간에 있기때문에..! 
	// 그리고 셰이더에서 뷰공간에서 라이트 처리하기로 했음

	Camera*	pCamera = m_pScene->GetMainCamera();

	m_tInfo.vPosition = m_tInfo.vPosition.TransformCoord(pCamera->GetViewMatrix());
	m_tInfo.vDirection = m_tInfo.vDirection.TransformNormal(pCamera->GetViewMatrix());

	// Directional Light만 역방향넣어준다.
	if (LT_DIR == m_tInfo.iType)
	{
		m_tInfo.vDirection *= -1.0f;
	}

	m_tInfo.vDirection.Normalize();


	// 그림자..라이트기준의VP
	// 섀도우를 위한 ..
	Transform* pLightTR = GetTransform();
	Matrix matLightView = {};
	Matrix matLightProj = {};

	matLightView.Identity();
	matLightProj.Identity();

	for (int i = 0; i < AXIS_END; ++i)
	{
		// 월드 회전축 넣어주기 (12바이트만큼(x,y,z))
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

	// 라이트의 셰이더 셋팅해주기 (상수버퍼 업데이트)
	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("Light", &m_tInfo);
}


