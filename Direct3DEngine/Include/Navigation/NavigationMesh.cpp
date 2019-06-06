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

	m_vMouseGridColPos = Vector3::Zero;			// ���� -> �׸���� �극����.. �ش� ��ġ

	m_OpenList.SetSortFunc(this, &NavigationMesh::OpenListSort);
}


NavigationMesh::~NavigationMesh()
{
	SAFE_DELETE_ARRAY(m_pCloseCellList);
	SAFE_DELETE_ARRAY(m_pSection);
	Safe_Delete_VectorList(m_vecCell);
}

#pragma region Get(), Set(), Check() �Լ�

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

	// ����, ���θ� 1�� ������ش�.
	vConvertPos.x /= vCellSize.x;
	vConvertPos.z /= vCellSize.z;

	// �簢�� �ε����� ���Ѵ�.
	int	idxX = (int)vConvertPos.x;
	int	idxZ = m_iLineRectCount - ((int)vConvertPos.z + 1);			// ��� �Ųٷδϱ� �̷��� ����

	if (idxX < 0 || idxX >= m_iLineRectCount)
		return 0.f;

	else if (idxZ < 0 || idxZ >= m_iLineRectCount)
		return 0.f;

	int	idx = (idxZ * m_iLineRectCount + idxX) * 2;			// �簢���̴ϱ� *2

	//if (!m_vecCell[idx]->bEnable)							// �ش� ���� ������ ���Ƽ� false�� ��� return ���̸� �� �ʿ䰡 ����. (���ö󰣴ٰ� �����ϴϱ�)
	//	return 0;

	// ������ �簢���� �»�� ���� ���Ѵ�.
	Vector3	vLTPos = m_vecCell[idx]->vPos[0];
	vLTPos.x /= vCellSize.x;
	vLTPos.z /= vCellSize.z;

	// ��ġ�� �簢���� �ﰢ������ �ϴ����� ������� �ľ�
	float	fX = vConvertPos.x - vLTPos.x;
	float	fZ = vLTPos.z - vConvertPos.z;

	// ���� �ﰢ���� ���
	if (fX >= fZ)
	{
		// Cell�� �����ϴ� ���� Y���� ���´�.
		float	fY[3] = {};
		for (int i = 0; i < 3; ++i)
		{
			fY[i] = m_vecCell[idx]->vPos[i].y;
		}

		return fY[0] + (fY[1] - fY[0]) * fX + (fY[2] - fY[1]) * fZ;			// ������ �ֱ� ������ ��� ��ġ���� �����ؼ� Y���� �ִ´�!
	}

	// ���ϴ� �ﰢ���� ���
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

