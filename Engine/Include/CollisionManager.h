#pragma once

#include "Component\Collider.h"

ENGINE_BEGIN

#define MAX_COLLISION_CHANNEL	32				// 현재 충돌체의 채널은 총 32개로 정한다. (그 이상일 경우 효율이 별로 안좋음 )

// 공간을 분할해서 충돌 처리를 할 건데
// 공간 분할은 그리드로 쪼갤것임.(격자방식)
// 그리고 충돌체들은 생성될때 고유식별번호(Serial Number)가 부여되며, 한계는 2^N 지금은.. 4096까지
// 충돌은 인접행렬 방식으로 처리할 것이다.
//
//   0 1 2 3 4 5 
// 0 0 1 0 0 0 0
// 1 0 0 0 0 0 0
// 2 0 0 0 0 0 0
// 3 0 0 0 0 0 0
// 4 0 0 1 0 0 0
// 5 0 0 0 0 0 0				// 1은 충돌 0 은 충돌 X 그리고 본인자신들은 충돌하지 않으므로 꼭 대각선상의 값들은 0 이다.


// 처음 _tagColliderInfo의 생성자에서
//   0 1 2 3 4 5 . . . . . 127     <- 가로의 경우 한개당 int->4바이트를 -> 비트단위로 쪼개서 32개씩 만들어짐 32 * 128 = 4096  즉 4096 * 4096 인접행렬이 나온다.
// 0 0
// 1   0
// 2     0
// 3       0
// 4         0
// ...         0
// 4096                     0

// 콜라이더 정보
typedef struct ENGINE_DLL _tagColliderInfo
{
	Collider**		pColliderList;		// 충돌할 콜라이더들 ( 월드맵에 배치된 모든 콜라이더들)
	unsigned int**	pAdjMatrix;			// 인접행렬 ( 충돌 체크 용도 )
	unsigned int	iSize;				// pColliderList 개수
	unsigned int	iCapacity;

	// 기존에 할당된것보다 더 추가하려고했을때
	void Resize()
	{
		iCapacity *= 2;

		Collider** pList = new Collider*[iCapacity];

		memcpy(pList, pColliderList, sizeof(Collider*) * iSize);

		SAFE_DELETE_ARRAY(pColliderList);

		pColliderList = pList;

		unsigned int** pMatrix = new unsigned int*[iCapacity];

		unsigned int	iWidth = iCapacity / 32;					// 128  -> 32(int를 비트단위로..) * 128 = 4096
		
		for (unsigned int i = 0; i < iCapacity; ++i)
		{
			pMatrix[i] = new unsigned int[iWidth];
			memset(pMatrix[i], 0, sizeof(unsigned int) * iWidth);
			memcpy(pMatrix[i], pAdjMatrix[i], sizeof(unsigned int) * (iWidth / 2));		// 기존에 있던것들을 넣어줘야함
			SAFE_DELETE_ARRAY(pAdjMatrix[i]);
		}

		SAFE_DELETE_ARRAY(pAdjMatrix);
		pAdjMatrix = pMatrix;
	}

	_tagColliderInfo()
	{
		iSize = 0;
		iCapacity = 4096;
		pColliderList = new Collider*[iCapacity];
		pAdjMatrix = new unsigned int*[iCapacity];

		unsigned int	iWidth = iCapacity / 32;			// 세로로 32개씩 끊어줄거임 = 128
		
		for (unsigned int i = 0; i < iCapacity; ++i)
		{
			pAdjMatrix[i] = new unsigned int[iWidth];
			memset(pAdjMatrix[i], 0, sizeof(unsigned int) * iWidth);
		}
	}

	~_tagColliderInfo()
	{
		SAFE_DELETE_ARRAY(pColliderList);

		for (unsigned int i = 0; i < iCapacity; ++i)
		{
			SAFE_DELETE_ARRAY(pAdjMatrix[i]);
		}

		SAFE_DELETE_ARRAY(pAdjMatrix);
	}

}ColliderInfo, *PColliderInfo;

