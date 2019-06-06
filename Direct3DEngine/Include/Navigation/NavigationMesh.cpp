#include "NavigationMesh.h"
ENGINE_USING

NavigationMesh::NavigationMesh()
{
	m_bGrid = false;
	m_iLineRectCount = 0;
	m_iSectionX = 1;
	m_iSectionZ = 1;
	m_pSection = new NavSection[m_iSectionX * m_iSectionZ];

	m_iCloseCellListSize = 0;
	m_pCloseCellList = nullptr;

	m_vMouseGridColPos = Vector3::Zero;			// 숙제 -> 그리드용 브레젠험.. 해당 위치

	m_OpenList.SetSortFunc(this, &NavigationMesh::OpenListSort);
}


NavigationMesh::~NavigationMesh()
{
	SAFE_DELETE_ARRAY(m_pCloseCellList);
	SAFE_DELETE_ARRAY(m_pSection);
	Safe_Delete_VectorList(m_vecCell);
}

#pragma region Get(), Set(), Check() 함수

// Get
Vector3 NavigationMesh::GetMin() const
{
	return m_vMin;
}

Vector3 NavigationMesh::GetMax() const
{
	return m_vMax;
}

Vector3 NavigationMesh::GetOffset() const
{
	return m_vOffset;
}

std::list<Vector3> NavigationMesh::GetPathList() const
{
	return m_PathList;
}

void NavigationMesh::PathListClear()
{
	m_PathList.clear();
}

float NavigationMesh::GetY(const Vector3 & _vPos)
{
	if (!m_bGrid)
	{
		int	iSectionIndex = GetSectionIndex(_vPos);

		if (0 <= iSectionIndex && iSectionIndex < m_iSectionX * m_iSectionZ)
		{
			Vector3	vOrigin = _vPos;
			vOrigin.y = m_vMax.y;
			Vector3	vDir = Vector3(0.f, -1.f, 0.f);

			for (int i = 0; i < m_pSection[iSectionIndex].tCellList.iSize; ++i)
			{
				Vector3	vIntersect;
				float	fDist = 0.f;
				PNavigationCell	pCell = m_pSection[iSectionIndex].tCellList.pCellList[i];

				if (RayIntersectTriangle(vOrigin, vDir, pCell->vPos[0],
					pCell->vPos[1],
					pCell->vPos[2],
					fDist, vIntersect))
					return vIntersect.y;
			}
		}

		return 0.f;
	}

	Vector3	vCellSize = (m_vMax - m_vMin) / m_iLineRectCount;
	Vector3	vConvertPos = _vPos - m_vMin;

	// 가로, 세로를 1로 만들어준다.
	vConvertPos.x /= vCellSize.x;
	vConvertPos.z /= vCellSize.z;

	// 사각형 인덱스를 구한다.
	int	idxX = (int)vConvertPos.x;
	int	idxZ = m_iLineRectCount - ((int)vConvertPos.z + 1);			// 얘는 거꾸로니까 이렇게 구함

	if (idxX < 0 || idxX >= m_iLineRectCount)
		return 0.f;

	else if (idxZ < 0 || idxZ >= m_iLineRectCount)
		return 0.f;

	int	idx = (idxZ * m_iLineRectCount + idxX) * 2;			// 사각형이니까 *2

	//if (!m_vecCell[idx]->bEnable)							// 해당 셀이 경사면이 높아서 false인 경우 return 높이를 알 필요가 없다. (못올라간다고 생각하니까)
	//	return 0;

	// 구해준 사각형의 좌상단 점을 구한다.
	Vector3	vLTPos = m_vecCell[idx]->vPos[0];
	vLTPos.x /= vCellSize.x;
	vLTPos.z /= vCellSize.z;

	// 위치로 사각형의 삼각형들중 하단인지 상단인지 파악
	float	fX = vConvertPos.x - vLTPos.x;
	float	fZ = vLTPos.z - vConvertPos.z;

	// 우상단 삼각형일 경우
	if (fX >= fZ)
	{
		// Cell을 구성하는 점의 Y값을 얻어온다.
		float	fY[3] = {};
		for (int i = 0; i < 3; ++i)
		{
			fY[i] = m_vecCell[idx]->vPos[i].y;
		}

		return fY[0] + (fY[1] - fY[0]) * fX + (fY[2] - fY[1]) * fZ;			// 보간값 넣기 ㅇㅁㅇ 어느 위치인지 보간해서 Y값을 넣는다!
	}

	// 좌하단 삼각형일 경우
	float	fY[3] = {};
	for (int i = 0; i < 3; ++i)
	{
		fY[i] = m_vecCell[idx + 1]->vPos[i].y;
	}

	return fY[0] + (fY[1] - fY[2]) * fX + (fY[2] - fY[0]) * fZ;
}

float NavigationMesh::GetY(int _iCellIndex, const Vector3 & _vPos)
{
	return 0.0f;
}

// Set

void NavigationMesh::SetOffset(const Vector3 & _vOffset)
{
	m_vOffset = _vOffset;
}

void NavigationMesh::SetOffsetScale(const Vector3 & _vOffsetScale)
{
	m_vOffsetScale = _vOffsetScale;
}

