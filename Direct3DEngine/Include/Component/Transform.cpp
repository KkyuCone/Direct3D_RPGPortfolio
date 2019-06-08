#include "Transform.h"
#include "..\Device.h"
#include "..\Render\ShaderManager.h"
#include "..\GameObject.h"
#include "..\Scene\Scene.h"
#include "Animation.h"
#include "Camera.h"

ENGINE_USING

Transform::Transform()
	: m_bStatic(false)
	, m_bUpdate(true)
	, m_bUI(false)
	, m_pLookAt(nullptr)
	, m_bLookAt(false)
{
	m_vMove = Vector3::Zero;
	m_tShadowCBuffer = {};
	m_eComponentType = CT_TRANSFORM;
	m_vLocalRelativeView = Vector3::Axis[AXIS_Z];
	m_vLocalView = m_vLocalRelativeView;
	m_vWorldView = m_vLocalRelativeView;
	m_eLookAxis = LOOK_ALL;				// 기본
}


Transform::~Transform()
{
	SAFE_RELEASE(m_pLookAt);
}

Transform::Transform(const Transform & _Component)
	: Component(_Component)
{
	*this = _Component;
	m_iReferenceCount = 1;
	m_bUpdate = true;
	m_pLookAt = nullptr;
	m_bUI = _Component.m_bUI;
	m_vMove = Vector3::Zero;
}

#pragma region Get, Set
// Static, Update

bool Transform::GetIsStatic()	const
{
	return m_bStatic;
}

bool Transform::GetIsUpdate()	const
{
	return m_bUpdate;
}

bool Transform::GetUIEnable()	const
{
	return m_bUI;
}

void Transform::SetUIEnable(bool _Enable)
{
	m_bUI = _Enable;
}

// 로컬

Vector3 Transform::GetLocalPosition()	const
{
	return m_vLocalPosition;
}

Vector3	Transform::GetLocalRotation()	const
{
	return m_vLocalRotation;
}

Vector3	Transform::GetLocalScale()		const
{
	return m_vLocalSacle;
}

Vector3 Transform::GetLocalView()		const
{
	return m_vLocalView;
}

void Transform::SetLocalPosition(float _x, float _y, float _z)
{
	m_vLocalPosition.x = _x;
	m_vLocalPosition.y = _y;
	m_vLocalPosition.z = _z;

	m_MatrixLocalPosition.Translation(m_vLocalPosition);
	m_bUpdate = true;
}

void Transform::SetLocalPosition(const Vector3& _vPosition)
{
	m_vLocalPosition = _vPosition;
	m_MatrixLocalPosition.Translation(m_vLocalPosition);
	m_bUpdate = true;
}


void Transform::SetLocalRotation(float _x, float _y, float _z)
{
	m_vLocalRotation.x = _x;
	m_vLocalRotation.y = _y;
	m_vLocalRotation.z = _z;

	m_MatrixLocalRotation.Rotation(m_vLocalRotation);

	m_vLocalView = m_vLocalRelativeView.TransformNormal(m_MatrixLocalRotation);
	m_vLocalView.Normalize();

	m_bUpdate = true;
}

void Transform::SetLocalRotation(const Vector3& _vRotation)
{
	m_vLocalRotation = _vRotation;
	m_MatrixLocalRotation.Rotation(m_vLocalRotation);

	m_vLocalView = m_vLocalRelativeView.TransformNormal(m_MatrixLocalRotation);
	m_vLocalView.Normalize();

	m_bUpdate = true;
}

void Transform::SetLocalRotateX(float _x)
{
	m_vLocalRotation.x = _x;
	m_MatrixLocalRotation.Rotation(m_vLocalRotation);

	m_vLocalView = m_vLocalRelativeView.TransformNormal(m_MatrixLocalRotation);
	m_vLocalView.Normalize();

	m_bUpdate = true;
}

void Transform::SetLocalRotateY(float _y)
{
	m_vLocalRotation.y = _y;
	m_MatrixLocalRotation.Rotation(m_vLocalRotation);

	m_vLocalView = m_vLocalRelativeView.TransformNormal(m_MatrixLocalRotation);
	m_vLocalView.Normalize();

	m_bUpdate = true;
}

