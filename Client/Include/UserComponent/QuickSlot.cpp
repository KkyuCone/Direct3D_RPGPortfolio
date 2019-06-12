#include "QuickSlot.h"
#include "Scene\Scene.h"
#include "Scene\Layer.h"
#include "Device.h"
#include "Time.h"
#include "GameObject.h"
#include "InputManager.h"


QuickSlot::QuickSlot() :
	m_pQuickSlotBGObj(nullptr)
	, m_pQuickSlotBG(nullptr)
{
}

QuickSlot::QuickSlot(const QuickSlot & _QuickSlot) : Base(_QuickSlot)
{
	*this = _QuickSlot;
	m_pLayer = _QuickSlot.m_pLayer;
	m_iReferenceCount = 1;
}


QuickSlot::~QuickSlot()
{
	SAFE_RELEASE(m_pQuickSlotBG);
	SAFE_RELEASE(m_pQuickSlotBGObj);
}

void QuickSlot::Start()
{
	m_pQuickSlotBGObj = GameObject::CreateObject("QuickSlot_BG", m_pLayer);

	m_pQuickSlotBG = m_pQuickSlotBGObj->AddComponent<UIBar>("QuickSlot_BG");
	m_pQuickSlotBG->SetTexture("QuickSlot_BG", TEXT("ab2_main_frame.png"), PATH_UI_GAGEBAR);
	m_pQuickSlotBG->SetShader(SHADER_UI_BAR);

	Transform* pQuickBGSlot = m_pQuickSlotBGObj->GetTransform();
	pQuickBGSlot->SetWorldScale(1162.2f, 139.8f, 1.0f);
	pQuickBGSlot->SetWorldPosition(_RESOLUTION.iWidth * 0.5f - 590.0f, _RESOLUTION.iHeight * 0.5f - 528.f, 1.4f);

	Vector3 vQuickSlotBasePos = pQuickBGSlot->GetWorldPosition();
	vQuickSlotBasePos.x += 74.6f;
	vQuickSlotBasePos.y += 37.0f;

	SAFE_RELEASE(m_pQuickSlotBG);
	SAFE_RELEASE(pQuickBGSlot);

	m_vecQuickSlots.resize(10);
	// 각 슬롯들 생성하기 1~0까지만, 나머지는 생성 X , 총 10개
	for (int i = 0; i < 10; ++i)
	{
		// 퀵슬롯 생성 후 넣어주기
		QuickSlotType* pNewQuickSlot = new QuickSlotType;
		m_vecQuickSlots[i] = pNewQuickSlot;

		Vector3 vNewQuickSlotPos = vQuickSlotBasePos;
		vNewQuickSlotPos.x += (76.22f* i);

		// 퀵슬롯에 필요한 애들 생성하기
		GameObject* pNewEmptyQuickObj = GameObject::CreateObject("InvenEmpty", m_pLayer);
		UIButton* pNewEmptyQuickImage = pNewEmptyQuickObj->AddComponent<UIButton>("InvenEmptyImage");
		pNewEmptyQuickImage->SetStateTexture(BS_NORMAL, "InvenEmptyImage_NORMAL", TEXT("Inventory_NORMAL.png"), PATH_UI_INVENTORY);
		pNewEmptyQuickImage->SetStateTexture(BS_MOUSEON, "InvenEmptyImage_MOUSEON", TEXT("Inventory_MouseON.png"), PATH_UI_INVENTORY);
		pNewEmptyQuickImage->SetStateTexture(BS_CLICK, "InvenEmptyImage_MOUSEON", TEXT("Inventory_MouseON.png"), PATH_UI_INVENTORY);
		pNewEmptyQuickImage->SetStateColor(BS_CLICK, Vector4(0.6f, 0.6f, 0.6f, 1.0f));
		//pInvenEmptyImage->SetShader(SHADER_UI_BAR);
		Transform*	pInvenEmptyTr = pNewEmptyQuickObj->GetTransform();
		pInvenEmptyTr->SetWorldPosition(vNewQuickSlotPos.x, vNewQuickSlotPos.y, 0.7f);
		pInvenEmptyTr->SetWorldScale(73.2f, 72.6f, 1.0f);

		pNewQuickSlot->pEmptyQuickSlotObject = pNewEmptyQuickObj;
		pNewQuickSlot->pEmptyQuickSlotButton = pNewEmptyQuickImage;
		SAFE_RELEASE(pInvenEmptyTr);
		SAFE_RELEASE(pNewEmptyQuickImage);
		SAFE_RELEASE(pNewEmptyQuickObj);

		// 퀵슬롯 번호 생성
		std::string strQuickSlotNum = "QuickSlotNum_";
		int iSlotNum = i;

		if (iSlotNum == 9)
		{
			iSlotNum = 0;
		}
		else
		{
			++iSlotNum;
		}

		std::string strSlotNum = IntToString(iSlotNum);
		strQuickSlotNum += strSlotNum;

		std::string strQuickSlotImageName = strQuickSlotNum;
		strQuickSlotImageName += ".png";

		std::wstring wstrTextureName = StringToLPWSTR(strQuickSlotImageName);

		GameObject* pNewQuickNumObj = GameObject::CreateObject("QuickSlotNum", m_pLayer);
		UIBar* pNewQuickNumImage = pNewQuickNumObj->AddComponent<UIBar>("QuickSlotNum");
		pNewQuickNumImage->SetTexture(strQuickSlotImageName, wstrTextureName.c_str(), PATH_UI_QUICKSLOT);
		pNewQuickNumImage->SetShader(SHADER_UI_BAR);

		vNewQuickSlotPos.x += 6.0f;
		vNewQuickSlotPos.y += 55.0f;
		Transform*	pNewQuickNumTr = pNewQuickNumObj->GetTransform();
		pNewQuickNumTr->SetWorldPosition(vNewQuickSlotPos.x, vNewQuickSlotPos.y, 0.3f);
		pNewQuickNumTr->SetWorldScale(11.0f, 13.0f, 1.0f);

		pNewQuickSlot->pQuickSlotNumObject = pNewQuickNumObj;
		pNewQuickSlot->pQuickSlotNumImage = pNewQuickNumImage;
		SAFE_RELEASE(pNewQuickNumTr);
		SAFE_RELEASE(pNewQuickNumImage);
		SAFE_RELEASE(pNewQuickNumObj);

		SAFE_DELETE(pNewQuickSlot);
	}
}

bool QuickSlot::Init()
{
	return true;
}

int QuickSlot::Input(float _fTime)
{
	return 0;
}

int QuickSlot::Update(float _fTime)
{
	return 0;
}

int QuickSlot::LateUpdate(float _fTime)
{
	return 0;
}

int QuickSlot::Collision(float _fTime)
{
	return 0;
}

int QuickSlot::PrevRender(float _fTime)
{
	return 0;
}

int QuickSlot::Render(float _fTime)
{
	return 0;
}

QuickSlot * QuickSlot::Clone() const
{
	return nullptr;
}
