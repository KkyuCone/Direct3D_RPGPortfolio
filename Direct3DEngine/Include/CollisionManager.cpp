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
	m_pSection = nullptr;				// �Ϲ� ���� �浹 ����
	m_pPickSection = nullptr;			// ��ŷ�� ����
	m_pUISection = nullptr;				// UI�浹 ����

	m_pPrevMousePick = nullptr;
	m_pPrevMouseCollider = nullptr;

	m_pMouseCollisionList = new ColliderList;			// ���콺 �浹 ����Ʈ

	m_iSerialNumber = 0;

	m_iProfileSize = 0;
	m_iProfileCapacity = 16;				// �̸� ũ�⸦ 16�� ���ϰ� ���߿� �ʰ��� �� �߰��ϸ� �ȴ�.
	m_pProfileList = new CollisionProfile[m_iProfileCapacity];

	m_iChannelSize = 0;
	m_pChannelList = new CollisionChannel[MAX_COLLISION_CHANNEL];			// ä�� ������ �ϴ� �ִ� 32������ ���µ� �� ���� ��� ȿ���� ������ ������

	m_iValidSize = 0;
	m_iValidCapacity = 100;

	m_pValidNumber = new unsigned int[m_iValidCapacity];					// �굵 �̸� �����α⸸ �ϴ°� ������ ���߿� �߰��� �� ����
	memset(m_pValidNumber, 0, sizeof(unsigned int) * m_iValidCapacity);		// 0���� �ʱ�ȭ ���ϸ� �����Ⱚ ��
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

// ���� �ĺ� ��ȣ �ο� �� Collider�� Start()�Լ����� ȣ��ȴ�.
unsigned int CollisionManager::GetSerialNumber()
{
	// ������ �ĺ���ȣ�� ���ٸ� �߰��ؼ� �־��ֱ�
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

	// ������ �ĺ���ȣ���� �ִٸ�
	// �� ������ �ĺ���ȣ�� �ο��Ѵ�. (�ڿ������ͤ�...)
	--m_iValidSize;

	return m_pValidNumber[m_iValidSize];
}

// �����ĺ���ȣ�� �����Ҷ� ����Ҳ�.. �� �ݶ��̴� �����ҋ�?? ����Ұ���
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

	m_tColliderInfo.pColliderList[_iNumber] = nullptr;		// ���������ϱ� ������ ��ġ�� nullptr�� �ִ´�.
	--m_tColliderInfo.iSize;								// �ݶ��̴� ������ �ٿ��ְ� ������
}

