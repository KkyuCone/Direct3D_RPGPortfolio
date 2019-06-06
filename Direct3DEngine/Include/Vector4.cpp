#include "Vector4.h"

ENGINE_USING

_tagVector4	_tagVector4::Zero = Vector4(0.f, 0.f, 0.f, 0.f);
_tagVector4	_tagVector4::One = Vector4(1.f, 1.f, 1.f, 1.f);

_tagVector4	_tagVector4::White = Vector4(1.f, 1.f, 1.f, 1.f);
_tagVector4	_tagVector4::Red = Vector4(1.f, 0.f, 0.f, 1.f);
_tagVector4	_tagVector4::Green = Vector4(0.f, 1.f, 0.f, 1.f);
_tagVector4	_tagVector4::Blue = Vector4(0.f, 0.f, 1.f, 1.f);
_tagVector4	_tagVector4::Black = Vector4(0.f, 0.f, 0.f, 1.f);
_tagVector4	_tagVector4::Yellow = Vector4(1.f, 1.f, 0.f, 1.f);
_tagVector4	_tagVector4::Magenta = Vector4(1.f, 0.f, 1.f, 1.f);

_tagVector4::_tagVector4()
	:x(0.f),
	y(0.f),
	z(0.f),
	w(0.f)
{
}

_tagVector4::_tagVector4(const _tagVector4 & _v)
{
	x = _v.x;
	y = _v.y;
	z = _v.z;
	w = _v.w;
}

_tagVector4::_tagVector4(const XMVECTOR & _v)
{
	XMStoreFloat4((XMFLOAT4*)this, _v);
}

_tagVector4::_tagVector4(const XMFLOAT4 & _v)
{
	x = _v.x;
	y = _v.y;
	z = _v.z;
	w = _v.w;
}

_tagVector4::_tagVector4(int _ix, int _iy, int _iz, int _iw)
{
	x = (float)_ix;
	y = (float)_iy;
	z = (float)_iz;
	w = (float)_iw;
}

_tagVector4::_tagVector4(float _fx, float _fy, float _fz, float _fw)
{
	x = _fx;
	y = _fy;
	z = _fz;
	w = _fw;
}

_tagVector4::_tagVector4(float _fValue)
{
	x = _fValue;
	y = _fValue;
	z = _fValue;
	w = _fValue;
}

_tagVector4::_tagVector4(int _iValue)
{
	x = (float)_iValue;
	y = (float)_iValue;
	z = (float)_iValue;
	w = (float)_iValue;
}

_tagVector4::_tagVector4(float _fArrValue[4])
{
	x = _fArrValue[0];
	y = _fArrValue[1];
	z = _fArrValue[2];
	w = _fArrValue[3];
}

_tagVector4::_tagVector4(int _iArrValue[4])
{
	x = (float)_iArrValue[0];
	y = (float)_iArrValue[1];
	z = (float)_iArrValue[2];
	w = (float)_iArrValue[3];
}

//////////////////////////////////[]///////////////////////////
float & _tagVector4::operator[](unsigned int _idx)
{
	switch (_idx)
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	case 3:
		return w;
	}

	assert(false);

	return x;
}
/////////////////////////////////=//////////////////////////
void _tagVector4::operator=(const _tagVector4 & _v)
{
	x = _v.x;
	y = _v.y;
	z = _v.z;
	w = _v.w;
}

void _tagVector4::operator=(const XMVECTOR & _v)
{
	XMStoreFloat4((XMFLOAT4*)this, _v);
}

void _tagVector4::operator=(const XMFLOAT4 & _v)
{
	x = _v.x;
	y = _v.y;
	z = _v.z;
	w = _v.w;
}

void _tagVector4::operator=(float _fValue)
{
	x = _fValue;
	y = _fValue;
	z = _fValue;
	w = _fValue;
}

