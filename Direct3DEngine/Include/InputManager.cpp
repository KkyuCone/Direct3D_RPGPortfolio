#include "InputManager.h"
#include "Core.h"
#include "Device.h"
#include "GameObject.h"
#include "Component\Renderer.h"
#include "Component\Material.h"
#include "Component\ColliderRay.h"
#include "Component\ColliderPoint.h"

ENGINE_USING

DEFINITION_SINGLETON(InputManager)

InputManager::InputManager() :
	m_pInput(nullptr), 
	m_pKeyboard(nullptr),
	m_pMouseDevice(nullptr),
	/*m_pJoyStick(nullptr),*/
	m_sWheel(0),
	m_iAddKeySize(0)
{
	memset(m_AddKeyList, 0, sizeof(unsigned char) * 256);
	memset(&m_tMouseState, 0, sizeof(m_tMouseState));

	// 杷天遂 原酔什神崎詮闘 幻級奄
	m_pMouseObj = GameObject::CreateObject("Mouse");
	m_pMouseRay = m_pMouseObj->AddComponent<ColliderRay>("MouseRay");
	m_pMouseRay->PickEnable(true);
	m_pMouseRay->MouseEnable(true);
	m_pMouseRay->CollisionManagerEnable(false);
}


InputManager::~InputManager()
{
	SAFE_RELEASE(m_pMouseRay);
	SAFE_RELEASE(m_pMouseObj);

	// Axis, Action map 走酔奄
	std::unordered_map<std::string, PBindAxisInfo>::iterator AxisStart = m_mapAxis.begin();
	std::unordered_map<std::string, PBindAxisInfo>::iterator AxisEnd = m_mapAxis.end();

	for (; AxisStart != AxisEnd; ++AxisStart)
	{
		Safe_Delete_VectorList(AxisStart->second->KeyList);
		SAFE_DELETE(AxisStart->second);
	}

	std::unordered_map<std::string, PBindActionInfo>::iterator ActionStart = m_mapAction.begin();
	std::unordered_map<std::string, PBindActionInfo>::iterator ActionEnd = m_mapAction.end();

	for (; ActionStart != ActionEnd; ++ActionStart)
	{
		Safe_Delete_VectorList(ActionStart->second->KeyList);
		SAFE_DELETE(ActionStart->second);
	}


	// 走随凶澗 伽 Unacquire胡煽馬壱 Release廃陥.
	if (nullptr != m_pMouseDevice)
	{
		m_pMouseDevice->Unacquire();
		SAFE_RELEASE(m_pMouseDevice);
	}

	if (nullptr != m_pKeyboard)
	{
		m_pKeyboard->Unacquire();
		SAFE_RELEASE(m_pKeyboard);
	}

	/*if (nullptr != m_pJoyStick)
	{
		m_pJoyStick->Unacquire();
		SAFE_RELEASE(m_pJoyStick);
	}*/


	SAFE_RELEASE(m_pInput);
}

void InputManager::SetWheel(short _sWheel)
{
	// 葵精 牌雌 -120 or 120生稽 級嬢紳陥. (蕃 是楕(+), 焼掘楕(-) )
	m_sWheel = _sWheel / 120;
}

short InputManager::GetWheelDir() const
{
	return m_sWheel;
}

bool InputManager::MousePress(MOUSE_BUTTON _eMouseButton)
{
	return m_tMouseInfo[_eMouseButton].bPress;
}

bool InputManager::MousePush(MOUSE_BUTTON _eMouseButton)
{
	return m_tMouseInfo[_eMouseButton].bPush;
}

bool InputManager::MouseRelease(MOUSE_BUTTON _eMouseButton)
{
	return m_tMouseInfo[_eMouseButton].bRelease;
}

Vector2 InputManager::GetMouseWindowPos() const
{
	return m_vWindowMousePos;
}

Vector2 InputManager::GetMouseWindowMove() const
{
	return m_vWindowMouseMove;
}

Vector2 InputManager::GetMouseViewportPos() const
{
	return m_vViewportMousePos;
}

Vector2 InputManager::GetMouseViewportMove() const
{
	return m_vViewportMouseMove;
}

Vector2 InputManager::GetUIMouseViewportPos() const
{
	return m_vViewportUIMousePos;
}

Vector2 InputManager::GetUIMouseViewportMove() const
{
	return m_vViewportUIMouseMove;
}