// CheckCell() : 땅을 밟고 있는지 체크
bool NavigationMesh::CheckCell(const Vector3 & _vPos)
{
	int	iCellIndex = GetCellIndex(_vPos);

	if (iCellIndex == -1)
		return false;

	float	fY = GetY(iCellIndex, _vPos);			// y위치 가져오기 ( 해당 셀에 존재하는.. + 오브젝트가 위치한)

	// 해당셀이 존재하지 않거나 ( 내가 설정한 각도값보다 더 높은 각도에 있는 셀이거나.. 그냥 없거나 ㅇㅅㅇ)
	// 2.0f는 그냥 매직넘버 ㅇㅅㅇ
	if (false == m_vecCell[iCellIndex]->bEnable ||
		(_vPos.y - 2.f > fY || fY > _vPos.y + 2.f))
		return false;

	return true;
}

bool NavigationMesh::GetCellEnable(const Vector3 & _vPos)
{
	int	iCellIndex = GetCellIndex(_vPos);

	if (iCellIndex == -1)
		return false;

	return m_vecCell[iCellIndex]->bEnable;
}

#pragma endregion


bool NavigationMesh::Init()
{
	m_vMin.x = 10000000.f;
	m_vMin.y = 10000000.f;
	m_vMin.z = 10000000.f;

	m_vMax.x = -10000000.f;
	m_vMax.y = -10000000.f;
	m_vMax.z = -10000000.f;

	return true;
}

// 셀추가하기 ( 지형의 정점정보들 가져옴 - 최소단위 삼각형으로 구성 )
void NavigationMesh::AddCell(const Vector3 _vPos[3])
{
	PNavigationCell	pCell = new NavigationCell;

	for (int i = 0; i < 3; ++i)
	{
		pCell->vPos[i] = _vPos[i];
	}

	pCell->vEdge[0] = _vPos[1] - _vPos[0];
	pCell->vEdgeCenter[0] = (_vPos[1] + _vPos[0]) / 2.f;

	pCell->vEdge[1] = _vPos[2] - _vPos[1];
	pCell->vEdgeCenter[1] = (_vPos[2] + _vPos[1]) / 2.f;

	pCell->vEdge[2] = _vPos[2] - _vPos[0];
	pCell->vEdgeCenter[2] = (_vPos[2] + _vPos[0]) / 2.f;

	pCell->vCenter = (_vPos[0] + _vPos[1] + _vPos[2]) / 3.f;

	pCell->iIndex = (int)(m_vecCell.size());			// 먼저 인덱스 넣고 나중에 셀 넣어줄거 ㅇㅅㅇ그래야 인덱스가 맞아..

	for (int i = 0; i < 3; ++i)
	{
		// 네비메쉬의 (위에서 초기화해준..) 최대, 최소값이랑 비교해서 넣기
		if (m_vMin.x > _vPos[i].x)
			m_vMin.x = _vPos[i].x;
		if (m_vMin.y > _vPos[i].y)
			m_vMin.y = _vPos[i].y;
		if (m_vMin.z > _vPos[i].z)
			m_vMin.z = _vPos[i].z;

		if (m_vMax.x < _vPos[i].x)
			m_vMax.x = _vPos[i].x;
		if (m_vMax.y < _vPos[i].y)
			m_vMax.y = _vPos[i].y;
		if (m_vMax.z < _vPos[i].z)
			m_vMax.z = _vPos[i].z;
	}

	Vector3	vDir1 = pCell->vEdge[0];			// 바
	Vector3	vDir2 = pCell->vEdge[2];

	vDir1.Normalize();
	vDir2.Normalize();

	Vector3	vFaceNormal = vDir1.Cross(vDir2);		// 면법선 구하기(외적)
	vFaceNormal.Normalize();

	Vector3	vView = Vector3::Axis[AXIS_Y];			// 

	float	fAngle = vView.Angle(vFaceNormal);		// 기울기 제한, 일정 각도 이상의 경사일 때 이동X

	// 일단 65도로 제한준다.
	if (fAngle >= 50.f)
		pCell->bEnable = false;

	m_vecCell.push_back(pCell);
}

float NavigationMesh::GetFaceNormalAngle(Vector3 _vPos)
{
	PNavigationCell pCell = FindCell(_vPos);

	Vector3	vDir1 = pCell->vEdge[0];			// 바
	Vector3	vDir2 = pCell->vEdge[2];

	vDir1.Normalize();
	vDir2.Normalize();

	Vector3	vFaceNormal = vDir1.Cross(vDir2);		// 면법선 구하기(외적)
	vFaceNormal.Normalize();

	Vector3	vView = Vector3::Axis[AXIS_Y];			// 

	float	fAngle = vView.Angle(vFaceNormal);		// 기울기 제한, 일정 각도 이상의 경사일 때 이동X
	return fAngle;
}

// 인접 정보 추가
void NavigationMesh::AddAdj(int _iCellIdx, int _iAdjIdx)
{
	if (_iCellIdx < 0 || _iCellIdx >= (int)(m_vecCell.size()))
		return;

	ADJINFO	tInfo = {};
	tInfo.iIndex = _iAdjIdx;
	tInfo.iEdgeIndex = 0;
	m_vecCell[_iCellIdx]->vecAdj.push_back(tInfo);
}