void _tagVector4::operator=(int _iValue)
{
	x = (float)_iValue;
	y = (float)_iValue;
	z = (float)_iValue;
	w = (float)_iValue;
}

void _tagVector4::operator=(float _fArrValue[4])
{
	x = _fArrValue[0];
	y = _fArrValue[1];
	z = _fArrValue[2];
	w = _fArrValue[3];
}

void _tagVector4::operator=(int _iArrValue[4])
{
	for (int i = 0; i < 4; ++i)
	{
		(*this)[i] = (float)_iArrValue[i];
	}
}
////////////////////////////////+////////////////////////
_tagVector4 _tagVector4::operator+(const _tagVector4& _v)
{
	XMVECTOR tv, tv1, tv2;
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = XMLoadFloat4((XMFLOAT4*)&_v);

	tv = tv1 + tv2;

	return _tagVector4(tv);
}

_tagVector4 _tagVector4::operator+(const XMVECTOR & _v)
{
	XMVECTOR _tv, _v1;
	_v1 = XMLoadFloat4((XMFLOAT4*)this);

	_tv = _v1 + _v;

	return _tagVector4(_tv);
}

_tagVector4 _tagVector4::operator+(const XMFLOAT4 & _v)
{
	XMVECTOR tv, tv1, tv2;
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = XMLoadFloat4((XMFLOAT4*)&_v);

	tv = tv1 + tv2;

	return _tagVector4(tv);
}

_tagVector4 _tagVector4::operator+(float _fValue)
{
	XMVECTOR tv, tv1, tv2;
	_tagVector4 fValue(_fValue);
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = fValue.Convert();

	tv = tv1 + tv2;

	return _tagVector4(tv);
}

_tagVector4 _tagVector4::operator+(int _iValue)
{
	XMVECTOR tv, tv1, tv2;
	_tagVector4 iValue(_iValue);
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = iValue.Convert();

	tv = tv1 + tv2;

	return _tagVector4(tv);
}

_tagVector4 _tagVector4::operator+(float _fArrValue[4])
{
	XMVECTOR tv, tv1, tv2;
	_tagVector4 fValue(_fArrValue);
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = fValue.Convert();

	tv = tv1 + tv2;

	return _tagVector4(tv);
}

_tagVector4 _tagVector4::operator+(int _iArrValue[4])
{
	XMVECTOR tv, tv1, tv2;
	_tagVector4 iValue(_iArrValue);
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = iValue.Convert();

	tv = tv1 + tv2;

	return _tagVector4(tv);
}
/////////////////////////////+=////////////////////
void _tagVector4::operator+=(const _tagVector4 & _v)
{
	XMVECTOR _v1, _v2;
	_v1 = XMLoadFloat4((XMFLOAT4*)this);
	_v2 = _v.Convert();

	_v1 += _v2;

	Convert(_v1);
}

void _tagVector4::operator+=(const XMVECTOR& _v)
{
	XMVECTOR _v1;
	_v1 = Convert();

	_v1 += _v;

	Convert(_v1);
}

void _tagVector4::operator+=(const XMFLOAT4 & _v)
{
	XMVECTOR _v1, _v2;
	_v1 = Convert();
	_v2 = XMLoadFloat4((XMFLOAT4*)&_v);
	_v1 += _v2;

	Convert(_v1);
}

void _tagVector4::operator+=(float _fValue)
{
	XMVECTOR _v1, _v2;
	_tagVector4 v(_fValue);
	_v1 = Convert();
	_v2 = XMLoadFloat4((XMFLOAT4*)&v);
	_v1 += _v2;

	Convert(_v1);
}

void _tagVector4::operator+=(int _iValue)
{
	XMVECTOR _v1, _v2;
	_tagVector4 v(_iValue);
	_v1 = Convert();
	_v2 = XMLoadFloat4((XMFLOAT4*)&v);
	_v1 += _v2;

	Convert(_v1);
}

