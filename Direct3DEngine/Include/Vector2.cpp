#include "Vector2.h"

ENGINE_USING

_tagVector2	_tagVector2::Zero = Vector2(0.f, 0.f);
_tagVector2	_tagVector2::One = Vector2(1.f, 1.f);

_tagVector2::_tagVector2()
	:x(0.f),
	y(0.f)
{
}

_tagVector2::_tagVector2(const _tagVector2 & _v)
{
	x = _v.x;
	y = _v.y;
}

_tagVector2::_tagVector2(const XMVECTOR & _v)
{
	XMStoreFloat2((XMFLOAT2*)this, _v);
}

_tagVector2::_tagVector2(const XMFLOAT2 & _v)
{
	x = _v.x;
	y = _v.y;
}

_tagVector2::_tagVector2(int _ix, int _iy)
{
	x = (float)_ix;
	y = (float)_iy;
}

_tagVector2::_tagVector2(float _fx, float _fy)
{
	x = _fx;
	y = _fy;
}

_tagVector2::_tagVector2(float _fValue)
{
	x = _fValue;
	y = _fValue;
}

_tagVector2::_tagVector2(int _iValue)
{
	x = (float)_iValue;
	y = (float)_iValue;
}

_tagVector2::_tagVector2(float _fArrValue[2])
{
	x = _fArrValue[0];
	y = _fArrValue[1];
}

_tagVector2::_tagVector2(int _iArrValue[2])
{
	x = (float)_iArrValue[0];
	y = (float)_iArrValue[1];
}

//////////////////////////////////[]///////////////////////////
float & _tagVector2::operator[](unsigned int _idx)
{
	switch (_idx)
	{
	case 0:
		return x;
	case 1:
		return y;
	}

	assert(false);

	return x;
}
/////////////////////////////////=//////////////////////////
void _tagVector2::operator=(const _tagVector2 & _v)
{
	x = _v.x;
	y = _v.y;
}

void _tagVector2::operator=(const XMVECTOR & _v)
{
	XMStoreFloat2((XMFLOAT2*)this, _v);
}

void _tagVector2::operator=(const XMFLOAT2 & _v)
{
	x = _v.x;
	y = _v.y;
}

void _tagVector2::operator=(float _fValue)
{
	x = _fValue;
	y = _fValue;
}

void _tagVector2::operator=(int _iValue)
{
	x = (float)_iValue;
	y = (float)_iValue;
}

void _tagVector2::operator=(float _fArrValue[2])
{
	x = _fArrValue[0];
	y = _fArrValue[1];
}

void _tagVector2::operator=(int _iArrValue[2])
{
	for (int i = 0; i < 2; ++i)
	{
		(*this)[i] = (float)_iArrValue[i];
	}
}
////////////////////////////////+////////////////////////
_tagVector2 _tagVector2::operator+(const _tagVector2& _v)
{
	XMVECTOR tv, tv1, tv2;
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = XMLoadFloat2((XMFLOAT2*)&_v);

	tv = tv1 + tv2;

	return _tagVector2(tv);
}

_tagVector2 _tagVector2::operator+(const XMVECTOR & _v)
{
	XMVECTOR _tv, _v1;
	_v1 = XMLoadFloat2((XMFLOAT2*)this);

	_tv = _v1 + _v;

	return _tagVector2(_tv);
}

_tagVector2 _tagVector2::operator+(const XMFLOAT2 & _v)
{
	XMVECTOR tv, tv1, tv2;
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = XMLoadFloat2((XMFLOAT2*)&_v);

	tv = tv1 + tv2;

	return _tagVector2(tv);
}

_tagVector2 _tagVector2::operator+(float _fValue)
{
	XMVECTOR tv, tv1, tv2;
	_tagVector2 fValue(_fValue);
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = fValue.Convert();

	tv = tv1 + tv2;

	return _tagVector2(tv);
}

_tagVector2 _tagVector2::operator+(int _iValue)
{
	XMVECTOR tv, tv1, tv2;
	_tagVector2 iValue(_iValue);
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = iValue.Convert();

	tv = tv1 + tv2;

	return _tagVector2(tv);
}