bool InputManager::MouseRayEnable() const
{
	if (nullptr != m_pMouseRay)
	{
		return true;
	}

	return false;
}

ColliderRay * InputManager::GetMouseRay() const
{
	if (nullptr != m_pMouseRay)
	{
 		m_pMouseRay->AddReference();
	}

	return m_pMouseRay;
}

void InputManager::ChangeMouseScene(Scene * _pScene)
{
	m_pMouseObj->SetScene(_pScene);
}

bool InputManager::Init(HINSTANCE _hInst, HWND _hWnd)
{
	// Direct Input 段奄鉢 ( 析舘 徹左球幻梅製)

	// DirectInput8Create()研 戚遂背辞 DirectInput 昔斗凪戚什研 段奄鉢敗.
	// Direct Input 梓端研 条惟鞠檎 陥献 脊径 舌帖級聖 段奄鉢拝 呪 赤製
	if (FAILED(DirectInput8Create(_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&m_pInput, nullptr)))
	{
		return false;
	}

	// 徹左球 段奄鉢馬奄
	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
	{
		return false;
	}

	// SetDataFormat澗 背雁 舌帖拭 企廃 汽戚斗隔奄
	if (FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		return false;
	}

	// 原酔什
	if (FAILED(m_pInput->CreateDevice(GUID_SysMouse, &m_pMouseDevice, nullptr)))
	{
		return false;
	}

	if (FAILED(m_pMouseDevice->SetDataFormat(&c_dfDIMouse2)))
	{
		return false;
	}

	// 繕戚什峠
	//if (FAILED(m_pInput->CreateDevice(GUID_Joystick, &m_pJoyStick, nullptr)))
	//{
	//	return false;
	//}

	//if (FAILED(m_pJoyStick->SetDataFormat(&c_dfDIJoystick)))
	//{
	//	return false;
	//}

	// 剰 照床澗茨 しけし 戚暗床形檎 位蟹 しけ 弦戚 蓄亜背醤喫
	//AddActionKey("MouseRButtron", DIK_RBUTTON, 0, MB_RBUTTON);

	return true;
}

void InputManager::Update(float _fTime)
{
	// 背雁 舌帖級拭 企背辞 喚携澗走 石嬢紳陥.
	ReadKeyboard();
	ReadMouse();
	ReadJoyStick();

	// 石精 板拭 唖 敗呪研 硲窒拝走 伊紫 -> 硲窒  or 原酔什 疎妊 去 伊紫
	UpdateKeyboard(_fTime);
	UpdateMouse(_fTime);
	UpdateAxis(_fTime);
	UpdateAction(_fTime);

	m_pMouseObj->Update(_fTime);
	m_pMouseObj->LateUpdate(_fTime);
}

void InputManager::Render(float _fTime)
{
	m_sWheel = 0;
	m_pMouseObj->Render(_fTime);
}

// Axis達奄 string 徹葵生稽
PBindAxisInfo InputManager::FindAxis(const std::string & _strName)
{
	std::unordered_map<std::string, PBindAxisInfo>::iterator FIter = m_mapAxis.find(_strName);

	if (FIter == m_mapAxis.end())
	{
		return nullptr;
	}

	return FIter->second;
}

PBindActionInfo InputManager::FindAction(const std::string & _strName)
{
	std::unordered_map<std::string, PBindActionInfo>::iterator FIter = m_mapAction.find(_strName);

	if (FIter == m_mapAction.end())
	{
		return nullptr;
	}

	return FIter->second;
}

