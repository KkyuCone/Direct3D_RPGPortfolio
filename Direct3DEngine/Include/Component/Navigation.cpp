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

	//AI가 아니면 경로 찾을 필요 없으니까 지워준다.
	if (false == m_bAI)
	{
		m_PathList.clear();
		m_fTargetDetectTime = 0.0f;
	}
	else if (nullptr != m_pTarget)
	{
		// 해당 타겟이 nullptr이 아니면  타겟의 위치를 넣어준다.
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
	m_fTargetDetectTime = 0.0f;				// 타겟바꾸면 시간 다시 설정해주는 함수 호출해줘 ㅇㅅㅇ!
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
	// 여기서 현재 충돌 위치 알아오기 ( 마우스 레이가 있을 경우만 체크한다. )
	// 마우스 알아오기
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

		// 경로가 비어 있을 경우 길을 탐색한다.
		if (true == m_PathList.empty())
		{
			FindPath();
		}

		MoveAI(_fTime);


	}


	// AI 경로 찾기 ( 일반 몹같은 애들 )
	if (true == m_bAI && nullptr != m_pTarget)
	{
		if (nullptr != m_pTarget)
		{
			Vector3 vPos = m_pTarget->GetWorldPosition();
			CheckAIPos(vPos, _fTime);
		}

		// 경로가 비어 있을 경우 길을 탐색한다.
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
	// 셀포지션의 Y값이 true이면 y값 적용
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
				// 슬라이딩벡터 처리
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

// 마우스 찍은 곳으로 이동
void Navigation::CheckMousePos()
{
	// 마우스가 안눌려진 상태면 해줄필요 없다 ㅇㅅㅇ
	if (false == GET_SINGLETON(InputManager)->MousePress(MB_LBUTTON))
		return;

	Scene* pCurScene = GET_SINGLETON(SceneManager)->GetScene();				// 현재씬 받아오고
	ColliderRay* pMouseRay = GET_SINGLETON(InputManager)->GetMouseRay();	// 마우스 레이 받아오고

	// 마우스 레이와 네비메쉬의 씬과 현재씬이 같을때만 계산한다.
	if (nullptr != pMouseRay && pCurScene == m_pScene)
	{
		// 1. 마우스 광선이랑 지형공간을 비교해서 충돌가능한 부분만 추려내기
		
		// 1-1. 마우스 광선 가져오기
		RayInfo sRayInfo = pMouseRay->GetInfo();
		// 1-2. 지형공간을 비교해서 충돌
		if (0 == GET_SINGLETON(NavigationManager)->MouseNavSectionCount(m_pScene, sRayInfo))
		{
			m_bMousePosCheck = false;
			SAFE_RELEASE(pMouseRay);
			return;
		}


		// 2. 충돌한 공간만 마우스 광선이랑 삼각형 충돌하기
		// 3. 정렬해서, 가장 가까운 삼각형가져오기 (vCenter)
		m_vMousePos = GET_SINGLETON(NavigationManager)->MouseNavPosition(m_pScene, sRayInfo);


		// 4. 거기 삼각형으로 이동하기 (클릭시)

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

	// 경로 체크할 시간 체크함 m_fTargetCheckTime초 간격마다
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
	// 방향 찾기
	Vector3 vDir = m_vWay - m_pTransform->GetWorldPosition();
	vDir.Normalize();

	// 거리 구하기
	float fDist = m_vWay.Distance(m_pTransform->GetWorldPosition());

	// 속도
	float fSpeed = 3.0f * _fTime;

	if (fDist < fSpeed)
	{
		fSpeed = fDist;			// 3정도 오차처리ㅇㅅㅇ 3정도 거리차이가 있으면 바로 그 자리로 이동ㅎ

								// 해당 위치에 있으므로 나머지 경로들이 있을 경우 지워준다.
		if (false == m_PathList.empty())
		{
			m_vWay = m_PathList.front();
			m_PathList.pop_front();
		}
		else
		{
			// 남은 경로가 없을 경우
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

	m_pTransform->Move(vDir, 3.0f, _fTime);		// 이동하기

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
		m_vWay = m_PathList.front();		// 이동할 위치
		m_PathList.pop_front();
	}

	SAFE_RELEASE(pNavMesh);
	return true;
}