_tagVector2 _tagVector2::operator+(float _fArrValue[2])
{
	XMVECTOR tv, tv1, tv2;
	_tagVector2 fValue(_fArrValue);
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = fValue.Convert();

	tv = tv1 + tv2;

	return _tagVector2(tv);
}

_tagVector2 _tagVector2::operator+(int _iArrValue[2])
{
	XMVECTOR tv, tv1, tv2;
	_tagVector2 iValue(_iArrValue);
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = iValue.Convert();

	tv = tv1 + tv2;

	return _tagVector2(tv);
}
/////////////////////////////+=////////////////////
void _tagVector2::operator+=(const _tagVector2 & _v)
{
	XMVECTOR _v1, _v2;
	_v1 = XMLoadFloat2((XMFLOAT2*)this);
	_v2 = _v.Convert();

	_v1 += _v2;

	Convert(_v1);
}

void _tagVector2::operator+=(const XMVECTOR& _v)
{
	XMVECTOR _v1;
	_v1 = Convert();

	_v1 += _v;

	Convert(_v1);
}

void _tagVector2::operator+=(const XMFLOAT2 & _v)
{
	XMVECTOR _v1, _v2;
	_v1 = Convert();
	_v2 = XMLoadFloat2((XMFLOAT2*)&_v);
	_v1 += _v2;

	Convert(_v1);
}

void _tagVector2::operator+=(float _fValue)
{
	XMVECTOR _v1, _v2;
	_tagVector2 v(_fValue);
	_v1 = Convert();
	_v2 = XMLoadFloat2((XMFLOAT2*)&v);
	_v1 += _v2;

	Convert(_v1);
}

void _tagVector2::operator+=(int _iValue)
{
	XMVECTOR _v1, _v2;
	_tagVector2 v(_iValue);
	_v1 = Convert();
	_v2 = XMLoadFloat2((XMFLOAT2*)&v);
	_v1 += _v2;

	Convert(_v1);
}

void _tagVector2::operator+=(float _fArrValue[2])
{
	XMVECTOR _v1, _v2;
	_tagVector2 v(_fArrValue);
	_v1 = Convert();
	_v2 = XMLoadFloat2((XMFLOAT2*)&v);
	_v1 += _v2;

	Convert(_v1);
}

void _tagVector2::operator+=(int _iArrValue[2])
{
	XMVECTOR _v1, _v2;
	_tagVector2 v(_iArrValue);
	_v1 = Convert();
	_v2 = XMLoadFloat2((XMFLOAT2*)&v);
	_v1 += _v2;

	Convert(_v1);
}
//=============================== - ==========================

_tagVector2 _tagVector2::operator-(const _tagVector2& _v)
{
	XMVECTOR tv, tv1, tv2;
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = XMLoadFloat2((XMFLOAT2*)&_v);

	tv = tv1 - tv2;

	return _tagVector2(tv);
}

_tagVector2 _tagVector2::operator-(const XMVECTOR & _v)
{
	XMVECTOR _tv, _v1;
	_v1 = XMLoadFloat2((XMFLOAT2*)this);

	_tv = _v1 - _v;

	return _tagVector2(_tv);
}

_tagVector2 _tagVector2::operator-(const XMFLOAT2 & _v)
{
	XMVECTOR tv, tv1, tv2;
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = XMLoadFloat2((XMFLOAT2*)&_v);

	tv = tv1 - tv2;

	return _tagVector2(tv);
}

_tagVector2 _tagVector2::operator-(float _fValue)
{
	XMVECTOR tv, tv1, tv2;
	_tagVector2 fValue(_fValue);
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = fValue.Convert();

	tv = tv1 - tv2;

	return _tagVector2(tv);
}

_tagVector2 _tagVector2::operator-(int _iValue)
{
	XMVECTOR tv, tv1, tv2;
	_tagVector2 iValue(_iValue);
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = iValue.Convert();

	tv = tv1 - tv2;

	return _tagVector2(tv);
}

_tagVector2 _tagVector2::operator-(float _fArrValue[2])
{
	XMVECTOR tv, tv1, tv2;
	_tagVector2 fValue(_fArrValue);
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = fValue.Convert();

	tv = tv1 - tv2;

	return _tagVector2(tv);
}

