#pragma once
#include "..\Reference.h"
#include "..\Heap.h"

ENGINE_BEGIN

// A스타할때 필요, 추후 설명
enum NAVIMESH_CELL_LIST_TYPE
{
	NCLT_NONE,			// X ( 맨 첫부분 )
	NCLT_OPEN,			// 열린 목록
	NCLT_CLOSE			// 닫힌 목록
};

// 인접정보 (마주 보는 엣지들의 인덱스)
// 이동하는 오브젝트가 이동시 인접한 인덱스들을 알고 있어야한다. (현상태의 지형 폴리곤에서..)
typedef struct ENGINE_DLL _tagAdjInfo
{
	int	iIndex;			// 해당하는 인덱스
	int	iEdgeIndex;		// 엣지 인덱스 (0, 1, 2)
}ADJINFO, *PADJINFO;

// 네비게이션은 셀이 여러개 존재한다.(최소단위 삼각형임)
typedef struct ENGINE_DLL _tagNavigationCell
{
	NAVIMESH_CELL_LIST_TYPE	eType;		// A스타할때 사용
	Vector3		vPos[3];				// 삼각형의 정점 정보
	Vector3		vEdge[3];				// 엣지정보
	Vector3		vEdgeCenter[3];			// 각 엣지들의 중점
	Vector3		vCenter;				// 해당 삼각형의 중점
	std::vector<ADJINFO>	vecAdj;		// 다른 면와 인접한 인덱스정보(엣지)
	int			iIndex;					// 셀의 인덱스
	int			iParentIdx;				// A스타용
	float		fG;						// A스타용 -  간선비용 (가중치 비용)
	float		fH;						// A스타용 - 휴리스틱(Heuristics) or 발견법
	float		fTotal;					// A스타용
	bool		bEnable;				// 네비메쉬 셀 존재여부, 플레이어나 다른 오브젝트들 이동시에 해당 셀이 존재해야만 이동가능하게 함.

	void Clear()
	{
		eType = NCLT_NONE;
		iParentIdx = -1;
		fG = -1.f;
		fH = -1.f;
		fTotal = -1.f;
	}

	_tagNavigationCell()
	{
		eType = NCLT_NONE;
		iParentIdx = -1;
		iIndex = -1;
		fG = -1.f;
		fH = -1.f;
		fTotal = -1.f;
		bEnable = true;
	}
}NavigationCell, *PNavigationCell;

// 셀리스트 ( 셀은 한개만 존재하지않음, 여러개 존재 )
typedef struct ENGINE_DLL _tagCellList
{
	PNavigationCell*	pCellList;
	int			iSize;
	int			iCapacity;

	void Resize()
	{
		if (iSize == iCapacity)
		{
			iCapacity *= 2;
			PNavigationCell*	pList = new PNavigationCell[iCapacity];

			memset(pList, 0, sizeof(PNavigationCell) * iCapacity);

			memcpy(pList, pCellList, sizeof(PNavigationCell) * iSize);

			SAFE_DELETE_ARRAY(pCellList);
			pCellList = pList;
		}
	}

	void Add(PNavigationCell pCell)
	{
		Resize();
		pCellList[iSize] = pCell;
		++iSize;
	}

	_tagCellList()
	{
		iSize = 0;
		iCapacity = 2048;
		pCellList = new PNavigationCell[iCapacity];

		memset(pCellList, 0, sizeof(PNavigationCell) * iCapacity);
	}

	~_tagCellList()
	{
		SAFE_DELETE_ARRAY(pCellList);
	}
}CellList, *PCellList;

// 네비게이션도 충돌체 처럼 공간을 나눈다.
typedef struct ENGINE_DLL _tagNavSection
{
	Vector3		vSize;					// 네비 공간 .. 몇번째 공간인지 (충돌체처럼..)
	CellList	tCellList;				// 한공간당 어떤 셀이 존재하는지.. (리스트별로 들고있음) 아마 공간나눌떄 나눠지겠지
	Vector3		vMin;					// 네비 공간의 최솟값
	Vector3		vMax;					// 네비 공간 최대값

	void Add(PNavigationCell pCell)
	{
		tCellList.Add(pCell);
	}

	_tagNavSection()
	{
	}

	~_tagNavSection()
	{
	}
}NavSection, *PNavSection;

// 우리꺼 휴리스틱 알고리즘을 이용한 A스타 적용함

// 휴리스틱 알고리즘 A*의 관리 대상
// 1. Open List			: 조사하지 않은 상태의 목록 -> 조사후 Closed List로 이동
// 2. Closed List		: 조사한 상태의 목록  -> Open List항목에 없으면 여기에 있는거임.
// 3. CostFormStart[x]	: 시작 노드로 부터 현재위치까지의 비용 -> g(n) 우리 간선비용말하는거임 그거의 총 합계.
// 4. CostFromGoal[x]	: 현재 위치에서 목표 노드까지의 비용 -> h(n) 휴리스틱 
// 5. TotalCost[x]		: 이 알고리즘이 실행 후 연산뒤 만족해야할 조건 f(n) = g(n) + h(n)

// 우선순위큐 -> A스타할때 사용
class ENGINE_DLL NavigationMesh : public Reference
{
private:
	friend class NavigationManager;

private:
	NavigationMesh();
	~NavigationMesh();