void _tagVector4::operator+=(float _fArrValue[4])
{
	XMVECTOR _v1, _v2;
	_tagVector4 v(_fArrValue);
	_v1 = Convert();
	_v2 = XMLoadFloat4((XMFLOAT4*)&v);
	_v1 += _v2;

	Convert(_v1);
}

void _tagVector4::operator+=(int _iArrValue[4])
{
	XMVECTOR _v1, _v2;
	_tagVector4 v(_iArrValue);
	_v1 = Convert();
	_v2 = XMLoadFloat4((XMFLOAT4*)&v);
	_v1 += _v2;

	Convert(_v1);
}
//=============================== - ==========================

_tagVector4 _tagVector4::operator-(const _tagVector4& _v)
{
	XMVECTOR tv, tv1, tv2;
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = XMLoadFloat4((XMFLOAT4*)&_v);

	tv = tv1 - tv2;

	return _tagVector4(tv);
}

_tagVector4 _tagVector4::operator-(const XMVECTOR & _v)
{
	XMVECTOR _tv, _v1;
	_v1 = XMLoadFloat4((XMFLOAT4*)this);

	_tv = _v1 - _v;

	return _tagVector4(_tv);
}

_tagVector4 _tagVector4::operator-(const XMFLOAT4 & _v)
{
	XMVECTOR tv, tv1, tv2;
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = XMLoadFloat4((XMFLOAT4*)&_v);

	tv = tv1 - tv2;

	return _tagVector4(tv);
}

_tagVector4 _tagVector4::operator-(float _fValue)
{
	XMVECTOR tv, tv1, tv2;
	_tagVector4 fValue(_fValue);
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = fValue.Convert();

	tv = tv1 - tv2;

	return _tagVector4(tv);
}

_tagVector4 _tagVector4::operator-(int _iValue)
{
	XMVECTOR tv, tv1, tv2;
	_tagVector4 iValue(_iValue);
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = iValue.Convert();

	tv = tv1 - tv2;

	return _tagVector4(tv);
}

_tagVector4 _tagVector4::operator-(float _fArrValue[4])
{
	XMVECTOR tv, tv1, tv2;
	_tagVector4 fValue(_fArrValue);
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = fValue.Convert();

	tv = tv1 - tv2;

	return _tagVector4(tv);
}

_tagVector4 _tagVector4::operator-(int _iArrValue[4])
{
	XMVECTOR tv, tv1, tv2;
	_tagVector4 iValue(_iArrValue);
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = iValue.Convert();

	tv = tv1 - tv2;

	return _tagVector4(tv);
}
//=============================== -= ==========================

void _tagVector4::operator-=(const _tagVector4 & _v)
{
	XMVECTOR _v1, _v2;
	_v1 = XMLoadFloat4((XMFLOAT4*)this);
	_v2 = _v.Convert();

	_v1 -= _v2;

	Convert(_v1);
}

void _tagVector4::operator-=(const XMVECTOR& _v)
{
	XMVECTOR _v1;
	_v1 = Convert();

	_v1 -= _v;

	Convert(_v1);
}

void _tagVector4::operator-=(const XMFLOAT4 & _v)
{
	XMVECTOR _v1, _v2;
	_v1 = Convert();
	_v2 = XMLoadFloat4((XMFLOAT4*)&_v);
	_v1 -= _v2;

	Convert(_v1);
}

void _tagVector4::operator-=(float _fValue)
{
	XMVECTOR _v1, _v2;
	_tagVector4 v(_fValue);
	_v1 = Convert();
	_v2 = XMLoadFloat4((XMFLOAT4*)&v);
	_v1 -= _v2;

	Convert(_v1);
}

void _tagVector4::operator-=(int _iValue)
{
	XMVECTOR _v1, _v2;
	_tagVector4 v(_iValue);
	_v1 = Convert();
	_v2 = XMLoadFloat4((XMFLOAT4*)&v);
	_v1 -= _v2;

	Convert(_v1);
}

