#include "TPCamera.h"
#include "..\GameObject.h"
#include "Transform.h"
#include "..\InputManager.h"

ENGINE_USING

TPCamera::TPCamera() :
	m_vDistance(Vector3::Zero),
	m_pTarget(nullptr),
	m_bMouseEnable(false),
	m_bSoketMove(false),
	m_bBaseNoneMove(true)
{
	m_eComponentType = CT_TPCAMERA;
}

TPCamera::TPCamera(const TPCamera & _Com) : Component(_Com)
{
	m_iReferenceCount = 1;
	m_pTarget = nullptr;
	m_bMouseEnable = _Com.m_bMouseEnable;
}


TPCamera::~TPCamera()
{
	SAFE_RELEASE(m_pTarget);
}

void TPCamera::SetTarget(GameObject * _pTarget)
{
	SAFE_RELEASE(m_pTarget);
	m_pTarget = _pTarget->GetTransform();

	if (0.0f == m_vDistance.Length())
	{
		m_vDistance = m_pTarget->GetWorldPosition() - m_pTransform->GetWorldPosition();		// 방향 구했음
	}
}

void TPCamera::SetTarget(Component * _pTarget)
{
	SAFE_RELEASE(m_pTarget);
	m_pTarget = _pTarget->GetTransform();

	if (0.0f == m_vDistance.Length())
	{
		m_vDistance = m_pTarget->GetWorldPosition() - m_pTransform->GetWorldPosition();		// 방향 구했음
	}
}

void TPCamera::SetDistance(const Vector3 & _vDistance)
{
	m_vDistance = _vDistance;
}

void TPCamera::SetMouseEnable(bool _bEnable)
{
	m_bMouseEnable = _bEnable;
}

void TPCamera::SetSoketMoveEnable(bool _bEnable)
{
	m_bSoketMove = _bEnable;
}

void TPCamera::SetBaseNoneMoveEnable(bool _bEnable)
{
	m_bBaseNoneMove = _bEnable;
}

void TPCamera::Start()
{
}

bool TPCamera::Init()
{
	return true;
}

int TPCamera::Input(float _fTime)
{
	return 0;
}

int TPCamera::Update(float _fTime)
{
	// 회전 (R버튼만 해당)
	if (true == GET_SINGLETON(InputManager)->MousePush(MB_RBUTTON))
	{
		Vector2 vMove = GET_SINGLETON(InputManager)->GetMouseViewportMove();

		if (0.0 != vMove.x)
		{
			m_pTransform->RotateY(vMove.x);
		}

		if (0.0f != vMove.y)
		{
			m_pTransform->RotateX(vMove.y);
		}

		// 위에까지만하면 자전만 가능하다.
		// 3인칭 카메라는 공전을 해야하기 때문에 해당 물체와의 거리도 하다.
		// 거리를 재고, 3인칭카메라가 회전후의 노말값만큼 타겟 위치에 더하고 거리만큼 띄워주면된다.
		// -1.0f을 곱한 이유는 타겟을 바라봐야하기때문에 역방향이 필요하기 때문이다.

		float fDist = m_pTransform->GetWorldPosition().Distance(m_pTarget->GetWorldPositionAtMatrix());
		m_pTransform->SetWorldPosition(m_pTarget->GetWorldPositionAtMatrix() + m_pTransform->GetWorldAxis(AXIS_Z) * -fDist);

	}


	// Zoom In / Out
	if (true == m_bMouseEnable)
	{
		short sWheel = GET_SINGLETON(InputManager)->GetWheelDir();		// 마우스 휠 방향

		// 0이 아닌 경우는 마우스 휠을 움직이였다는 소리
		if (0 != sWheel)
		{
			m_pTransform->Move(AXIS_Z, 20.0f * sWheel, _fTime);
		}
	}

	return 0;
}

int TPCamera::LateUpdate(float _fTime)
{
	GameObject* pTargetObj = m_pTarget->GetGameObject();
	GameObject* pParentObj = pTargetObj->GetParent();
	Vector3 vMove = Vector3::Zero;

	if (nullptr != pParentObj)
	{
		if (true == m_bSoketMove)
		{
			// 공격들.. ( 피봇 위치 변경되는 애니메이션들 )
			vMove = m_pTarget->GetWorldPositionAtMatrix() - m_pTarget->GetPrevWorldPosiitonAtMatrix();
		}
		else
		{
			if (true == m_bBaseNoneMove)
			{
				// 아예 안움직이는거 - Idle, Wait
				vMove = 0.0f;
				//m_bSoketMove = true;
			}
			else
			{
				// 달리는거 
				GameObject* pTargetObj = m_pTarget->GetGameObject();
				GameObject* pParentObj = pTargetObj->GetParent();
				Transform* pParenetTR = pParentObj->GetTransform();

				vMove = pParenetTR->GetMove();

				SAFE_RELEASE(pTargetObj);
				SAFE_RELEASE(pParenetTR);
			}
		}
	}
	else
	{
		// 타겟이 움직일 경우 따라서 움직인다.
		vMove = m_pTarget->GetMove();
	}


	// 움직인 경우w
	if (0.0f != vMove.Length())
	{
		m_pTransform->Move(vMove);
	}

	SAFE_RELEASE(pTargetObj);
	return 0;
}

int TPCamera::Collision(float _fTime)
{
	return 0;
}

int TPCamera::PrevRender(float _fTime)
{
	return 0;
}

int TPCamera::Render(float _fTime)
{
	return 0;
}

int TPCamera::RenderShadow(float _fTime)
{
	return 0;
}

TPCamera * TPCamera::Clone() const
{
	return new TPCamera(*this);
}