void NavigationMesh::CreateGridMapAdj(int _iLineRectCount)
{
	// 그리드로 네비메쉬 생성시 -> 인접정보들 생성
	m_bGrid = true;
	m_iLineRectCount = _iLineRectCount;

	size_t iSize = m_vecCell.size();
	ADJINFO	tInfo = {};

	// 사각형이니까 +2
	for (size_t i = 0; i < iSize; i += 2)
	{
		// 우상단 삼각형 인접정보 구성.
		// 위쪽 사각형, 오른쪽 사각형, 자기자신 사각형의 왼쪽 하단 삼각형이
		// 인접정보 후보가 된다.

		// 위쪽 사각형의 좌하단 삼각형 인덱스를 구한다.
		int	idx = (int)(i) - _iLineRectCount * 2 + 1;
		if (idx >= 0 && idx < (int)(m_vecCell.size()))
		{
			tInfo.iIndex = idx;
			tInfo.iEdgeIndex = 0;
			m_vecCell[i]->vecAdj.push_back(tInfo);
		}

		// 자기자신 사각형의 왼쪽 하단 삼각형 인덱스를 구한다.
		idx = (int)(i + 1);
		if (idx >= 0 && idx < (int)(m_vecCell.size()))
		{
			tInfo.iIndex = idx;
			tInfo.iEdgeIndex = 2;
			m_vecCell[i]->vecAdj.push_back(tInfo);
		}

		// 오른쪽 사각형의 왼쪽 하단 삼각형 인덱스를 구한다.
		if ((i / 2) % _iLineRectCount != _iLineRectCount - 1)
		{
			idx = (int)i + 3;
			tInfo.iIndex = idx;
			tInfo.iEdgeIndex = 1;		// 엣지 개수인듯 인접한!!
			m_vecCell[i]->vecAdj.push_back(tInfo);
		}

		// 좌하단 삼각형 인접정보 구성.
		// 아래쪽 사각형, 왼쪽 사각형, 자기자신 사각형의 오른쪽 상단 삼각형이
		// 인접정보 후보가 된다.

		// 자기자신 사각형의 우상단 삼각형 인덱스를 구한다.
		tInfo.iIndex = (int)i;
		tInfo.iEdgeIndex = 0;
		m_vecCell[i + 1]->vecAdj.push_back(tInfo);

		// 왼쪽 사각형의 우상단 삼각형 인덱스를 구한다.
		if ((i / 2) % _iLineRectCount != 0)
		{
			tInfo.iIndex = (int)(i - 3);
			tInfo.iEdgeIndex = 2;
			m_vecCell[i + 1]->vecAdj.push_back(tInfo);
		}

		idx = (int)i + (_iLineRectCount * 2);
		if (idx < (int)(m_vecCell.size()))
		{
			tInfo.iIndex = idx;
			tInfo.iEdgeIndex = 1;
			m_vecCell[i + 1]->vecAdj.push_back(tInfo);
		}
	}

	CreateSection();
	
}

