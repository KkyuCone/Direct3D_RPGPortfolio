#include "SerialNumber.h"

ENGINE_USING

SerialNumber::SerialNumber()
{
	m_iSize = 0;				// 개수
	m_iNumber = 1;				// 식별번호
	m_iCapacity = 100;			// 공간 크기
	m_iMin = 0;					
	m_iMax = 0;

	m_pValidNumber = new unsigned int[m_iCapacity];
}


SerialNumber::~SerialNumber()
{
	SAFE_DELETE_ARRAY(m_pValidNumber);
}

void SerialNumber::SetMinMax(unsigned int _iMin, unsigned int _iMax)
{
	m_iMin = _iMin;
	m_iMax = _iMax;
}

// 식별 번호 부여
unsigned int SerialNumber::GetSerialNumber()
{
	// 삭제된 식별 번호가 하나도 없을 경우
	if (0 == m_iSize)
	{
		unsigned int iNumber = m_iNumber + m_iMin;
		++m_iNumber;
		return iNumber;
	}

	// 삭제된 식별 번호가 하나라도 존재할 경우 ( 삭제된 식별번호를 우선적으로 넣어준다. )
	--m_iSize;
	return m_pValidNumber[m_iSize];
}

// 삭제된 식별번호 추가하기
void SerialNumber::AddValidNumber(unsigned int _iNumber)
{
	m_pValidNumber[m_iSize] = _iNumber;
	++m_iSize;
}
