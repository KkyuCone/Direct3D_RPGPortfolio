#include "Navigation.h"
#include "Transform.h"
#include "..\Scene\Scene.h"
#include "..\Scene\SceneManager.h"
#include "..\InputManager.h"
#include "..\GameObject.h"
#include "..\Component\ColliderRay.h"
#include "..\Navigation\NavigationManager.h"
#include "..\Navigation\NavigationMesh.h"
#include "Camera.h"

ENGINE_USING

Navigation::Navigation() :
	m_bOnCellYPosition(true),
	m_bMouseMove(false),
	m_bAI(false),
	m_pTarget(nullptr),
	m_bMousePosCheck(false),
	m_bddddddddddddddd(false),
	m_pNavMesh(nullptr),
	m_fTargetDetectTime(0.0f),
	m_fTargetCheckTime(0.0f)
{
	m_eComponentType = CT_NAVIGATION;
}

Navigation::Navigation(const Navigation & _Com) : Component(_Com)
{
	m_bOnCellYPosition = _Com.m_bOnCellYPosition;
	m_bMouseMove = _Com.m_bMouseMove;
	m_bAI = _Com.m_bMouseMove;
	m_pTarget = nullptr;
	m_fTargetDetectTime = 0.0f;
}


Navigation::~Navigation()
{
	SAFE_RELEASE(m_pTarget);
}


Vector3 Navigation::GetMousePos() const
{
	return m_vMousePos;
}

void Navigation::SetCellYPosition(bool _bEnable)
{
	m_bOnCellYPosition = _bEnable;
}

void Navigation::SetMouseMove(bool _bEnable)
{
	m_bMouseMove = _bEnable;
}

void Navigation::SetAIFindPath(bool _bEnable)
{
	m_bAI = _bEnable;

	//AI�� �ƴϸ� ��� ã�� �ʿ� �����ϱ� �����ش�.
	if (false == m_bAI)
	{
		m_PathList.clear();
		m_fTargetDetectTime = 0.0f;
	}
	else if (nullptr != m_pTarget)
	{
		// �ش� Ÿ���� nullptr�� �ƴϸ�  Ÿ���� ��ġ�� �־��ش�.
		m_vTargetPos = m_pTarget->GetWorldPosition();
	}
}

void Navigation::SetTarget(GameObject * _pTarget)
{
	SAFE_RELEASE(m_pTarget);

	if (nullptr != _pTarget)
	{
		m_pTarget = _pTarget->GetTransform();
		m_vTargetPos = m_pTarget->GetWorldPosition();
	}
	else
	{
		m_pTarget = nullptr;
		m_vTargetPos = Vector3::Zero;
	}
	m_fTargetDetectTime = 0.0f;				// Ÿ�ٹٲٸ� �ð� �ٽ� �������ִ� �Լ� ȣ������ ������!
}

void Navigation::SetTarget(Component * _pTarget)
{
	SAFE_RELEASE(m_pTarget);

	if (nullptr != _pTarget)
	{
		m_pTarget = _pTarget->GetTransform();
		m_vTargetPos = m_pTarget->GetWorldPosition();
	}
	else
	{
		m_pTarget = nullptr;
		m_vTargetPos = Vector3::Zero;
	}

	m_fTargetDetectTime = 0.0f;
}

void Navigation::SetTargetDetectTime(float _fTime)
{
	if (0 > _fTime)
		return;

	m_fTargetCheckTime = _fTime;
}

//////////////////////////////////////////////////////////////

void Navigation::Start()
{
	m_vMousePos = Vector3::Zero;
}

bool Navigation::Init()
{
	return true;
}

int Navigation::Input(float _fTime)
{
	return 0;
}

int Navigation::Update(float _fTime)
{
	// ���⼭ ���� �浹 ��ġ �˾ƿ��� ( ���콺 ���̰� ���� ��츸 üũ�Ѵ�. )
	// ���콺 �˾ƿ���
	if (true == m_bMouseMove)
	{
		CheckMousePos();

		if (true == m_bMousePosCheck)
		{
			CheckAIPos(m_vMousePos, _fTime);
		}
		else
		{
			return 0;
		}

		// ��ΰ� ��� ���� ��� ���� Ž���Ѵ�.
		if (true == m_PathList.empty())
		{
			FindPath();
		}

		MoveAI(_fTime);


	}


	// AI ��� ã�� ( �Ϲ� ������ �ֵ� )
	if (true == m_bAI && nullptr != m_pTarget)
	{
		if (nullptr != m_pTarget)
		{
			Vector3 vPos = m_pTarget->GetWorldPosition();
			CheckAIPos(vPos, _fTime);
		}

		// ��ΰ� ��� ���� ��� ���� Ž���Ѵ�.
		if (true == m_PathList.empty())
		{
			FindPath();
		}

		MoveAI(_fTime);

	}

	return 0;
}