void _tagVector4::operator-=(float _fArrValue[4])
{
	XMVECTOR _v1, _v2;
	_tagVector4 v(_fArrValue);
	_v1 = Convert();
	_v2 = XMLoadFloat4((XMFLOAT4*)&v);
	_v1 -= _v2;

	Convert(_v1);
}

void _tagVector4::operator-=(int _iArrValue[4])
{
	XMVECTOR _v1, _v2;
	_tagVector4 v(_iArrValue);
	_v1 = Convert();
	_v2 = XMLoadFloat4((XMFLOAT4*)&v);
	_v1 -= _v2;

	Convert(_v1);
}

//=============================== * ==========================

_tagVector4 _tagVector4::operator *(const _tagVector4& _v)
{
	XMVECTOR tv, tv1, tv2;
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = XMLoadFloat4((XMFLOAT4*)&_v);

	tv = tv1 * tv2;

	return _tagVector4(tv);
}

_tagVector4 _tagVector4::operator*(const XMVECTOR & _v)
{
	XMVECTOR _tv, _v1;
	_v1 = XMLoadFloat4((XMFLOAT4*)this);

	_tv = _v1 * _v;

	return _tagVector4(_tv);
}

_tagVector4 _tagVector4::operator*(const XMFLOAT4 & _v)
{
	XMVECTOR tv, tv1, tv2;
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = XMLoadFloat4((XMFLOAT4*)&_v);

	tv = tv1 * tv2;

	return _tagVector4(tv);
}

_tagVector4 _tagVector4::operator*(float _fValue)
{
	XMVECTOR tv, tv1, tv2;
	_tagVector4 fValue(_fValue);
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = fValue.Convert();

	tv = tv1 * tv2;

	return _tagVector4(tv);
}

_tagVector4 _tagVector4::operator*(int _iValue)
{
	XMVECTOR tv, tv1, tv2;
	_tagVector4 iValue(_iValue);
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = iValue.Convert();

	tv = tv1 * tv2;

	return _tagVector4(tv);
}

_tagVector4 _tagVector4::operator*(float _fArrValue[4])
{
	XMVECTOR tv, tv1, tv2;
	_tagVector4 fValue(_fArrValue);
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = fValue.Convert();

	tv = tv1 * tv2;

	return _tagVector4(tv);
}

_tagVector4 _tagVector4::operator*(int _iArrValue[4])
{
	XMVECTOR tv, tv1, tv2;
	_tagVector4 iValue(_iArrValue);
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = iValue.Convert();

	tv = tv1 * tv2;

	return _tagVector4(tv);
}

/////////////////////////////*=////////////////////
void _tagVector4::operator*=(const _tagVector4 & _v)
{
	XMVECTOR _v1, _v2;
	_v1 = XMLoadFloat4((XMFLOAT4*)this);
	_v2 = _v.Convert();

	_v1 *= _v2;

	Convert(_v1);
}

void _tagVector4::operator*=(const XMVECTOR& _v)
{
	XMVECTOR _v1;
	_v1 = Convert();

	_v1 *= _v;

	Convert(_v1);
}

void _tagVector4::operator*=(const XMFLOAT4 & _v)
{
	XMVECTOR _v1, _v2;
	_v1 = Convert();
	_v2 = XMLoadFloat4((XMFLOAT4*)&_v);
	_v1 *= _v2;

	Convert(_v1);
}

void _tagVector4::operator*=(float _fValue)
{
	XMVECTOR _v1, _v2;
	_tagVector4 v(_fValue);
	_v1 = Convert();
	_v2 = XMLoadFloat4((XMFLOAT4*)&v);
	_v1 *= _v2;

	Convert(_v1);
}

void _tagVector4::operator*=(int _iValue)
{
	XMVECTOR _v1, _v2;
	_tagVector4 v(_iValue);
	_v1 = Convert();
	_v2 = XMLoadFloat4((XMFLOAT4*)&v);
	_v1 *= _v2;

	Convert(_v1);
}