void Transform::SetLocalRotateZ(float _z)
{
	m_vLocalRotation.z = _z;
	m_MatrixLocalRotation.Rotation(m_vLocalRotation);

	m_vLocalView = m_vLocalRelativeView.TransformNormal(m_MatrixLocalRotation);
	m_vLocalView.Normalize();


	m_bUpdate = true;
}

void Transform::SetLocalScale(float _x, float _y, float _z)
{
	m_vLocalSacle.x = _x;
	m_vLocalSacle.y = _y;
	m_vLocalSacle.z = _z;

	m_MatrixLocalScale.Scaling(m_vLocalSacle);
	m_bUpdate = true;
}

void Transform::SetLocalScale(const Vector3& _vScale)
{
	m_vLocalSacle = _vScale;
	m_MatrixLocalScale.Scaling(m_vLocalSacle);
	m_bUpdate = true;
}

void Transform::SetLocalRelativeView(float _x, float _y, float _z)
{
	m_vLocalRelativeView = Vector3(_x, _y, _z);
	m_vLocalView = m_vLocalRelativeView.TransformNormal(m_MatrixLocalRotation);		// 로컬 회전축이 변환함
	m_vLocalView.Normalize();			// 회전축은 놈이 1인 값이여야하므로.. 즉 방향값만 가져야하므로 정규화해주기
}

void Transform::SetLocalRelativeView(const Vector3& _vView)
{
	m_vLocalRelativeView = _vView;
	m_vLocalView = m_vLocalRelativeView.TransformNormal(m_MatrixLocalRotation);
	m_vLocalView.Normalize();
}

// 월드
Matrix Transform::GetWorldRotationMatrix() const
{
	return m_MatrixWorldRotation;
}

Vector3 Transform::GetWorldPosition()	const
{
	return m_vWorldPosition;
}



Vector3 Transform::GetWorldRotation()	const
{
	return m_vWorldRotation;
}

Vector3 Transform::GetWorldScale()		const
{
	return m_vWorldScale;
}

Vector3 Transform::GetWorldAxis(AXIS _Axis)	const
{
	return  m_vWorldAxis[_Axis];
}

Vector3 Transform::GetWorldView()	const
{
	return m_vWorldView;
}

Vector3 Transform::GetMove()	const
{
	return m_vMove;
}

void Transform::SetWorldPosition(float _x, float _y, float _z)
{
	m_vMove += Vector3(_x, _y, _z) - m_vWorldPosition;
	m_vWorldPosition.x = _x;
	m_vWorldPosition.y = _y;
	m_vWorldPosition.z = _z;

	m_MatrixWorldPosition.Translation(m_vWorldPosition);
	m_bUpdate = true;
}

void Transform::SetWorldPosition(const Vector3& _Position)
{
	m_vMove += _Position - m_vWorldPosition;
	m_vWorldPosition = _Position;

	m_MatrixWorldPosition.Translation(m_vWorldPosition);
	m_bUpdate = true;
}

void Transform::SetWorldRotation(float _x, float _y, float _z)
{
	m_vWorldRotation.x = _x;
	m_vWorldRotation.y = _y;
	m_vWorldRotation.z = _z;

	m_MatrixWorldRotation.Rotation(m_vWorldRotation);

	ComputeAxis();

	m_bUpdate = true;
}

void Transform::SetWorldRotation(const Vector3& _Rotation)
{
	m_vWorldRotation = _Rotation;

	m_MatrixWorldRotation.Rotation(m_vWorldRotation);
	ComputeAxis();

	m_bUpdate = true;
}

void Transform::SetWorldRotateX(float _x)
{
	m_vWorldRotation.x = _x;

	m_MatrixWorldRotation.Rotation(m_vWorldRotation);

	ComputeAxis();
	m_bUpdate = true;
}

