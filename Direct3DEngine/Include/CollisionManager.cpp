#include "CollisionManager.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Component\ColliderRay.h"
#include "Core.h"
#include "Scene\SceneManager.h"
#include <algorithm>

ENGINE_USING

DEFINITION_SINGLETON(CollisionManager)

CollisionManager::CollisionManager()
{
	m_pSection = nullptr;				// 일반 월드 충돌 공간
	m_pPickSection = nullptr;			// 피킹용 공간
	m_pUISection = nullptr;				// UI충돌 공간

	m_pPrevMousePick = nullptr;
	m_pPrevMouseCollider = nullptr;

	m_pMouseCollisionList = new ColliderList;			// 마우스 충돌 리스트

	m_iSerialNumber = 0;

	m_iProfileSize = 0;
	m_iProfileCapacity = 16;				// 미리 크기를 16로 정하고 나중에 초과시 더 추가하면 된다.
	m_pProfileList = new CollisionProfile[m_iProfileCapacity];

	m_iChannelSize = 0;
	m_pChannelList = new CollisionChannel[MAX_COLLISION_CHANNEL];			// 채널 개수는 일단 최대 32개까지 갖는데 더 갖을 경우 효율이 별로일 수있음

	m_iValidSize = 0;
	m_iValidCapacity = 100;

	m_pValidNumber = new unsigned int[m_iValidCapacity];					// 얘도 미리 만들어두기만 하는거 ㅇㅅㅇ 나중에 추가될 수 있음
	memset(m_pValidNumber, 0, sizeof(unsigned int) * m_iValidCapacity);		// 0으로 초기화 안하면 쓰레기값 들어감
}


CollisionManager::~CollisionManager()
{
	SAFE_DELETE(m_pMouseCollisionList);
	SAFE_DELETE_ARRAY(m_pValidNumber);
	SAFE_DELETE_ARRAY(m_pChannelList);
	SAFE_DELETE_ARRAY(m_pProfileList);
	SAFE_DELETE(m_pSection);
	SAFE_DELETE(m_pPickSection);
	SAFE_DELETE(m_pUISection);
}

// 고유 식별 번호 부여 각 Collider의 Start()함수에서 호출된다.
unsigned int CollisionManager::GetSerialNumber()
{
	// 삭제된 식별번호가 없다면 추가해서 넣어주기
	if (m_iValidSize == 0)
	{
		if (m_iSerialNumber == m_tColliderInfo.iCapacity)
		{
			m_tColliderInfo.Resize();
		}

		unsigned int iNumber = m_iSerialNumber;
		++m_iSerialNumber;
		return iNumber;
	}

	// 삭제된 식별번호들이 있다면
	// 그 삭제된 식별번호를 부여한다. (뒤에서부터ㅣ...)
	--m_iValidSize;

	return m_pValidNumber[m_iValidSize];
}

// 고유식별번호를 삭제할때 사용할꺼.. 즉 콜라이더 삭제할떄?? 사용할거임
void CollisionManager::AddValidSerialNumber(unsigned int _iNumber)
{
	if (m_iValidSize == m_iValidCapacity)
	{
		m_iValidCapacity *= 2;

		unsigned int*	pNumber = new unsigned int[m_iValidCapacity];

		memcpy(pNumber, m_pValidNumber, sizeof(unsigned int) * m_iValidSize);

		SAFE_DELETE_ARRAY(m_pValidNumber);

		m_pValidNumber = pNumber;
	}

	m_pValidNumber[m_iValidSize] = _iNumber;
	++m_iValidSize;

	m_tColliderInfo.pColliderList[_iNumber] = nullptr;		// 삭제했으니까 기존의 위치에 nullptr을 넣는다.
	--m_tColliderInfo.iSize;								// 콜라이더 개수도 줄여주고 ㅇㅅㅇ
}

void CollisionManager::AddPrevCollider(unsigned int _iSrc, unsigned int _iDest)
{
	// _iSrc, _iDest : 고유 식별 번호들 Serial Number
	unsigned int	iSrcIdx = _iSrc / 32;				// 현재 int를 비트단위로 쪼개서 사용.. -> 그래서 32로 나눠야 자기 인덱스를 앎
	unsigned int	iDestIdx = _iDest / 32;				// 얘도 ㅇㅅㅇ

	unsigned int	iSrcBit = 31 - (_iSrc % 32);		// 해당인덱스는 32비트로 이뤄져있으니까 자기가 어느 즉, 몇번째 비트에 속해야하는지.. (얘도 인덱스임 ㅇㅁㅇ..)
	unsigned int	iDestBit = 31 - (_iDest % 32);

	unsigned int	iSrcValue = 1 << iSrcBit;			// 쉬프트 연산자로 맞는 위치에 넣어준다!! 0과 1만 사용할것임 ㅇㅅㅇ 0은 충돌 X 1은 충돌
	unsigned int	iDestValue = 1 << iDestBit;

	// 인접행렬에 값을 넣어준다. (현재값과 추가해야될 값을 비교해서 넣는데 OR연산임 )
	// [세로 = 4096 ][가로 = 128 * 32(4byte) = 4096]
	// |연산으로 항상 추가되게한다.  = 1
	m_tColliderInfo.pAdjMatrix[_iSrc][iDestIdx] |= iDestValue;
	m_tColliderInfo.pAdjMatrix[_iDest][iSrcIdx] |= iSrcValue;
}

void CollisionManager::DeletePrevCollider(unsigned int _iSrc, unsigned int _iDest)
{
	unsigned int	iSrcIdx = _iSrc / 32;
	unsigned int	iDestIdx = _iDest / 32;

	unsigned int	iSrcBit = 31 - (_iSrc % 32);
	unsigned int	iDestBit = 31 - (_iDest % 32);

	unsigned int	iSrcValue = 1 << iSrcBit;
	unsigned int	iDestValue = 1 << iDestBit;

	if (m_tColliderInfo.pAdjMatrix[_iSrc][iDestIdx] & iDestValue)
		m_tColliderInfo.pAdjMatrix[_iSrc][iDestIdx] ^= iDestValue;

	if (m_tColliderInfo.pAdjMatrix[_iDest][iSrcIdx] & iSrcValue)
		m_tColliderInfo.pAdjMatrix[_iDest][iSrcIdx] ^= iSrcValue;
}