void NavigationMesh::CreateAdj()
{
#pragma region 함수 설명

	// 이 함수는 그리드로 생성한 지형이아니더라도 인접정보를 만들어내는 함수이다.
	// 영역을 정하면 그 안에 있는 메쉬정보를 다 읽어서 (Bake) 
	// 새로 메쉬로 만든다고 생각하면된다.

	// 그리고 인접한 엣지들을 찾아야하는데 ( 인접정보를 알아야 다음으로 넘어갈 ㅁ메쉬를 알 수 있다. (높이맵 적용을 위해) )

	// 인접 조건들

	// 1. 정점이 일치 ( 삼각형의 정점정보들중 하나라도.. 해당 엣지와 정점이 같다면 그 엣지는 해당 삼각형에 인접한다.
	// 2. 평행한 엣지가 존재. ( 해당 엣지와 삼각형의 엣지 3개들중에 하나라도 평행할 경우 )
	//    -> 일치 or 평행하면 그 엣지는 삼각형과 인접한다.
	//    -> 일치의 경우는 두 벡터 (엣지와 삼각형 엣지 3개들 중 한개씩.. )
	//       를 내적(당빠 노말값) 할 경우에 -1 or 1이 나올경우 (일치)
	
	// 그리드일 경우에는 메쉬를 탈때.. 선형 보간 처리를 이용하면 되는데
	// 아닐 경우에는 광선과 삼각형 충돌을 통해 파악한다.

#pragma endregion
	size_t	iSize = m_vecCell.size();

	FILE*	pFile = NULL;

	fopen_s(&pFile, "NaviAdj.txt", "wt");			// 쓰는겅 ㅇㅁㅇ

	Vector3	vOrigin[2][3];
	Vector3	vEdgeDir[2][3];
	float	fLength[2][3];

	for (size_t i = 0; i < iSize; ++i)
	{
		Vector3	vSrc[3];
		Vector3	vSrcOrigin[3];
		float	fSrcLength[3] = {};
		for (int k = 0; k < 3; ++k)
		{
			vEdgeDir[0][k] = m_vecCell[i]->vEdge[k];				// 엣지 방향

			fLength[0][k] = vEdgeDir[0][k].Length();				// 길이

			vEdgeDir[0][k].Normalize();								// 노말값 ( 단위벡터다)
		}

		vOrigin[0][0] = m_vecCell[i]->vPos[0];
		vOrigin[0][1] = m_vecCell[i]->vPos[1];
		vOrigin[0][2] = m_vecCell[i]->vPos[2];

		for (size_t j = i + 1; j < iSize; ++j)
		{
			// 삼각형의 3개의 엣지들을 서로 내적하여 평행한지 비교한다.
			Vector3	vDest[3];
			//float fDestLength[3];
			Vector3	vDestOrigin[3];
			int	iPosIdx[3][2] = { 1, 0, 2, 1, 2, 0 };
			bool	bInsert = false;

			vOrigin[1][0] = m_vecCell[j]->vPos[0];
			vOrigin[1][1] = m_vecCell[j]->vPos[1];
			vOrigin[1][2] = m_vecCell[j]->vPos[2];

			for (int k = 0; k < 3; ++k)
			{
				vEdgeDir[1][k] = m_vecCell[j]->vEdge[k];

				fLength[1][k] = vEdgeDir[1][k].Length();

				vEdgeDir[1][k].Normalize();
			}

			for (int k = 0; k < 3; ++k)
			{
				if (bInsert)
					break;

				for (int l = 0; l < 3; ++l)
				{
					// A삼각형과 B삼각형의 엣지를 구성하는 두 정점이 같을 경우
					// 이 삼각형은 무조건 붙어있는 삼각형이다.
					if ((vOrigin[0][iPosIdx[k][0]] == vOrigin[1][iPosIdx[l][0]] ||
						vOrigin[0][iPosIdx[k][0]] == vOrigin[1][iPosIdx[l][1]]) &&
						(vOrigin[0][iPosIdx[k][1]] == vOrigin[1][iPosIdx[l][0]] ||
							vOrigin[0][iPosIdx[k][1]] == vOrigin[1][iPosIdx[l][1]]))
					{
						ADJINFO	tInfo = {};
						tInfo.iIndex = (int)j;
						tInfo.iEdgeIndex = k;
						m_vecCell[i]->vecAdj.push_back(tInfo);

						tInfo.iIndex = (int)i;
						tInfo.iEdgeIndex = l;
						m_vecCell[j]->vecAdj.push_back(tInfo);
						bInsert = true;
						break;
					}

					// 두 엣지에 대해서 상대 엣지를 구성하는 2개의 점이 대상 엣지
					// 위에 둘다 존재하는지를 판단한다. 둘다 존재한다면
					// 무조건 붙어있는 것이다.
					// (B1 - A1, B2 - A1), (B1 - A2, B2 - A2)
					// 내적을 통해서 빼준 벡터의 크기가 그대로 나온다면
					// 직선상에 존재하는 것이다. 단, 둘다 크기가 나올 경우는
					// 무조건 존재하는 것이고 둘중 하나만 나온다면 크기가 0이라면
					// 하나의 점만 직선위에 존재하고 두 점 위치가 같다는 것이므로
					// 해당 삼각형은 인접 삼각형이 아니다.
					// B1 - A1 처리
					if (CheckOnEdge((int)i, (int)j, vOrigin[1][iPosIdx[l][0]],
						vOrigin[0][iPosIdx[k][0]], vEdgeDir[0][k], fLength[0][k], k, l))
					{
						bInsert = true;
						break;
					}

					// B2 - A1 처리
					else if (CheckOnEdge((int)i, (int)j, vOrigin[1][iPosIdx[l][1]],
						vOrigin[0][iPosIdx[k][0]], vEdgeDir[0][k], fLength[0][k], k, l))
					{
						bInsert = true;
						break;
					}

					// B1 - A2 처리
					else if (CheckOnEdge((int)i, (int)j, vOrigin[1][iPosIdx[l][0]],
						vOrigin[0][iPosIdx[k][1]], vEdgeDir[0][k] * -1.f, fLength[0][k], k, l))
					{
						bInsert = true;
						break;
					}

					// B2 - A2 처리
					else if (CheckOnEdge((int)i, (int)j, vOrigin[1][iPosIdx[l][1]],
						vOrigin[0][iPosIdx[k][1]], vEdgeDir[0][k] * -1.f, fLength[0][k], k, l))
					{
						bInsert = true;
						break;
					}
				}
			}
		}
	}

	fclose(pFile);

	CreateSection();
}

bool NavigationMesh::CheckOnEdge(int _iSrc, int _iDest, 
	const Vector3 & _vOrigin1, const Vector3 & _vOrigin2,
	const Vector3 & _vEdge,
	float _fEdgeLength, 
	int _iEdge1, int _iEdge2)
{
	// 소수점 2개까지만 확인
	if ((int)(_vOrigin2.x * 100) == (int)(_vOrigin1.x * 100) && (int)(_vOrigin2.y * 100) == (int)(_vOrigin1.y * 100) &&
		(int)(_vOrigin2.z * 100) == (int)(_vOrigin1.z * 100))
		return false;

	Vector3	vResult = _vOrigin1 - _vOrigin2;

	float	fDist = vResult.Length();

	// 위에서 구해준 거리를 이용해서 원점으로부터 엣지 방향으로 지정된 거리만큼 이동된 위치에 대상 점이 있는지를
	// 판단한다.
	vResult = _vOrigin2 + _vEdge * fDist;

	if ((int)(vResult.x * 100) == (int)(_vOrigin1.x * 100) && (int)(vResult.y * 100) == (int)(_vOrigin1.y * 100) &&
		(int)(vResult.z * 100) == (int)(_vOrigin1.z * 100))
	{
		vResult -= _vOrigin2;
		if (vResult.Length() < _fEdgeLength)
		{
			ADJINFO	tInfo = {};
			tInfo.iIndex = _iDest;
			tInfo.iEdgeIndex = _iEdge1;
			m_vecCell[_iSrc]->vecAdj.push_back(tInfo);

			tInfo.iIndex = _iSrc;
			tInfo.iEdgeIndex = _iEdge2;
			m_vecCell[_iDest]->vecAdj.push_back(tInfo);
			return true;
		}
	}

	return false;
}

bool NavigationMesh::CheckPathDir(const Vector3 & _vSrc1, const Vector3 & _vSrc2, const Vector3 & _vDest1, const Vector3 & _vDest2, Vector3 & _vIntersect)
{
	return false;
}