_tagVector2 _tagVector2::operator-(int _iArrValue[2])
{
	XMVECTOR tv, tv1, tv2;
	_tagVector2 iValue(_iArrValue);
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = iValue.Convert();

	tv = tv1 - tv2;

	return _tagVector2(tv);
}
//=============================== -= ==========================

void _tagVector2::operator-=(const _tagVector2 & _v)
{
	XMVECTOR _v1, _v2;
	_v1 = XMLoadFloat2((XMFLOAT2*)this);
	_v2 = _v.Convert();

	_v1 -= _v2;

	Convert(_v1);
}

void _tagVector2::operator-=(const XMVECTOR& _v)
{
	XMVECTOR _v1;
	_v1 = Convert();

	_v1 -= _v;

	Convert(_v1);
}

void _tagVector2::operator-=(const XMFLOAT2 & _v)
{
	XMVECTOR _v1, _v2;
	_v1 = Convert();
	_v2 = XMLoadFloat2((XMFLOAT2*)&_v);
	_v1 -= _v2;

	Convert(_v1);
}

void _tagVector2::operator-=(float _fValue)
{
	XMVECTOR _v1, _v2;
	_tagVector2 v(_fValue);
	_v1 = Convert();
	_v2 = XMLoadFloat2((XMFLOAT2*)&v);
	_v1 -= _v2;

	Convert(_v1);
}

void _tagVector2::operator-=(int _iValue)
{
	XMVECTOR _v1, _v2;
	_tagVector2 v(_iValue);
	_v1 = Convert();
	_v2 = XMLoadFloat2((XMFLOAT2*)&v);
	_v1 -= _v2;

	Convert(_v1);
}

void _tagVector2::operator-=(float _fArrValue[2])
{
	XMVECTOR _v1, _v2;
	_tagVector2 v(_fArrValue);
	_v1 = Convert();
	_v2 = XMLoadFloat2((XMFLOAT2*)&v);
	_v1 -= _v2;

	Convert(_v1);
}

void _tagVector2::operator-=(int _iArrValue[2])
{
	XMVECTOR _v1, _v2;
	_tagVector2 v(_iArrValue);
	_v1 = Convert();
	_v2 = XMLoadFloat2((XMFLOAT2*)&v);
	_v1 -= _v2;

	Convert(_v1);
}

//=============================== * ==========================

_tagVector2 _tagVector2::operator *(const _tagVector2& _v)
{
	XMVECTOR tv, tv1, tv2;
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = XMLoadFloat2((XMFLOAT2*)&_v);

	tv = tv1 * tv2;

	return _tagVector2(tv);
}

_tagVector2 _tagVector2::operator*(const XMVECTOR & _v)
{
	XMVECTOR _tv, _v1;
	_v1 = XMLoadFloat2((XMFLOAT2*)this);

	_tv = _v1 * _v;

	return _tagVector2(_tv);
}

_tagVector2 _tagVector2::operator*(const XMFLOAT2 & _v)
{
	XMVECTOR tv, tv1, tv2;
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = XMLoadFloat2((XMFLOAT2*)&_v);

	tv = tv1 * tv2;

	return _tagVector2(tv);
}

_tagVector2 _tagVector2::operator*(float _fValue)
{
	XMVECTOR tv, tv1, tv2;
	_tagVector2 fValue(_fValue);
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = fValue.Convert();

	tv = tv1 * tv2;

	return _tagVector2(tv);
}

_tagVector2 _tagVector2::operator*(int _iValue)
{
	XMVECTOR tv, tv1, tv2;
	_tagVector2 iValue(_iValue);
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = iValue.Convert();

	tv = tv1 * tv2;

	return _tagVector2(tv);
}

_tagVector2 _tagVector2::operator*(float _fArrValue[2])
{
	XMVECTOR tv, tv1, tv2;
	_tagVector2 fValue(_fArrValue);
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = fValue.Convert();

	tv = tv1 * tv2;

	return _tagVector2(tv);
}

_tagVector2 _tagVector2::operator*(int _iArrValue[2])
{
	XMVECTOR tv, tv1, tv2;
	_tagVector2 iValue(_iArrValue);
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = iValue.Convert();

	tv = tv1 * tv2;

	return _tagVector2(tv);
}