bool CollisionManager::CheckPrevCollider(unsigned int _iSrc, unsigned int _iDest)
{
	unsigned int	iDestIdx = _iDest / 32;

	unsigned int	iDestBit = 31 - (_iDest % 32);

	unsigned int	iDestValue = 1 << iDestBit;

	if (m_tColliderInfo.pAdjMatrix[_iSrc][iDestIdx] & iDestValue)
		return true;

	return false;
}

void CollisionManager::CreateSection(int _iNumX, int _iNumY, int _iNumZ,
	const Vector3 & _vCellSize, 
	const Vector3 & _vPos /*= Vector3::Zero*/,
	const Vector3 & _vPivot /*= Vector3::Zero*/)
{
	// 기존꺼 삭제
	if (nullptr != m_pSection)
	{
		SAFE_DELETE(m_pSection);
	}

	m_pSection = new CollisionSection;

	m_pSection->iNumX = _iNumX;
	m_pSection->iNumY = _iNumY;
	m_pSection->iNumZ = _iNumZ;
	m_pSection->vCellSize = _vCellSize;			// 분할된 공간 한개의 사이즈

	m_pSection->pSectionList = new ColliderList[_iNumX * _iNumY * _iNumZ];
	m_pSection->vLength = m_pSection->vCellSize * Vector3(_iNumX, _iNumY, _iNumZ);			// 공간의 총 크기

	m_vPos = _vPos;					// 공간의 기준점
	m_vPivot = _vPivot;				// 공간기준점의 피봇 percent값 (  )  0~1.0f사이의값

	m_pSection->vMin = m_vPos - m_vPivot * m_pSection->vLength;						// 공간의 최솟값( 기준점을 기준으로의 최솟값임)
	m_pSection->vMax = m_vPos + (Vector3(1.0f, 1.0f, 1.0f) - m_vPivot) * m_pSection->vLength;		// 공간의 최댓값

	// Edit모드가 활성화 되어있을때만 피킹용 공간을 만든다.
	if (true == GET_SINGLETON(Core)->GetEditMode())
	{
		SAFE_DELETE(m_pPickSection);			// 기존 공간 삭제(있으면..)

		m_pPickSection = new CollisionSection;
		
		m_pPickSection->iNumX = _iNumX;
		m_pPickSection->iNumY = _iNumY;
		m_pPickSection->iNumZ = _iNumZ;
		m_pPickSection->vCellSize = _vCellSize;

		m_pPickSection->pSectionList = new ColliderList[_iNumX * _iNumY * _iNumZ];
		m_pPickSection->vLength = m_pPickSection->vCellSize * Vector3(_iNumX, _iNumY, _iNumZ);
		m_pPickSection->vMin = m_vPos - m_vPivot * m_pPickSection->vLength;
		m_pPickSection->vMax = m_vPos + (Vector3(1.0f, 1.0f, 1.0f) - m_vPivot) * m_pPickSection->vLength;
	}
}

bool CollisionManager::AddChannel(const std::string & _strName,
	COLLISION_CHANNEL_STATE _eState /*= CCS_IGNORE*/)
{
	// 현재 채널의 최대 개수는 32개로 설정되어 있다. 그 이상을 만들려고하면 return
	if (m_iChannelSize == MAX_COLLISION_CHANNEL)
		return false;

	// 같은 이름을 가졌으면 return 시킨다.
	if (true == CheckChannel(_strName))
	{
		return false;
	}

	m_pChannelList[m_iChannelSize].strName = _strName;
	m_pChannelList[m_iChannelSize].eState = _eState;

	// 현재 생성되어 있는 모든 Profile에 추가된 채널 정보를 추가한다.
	for (unsigned int i = 0; i < m_iProfileSize; ++i)
	{
		m_pProfileList[i].pChannelList[m_iChannelSize].strName = _strName;
		m_pProfileList[i].pChannelList[m_iChannelSize].eState = _eState;
		++m_pProfileList[i].iChannelSize;
	}

	++m_iChannelSize;

	return true;
}

bool CollisionManager::DeleteChannel(const std::string & _strName)
{
	for (unsigned int i = 0; i < m_iChannelSize; ++i)
	{
		if (m_pChannelList[i].strName == _strName)
		{
			--m_iChannelSize;

			for (unsigned int j = i; j < m_iChannelSize; ++j)
			{
				// 채널을 삭제 해준 후에 
				// 따로 당기는 작업.. 즉 옆으로 이동시키는 작업을 해준다. ( 벡터처럼 )
				// 모두 해주는 건 비효율적이기 떄문에 삭제된 공간부터 해주는 것이다.
				m_pChannelList[j] = m_pChannelList[j + 1];
			}

			for (unsigned int j = 0; j < m_iProfileSize; ++j)
			{
				// 채널리스트들을 줄여줬으니까
				// 프로파일의 채널도 똑같이 제거해준다.(그냥 위와같이 당기는 작업임)
				--m_pProfileList[j].iChannelSize;
				for (unsigned int k = i; k < m_iChannelSize; ++k)
				{
					m_pProfileList[j].pChannelList[k] = m_pProfileList[j].pChannelList[k + 1];
				}
			}

			break;
		}
	}

	return true;
}

bool CollisionManager::CheckChannel(const std::string & _strName)
{
	for (unsigned int i = 0; i < m_iChannelSize; ++i)
	{
		if (m_pChannelList[i].strName == _strName)
			return true;
	}

	return false;
}