void NavigationMesh::FindPath(const Vector3 & _vStart, const Vector3 & _vEnd)
{
	// 경로/길 찾기 함수
	// 시작 위치를 가져와서 도착 지점까지의 빠른 경로를 찾아서 이동한다.
	// 부드럽게 이동하기 위해서 엣지의 중간값으로 이동한다.
	PNavigationCell pStart = FindCell(_vStart);
	PNavigationCell pEnd = FindCell(_vEnd);

	// 어느 한지점이라도 nullptr 이면
	// -> 이동할 수 있는 셀이 존재하지 않는다면
	if (nullptr == pStart || nullptr == pEnd)
	{
		return;
	}
	else if (false == pEnd->bEnable)
	{
		return;
	}


	// 시작점 = 끝점 ( 제자리를 클릭한경우 )
	if (pStart == pEnd)
	{
		m_PathList.clear();
		m_PathList.push_back(_vEnd);
		return;
	}

	// 처음 시작하는부분.
	if (nullptr == m_pCloseCellList)
	{
		m_pCloseCellList = new PNavigationCell[m_vecCell.size()];
	}
	
	// 일단 값들 초기화해주고
	for (int i = 0; i < m_iCloseCellListSize; ++i)
	{
		m_pCloseCellList[i]->eType = NCLT_NONE;
		m_pCloseCellList[i]->iParentIdx = -1;
		m_pCloseCellList[i]->fG = -1.0f;		// 간선비용
		m_pCloseCellList[i]->fH = -1.0f;
		m_pCloseCellList[i]->fTotal = -1.0f;
	}

	m_iCloseCellListSize = 0;

	while (false == m_FindStack.empty())
	{
		m_FindStack.pop();
	}

	m_OpenList.Clear();

	// 시작노드를 열린목록에 넣어준다.
	pStart->eType = NCLT_OPEN;
	pStart->fG = 0.f;
	pStart->fH = _vStart.Distance(_vEnd);
	pStart->fTotal = pStart->fH;

	m_OpenList.Insert(pStart);

	m_bFindEnd = false;

	PNavigationCell	pCell = nullptr;

	// 이부분에서 최단경로를 찾게된다.
	while (false == m_OpenList.Empty() && false == m_bFindEnd)
	{
		// 열린목록에서 셀을 얻어온다.
		m_OpenList.Pop(pCell);

		// 얻어온 셀을 닫힌목록으로 만들어준다.
		pCell->eType = NCLT_CLOSE;

		// 최단경로찾는거인듯
		AddOpenList(pCell, pEnd, _vStart, _vEnd);
	}

}

void NavigationMesh::CreateSection()
{
	m_iSectionX = 5;
	m_iSectionZ = 5;

	Vector3	vLength = m_vMax - m_vMin;			// 전체길이
	m_vSectionSize = vLength / Vector3(m_iSectionX, 1, m_iSectionZ);	// 한 네비공간의 사이즈

	SAFE_DELETE_ARRAY(m_pSection);										// 기존에 공간있었던거 삭제

	m_pSection = new NavSection[m_iSectionX * m_iSectionZ];				// 총 공간의 크기 넣고 값 넣기

	for (int i = 0; i < m_iSectionZ; ++i)
	{
		for (int j = 0; j < m_iSectionX; ++j)
		{
			int	idx = i * m_iSectionX + j;					// 공간의 인덱스
			m_pSection[idx].vSize = m_vSectionSize;			// 한 공간의 크기 넣기
			m_pSection[idx].vMin = m_vMin + m_vSectionSize * Vector3(j, 0, i);		// 해당 공간의 min값
			m_pSection[idx].vMax = m_vMin + m_vSectionSize * Vector3((j + 1), 1, (i + 1));	// 해당 공간의 max값
		}
	}

	// 최대 4개의 영역에 포함될 수 있다.
	// 어디 포함되어 있는지 판단해야 한다. 제발.
	for (size_t i = 0; i < m_vecCell.size(); ++i)
	{
		// 삼각형을 구성하는 3개의 점을 이용해서 인덱스를 구한다.
		int	iMinX = 100000, iMinZ = 100000;
		int	iMaxX = -100000, iMaxZ = -100000;

		for (int j = 0; j < 3; ++j)
		{
			Vector3	vPos = m_vecCell[i]->vPos[j] - m_vMin;
			vPos /= m_vSectionSize;				// 어디 공간에 해당하는지 나누기 ㅇㅅㅇ

			// xz평면이니까 (어디 공간에 속하는지만 볼거임 )
			int x, z;
			x = (int)vPos.x;
			z = (int)vPos.z;

			if (iMinX > x)
				iMinX = x;

			if (iMaxX < x)
				iMaxX = x;

			if (iMinZ > z)
				iMinZ = z;

			if (iMaxZ < z)
				iMaxZ = z;
		}

		// 공간이 0보단 작을수가 없으니까 (인덱스니까)
		iMinX = iMinX < 0 ? 0 : iMinX;
		iMinZ = iMinZ < 0 ? 0 : iMinZ;
		// 맥스도 마찬가지로 .. 인덱스땜에 ㅇㅅㅇ..내가 설정한 공간 밖에있으면 X
		// 제한시킨다.
		iMaxX = iMaxX >= m_iSectionX ? m_iSectionX - 1 : iMaxX;
		iMaxZ = iMaxZ >= m_iSectionZ ? m_iSectionZ - 1 : iMaxZ;

		// 자신이 어떤 공간에 속했는지 넣어준다. ( 여러공간에 들어갈 수 있음 중복공간도 생각함 )
		if (iMinX < m_iSectionX && iMaxX >= 0 &&
			iMinZ < m_iSectionZ && iMaxZ >= 0)
		{
			for (int j = iMinZ; j <= iMaxZ; ++j)
			{
				for (int k = iMinX; k <= iMaxX; ++k)
				{
					int	idx = j * m_iSectionX + k;

					m_pSection[idx].Add(m_vecCell[i]);
				}
			}
		}
	}
}

