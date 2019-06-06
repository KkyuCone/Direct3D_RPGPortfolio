#pragma once
#include "Component\UserComponent.h"
#include "Component\Renderer.h"
#include "Component\Transform.h"
#include "Component\Material.h"
#include "Component\Animation.h"
#include "Component\Collider.h"

ENGINE_USING

class Weapon : public UserComponent
{
private:
	// 몸
	Transform* m_pWeaponTransform;
	Renderer* m_pWeaponRenderer;
	Material* m_pWeaponMaterial;

	bool m_bParentBone;
	std::string m_strParentBone;

public:
	Weapon();
	Weapon(const Weapon& _Weapon);
	~Weapon();

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출, 즉 레이어에서 호출해준다.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// 렌더를 하기 직전에 호출해주는 함수ㅔㄱㅁ
	virtual int Render(float _fTime);
	virtual Weapon* Clone()	const;

private:
	void ComponentInit();

};