bool CollisionManager::CreateProfile(const std::string & _strName)
{
	// 미리 할당해둔 메모리 보다 더 넣어둘려면
	// 메모리를 더 늘려준다. 그리고나서 추가함(기존값 채우고..)
	if (m_iProfileSize == m_iProfileCapacity)
	{
		m_iProfileCapacity *= 2;

		PCollisionProfile	pProfileList = new CollisionProfile[m_iProfileCapacity];

		for (unsigned int i = 0; i < m_iProfileSize; ++i)
		{
			pProfileList[i].strName = m_pProfileList[i].strName;

			for (unsigned int j = 0; j < m_pProfileList[i].iChannelSize; ++j)
			{
				pProfileList[i].pChannelList[j] = m_pProfileList[i].pChannelList[j];
			}

			pProfileList[i].iChannelSize = m_pProfileList[i].iChannelSize;
		}

		SAFE_DELETE_ARRAY(m_pProfileList);

		m_pProfileList = pProfileList;
	}

	m_pProfileList[m_iProfileSize].strName = _strName;
	m_pProfileList[m_iProfileSize].iChannelSize = m_iChannelSize;

	for (unsigned int i = 0; i < m_iChannelSize; ++i)
	{
		m_pProfileList[m_iProfileSize].pChannelList[i] =
			m_pChannelList[i];
	}

	++m_iProfileSize;

	return true;
}

bool CollisionManager::SetProfileChannelState(const std::string & _strProfile,
	const std::string & _strChannel,
	COLLISION_CHANNEL_STATE _eState)
{
	for (unsigned int i = 0; i < m_iProfileSize; ++i)
	{
		if (m_pProfileList[i].strName == _strProfile)
		{
			for (unsigned int j = 0; j < m_iChannelSize; ++j)
			{
				if (m_pProfileList[i].pChannelList[j].strName == _strChannel)
				{
					m_pProfileList[i].pChannelList[j].eState = _eState;
					return true;
				}
			}
		}
	}

	return false;
}

PCollisionProfile CollisionManager::FindProfile(const std::string & _strName)
{
	for (unsigned int i = 0; i < m_iProfileSize; ++i)
	{
		if (m_pProfileList[i].strName == _strName)
			return &m_pProfileList[i];
	}

	return nullptr;
}

unsigned int CollisionManager::GetChannelIndex(const std::string & _strName)
{
	for (unsigned int i = 0; i < m_iChannelSize; ++i)
	{
		if (m_pChannelList[i].strName == _strName)
			return i;
	}

	return UINT_MAX;
}

void CollisionManager::AddCollider(GameObject * _pObject)
{
	if (false == _pObject->IsActive())
	{
		return;
	}

	const std::list<Collider*>*	pColliderList = _pObject->GetColliderList();				// 콜라이더를 추가한 오브젝트의 콜라이더 리스트들을 가져온다. 한 게임오브젝트는 여러개의 콜라이더를 가질 수 있다.

	std::list<Collider*>::const_iterator StartIter = pColliderList->begin();
	std::list<Collider*>::const_iterator EndIter = pColliderList->end();

	if (true == pColliderList->empty())
	{
		return;
	}

	else if (false == _pObject->IsEnable())
	{
		// 게임 오브젝트가 활성화된 상태가아니라면 현재 그 오브젝트와 관련된 콜라이더의 충돌된 
		// 다른 콜라이더들의 상태들을 전부 지워준다. ( 콜라이더 비활성화상태니까 )

		for (; StartIter != EndIter; ++StartIter)
		{
			(*StartIter)->ClearState();
		}

		return;
	}


	for (StartIter = pColliderList->begin(); StartIter != EndIter; ++StartIter)
	{
		Collider*	pCollider = *StartIter;

		// 해당 콜라이더가 비활성화 상태라면 상태를 지워준다.
		if (false == pCollider->IsEnable())
		{
			pCollider->ClearState();
			continue;
		}

		if (true == pCollider->GetPickEnable())
		{
			// 피킹용 콜라이더면 일반 충돌체 공간에 넣지 않는다.
			// 피킹용 콜라이더 공간에 따로 넣을거임.
			pCollider->ClearState();
			continue;
		}

		// 충돌체의 Min, Max 값을 얻어온다.
		Vector3	vCollMin = pCollider->GetSectionMin();
		Vector3	vCollMax = pCollider->GetSectionMax();

		// 전체 영역의 Min값을 제거해서 0, 0, 0으로 만들어준다.
		vCollMin -= m_pSection->vMin;
		vCollMax -= m_pSection->vMin;

		// 1, 1, 1 공간으로 변환한다.  -> 자신이 공간의 어디의 공간에 위치하는지..
		vCollMin /= m_pSection->vCellSize;
		vCollMax /= m_pSection->vCellSize;

		int	iStartX = -1, iStartY = -1, iStartZ = -1;
		int	iEndX = -1, iEndY = -1, iEndZ = -1;

		iStartX = (int)vCollMin.x;
		iStartY = (int)vCollMin.y;
		iStartZ = (int)vCollMin.z;

		iEndX = (int)vCollMax.x;
		iEndY = (int)vCollMax.y;
		iEndZ = (int)vCollMax.z;

		if (iStartX >= m_pSection->iNumX)
			continue;

		else if (iStartY >= m_pSection->iNumY)
			continue;

		else if (iStartZ >= m_pSection->iNumZ)
			continue;

		iStartX = iStartX < 0 ? 0 : iStartX;
		iStartY = iStartY < 0 ? 0 : iStartY;
		iStartZ = iStartZ < 0 ? 0 : iStartZ;

		if (iEndX < 0)
			continue;

		else if (iEndY < 0)
			continue;

		else if (iEndZ < 0)
			continue;

		iEndX = iEndX >= m_pSection->iNumX ? m_pSection->iNumX - 1 : iEndX;
		iEndY = iEndY >= m_pSection->iNumY ? m_pSection->iNumY - 1 : iEndY;
		iEndZ = iEndZ >= m_pSection->iNumZ ? m_pSection->iNumZ - 1 : iEndZ;

		for (int z = iStartZ; z <= iEndZ; ++z)
		{
			for (int y = iStartY; y <= iEndY; ++y)
			{
				for (int x = iStartX; x <= iEndX; ++x)
				{
					int	idx = z * (m_pSection->iNumX * m_pSection->iNumY) +
						y * m_pSection->iNumX + x;

					PColliderList	pSection = &m_pSection->pSectionList[idx];

					if (pSection->iSize == pSection->iCapacity)
					{
						pSection->iCapacity *= 2;

						Collider**	pList = new Collider*[pSection->iCapacity];

						memcpy(pList, pSection->pList, sizeof(Collider*) * pSection->iSize);

						SAFE_DELETE_ARRAY(pSection->pList);

						pSection->pList = pList;
					}

					pSection->pList[pSection->iSize] = pCollider;			// 콜라이더 추가
					++pSection->iSize;
				}
			}
		}
	}
}

