#pragma once
#include "EngineMath.h"

ENGINE_BEGIN

typedef union ENGINE_DLL _tagVector2
{
	struct
	{
		float x, y;
	};
	struct
	{
		float r, g;
	};

	_tagVector2();
	_tagVector2(const _tagVector2& _v);
	_tagVector2(const XMVECTOR& _v);
	_tagVector2(const XMFLOAT2& _v);
	_tagVector2(int _ix, int _iy);
	_tagVector2(float _fx, float _fy);
	_tagVector2(float _fValue);
	_tagVector2(int _iValue);
	_tagVector2(float _fArrValue[2]);
	_tagVector2(int _iArrValue[2]);

	//[]
	float& operator [] (unsigned int _idx);


	// =
	void operator = (const _tagVector2& _v);
	void operator = (const XMVECTOR& _v);
	void operator = (const XMFLOAT2& _v);
	void operator = (float _fValue);
	void operator = (int _iValue);
	void operator = (float _fArrValue[2]);
	void operator = (int _iArrValue[2]);

	// +

	_tagVector2 operator + (const _tagVector2& _v);
	_tagVector2 operator + (const XMVECTOR& _v);
	_tagVector2 operator + (const XMFLOAT2& _v);
	_tagVector2 operator + (float _fValue);
	_tagVector2 operator + (int _iValue);
	_tagVector2 operator + (float _fArrValue[2]);
	_tagVector2 operator + (int _iArrValue[2]);

	// +=

	void operator += (const _tagVector2& _v);
	void operator += (const XMVECTOR& _v);
	void operator += (const XMFLOAT2& _v);
	void operator += (float _fValue);
	void operator += (int _iValue);
	void operator += (float _fArrValue[2]);
	void operator += (int _iArrValue[2]);

	// - 
	_tagVector2 operator -(const _tagVector2& _v);
	_tagVector2 operator -(const XMVECTOR& _v);
	_tagVector2 operator -(const XMFLOAT2& _v);
	_tagVector2 operator -(float _fValue);
	_tagVector2 operator -(int _iValue);
	_tagVector2 operator -(float _fArrValue[2]);
	_tagVector2 operator -(int _iArrValue[2]);

	// -=

	void operator -= (const _tagVector2& _v);
	void operator -= (const XMVECTOR& _v);
	void operator -= (const XMFLOAT2& _v);
	void operator -= (float _fValue);
	void operator -= (int _iValue);
	void operator -= (float _fArrValue[2]);
	void operator -= (int _iArrValue[2]);

	// *
	_tagVector2 operator * (const _tagVector2& _v);
	_tagVector2 operator * (const XMVECTOR& _v);
	_tagVector2 operator * (const XMFLOAT2& _v);
	_tagVector2 operator * (float _fValue);
	_tagVector2 operator * (int _iValue);
	_tagVector2 operator * (float _fArrValue[2]);
	_tagVector2 operator * (int _iArrValue[2]);

	// *=
	void operator *= (const _tagVector2& _v);
	void operator *= (const XMVECTOR& _v);
	void operator *= (const XMFLOAT2& _v);
	void operator *= (float _fValue);
	void operator *= (int _iValue);
	void operator *= (float _fArrValue[2]);
	void operator *= (int _iArrValue[2]);

	// /(³ª´©±â)
	_tagVector2 operator / (const _tagVector2& _v);
	_tagVector2 operator / (const XMVECTOR& _v);
	_tagVector2 operator / (const XMFLOAT2& _v);
	_tagVector2 operator / (float _fValue);
	_tagVector2 operator / (int _iValue);
	_tagVector2 operator / (float _fArrValue[2]);
	_tagVector2 operator / (int _iArrValue[2]);

	// /=
	void operator /= (const _tagVector2& _v);
	void operator /= (const XMVECTOR& _v);
	void operator /= (const XMFLOAT2& _v);
	void operator /= (float _fValue);
	void operator /= (int _iValue);
	void operator /= (float _fArrValue[2]);
	void operator /= (int _iArrValue[2]);

	// ==
	bool operator == (const _tagVector2& _v) const;
	bool operator == (const XMVECTOR& _v) const;
	bool operator == (const XMFLOAT2& _v) const;
	bool operator == (float _fValue) const;
	bool operator == (int _iValue) const;
	bool operator == (float _fArrValue[2]) const;
	bool operator == (int _iArrValue[2]) const;

	// !=
	bool operator != (const _tagVector2& _v) const;
	bool operator != (const XMVECTOR& _v) const;
	bool operator != (const XMFLOAT2& _v) const;
	bool operator != (float _fValue) const;
	bool operator != (int _iValue) const;
	bool operator != (float _fArrValue[2]) const;
	bool operator != (int _iArrValue[2]) const;


	//function
	float Length() const;
	XMVECTOR Convert() const;
	void Convert(const XMVECTOR& _v);
	static _tagVector2 Normalize(const _tagVector2& _v);
	static _tagVector2 Zero;
	static _tagVector2 One;

}Vector2, *PVector2;

ENGINE_END
