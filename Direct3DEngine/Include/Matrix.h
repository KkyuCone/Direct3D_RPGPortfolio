#pragma once

#include "EngineMath.h"
#include "Vector4.h"

ENGINE_BEGIN

typedef union ENGINE_DLL __declspec(align(16)) _tagMatrix
{
	XMMATRIX	m;

	struct
	{
		float	_11, _12, _13, _14;
		float	_21, _22, _23, _24;
		float	_31, _32, _33, _34;
		float	_41, _42, _43, _44;
	};

	struct
	{
		Vector4	v[4];
	};

	_tagMatrix();
	_tagMatrix(const _tagMatrix& _m);
	_tagMatrix(const XMMATRIX& _m);

	void operator = (const _tagMatrix& _m);
	void operator = (const XMMATRIX& _m);
	void operator = (float _f[4][4]);

	Vector4& operator [] (unsigned int idx);

	// *
	_tagMatrix operator * (const _tagMatrix& _m)	const;
	_tagMatrix operator * (const XMMATRIX& _m)	const;
	_tagMatrix operator * (float f)	const;
	_tagMatrix operator * (int i)	const;

	// *=
	_tagMatrix& operator *= (const _tagMatrix& _m);
	_tagMatrix& operator *= (const XMMATRIX& _m);
	_tagMatrix& operator *= (float f);
	_tagMatrix& operator *= (int i);

	// +=
	_tagMatrix& operator += (const _tagMatrix& _m);

	void* operator new (size_t iSize);
	void operator delete (void* p);

	XMMATRIX Identity();
	XMMATRIX Transpose();
	XMMATRIX Inverse();

	XMMATRIX Scaling(float x, float y, float z);
	XMMATRIX Scaling(const union _tagVector3& vScale);
	XMMATRIX Rotation(float x, float y, float z);
	XMMATRIX Rotation(const union _tagVector3& vRot);
	XMMATRIX RotationX(float x);
	XMMATRIX RotationY(float y);
	XMMATRIX RotationZ(float z);
	XMMATRIX RotationAxis(float fAngle, const union _tagVector3& vAxis);
	XMMATRIX Translation(float x, float y, float z);
	XMMATRIX Translation(const union _tagVector3& vPos);
}Matrix, *PMatrix;

ENGINE_END