Vector3 NavigationMesh::GetCellFaceNormal(const Vector3 & _vPos)
{
	Vector3 vFaceNormal = Vector3::Zero;
	PNavigationCell pCell = FindCell(_vPos);

	if (nullptr == pCell)
	{
		return Vector3(-100.0f, -100.0f, -100.0f);
	}

	Vector3	vDir1 = pCell->vEdge[0];			// 바
	Vector3	vDir2 = pCell->vEdge[2];

	vDir1.Normalize();
	vDir2.Normalize();

	vFaceNormal = vDir1.Cross(vDir2);			// 외적해서 면법선을 구한다.
	vFaceNormal.Normalize();

	return vFaceNormal;
}

void NavigationMesh::AddOpenList(PNavigationCell _pCell, PNavigationCell _pEnd, const Vector3 & _vStart, const Vector3 & _vEnd)
{
	for (size_t i = 0; i < _pCell->vecAdj.size(); ++i)
	{
		if (_pCell->vecAdj[i].iIndex == -1)
			continue;

		PNavigationCell	pAdj = m_vecCell[_pCell->vecAdj[i].iIndex];

		if (!pAdj->bEnable)
			continue;

		else if (pAdj->eType == NCLT_CLOSE)
			continue;

		// 도착 셀을 찾았을 경우
		if (pAdj == _pEnd)
		{
			// 부모의 인덱스를 얻어온다.
			int	iParent = _pCell->iIndex;

			while (iParent != -1)
			{
				m_FindStack.push(iParent);
				iParent = m_vecCell[iParent]->iParentIdx;
			}

			// 시작노드는 제거해준다.
			m_FindStack.pop();

			// 스택으로 만들어진 경로를 원래대로 되돌린다.
			std::vector<int>	vecPathIndex;

			while (!m_FindStack.empty())
			{
				int	idx = m_FindStack.top();
				m_FindStack.pop();
				vecPathIndex.push_back(idx);
			}

			m_PathList.clear();

			std::vector<int>	vecCenter;
			for (size_t j = 0; j < vecPathIndex.size() -1; ++j)
			{
				int	iEdgeIndex = -1;

				_pCell = m_vecCell[vecPathIndex[j]];

				for (size_t k = 0; k < _pCell->vecAdj.size(); ++k)
				{
					if (_pCell->vecAdj[k].iIndex == vecPathIndex[j+1])
					{
						iEdgeIndex = _pCell->vecAdj[k].iEdgeIndex;
						break;
					}
				}

				// 위에서 어느 엣지를 기준으로 인접정보가 구성되었는지
				// 찾았다면 여기서 센터정보를 구해준다.
				vecCenter.push_back(iEdgeIndex);

				m_PathList.push_back(_pCell->vEdgeCenter[iEdgeIndex]);
			}

			// 도착위치를 넣어준다.
			m_PathList.push_back(_vEnd);

			m_bFindEnd = true;

			return;
		}

		float	fG = pAdj->vCenter.Distance(_pCell->vCenter);			// 간선비용 - 거리
		float	fH = pAdj->vCenter.Distance(_vEnd);

		if (pAdj->eType == NCLT_NONE)
		{
			m_pCloseCellList[m_iCloseCellListSize] = pAdj;
			++m_iCloseCellListSize;

			pAdj->fG = fG + _pCell->fG;
			pAdj->fH = fH;
			pAdj->fTotal = pAdj->fG + pAdj->fH;
			pAdj->iParentIdx = _pCell->iIndex;
			pAdj->eType = NCLT_OPEN;

			m_OpenList.Insert(pAdj);
		}

		// 열린목록에 들어가있는 Cell일 경우
		else if (pAdj->fG > fG + _pCell->fG)
		{
			pAdj->fG = fG + _pCell->fG;
			pAdj->fH = fH;
			pAdj->fTotal = pAdj->fG + pAdj->fH;
			pAdj->iParentIdx = _pCell->iIndex;
			m_OpenList.Sort();
		}
	}
}

PNavigationCell NavigationMesh::FindCell(const Vector3 & _vPos)
{
	int	idx = GetCellIndex(_vPos);

	if (idx < 0 || idx >= (int)(m_vecCell.size()))
		return nullptr;

	return m_vecCell[idx];
}