	std::string		m_strName;
	std::vector<PNavigationCell>	m_vecCell;			// 해당 네비매쉬의 전체 셀정보
	PNavigationCell*		m_pCloseCellList;				// A스타할때 사용 ( 따로빼둠) -> 이미 경로를 지난것들 모음
	int						m_iCloseCellListSize;
	Vector3		m_vOffset;								// 네비게이션이 어디서부터 시작하는지에 대한 오프셋
	Vector3		m_vOffsetScale;
	Vector3		m_vMin;									// 네비메쉬의 최솟값(위치)
	Vector3		m_vMax;									// 네비메쉬의 최댓값(위치)
	int			m_iLineRectCount;						// 한줄(그리드)의 사각형 개수
	bool		m_bGrid;								// 그리드 여부
	int			m_iSectionX;							// 네비게이션 공간만들때 X, 가로로 몇개의 공간으로 만들건지
	int			m_iSectionZ;							// 위와 동일 Z, 세로쪽
	Vector3		m_vSectionSize;							// 한 네비공간당 크기
	PNavSection	m_pSection;								// 네비게이션 공간

private:
	Heap<PNavigationCell>		m_OpenList;				// A스타용 - 열린 목록
	std::stack<int>				m_FindStack;			// A스타용 -  
	std::list<Vector3>			m_PathList;				// A스타용 - 경로라고해야하나 위치리스트들
	bool						m_bFindEnd;				

public:
	Vector3	GetMin()	const;
	Vector3 GetMax()	const;
	Vector3 GetOffset()	const;
	std::list<Vector3> GetPathList()	const;

public:
	void SetOffset(const Vector3& _vOffset);
	void SetOffsetScale(const Vector3& _vOffsetScale);

public:
	void PathListClear();

public:
	bool Init();
	void AddCell(const Vector3 _vPos[3]);				// 네비셀 추가
	void AddAdj(int _iCellIdx, int _iAdjIdx);			// 네비셀의 인접정보 추가 ( 인접한 곳에만 이동이 가능하니까 )
	void CreateGridMapAdj(int _iLineRectCount);			// 그리드용 셀 인접정보추가
	void CreateAdj();									// 인접생성 ( 그리드 아니더라도 사용가능하다.)
	bool CheckOnEdge(int _iSrc, int _iDest, const Vector3& _vOrigin1, const Vector3& _vOrigin2,
		const Vector3& _vEdge, float _fEdgeLength, int _iEdge1, int _iEdge2);
	bool CheckPathDir(const Vector3& _vSrc1, const Vector3& _vSrc2,
		const Vector3& _vDest1, const Vector3& _vDest2, Vector3& _vIntersect);
	void FindPath(const Vector3& _vStart, const Vector3& _vEnd);

public:
	float GetY(const Vector3& _vPos);
	bool CheckCell(const Vector3& _vPos);
	bool GetCellEnable(const Vector3& _vPos);
	float GetY(int _iCellIndex, const Vector3& _vPos);
	void CreateSection();

	Vector3 GetCellFaceNormal(const Vector3& _vPos);		// 면법선 알아내기

	PNavigationCell FindCell(const Vector3& _vPos);

	float NavigationMesh::GetFaceNormalAngle(Vector3 _vPos);

private:
	void AddOpenList(PNavigationCell _pCell, PNavigationCell _pEnd,
		const Vector3& _vStart, const Vector3& _vEnd);
	int GetCellIndex(const Vector3& _vPos);
	bool RayIntersectTriangle(Vector3 _rayOrigin, Vector3 _rayDir,
		Vector3 _v0, Vector3 _v1, Vector3 _v2,
		float& _t, Vector3& _vIntersect);
	int GetSectionIndex(Vector3 _vPos);

public:
	void Save(const char* _pFileName, const std::string& _strPathKey = PATH_DATA);
	void SaveFromFullPath(const char* pFullPath);
	void Load(const char* _pFileName, const std::string& _strPathKey = PATH_DATA);
	void LoadFromFullPath(const char* _pFullPath);

private:
	bool OpenListSort(const PNavigationCell& _pSrc,
		const PNavigationCell& _pDest);

public:
	// 공간에 따른 네비메쉬랑 레이충돌 -> 클릭한 지점 찾기 ( Ray, OBB 충돌 이용 )
	std::vector<int> vMouseSectionIndex;				// 마우스랑 충돌한 공간 인덱스
	int MouseSectionColCount(RayInfo _RayInfo);			// 마우스랑 해당 공간이랑 충돌한 개수
	Vector3 MouseNavCol(RayInfo _RayInfo);				// 마우스랑 지형 충돌한 지점

	bool NavigationMesh::CollisionRayToOBB(const RayInfo & _tSrc, const OBBInfo & _tDest);


public:
	// 숙제 ( 브레젠험 알고리즘으로 내가 클릭한 지점 찾기  -> 얜 그리드만 됨 )
	//  1. 충돌한 네비메쉬들의 인덱스 구하기  ( 위의 vMouseSectionIndex 이거 처럼 이용 )
	std::vector<int> vMouseNavIndex;
	Vector3 m_vMouseGridColPos;				// 마우스랑 충돌한 지점 찾기
	bool CheckMouseNavIndex(RayInfo _RayInfo);	// 브레젠험 알고리즘으로 해당 메쉬들 찾기
	bool MouseNavColGrid(RayInfo _RayInfo);		// 해당 메쉬들의 삼각형과 광선 충돌로 해당되는거 찾아서 Pos 주기
};

ENGINE_END