void Transform::SetWorldRotateY(float _y)
{
	m_vWorldRotation.y = _y;

	m_MatrixWorldRotation.Rotation(m_vWorldRotation);

	ComputeAxis();
	m_bUpdate = true;
}
void Transform::SetWorldRotateZ(float _z)
{
	m_vWorldRotation.z = _z;

	m_MatrixWorldRotation.Rotation(m_vWorldRotation);

	ComputeAxis();
	m_bUpdate = true;
}

void Transform::SetWorldScale(float _x, float _y, float _z)
{
	m_vWorldScale.x = _x;
	m_vWorldScale.y = _y;
	m_vWorldScale.z = _z;

	m_MatrixWorldScale.Scaling(m_vWorldScale);
	m_bUpdate = true;
}

void Transform::SetWorldScaleX(float _x)
{
	m_vWorldScale.x = _x;
	m_MatrixWorldScale.Scaling(m_vWorldScale);
	m_bUpdate = true;
}

void Transform::SetWorldScaleY(float _y)
{
	m_vWorldScale.y = _y;

	m_MatrixWorldScale.Scaling(m_vWorldScale);
	m_bUpdate = true;
}

void Transform::SetWorldScaleZ(float _z)
{
	m_vWorldScale.z = _z;

	m_MatrixWorldScale.Scaling(m_vWorldScale);
	m_bUpdate = true;
}


void Transform::SetWorldScale(const Vector3& _Scale)
{
	m_vWorldScale = _Scale;
	m_MatrixWorldScale.Scaling(m_vWorldScale);
	m_bUpdate = true;
}

void Transform::SetLookAtAxis(LOOKAT_AXIS _eAxis)
{
	m_eLookAxis = _eAxis;
}
#pragma endregion


void Transform::Start()
{

}

bool Transform::Init()
{
	// 기본값 설정
	m_vLocalSacle = Vector3(1.0f, 1.0f, 1.0f);
	m_vWorldScale = Vector3(1.0f, 1.0f, 1.0f);

	for (int i = 0; i < AXIS_END; ++i)
	{
		// 0 = (1.0f, 0.0f, 0.0f)
		// 1 = (0.0f, 1.0f, 0.0f)
		// 2 = (0.0f, 0.0f, 1.0f)
		m_vWorldAxis[i] = Vector3::Axis[i];
	}
	return true;
}

int Transform::Input(float _fTime)
{
	return 0;
}

int Transform::Update(float _fTime)
{
	return 0;
}

int Transform::LateUpdate(float _fTime)
{
	// 이전 행렬
	m_PrevMatrixWolrd = m_MatrixWorld;

	// 최종행렬 계산
	m_MatrixLocal = m_MatrixLocalScale * m_MatrixLocalRotation * m_MatrixLocalPosition;

	// 부모가 있는 경우랑 없는 경우 나눠서 진행
	GameObject* pParent = m_pGameObject->GetParent();
	m_MatrixWorldParent.Identity();
	Matrix pmatParentBone;

	if (nullptr != pParent)
	{
		// 소켓여부
		if (true == m_pGameObject->GetSoketEnable())
		{
			Animation* pParentAni = pParent->FindComponentFromType<Animation>(CT_ANIMATION);

			if (nullptr != pParentAni)
			{
				pmatParentBone = *(pParentAni->FindBone(m_pGameObject->GetSoketBoneNum())->matBone);
			}
			else
			{
				int a = 0;
			}

			SAFE_RELEASE(pParentAni);
		}

		// 부모가 있는 경우 해당 부모의 월드 행렬을 가져온다.
		Transform* pParentTR = pParent->GetTransform();
		Matrix matParentWorld = pParentTR->GetWorldMatrix();
		Matrix matParentLocal = pParentTR->GetLocalMatrix();

		m_MatrixWorldParent = matParentLocal * matParentWorld;
		m_MatrixWorld = m_MatrixWorldScale * m_MatrixWorldRotation * m_MatrixWorldPosition * pmatParentBone * m_MatrixWorldParent;
		//SAFE_RELEASE(pAni);
		SAFE_RELEASE(pParentTR);
		return 0;
	}
	else
	{
		m_MatrixWorld = m_MatrixWorldScale * m_MatrixWorldRotation * m_MatrixWorldPosition;
	}


	return 0;
}