// 콜라이더 리스트
typedef struct ENGINE_DLL _tagColliderList
{
	Collider**	pList;
	int		iSize;
	int		iCapacity;

	_tagColliderList()
	{
		iSize = 0;
		iCapacity = 100;
		pList = new Collider*[iCapacity];
		memset(pList, 0, sizeof(Collider*) * iCapacity);
	}

	~_tagColliderList()
	{
		SAFE_DELETE_ARRAY(pList);
	}

	// resize()
	void Resize()
	{
		iCapacity *= 2;
		Collider** pNewList = new Collider*[iCapacity];
		memset(pNewList, 0, sizeof(Collider*) * iCapacity);		// 일단 0으로 전부 초기화
		memcpy(pNewList, pList, sizeof(Collider*) * iSize);

		SAFE_DELETE_ARRAY(pList);			// 기존거 제거후 새로 추가(다시 넣어주는거임)
		pList = pNewList;
	}

	// 콜라이더 추가함수
	void Add(class Collider* _pCollider)
	{
		// 메모리공간이 가득 찼으면 공간 늘려주기
		if (iSize == iCapacity)
		{
			Resize();
		}

		pList[iSize] = _pCollider;
		++iSize;
	}

	// 클리어함수
	void Clear()
	{
		iSize = 0;
	}

}ColliderList, *PColliderList;

// 콜리전 채널
typedef struct ENGINE_DLL _tagCollisionChannel
{
	std::string strName;
	COLLISION_CHANNEL_STATE eState;

	_tagCollisionChannel()
	{
		strName = "";
		eState = CCS_IGNORE;			// 기본값
	}
}CollisionChannel, *PCollisionChannel;

// 콜리전 프로파일 -> 각 오브젝트들은 충돌체가 있을 경우 콜리전 프로파일이 Init()에서 연결되어야한다.
typedef struct ENGINE_DLL _tagCollisionProfile
{
	std::string strName;
	PCollisionChannel	pChannelList;				// 프로파일은 채널리스트들을 갖고 있다.
	unsigned int		iChannelSize;

	PCollisionChannel FindChannel(const std::string& _strName)
	{
		for (unsigned int i = 0; i < iChannelSize; ++i)
		{
			if (pChannelList[i].strName == _strName)
			{
				return &pChannelList[i];
			}
		}

		return nullptr;
	}

	_tagCollisionProfile()
	{
		strName == "";
		iChannelSize = 0;
		pChannelList = new CollisionChannel[MAX_COLLISION_CHANNEL];
	}

	~_tagCollisionProfile()
	{
		SAFE_DELETE_ARRAY(pChannelList);
	}

}CollisionProfile, *PCollisionProfile;

// 콜리전 섹션 ( 분할된 공간)
typedef struct ENGINE_DLL _tagCollisionSection
{
	PColliderList	pSectionList;			// 분할된 공간들
	int		iNumX;							// X축으로 얼마나 쪼갤건지
	int		iNumY;
	int		iNumZ;
	Vector3	vMin;							// 공간의 최솟값
	Vector3	vMax;							// 공간의 최댓값
	Vector3	vCellSize;						// 한 공간의 사이즈
	Vector3	vLength;						// 공간의 총 크기

	_tagCollisionSection() :
		iNumX(1),
		iNumY(1),
		iNumZ(1),
		pSectionList(nullptr)
	{
	}

	~_tagCollisionSection()
	{
		SAFE_DELETE_ARRAY(pSectionList);
	}
}CollisionSection, *PCollisionSection;

class ENGINE_DLL CollisionManager
{
private:
	ColliderInfo	m_tColliderInfo;
	unsigned int	m_iSerialNumber;			// 고유 식별번호를 왜 얘까 ㅇ갖고이찌??;;

#pragma region Vaild 설명
	// Collider의 고유 식별번호를 삭제했을때 따로 옮기는 작업을 하지 않고
	// 삭제된 애들을 모아둔다. -> m_pValidNumber
	// 이는 옮기는 작업보다 효율적이기 때문
	// 그냥 두면 무한대로 메모리공간을 쓰기때문에 잉여 메모리가 생김. 이걸 방지하기 위해서 사용한다.
#pragma endregion
	unsigned int*	m_pValidNumber;				// 삭제된 Collider의 고유식별번호 모음
	unsigned int	m_iValidSize;				// 삭제된 Collider의 고유식별번호 개수
	unsigned int	m_iValidCapacity;

private:
	PCollisionSection	m_pSection;				// 분할된 공간 ( 일반 모든 충돌 관련 )
	PCollisionSection	m_pPickSection;			// 피킹용 충돌 공간 ( 여기서 마우스 레이에 맞는 공간들만 나중에 처리 해주자 !! (얘 숙제;;)
	PCollisionSection	m_pUISection;			// UI 충돌 공간

