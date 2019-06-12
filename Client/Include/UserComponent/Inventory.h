#pragma once
//#include "Component\UserComponent.h"
#include "Component\Renderer.h"
#include "Component\Transform.h"
#include "Component\Material.h"
#include "Component\Animation.h"
#include "Component\UIButton.h"
#include "Component\UIBar.h"
#include "Base.h"

ENGINE_USING

struct InventoryType
{
	class GameObject* pIventoryEmptyObj;
	UIButton*		pInvenEmptyImage;
	class GameObject* pInventoryItemObj;
	UIButton*		pInventoryItemButton;
	ITEM_TYPE	eItemType;
	int			iCount;

	InventoryType()
	{
		pIventoryEmptyObj = nullptr;
		pInvenEmptyImage = nullptr;

		pInventoryItemObj = nullptr;
		pInventoryItemButton = nullptr;

		eItemType = ITEM_TYPE::ITEM_NONE;
		iCount = 0;
	}
};

class Inventory : public Base
{
private:
	// ���
	GameObject* m_pInventoryBGObject;
	UIBar*	m_pInventoryBGImage;

	//UI ��ư�� (�ݱ� ��ư������)
	GameObject* m_pCloseButtonObject;
	UIButton*	m_pCloseButton;

	// ��ũ�� �� 
	GameObject* m_pScrollBarBGObject;				// �� ����̹���
	UIBar*		m_pScrollBarBGImage;

	GameObject*	m_pScrollBarObject;
	UIButton*	m_pScrollBarButton;

	GameObject*	m_pScrollBarUpObject;
	UIButton*	m_pScrollBarUpButton;

	GameObject*	m_pScrollBarDownObject;
	UIButton*	m_pScrollBarDownButton;

	// ������ĭ�� 
	std::vector<InventoryType*> m_pItemObjects;

private:


public:
	Inventory();
	Inventory(const Inventory& _Inventory);
	~Inventory();

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);
	virtual int Render(float _fTime);
	virtual Inventory* Clone()	const;

public:
	void InventoryEnable(bool _Enable);
	void InventoryOff();
	void InventoryOn();

};

