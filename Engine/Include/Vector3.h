#pragma once

#include "EngineMath.h"

ENGINE_BEGIN

typedef union ENGINE_DLL _tagVector3
{
	struct
	{
		float	x, y, z;
	};

	struct
	{
		float	r, g, b;
	};

	_tagVector3();
	_tagVector3(const _tagVector3&dkslqslek);
	_tagVector3(const XMVECTOR& v);
	_tagVector3(const XMFLOAT3& v);
	_tagVector3(float _x, float _y, float _z);
	_tagVector3(int _x, int _y, int _z);
	_tagVector3(float f);
	_tagVector3(int i);
	_tagVector3(float f[3]);
	_tagVector3(int i[3]);

	// []
	float& operator [] (unsigned int idx);

	// =
	void operator = (const _tagVector3& v);
	void operator = (const XMVECTOR& v);
	void operator = (const XMFLOAT3& v);
	void operator = (float f);
	void operator = (int i);
	void operator = (float f[3]);
	void operator = (int i[3]);

	// +
	_tagVector3 operator + (const _tagVector3& v)	const;
	_tagVector3 operator + (const XMVECTOR& v)	const;
	_tagVector3 operator + (const XMFLOAT3& v)	const;
	_tagVector3 operator + (float f)	const;
	_tagVector3 operator + (int i)	const;
	_tagVector3 operator + (float f[3])	const;
	_tagVector3 operator + (int i[3])	const;

	// +=
	void operator += (const _tagVector3& v);
	void operator += (const XMVECTOR& v);
	void operator += (const XMFLOAT3& v);
	void operator += (float f);
	void operator += (int i);
	void operator += (float f[3]);
	void operator += (int i[3]);

	// -
	_tagVector3 operator - (const _tagVector3& v)	const;
	_tagVector3 operator - (const XMVECTOR& v)	const;
	_tagVector3 operator - (const XMFLOAT3& v)	const;
	_tagVector3 operator - (float f)	const;
	_tagVector3 operator - (int i)	const;
	_tagVector3 operator - (float f[3])	const;
	_tagVector3 operator - (int i[3])	const;

	// -=
	void operator -= (const _tagVector3& v);
	void operator -= (const XMVECTOR& v);
	void operator -= (const XMFLOAT3& v);
	void operator -= (float f);
	void operator -= (int i);
	void operator -= (float f[3]);
	void operator -= (int i[3]);

	// *
	_tagVector3 operator * (const _tagVector3& v)	const;
	_tagVector3 operator * (const XMVECTOR& v)	const;
	_tagVector3 operator * (const XMFLOAT3& v)	const;
	_tagVector3 operator * (float f)	const;
	_tagVector3 operator * (int i)	const;
	_tagVector3 operator * (float f[3])	const;
	_tagVector3 operator * (int i[3])	const;

	// *=
	void operator *= (const _tagVector3& v);
	void operator *= (const XMVECTOR& v);
	void operator *= (const XMFLOAT3& v);
	void operator *= (float f);
	void operator *= (int i);
	void operator *= (float f[3]);
	void operator *= (int i[3]);

	// /
	_tagVector3 operator / (const _tagVector3& v)	const;
	_tagVector3 operator / (const XMVECTOR& v)	const;
	_tagVector3 operator / (const XMFLOAT3& v)	const;
	_tagVector3 operator / (float f)	const;
	_tagVector3 operator / (int i)	const;
	_tagVector3 operator / (float f[3])	const;
	_tagVector3 operator / (int i[3])	const;

	// /=
	void operator /= (const _tagVector3& v);
	void operator /= (const XMVECTOR& v);
	void operator /= (const XMFLOAT3& v);
	void operator /= (float f);
	void operator /= (int i);
	void operator /= (float f[3]);
	void operator /= (int i[3]);

	// ==
	bool operator == (const _tagVector3& v)	const;
	bool operator == (const XMVECTOR& v)	const;
	bool operator == (const XMFLOAT3& v)	const;
	bool operator == (float f[3])	const;
	bool operator == (int i[3])	const;
	bool operator == (float f)	const;
	bool operator == (int i)	const;

	// !=
	bool operator != (const _tagVector3& v)	const;
	bool operator != (const XMVECTOR& v)	const;
	bool operator != (const XMFLOAT3& v)	const;
	bool operator != (float f[3])	const;
	bool operator != (int i[3])	const;
	bool operator != (float f)	const;
	bool operator != (int i)	const;

	// Function
	float Length()	const;
	float Distance(const _tagVector3& v)	const;
	void Normalize();
	float Dot(const _tagVector3& v)	const;
	float Dot(const XMVECTOR& v)	const;
	float Dot(const XMFLOAT3& v)	const;
	float Dot(float f[3])	const;
	float Dot(int i[3])	const;
	float Angle(const _tagVector3& v)	const;
	float Angle(const XMVECTOR& v)	const;
	float Angle(const XMFLOAT3& v)	const;
	float Angle(float f[3])	const;
	float Angle(int i[3])	const;
	_tagVector3 Cross(const _tagVector3& v)	const;
	_tagVector3 Cross(const XMVECTOR& v)	const;
	_tagVector3 Cross(const XMFLOAT3& v)	const;
	_tagVector3 Cross(float f[3])	const;
	_tagVector3 Cross(int i[3])	const;
	_tagVector3 TransformNormal(const union _tagMatrix& m);
	_tagVector3 TransformNormal(const XMMATRIX& m);
	_tagVector3 TransformCoord(const union _tagMatrix& m);
	_tagVector3 TransformCoord(const XMMATRIX& m);

	XMVECTOR Convert()	const;
	void Convert(const XMVECTOR& v);

	static _tagVector3 Normalize(const _tagVector3& v);

	static _tagVector3	Axis[3];
	static _tagVector3	Zero;
	static _tagVector3	One;
}Vector3, *PVector3;

ENGINE_END

