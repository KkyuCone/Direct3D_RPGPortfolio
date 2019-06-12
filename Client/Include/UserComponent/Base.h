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
	ATK01,							// 몬스터
	ATK02,
	ATK03,
	ATK04,
	ATK05,
	COMBO1,							// 플레이어
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

// 아이템 타입
enum ITEM_TYPE
{
	ITEM_NONE = 0,
	POTION_HP,
	POTION_MP,
};

enum SKILL_TYPE
{
	SKILL_NONE =0,
	SKILL_01,
	SKILL_02,
	SKILL_03,
	SKILL_04,
	SKILL_05,
	CUTTING_SLASH = SKILL_01,
	STINGER_BLADE = SKILL_02,
	RISING_ATTCK = SKILL_03,
	WIND_CUTTER1 = SKILL_04,
	WIND_CUTTER5 = SKILL_05,
};

//struct InventoryType
//{
//	ITEM_TYPE eItemType;
//	SKILL_TYPE eSkillType;
//
//	InventoryType()
//	{
//		eItemType = ITEM_TYPE::ITEM_NONE;
//		eSkillType = SKILL_TYPE::SKILL_NONE;
//	}
//};


//struct QuickSlotType
//{
//	ITEM_TYPE	eItemType;
//	SKILL_TYPE	eSkillType;
//	bool		bCoolTime;
//	float		fCoolTime;
//	int			iCount;
//
//	QuickSlotType()
//	{
//		eItemType = ITEM_TYPE::ITEM_NONE;
//		eSkillType = SKILL_TYPE::SKILL_NONE;
//		bCoolTime = false;
//		fCoolTime = 0.0f;
//		iCount = -1;
//	}
//};

class Base : public UserComponent
{
public:
	Base();
	Base(const Base& _Base);
	~Base();

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출, 즉 레이어에서 호출해준다.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// 렌더를 하기 직전에 호출해주는 함수ㅔㄱㅁ
	virtual int Render(float _fTime);
	virtual Base* Clone()	const;
};