int Navigation::LateUpdate(float _fTime)
{
	// ���������� Y���� true�̸� y�� ����
	if (true == m_bOnCellYPosition)
	{
		Vector3   vPos = m_pTransform->GetWorldPosition();
		NavigationMesh* pNavMesh = GET_SINGLETON(NavigationManager)->FindNavMesh(m_pScene,
			vPos);

		if (nullptr != pNavMesh)
		{
			float NY = pNavMesh->GetY(vPos);

			if (pNavMesh->GetCellEnable(vPos) || NY < vPos.y)
			{
				vPos.y = pNavMesh->GetY(vPos);

				m_pTransform->SetWorldPosition(vPos);
			}
			else
			{
				// �����̵����� ó��
				PNavigationCell NCell = pNavMesh->FindCell(vPos);

				Vector3 P0 = NCell->vPos[0];
				Vector3 P1 = NCell->vPos[1];
				Vector3 P2 = NCell->vPos[2];
				Vector3 Move = m_pTransform->GetMove();
				//Vector3 MoveR = m_pTransform->GetMove();

				P1 = P1 - P0;
				P2 = P2 - P0;

				Vector3 NomalV = P1.Cross(P2);

				if (NomalV.Dot(Move)  > 0.0f)
				{
					NomalV = Vector3::Zero - NomalV;
				}

				NomalV.y = 0.0f;
				NomalV.Normalize();

				float Size = NomalV.Dot(Move);
				Size = Size * 1.3f;

				Move.y = 0.0f;
				Move = Move - (NomalV * Size);
				m_pTransform->MoveBack();
				m_pTransform->Move(Move);

				Vector3   vPosS = m_pTransform->GetWorldPosition();

				NY = pNavMesh->GetY(vPosS);

				if (pNavMesh->GetCellEnable(vPosS) || NY < vPosS.y)
				{
					if ( 70.f <= pNavMesh->GetFaceNormalAngle(vPos))
					{
						m_pTransform->MoveBack();
						SAFE_RELEASE(pNavMesh);
						return 0;
					}

					vPosS.y = pNavMesh->GetY(vPosS);
					m_pTransform->SetWorldPosition(vPosS);
				}
				else
				{
					m_pTransform->MoveBack();
					m_pTransform->Move(Move * -1.0f);
					vPosS = m_pTransform->GetWorldPosition();
					vPosS.y = pNavMesh->GetY(vPosS);
					m_pTransform->SetWorldPosition(vPosS);
				}

			}
			SAFE_RELEASE(pNavMesh);
		}
	}

	return 0;
}

int Navigation::Collision(float _fTime)
{
	return 0;
}

int Navigation::PrevRender(float _fTime)
{
	return 0;
}

int Navigation::Render(float _fTime)
{
	return 0;
}

Navigation * Navigation::Clone() const
{
	return new Navigation(*this);
}