void _tagVector4::operator*=(float _fArrValue[4])
{
	XMVECTOR _v1, _v2;
	_tagVector4 v(_fArrValue);
	_v1 = Convert();
	_v2 = XMLoadFloat4((XMFLOAT4*)&v);
	_v1 *= _v2;

	Convert(_v1);
}

void _tagVector4::operator*=(int _iArrValue[4])
{
	XMVECTOR _v1, _v2;
	_tagVector4 v(_iArrValue);
	_v1 = Convert();
	_v2 = XMLoadFloat4((XMFLOAT4*)&v);
	_v1 *= _v2;

	Convert(_v1);
}

//=============================== / ==========================
_tagVector4 _tagVector4::operator/(const _tagVector4& _v)
{
	XMVECTOR tv, tv1, tv2;
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = XMLoadFloat4((XMFLOAT4*)&_v);

	tv = tv1 / tv2;

	return _tagVector4(tv);
}

_tagVector4 _tagVector4::operator/(const XMVECTOR & _v)
{
	XMVECTOR _tv, _v1;
	_v1 = XMLoadFloat4((XMFLOAT4*)this);

	_tv = _v1 / _v;

	return _tagVector4(_tv);
}

_tagVector4 _tagVector4::operator/(const XMFLOAT4 & _v)
{
	XMVECTOR tv, tv1, tv2;
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = XMLoadFloat4((XMFLOAT4*)&_v);

	tv = tv1 / tv2;

	return _tagVector4(tv);
}

_tagVector4 _tagVector4::operator/(float _fValue)
{
	XMVECTOR tv, tv1, tv2;
	_tagVector4 fValue(_fValue);
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = fValue.Convert();

	tv = tv1 / tv2;

	return _tagVector4(tv);
}

_tagVector4 _tagVector4::operator/(int _iValue)
{
	XMVECTOR tv, tv1, tv2;
	_tagVector4 iValue(_iValue);
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = iValue.Convert();

	tv = tv1 / tv2;

	return _tagVector4(tv);
}

_tagVector4 _tagVector4::operator/(float _fArrValue[4])
{
	XMVECTOR tv, tv1, tv2;
	_tagVector4 fValue(_fArrValue);
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = fValue.Convert();

	tv = tv1 / tv2;

	return _tagVector4(tv);
}

_tagVector4 _tagVector4::operator/(int _iArrValue[4])
{
	XMVECTOR tv, tv1, tv2;
	_tagVector4 iValue(_iArrValue);
	tv1 = XMLoadFloat4((XMFLOAT4*)this);
	tv2 = iValue.Convert();

	tv = tv1 / tv2;

	return _tagVector4(tv);
}
/////////////////////////////////// /= ////////////////////////
void _tagVector4::operator/=(const _tagVector4 & _v)
{
	XMVECTOR _v1, _v2;
	_v1 = XMLoadFloat4((XMFLOAT4*)this);
	_v2 = _v.Convert();

	_v1 /= _v2;

	Convert(_v1);
}

void _tagVector4::operator/=(const XMVECTOR& _v)
{
	XMVECTOR _v1;
	_v1 = Convert();

	_v1 /= _v;

	Convert(_v1);
}

void _tagVector4::operator/=(const XMFLOAT4 & _v)
{
	XMVECTOR _v1, _v2;
	_v1 = Convert();
	_v2 = XMLoadFloat4((XMFLOAT4*)&_v);
	_v1 /= _v2;

	Convert(_v1);
}

void _tagVector4::operator/=(float _fValue)
{
	XMVECTOR _v1, _v2;
	_tagVector4 v(_fValue);
	_v1 = Convert();
	_v2 = XMLoadFloat4((XMFLOAT4*)&v);
	_v1 /= _v2;

	Convert(_v1);
}

