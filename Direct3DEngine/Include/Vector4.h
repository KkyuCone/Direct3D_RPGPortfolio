#pragma once

#include "EngineMath.h"

ENGINE_BEGIN

typedef union ENGINE_DLL _tagVector4
{
	struct
	{
		float x, y, z, w;
	};
	struct
	{
		float r, g, b, a;
	};

	_tagVector4();
	_tagVector4(const _tagVector4& _v);
	_tagVector4(const XMVECTOR& _v);
	_tagVector4(const XMFLOAT4& _v);
	_tagVector4(int _ix, int _iy, int _iz, int _iw);
	_tagVector4(float _fx, float _fy, float _fz, float _fw);
	_tagVector4(float _fValue);
	_tagVector4(int _iValue);
	_tagVector4(float _fArrValue[4]);
	_tagVector4(int _iArrValue[4]);

	//[]
	float& operator [] (unsigned int _idx);


	// =
	void operator = (const _tagVector4& _v);
	void operator = (const XMVECTOR& _v);
	void operator = (const XMFLOAT4& _v);
	void operator = (float _fValue);
	void operator = (int _iValue);
	void operator = (float _fArrValue[4]);
	void operator = (int _iArrValue[4]);

	// +

	_tagVector4 operator + (const _tagVector4& _v);
	_tagVector4 operator + (const XMVECTOR& _v);
	_tagVector4 operator + (const XMFLOAT4& _v);
	_tagVector4 operator + (float _fValue);
	_tagVector4 operator + (int _iValue);
	_tagVector4 operator + (float _fArrValue[4]);
	_tagVector4 operator + (int _iArrValue[4]);

	// +=

	void operator += (const _tagVector4& _v);
	void operator += (const XMVECTOR& _v);
	void operator += (const XMFLOAT4& _v);
	void operator += (float _fValue);
	void operator += (int _iValue);
	void operator += (float _fArrValue[4]);
	void operator += (int _iArrValue[4]);

	// - 
	_tagVector4 operator -(const _tagVector4& _v);
	_tagVector4 operator -(const XMVECTOR& _v);
	_tagVector4 operator -(const XMFLOAT4& _v);
	_tagVector4 operator -(float _fValue);
	_tagVector4 operator -(int _iValue);
	_tagVector4 operator -(float _fArrValue[4]);
	_tagVector4 operator -(int _iArrValue[4]);

	// -=

	void operator -= (const _tagVector4& _v);
	void operator -= (const XMVECTOR& _v);
	void operator -= (const XMFLOAT4& _v);
	void operator -= (float _fValue);
	void operator -= (int _iValue);
	void operator -= (float _fArrValue[4]);
	void operator -= (int _iArrValue[4]);

	// *
	_tagVector4 operator * (const _tagVector4& _v);
	_tagVector4 operator * (const XMVECTOR& _v);
	_tagVector4 operator * (const XMFLOAT4& _v);
	_tagVector4 operator * (float _fValue);
	_tagVector4 operator * (int _iValue);
	_tagVector4 operator * (float _fArrValue[4]);
	_tagVector4 operator * (int _iArrValue[4]);

	// *=
	void operator *= (const _tagVector4& _v);
	void operator *= (const XMVECTOR& _v);
	void operator *= (const XMFLOAT4& _v);
	void operator *= (float _fValue);
	void operator *= (int _iValue);
	void operator *= (float _fArrValue[4]);
	void operator *= (int _iArrValue[4]);

	// /(³ª´©±â)
	_tagVector4 operator / (const _tagVector4& _v);
	_tagVector4 operator / (const XMVECTOR& _v);
	_tagVector4 operator / (const XMFLOAT4& _v);
	_tagVector4 operator / (float _fValue);
	_tagVector4 operator / (int _iValue);
	_tagVector4 operator / (float _fArrValue[4]);
	_tagVector4 operator / (int _iArrValue[4]);

	// /=
	void operator /= (const _tagVector4& _v);
	void operator /= (const XMVECTOR& _v);
	void operator /= (const XMFLOAT4& _v);
	void operator /= (float _fValue);
	void operator /= (int _iValue);
	void operator /= (float _fArrValue[4]);
	void operator /= (int _iArrValue[4]);

	// ==
	bool operator == (const _tagVector4& _v) const;
	bool operator == (const XMVECTOR& _v) const;
	bool operator == (const XMFLOAT4& _v) const;
	bool operator == (float _fValue) const;
	bool operator == (int _iValue) const;
	bool operator == (float _fArrValue[4]) const;
	bool operator == (int _iArrValue[4]) const;

	// !=
	bool operator != (const _tagVector4& _v) const;
	bool operator != (const XMVECTOR& _v) const;
	bool operator != (const XMFLOAT4& _v) const;
	bool operator != (float _fValue) const;
	bool operator != (int _iValue) const;
	bool operator != (float _fArrValue[4]) const;
	bool operator != (int _iArrValue[4]) const;


	//function
	XMVECTOR Convert() const;
	void Convert(const XMVECTOR& _v);
	static _tagVector4 Normalize(const _tagVector4& _v);
	static _tagVector4 Zero;
	static _tagVector4 One;
	static _tagVector4 White;
	static _tagVector4 Red;
	static _tagVector4 Green;
	static _tagVector4 Blue;
	static _tagVector4 Black;
	static _tagVector4 Yellow;
	static _tagVector4 Magenta;

}Vector4, *PVector4;

ENGINE_END
