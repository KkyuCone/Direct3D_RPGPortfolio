#include "Inventory.h"
#include "Scene\Scene.h"
#include "Scene\Layer.h"
#include "Device.h"
#include "Time.h"
#include "GameObject.h"
#include "InputManager.h"

Inventory::Inventory() :
	m_pInventoryBGObject(nullptr)
	, m_pCloseButtonObject(nullptr)
	, m_pCloseButton(nullptr)
	, m_pInventoryBGImage(nullptr)
	, m_pScrollBarBGObject(nullptr)
	, m_pScrollBarBGImage(nullptr)
	, m_pScrollBarObject(nullptr)
	, m_pScrollBarButton(nullptr)
	, m_pScrollBarUpObject(nullptr)
	, m_pScrollBarUpButton(nullptr)
	, m_pScrollBarDownObject(nullptr)
	, m_pScrollBarDownButton(nullptr)
{
}

Inventory::Inventory(const Inventory & _Inventory) : Base(_Inventory)
{
	*this = _Inventory;
	m_pLayer = _Inventory.m_pLayer;
	m_iReferenceCount = 1;
}


Inventory::~Inventory()
{
	SAFE_RELEASE(m_pInventoryBGObject);
	SAFE_RELEASE(m_pInventoryBGImage);

	SAFE_RELEASE(m_pCloseButton);
	SAFE_RELEASE(m_pCloseButtonObject);

	// 스크롤
	SAFE_RELEASE(m_pScrollBarBGObject);
	SAFE_RELEASE(m_pScrollBarBGImage);

	SAFE_RELEASE(m_pScrollBarObject);
	SAFE_RELEASE(m_pScrollBarButton);

	SAFE_RELEASE(m_pScrollBarUpObject);
	SAFE_RELEASE(m_pScrollBarUpButton);

	SAFE_RELEASE(m_pScrollBarDownObject);
	SAFE_RELEASE(m_pScrollBarDownButton);
}