void _tagVector4::operator/=(int _iValue)
{
	XMVECTOR _v1, _v2;
	_tagVector4 v(_iValue);
	_v1 = Convert();
	_v2 = XMLoadFloat4((XMFLOAT4*)&v);
	_v1 /= _v2;

	Convert(_v1);
}

void _tagVector4::operator/=(float _fArrValue[4])
{
	XMVECTOR _v1, _v2;
	_tagVector4 v(_fArrValue);
	_v1 = Convert();
	_v2 = XMLoadFloat4((XMFLOAT4*)&v);
	_v1 /= _v2;

	Convert(_v1);
}

void _tagVector4::operator/=(int _iArrValue[4])
{
	XMVECTOR _v1, _v2;
	_tagVector4 v(_iArrValue);
	_v1 = Convert();
	_v2 = XMLoadFloat4((XMFLOAT4*)&v);
	_v1 /= _v2;

	Convert(_v1);
}
/////////////////////////////////==/////////////////////////////
bool _tagVector4::operator==(const _tagVector4 & _v) const
{
	return x == _v.x && y == _v.y && z == _v.z && _v.w;
}

bool _tagVector4::operator==(const XMVECTOR & _v) const
{
	_tagVector4 v(_v);
	return x == v.x && y == v.y && z == v.z && v.w;
}

bool _tagVector4::operator==(const XMFLOAT4 & _v) const
{
	return x == _v.x && y == _v.y && z == _v.z && w == _v.w;
}

bool _tagVector4::operator==(float _fValue) const
{
	return x == _fValue && y == _fValue && z == _fValue && w == _fValue;
}

bool _tagVector4::operator==(int _iValue) const
{
	return x == _iValue && y == _iValue && z == _iValue && w == _iValue;
}

bool _tagVector4::operator==(float _fArrValue[4]) const
{
	return x == _fArrValue[0] && y == _fArrValue[1] && z == _fArrValue[2]
		&& w == _fArrValue[3];
}

bool _tagVector4::operator==(int _iArrValue[4]) const
{
	return x == _iArrValue[0] && y == _iArrValue[1] && z == _iArrValue[2]
		&& w == _iArrValue[3];
}

/////////////////////////////////!=/////////////////////////////
bool _tagVector4::operator!=(const _tagVector4 & _v) const
{
	return !(x == _v.x && y == _v.y && z == _v.z && _v.w);
}

bool _tagVector4::operator != (const XMVECTOR & _v) const
{
	_tagVector4 v(_v);
	return !(x == v.x && y == v.y && z == v.z && w == v.w);
}

bool _tagVector4::operator!=(const XMFLOAT4 & _v) const
{
	return !(x == _v.x && y == _v.y && z == _v.z && w == _v.w);
}

bool _tagVector4::operator!=(float _fValue) const
{
	return !(x == _fValue && y == _fValue && z == _fValue && w == _fValue);
}

bool _tagVector4::operator!=(int _iValue) const
{
	return !(x == _iValue && y == _iValue && z == _iValue && w == _iValue);
}

bool _tagVector4::operator!=(float _fArrValue[4]) const
{
	return !(x == _fArrValue[0] && y == _fArrValue[1] && z == _fArrValue[2]
		&& w == _fArrValue[3]);
}

bool _tagVector4::operator!=(int _iArrValue[4]) const
{
	return !(x == _iArrValue[0] && y == _iArrValue[1] && z == _iArrValue[2]
		&& w == _iArrValue[3]);
}

XMVECTOR _tagVector4::Convert() const
{
	return XMLoadFloat4((XMFLOAT4*)this);
}

void _tagVector4::Convert(const XMVECTOR & _v)
{
	XMStoreFloat4((XMFLOAT4*)this, _v);
}

_tagVector4 _tagVector4::Normalize(const _tagVector4 & _v)
{
	XMVECTOR tv = _v.Convert();
	return _tagVector4(XMVector4Normalize(tv));
}