void CollisionManager::AddCollider(Collider * _pCollider)
{
	// 콜라이더들을 모아두는 리스트에 넣어둔다. ( 고유식별번호로 판별 )
	m_tColliderInfo.pColliderList[_pCollider->GetSerialNumber()] = _pCollider;
	++m_tColliderInfo.iSize;
}

Collider * CollisionManager::FindCollider(unsigned int _iSerialNumber)
{
	return m_tColliderInfo.pColliderList[_iSerialNumber];
}

void CollisionManager::DeleteCollider(unsigned int _iSerialNumber)
{
	m_tColliderInfo.pColliderList[_iSerialNumber] = nullptr;
}

void CollisionManager::ComputeSection()
{
	// 충돌체들 공간계산
	for (unsigned iColl = 0; iColl < m_tColliderInfo.iSize + m_iValidSize; ++iColl)
	{
		Collider*	pCollider = m_tColliderInfo.pColliderList[iColl];

		if (nullptr == pCollider)
		{
			continue;
		}

		if (false == pCollider->IsEnable() || false == pCollider->IsObjectEnable())
		{
			pCollider->ClearState();
			continue;
		}

		// 충돌체의 Min, Max 값을 얻어온다.
		Vector3	vCollMin = pCollider->GetSectionMin();
		Vector3	vCollMax = pCollider->GetSectionMax();

		// 전체 영역의 Min값을 제거해서 0, 0, 0으로 만들어준다.
		vCollMin -= m_pSection->vMin;
		vCollMax -= m_pSection->vMin;

		// 1, 1, 1 공간으로 변환한다.
		vCollMin /= m_pSection->vCellSize;
		vCollMax /= m_pSection->vCellSize;

		int	iStartX = -1, iStartY = -1, iStartZ = -1;
		int	iEndX = -1, iEndY = -1, iEndZ = -1;

		iStartX = (int)vCollMin.x;
		iStartY = (int)vCollMin.y;
		iStartZ = (int)vCollMin.z;

		iEndX = (int)vCollMax.x;
		iEndY = (int)vCollMax.y;
		iEndZ = (int)vCollMax.z;

		// 충돌할 공간을 벗어나면..(제한된 공간 벗어나면 충돌 처리 X)
		if (iStartX >= m_pSection->iNumX || iStartY >= m_pSection->iNumY ||
			iStartZ >= m_pSection->iNumZ || iEndX < 0 || iEndY < 0 ||
			iEndZ < 0)
		{
			pCollider->ClearState();
			continue;
		}

		iStartX = iStartX < 0 ? 0 : iStartX;
		iStartY = iStartY < 0 ? 0 : iStartY;
		iStartZ = iStartZ < 0 ? 0 : iStartZ;

		iEndX = iEndX >= m_pSection->iNumX ? m_pSection->iNumX - 1 : iEndX;
		iEndY = iEndY >= m_pSection->iNumY ? m_pSection->iNumY - 1 : iEndY;
		iEndZ = iEndZ >= m_pSection->iNumZ ? m_pSection->iNumZ - 1 : iEndZ;

		for (int z = iStartZ; z <= iEndZ; ++z)
		{
			for (int y = iStartY; y <= iEndY; ++y)
			{
				for (int x = iStartX; x <= iEndX; ++x)
				{
					int	idx = z * (m_pSection->iNumX * m_pSection->iNumY) +
						y * m_pSection->iNumX + x;

					PColliderList	pSection = &m_pSection->pSectionList[idx];

					// 피킹용이고 에디터면.. 피킹용 공간으로 바꾼다.
					if (true == pCollider->GetPickEnable())
					{
						if (true == GET_SINGLETON(Core)->GetEditMode())
						{
							pSection = &m_pPickSection->pSectionList[idx];
						}
						else
						{
							continue;
						}
					}

					if (pSection->iSize == pSection->iCapacity)
					{
						pSection->iCapacity *= 2;

						Collider**	pList = new Collider*[pSection->iCapacity];

						memcpy(pList, pSection->pList, sizeof(Collider*) * pSection->iSize);

						SAFE_DELETE_ARRAY(pSection->pList);

						pSection->pList = pList;
					}

					/*for (int i = 0; i < pSection->iSize; ++i)
					{
					if (pSection->pList[i] == pCollider)
					int a = 10;
					}*/

					pSection->pList[pSection->iSize] = pCollider;
					++pSection->iSize;
				}
			}
		}
	}
}

bool CollisionManager::Init()
{
	// 공간분할
	CreateSection(5, 5, 5, Vector3(30.f, 30.f, 30.f));

	CreateProfile("WorldStatic");
	CreateProfile("WorldDynamic");

	AddChannel("WorldStatic");
	AddChannel("WorldDynamic");

	SetProfileChannelState("WorldStatic", "WorldStatic", CCS_IGNORE);
	SetProfileChannelState("WorldStatic", "WorldDynamic", CCS_BLOCK);

	SetProfileChannelState("WorldDynamic", "WorldStatic", CCS_BLOCK);
	SetProfileChannelState("WorldDynamic", "WorldDynamic", CCS_BLOCK);

	return true;
}