void Inventory::Start()
{
	// 인벤토리 배경
	m_pInventoryBGObject = GameObject::CreateObject("InventoryBG", m_pLayer);
	m_pInventoryBGImage = m_pInventoryBGObject->AddComponent<UIBar>("InventoryBG");
	m_pInventoryBGImage->SetTexture("HPBar", TEXT("Inventory_BG_Coin.png"), PATH_UI_INVENTORY);
	m_pInventoryBGImage->SetShader(SHADER_UI_BAR);

	Transform* pInvenBGTR = m_pInventoryBGObject->GetTransform();
	pInvenBGTR->SetWorldPosition(_RESOLUTION.iWidth * 0.5f, _RESOLUTION.iHeight * 0.5f, 1.0f);
	pInvenBGTR->SetWorldScale(583.5f, 378.5f, 1.0f);

	Vector3 vInvenBasePos = pInvenBGTR->GetWorldPosition();								// 인벤토리 기본 위치

	SAFE_RELEASE(pInvenBGTR);

	// 인벤토리 닫기 버튼
	m_pCloseButtonObject = GameObject::CreateObject("InvenetoryCloseButton", m_pLayer);
	//m_pCloseButtonObject->SetParent(m_pGameObject);
	m_pCloseButton = m_pCloseButtonObject->AddComponent<UIButton>("InventoryCloseButton");
	m_pCloseButton->SetStateTexture(BS_NORMAL, "UI_InvenCloseNormal", TEXT("c_header_close.png"), PATH_UI_INVENTORY);
	m_pCloseButton->SetStateTexture(BS_CLICK, "UI_InvenCloseClick", TEXT("c_header_close-PUSH.png"), PATH_UI_INVENTORY);
	m_pCloseButton->SetStateTexture(BS_MOUSEON, "UI_InvenCloseMouseON", TEXT("c_header_close-HOVER.png"), PATH_UI_INVENTORY);

	Transform* pInvenCloseButtonTR = m_pCloseButtonObject->GetTransform();
	pInvenCloseButtonTR->SetWorldPosition(_RESOLUTION.iWidth * 0.5f + 546.0f, _RESOLUTION.iHeight * 0.5f + 342.0f, 0.8f);
	pInvenCloseButtonTR->SetWorldScale(29.0f, 29.0f, 1.0f);

	SAFE_RELEASE(pInvenCloseButtonTR);

	// 인벤토리 스크롤
	// 스크롤 -  바 배경
	m_pScrollBarBGObject = GameObject::CreateObject("Inventory_ScrollBarBG", m_pLayer);
	m_pScrollBarBGImage = m_pScrollBarBGObject->AddComponent<UIBar>("Inventory_ScrollBarBG");
	m_pScrollBarBGImage->SetTexture("Inventory_ScrollBarBG", TEXT("scroll_bar.png"), PATH_UI_SCROLLBAR);
	m_pScrollBarBGImage->SetShader(SHADER_UI_BAR);

	Transform* pScrollBarBGTR = m_pScrollBarBGObject->GetTransform();
	pScrollBarBGTR->SetWorldPosition(_RESOLUTION.iWidth * 0.5f + 559.0f, _RESOLUTION.iHeight * 0.5f + 32.0f, 0.8f);
	pScrollBarBGTR->SetWorldScale(14.0f, 305.0f, 1.0f);
	SAFE_RELEASE(pScrollBarBGTR);

	// 스크롤 - 위(버튼)
	m_pScrollBarUpObject = GameObject::CreateObject("Inventory_ScrollBar_UpButton", m_pLayer);
	m_pScrollBarUpButton = m_pScrollBarUpObject->AddComponent<UIButton>("Inventory_ScrollBar_UpButton");
	m_pScrollBarUpButton->SetStateTexture(BS_NORMAL, "UI_ScrollBar_UpButton_Normal", TEXT("scroll_top_arrow.png"), PATH_UI_SCROLLBAR);
	m_pScrollBarUpButton->SetStateTexture(BS_CLICK, "UI_ScrollBar_UpButton_Click", TEXT("scroll_top_arrow-HOVER.png"), PATH_UI_SCROLLBAR);
	m_pScrollBarUpButton->SetStateTexture(BS_MOUSEON, "UI_ScrollBar_UpButton_MouseON", TEXT("scroll_top_arrow-HOVER.png"), PATH_UI_SCROLLBAR);
	m_pScrollBarUpButton->SetStateColor(BS_CLICK, Vector4(0.6f, 0.6f, 0.6f, 1.0f));

	Transform* pScrollBarUpTR = m_pScrollBarUpObject->GetTransform();
	pScrollBarUpTR->SetWorldPosition(_RESOLUTION.iWidth * 0.5f + 556.0f, _RESOLUTION.iHeight * 0.5f + 316.0f, 0.6f);
	pScrollBarUpTR->SetWorldScale(22.0f, 22.0f, 1.0f);
	SAFE_RELEASE(pScrollBarUpTR);

	// 스크롤 - 아래(버튼)
	m_pScrollBarDownObject = GameObject::CreateObject("Inventory_ScrollBar_DownButton", m_pLayer);
	m_pScrollBarDownButton = m_pScrollBarDownObject->AddComponent<UIButton>("Inventory_ScrollBar_DownButton");
	m_pScrollBarDownButton->SetStateTexture(BS_NORMAL, "UI_ScrollBar_DownButton_Normal", TEXT("scroll_bot_arrow.png"), PATH_UI_SCROLLBAR);
	m_pScrollBarDownButton->SetStateTexture(BS_CLICK, "UI_ScrollBar_DownButton_Click", TEXT("scroll_bot_arrow-HOVER.png"), PATH_UI_SCROLLBAR);
	m_pScrollBarDownButton->SetStateTexture(BS_MOUSEON, "UI_ScrollBar_DownButton_MouseON", TEXT("scroll_bot_arrow-HOVER.png"), PATH_UI_SCROLLBAR);
	m_pScrollBarDownButton->SetStateColor(BS_CLICK, Vector4(0.6f, 0.6f, 0.6f, 1.0f));

	Transform* pScrollBarDownTR = m_pScrollBarDownObject->GetTransform();
	pScrollBarDownTR->SetWorldPosition(_RESOLUTION.iWidth * 0.5f + 556.0f, _RESOLUTION.iHeight * 0.5f + 32.0f, 0.6f);
	pScrollBarDownTR->SetWorldScale(22.0f, 22.0f, 1.0f);
	SAFE_RELEASE(pScrollBarDownTR);

	// 스크롤 - 바
	m_pScrollBarObject = GameObject::CreateObject("Inventory_ScrollBar", m_pLayer);
	m_pScrollBarButton = m_pScrollBarObject->AddComponent<UIButton>("Inventory_ScrollBar");
	m_pScrollBarButton->SetStateTexture(BS_NORMAL, "UI_ScrollBar_Normal", TEXT("scroll_handler.png"), PATH_UI_SCROLLBAR);
	m_pScrollBarButton->SetStateTexture(BS_CLICK, "UI_ScrollBar_Click", TEXT("scroll_handler-HOVER.png"), PATH_UI_SCROLLBAR);
	m_pScrollBarButton->SetStateTexture(BS_MOUSEON, "UI_ScrollBar_MouseON", TEXT("scroll_handler-HOVER.png"), PATH_UI_SCROLLBAR);
	m_pScrollBarButton->SetStateColor(BS_CLICK, Vector4(0.6f, 0.6f, 0.6f, 1.0f));

	Transform* pScrollBarTR = m_pScrollBarObject->GetTransform();
	pScrollBarTR->SetWorldPosition(_RESOLUTION.iWidth * 0.5f + 556.0f, _RESOLUTION.iHeight * 0.5f + 245.0f, 0.6f);				// 세로  52.0f ~ 245.0f
	pScrollBarTR->SetWorldScale(22.0f, 73.0f, 1.0f);
	SAFE_RELEASE(pScrollBarTR);

	// 인벤토리 타일 생성
	vInvenBasePos.x += 10.0f;
	vInvenBasePos.y += 275.0f;
	vInvenBasePos.z = 0.7f;

	Vector3 vInventoryPos = vInvenBasePos;

	m_pItemObjects.resize(9 * 5);

	for (int y = 0; y < 5; ++y)
	{
		vInventoryPos = vInvenBasePos;
		float fYPos = y * 60.5f;
		vInventoryPos.y -= fYPos;

		for (int x = 0; x < 9; ++x)
		{
			vInventoryPos.x = vInvenBasePos.x;
			float fXPos = x * 61.0f;
			vInventoryPos.x += fXPos;

			// 인벤토리 해당칸
			InventoryType* pNewInven = new InventoryType;
			m_pItemObjects[(3 * y) + x] = pNewInven;
			//int Numbr = x + (y * 3);

			// 기본 인벤토리 이미지 생성
			GameObject* pInvenEmptyObj = GameObject::CreateObject("InvenEmpty", m_pLayer);
			UIButton* pInvenEmptyImage = pInvenEmptyObj->AddComponent<UIButton>("InvenEmptyImage");
			pInvenEmptyImage->SetStateTexture(BS_NORMAL, "InvenEmptyImage_NORMAL", TEXT("Inventory_NORMAL.png"), PATH_UI_INVENTORY);
			pInvenEmptyImage->SetStateTexture(BS_MOUSEON, "InvenEmptyImage_MOUSEON", TEXT("Inventory_MouseON.png"), PATH_UI_INVENTORY);
			pInvenEmptyImage->SetStateTexture(BS_CLICK, "InvenEmptyImage_MOUSEON", TEXT("Inventory_MouseON.png"), PATH_UI_INVENTORY);
			pInvenEmptyImage->SetStateColor(BS_CLICK, Vector4(0.6f, 0.6f, 0.6f, 1.0f));
			//pInvenEmptyImage->SetShader(SHADER_UI_BAR);
			Transform*	pInvenEmptyTr = pInvenEmptyObj->GetTransform();
			pInvenEmptyTr->SetWorldPosition(vInventoryPos.x, vInventoryPos.y, 0.7f);
			pInvenEmptyTr->SetWorldScale(61.0f, 60.5f, 1.0f);

			pNewInven->pIventoryEmptyObj = pInvenEmptyObj;
			pNewInven->pInvenEmptyImage = pInvenEmptyImage;
			SAFE_RELEASE(pInvenEmptyTr);
			SAFE_RELEASE(pInvenEmptyImage);
			SAFE_RELEASE(pInvenEmptyObj);

			// 아이템 이미지
			GameObject* pInvenItemObj = GameObject::CreateObject("InvenEdge", m_pLayer);
			UIButton* pInvenItemImage = pInvenItemObj->AddComponent<UIButton>("InvenEdgeImage");
			pInvenItemImage->SetStateTexture(BS_NORMAL, "InvenEdgeImage_Normal", TEXT("Inventory_MouseON_Empty.png"), PATH_UI_INVENTORY);
			pInvenItemImage->SetStateTexture(BS_MOUSEON, "InvenEdgeImage_MouseON", TEXT("Inventory_MouseON.png"), PATH_UI_INVENTORY);
			pInvenItemImage->SetStateTexture(BS_CLICK, "InvenEdgeImage_Empty", TEXT("Inventory_MouseON.png"), PATH_UI_INVENTORY);
			pInvenItemImage->SetStateColor(BS_CLICK, Vector4(1.0f, 1.0f, 1.0f, 0.7f));
			Transform*	pInvenItemTr = pInvenItemObj->GetTransform();
			pInvenItemTr->SetWorldPosition(vInventoryPos.x, vInventoryPos.y, 0.6f);
			pInvenItemTr->SetWorldScale(61.0f, 60.5f, 1.0f);

			pNewInven->pInventoryItemObj = pInvenItemObj;
			pNewInven->pInventoryItemButton = pInvenItemImage;
			SAFE_RELEASE(pInvenItemTr);
			SAFE_RELEASE(pInvenItemImage);
			SAFE_RELEASE(pInvenItemObj);

			SAFE_DELETE(pNewInven);
		}
	}
}

bool Inventory::Init()
{
	return true;
}

int Inventory::Input(float _fTime)
{
	return 0;
}

int Inventory::Update(float _fTime)
{
	return 0;
}

int Inventory::LateUpdate(float _fTime)
{

	return 0;
}

int Inventory::Collision(float _fTime)
{
	return 0;
}

int Inventory::PrevRender(float _fTime)
{
	return 0;
}

int Inventory::Render(float _fTime)
{
	return 0;
}

Inventory * Inventory::Clone() const
{
	return new Inventory(*this);
}

void Inventory::InventoryEnable(bool _Enable)
{
	if (true == _Enable)
	{
		// 인벤토리 켜기
	}
	else
	{
		// 인벤토리 끄기
	}
}

void Inventory::InventoryOff()
{
}

void Inventory::InventoryOn()
{
}