	PColliderList		m_pMouseCollisionList;	// 마우스 충돌 리스트 (따로 모아둬서 정렬해야함)

	Vector3				m_vPos;					// 공간의 기준점 ( Main camera의 위치 )
	Vector3				m_vPivot;				// 0~1.0f사이의값, 즉 퍼센트로 이용됨

	class Collider* m_pPrevMousePick;			// 이전 프레임에 마우스와 충돌한 피킹용 콜라이더 모음
	class Collider* m_pPrevMouseCollider;		// 이전 프레임에 마우스와 충돌한 월드 오브젝트 콜라이더 모음

private:
	PCollisionProfile	m_pProfileList;			// Profile List 
	unsigned int		m_iProfileSize;
	unsigned int		m_iProfileCapacity;

	PCollisionChannel	m_pChannelList;			// Channel List -> 전체 채널 리스트
	unsigned int		m_iChannelSize;

public:
	unsigned int GetSerialNumber();								// 식별 번호를 부여하는 Get함수
	void AddValidSerialNumber(unsigned int _iNumber);			// 콜라이더가 삭제될때마다 해당 식별번호를 가져올 함수(여튼 삭제될 콜라이더의 식별번호를 모아서 나중에 콜라이더를 생성할때 삭제된 콜라이더의 식별번호가 있을 경우 그 식별번호를 부여할 것임)

	// 충돌한애들을 모아두거나 삭제, 체크할 함수
	void AddPrevCollider(unsigned int _iSrc, unsigned int _iDest);
	void DeletePrevCollider(unsigned int _iSrc, unsigned int _iDest);
	bool CheckPrevCollider(unsigned int _iSrc, unsigned int _iDest);

public:
	// 공간 분할
	void CreateSection(int _iNumX, int _iNumY, int _iNumZ,
		const Vector3& _vCellSize, const Vector3& _vPos = Vector3::Zero,
		const Vector3& _vPivot = Vector3::Zero);

	bool AddChannel(const std::string& _strName, COLLISION_CHANNEL_STATE eState = CCS_IGNORE);	// 채널 추가, 현재는 3갠뎅 내가 나중에 채널값들을 추가하고 싶으면 추가해주면 된다.
	bool DeleteChannel(const std::string& _strName);											// 채널 삭제
	bool CheckChannel(const std::string& _strName);
	unsigned int GetChannelIndex(const std::string& _strName);				// 채널 인덱스 찾기

	bool CreateProfile(const std::string& _strName);						// 프로파일 생성 
	bool SetProfileChannelState(const std::string& _strProfile,
		const std::string& _strChannel, COLLISION_CHANNEL_STATE _eState);	// 프로파일안의 채널 셋팅을바꿀수있는 함수
	PCollisionProfile FindProfile(const std::string& _strName);				// 프로파일 찾기

	void AddCollider(class GameObject* _pObject);							// 콜라이더 추가하기(생성되면 추가하는듯) -> 무조건 생성될때마다 호출할애
	void AddCollider(class Collider* pCollider);

	class Collider* FindCollider(unsigned int _iSerialNumber);				// 콜라이더 찾기 ( 고유 식별 번호로 )
	void DeleteCollider(unsigned int _iSerialNumber);						// 콜라이더 삭제
	void ComputeSection();													// 공간 계산

public:
	bool Init();
	void Collision(float _fTime);
	void Render(float _fTime);

public:
	void DeletePrevMouse(Collider* _pCollider);

private:
	bool CollisionMouseUI(float _fTime);			// 마우스랑 UI 충돌
	bool CollisionMouseWorld(float _fTime);			// 마우스랑 월드 오브젝트들 충돌
	bool CollisionWorld(float _fTime);				// 월드 오브젝트들끼리 충돌(기존충돌)
	bool Collision(class Collider* _pSrc, class Collider* _pDest);	// 충돌

public:
	static int SortZ(const void* _pSrc, const void* _pDest);

	DECLARE_SINGLETON(CollisionManager)
};

ENGINE_END