// CheckCell() : ���� ��� �ִ��� üũ
bool NavigationMesh::CheckCell(const Vector3 & _vPos)
{
	int	iCellIndex = GetCellIndex(_vPos);

	if (iCellIndex == -1)
		return false;

	float	fY = GetY(iCellIndex, _vPos);			// y��ġ �������� ( �ش� ���� �����ϴ�.. + ������Ʈ�� ��ġ��)

	// �ش缿�� �������� �ʰų� ( ���� ������ ���������� �� ���� ������ �ִ� ���̰ų�.. �׳� ���ų� ������)
	// 2.0f�� �׳� �����ѹ� ������
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

// ���߰��ϱ� ( ������ ���������� ������ - �ּҴ��� �ﰢ������ ���� )
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

	pCell->iIndex = (int)(m_vecCell.size());			// ���� �ε��� �ְ� ���߿� �� �־��ٰ� �������׷��� �ε����� �¾�..

	for (int i = 0; i < 3; ++i)
	{
		// �׺�޽��� (������ �ʱ�ȭ����..) �ִ�, �ּҰ��̶� ���ؼ� �ֱ�
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

	Vector3	vDir1 = pCell->vEdge[0];			// ��
	Vector3	vDir2 = pCell->vEdge[2];

	vDir1.Normalize();
	vDir2.Normalize();

	Vector3	vFaceNormal = vDir1.Cross(vDir2);		// ����� ���ϱ�(����)
	vFaceNormal.Normalize();

	Vector3	vView = Vector3::Axis[AXIS_Y];			// 

	float	fAngle = vView.Angle(vFaceNormal);		// ���� ����, ���� ���� �̻��� ����� �� �̵�X

	// �ϴ� 65���� �����ش�.
	if (fAngle >= 50.f)
		pCell->bEnable = false;

	m_vecCell.push_back(pCell);
}

float NavigationMesh::GetFaceNormalAngle(Vector3 _vPos)
{
	PNavigationCell pCell = FindCell(_vPos);

	Vector3	vDir1 = pCell->vEdge[0];			// ��
	Vector3	vDir2 = pCell->vEdge[2];

	vDir1.Normalize();
	vDir2.Normalize();

	Vector3	vFaceNormal = vDir1.Cross(vDir2);		// ����� ���ϱ�(����)
	vFaceNormal.Normalize();

	Vector3	vView = Vector3::Axis[AXIS_Y];			// 

	float	fAngle = vView.Angle(vFaceNormal);		// ���� ����, ���� ���� �̻��� ����� �� �̵�X
	return fAngle;
}

// ���� ���� �߰�
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
	// �׸���� �׺�޽� ������ -> ���������� ����
	m_bGrid = true;
	m_iLineRectCount = _iLineRectCount;

	size_t iSize = m_vecCell.size();
	ADJINFO	tInfo = {};

	// �簢���̴ϱ� +2
	for (size_t i = 0; i < iSize; i += 2)
	{
		// ���� �ﰢ�� �������� ����.
		// ���� �簢��, ������ �簢��, �ڱ��ڽ� �簢���� ���� �ϴ� �ﰢ����
		// �������� �ĺ��� �ȴ�.

		// ���� �簢���� ���ϴ� �ﰢ�� �ε����� ���Ѵ�.
		int	idx = (int)(i) - _iLineRectCount * 2 + 1;
		if (idx >= 0 && idx < (int)(m_vecCell.size()))
		{
			tInfo.iIndex = idx;
			tInfo.iEdgeIndex = 0;
			m_vecCell[i]->vecAdj.push_back(tInfo);
		}

		// �ڱ��ڽ� �簢���� ���� �ϴ� �ﰢ�� �ε����� ���Ѵ�.
		idx = (int)(i + 1);
		if (idx >= 0 && idx < (int)(m_vecCell.size()))
		{
			tInfo.iIndex = idx;
			tInfo.iEdgeIndex = 2;
			m_vecCell[i]->vecAdj.push_back(tInfo);
		}

		// ������ �簢���� ���� �ϴ� �ﰢ�� �ε����� ���Ѵ�.
		if ((i / 2) % _iLineRectCount != _iLineRectCount - 1)
		{
			idx = (int)i + 3;
			tInfo.iIndex = idx;
			tInfo.iEdgeIndex = 1;		// ���� �����ε� ������!!
			m_vecCell[i]->vecAdj.push_back(tInfo);
		}

		// ���ϴ� �ﰢ�� �������� ����.
		// �Ʒ��� �簢��, ���� �簢��, �ڱ��ڽ� �簢���� ������ ��� �ﰢ����
		// �������� �ĺ��� �ȴ�.

		// �ڱ��ڽ� �簢���� ���� �ﰢ�� �ε����� ���Ѵ�.
		tInfo.iIndex = (int)i;
		tInfo.iEdgeIndex = 0;
		m_vecCell[i + 1]->vecAdj.push_back(tInfo);

		// ���� �簢���� ���� �ﰢ�� �ε����� ���Ѵ�.
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
#pragma region �Լ� ����

	// �� �Լ��� �׸���� ������ �����̾ƴϴ��� ���������� ������ �Լ��̴�.
	// ������ ���ϸ� �� �ȿ� �ִ� �޽������� �� �о (Bake) 
	// ���� �޽��� ����ٰ� �����ϸ�ȴ�.

	// �׸��� ������ �������� ã�ƾ��ϴµ� ( ���������� �˾ƾ� �������� �Ѿ ���޽��� �� �� �ִ�. (���̸� ������ ����) )

	// ���� ���ǵ�

	// 1. ������ ��ġ ( �ﰢ���� ������������ �ϳ���.. �ش� ������ ������ ���ٸ� �� ������ �ش� �ﰢ���� �����Ѵ�.
	// 2. ������ ������ ����. ( �ش� ������ �ﰢ���� ���� 3�����߿� �ϳ��� ������ ��� )
	//    -> ��ġ or �����ϸ� �� ������ �ﰢ���� �����Ѵ�.
	//    -> ��ġ�� ���� �� ���� (������ �ﰢ�� ���� 3���� �� �Ѱ���.. )
	//       �� ����(��� �븻��) �� ��쿡 -1 or 1�� ���ð�� (��ġ)
	
	// �׸����� ��쿡�� �޽��� Ż��.. ���� ���� ó���� �̿��ϸ� �Ǵµ�
	// �ƴ� ��쿡�� ������ �ﰢ�� �浹�� ���� �ľ��Ѵ�.

#pragma endregion
	size_t	iSize = m_vecCell.size();

	FILE*	pFile = NULL;

	fopen_s(&pFile, "NaviAdj.txt", "wt");			// ���°� ������

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
			vEdgeDir[0][k] = m_vecCell[i]->vEdge[k];				// ���� ����

			fLength[0][k] = vEdgeDir[0][k].Length();				// ����

			vEdgeDir[0][k].Normalize();								// �븻�� ( �������ʹ�)
		}

		vOrigin[0][0] = m_vecCell[i]->vPos[0];
		vOrigin[0][1] = m_vecCell[i]->vPos[1];
		vOrigin[0][2] = m_vecCell[i]->vPos[2];

		for (size_t j = i + 1; j < iSize; ++j)
		{
			// �ﰢ���� 3���� �������� ���� �����Ͽ� �������� ���Ѵ�.
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
					// A�ﰢ���� B�ﰢ���� ������ �����ϴ� �� ������ ���� ���
					// �� �ﰢ���� ������ �پ��ִ� �ﰢ���̴�.
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

					// �� ������ ���ؼ� ��� ������ �����ϴ� 2���� ���� ��� ����
					// ���� �Ѵ� �����ϴ����� �Ǵ��Ѵ�. �Ѵ� �����Ѵٸ�
					// ������ �پ��ִ� ���̴�.
					// (B1 - A1, B2 - A1), (B1 - A2, B2 - A2)
					// ������ ���ؼ� ���� ������ ũ�Ⱑ �״�� ���´ٸ�
					// ������ �����ϴ� ���̴�. ��, �Ѵ� ũ�Ⱑ ���� ����
					// ������ �����ϴ� ���̰� ���� �ϳ��� ���´ٸ� ũ�Ⱑ 0�̶��
					// �ϳ��� ���� �������� �����ϰ� �� �� ��ġ�� ���ٴ� ���̹Ƿ�
					// �ش� �ﰢ���� ���� �ﰢ���� �ƴϴ�.
					// B1 - A1 ó��
					if (CheckOnEdge((int)i, (int)j, vOrigin[1][iPosIdx[l][0]],
						vOrigin[0][iPosIdx[k][0]], vEdgeDir[0][k], fLength[0][k], k, l))
					{
						bInsert = true;
						break;
					}

					// B2 - A1 ó��
					else if (CheckOnEdge((int)i, (int)j, vOrigin[1][iPosIdx[l][1]],
						vOrigin[0][iPosIdx[k][0]], vEdgeDir[0][k], fLength[0][k], k, l))
					{
						bInsert = true;
						break;
					}

					// B1 - A2 ó��
					else if (CheckOnEdge((int)i, (int)j, vOrigin[1][iPosIdx[l][0]],
						vOrigin[0][iPosIdx[k][1]], vEdgeDir[0][k] * -1.f, fLength[0][k], k, l))
					{
						bInsert = true;
						break;
					}

					// B2 - A2 ó��
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
	// �Ҽ��� 2�������� Ȯ��
	if ((int)(_vOrigin2.x * 100) == (int)(_vOrigin1.x * 100) && (int)(_vOrigin2.y * 100) == (int)(_vOrigin1.y * 100) &&
		(int)(_vOrigin2.z * 100) == (int)(_vOrigin1.z * 100))
		return false;

	Vector3	vResult = _vOrigin1 - _vOrigin2;

	float	fDist = vResult.Length();

	// ������ ������ �Ÿ��� �̿��ؼ� �������κ��� ���� �������� ������ �Ÿ���ŭ �̵��� ��ġ�� ��� ���� �ִ�����
	// �Ǵ��Ѵ�.
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
	// ���/�� ã�� �Լ�
	// ���� ��ġ�� �����ͼ� ���� ���������� ���� ��θ� ã�Ƽ� �̵��Ѵ�.
	// �ε巴�� �̵��ϱ� ���ؼ� ������ �߰������� �̵��Ѵ�.
	PNavigationCell pStart = FindCell(_vStart);
	PNavigationCell pEnd = FindCell(_vEnd);

	// ��� �������̶� nullptr �̸�
	// -> �̵��� �� �ִ� ���� �������� �ʴ´ٸ�
	if (nullptr == pStart || nullptr == pEnd)
	{
		return;
	}
	else if (false == pEnd->bEnable)
	{
		return;
	}


	// ������ = ���� ( ���ڸ��� Ŭ���Ѱ�� )
	if (pStart == pEnd)
	{
		m_PathList.clear();
		m_PathList.push_back(_vEnd);
		return;
	}

	// ó�� �����ϴºκ�.
	if (nullptr == m_pCloseCellList)
	{
		m_pCloseCellList = new PNavigationCell[m_vecCell.size()];
	}
	
	// �ϴ� ���� �ʱ�ȭ���ְ�
	for (int i = 0; i < m_iCloseCellListSize; ++i)
	{
		m_pCloseCellList[i]->eType = NCLT_NONE;
		m_pCloseCellList[i]->iParentIdx = -1;
		m_pCloseCellList[i]->fG = -1.0f;		// �������
		m_pCloseCellList[i]->fH = -1.0f;
		m_pCloseCellList[i]->fTotal = -1.0f;
	}

	m_iCloseCellListSize = 0;

	while (false == m_FindStack.empty())
	{
		m_FindStack.pop();
	}

	m_OpenList.Clear();

	// ���۳�带 ������Ͽ� �־��ش�.
	pStart->eType = NCLT_OPEN;
	pStart->fG = 0.f;
	pStart->fH = _vStart.Distance(_vEnd);
	pStart->fTotal = pStart->fH;

	m_OpenList.Insert(pStart);

	m_bFindEnd = false;

	PNavigationCell	pCell = nullptr;

	// �̺κп��� �ִܰ�θ� ã�Եȴ�.
	while (false == m_OpenList.Empty() && false == m_bFindEnd)
	{
		// ������Ͽ��� ���� ���´�.
		m_OpenList.Pop(pCell);

		// ���� ���� ����������� ������ش�.
		pCell->eType = NCLT_CLOSE;

		// �ִܰ��ã�°��ε�
		AddOpenList(pCell, pEnd, _vStart, _vEnd);
	}

}

void NavigationMesh::CreateSection()
{
	m_iSectionX = 5;
	m_iSectionZ = 5;

	Vector3	vLength = m_vMax - m_vMin;			// ��ü����
	m_vSectionSize = vLength / Vector3(m_iSectionX, 1, m_iSectionZ);	// �� �׺������ ������

	SAFE_DELETE_ARRAY(m_pSection);										// ������ �����־����� ����

	m_pSection = new NavSection[m_iSectionX * m_iSectionZ];				// �� ������ ũ�� �ְ� �� �ֱ�

	for (int i = 0; i < m_iSectionZ; ++i)
	{
		for (int j = 0; j < m_iSectionX; ++j)
		{
			int	idx = i * m_iSectionX + j;					// ������ �ε���
			m_pSection[idx].vSize = m_vSectionSize;			// �� ������ ũ�� �ֱ�
			m_pSection[idx].vMin = m_vMin + m_vSectionSize * Vector3(j, 0, i);		// �ش� ������ min��
			m_pSection[idx].vMax = m_vMin + m_vSectionSize * Vector3((j + 1), 1, (i + 1));	// �ش� ������ max��
		}
	}

	// �ִ� 4���� ������ ���Ե� �� �ִ�.
	// ��� ���ԵǾ� �ִ��� �Ǵ��ؾ� �Ѵ�. ����.
	for (size_t i = 0; i < m_vecCell.size(); ++i)
	{
		// �ﰢ���� �����ϴ� 3���� ���� �̿��ؼ� �ε����� ���Ѵ�.
		int	iMinX = 100000, iMinZ = 100000;
		int	iMaxX = -100000, iMaxZ = -100000;

		for (int j = 0; j < 3; ++j)
		{
			Vector3	vPos = m_vecCell[i]->vPos[j] - m_vMin;
			vPos /= m_vSectionSize;				// ��� ������ �ش��ϴ��� ������ ������

			// xz����̴ϱ� (��� ������ ���ϴ����� ������ )
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

		// ������ 0���� �������� �����ϱ� (�ε����ϱ�)
		iMinX = iMinX < 0 ? 0 : iMinX;
		iMinZ = iMinZ < 0 ? 0 : iMinZ;
		// �ƽ��� ���������� .. �ε������� ������..���� ������ ���� �ۿ������� X
		// ���ѽ�Ų��.
		iMaxX = iMaxX >= m_iSectionX ? m_iSectionX - 1 : iMaxX;
		iMaxZ = iMaxZ >= m_iSectionZ ? m_iSectionZ - 1 : iMaxZ;

		// �ڽ��� � ������ ���ߴ��� �־��ش�. ( ���������� �� �� ���� �ߺ������� ������ )
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

	Vector3	vDir1 = pCell->vEdge[0];			// ��
	Vector3	vDir2 = pCell->vEdge[2];

	vDir1.Normalize();
	vDir2.Normalize();

	vFaceNormal = vDir1.Cross(vDir2);			// �����ؼ� ������� ���Ѵ�.
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

		// ���� ���� ã���� ���
		if (pAdj == _pEnd)
		{
			// �θ��� �ε����� ���´�.
			int	iParent = _pCell->iIndex;

			while (iParent != -1)
			{
				m_FindStack.push(iParent);
				iParent = m_vecCell[iParent]->iParentIdx;
			}

			// ���۳��� �������ش�.
			m_FindStack.pop();

			// �������� ������� ��θ� ������� �ǵ�����.
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

				// ������ ��� ������ �������� ���������� �����Ǿ�����
				// ã�Ҵٸ� ���⼭ ���������� �����ش�.
				vecCenter.push_back(iEdgeIndex);

				m_PathList.push_back(_pCell->vEdgeCenter[iEdgeIndex]);
			}

			// ������ġ�� �־��ش�.
			m_PathList.push_back(_vEnd);

			m_bFindEnd = true;

			return;
		}

		float	fG = pAdj->vCenter.Distance(_pCell->vCenter);			// ������� - �Ÿ�
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

		// ������Ͽ� ���ִ� Cell�� ���
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
		// �׸��尡 �ƴѰ��
		int	iSectionIndex = GetSectionIndex(_vPos);		// �׺���� ��������

		if (0 <= iSectionIndex && iSectionIndex < m_iSectionX * m_iSectionZ)
		{
			Vector3	vOrigin = _vPos;
			vOrigin.y = m_vMax.y;
			Vector3	vDir = Vector3(0.f, -1.f, 0.f);

			for (int i = 0; i < m_pSection[iSectionIndex].tCellList.iSize; ++i)
			{
				Vector3	vIntersect;		// ������
				float	fDist = 0.f;
				PNavigationCell	pCell = m_pSection[iSectionIndex].tCellList.pCellList[i];

				// �ﰢ���� ������ ������ ã�� ( �׸��尡 �ƴѰ�쿡 ��� )
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

	// ����, ���θ� 1�� ������ش�.
	vConvertPos.x /= vCellSize.x;
	vConvertPos.z /= vCellSize.z;

	// �簢�� �ε����� ���Ѵ�.
	int	idxX = (int)vConvertPos.x;
	int	idxZ = m_iLineRectCount - ((int)vConvertPos.z + 1);

	if (idxX < 0 || idxX > m_iLineRectCount)
		return -1;

	else if (idxZ < 0 || idxZ > m_iLineRectCount)
		return -1;

	return (idxZ * m_iLineRectCount + idxX) * 2;
}

// ���� - �ﰢ�� �浹
bool NavigationMesh::RayIntersectTriangle(Vector3 _rayOrigin, Vector3 _rayDir,
	Vector3 _v0, Vector3 _v1, Vector3 _v2,			// �ﰢ���� 3���� ����
	float & _t,			// t�� �Ÿ� ( ������ ������.. )
	Vector3 & _vIntersect)
{
	Vector3 e1, e2, h, s, q;
	float a, f, u, v;

	e1 = _v1 - _v0;			// ����1
	e2 = _v2 - _v0;			// ����2 
	h = _rayDir.Cross(e2);	// ����2�� ���� ����-> ���� ��������
	a = e1.Dot(h);			// ����1�� ���� �������͸� ����

	// �������� -> �κ��Ͱ� �����Ѱ�� 0�� ���ö���  ������ ����.
	// �����̶� �ﰢ������ ������ ��� false
	if (a > -0.00001 && a < 0.00001)
		return false;

	f = 1.f / a;
	s = _rayOrigin - _v0;			// �� v0�� ������ �ٶ󺸴� ����

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

// �ش� ��ġ�� ���� �ε��� ��������
int NavigationMesh::GetSectionIndex(Vector3 _vPos)
{
	// min���� ���ش�. (  0 , 1, 2.. )�̷��� �����ϱ�..���ϰ��Ϸ��� (Ÿ�ϸ�ó��)
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

// ���콺�� ���� �浹�Ǵ� ���� ã��
int NavigationMesh::MouseSectionColCount(RayInfo _RayInfo)
{
	for (int z = 0; z < m_iSectionZ; ++z)
	{
		for (int x = 0; x < m_iSectionX; ++x)
		{
			int	idx = z * m_iSectionX + x;					// ������ �ε���
			
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

	// ���⼭ �ﰢ���̶� ���� �˻��ϰ�
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

			Vector3	vIntersect;		// ������
			float	fDist = 10000.0f;

			// �ﰢ���� ������ ������ ã�� ( �׸��尡 �ƴѰ�쿡 ��� )
			if (RayIntersectTriangle(_RayInfo.vOrigin, _RayInfo.vDir, pCell->vPos[0],
				pCell->vPos[1],
				pCell->vPos[2],
				fDist, vIntersect))
			{
				// ó�����°Ÿ� �ٷ� �־��ְ� for�� ������
				if (nullptr == ReturnCell)
				{
					ReturnCell = pCell;
					continue;
				}

				// �ι�°���ʹ� �Ÿ� ���ؼ� �ֱ�
				float OriginDist = (ReturnCell->vCenter).Distance(_RayInfo.vOrigin);
				float pCellDist = (pCell->vCenter).Distance(_RayInfo.vOrigin);

				// �� ������ �ֱ�
				if (OriginDist > pCellDist) 
				{
					ReturnCell = pCell;
				}

			}
		}
	}


	// ���⼭ ���� ����� �ﰢ�� �˻��ؼ� �������ֱ�
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

	// x ��
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

	// y ��
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

	// z��

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
	// 1. ���콺 �������� y���� ��������, �ش� ���̿� �浹�ϴ� �޽����� �ε����� ���Ѵ�.
	//    ���⸦ �̿��ؼ� ã��
	Vector3 vRayOrigin = _RayInfo.vOrigin;
	Vector3 vRayDir = _RayInfo.vDir;

	int RayIndexX = (int)(vRayOrigin.x / m_vSectionSize.x);
	int RayIndexZ = (int)(vRayOrigin.z / m_vSectionSize.z);

	// �׸���ϱ� �����ε����� ������� �ε����� ������ �Ѿ �� ����.
	if (RayIndexX < 0 || RayIndexX >= m_iSectionX)
	{
		return false;
	}

	if (RayIndexZ < 0 || RayIndexZ >= m_iSectionZ)
	{
		return false;
	}

	// ���� ã��
	float m = vRayDir.z / vRayDir.x;

	// x���� ���� z�� ã��
	for (int i = 0; i < RayIndexX; ++i)
	{

	}



	// 2. ������ 1. ���̿� �浹�ϴ� �޽����� ã�������� MouseNavColGrid() �˻�  -> �̰� ���
	//    ������ 2. ���̿� �浹�ϴ� �׸����� �ε������� ��� ã�� �Ŀ� MouseNavColGrid() �˻�  

	// 3. MouseNavColGrid()�� ���� true�̸� �ش� �ﰢ���� ���͸� m_vMouseGridColPos�� �־��ֱ�


	// 4. ���ϰ��� true�϶��� ���� �ﰢ���� Ŭ���Ѱ���
	return false;
}

bool NavigationMesh::MouseNavColGrid(RayInfo _RayInfo)
{
	return false;
}
