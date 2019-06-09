#pragma once
#include "Component\UserComponent.h"

ENGINE_USING

enum BaseState
{
	NONE = -1,
	IDLE = 0,
	WALK,
	RUN,
	WAIT,
	TALK,
	DANCE_Q,
	DEATH,
	DEATHWAIT,
	KNOCKBACK,
	GROGGY,
	TUMBLING,
	SKILL01,
	SKILL02,
	SKILL03,
	SKILL04,
	SKILL05,
	ATK01,							// ����
	ATK02,
	ATK03,
	ATK04,
	ATK05,
	COMBO1,							// �÷��̾�
	COMBO1R,
	COMBO2,
	COMBO2R,
	COMBO3,
	COMBO3R,
	COMBO4,
	MAX,
};

struct BaseUIState
{
	int m_iLevel;
	int m_iHP;
	int m_iMP;
	int m_iCurEXP;
	int m_iMaxEXP;
};

struct CurCoolTime
{
	float m_fCurCoolTime01;
	float m_fCurCoolTime02;
	float m_fCurCoolTime03;
	float m_fCurCoolTime04;
	float m_fCurCoolTime05;
};

// ������ Ÿ��
enum ITEM_TYPE
{
	NONE = 0,
	POTION_HP,
	POTION_MP,
};

enum SKILL_TYPE
{
	NONE =0,
};

struct EnventoryType
{
	ITEM_TYPE eItemType;
	SKILL_TYPE eSkillType;

	EnventoryType()
	{
		eItemType = ITEM_TYPE::NONE;
		eSkillType = SKILL_TYPE::NONE;
	}
};


struct QuickSlotType
{
	ITEM_TYPE eItemType;
	SKILL_TYPE eSkillType;

	QuickSlotType()
	{
		eItemType = ITEM_TYPE::NONE;
		eSkillType = SKILL_TYPE::NONE;
	}
};

class Base : public UserComponent
{
public:
	Base();
	Base(const Base& _Base);
	~Base();

public:
	virtual void Start();				// ��ġ�ɷ��� ���̾ ���Եɶ� (��ġ�ɶ�) ȣ��, �� ���̾�� ȣ�����ش�.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// ������ �ϱ� ������ ȣ�����ִ� �Լ��Ĥ���
	virtual int Render(float _fTime);
	virtual Base* Clone()	const;
};