int NavigationMesh::GetCellIndex(const Vector3 & _vPos)
{
	if (false == m_bGrid)
	{
		// 그리드가 아닌경우
		int	iSectionIndex = GetSectionIndex(_vPos);		// 네비공간 가져오기

		if (0 <= iSectionIndex && iSectionIndex < m_iSectionX * m_iSectionZ)
		{
			Vector3	vOrigin = _vPos;
			vOrigin.y = m_vMax.y;
			Vector3	vDir = Vector3(0.f, -1.f, 0.f);

			for (int i = 0; i < m_pSection[iSectionIndex].tCellList.iSize; ++i)
			{
				Vector3	vIntersect;		// 교차점
				float	fDist = 0.f;
				PNavigationCell	pCell = m_pSection[iSectionIndex].tCellList.pCellList[i];

				// 삼각형과 광선의 교차점 찾기 ( 그리드가 아닌경우에 사용 )
				if (RayIntersectTriangle(vOrigin, vDir, pCell->vPos[0],
					pCell->vPos[1],
					pCell->vPos[2],
					fDist, vIntersect))
					return pCell->iIndex;
			}
		}

		return -1;
	}

	Vector3	vCellSize = (m_vMax - m_vMin) / m_iLineRectCount;
	Vector3	vConvertPos = _vPos - m_vMin;

	// 가로, 세로를 1로 만들어준다.
	vConvertPos.x /= vCellSize.x;
	vConvertPos.z /= vCellSize.z;

	// 사각형 인덱스를 구한다.
	int	idxX = (int)vConvertPos.x;
	int	idxZ = m_iLineRectCount - ((int)vConvertPos.z + 1);

	if (idxX < 0 || idxX > m_iLineRectCount)
		return -1;

	else if (idxZ < 0 || idxZ > m_iLineRectCount)
		return -1;

	return (idxZ * m_iLineRectCount + idxX) * 2;
}

// 광선 - 삼각형 충돌
bool NavigationMesh::RayIntersectTriangle(Vector3 _rayOrigin, Vector3 _rayDir,
	Vector3 _v0, Vector3 _v1, Vector3 _v2,			// 삼각형의 3개의 정점
	float & _t,			// t는 거리 ( 광선이 관통할.. )
	Vector3 & _vIntersect)
{
	Vector3 e1, e2, h, s, q;
	float a, f, u, v;

	e1 = _v1 - _v0;			// 엣지1
	e2 = _v2 - _v0;			// 엣지2 
	h = _rayDir.Cross(e2);	// 엣지2와 광선 외적-> 둘의 수직벡터
	a = e1.Dot(h);			// 엣지1과 그의 수직벡터를 내적

	// 오차범위 -> 두벡터가 내적한경우 0이 나올때는  수직일 경우다.
	// 광선이랑 삼각형면이 평행일 경우 false
	if (a > -0.00001 && a < 0.00001)
		return false;

	f = 1.f / a;
	s = _rayOrigin - _v0;			// 점 v0이 광선을 바라보는 방향

	u = f * s.Dot(h);

	if (u < 0.f || u > 1.f)
		return false;

	q = s.Cross(e1);

	v = f * _rayDir.Dot(q);

	if (v < 0.f || u + v > 1.f)
		return false;

	_t = f * e2.Dot(q);

	if (_t > 0.00001)
	{
		_vIntersect = _rayOrigin + _rayDir * _t;
		return true;
	}

	return false;
}

// 해당 위치의 공간 인덱스 가져오기
int NavigationMesh::GetSectionIndex(Vector3 _vPos)
{
	// min값을 빼준다. (  0 , 1, 2.. )이렇게 시작하기..편하게하려고 (타일맵처럼)
	_vPos -= m_vMin;
	_vPos /= m_vSectionSize;

	int x, z;
	x = (int)_vPos.x;
	z = (int)_vPos.z;

	return z * m_iSectionX + x;
}

void NavigationMesh::Save(const char * _pFileName, const std::string & _strPathKey)
{
}

void NavigationMesh::SaveFromFullPath(const char * pFullPath)
{
}

void NavigationMesh::Load(const char * _pFileName, const std::string & _strPathKey)
{
}

void NavigationMesh::LoadFromFullPath(const char * _pFullPath)
{
}

bool NavigationMesh::OpenListSort(const PNavigationCell & _pSrc, const PNavigationCell & _pDest)
{
	return _pSrc->fTotal > _pDest->fTotal;
}

// 마우스랑 공간 충돌되는 개수 찾기
int NavigationMesh::MouseSectionColCount(RayInfo _RayInfo)
{
	for (int z = 0; z < m_iSectionZ; ++z)
	{
		for (int x = 0; x < m_iSectionX; ++x)
		{
			int	idx = z * m_iSectionX + x;					// 공간의 인덱스
			
			OBBInfo SectionInfo;
			SectionInfo.vCenter = (m_pSection[idx].vMin + m_pSection[idx].vMax) / 2.0f;
			SectionInfo.vHalfLength.x = m_vSectionSize.x / 2.0f;
			SectionInfo.vHalfLength.y = m_vSectionSize.y;
			SectionInfo.vHalfLength.z = m_vSectionSize.z / 2.0f;

			if (true == CollisionRayToOBB(_RayInfo, SectionInfo))
			{
				vMouseSectionIndex.push_back(idx);
			}
		}
	}

	return (int)(vMouseSectionIndex.size());
}

Vector3 NavigationMesh::MouseNavCol(RayInfo _RayInfo)
{
	PNavigationCell ReturnCell = nullptr;

	// 여기서 삼각형이랑 레이 검사하고
	for (int i = 0; i < (int)(vMouseSectionIndex.size()); ++i)
	{
		int MouseSectionIndex = vMouseSectionIndex[i];
		int CellCount = m_pSection[MouseSectionIndex].tCellList.iSize;

		for (int j = 0 ; j < CellCount; ++j)
		{
			PNavigationCell	pCell = m_pSection[MouseSectionIndex].tCellList.pCellList[j];

			if (false == pCell->bEnable)
			{
				continue;
			}

			Vector3	vIntersect;		// 교차점
			float	fDist = 10000.0f;

			// 삼각형과 광선의 교차점 찾기 ( 그리드가 아닌경우에 사용 )
			if (RayIntersectTriangle(_RayInfo.vOrigin, _RayInfo.vDir, pCell->vPos[0],
				pCell->vPos[1],
				pCell->vPos[2],
				fDist, vIntersect))
			{
				// 처음들어온거면 바로 넣어주고 for문 돌리기
				if (nullptr == ReturnCell)
				{
					ReturnCell = pCell;
					continue;
				}

				// 두번째부터는 거리 비교해서 넣기
				float OriginDist = (ReturnCell->vCenter).Distance(_RayInfo.vOrigin);
				float pCellDist = (pCell->vCenter).Distance(_RayInfo.vOrigin);

				// 더 작은거 넣기
				if (OriginDist > pCellDist) 
				{
					ReturnCell = pCell;
				}

			}
		}
	}


	// 여기서 제일 가까운 삼각형 검사해서 리턴해주기
	vMouseSectionIndex.clear();

	if (nullptr == ReturnCell)
	{
		return Vector3(INT_MAX, INT_MAX, INT_MAX);
	}

	return ReturnCell->vCenter;
}