int Transform::Collision(float _fTime)
{
	return 0;
}

int Transform::PrevRender(float _fTime)
{
	Camera* pCamera = nullptr;

	if (true == m_bUI)
	{
		pCamera = m_pScene->GetUICamera();
	}
	else
	{
		pCamera = m_pScene->GetMainCamera();
	}

	// 처음 무조건 한번은 상수버퍼를 갱신해준다. ( m_bUpdate 초기값이 true임)
	// 그 후에 필요할때마다 갱신해주면됨 ( 아래는 상수버퍼 갱신내용임
	// 크기, 위치, 회전값이 바뀌었을 경우마다 해주면됨), 나무같이 정적으로 있는 오브젝트들은 처음 한번만 해주면됨
	if (true == m_pGameObject->GetSoketEnable() || true == m_bUpdate || true == pCamera->GetUpdate())
	{
		// 회전행렬 계산
		m_tConstBuffer.WorldRotationMatrix = m_MatrixLocalRotation * m_MatrixWorldRotation;

		// 월드행렬 계산 ( 로컬행렬에 월드변환행렬을 곱함 )
		m_tConstBuffer.WorldMatrix = m_MatrixLocal * m_MatrixWorld;

		// 월드 뷰 회전(라이트때문에 추가, 같은 공간에 맞춰서 계산하는게 유리해서 뷰공간에서 맞춰주기)
		m_tConstBuffer.WorldViewRotationMatrix = m_tConstBuffer.WorldRotationMatrix * pCamera->GetViewMatrix();

		// 카메라(뷰) 행렬 계산        
		// XMMatrixLookAtLH( 카메라의 위치(보고있는 위치), 바라보는 방향, 업벡터 )
		m_tConstBuffer.ViewMatrix = pCamera->GetViewMatrix();

		m_tConstBuffer.InversViewMatrix = m_tConstBuffer.ViewMatrix;
		m_tConstBuffer.InversViewMatrix.Inverse();

		// 프로젝션 행렬 계산 (시야각(우린 90도), 종횡비(float로한 이유는 i는 인트라서 소수점이 짤림 그럼 정확하지 않음), near, far)  
		//m_tConstBuffer.ProjectionMatrix = XMMatrixPerspectiveFovLH(ENGINE_PI / 2.0f,
		//	_RESOLUTION.iWidth / (float)_RESOLUTION.iHeight, 0.03f, 1000.0f);
		m_tConstBuffer.ProjectionMatrix = pCamera->GetProjectionMatrix();

		// 프로젝션 행렬의 역행렬 계산
		m_tConstBuffer.InversProjectionMatrix = m_tConstBuffer.ProjectionMatrix;
		m_tConstBuffer.InversProjectionMatrix.Inverse();

		// World View
		m_tConstBuffer.WorldViewMatrix = m_tConstBuffer.WorldMatrix * m_tConstBuffer.ViewMatrix;

		// World View Projection
		m_tConstBuffer.WorldViewProjectionMatrix = m_tConstBuffer.WorldViewMatrix * m_tConstBuffer.ProjectionMatrix;

		// View Projection
		m_tConstBuffer.ViewProjectionMatrix = m_tConstBuffer.ViewMatrix * m_tConstBuffer.ProjectionMatrix;

		// 전치행렬 ( 열과 행을 바꿔준다. )
		// 왜 전치를 했냐면
		// 메모리를 읽는 구조가  CPU의 경우엔 가로로 읽는데 GPU의 경우엔 세로로 읽기 때문이다.
		// 셰이더 내부에서 전치를 해주면 연산이 느리기때문에 C++에서 해준다. ( 픽셀개수만큼 연산을 하기 떄문)
		m_tConstBuffer.WorldRotationMatrix.Transpose();
		m_tConstBuffer.WorldMatrix.Transpose();
		m_tConstBuffer.WorldViewRotationMatrix.Transpose();
		m_tConstBuffer.ViewMatrix.Transpose();
		m_tConstBuffer.InversViewMatrix.Transpose();
		m_tConstBuffer.ProjectionMatrix.Transpose();
		m_tConstBuffer.InversProjectionMatrix.Transpose();
		m_tConstBuffer.WorldViewMatrix.Transpose();
		m_tConstBuffer.WorldViewProjectionMatrix.Transpose();
		m_tConstBuffer.ViewProjectionMatrix.Transpose();

	}

	SAFE_RELEASE(pCamera);
	//m_bUpdate = false;

	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("Transform", &m_tConstBuffer);

	return 0;
}

