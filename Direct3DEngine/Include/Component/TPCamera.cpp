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
		m_vDistance = m_pTarget->GetWorldPosition() - m_pTransform->GetWorldPosition();		// ���� ������
	}
}

void TPCamera::SetTarget(Component * _pTarget)
{
	SAFE_RELEASE(m_pTarget);
	m_pTarget = _pTarget->GetTransform();

	if (0.0f == m_vDistance.Length())
	{
		m_vDistance = m_pTarget->GetWorldPosition() - m_pTransform->GetWorldPosition();		// ���� ������
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
	// ȸ�� (R��ư�� �ش�)
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

		// �����������ϸ� ������ �����ϴ�.
		// 3��Ī ī�޶�� ������ �ؾ��ϱ� ������ �ش� ��ü���� �Ÿ��� �ϴ�.
		// �Ÿ��� ���, 3��Īī�޶� ȸ������ �븻����ŭ Ÿ�� ��ġ�� ���ϰ� �Ÿ���ŭ ����ָ�ȴ�.
		// -1.0f�� ���� ������ Ÿ���� �ٶ�����ϱ⶧���� �������� �ʿ��ϱ� �����̴�.

		float fDist = m_pTransform->GetWorldPosition().Distance(m_pTarget->GetWorldPositionAtMatrix());
		m_pTransform->SetWorldPosition(m_pTarget->GetWorldPositionAtMatrix() + m_pTransform->GetWorldAxis(AXIS_Z) * -fDist);

	}


	// Zoom In / Out
	if (true == m_bMouseEnable)
	{
		short sWheel = GET_SINGLETON(InputManager)->GetWheelDir();		// ���콺 �� ����

		// 0�� �ƴ� ���� ���콺 ���� �����̿��ٴ� �Ҹ�
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
			// ���ݵ�.. ( �Ǻ� ��ġ ����Ǵ� �ִϸ��̼ǵ� )
			vMove = m_pTarget->GetWorldPositionAtMatrix() - m_pTarget->GetPrevWorldPosiitonAtMatrix();
		}
		else
		{
			if (true == m_bBaseNoneMove)
			{
				// �ƿ� �ȿ����̴°� - Idle, Wait
				vMove = 0.0f;
				//m_bSoketMove = true;
			}
			else
			{
				// �޸��°� 
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
		// Ÿ���� ������ ��� ���� �����δ�.
		vMove = m_pTarget->GetMove();
	}


	// ������ ���w
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

