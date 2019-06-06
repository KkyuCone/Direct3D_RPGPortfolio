#pragma once
#include "Engine.h"

ENGINE_BEGIN

class ENGINE_DLL SerialNumber
{
public:
	SerialNumber();
	~SerialNumber();

private:
	unsigned int	m_iNumber;			// 얘가 식별번호
	unsigned int*	m_pValidNumber;		// 삭제된 식별번호 모음
	unsigned int	m_iSize;			// 삭제된 식별번호 개수
	unsigned int	m_iCapacity;		// 공간
	unsigned int	m_iMin;				// 인스턴싱 최소값
	unsigned int	m_iMax;				// 인스턴싱 최대값..?

public:
	void			SetMinMax(unsigned int _iMin, unsigned int _iMax);
	unsigned int	GetSerialNumber();
	void			AddValidNumber(unsigned int _iNumber);
};

ENGINE_END