void CollisionManager::Collision(float _fTime)
{
	// 충돌 전에 공간을 분리해준다. -> 충돌체들이 어느 공간에 들어가야하는지 넣어줌(매번 갱신)
	ComputeSection();

	bool bCollision = CollisionMouseUI(_fTime);

	// UI가아닐때 다음을 실행
	if (false == bCollision)
	{
		CollisionMouseWorld(_fTime);
	}

	CollisionMouseUI(_fTime);
	CollisionWorld(_fTime);

	if (nullptr != m_pPickSection)
	{
		int SectionCount = m_pPickSection->iNumX * m_pPickSection->iNumY * m_pPickSection->iNumZ;

		for (int i = 0; i <SectionCount; ++i)
		{
			m_pPickSection->pSectionList[i].Clear();
		}
	}
}

void CollisionManager::Render(float _fTime)
{
	// 렌더해주기
	for (unsigned iColl = 0; iColl < m_tColliderInfo.iSize + m_iValidSize;)
	{
		Collider*	pCollider = m_tColliderInfo.pColliderList[iColl];

		if (nullptr == pCollider)
		{
			++iColl;
			continue;
		}

		if (false == pCollider->IsEnable())
		{
			++iColl;
			continue;
		}

		if (false == pCollider->IsActive())
		{
			++iColl;
			continue;
		}

		pCollider->PrevRender(_fTime);
		pCollider->Render(_fTime);
		++iColl;
	}
}

void CollisionManager::DeletePrevMouse(Collider* _pCollider)
{
	ColliderRay* pMouseRay = GET_SINGLETON(InputManager)->GetMouseRay();

	if (_pCollider == m_pPrevMouseCollider)
	{
		pMouseRay->DeletePrevSerialNumber(m_pPrevMouseCollider->GetSerialNumber());
		m_pPrevMouseCollider->DeletePrevSerialNumber(pMouseRay->GetSerialNumber());

		DeletePrevCollider(pMouseRay->GetSerialNumber(),
			m_pPrevMouseCollider->GetSerialNumber());

		m_pPrevMouseCollider = nullptr;
	}

	if (_pCollider == m_pPrevMousePick)
	{
		pMouseRay->DeletePrevSerialNumber(m_pPrevMousePick->GetSerialNumber());
		m_pPrevMousePick->DeletePrevSerialNumber(pMouseRay->GetSerialNumber());

		DeletePrevCollider(pMouseRay->GetSerialNumber(), m_pPrevMousePick->GetSerialNumber());
		m_pPrevMousePick = nullptr;
	}

	SAFE_RELEASE(pMouseRay);
}

// 마우스 - UI 충돌
bool CollisionManager::CollisionMouseUI(float _fTime)
{
	return false;
}