int Transform::Render(float _fTime)
{
	m_vMove = Vector3::Zero;
	m_bUpdate = false;
	return 0;
}

Component* Transform::Clone()	const
{
	return new Transform(*this);
}


//

void Transform::Move(AXIS _eAxis, float _fSpeed, float _fTime)
{
	m_vMove += m_vWorldAxis[_eAxis] * _fSpeed * _fTime;
	m_vWorldPosition += m_vWorldAxis[_eAxis] * _fSpeed * _fTime;
	m_MatrixWorldPosition.Translation(m_vWorldPosition);
	m_bUpdate = true;
}

void Transform::Move(const Vector3 & _vDir, float _fSpeed, float _fTime)
{
	m_vMove += _vDir * _fSpeed * _fTime;
	m_vWorldPosition += _vDir * _fSpeed * _fTime;
	m_MatrixWorldPosition.Translation(m_vWorldPosition);
	m_bUpdate = true;
}

void Transform::Move(const Vector3 & _vMove)
{
	m_vMove += _vMove;
	m_vWorldPosition += _vMove;

	m_MatrixWorldPosition.Translation(m_vWorldPosition);
	
	m_bUpdate = true;		// 이동시켰으니까 활성화
}

void Transform::MoveBack()
{
	m_vWorldPosition -= m_vMove;
	m_vMove = Vector3::Zero;

	m_MatrixWorldPosition.Translation(m_vWorldPosition);

	m_bUpdate = true;
}



void Transform::Rotate(const Vector3 & _vRot, float _fTime)
{
	m_vWorldRotation += _vRot * _fTime;

	m_MatrixWorldRotation.Rotation(m_vWorldRotation);
	ComputeAxis();
	m_bUpdate = true;
}

void Transform::Rotate(const Vector3 & _vRot)
{
	m_vWorldRotation += _vRot;
	m_MatrixWorldRotation.Rotation(m_vWorldRotation);
	ComputeAxis();
	m_bUpdate = true;
}

void Transform::RotateX(float _fSpeed, float _fTime)
{
	m_vWorldRotation.x += _fSpeed * _fTime;
	m_MatrixWorldRotation.Rotation(m_vWorldRotation);
	ComputeAxis();
	m_bUpdate = true;
}

void Transform::RotateX(float _fSpeed)
{
	m_vWorldRotation.x += _fSpeed;
	m_MatrixWorldRotation.Rotation(m_vWorldRotation);
	ComputeAxis();
	m_bUpdate = true;
}

void Transform::RotateY(float _fSpeed, float _fTime)
{
	m_vWorldRotation.y += _fSpeed * _fTime;
	m_MatrixWorldRotation.Rotation(m_vWorldRotation);
	ComputeAxis();
	m_bUpdate = true;
}

void Transform::RotateY(float _fSpeed)
{
	m_vWorldRotation.y += _fSpeed;
	m_MatrixWorldRotation.Rotation(m_vWorldRotation);
	ComputeAxis();
	m_bUpdate = true;
}


void Transform::RotateZ(float _fSpeed, float _fTime)
{
	m_vWorldRotation.z += _fSpeed * _fTime;
	m_MatrixWorldRotation.Rotation(m_vWorldRotation);
	ComputeAxis();
	m_bUpdate = true;
}

void Transform::RotateZ(float _fSpeed)
{
	m_vWorldRotation.z += _fSpeed;
	m_MatrixWorldRotation.Rotation(m_vWorldRotation);
	ComputeAxis();
	m_bUpdate = true;
}