bool InputManager::ReadKeyboard()
{
	// 徹左球 雌殿研 m_Key拭 煽舌敗
	// GetDeviceState( 紫戚綜(or滴奄), 背雁徹雌殿研 隔聖 痕呪)
	// 背雁雌殿徹澗 喚君然澗走 焼観走研 煽舌廃陥.
	// 照喚袈生檎 乞砧 null葵昔牛
	HRESULT result = m_pKeyboard->GetDeviceState(256, m_Key);
	
	// 徹左球 雌殿研 石澗汽 叔鳶廃 井酔
	if (FAILED(result))
	{
		// 焼掘澗 聡 徹左球亜 匂朕什研 籍暗蟹 昼究 災亜 雌殿昔 井酔研 源廃陥.
		// DIERR_INPUTLOST : 脊径坦軒研 籍嬢獄鍵 井酔
		// DIERR_NOTACQUIRED : 脊径坦軒研 薦嬢馬走 公廃井酔
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
		{
			// SetCooperativeLevel (漠径傾婚)
			// : 戚 舌帖亜 巷譲聖 馬澗走, 嬢胸惟 紫遂吃 依昔走 衣舛  -> 焼掘税 砧鯵梢... 却掻 馬蟹澗 巷繕闇 竺舛背醤廃陥. 
			// DISCL_EXCLUSIVE : 陥献 覗稽益轡級引 因政馬走 省澗陥. (壕薦 雌殿)
			// DISCL_NONEXCLUSIVE : 陥献 覗稽益轡級拭辞亀 脊径 羨悦 亜管
			// DISCL_BACKGROUND  : 誓遂覗稽益轡戚 醗失鉢掻戚暗蟹 搾醗失掻戚窮 更窮 脊径舌帖 紫遂 亜管
			// DISCL_FOREGROUND  : 誓遂覗稽益轡戚 醗失鉢 掻析凶幻 脊径舌帖 紫遂 亜管

			if (FAILED(m_pKeyboard->SetCooperativeLevel(WINDOWHANDLE, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
			{
				return false;
			}

			// 舘授備 尻衣坦軒亜 照吉依戚糠稽 尻衣背層陥.    
			//	Acquire澗 背雁 舌帖(薄仙澗 徹左球)拭 羨悦戚 亜管馬惟 馬澗 敗呪
			m_pKeyboard->Acquire();
		}
		else
		{
			// 尻衣戚 照吉依 須拭澗 焼拭 叔鳶廃 井酔戚陥.
			return false;
		}
	}

	return true;
}

bool InputManager::ReadMouse()
{
	HRESULT result = m_pMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &m_tMouseState);

	// 徹左球 雌殿研 石澗汽 叔鳶廃 井酔
	if (FAILED(result))
	{
		// 焼掘澗 聡 徹左球亜 匂朕什研 籍暗蟹 昼究 災亜 雌殿昔 井酔研 源廃陥.
		// DIERR_INPUTLOST : 脊径坦軒研 籍嬢獄鍵 井酔
		// DIERR_NOTACQUIRED : 脊径坦軒研 薦嬢馬走 公廃井酔
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
		{
			if (FAILED(m_pMouseDevice->SetCooperativeLevel(WINDOWHANDLE, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
			{
				return false;
			}

			// 舘授備 尻衣坦軒亜 照吉依戚糠稽 尻衣背層陥.
			m_pMouseDevice->Acquire();
		}
		else
		{
			// 尻衣戚 照吉依 須拭澗 焼拭 叔鳶廃 井酔戚陥.
			return false;
		}
	}

	return true;
}

bool InputManager::ReadJoyStick()
{
	return false;
}

void InputManager::UpdateKeyboard(float _fTime)
{
	// 去系吉 徹 鯵呪幻鏑幻 宜鍵陥. ( しさし 希匙硯!!) 
	for (int i = 0; i < m_iAddKeySize; ++i)
	{
		// 1000 0000 0x80 -> Down
		unsigned char cKey = m_AddKeyList[i];		// 背雁 徹 亜閃神奄

		if (m_Key[cKey] & 0x80)
		{
			if (false == m_tKeyInfo[cKey].bPress && false == m_tKeyInfo[cKey].bPush)
			{
				// 徹澗 喚袈澗汽 幻鉦 雌殿亜 刊献 雌殿, 暁澗 喚君走壱 赤澗 雌殿亜 焼艦虞檎 郊蚊層陥.
				// 戚凶 雌伐精 廃腰 喚君遭 雌殿績
				m_tKeyInfo[cKey].bPress = true;
				m_tKeyInfo[cKey].bPush = true;
			}
			else
			{
				// 刊牽壱 赤澗 雌殿 -> 戚穿覗傾績拭亀 刊牽壱 赤壱 
				m_tKeyInfo[cKey].bPress = false;
			}
		}
		else if (true == m_tKeyInfo[cKey].bPress || true == m_tKeyInfo[cKey].bPush)
		{
			// 蕎雌殿
			m_tKeyInfo[cKey].bPress = false;
			m_tKeyInfo[cKey].bPush = false;
			m_tKeyInfo[cKey].bRelease = true;
		}
		else if (true == m_tKeyInfo[cKey].bRelease)
		{
			// 謝脅壱赤澗 雌殿
			m_tKeyInfo[cKey].bRelease = false;
		}
	}
}

void InputManager::UpdateMouse(float _fTime)
{
	// DIMOUSESTATE2 澗 8鯵税 原酔什 脊径 獄動戚 糎仙廃陥. (推葬 原酔什襖)
	for (int i = 0; i < 8; ++i)
	{
		if (m_tMouseState.rgbButtons[i] & 0x80)
		{
			if (false == m_tMouseInfo[i].bPress && false == m_tMouseInfo[i].bPush)
			{
				// 坦製 喚袈聖凶
				m_tMouseInfo[i].bPress = true;
				m_tMouseInfo[i].bPush = true;
			}
			else 
			{
				// 刊牽壱 赤澗 雌殿
				m_tMouseInfo[i].bPress = false;
			}
		}
		else if (true == m_tMouseInfo[i].bPress || true == m_tMouseInfo[i].bPush)
		{
			// 厳 蕎 雌殿
			m_tMouseInfo[i].bPress = false;
			m_tMouseInfo[i].bPush = false;
			m_tMouseInfo[i].bRelease = true;
		}
		else if ( true == m_tMouseInfo[i].bRelease)
		{
			m_tMouseInfo[i].bRelease = false;
		}
	}

	// 原酔什 疎妊 姥馬奄
	POINT	ptMouse;
	GetCursorPos(&ptMouse);						// 穿端鉢檎聖 奄層生稽 疎妊研 閤焼身 
	ScreenToClient(WINDOWHANDLE, &ptMouse);		// 穿薦鉢檎税 疎妊研 薄仙 適虞戚情闘 奄層生稽 疎妊研 閤焼身

	Vector2 vMousePos = {};
	vMousePos.x = (float)(ptMouse.x);
	vMousePos.y = (float)(ptMouse.y);

	// 銚芝, 左球 y滴奄
	RECT	ptClientSize;
	GetClientRect(WINDOWHANDLE, &ptClientSize);
	float fyCapBoSize = (float)(_RESOLUTION.iHeight) - ptClientSize.bottom;

	Vector2 vUIMousePos = {};
	vUIMousePos.x = (float)(ptMouse.x);
	vUIMousePos.y = (float)(_RESOLUTION.iHeight) - (float)(ptMouse.y) - fyCapBoSize;

	m_vWindowMouseMove = vMousePos - m_vWindowMousePos;
	m_vWindowMousePos = vMousePos;

	Vector2 vRatio = GET_SINGLETON(Device)->GetWindowToViewportRatio();

	// 坂匂闘遂戚艦猿 搾晴聖 咽背層陥.( 限仲爽形壱, 照限仲爽檎 析鋼 適虞戚情闘雌殿人 業旭生艦猿 照限製)
	vMousePos *= vRatio;
	m_vViewportMouseMove = vMousePos - m_vViewportMousePos;
	m_vViewportMousePos = vMousePos;

	vUIMousePos *= vRatio;
	m_vViewportUIMouseMove = vUIMousePos - m_vViewportUIMousePos;
	m_vViewportUIMousePos = vUIMousePos;
}

void InputManager::UpdateAxis(float _fTime)
{
	// Axis
	std::unordered_map<std::string, PBindAxisInfo>::iterator StartIter = m_mapAxis.begin();
	std::unordered_map<std::string, PBindAxisInfo>::iterator EndIter = m_mapAxis.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		std::list<PKeyScale>::iterator StartList = StartIter->second->KeyList.begin();
		std::list<PKeyScale>::iterator EndList = StartIter->second->KeyList.end();

		for (; StartList != EndList; ++StartList)
		{
			float fScale = 0.0f;

			// 背雁徹研 喚袈陥檎
			if (m_Key[(*StartList)->cKey] & 0x80)
			{
				fScale = (*StartList)->fScale;
			}

			// 尻衣吉 敗呪亜 赤陥檎 陥製聖 叔楳廃陥.
			if (true == StartIter->second->bFunctionBind)
			{
				StartIter->second->Func(fScale, _fTime);
			}
		}
	}
}

void InputManager::UpdateAction(float _fTime)
{
	// Action
	// FK : Function Key (奄管徹(繕杯) = 珍闘継 , 習覗闘, 硝闘 )
	const  unsigned char cFKKey[FK_END] = { DIK_LCONTROL, DIK_LALT, DIK_LSHIFT };
	bool bFKKeyState[FK_END] = {};			// 徹 雌殿澗 析舘 0 生稽 段奄鉢

	// 繕杯徹亜 喚携澗走 伊紫 ( 繕杯徹級幻 伊紫馬檎 吉陥. )
	for (size_t i = 0; i < FK_END; ++i)
	{
		if (m_Key[cFKKey[i]] & 0x80)
		{
			bFKKeyState[i] = true;
		}
		else 
		{
			bFKKeyState[i] = false;
		}
	}

	std::unordered_map<std::string, PBindActionInfo>::iterator ActionStart = m_mapAction.begin();
	std::unordered_map<std::string, PBindActionInfo>::iterator ActionEnd = m_mapAction.end();

	// 去系吉 繕杯徹拭 企廃 舛左級 伊紫.. (敗呪硲窒食採)
	for (; ActionStart != ActionEnd; ++ActionStart)
	{
		// 繕杯徹 伊紫 (喚携澗走 )
		std::list<PActionKeyInfo>::iterator ActionKeyListStart = ActionStart->second->KeyList.begin();
		std::list<PActionKeyInfo>::iterator ActionKeyListEnd = ActionStart->second->KeyList.end();

		bool bFunctionCall[KS_END] = {};			// 背雁 徹雌殿拭 魚献 敗呪 硲窒 食採

		for (; ActionKeyListStart != ActionKeyListEnd; ++ActionKeyListStart)
		{
			bool bFKeyEnable = true;

			for (int i = 0; i < FK_END; ++i)
			{
				// 繕杯徹亜 糎仙馬澗 井酔
				if (true == (*ActionKeyListStart)->bFKKey[i])
				{
					// 繕杯徹亜 照喚携聖 ��
					if (false == bFKKeyState[i])
					{
						bFKeyEnable = false;
						break;
					}
				}
				else			// 背雁 繕杯徹亜 糎仙馬走 省聖 井酔..
				{
					// 繕杯徹亜 蒸澗汽 .. 繕杯徹研 喚袈聖 井酔)
					if (true == bFKKeyState[i])
					{
						bFKeyEnable = false;
						break;
					}
				}
			}

			// 原酔什 繕杯戚 竺舛 X 井酔 ( 授呪馬惟 繕杯徹稽幻 ..戚件遭蕉級幻 敗呪硲窒 )
			if (MB_NONE == (*ActionKeyListStart)->eMouseType)
			{
				if (true == m_tKeyInfo[(*ActionKeyListStart)->cKey].bPress && true == bFKeyEnable)
				{
					// 背雁徹研 喚袈壱, 繕杯徹亀 刊献 井酔 -> Press O  (坦製 刊献 雌殿績)
					(*ActionKeyListStart)->tKeyInfo.bPress = true;
					(*ActionKeyListStart)->tKeyInfo.bPush = true;

					bFunctionCall[KS_PRESS] = true;						// 繕杯徹研 喚袈聖凶税 敗呪硲窒 O
				}
				else if (true == m_tKeyInfo[(*ActionKeyListStart)->cKey].bPush && true == bFKeyEnable)
				{
					// 背雁徹研 域紗 刊牽壱 赤澗 雌殿 -> Push O
					(*ActionKeyListStart)->tKeyInfo.bPress = false;
					(*ActionKeyListStart)->tKeyInfo.bPush = true;
					bFunctionCall[KS_PUSH] = true;
				}
				else if (true == (*ActionKeyListStart)->tKeyInfo.bPush &&
					(false == bFKeyEnable || true == m_tKeyInfo[(*ActionKeyListStart)->cKey].bRelease))
				{
					// 背雁 繕杯徹研 喚袈陥亜 蕎 雌殿 -> Release
					(*ActionKeyListStart)->tKeyInfo.bPush = false;
					(*ActionKeyListStart)->tKeyInfo.bRelease = true;
					bFunctionCall[KS_RELEASE] = true;
				}
				else if (true ==  (*ActionKeyListStart)->tKeyInfo.bRelease )
				{
					// 益鈎 脅壱赤澗雌殿 (焼巷雌殿 X)
					(*ActionKeyListStart)->tKeyInfo.bRelease = false;
				}
			}
		}

		// 尻衣吉 敗呪 硲窒馬奄
		if (true == bFunctionCall[KS_PRESS] && true == ActionStart->second->bFunctionBind[KS_PRESS])
		{
			ActionStart->second->Func[KS_PRESS](_fTime);
		}

		if (true == bFunctionCall[KS_PUSH] && true == ActionStart->second->bFunctionBind[KS_PUSH])
		{
			ActionStart->second->Func[KS_PUSH](_fTime);
		}

		if (true == bFunctionCall[KS_RELEASE] && true == ActionStart->second->bFunctionBind[KS_RELEASE])
		{
			ActionStart->second->Func[KS_RELEASE](_fTime);
		}
	}
}


// 徹 去系
void InputManager::AddAxisKey(const std::string & _strName, 
	unsigned char _cKey, float _fScale)
{
	PBindAxisInfo pBind = FindAxis(_strName);

	// 蒸生檎 幻級嬢辞 去系
	if (nullptr == pBind)
	{
		pBind = new BindAxisInfo;
		pBind->strName = _strName;
		m_mapAxis.insert(std::make_pair(_strName, pBind));
	}

	// 背雁 PBindAxisInfo拭 徹亜 戚耕 去系鞠嬢赤澗走 伊紫廃陥. (赤生檎 去系拝 琶推 蒸製)
	std::list<PKeyScale>::iterator StartIter	 = pBind->KeyList.begin();
	std::list<PKeyScale>::iterator EndIter		 = pBind->KeyList.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		// 戚耕 背雁 徹亜 去系鞠嬢赤生檎 return
		if ((*StartIter)->cKey == _cKey)
		{
			return;
		}
	}

	PKeyScale pKeyScale = new KeyScale;
	pKeyScale->cKey = _cKey;
	pKeyScale->fScale = _fScale;

	// 背雁 徹亜 戚耕 穿端 徹 軒什闘拭 去系鞠嬢赤澗走 毒舘
	bool bFind = false;

	for (int i = 0; i < m_iAddKeySize; ++i)
	{
		if (m_AddKeyList[i] == _cKey)
		{
			bFind = true;
			break;
		}
	}

	// 去系戚 照鞠嬢赤陥檎 蓄亜
	if (false == bFind)
	{
		m_AddKeyList[m_iAddKeySize] = _cKey;
		++m_iAddKeySize;
	}

	pBind->KeyList.push_back(pKeyScale);
}

// 剰澗 原酔什猿走 しけし (繕杯徹)  // _iFKey剰 繕杯徹績
void InputManager::AddActionKey(const std::string & _strName,
	unsigned char _cKey, int _iFKey,
	MOUSE_BUTTON _eMouseButton)
{
	PBindActionInfo pBind = FindAction(_strName);

	if (nullptr == pBind)
	{
		pBind = new BindActionInfo;
		pBind->strName = _strName;
		m_mapAction.insert(std::make_pair(_strName, pBind));
	}

	std::list<PActionKeyInfo>::iterator StartIter = pBind->KeyList.begin();
	std::list<PActionKeyInfo>::iterator EndIter = pBind->KeyList.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		if ((*StartIter)->cKey == _cKey)
		{
			return;
		}
	}

	PActionKeyInfo pActionKeyInfo = new ActionKeyInfo;

	pActionKeyInfo->cKey = _cKey;
	pActionKeyInfo->eMouseType = _eMouseButton;

	// 繕杯徹 -> 糎仙食採..(enum拭 級嬢赤澗依幻 坦軒)
	if (_iFKey & FKE_CTRL)
	{
		pActionKeyInfo->bFKKey[FK_CTRL] = true;
	}

	if (_iFKey & FKE_ALT)
	{
		pActionKeyInfo->bFKKey[FK_ALT] = true;
	}

	if (_iFKey & FKE_SHIFT)
	{
		pActionKeyInfo->bFKKey[FK_SHIFT] = true;
	}

	// 薄仙 徹亜 戚耕 去系吉 徹昔走 毒舘 ( 赤生檎 希 蓄亜拝 琶推亜 蒸生艦猿)
	bool bFind = false;

	for (int i = 0; i < m_iAddKeySize; ++i)
	{
		if (m_AddKeyList[i] == _cKey)
		{
			bFind = true;
			break;
		}
	}

	if (false == bFind)
	{
		m_AddKeyList[m_iAddKeySize] = _cKey;
		++m_iAddKeySize;
	}

	pBind->KeyList.push_back(pActionKeyInfo);

}