// 마우스 - 월드오브젝트 충돌
bool CollisionManager::CollisionMouseWorld(float _fTime)
{
	// 마우스 광선을 얻어온다.
	ColliderRay* pMouseRay = GET_SINGLETON(InputManager)->GetMouseRay();

	RayInfo tRayInfo = pMouseRay->GetInfo();

	Vector3	vCollMin, vCollMax;
	Vector3	vPos1, vPos2;
	vPos1 = tRayInfo.vOrigin;
	vPos2 = tRayInfo.vOrigin + tRayInfo.vDir * 1500.f;

	vCollMin.x = vPos1.x < vPos2.x ? vPos1.x : vPos2.x;
	vCollMin.y = vPos1.y < vPos2.y ? vPos1.y : vPos2.y;
	vCollMin.z = vPos1.z < vPos2.z ? vPos1.z : vPos2.z;

	vCollMax.x = vPos1.x > vPos2.x ? vPos1.x : vPos2.x;
	vCollMax.y = vPos1.y > vPos2.y ? vPos1.y : vPos2.y;
	vCollMax.z = vPos1.z > vPos2.z ? vPos1.z : vPos2.z;

	vCollMin /= m_pSection->vCellSize;
	vCollMax /= m_pSection->vCellSize;

	int	iStartX = -1, iStartY = -1, iStartZ = -1;
	int	iEndX = -1, iEndY = -1, iEndZ = -1;

	iStartX = (int)vCollMin.x;
	iStartY = (int)vCollMin.y;
	iStartZ = (int)vCollMin.z;

	iEndX = (int)vCollMax.x;
	iEndY = (int)vCollMax.y;
	iEndZ = (int)vCollMax.z;

	iStartX = iStartX < 0 ? 0 : iStartX;
	iStartY = iStartY < 0 ? 0 : iStartY;
	iStartZ = iStartZ < 0 ? 0 : iStartZ;

	iEndX = iEndX >= m_pSection->iNumX ? m_pSection->iNumX - 1 : iEndX;
	iEndY = iEndY >= m_pSection->iNumY ? m_pSection->iNumY - 1 : iEndY;
	iEndZ = iEndZ >= m_pSection->iNumZ ? m_pSection->iNumZ - 1 : iEndZ;

	bool	bEditPick = false;

	if (true == GET_SINGLETON(Core)->GetEditMode())
	{
		for (int z = iStartZ; z <= iEndZ; ++z)
		{
			for (int y = iStartY; y <= iEndY; ++y)
			{
				for (int x = iStartX; x <= iEndX; ++x)
				{
					int	idx = z * (m_pPickSection->iNumX * m_pPickSection->iNumY) +
						y * m_pPickSection->iNumX + x;

					PColliderList	pSectionColList = &m_pPickSection->pSectionList[idx];
					
					
					for (int i = 0; i < pSectionColList->iSize; ++i)
					{
						m_pMouseCollisionList->Add(pSectionColList->pList[i]);
					}
				}
			}
		}

		// 추가된 충돌체들을 거리 순으로 오름차순 정렬한다.
		qsort(m_pMouseCollisionList->pList, m_pMouseCollisionList->iSize,
			sizeof(Collider*), CollisionManager::SortZ);

		for (int i = 0; i < m_pMouseCollisionList->iSize; ++i)
		{
			Collider*	pDest = m_pMouseCollisionList->pList[i];

			if (true == pMouseRay->Collision(pDest))
			{

				// 이전에 마우스와 충돌한 피킹용 콜라이더가 존재하고, 
				// 현재 마우스 충돌 리스트를 가져온것중에 피킹용 모음이 아니면 지워준다.
				// 안지워주면 계속 충돌한 채로 남을 수 있기때문에 충돌하지않으면 충돌했다고 한걸 지워준다.
				if (nullptr != m_pPrevMousePick && pDest != m_pPrevMousePick)
				{
					pMouseRay->DeletePrevSerialNumber(m_pPrevMousePick->GetSerialNumber());
					m_pPrevMousePick->DeletePrevSerialNumber(pMouseRay->GetSerialNumber());

					DeletePrevCollider(pMouseRay->GetSerialNumber(),
						m_pPrevMousePick->GetSerialNumber());
				}

				// 각 충돌체에 서로 충돌된 충돌체라고 등록해준다.
				// 현재 영역에서 충돌되었다고 등록을 해준다.
				// 이 리스트는 다음 영역에서 충돌체크시에 이전 영역에서
				// 충돌되었다면 빠져나가기 위함이다.
				// 왜냐하면 지금 서로 다른 영역에 걸쳐있을때 앞에 체크한
				// 영역에서 만약 충돌이 되었다면 이 충돌체는 계속
				// 충돌상태라고 표현이 되어버리므로 이전 영역에서
				// 충돌이 되었는지를 판단해야 한다.
				pMouseRay->AddCollisionList(pDest->GetSerialNumber());
				pDest->AddCollisionList(pMouseRay->GetSerialNumber());

				// 이전에 충돌되었는지를 판단한다.
				// 처음 충돌될 경우
				if (!CheckPrevCollider(pMouseRay->GetSerialNumber(),
					pDest->GetSerialNumber()))
				{
					// 충돌 매트릭스에 이전충돌목록으로
					// 등록해준다.
					AddPrevCollider(pMouseRay->GetSerialNumber(),
						pDest->GetSerialNumber());

					// 각 충돌체에 이전 충돌목록으로 등록한다.
					pMouseRay->AddPrevSerialNumber(pDest->GetSerialNumber());
					pDest->AddPrevSerialNumber(pMouseRay->GetSerialNumber());

					// 처음 충돌되었으므로 처음 충돌되었을때 호출할
					// 콜백을 처리한다.
					pMouseRay->Call(CCBS_ENTER, pDest, _fTime);
					pDest->Call(CCBS_ENTER, pMouseRay, _fTime);
				}

				// 이전 충돌목록에 있을 경우 계속 충돌상태로
				// 처리한다.
				else
				{
					// 콜백을 처리한다.
					pMouseRay->Call(CCBS_STAY, pDest, _fTime);
					pDest->Call(CCBS_STAY, pMouseRay, _fTime);
				}

				bEditPick = true;
				m_pPrevMousePick = pDest;
				break;
			}

			// 충돌이 안된 상태일 경우
			else
			{
				// 이전 충돌목록에 서로 존재할 경우 충돌 되다가
				// 떨어진다는 것이다.
				if (CheckPrevCollider(pMouseRay->GetSerialNumber(),
					pDest->GetSerialNumber()))
				{
					// 이전 충돌목록에서 제거해준다.
					DeletePrevCollider(pMouseRay->GetSerialNumber(),
						pDest->GetSerialNumber());

					pMouseRay->DeletePrevSerialNumber(pDest->GetSerialNumber());
					pDest->DeletePrevSerialNumber(pMouseRay->GetSerialNumber());

					// 콜백을 처리한다.
					pMouseRay->Call(CCBS_EXIT, pDest, _fTime);
					pDest->Call(CCBS_EXIT, pMouseRay, _fTime);
				}
			}
		}

	}

	// 에디터 모드가 아니면 피킹이 필요없으로 지워준다.
	if (false == bEditPick)
	{
		if (nullptr != m_pPrevMousePick)
		{
			pMouseRay->DeletePrevSerialNumber(m_pPrevMousePick->GetSerialNumber());
			m_pPrevMousePick->DeletePrevSerialNumber(pMouseRay->GetSerialNumber());

			DeletePrevCollider(pMouseRay->GetSerialNumber(), m_pPrevMousePick->GetSerialNumber());
		}

		m_pPrevMousePick = nullptr;
	}


	// 아래는 피킹용이 아닌듯
	for (int z = iStartZ; z <= iEndZ; ++z)
	{
		for (int y = iStartY; y <= iEndY; ++y)
		{
			for (int x = iStartX; x <= iEndX; ++x)
			{
				int	idx = z * (m_pSection->iNumX * m_pSection->iNumY) +
					y * m_pSection->iNumX + x;

				PColliderList	pSectionColList = &m_pSection->pSectionList[idx];

				for (int i = 0; i < pSectionColList->iSize; ++i)
				{
					m_pMouseCollisionList->Add(pSectionColList->pList[i]);
				}
			}
		}
	}

	// 추가된 충돌체들을 거리 순으로 오름차순 정렬한다.
	qsort(m_pMouseCollisionList->pList, m_pMouseCollisionList->iSize,
		sizeof(Collider*), CollisionManager::SortZ);

	for (int i = 0; i < m_pMouseCollisionList->iSize; ++i)
	{
		Collider*	pDest = m_pMouseCollisionList->pList[i];

		// 충돌 true
		if (true == pMouseRay->Collision(pDest))
		{
			// 피킹용 제거
			if (nullptr != m_pPrevMouseCollider && pDest != m_pPrevMouseCollider)
			{
				pMouseRay->DeletePrevSerialNumber(m_pPrevMouseCollider->GetSerialNumber());
				m_pPrevMouseCollider->DeletePrevSerialNumber(pMouseRay->GetSerialNumber());

				DeletePrevCollider(pMouseRay->GetSerialNumber(),
					m_pPrevMouseCollider->GetSerialNumber());
			}

			// 각 충돌체에 서로 충돌된 충돌체라고 등록해준다.
			// 현재 영역에서 충돌되었다고 등록을 해준다.
			// 이 리스트는 다음 영역에서 충돌체크시에 이전 영역에서
			// 충돌되었다면 빠져나가기 위함이다.
			// 왜냐하면 지금 서로 다른 영역에 걸쳐있을때 앞에 체크한
			// 영역에서 만약 충돌이 되었다면 이 충돌체는 계속
			// 충돌상태라고 표현이 되어버리므로 이전 영역에서
			// 충돌이 되었는지를 판단해야 한다.
			pMouseRay->AddCollisionList(pDest->GetSerialNumber());
			pDest->AddCollisionList(pMouseRay->GetSerialNumber());

			// 이전에 충돌되었는지를 판단한다.
			// 처음 충돌될 경우
			if (!CheckPrevCollider(pMouseRay->GetSerialNumber(),
				pDest->GetSerialNumber()))
			{
				// 충돌 매트릭스에 이전충돌목록으로
				// 등록해준다.
				AddPrevCollider(pMouseRay->GetSerialNumber(),
					pDest->GetSerialNumber());

				// 각 충돌체에 이전 충돌목록으로 등록한다.
				pMouseRay->AddPrevSerialNumber(pDest->GetSerialNumber());
				pDest->AddPrevSerialNumber(pMouseRay->GetSerialNumber());

				// 처음 충돌되었으므로 처음 충돌되었을때 호출할
				// 콜백을 처리한다.
				pMouseRay->Call(CCBS_ENTER, pDest, _fTime);
				pDest->Call(CCBS_ENTER, pMouseRay, _fTime);
			}

			// 이전 충돌목록에 있을 경우 계속 충돌상태로
			// 처리한다.
			else
			{
				// 콜백을 처리한다.
				pMouseRay->Call(CCBS_STAY, pDest, _fTime);
				pDest->Call(CCBS_STAY, pMouseRay, _fTime);
			}

			m_pPrevMouseCollider = pDest;
			m_pMouseCollisionList->Clear();
			SAFE_RELEASE(pMouseRay);
			return true;
		}

		// 충돌이 안된 상태일 경우
		else
		{
			// 이전 충돌목록에 서로 존재할 경우 충돌 되다가
			// 떨어진다는 것이다.
			if (CheckPrevCollider(pMouseRay->GetSerialNumber(),
				pDest->GetSerialNumber()))
			{
				// 이전 충돌목록에서 제거해준다.
				DeletePrevCollider(pMouseRay->GetSerialNumber(),
					pDest->GetSerialNumber());

				pMouseRay->DeletePrevSerialNumber(pDest->GetSerialNumber());
				pDest->DeletePrevSerialNumber(pMouseRay->GetSerialNumber());

				// 콜백을 처리한다.
				pMouseRay->Call(CCBS_EXIT, pDest, _fTime);
				pDest->Call(CCBS_EXIT, pMouseRay, _fTime);
			}
		}
	}

	if (nullptr != m_pPrevMouseCollider)
	{
		pMouseRay->DeletePrevSerialNumber(m_pPrevMouseCollider->GetSerialNumber());
		m_pPrevMouseCollider->DeletePrevSerialNumber(pMouseRay->GetSerialNumber());

		DeletePrevCollider(pMouseRay->GetSerialNumber(),
			m_pPrevMouseCollider->GetSerialNumber());

		m_pPrevMouseCollider = nullptr;
	}

	m_pMouseCollisionList->Clear();

	SAFE_RELEASE(pMouseRay);

	return false;
}

