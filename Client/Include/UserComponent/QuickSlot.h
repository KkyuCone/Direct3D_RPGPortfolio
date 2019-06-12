#pragma once
#include "Base.h"
#include "Component\Renderer.h"
#include "Component\Transform.h"
#include "Component\Material.h"
#include "Component\Animation.h"
#include "Component\UIButton.h"
#include "Component\UIBar.h"
#include "Component\UIText.h"

ENGINE_USING

struct QuickSlotType
{
	// ºó ½½·Ô
	GameObject* pEmptyQuickSlotObject;
	UIButton*	pEmptyQuickSlotButton;

	// ¾ÆÀÌÅÛ OR ½ºÅ³ ½½·Ô
	GameObject* pQuickSlotObject;
	UIButton*	pQuickSlotButton;

	// Äü½½·Ô ¹øÈ£ (Å°)
	GameObject* pQuickSlotNumObject;
	UIBar*		pQuickSlotNumImage;

	ITEM_TYPE	eItemType;
	SKILL_TYPE	eSkillType;
	int			iSlotNumber;					// ÇØ´ç ½½·Ô Å° (Å°º¸µå Å°)
	bool		bCoolTime;
	float		fCurCoolTime;
	float		fCoolTime;
	int			iCount;

	QuickSlotType()
	{
		pEmptyQuickSlotObject = nullptr;
		pEmptyQuickSlotButton = nullptr;
		pQuickSlotObject = nullptr;
		pQuickSlotButton = nullptr;
		pQuickSlotNumObject = nullptr;
		pQuickSlotNumImage = nullptr;
		eItemType = ITEM_TYPE::ITEM_NONE;
		eSkillType = SKILL_TYPE::SKILL_NONE;
		iSlotNumber = 0;
		bCoolTime = false;
		fCurCoolTime = 0.0f;
		fCoolTime = 0.0f;
		iCount = 0;
	}
};

class QuickSlot : public Base
{
private:
	GameObject* m_pQuickSlotBGObj;
	UIBar* m_pQuickSlotBG;

	std::vector<QuickSlotType*> m_vecQuickSlots;

public:
	QuickSlot();
	QuickSlot(const QuickSlot& _QuickSlot);
	~QuickSlot();

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);
	virtual int Render(float _fTime);
	virtual QuickSlot* Clone()	const;
};