void Transform::LookAt(GameObject * _pObject)
{
	// GetTransform()하는 순간 레퍼런스 카운터가 증가하니 기존에 있던건 Release를 통해 차감해준다.
	SAFE_RELEASE(m_pLookAt);
	m_pLookAt = _pObject->GetTransform();
	m_bLookAt = true;
}

void Transform::LookAt(Component * _pComponent)
{
	// 해당 컴포넌트 타입이 트랜스폼이라면
	if (_pComponent->GetComponentType() == CT_TRANSFORM)
	{
		// 해당컴포넌트 타입이 트랜스폼이라면 GetTransform()할것 없이 바로 넣어준다.
		// 그러므로 레퍼런스 카운터를 1증가시킨다.
		m_pLookAt = (Transform*)_pComponent;
		_pComponent->AddReference();
		m_bLookAt = true;
	}
	else
	{
		SAFE_RELEASE(m_pLookAt);
		m_pLookAt = _pComponent->GetTransform();
		m_bLookAt = true;
	}
}

void Transform::RemoveLookAt()
{
	SAFE_RELEASE(m_pLookAt);			// LookAt을 지워주므로 해당 레퍼런스 카운터를 감소시킨다.
}

void Transform::RotationLookAt()
{
	// 바라보는 대상이 없으면 return;
	if (nullptr == m_pLookAt || false == m_bLookAt)
	{
		return;
	}
	RotationLookAt(m_pLookAt->GetWorldPosition());
}

void Transform::SetLookAtActive(bool _Active)
{
	m_bLookAt = _Active;
}

void Transform::RotationLookAt(Vector3 _vLookAt)
{
	// 바라볼 방향을 구한다.
	Vector3	vView = m_vLocalView;
	Vector3	vPos = m_vWorldPosition;

	switch (m_eLookAxis)
	{
	case LOOK_X:
		_vLookAt.x = 0.f;
		vPos.x = 0.f;
		vView.x = 0.f;
		break;
	case LOOK_Y:
		_vLookAt.y = 0.f;
		vPos.y = 0.f;
		vView.y = 0.f;
		break;
	case LOOK_Z:
		_vLookAt.z = 0.f;
		vPos.z = 0.f;
		vView.z = 0.f;
		break;
	}


	// 여기서 정말 해당 벡터를 바라보게 한다.
	// 먼저 바라볼 방향을 구하고 정규화를 통해 방향값만 구한다.
	Vector3 vDir = _vLookAt - vPos;
	vDir.Normalize();

	// 뷰와 바라볼 방향을 외적하면 회전축을 알 수 있다. (수직인 벡터가 나옴, 그거에따라 음의방향인지 양의방향인지도 알수 있음)
	Vector3 vRotationAxis = m_vLocalView.Cross(vDir);
	vRotationAxis.Normalize();

	if (0.0001f >= vRotationAxis.x && 0.0001f >= vRotationAxis.y && 0.0001f >= vRotationAxis.z)
	{
		return;
	}

	// 뷰와 바라볼 방향을 내적한 후 아크코사인을 곱하면 세타값(각도값)이 나온다.
	// 방향벡터이기 때문에 크기가 1이므로 'ㅅ' cos세타값이 나옴
	float fAngle = m_vLocalView.Angle(vDir);
	m_MatrixWorldRotation.RotationAxis(fAngle, vRotationAxis);

	ComputeAxis();			// 축전환 

	m_bUpdate = true;		// 행렬변환을 했으니 업데이트를 해야한다. 
}

void Transform::ComputeAxis()
{
	// 축계산
	// 월드만 적용한다. 로컬은 축이 틀어질 수 있기때문
	for (int i = 0; i < AXIS_END; ++i)
	{
		// 지금은 계층 구조는 생각하지 않음 ㅇㅅㅇ 나중에 추가해줄거
		m_vWorldAxis[i] = Vector3::Axis[i].TransformNormal(m_MatrixWorldRotation);
		m_vWorldAxis[i].Normalize();
	}

	m_vWorldView = m_vLocalView.TransformNormal(m_MatrixWorldRotation);
	m_vWorldView.Normalize();
}

