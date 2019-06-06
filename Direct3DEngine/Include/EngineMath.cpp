#include "EngineMath.h"

float RadianToDegree(float fRadian)
{
	return fRadian / ENGINE_PI * 180.f;
}

float DegreeToRadian(float fDegree)
{
	return fDegree / 180.f * ENGINE_PI;
}

