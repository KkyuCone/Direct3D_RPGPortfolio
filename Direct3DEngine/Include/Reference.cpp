#include "Reference.h"

ENGINE_BEGIN

Reference::Reference()
	: m_iReferenceCount(1)
	, m_bActive(true)
	, m_bEnable(true)
	, m_iSerialNumber(UINT_MAX)
{
}


Reference::~Reference()
{
	int a = 0;
}

void Reference::SetTag(const std::string& _pTagName)
{
	m_strTag = _pTagName;
}

std::string Reference::GetTag() const
{
	return m_strTag;
}

bool Reference::IsActive() const
{
	return m_bActive;
}

bool Reference::IsEnable() const
{
	return m_bEnable;
}

void Reference::Active(bool _bActive)
{
	m_bActive = _bActive;
}

void Reference::Enable(bool _bEnable)
{
	m_bEnable = _bEnable;
}

unsigned int Reference::GetSerialNumber() const
{
	return m_iSerialNumber;
}

void Reference::SetSerialNumber(unsigned int _iNumber)
{
	m_iSerialNumber = _iNumber;
}

void Reference::Release()
{
	--m_iReferenceCount;

	if (0 == m_iReferenceCount)
	{
		delete this;
	}
}

void Reference::AddReference()
{
	++m_iReferenceCount;
}

ENGINE_END