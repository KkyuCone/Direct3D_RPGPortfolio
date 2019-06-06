#pragma once
#include "Engine.h"

ENGINE_BEGIN

class ENGINE_DLL Reference
{
protected:
	Reference();
	virtual ~Reference();

protected:
	int			m_iReferenceCount;
	std::string m_strTag;
	bool		m_bActive;				// 해당 객체가 살아있는지 여부
	bool		m_bEnable;				// 해당 객체가 활성화 되어있는지 여부
	unsigned int m_iSerialNumber;		// 식별 번호

public:
	unsigned int GetSerialNumber() const;
	void SetSerialNumber(unsigned int _iNumber);

public:
	void Release();
	void AddReference();

	void SetTag(const std::string& _pTagName);
	std::string GetTag() const;

	bool IsActive()	const;
	bool IsEnable() const;

	void Active(bool _bActive);
	void Enable(bool _bEnable);
};

ENGINE_END