void CollisionManager::AddPrevCollider(unsigned int _iSrc, unsigned int _iDest)
{
	// _iSrc, _iDest : ���� �ĺ� ��ȣ�� Serial Number
	unsigned int	iSrcIdx = _iSrc / 32;				// ���� int�� ��Ʈ������ �ɰ��� ���.. -> �׷��� 32�� ������ �ڱ� �ε����� ��
	unsigned int	iDestIdx = _iDest / 32;				// �굵 ������

	unsigned int	iSrcBit = 31 - (_iSrc % 32);		// �ش��ε����� 32��Ʈ�� �̷��������ϱ� �ڱⰡ ��� ��, ���° ��Ʈ�� ���ؾ��ϴ���.. (�굵 �ε����� ������..)
	unsigned int	iDestBit = 31 - (_iDest % 32);

	unsigned int	iSrcValue = 1 << iSrcBit;			// ����Ʈ �����ڷ� �´� ��ġ�� �־��ش�!! 0�� 1�� ����Ұ��� ������ 0�� �浹 X 1�� �浹
	unsigned int	iDestValue = 1 << iDestBit;

	// ������Ŀ� ���� �־��ش�. (���簪�� �߰��ؾߵ� ���� ���ؼ� �ִµ� OR������ )
	// [���� = 4096 ][���� = 128 * 32(4byte) = 4096]
	// |�������� �׻� �߰��ǰ��Ѵ�.  = 1
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
	// ������ ����
	if (nullptr != m_pSection)
	{
		SAFE_DELETE(m_pSection);
	}

	m_pSection = new CollisionSection;

	m_pSection->iNumX = _iNumX;
	m_pSection->iNumY = _iNumY;
	m_pSection->iNumZ = _iNumZ;
	m_pSection->vCellSize = _vCellSize;			// ���ҵ� ���� �Ѱ��� ������

	m_pSection->pSectionList = new ColliderList[_iNumX * _iNumY * _iNumZ];
	m_pSection->vLength = m_pSection->vCellSize * Vector3(_iNumX, _iNumY, _iNumZ);			// ������ �� ũ��

	m_vPos = _vPos;					// ������ ������
	m_vPivot = _vPivot;				// ������������ �Ǻ� percent�� (  )  0~1.0f�����ǰ�

	m_pSection->vMin = m_vPos - m_vPivot * m_pSection->vLength;						// ������ �ּڰ�( �������� ���������� �ּڰ���)
	m_pSection->vMax = m_vPos + (Vector3(1.0f, 1.0f, 1.0f) - m_vPivot) * m_pSection->vLength;		// ������ �ִ�

	// Edit��尡 Ȱ��ȭ �Ǿ��������� ��ŷ�� ������ �����.
	if (true == GET_SINGLETON(Core)->GetEditMode())
	{
		SAFE_DELETE(m_pPickSection);			// ���� ���� ����(������..)

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
	// ���� ä���� �ִ� ������ 32���� �����Ǿ� �ִ�. �� �̻��� ��������ϸ� return
	if (m_iChannelSize == MAX_COLLISION_CHANNEL)
		return false;

	// ���� �̸��� �������� return ��Ų��.
	if (true == CheckChannel(_strName))
	{
		return false;
	}

	m_pChannelList[m_iChannelSize].strName = _strName;
	m_pChannelList[m_iChannelSize].eState = _eState;

	// ���� �����Ǿ� �ִ� ��� Profile�� �߰��� ä�� ������ �߰��Ѵ�.
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
				// ä���� ���� ���� �Ŀ� 
				// ���� ���� �۾�.. �� ������ �̵���Ű�� �۾��� ���ش�. ( ����ó�� )
				// ��� ���ִ� �� ��ȿ�����̱� ������ ������ �������� ���ִ� ���̴�.
				m_pChannelList[j] = m_pChannelList[j + 1];
			}

			for (unsigned int j = 0; j < m_iProfileSize; ++j)
			{
				// ä�θ���Ʈ���� �ٿ������ϱ�
				// ���������� ä�ε� �Ȱ��� �������ش�.(�׳� ���Ͱ��� ���� �۾���)
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
	// �̸� �Ҵ��ص� �޸� ���� �� �־�ѷ���
	// �޸𸮸� �� �÷��ش�. �׸����� �߰���(������ ä���..)
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

	const std::list<Collider*>*	pColliderList = _pObject->GetColliderList();				// �ݶ��̴��� �߰��� ������Ʈ�� �ݶ��̴� ����Ʈ���� �����´�. �� ���ӿ�����Ʈ�� �������� �ݶ��̴��� ���� �� �ִ�.

	std::list<Collider*>::const_iterator StartIter = pColliderList->begin();
	std::list<Collider*>::const_iterator EndIter = pColliderList->end();

	if (true == pColliderList->empty())
	{
		return;
	}

	else if (false == _pObject->IsEnable())
	{
		// ���� ������Ʈ�� Ȱ��ȭ�� ���°��ƴ϶�� ���� �� ������Ʈ�� ���õ� �ݶ��̴��� �浹�� 
		// �ٸ� �ݶ��̴����� ���µ��� ���� �����ش�. ( �ݶ��̴� ��Ȱ��ȭ���´ϱ� )

		for (; StartIter != EndIter; ++StartIter)
		{
			(*StartIter)->ClearState();
		}

		return;
	}


	for (StartIter = pColliderList->begin(); StartIter != EndIter; ++StartIter)
	{
		Collider*	pCollider = *StartIter;

		// �ش� �ݶ��̴��� ��Ȱ��ȭ ���¶�� ���¸� �����ش�.
		if (false == pCollider->IsEnable())
		{
			pCollider->ClearState();
			continue;
		}

		if (true == pCollider->GetPickEnable())
		{
			// ��ŷ�� �ݶ��̴��� �Ϲ� �浹ü ������ ���� �ʴ´�.
			// ��ŷ�� �ݶ��̴� ������ ���� ��������.
			pCollider->ClearState();
			continue;
		}

		// �浹ü�� Min, Max ���� ���´�.
		Vector3	vCollMin = pCollider->GetSectionMin();
		Vector3	vCollMax = pCollider->GetSectionMax();

		// ��ü ������ Min���� �����ؼ� 0, 0, 0���� ������ش�.
		vCollMin -= m_pSection->vMin;
		vCollMax -= m_pSection->vMin;

		// 1, 1, 1 �������� ��ȯ�Ѵ�.  -> �ڽ��� ������ ����� ������ ��ġ�ϴ���..
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

					pSection->pList[pSection->iSize] = pCollider;			// �ݶ��̴� �߰�
					++pSection->iSize;
				}
			}
		}
	}
}