bool CollisionManager::CollisionWorld(float _fTime)
{
	for (unsigned int z = 0; z < (unsigned int)(m_pSection->iNumZ); ++z)
	{
		for (unsigned int y = 0; y < (unsigned int)(m_pSection->iNumY); ++y)
		{
			for (unsigned int x = 0; x < (unsigned int)(m_pSection->iNumX); ++x)
			{
				unsigned int	idx = z * (m_pSection->iNumX * m_pSection->iNumY) +
					y * m_pSection->iNumX + x;

				PColliderList	pSectionColList = &m_pSection->pSectionList[idx];			// 해당 공간의 콜라이더 리스트들을 갖고 온다.

				if (pSectionColList->iSize <= 1)		// 충돌체 한개까지는 계산할필요없음 (두개 이상이여야만됨)
				{
					for (unsigned int i = 0; i < (unsigned int)(pSectionColList->iSize); ++i)
					{
						if (false != pSectionColList->pList[i]->CheckCollisionList())
						{
							pSectionColList->pList[i]->ClearState();		// 상태들을 지워준다.
						}
					}

					pSectionColList->iSize = 0;
					continue;
				}

				// 버블 정렬을 이용 그래서 1빼줌
				for (unsigned int i = 0; i < (unsigned int)(pSectionColList->iSize - 1); ++i)
				{
					Collider*	pSrc = pSectionColList->pList[i];
					for (unsigned int j = i + 1; j < (unsigned int)(pSectionColList->iSize); ++j)
					{
						Collider*	pDest = pSectionColList->pList[j];

						// 현재 다른 섹션에서 충돌되었는지를 판단한다.
						if (pSrc->CheckCollisionList(pDest->GetSerialNumber()))
							continue;

						// 두 충돌체가 사용하는 Profile을 얻어온다.
						PCollisionProfile	pSrcProfile = pSrc->GetCollisionProfile();
						PCollisionProfile	pDestProfile = pDest->GetCollisionProfile();

						PCollisionChannel	pSrcChannel = &pSrcProfile->pChannelList[pDest->GetCollisionChannelIndex()];
						PCollisionChannel	pDestChannel = &pDestProfile->pChannelList[pSrc->GetCollisionChannelIndex()];

						// 둘다 채널이 ignore가 아닌경우 ( 충돌상태인경우) 
						if (pSrcChannel->eState != CCS_IGNORE ||
							pDestChannel->eState != CCS_IGNORE)
						{
							// A, B충돌체가 충돌된 경우 
							if (true == pSrc->Collision(pDest))
							{
								// 각 충돌체에 서로 충돌된 충돌체라고 등록해준다.
								// 현재 영역에서 충돌되었다고 등록을 해준다.
								// 이 리스트는 다음 영역에서 충돌체크시에 이전 영역에서
								// 충돌되었다면 빠져나가기 위함이다.
								// 왜냐하면 지금 서로 다른 영역에 걸쳐있을때 앞에 체크한
								// 영역에서 만약 충돌이 되었다면 이 충돌체는 계속
								// 충돌상태라고 표현이 되어버리므로 이전 영역에서
								// 충돌이 되었는지를 판단해야 한다.

								pSrc->AddCollisionList(pDest->GetSerialNumber());			// 충돌 리스트에게 상대의 고유 식별번호를 넘겨준다.
								pDest->AddCollisionList(pSrc->GetSerialNumber());

								// 이전에도 충돌이 되었는지 판단한다. (Stay인지 Enter인지 확인하려고)

								if (false == CheckPrevCollider(pSrc->GetSerialNumber(), pDest->GetSerialNumber()))
								{
									// 처음 충돌된 경우 (Enter)

									// CollisionManager의 충돌 인접행렬에 이전 충돌 목록으로 등록한다.
									AddPrevCollider(pSrc->GetSerialNumber(), pDest->GetSerialNumber());

									// 각 충돌체에 이전 충돌 목록으로 등록한다. (Prev임, 다음 프레임때 사용 될 것)
									pSrc->AddPrevSerialNumber(pDest->GetSerialNumber());
									pDest->AddPrevSerialNumber(pSrc->GetSerialNumber());

									// 처음 충돌 되었으므로 처음 충돌 되었을 때 호출할 콜백 함수를 불러준다.
									if (CCS_IGNORE != pSrcChannel->eState)
									{
										pSrc->Call(CCBS_ENTER, pDest, _fTime);
									}

									if (CCS_IGNORE != pDestChannel->eState)
									{
										pDest->Call(CCBS_ENTER, pSrc, _fTime);
									}
								}
								else
								{
									// 충돌 중인 경우 (Stay) -> 이전 충돌 목록에 있얼 경우, 계속 충돌한 상태이다.
									if (CCS_IGNORE != pSrcChannel->eState)
									{
										pSrc->Call(CCBS_STAY, pDest, _fTime);
									}

									if (CCS_IGNORE != pDestChannel->eState)
									{
										pDest->Call(CCBS_STAY, pSrc, _fTime);
									}
								}

								//OutputDebugString(TEXT("충돌\n"));
							}

							else
							{
								// 충돌상태가 아닌 경우 ( 아예 충돌이 안된건지, 아님 충돌중에 바깥으로 범위가 벗어난건지 체크해야함 )

								// Exit 상태
								// 이전 충돌 목록에 서로 존재할 경우 충돌 되다가 밖으로 벗어난 것이다. 
								if (true == CheckPrevCollider(pSrc->GetSerialNumber(), pDest->GetSerialNumber()))
								{
									// 이전 충돌목록에서 제거한다. ( 인접행렬에서 제거 )
									DeletePrevCollider(pSrc->GetSerialNumber(), pDest->GetSerialNumber());

									// 각 해당 콜라이더의 Prev에서 제거
									pSrc->DeletePrevSerialNumber(pDest->GetSerialNumber());
									pDest->DeletePrevSerialNumber(pSrc->GetSerialNumber());

									// 콜백 함수를 호출한다. -> Exit
									if (CCS_IGNORE != pSrcChannel->eState)
									{
										pSrc->Call(CCBS_EXIT, pDest, _fTime);
									}

									if (CCS_IGNORE != pDestChannel->eState)
									{
										pDest->Call(CCBS_EXIT, pSrc, _fTime);
									}
								}
								else
								{
									// 이건 아예 충돌이 안된상태이다.
									continue;
								}
							}
						}

						else
						{
							// 여긴 오면 안됨 -_-;;
							if (true == CheckPrevCollider(pSrc->GetSerialNumber(), pDest->GetSerialNumber()))
							{
								// 이전 충돌목록에서 제거한다. ( 인접행렬에서 제거 )
								DeletePrevCollider(pSrc->GetSerialNumber(), pDest->GetSerialNumber());

								// 각 해당 콜라이더의 Prev에서 제거
								pSrc->DeletePrevSerialNumber(pDest->GetSerialNumber());
								pDest->DeletePrevSerialNumber(pSrc->GetSerialNumber());

								// 콜백 함수를 호출한다. -> Exit
								if (CCS_IGNORE != pSrcChannel->eState)
								{
									pSrc->Call(CCBS_EXIT, pDest, _fTime);
								}

								if (CCS_IGNORE != pDestChannel->eState)
								{
									pDest->Call(CCBS_EXIT, pSrc, _fTime);
								}
							}
							else
							{
								// 이건 아예 충돌이 안된상태이다.
								continue;
							}
						}
					}
				}


				// 충돌을 다 해준 후에는 0으로 다시 만들어준다. ( 콜라이더 리스트)
				pSectionColList->iSize = 0;
			}
		}
	}

	return true;
}