/////////
bool NavigationMesh::CollisionRayToOBB(const RayInfo & _tSrc, const OBBInfo & _tDest)
{
	Vector3 vDir = _tDest.vCenter - _tSrc.vOrigin;
	Vector3 RayOrigin = _tSrc.vOrigin;

	float tMin = 0.0f;
	float tMax = 100000.0f;


	Vector3 vSrcCenter =
	{ _tDest.vAxis[AXIS_X].Dot(vDir),
		_tDest.vAxis[AXIS_Y].Dot(vDir),
		_tDest.vAxis[AXIS_Z].Dot(vDir)
	};

	Vector3 vDestCenter =
	{
		_tDest.vAxis[AXIS_X].Dot(_tSrc.vDir),
		_tDest.vAxis[AXIS_Y].Dot(_tSrc.vDir),
		_tDest.vAxis[AXIS_Z].Dot(_tSrc.vDir)
	};

	// x 축
	float AxisDotDir = vSrcCenter.x;
	float AxisDotRay = vDestCenter.x;

	// Beware, don't do the division if f is near 0 ! See full source code for details.
	float t1 = (AxisDotDir - _tDest.vHalfLength.x) / AxisDotRay;	// Intersection with the "left" plane
	float t2 = (AxisDotDir + _tDest.vHalfLength.x) / AxisDotRay;	// Intersection with the "right" plane


	if (t1 > t2)
	{
		float w = t1;
		t1 = t2;
		t2 = w;
	}

	if (t2 < tMax)
	{
		tMax = t2;
	}

	if (t1 > tMin)
	{
		tMin = t1;
	}

	if (tMax < tMin)
	{
		return false;
	}

	// y 축
	AxisDotDir = vSrcCenter.y;
	AxisDotRay = vDestCenter.y;

	t1 = (AxisDotDir - _tDest.vHalfLength.y) / AxisDotRay;
	t2 = (AxisDotDir + _tDest.vHalfLength.y) / AxisDotRay;

	if (t1 > t2)
	{
		float w = t1;
		t1 = t2;
		t2 = w;
	}

	if (t2 < tMax)
	{
		tMax = t2;
	}

	if (t1 > tMin)
	{
		tMin = t1;
	}

	if (tMax < tMin)
	{
		return false;
	}

	// z축

	AxisDotDir = vSrcCenter.z;
	AxisDotRay = vDestCenter.z;

	t1 = (AxisDotDir - _tDest.vHalfLength.z) / AxisDotRay;
	t2 = (AxisDotDir + _tDest.vHalfLength.z) / AxisDotRay;

	if (t1 > t2)
	{
		float w = t1;
		t1 = t2;
		t2 = w;
	}

	if (t2 < tMax)
	{
		tMax = t2;
	}

	if (t1 > tMin)
	{
		tMin = t1;
	}

	if (tMax < tMin)
	{
		return false;
	}

	return true;
}

bool NavigationMesh::CheckMouseNavIndex(RayInfo _RayInfo)
{
	// 1. 마우스 광선에서 y값을 제거한후, 해당 레이와 충돌하는 메쉬들의 인덱스를 구한다.
	//    기울기를 이용해서 찾기
	Vector3 vRayOrigin = _RayInfo.vOrigin;
	Vector3 vRayDir = _RayInfo.vDir;

	int RayIndexX = (int)(vRayOrigin.x / m_vSectionSize.x);
	int RayIndexZ = (int)(vRayOrigin.z / m_vSectionSize.z);

	// 그리드니까 음수인덱스나 만들어진 인덱스의 범위를 넘어설 수 없다.
	if (RayIndexX < 0 || RayIndexX >= m_iSectionX)
	{
		return false;
	}

	if (RayIndexZ < 0 || RayIndexZ >= m_iSectionZ)
	{
		return false;
	}

	// 기울기 찾기
	float m = vRayDir.z / vRayDir.x;

	// x값에 따른 z값 찾기
	for (int i = 0; i < RayIndexX; ++i)
	{

	}



	// 2. 선택지 1. 레이와 충돌하는 메쉬들을 찾을때마다 MouseNavColGrid() 검사  -> 이거 사용
	//    선택지 2. 레이와 충돌하는 그리드의 인덱스들을 모두 찾고난 후에 MouseNavColGrid() 검사  

	// 3. MouseNavColGrid()의 값이 true이면 해당 삼각형의 센터를 m_vMouseGridColPos에 넣어주기


	// 4. 리턴값이 true일때만 내가 삼각형을 클릭한거임
	return false;
}

bool NavigationMesh::MouseNavColGrid(RayInfo _RayInfo)
{
	return false;
}