Matrix Transform::GetWorldMatrix() const
{
	return m_MatrixWorld;
}

Matrix Transform::GetLocalRotationMatrix() const
{
	return m_MatrixLocalRotation;
}


Matrix Transform::GetLocalMatrix() const
{
	return m_MatrixLocal;
}

int Transform::PrevShadowRender(Matrix _matLightView, Matrix _matLightProj, float _fTime)
{
	Camera* pCamera = nullptr;

	if (true == m_bUI)
	{
		pCamera = m_pScene->GetUICamera();
	}
	else
	{
		pCamera = m_pScene->GetMainCamera();
	}

	if (true == m_bUpdate || true == pCamera->GetUpdate())
	{
		// 회전행렬 계산
		m_tShadowCBuffer.WorldRotationMatrix = m_MatrixLocalRotation * m_MatrixWorldRotation;

		// 월드행렬 계산 ( 로컬행렬에 월드변환행렬을 곱함 )
		m_tShadowCBuffer.WorldMatrix = m_MatrixLocal * m_MatrixWorld;
		m_tShadowCBuffer.WorldViewRotationMatrix = m_tShadowCBuffer.WorldRotationMatrix *_matLightView;

		// 카메라(뷰) 행렬 계산        
		m_tShadowCBuffer.ViewMatrix = _matLightView;
		m_tShadowCBuffer.ProjectionMatrix = _matLightProj;

		m_tShadowCBuffer.InversViewMatrix = m_tShadowCBuffer.ViewMatrix;
		m_tShadowCBuffer.InversViewMatrix.Inverse();

		// 프로젝션 행렬의 역행렬 계산
		m_tShadowCBuffer.InversProjectionMatrix = m_tShadowCBuffer.ProjectionMatrix;
		m_tShadowCBuffer.InversProjectionMatrix.Inverse();

		// World View
		m_tShadowCBuffer.WorldViewMatrix = m_tShadowCBuffer.WorldMatrix * m_tShadowCBuffer.ViewMatrix;

		// World View Projection
		m_tShadowCBuffer.WorldViewProjectionMatrix = m_tShadowCBuffer.WorldViewMatrix * m_tShadowCBuffer.ProjectionMatrix;

		// View Projection
		m_tShadowCBuffer.ViewProjectionMatrix = m_tShadowCBuffer.ViewMatrix * m_tShadowCBuffer.ProjectionMatrix;

		// 전치행렬 ( 열과 행을 바꿔준다. )
		m_tShadowCBuffer.WorldRotationMatrix.Transpose();
		m_tShadowCBuffer.WorldMatrix.Transpose();
		m_tShadowCBuffer.WorldViewRotationMatrix.Transpose();
		m_tShadowCBuffer.ViewMatrix.Transpose();
		m_tShadowCBuffer.InversViewMatrix.Transpose();
		m_tShadowCBuffer.ProjectionMatrix.Transpose();
		m_tShadowCBuffer.InversProjectionMatrix.Transpose();
		m_tShadowCBuffer.WorldViewMatrix.Transpose();
		m_tShadowCBuffer.WorldViewProjectionMatrix.Transpose();
		m_tShadowCBuffer.ViewProjectionMatrix.Transpose();
	}


	SAFE_RELEASE(pCamera);
	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("Transform", &m_tShadowCBuffer);

	return 0;
}

Matrix Transform::GetWorldPositionMatrix() const
{
	return m_MatrixWorldPosition;
}

Matrix Transform::GetLocalPositionMatrix() const
{
	return m_MatrixLocalPosition;
}

Vector3 Transform::GetWorldPositionAtMatrix() const
{
	return Vector3(m_MatrixWorld._41, m_MatrixWorld._42, m_MatrixWorld._43);
}

Vector3 Transform::GetPrevWorldPosiitonAtMatrix() const
{
	return Vector3(m_PrevMatrixWolrd._41, m_PrevMatrixWolrd._42, m_PrevMatrixWolrd._43);
}
