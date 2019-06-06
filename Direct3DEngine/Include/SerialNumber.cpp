#include "SerialNumber.h"

ENGINE_USING

SerialNumber::SerialNumber()
{
	m_iSize = 0;				// ����
	m_iNumber = 1;				// �ĺ���ȣ
	m_iCapacity = 100;			// ���� ũ��
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

// �ĺ� ��ȣ �ο�
unsigned int SerialNumber::GetSerialNumber()
{
	// ������ �ĺ� ��ȣ�� �ϳ��� ���� ���
	if (0 == m_iSize)
	{
		unsigned int iNumber = m_iNumber + m_iMin;
		++m_iNumber;
		return iNumber;
	}

	// ������ �ĺ� ��ȣ�� �ϳ��� ������ ��� ( ������ �ĺ���ȣ�� �켱������ �־��ش�. )
	--m_iSize;
	return m_pValidNumber[m_iSize];
}

// ������ �ĺ���ȣ �߰��ϱ�
void SerialNumber::AddValidNumber(unsigned int _iNumber)
{
	m_pValidNumber[m_iSize] = _iNumber;
	++m_iSize;
}
