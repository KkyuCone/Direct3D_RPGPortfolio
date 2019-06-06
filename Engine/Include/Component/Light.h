#pragma once
#include "Component.h"

ENGINE_BEGIN

class ENGINE_DLL Light : public Component
{
private:
	friend class GameObject;

protected:
	Light();
	Light(const Light& _Component);
	~Light();

private:
	LightInfo	m_tInfo;
	float		m_fInAngle;			// 안쪽 반경 각도 (스폿라이트)
	float		m_fOutAngle;		// 바깥쪽 반경 각도 (스폿라이트)

public:
	LightInfo GetLightInfo()		const;

	float GetInAngle()	const;
	float GetOutAngle()	const;
	Vector4 GetDiffuseColor() const;
	Vector4 GetAmbientColor() const;

public:
	void SetLightType(LIGHT_TYPE _eType);
	void SetLightColor(const Vector4& _vDiffuse, const Vector4& _vAmbient, const Vector4& _vSpecular);
	void SetLightDistance(float _fDistance);
	void SetLightAngle(float _fInAngle, float _fOutAngle);

	void SetLightVP(Matrix _VP);

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출, 즉 레이어에서 호출해준다.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// 렌더를 하기 직전에 호출해주는 함수ㅔㄱㅁ
	virtual int Render(float _fTime);
	virtual Light* Clone()	const;

public:
	void SetShader();

};

ENGINE_END