/////////////////////////////*=////////////////////
void _tagVector2::operator*=(const _tagVector2 & _v)
{
	XMVECTOR _v1, _v2;
	_v1 = XMLoadFloat2((XMFLOAT2*)this);
	_v2 = _v.Convert();

	_v1 *= _v2;

	Convert(_v1);
}

void _tagVector2::operator*=(const XMVECTOR& _v)
{
	XMVECTOR _v1;
	_v1 = Convert();

	_v1 *= _v;

	Convert(_v1);
}

void _tagVector2::operator*=(const XMFLOAT2 & _v)
{
	XMVECTOR _v1, _v2;
	_v1 = Convert();
	_v2 = XMLoadFloat2((XMFLOAT2*)&_v);
	_v1 *= _v2;

	Convert(_v1);
}

void _tagVector2::operator*=(float _fValue)
{
	XMVECTOR _v1, _v2;
	_tagVector2 v(_fValue);
	_v1 = Convert();
	_v2 = XMLoadFloat2((XMFLOAT2*)&v);
	_v1 *= _v2;

	Convert(_v1);
}

void _tagVector2::operator*=(int _iValue)
{
	XMVECTOR _v1, _v2;
	_tagVector2 v(_iValue);
	_v1 = Convert();
	_v2 = XMLoadFloat2((XMFLOAT2*)&v);
	_v1 *= _v2;

	Convert(_v1);
}

void _tagVector2::operator*=(float _fArrValue[2])
{
	XMVECTOR _v1, _v2;
	_tagVector2 v(_fArrValue);
	_v1 = Convert();
	_v2 = XMLoadFloat2((XMFLOAT2*)&v);
	_v1 *= _v2;

	Convert(_v1);
}

void _tagVector2::operator*=(int _iArrValue[2])
{
	XMVECTOR _v1, _v2;
	_tagVector2 v(_iArrValue);
	_v1 = Convert();
	_v2 = XMLoadFloat2((XMFLOAT2*)&v);
	_v1 *= _v2;

	Convert(_v1);
}

//=============================== / ==========================
_tagVector2 _tagVector2::operator/(const _tagVector2& _v)
{
	XMVECTOR tv, tv1, tv2;
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = XMLoadFloat2((XMFLOAT2*)&_v);

	tv = tv1 / tv2;

	return _tagVector2(tv);
}

_tagVector2 _tagVector2::operator/(const XMVECTOR & _v)
{
	XMVECTOR _tv, _v1;
	_v1 = XMLoadFloat2((XMFLOAT2*)this);

	_tv = _v1 / _v;

	return _tagVector2(_tv);
}

_tagVector2 _tagVector2::operator/(const XMFLOAT2 & _v)
{
	XMVECTOR tv, tv1, tv2;
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = XMLoadFloat2((XMFLOAT2*)&_v);

	tv = tv1 / tv2;

	return _tagVector2(tv);
}

_tagVector2 _tagVector2::operator/(float _fValue)
{
	XMVECTOR tv, tv1, tv2;
	_tagVector2 fValue(_fValue);
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = fValue.Convert();

	tv = tv1 / tv2;

	return _tagVector2(tv);
}

_tagVector2 _tagVector2::operator/(int _iValue)
{
	XMVECTOR tv, tv1, tv2;
	_tagVector2 iValue(_iValue);
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = iValue.Convert();

	tv = tv1 / tv2;

	return _tagVector2(tv);
}

_tagVector2 _tagVector2::operator/(float _fArrValue[2])
{
	XMVECTOR tv, tv1, tv2;
	_tagVector2 fValue(_fArrValue);
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = fValue.Convert();

	tv = tv1 / tv2;

	return _tagVector2(tv);
}

_tagVector2 _tagVector2::operator/(int _iArrValue[2])
{
	XMVECTOR tv, tv1, tv2;
	_tagVector2 iValue(_iArrValue);
	tv1 = XMLoadFloat2((XMFLOAT2*)this);
	tv2 = iValue.Convert();

	tv = tv1 / tv2;

	return _tagVector2(tv);
}
/////////////////////////////////// /= ////////////////////////
void _tagVector2::operator/=(const _tagVector2 & _v)
{
	XMVECTOR _v1, _v2;
	_v1 = XMLoadFloat2((XMFLOAT2*)this);
	_v2 = _v.Convert();

	_v1 /= _v2;

	Convert(_v1);
}

