#include "Matrix.h"
#include "Vector3.h"

ENGINE_USING

_tagMatrix::_tagMatrix()
{
	/*
	1 0 0 0
	0 1 0 0
	0 0 1 0
	0 0 0 1
	*/
	// XMMatrixIdentity() : 항등행렬을 리턴해주는 함수이다.
	m = XMMatrixIdentity();
}

_tagMatrix::_tagMatrix(const _tagMatrix & _m)
{
	m = _m.m;
}

_tagMatrix::_tagMatrix(const XMMATRIX & _m)
{
	m = _m;
}

void _tagMatrix::operator=(const _tagMatrix & _m)
{
	m = _m.m;
}

void _tagMatrix::operator=(const XMMATRIX & _m)
{
	m = _m;
}

void _tagMatrix::operator=(float _f[4][4])
{
	for (int i = 0; i < 4; ++i)
	{
		v[i] = _f[i];
	}
}


_tagMatrix _tagMatrix::operator*(const _tagMatrix & _m)	const
{
	return _tagMatrix(m * _m.m);
}

_tagMatrix _tagMatrix::operator*(const XMMATRIX & _m)	const
{
	return _tagMatrix(m * _m);
}

_tagMatrix _tagMatrix::operator*(float f)	const
{
	return _tagMatrix(m * f);
}

_tagMatrix _tagMatrix::operator*(int i)	const
{
	return _tagMatrix(m * (float)i);
}

_tagMatrix & _tagMatrix::operator*=(const _tagMatrix & _m)
{
	m *= _m.m;
	return *this;
}

_tagMatrix & _tagMatrix::operator*=(const XMMATRIX & _m)
{
	m *= _m;
	return *this;
}

_tagMatrix & _tagMatrix::operator*=(float f)
{
	m *= f;
	return *this;
}

_tagMatrix & _tagMatrix::operator*=(int i)
{
	m *= (float)i;
	return *this;
}

XMMATRIX _tagMatrix::Identity()
{
	m = XMMatrixIdentity();
	return m;
}

XMMATRIX _tagMatrix::Transpose()
{
	m = XMMatrixTranspose(m);
	return m;
}

XMMATRIX _tagMatrix::Inverse()
{
	m = XMMatrixInverse(&XMMatrixDeterminant(m), m);
	return m;
}

XMMATRIX _tagMatrix::Scaling(float x, float y, float z)
{
	m = XMMatrixScaling(x, y, z);
	return m;
}

XMMATRIX _tagMatrix::Scaling(const _tagVector3 & vScale)
{
	m = XMMatrixScaling(vScale.x, vScale.y, vScale.z);
	return m;
}

XMMATRIX _tagMatrix::Rotation(float x, float y, float z)
{
	XMMATRIX	matRotX, matRotY, matRotZ;
	matRotX = XMMatrixRotationX(DegreeToRadian(x));
	matRotY = XMMatrixRotationY(DegreeToRadian(y));
	matRotZ = XMMatrixRotationZ(DegreeToRadian(z));
	m = matRotX * matRotY * matRotZ;
	return m;
}

XMMATRIX _tagMatrix::Rotation(const _tagVector3 & vRot)
{
	XMMATRIX	matRotX, matRotY, matRotZ;
	matRotX = XMMatrixRotationX(DegreeToRadian(vRot.x));
	matRotY = XMMatrixRotationY(DegreeToRadian(vRot.y));
	matRotZ = XMMatrixRotationZ(DegreeToRadian(vRot.z));
	m = matRotX * matRotY * matRotZ;
	return m;
}

XMMATRIX _tagMatrix::RotationX(float x)
{
	m = XMMatrixRotationX(DegreeToRadian(x));
	return m;
}

XMMATRIX _tagMatrix::RotationY(float y)
{
	m = XMMatrixRotationY(DegreeToRadian(y));
	return m;
}

XMMATRIX _tagMatrix::RotationZ(float z)
{
	m = XMMatrixRotationZ(DegreeToRadian(z));
	return m;
}

XMMATRIX _tagMatrix::RotationAxis(float fAngle,
	const _tagVector3 & vAxis)
{
	m = XMMatrixRotationAxis(vAxis.Convert(), DegreeToRadian(fAngle));
	return m;
}

XMMATRIX _tagMatrix::Translation(float x, float y, float z)
{
	m = XMMatrixTranslation(x, y, z);
	return m;
}

XMMATRIX _tagMatrix::Translation(const _tagVector3 & vPos)
{
	m = XMMatrixTranslation(vPos.x, vPos.y, vPos.z);
	return m;
}

void * _tagMatrix::operator new(size_t iSize)
{
	return _aligned_malloc(iSize, 16);
}

void _tagMatrix::operator delete(void * p)
{
	_aligned_free(p);
}

Vector4 & _tagMatrix::operator[](unsigned int idx)
{
	return v[idx];
}

_tagMatrix & _tagMatrix::operator+=(const _tagMatrix & _m)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	for (int i = 0; i < 4; ++i)
	{
		v[i] += _m.v[i];
	}

	return *this;
}
