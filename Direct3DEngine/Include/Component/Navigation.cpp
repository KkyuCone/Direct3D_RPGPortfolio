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

	//AI亜 焼艦檎 井稽 達聖 琶推 蒸生艦猿 走趨層陥.
	if (false == m_bAI)
	{
		m_PathList.clear();
		m_fTargetDetectTime = 0.0f;
	}
	else if (nullptr != m_pTarget)
	{
		// 背雁 展為戚 nullptr戚 焼艦檎  展為税 是帖研 隔嬢層陥.
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
	m_fTargetDetectTime = 0.0f;				// 展為郊荷檎 獣娃 陥獣 竺舛背爽澗 敗呪 硲窒背操 しさし!
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
	// 食奄辞 薄仙 中宜 是帖 硝焼神奄 ( 原酔什 傾戚亜 赤聖 井酔幻 端滴廃陥. )
	// 原酔什 硝焼神奄
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

		// 井稽亜 搾嬢 赤聖 井酔 掩聖 貼事廃陥.
		if (true == m_PathList.empty())
		{
			FindPath();
		}

		MoveAI(_fTime);


	}


	// AI 井稽 達奄 ( 析鋼 光旭精 蕉級 )
	if (true == m_bAI && nullptr != m_pTarget)
	{
		if (nullptr != m_pTarget)
		{
			Vector3 vPos = m_pTarget->GetWorldPosition();
			CheckAIPos(vPos, _fTime);
		}

		// 井稽亜 搾嬢 赤聖 井酔 掩聖 貼事廃陥.
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
	// 漆匂走芝税 Y葵戚 true戚檎 y葵 旋遂
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
				// 十虞戚漁困斗 坦軒
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

// 原酔什 啄精 員生稽 戚疑
void Navigation::CheckMousePos()
{
	// 原酔什亜 照喚形遭 雌殿檎 背匝琶推 蒸陥 しさし
	if (false == GET_SINGLETON(InputManager)->MousePress(MB_LBUTTON))
		return;

	Scene* pCurScene = GET_SINGLETON(SceneManager)->GetScene();				// 薄仙樟 閤焼神壱
	ColliderRay* pMouseRay = GET_SINGLETON(InputManager)->GetMouseRay();	// 原酔什 傾戚 閤焼神壱

	// 原酔什 傾戚人 革搾五習税 樟引 薄仙樟戚 旭聖凶幻 域至廃陥.
	if (nullptr != pMouseRay && pCurScene == m_pScene)
	{
		// 1. 原酔什 韻識戚櫛 走莫因娃聖 搾嘘背辞 中宜亜管廃 採歳幻 蓄形鎧奄
		
		// 1-1. 原酔什 韻識 亜閃神奄
		RayInfo sRayInfo = pMouseRay->GetInfo();
		// 1-2. 走莫因娃聖 搾嘘背辞 中宜
		if (0 == GET_SINGLETON(NavigationManager)->MouseNavSectionCount(m_pScene, sRayInfo))
		{
			m_bMousePosCheck = false;
			SAFE_RELEASE(pMouseRay);
			return;
		}


		// 2. 中宜廃 因娃幻 原酔什 韻識戚櫛 誌唖莫 中宜馬奄
		// 3. 舛慶背辞, 亜舌 亜猿錘 誌唖莫亜閃神奄 (vCenter)
		m_vMousePos = GET_SINGLETON(NavigationManager)->MouseNavPosition(m_pScene, sRayInfo);


		// 4. 暗奄 誌唖莫生稽 戚疑馬奄 (適遣獣)

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

	// 井稽 端滴拝 獣娃 端滴敗 m_fTargetCheckTime段 娃維原陥
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
	// 号狽 達奄
	Vector3 vDir = m_vWay - m_pTransform->GetWorldPosition();
	vDir.Normalize();

	// 暗軒 姥馬奄
	float fDist = m_vWay.Distance(m_pTransform->GetWorldPosition());

	// 紗亀
	float fSpeed = 3.0f * _fTime;

	if (fDist < fSpeed)
	{
		fSpeed = fDist;			// 3舛亀 神託坦軒しさし 3舛亀 暗軒託戚亜 赤生檎 郊稽 益 切軒稽 戚疑ぞ

								// 背雁 是帖拭 赤生糠稽 蟹袴走 井稽級戚 赤聖 井酔 走趨層陥.
		if (false == m_PathList.empty())
		{
			m_vWay = m_PathList.front();
			m_PathList.pop_front();
		}
		else
		{
			// 害精 井稽亜 蒸聖 井酔
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

	m_pTransform->Move(vDir, 3.0f, _fTime);		// 戚疑馬奄

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
		m_vWay = m_PathList.front();		// 戚疑拝 是帖
		m_PathList.pop_front();
	}

	SAFE_RELEASE(pNavMesh);
	return true;
}