void _tagVector2::operator/=(const XMVECTOR& _v)
{
	XMVECTOR _v1;
	_v1 = Convert();

	_v1 /= _v;

	Convert(_v1);
}

void _tagVector2::operator/=(const XMFLOAT2 & _v)
{
	XMVECTOR _v1, _v2;
	_v1 = Convert();
	_v2 = XMLoadFloat2((XMFLOAT2*)&_v);
	_v1 /= _v2;

	Convert(_v1);
}

void _tagVector2::operator/=(float _fValue)
{
	XMVECTOR _v1, _v2;
	_tagVector2 v(_fValue);
	_v1 = Convert();
	_v2 = XMLoadFloat2((XMFLOAT2*)&v);
	_v1 /= _v2;

	Convert(_v1);
}

void _tagVector2::operator/=(int _iValue)
{
	XMVECTOR _v1, _v2;
	_tagVector2 v(_iValue);
	_v1 = Convert();
	_v2 = XMLoadFloat2((XMFLOAT2*)&v);
	_v1 /= _v2;

	Convert(_v1);
}

void _tagVector2::operator/=(float _fArrValue[2])
{
	XMVECTOR _v1, _v2;
	_tagVector2 v(_fArrValue);
	_v1 = Convert();
	_v2 = XMLoadFloat2((XMFLOAT2*)&v);
	_v1 /= _v2;

	Convert(_v1);
}

void _tagVector2::operator/=(int _iArrValue[2])
{
	XMVECTOR _v1, _v2;
	_tagVector2 v(_iArrValue);
	_v1 = Convert();
	_v2 = XMLoadFloat2((XMFLOAT2*)&v);
	_v1 /= _v2;

	Convert(_v1);
}
/////////////////////////////////==/////////////////////////////
bool _tagVector2::operator==(const _tagVector2 & _v) const
{
	return x == _v.x && y == _v.y;
}

bool _tagVector2::operator==(const XMVECTOR & _v) const
{
	_tagVector2 v(_v);
	return x == v.x && y == v.y;
}

bool _tagVector2::operator==(const XMFLOAT2 & _v) const
{
	return x == _v.x && y == _v.y;
}

bool _tagVector2::operator==(float _fValue) const
{
	return x == _fValue && y == _fValue;
}

bool _tagVector2::operator==(int _iValue) const
{
	return x == _iValue && y == _iValue;
}

bool _tagVector2::operator==(float _fArrValue[2]) const
{
	return x == _fArrValue[0] && y == _fArrValue[1];
}

bool _tagVector2::operator==(int _iArrValue[2]) const
{
	return x == _iArrValue[0] && y == _iArrValue[1];
}

/////////////////////////////////!=/////////////////////////////
bool _tagVector2::operator!=(const _tagVector2 & _v) const
{
	return !(x == _v.x && y == _v.y);
}

bool _tagVector2::operator != (const XMVECTOR & _v) const
{
	_tagVector2 v(_v);
	return !(x == v.x && y == v.y);
}

bool _tagVector2::operator!=(const XMFLOAT2 & _v) const
{
	return !(x == _v.x && y == _v.y);
}

bool _tagVector2::operator!=(float _fValue) const
{
	return !(x == _fValue && y == _fValue);
}

bool _tagVector2::operator!=(int _iValue) const
{
	return !(x == _iValue && y == _iValue);
}

bool _tagVector2::operator!=(float _fArrValue[2]) const
{
	return !(x == _fArrValue[0] && y == _fArrValue[1]);
}

bool _tagVector2::operator!=(int _iArrValue[2]) const
{
	return !(x == _iArrValue[0] && y == _iArrValue[1]);
}


float _tagVector2::Length() const
{
	XMVECTOR v = Convert();
	return XMVectorGetX(XMVector2Length(v));
}

XMVECTOR _tagVector2::Convert() const
{
	return XMLoadFloat2((XMFLOAT2*)this);
}

void _tagVector2::Convert(const XMVECTOR & _v)
{
	XMStoreFloat2((XMFLOAT2*)this, _v);
}

_tagVector2 _tagVector2::Normalize(const _tagVector2 & _v)
{
	XMVECTOR tv = _v.Convert();
	return _tagVector2(XMVector2Normalize(tv));
}
