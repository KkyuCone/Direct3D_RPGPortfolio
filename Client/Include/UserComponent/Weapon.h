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
	// ��
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
	virtual void Start();				// ��ġ�ɷ��� ���̾ ���Եɶ� (��ġ�ɶ�) ȣ��, �� ���̾�� ȣ�����ش�.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// ������ �ϱ� ������ ȣ�����ִ� �Լ��Ĥ���
	virtual int Render(float _fTime);
	virtual Weapon* Clone()	const;

private:
	void ComponentInit();

};

