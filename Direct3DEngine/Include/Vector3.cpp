#include "Vector3.h"
#include "Matrix.h"

ENGINE_USING

_tagVector3	_tagVector3::Axis[3] = { Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Vector3(0.f, 0.f, 1.f) };
_tagVector3	_tagVector3::Zero = Vector3(0.f, 0.f, 0.f);
_tagVector3	_tagVector3::One = Vector3(1.f, 1.f, 1.f);

_tagVector3::_tagVector3() :
	x(0.f),
	y(0.f),
	z(0.f)
{
}

_tagVector3::_tagVector3(const _tagVector3 & v)
{
	x = v.x;
	g = v.g;
	z = v.z;
}

_tagVector3::_tagVector3(const XMVECTOR & v)
{
	XMStoreFloat3((XMFLOAT3*)this, v);
}

_tagVector3::_tagVector3(const XMFLOAT3 & v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

_tagVector3::_tagVector3(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

_tagVector3::_tagVector3(int _x, int _y, int _z)
{
	x = (float)_x;
	y = (float)_y;
	z = (float)_z;
}

_tagVector3::_tagVector3(float f)
{
	x = f;
	y = f;
	z = f;
}

_tagVector3::_tagVector3(int i)
{
	x = (float)i;
	y = (float)i;
	z = (float)i;
}

_tagVector3::_tagVector3(float f[3])
{
	x = f[0];
	y = f[1];
	z = f[2];
}

_tagVector3::_tagVector3(int i[3])
{
	x = (float)i[0];
	y = (float)i[1];
	z = (float)i[2];
}

float & _tagVector3::operator[](unsigned int idx)
{
	switch (idx)
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	}

	assert(false);

	return x;
}

void _tagVector3::operator=(const _tagVector3 & v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

void _tagVector3::operator=(const XMVECTOR & v)
{
	XMStoreFloat3((XMFLOAT3*)this, v);
}

void _tagVector3::operator=(const XMFLOAT3 & v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

void _tagVector3::operator=(float f)
{
	x = f;
	y = f;
	z = f;
}

void _tagVector3::operator=(int i)
{
	x = (float)i;
	y = (float)i;
	z = (float)i;
}

void _tagVector3::operator=(float f[3])
{
	x = f[0];
	y = f[1];
	z = f[2];
}

void _tagVector3::operator=(int i[3])
{
	for (int j = 0; j < 3; ++j)
	{
		(*this)[j] = (float)(i[j]);
	}
}

// +
_tagVector3 _tagVector3::operator+(const _tagVector3 & v) const
{
	XMVECTOR	_v, _v1, _v2;
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v = _v1 + _v2;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator+(const XMVECTOR & v) const
{
	XMVECTOR	_v, _v1;
	_v1 = XMLoadFloat3((XMFLOAT3*)this);

	_v = _v1 + v;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator+(const XMFLOAT3 & v) const
{
	XMVECTOR	_v, _v1, _v2;
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = XMLoadFloat3(&v);

	_v = _v1 + _v2;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator+(float f) const
{
	XMVECTOR	_v, _v1, _v2;
	_tagVector3	v(f);
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = v.Convert();

	_v = _v1 + _v2;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator+(int i) const
{
	XMVECTOR	_v, _v1, _v2;
	_tagVector3	v(i);
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = v.Convert();

	_v = _v1 + _v2;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator+(float f[3]) const
{
	XMVECTOR	_v, _v1, _v2;
	_tagVector3	v(f);
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = v.Convert();

	_v = _v1 + _v2;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator+(int i[3]) const
{
	XMVECTOR	_v, _v1, _v2;
	_tagVector3	v(i);
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = v.Convert();

	_v = _v1 + _v2;

	return _tagVector3(_v);
}

// +=
void _tagVector3::operator+=(const _tagVector3 & v)
{
	XMVECTOR	_v1, _v2;
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = v.Convert();

	_v1 += _v2;

	Convert(_v1);
}

void _tagVector3::operator+=(const XMVECTOR & v)
{
	XMVECTOR	_v1;
	_v1 = Convert();

	_v1 += v;

	Convert(_v1);
}

void _tagVector3::operator+=(const XMFLOAT3 & v)
{
	XMVECTOR	_v1, _v2;
	_v1 = Convert();
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v1 += _v2;

	Convert(_v1);
}

void _tagVector3::operator+=(float f)
{
	XMVECTOR	_v1, _v2;
	_tagVector3	v(f);
	_v1 = Convert();
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v1 += _v2;

	Convert(_v1);
}

void _tagVector3::operator+=(int i)
{
	XMVECTOR	_v1, _v2;
	_tagVector3	v(i);
	_v1 = Convert();
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v1 += _v2;

	Convert(_v1);
}

void _tagVector3::operator+=(float f[3])
{
	XMVECTOR	_v1, _v2;
	_tagVector3	v(f);
	_v1 = Convert();
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v1 += _v2;

	Convert(_v1);
}

void _tagVector3::operator+=(int i[3])
{
	XMVECTOR	_v1, _v2;
	_tagVector3	v(i);
	_v1 = Convert();
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v1 += _v2;

	Convert(_v1);
}

// -
_tagVector3 _tagVector3::operator-(const _tagVector3 & v) const
{
	XMVECTOR	_v, _v1, _v2;
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v = _v1 - _v2;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator-(const XMVECTOR & v) const
{
	XMVECTOR	_v, _v1;
	_v1 = XMLoadFloat3((XMFLOAT3*)this);

	_v = _v1 - v;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator-(const XMFLOAT3 & v) const
{
	XMVECTOR	_v, _v1, _v2;
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = XMLoadFloat3(&v);

	_v = _v1 - _v2;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator-(float f) const
{
	XMVECTOR	_v, _v1, _v2;
	_tagVector3	v(f);
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = v.Convert();

	_v = _v1 - _v2;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator-(int i) const
{
	XMVECTOR	_v, _v1, _v2;
	_tagVector3	v(i);
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = v.Convert();

	_v = _v1 - _v2;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator-(float f[3]) const
{
	XMVECTOR	_v, _v1, _v2;
	_tagVector3	v(f);
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = v.Convert();

	_v = _v1 - _v2;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator-(int i[3]) const
{
	XMVECTOR	_v, _v1, _v2;
	_tagVector3	v(i);
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = v.Convert();

	_v = _v1 - _v2;

	return _tagVector3(_v);
}

// -=
void _tagVector3::operator-=(const _tagVector3 & v)
{
	XMVECTOR	_v1, _v2;
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = v.Convert();

	_v1 -= _v2;

	Convert(_v1);
}

void _tagVector3::operator-=(const XMVECTOR & v)
{
	XMVECTOR	_v1;
	_v1 = Convert();

	_v1 -= v;

	Convert(_v1);
}

void _tagVector3::operator-=(const XMFLOAT3 & v)
{
	XMVECTOR	_v1, _v2;
	_v1 = Convert();
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v1 -= _v2;

	Convert(_v1);
}

void _tagVector3::operator-=(float f)
{
	XMVECTOR	_v1, _v2;
	_tagVector3	v(f);
	_v1 = Convert();
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v1 -= _v2;

	Convert(_v1);
}

void _tagVector3::operator-=(int i)
{
	XMVECTOR	_v1, _v2;
	_tagVector3	v(i);
	_v1 = Convert();
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v1 -= _v2;

	Convert(_v1);
}

void _tagVector3::operator-=(float f[3])
{
	XMVECTOR	_v1, _v2;
	_tagVector3	v(f);
	_v1 = Convert();
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v1 -= _v2;

	Convert(_v1);
}

void _tagVector3::operator-=(int i[3])
{
	XMVECTOR	_v1, _v2;
	_tagVector3	v(i);
	_v1 = Convert();
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v1 -= _v2;

	Convert(_v1);
}

// *
_tagVector3 _tagVector3::operator*(const _tagVector3 & v) const
{
	XMVECTOR	_v, _v1, _v2;
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v = _v1 * _v2;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator*(const XMVECTOR & v) const
{
	XMVECTOR	_v, _v1;
	_v1 = XMLoadFloat3((XMFLOAT3*)this);

	_v = _v1 * v;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator*(const XMFLOAT3 & v) const
{
	XMVECTOR	_v, _v1, _v2;
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = XMLoadFloat3(&v);

	_v = _v1 * _v2;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator*(float f) const
{
	XMVECTOR	_v, _v1, _v2;
	_tagVector3	v(f);
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = v.Convert();

	_v = _v1 * _v2;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator*(int i) const
{
	XMVECTOR	_v, _v1, _v2;
	_tagVector3	v(i);
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = v.Convert();

	_v = _v1 * _v2;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator*(float f[3]) const
{
	XMVECTOR	_v, _v1, _v2;
	_tagVector3	v(f);
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = v.Convert();

	_v = _v1 * _v2;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator*(int i[3]) const
{
	XMVECTOR	_v, _v1, _v2;
	_tagVector3	v(i);
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = v.Convert();

	_v = _v1 * _v2;

	return _tagVector3(_v);
}

// *=
void _tagVector3::operator*=(const _tagVector3 & v)
{
	XMVECTOR	_v1, _v2;
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = v.Convert();

	_v1 *= _v2;

	Convert(_v1);
}

void _tagVector3::operator*=(const XMVECTOR & v)
{
	XMVECTOR	_v1;
	_v1 = Convert();

	_v1 *= v;

	Convert(_v1);
}

void _tagVector3::operator*=(const XMFLOAT3 & v)
{
	XMVECTOR	_v1, _v2;
	_v1 = Convert();
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v1 *= _v2;

	Convert(_v1);
}

void _tagVector3::operator*=(float f)
{
	XMVECTOR	_v1, _v2;
	_tagVector3	v(f);
	_v1 = Convert();
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v1 *= _v2;

	Convert(_v1);
}

void _tagVector3::operator*=(int i)
{
	XMVECTOR	_v1, _v2;
	_tagVector3	v(i);
	_v1 = Convert();
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v1 *= _v2;

	Convert(_v1);
}

void _tagVector3::operator*=(float f[3])
{
	XMVECTOR	_v1, _v2;
	_tagVector3	v(f);
	_v1 = Convert();
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v1 *= _v2;

	Convert(_v1);
}

void _tagVector3::operator*=(int i[3])
{
	XMVECTOR	_v1, _v2;
	_tagVector3	v(i);
	_v1 = Convert();
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v1 *= _v2;

	Convert(_v1);
}

// /
_tagVector3 _tagVector3::operator/(const _tagVector3 & v) const
{
	XMVECTOR	_v, _v1, _v2;
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v = _v1 / _v2;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator/(const XMVECTOR & v) const
{
	XMVECTOR	_v, _v1;
	_v1 = XMLoadFloat3((XMFLOAT3*)this);

	_v = _v1 / v;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator/(const XMFLOAT3 & v) const
{
	XMVECTOR	_v, _v1, _v2;
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = XMLoadFloat3(&v);

	_v = _v1 / _v2;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator/(float f) const
{
	XMVECTOR	_v, _v1, _v2;
	_tagVector3	v(f);
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = v.Convert();

	_v = _v1 / _v2;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator/(int i) const
{
	XMVECTOR	_v, _v1, _v2;
	_tagVector3	v(i);
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = v.Convert();

	_v = _v1 / _v2;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator/(float f[3]) const
{
	XMVECTOR	_v, _v1, _v2;
	_tagVector3	v(f);
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = v.Convert();

	_v = _v1 / _v2;

	return _tagVector3(_v);
}

_tagVector3 _tagVector3::operator/(int i[3]) const
{
	XMVECTOR	_v, _v1, _v2;
	_tagVector3	v(i);
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = v.Convert();

	_v = _v1 / _v2;

	return _tagVector3(_v);
}

// /=
void _tagVector3::operator/=(const _tagVector3 & v)
{
	XMVECTOR	_v1, _v2;
	_v1 = XMLoadFloat3((XMFLOAT3*)this);
	_v2 = v.Convert();

	_v1 /= _v2;

	Convert(_v1);
}

void _tagVector3::operator/=(const XMVECTOR & v)
{
	XMVECTOR	_v1;
	_v1 = Convert();

	_v1 /= v;

	Convert(_v1);
}

void _tagVector3::operator/=(const XMFLOAT3 & v)
{
	XMVECTOR	_v1, _v2;
	_v1 = Convert();
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v1 /= _v2;

	Convert(_v1);
}

void _tagVector3::operator/=(float f)
{
	XMVECTOR	_v1, _v2;
	_tagVector3	v(f);
	_v1 = Convert();
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v1 /= _v2;

	Convert(_v1);
}

void _tagVector3::operator/=(int i)
{
	XMVECTOR	_v1, _v2;
	_tagVector3	v(i);
	_v1 = Convert();
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v1 /= _v2;

	Convert(_v1);
}

void _tagVector3::operator/=(float f[3])
{
	XMVECTOR	_v1, _v2;
	_tagVector3	v(f);
	_v1 = Convert();
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v1 /= _v2;

	Convert(_v1);
}

void _tagVector3::operator/=(int i[3])
{
	XMVECTOR	_v1, _v2;
	_tagVector3	v(i);
	_v1 = Convert();
	_v2 = XMLoadFloat3((XMFLOAT3*)&v);

	_v1 /= _v2;

	Convert(_v1);
}

bool _tagVector3::operator==(const _tagVector3 & v) const
{
	return x == v.x && y == v.y && z == v.z;
}

bool _tagVector3::operator==(const XMVECTOR & v) const
{
	_tagVector3	_v(v);
	return x == _v.x && y == _v.y && z == _v.z;
}

bool _tagVector3::operator==(const XMFLOAT3 & v) const
{
	return x == v.x && y == v.y && z == v.z;
}

bool _tagVector3::operator==(float f[3]) const
{
	return x == f[0] && y == f[1] && z == f[2];
}

bool _tagVector3::operator==(int i[3]) const
{
	return x == i[0] && y == i[1] && z == i[2];
}

bool _tagVector3::operator==(float f) const
{
	return x == f && y == f && z == f;
}

bool _tagVector3::operator==(int i) const
{
	return x == i && y == i && z == i;
}

bool _tagVector3::operator!=(const _tagVector3 & v) const
{
	return x != v.x || y != v.y || z != v.z;
}

bool _tagVector3::operator!=(const XMVECTOR & v) const
{
	_tagVector3	_v(v);
	return x != _v.x || y != _v.y || z != _v.z;
}

bool _tagVector3::operator!=(const XMFLOAT3 & v) const
{
	return x != v.x || y != v.y || z != v.z;
}

bool _tagVector3::operator!=(float f[3]) const
{
	return x != f[0] || y != f[1] || z != f[2];
}

bool _tagVector3::operator!=(int i[3]) const
{
	return x != i[0] || y != i[1] || z != i[2];
}

bool _tagVector3::operator!=(float f) const
{
	return x != f || y != f || z != f;
}

bool _tagVector3::operator!=(int i) const
{
	return x != i || y != i || z != i;
}

float _tagVector3::Length() const
{
	XMVECTOR	v = Convert();
	return XMVectorGetX(XMVector3Length(v));
}

float _tagVector3::Distance(const _tagVector3 & v) const
{
	return (*this - v).Length();
}

void _tagVector3::Normalize()
{
	XMVECTOR	v = Convert();
	*this = XMVector3Normalize(v);
}

float _tagVector3::Dot(const _tagVector3 & v) const
{
	return Dot(v.Convert());
}

float _tagVector3::Dot(const XMVECTOR & v) const
{
	return XMVectorGetX(XMVector3Dot(Convert(), v));
}

float _tagVector3::Dot(const XMFLOAT3 & v) const
{
	return Dot(XMLoadFloat3(&v));
}

float _tagVector3::Dot(float f[3]) const
{
	_tagVector3	v(f);
	return Dot(v.Convert());
}

float _tagVector3::Dot(int i[3]) const
{
	_tagVector3	v(i);
	return Dot(v.Convert());
}

float _tagVector3::Angle(const _tagVector3 & v) const
{
	_tagVector3	v1 = Normalize(*this);
	_tagVector3	v2 = Normalize(v);

	return RadianToDegree(acosf(v1.Dot(v2)));
}

float _tagVector3::Angle(const XMVECTOR & v) const
{
	_tagVector3	v1 = Normalize(*this);
	_tagVector3	v2 = Normalize(v);

	return RadianToDegree(acosf(v1.Dot(v2)));
}

float _tagVector3::Angle(const XMFLOAT3 & v) const
{
	_tagVector3	v1 = Normalize(*this);
	_tagVector3	v2 = Normalize(v);

	return RadianToDegree(acosf(v1.Dot(v2)));
}

float _tagVector3::Angle(float f[3]) const
{
	_tagVector3	v1 = Normalize(*this);
	_tagVector3	v2 = Normalize(f);

	return RadianToDegree(acosf(v1.Dot(v2)));
}

float _tagVector3::Angle(int i[3]) const
{
	_tagVector3	v1 = Normalize(*this);
	_tagVector3	v2 = Normalize(i);

	return RadianToDegree(acosf(v1.Dot(v2)));
}

_tagVector3 _tagVector3::Cross(const _tagVector3 & v) const
{
	return Cross(v.Convert());
}

_tagVector3 _tagVector3::Cross(const XMVECTOR & v) const
{
	_tagVector3 _v = _tagVector3(XMVector3Cross(Convert(), v));

	if (isnan(_v.x) || isnan(_v.y) || isnan(_v.z))
		return _tagVector3();

	return _v;
}

_tagVector3 _tagVector3::Cross(const XMFLOAT3 & v) const
{
	return Cross(XMLoadFloat3(&v));
}

_tagVector3 _tagVector3::Cross(float f[3]) const
{
	_tagVector3	v(f);
	return Cross(v.Convert());
}

_tagVector3 _tagVector3::Cross(int i[3]) const
{
	_tagVector3	v(i);
	return Cross(v.Convert());
}

_tagVector3 _tagVector3::TransformNormal(const _tagMatrix & m)
{
	// 벡터와 행렬의 곱 계산 (벡터 변환) 동차좌표 0 적용 ( w = 0 )
	//XMVECTOR XMVector3TransformNormal(FXMVECTOR V, CXMMATRIX M);
	return _tagVector3(XMVector3TransformNormal(Convert(), m.m));
}

_tagVector3 _tagVector3::TransformNormal(const XMMATRIX & m)
{
	return _tagVector3(XMVector3TransformNormal(Convert(), m));
}

_tagVector3 _tagVector3::TransformCoord(const _tagMatrix & m)
{
	// 벡터와 행렬의 곱 계산 (점 변환) 동차좌표 1 적용
	// 행렬 M으로부터 벡터 vector = (x, y, z, 1)을 변환한 다음
	// 그결과를 w = 1이 되도록한다.
	return _tagVector3(XMVector3TransformCoord(Convert(), m.m));
}

_tagVector3 _tagVector3::TransformCoord(const XMMATRIX & m)
{
	return _tagVector3(XMVector3TransformCoord(Convert(), m));
}

XMVECTOR _tagVector3::Convert() const
{
	return XMLoadFloat3((XMFLOAT3*)this);
}

void _tagVector3::Convert(const XMVECTOR & v)
{
	XMStoreFloat3((XMFLOAT3*)this, v);
}

_tagVector3 _tagVector3::Normalize(const _tagVector3 & v)
{
	XMVECTOR	_v = v.Convert();
	return _tagVector3(XMVector3Normalize(_v));
}