bool CollisionManager::Collision(Collider * _pSrc, Collider * _pDest)
{
	return false;
}

// Z소팅
int CollisionManager::SortZ(const void * _pSrc, const void * _pDest)
{
	Collider*	pSrcColl = *((Collider**)_pSrc);
	Collider*	pDestColl = *((Collider**)_pDest);

	Vector3	vSrcMin = pSrcColl->GetSectionMin();
	Vector3	vDestMin = pDestColl->GetSectionMin();
	Vector3	vSrcMax = pSrcColl->GetSectionMax();
	Vector3	vDestMax = pDestColl->GetSectionMax();

	// 중간지점구하구
	Vector3	vSrcCenter = (vSrcMin + vSrcMax) / 2.0f;
	Vector3	vDestCenter = (vDestMin + vDestMax) / 2.0f;

	// 카메라랑 거리비교해서 정렬함
	Vector3 vCameraPos = GET_SINGLETON(SceneManager)->GetMainCameraPos();

	float	fSrcDist = vSrcCenter.Distance(vCameraPos);
	float	fDestDist = vDestCenter.Distance(vCameraPos);
	

	if (fSrcDist > fDestDist)
	{
		return 1;
	}
	else if (fSrcDist < fDestDist)
	{
		return -1;
	}

	return 0;
}