// ���콺 ���� ������ �̵�
void Navigation::CheckMousePos()
{
	// ���콺�� �ȴ����� ���¸� �����ʿ� ���� ������
	if (false == GET_SINGLETON(InputManager)->MousePress(MB_LBUTTON))
		return;

	Scene* pCurScene = GET_SINGLETON(SceneManager)->GetScene();				// ����� �޾ƿ���
	ColliderRay* pMouseRay = GET_SINGLETON(InputManager)->GetMouseRay();	// ���콺 ���� �޾ƿ���

	// ���콺 ���̿� �׺�޽��� ���� ������� �������� ����Ѵ�.
	if (nullptr != pMouseRay && pCurScene == m_pScene)
	{
		// 1. ���콺 �����̶� ���������� ���ؼ� �浹������ �κи� �߷�����
		
		// 1-1. ���콺 ���� ��������
		RayInfo sRayInfo = pMouseRay->GetInfo();
		// 1-2. ���������� ���ؼ� �浹
		if (0 == GET_SINGLETON(NavigationManager)->MouseNavSectionCount(m_pScene, sRayInfo))
		{
			m_bMousePosCheck = false;
			SAFE_RELEASE(pMouseRay);
			return;
		}


		// 2. �浹�� ������ ���콺 �����̶� �ﰢ�� �浹�ϱ�
		// 3. �����ؼ�, ���� ����� �ﰢ���������� (vCenter)
		m_vMousePos = GET_SINGLETON(NavigationManager)->MouseNavPosition(m_pScene, sRayInfo);


		// 4. �ű� �ﰢ������ �̵��ϱ� (Ŭ����)

		if (INT_MAX == m_vMousePos.x)
		{
			m_PathList.clear();
			m_bMousePosCheck = false;
			SAFE_RELEASE(pMouseRay);
			return;
		}

		if (true == m_bMousePosCheck)
		{
			m_bddddddddddddddd = true;
		}

		m_bMousePosCheck = true;
		//m_pTransform->SetWorldPosition(m_vMousePos);
	}

	SAFE_RELEASE(pMouseRay);
	return;
}

void Navigation::CheckAIPos(Vector3 _vPos, float _fTime)
{
	m_fTargetDetectTime += _fTime;

	// ��� üũ�� �ð� üũ�� m_fTargetCheckTime�� ���ݸ���
	if (m_fTargetDetectTime >= m_fTargetCheckTime)
	{
		m_fTargetDetectTime -= m_fTargetCheckTime;

		Vector3 vTargetPos = _vPos;

		if (m_vTargetPos != vTargetPos)
		{
			m_vTargetPos = vTargetPos;
			m_PathList.clear();
			m_vWay = Vector3::Zero;
		}
	}
}

void Navigation::MoveAI(float _fTime)
{
	// ���� ã��
	Vector3 vDir = m_vWay - m_pTransform->GetWorldPosition();
	vDir.Normalize();

	// �Ÿ� ���ϱ�
	float fDist = m_vWay.Distance(m_pTransform->GetWorldPosition());

	// �ӵ�
	float fSpeed = 3.0f * _fTime;

	if (fDist < fSpeed)
	{
		fSpeed = fDist;			// 3���� ����ó�������� 3���� �Ÿ����̰� ������ �ٷ� �� �ڸ��� �̵���

								// �ش� ��ġ�� �����Ƿ� ������ ��ε��� ���� ��� �����ش�.
		if (false == m_PathList.empty())
		{
			m_vWay = m_PathList.front();
			m_PathList.pop_front();
		}
		else
		{
			// ���� ��ΰ� ���� ���
			m_vWay = Vector3::Zero;
			m_bAI = false;
			m_bMousePosCheck = false;
			m_PathList.clear();

			if (true == m_bMouseMove)
			{
				m_pTransform->SetWorldPosition(m_vMousePos);
				m_bddddddddddddddd = false;
			}
			else
			{
				m_pTransform->SetWorldPosition(m_pTarget->GetWorldPosition());
			}
			return;
		}
	}

	m_pTransform->Move(vDir, 3.0f, _fTime);		// �̵��ϱ�

	if (nullptr != m_pNavMesh)
	{
		if (0 < m_pNavMesh->GetPathList().size())
		{
			m_pNavMesh->PathListClear();
		}
	}
}

bool Navigation::FindPath()
{
	NavigationMesh* pNavMesh = GET_SINGLETON(NavigationManager)->FindNavMesh(m_pScene, m_pTransform->GetWorldPosition());

	if (nullptr == pNavMesh)
	{
		m_pNavMesh = nullptr;
		m_PathList.clear();
		return false;
	}

	m_PathList.clear();

	if (true == m_bMouseMove)
	{
		if (true == m_bddddddddddddddd)
		{
			m_PathList.clear();
			m_bddddddddddddddd = false;
		}

		pNavMesh->FindPath(m_pTransform->GetWorldPosition(), m_vMousePos);
	}
	else
	{
		pNavMesh->FindPath(m_pTransform->GetWorldPosition(), m_pTarget->GetWorldPosition());
	}

	m_pNavMesh = pNavMesh;
	m_PathList = pNavMesh->GetPathList();
	if (0 < m_PathList.size())
	{
		m_vWay = m_PathList.front();		// �̵��� ��ġ
		m_PathList.pop_front();
	}

	SAFE_RELEASE(pNavMesh);
	return true;
}