void CollisionManager::AddCollider(Collider * _pCollider)
{
	// �ݶ��̴����� ��Ƶδ� ����Ʈ�� �־�д�. ( �����ĺ���ȣ�� �Ǻ� )
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
	// �浹ü�� �������
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

		// �浹ü�� Min, Max ���� ���´�.
		Vector3	vCollMin = pCollider->GetSectionMin();
		Vector3	vCollMax = pCollider->GetSectionMax();

		// ��ü ������ Min���� �����ؼ� 0, 0, 0���� ������ش�.
		vCollMin -= m_pSection->vMin;
		vCollMax -= m_pSection->vMin;

		// 1, 1, 1 �������� ��ȯ�Ѵ�.
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

		// �浹�� ������ �����..(���ѵ� ���� ����� �浹 ó�� X)
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

					// ��ŷ���̰� �����͸�.. ��ŷ�� �������� �ٲ۴�.
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
	// ��������
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
	// �浹 ���� ������ �и����ش�. -> �浹ü���� ��� ������ �����ϴ��� �־���(�Ź� ����)
	ComputeSection();

	bool bCollision = CollisionMouseUI(_fTime);

	// UI���ƴҶ� ������ ����
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
	// �������ֱ�
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

// ���콺 - UI �浹
bool CollisionManager::CollisionMouseUI(float _fTime)
{
	return false;
}

// ���콺 - ���������Ʈ �浹
bool CollisionManager::CollisionMouseWorld(float _fTime)
{
	// ���콺 ������ ���´�.
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

		// �߰��� �浹ü���� �Ÿ� ������ �������� �����Ѵ�.
		qsort(m_pMouseCollisionList->pList, m_pMouseCollisionList->iSize,
			sizeof(Collider*), CollisionManager::SortZ);

		for (int i = 0; i < m_pMouseCollisionList->iSize; ++i)
		{
			Collider*	pDest = m_pMouseCollisionList->pList[i];

			if (true == pMouseRay->Collision(pDest))
			{

				// ������ ���콺�� �浹�� ��ŷ�� �ݶ��̴��� �����ϰ�, 
				// ���� ���콺 �浹 ����Ʈ�� �����°��߿� ��ŷ�� ������ �ƴϸ� �����ش�.
				// �������ָ� ��� �浹�� ä�� ���� �� �ֱ⶧���� �浹���������� �浹�ߴٰ� �Ѱ� �����ش�.
				if (nullptr != m_pPrevMousePick && pDest != m_pPrevMousePick)
				{
					pMouseRay->DeletePrevSerialNumber(m_pPrevMousePick->GetSerialNumber());
					m_pPrevMousePick->DeletePrevSerialNumber(pMouseRay->GetSerialNumber());

					DeletePrevCollider(pMouseRay->GetSerialNumber(),
						m_pPrevMousePick->GetSerialNumber());
				}

				// �� �浹ü�� ���� �浹�� �浹ü��� ������ش�.
				// ���� �������� �浹�Ǿ��ٰ� ����� ���ش�.
				// �� ����Ʈ�� ���� �������� �浹üũ�ÿ� ���� ��������
				// �浹�Ǿ��ٸ� ���������� �����̴�.
				// �ֳ��ϸ� ���� ���� �ٸ� ������ ���������� �տ� üũ��
				// �������� ���� �浹�� �Ǿ��ٸ� �� �浹ü�� ���
				// �浹���¶�� ǥ���� �Ǿ�����Ƿ� ���� ��������
				// �浹�� �Ǿ������� �Ǵ��ؾ� �Ѵ�.
				pMouseRay->AddCollisionList(pDest->GetSerialNumber());
				pDest->AddCollisionList(pMouseRay->GetSerialNumber());

				// ������ �浹�Ǿ������� �Ǵ��Ѵ�.
				// ó�� �浹�� ���
				if (!CheckPrevCollider(pMouseRay->GetSerialNumber(),
					pDest->GetSerialNumber()))
				{
					// �浹 ��Ʈ������ �����浹�������
					// ������ش�.
					AddPrevCollider(pMouseRay->GetSerialNumber(),
						pDest->GetSerialNumber());

					// �� �浹ü�� ���� �浹������� ����Ѵ�.
					pMouseRay->AddPrevSerialNumber(pDest->GetSerialNumber());
					pDest->AddPrevSerialNumber(pMouseRay->GetSerialNumber());

					// ó�� �浹�Ǿ����Ƿ� ó�� �浹�Ǿ����� ȣ����
					// �ݹ��� ó���Ѵ�.
					pMouseRay->Call(CCBS_ENTER, pDest, _fTime);
					pDest->Call(CCBS_ENTER, pMouseRay, _fTime);
				}

				// ���� �浹��Ͽ� ���� ��� ��� �浹���·�
				// ó���Ѵ�.
				else
				{
					// �ݹ��� ó���Ѵ�.
					pMouseRay->Call(CCBS_STAY, pDest, _fTime);
					pDest->Call(CCBS_STAY, pMouseRay, _fTime);
				}

				bEditPick = true;
				m_pPrevMousePick = pDest;
				break;
			}

			// �浹�� �ȵ� ������ ���
			else
			{
				// ���� �浹��Ͽ� ���� ������ ��� �浹 �Ǵٰ�
				// �������ٴ� ���̴�.
				if (CheckPrevCollider(pMouseRay->GetSerialNumber(),
					pDest->GetSerialNumber()))
				{
					// ���� �浹��Ͽ��� �������ش�.
					DeletePrevCollider(pMouseRay->GetSerialNumber(),
						pDest->GetSerialNumber());

					pMouseRay->DeletePrevSerialNumber(pDest->GetSerialNumber());
					pDest->DeletePrevSerialNumber(pMouseRay->GetSerialNumber());

					// �ݹ��� ó���Ѵ�.
					pMouseRay->Call(CCBS_EXIT, pDest, _fTime);
					pDest->Call(CCBS_EXIT, pMouseRay, _fTime);
				}
			}
		}

	}

	// ������ ��尡 �ƴϸ� ��ŷ�� �ʿ������ �����ش�.
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


	// �Ʒ��� ��ŷ���� �ƴѵ�
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

	// �߰��� �浹ü���� �Ÿ� ������ �������� �����Ѵ�.
	qsort(m_pMouseCollisionList->pList, m_pMouseCollisionList->iSize,
		sizeof(Collider*), CollisionManager::SortZ);

	for (int i = 0; i < m_pMouseCollisionList->iSize; ++i)
	{
		Collider*	pDest = m_pMouseCollisionList->pList[i];

		// �浹 true
		if (true == pMouseRay->Collision(pDest))
		{
			// ��ŷ�� ����
			if (nullptr != m_pPrevMouseCollider && pDest != m_pPrevMouseCollider)
			{
				pMouseRay->DeletePrevSerialNumber(m_pPrevMouseCollider->GetSerialNumber());
				m_pPrevMouseCollider->DeletePrevSerialNumber(pMouseRay->GetSerialNumber());

				DeletePrevCollider(pMouseRay->GetSerialNumber(),
					m_pPrevMouseCollider->GetSerialNumber());
			}

			// �� �浹ü�� ���� �浹�� �浹ü��� ������ش�.
			// ���� �������� �浹�Ǿ��ٰ� ����� ���ش�.
			// �� ����Ʈ�� ���� �������� �浹üũ�ÿ� ���� ��������
			// �浹�Ǿ��ٸ� ���������� �����̴�.
			// �ֳ��ϸ� ���� ���� �ٸ� ������ ���������� �տ� üũ��
			// �������� ���� �浹�� �Ǿ��ٸ� �� �浹ü�� ���
			// �浹���¶�� ǥ���� �Ǿ�����Ƿ� ���� ��������
			// �浹�� �Ǿ������� �Ǵ��ؾ� �Ѵ�.
			pMouseRay->AddCollisionList(pDest->GetSerialNumber());
			pDest->AddCollisionList(pMouseRay->GetSerialNumber());

			// ������ �浹�Ǿ������� �Ǵ��Ѵ�.
			// ó�� �浹�� ���
			if (!CheckPrevCollider(pMouseRay->GetSerialNumber(),
				pDest->GetSerialNumber()))
			{
				// �浹 ��Ʈ������ �����浹�������
				// ������ش�.
				AddPrevCollider(pMouseRay->GetSerialNumber(),
					pDest->GetSerialNumber());

				// �� �浹ü�� ���� �浹������� ����Ѵ�.
				pMouseRay->AddPrevSerialNumber(pDest->GetSerialNumber());
				pDest->AddPrevSerialNumber(pMouseRay->GetSerialNumber());

				// ó�� �浹�Ǿ����Ƿ� ó�� �浹�Ǿ����� ȣ����
				// �ݹ��� ó���Ѵ�.
				pMouseRay->Call(CCBS_ENTER, pDest, _fTime);
				pDest->Call(CCBS_ENTER, pMouseRay, _fTime);
			}

			// ���� �浹��Ͽ� ���� ��� ��� �浹���·�
			// ó���Ѵ�.
			else
			{
				// �ݹ��� ó���Ѵ�.
				pMouseRay->Call(CCBS_STAY, pDest, _fTime);
				pDest->Call(CCBS_STAY, pMouseRay, _fTime);
			}

			m_pPrevMouseCollider = pDest;
			m_pMouseCollisionList->Clear();
			SAFE_RELEASE(pMouseRay);
			return true;
		}

		// �浹�� �ȵ� ������ ���
		else
		{
			// ���� �浹��Ͽ� ���� ������ ��� �浹 �Ǵٰ�
			// �������ٴ� ���̴�.
			if (CheckPrevCollider(pMouseRay->GetSerialNumber(),
				pDest->GetSerialNumber()))
			{
				// ���� �浹��Ͽ��� �������ش�.
				DeletePrevCollider(pMouseRay->GetSerialNumber(),
					pDest->GetSerialNumber());

				pMouseRay->DeletePrevSerialNumber(pDest->GetSerialNumber());
				pDest->DeletePrevSerialNumber(pMouseRay->GetSerialNumber());

				// �ݹ��� ó���Ѵ�.
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

				PColliderList	pSectionColList = &m_pSection->pSectionList[idx];			// �ش� ������ �ݶ��̴� ����Ʈ���� ���� �´�.

				if (pSectionColList->iSize <= 1)		// �浹ü �Ѱ������� ������ʿ���� (�ΰ� �̻��̿��߸���)
				{
					for (unsigned int i = 0; i < (unsigned int)(pSectionColList->iSize); ++i)
					{
						if (false != pSectionColList->pList[i]->CheckCollisionList())
						{
							pSectionColList->pList[i]->ClearState();		// ���µ��� �����ش�.
						}
					}

					pSectionColList->iSize = 0;
					continue;
				}

				// ���� ������ �̿� �׷��� 1����
				for (unsigned int i = 0; i < (unsigned int)(pSectionColList->iSize - 1); ++i)
				{
					Collider*	pSrc = pSectionColList->pList[i];
					for (unsigned int j = i + 1; j < (unsigned int)(pSectionColList->iSize); ++j)
					{
						Collider*	pDest = pSectionColList->pList[j];

						// ���� �ٸ� ���ǿ��� �浹�Ǿ������� �Ǵ��Ѵ�.
						if (pSrc->CheckCollisionList(pDest->GetSerialNumber()))
							continue;

						// �� �浹ü�� ����ϴ� Profile�� ���´�.
						PCollisionProfile	pSrcProfile = pSrc->GetCollisionProfile();
						PCollisionProfile	pDestProfile = pDest->GetCollisionProfile();

						PCollisionChannel	pSrcChannel = &pSrcProfile->pChannelList[pDest->GetCollisionChannelIndex()];
						PCollisionChannel	pDestChannel = &pDestProfile->pChannelList[pSrc->GetCollisionChannelIndex()];

						// �Ѵ� ä���� ignore�� �ƴѰ�� ( �浹�����ΰ��) 
						if (pSrcChannel->eState != CCS_IGNORE ||
							pDestChannel->eState != CCS_IGNORE)
						{
							// A, B�浹ü�� �浹�� ��� 
							if (true == pSrc->Collision(pDest))
							{
								// �� �浹ü�� ���� �浹�� �浹ü��� ������ش�.
								// ���� �������� �浹�Ǿ��ٰ� ����� ���ش�.
								// �� ����Ʈ�� ���� �������� �浹üũ�ÿ� ���� ��������
								// �浹�Ǿ��ٸ� ���������� �����̴�.
								// �ֳ��ϸ� ���� ���� �ٸ� ������ ���������� �տ� üũ��
								// �������� ���� �浹�� �Ǿ��ٸ� �� �浹ü�� ���
								// �浹���¶�� ǥ���� �Ǿ�����Ƿ� ���� ��������
								// �浹�� �Ǿ������� �Ǵ��ؾ� �Ѵ�.

								pSrc->AddCollisionList(pDest->GetSerialNumber());			// �浹 ����Ʈ���� ����� ���� �ĺ���ȣ�� �Ѱ��ش�.
								pDest->AddCollisionList(pSrc->GetSerialNumber());

								// �������� �浹�� �Ǿ����� �Ǵ��Ѵ�. (Stay���� Enter���� Ȯ���Ϸ���)

								if (false == CheckPrevCollider(pSrc->GetSerialNumber(), pDest->GetSerialNumber()))
								{
									// ó�� �浹�� ��� (Enter)

									// CollisionManager�� �浹 ������Ŀ� ���� �浹 ������� ����Ѵ�.
									AddPrevCollider(pSrc->GetSerialNumber(), pDest->GetSerialNumber());

									// �� �浹ü�� ���� �浹 ������� ����Ѵ�. (Prev��, ���� �����Ӷ� ��� �� ��)
									pSrc->AddPrevSerialNumber(pDest->GetSerialNumber());
									pDest->AddPrevSerialNumber(pSrc->GetSerialNumber());

									// ó�� �浹 �Ǿ����Ƿ� ó�� �浹 �Ǿ��� �� ȣ���� �ݹ� �Լ��� �ҷ��ش�.
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
									// �浹 ���� ��� (Stay) -> ���� �浹 ��Ͽ� �־� ���, ��� �浹�� �����̴�.
									if (CCS_IGNORE != pSrcChannel->eState)
									{
										pSrc->Call(CCBS_STAY, pDest, _fTime);
									}

									if (CCS_IGNORE != pDestChannel->eState)
									{
										pDest->Call(CCBS_STAY, pSrc, _fTime);
									}
								}

								//OutputDebugString(TEXT("�浹\n"));
							}

							else
							{
								// �浹���°� �ƴ� ��� ( �ƿ� �浹�� �ȵȰ���, �ƴ� �浹�߿� �ٱ����� ������ ������� üũ�ؾ��� )

								// Exit ����
								// ���� �浹 ��Ͽ� ���� ������ ��� �浹 �Ǵٰ� ������ ��� ���̴�. 
								if (true == CheckPrevCollider(pSrc->GetSerialNumber(), pDest->GetSerialNumber()))
								{
									// ���� �浹��Ͽ��� �����Ѵ�. ( ������Ŀ��� ���� )
									DeletePrevCollider(pSrc->GetSerialNumber(), pDest->GetSerialNumber());

									// �� �ش� �ݶ��̴��� Prev���� ����
									pSrc->DeletePrevSerialNumber(pDest->GetSerialNumber());
									pDest->DeletePrevSerialNumber(pSrc->GetSerialNumber());

									// �ݹ� �Լ��� ȣ���Ѵ�. -> Exit
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
									// �̰� �ƿ� �浹�� �ȵȻ����̴�.
									continue;
								}
							}
						}

						else
						{
							// ���� ���� �ȵ� -_-;;
							if (true == CheckPrevCollider(pSrc->GetSerialNumber(), pDest->GetSerialNumber()))
							{
								// ���� �浹��Ͽ��� �����Ѵ�. ( ������Ŀ��� ���� )
								DeletePrevCollider(pSrc->GetSerialNumber(), pDest->GetSerialNumber());

								// �� �ش� �ݶ��̴��� Prev���� ����
								pSrc->DeletePrevSerialNumber(pDest->GetSerialNumber());
								pDest->DeletePrevSerialNumber(pSrc->GetSerialNumber());

								// �ݹ� �Լ��� ȣ���Ѵ�. -> Exit
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
								// �̰� �ƿ� �浹�� �ȵȻ����̴�.
								continue;
							}
						}
					}
				}


				// �浹�� �� ���� �Ŀ��� 0���� �ٽ� ������ش�. ( �ݶ��̴� ����Ʈ)
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

// Z����
int CollisionManager::SortZ(const void * _pSrc, const void * _pDest)
{
	Collider*	pSrcColl = *((Collider**)_pSrc);
	Collider*	pDestColl = *((Collider**)_pDest);

	Vector3	vSrcMin = pSrcColl->GetSectionMin();
	Vector3	vDestMin = pDestColl->GetSectionMin();
	Vector3	vSrcMax = pSrcColl->GetSectionMax();
	Vector3	vDestMax = pDestColl->GetSectionMax();

	// �߰��������ϱ�
	Vector3	vSrcCenter = (vSrcMin + vSrcMax) / 2.0f;
	Vector3	vDestCenter = (vDestMin + vDestMax) / 2.0f;

	// ī�޶�� �Ÿ����ؼ� ������
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
