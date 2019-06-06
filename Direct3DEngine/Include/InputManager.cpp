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

	// 피킹용 마우스오브젝트 만들기
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

	// Axis, Action map 지우기
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


	// 지울때는 꼭 Unacquire먼저하고 Release한다.
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
	// 값은 항상 -120 or 120으로 들어온다. (휠 위쪽(+), 아래쪽(-) )
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
	// Direct Input 초기화 ( 일단 키보드만했음)

	// DirectInput8Create()를 이용해서 DirectInput 인터페이스를 초기화함.
	// Direct Input 객체를 얻게되면 다른 입력 장치들을 초기화할 수 있음
	if (FAILED(DirectInput8Create(_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&m_pInput, nullptr)))
	{
		return false;
	}

	// 키보드 초기화하기
	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
	{
		return false;
	}

	// SetDataFormat는 해당 장치에 대한 데이터넣기
	if (FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		return false;
	}

	// 마우스
	if (FAILED(m_pInput->CreateDevice(GUID_SysMouse, &m_pMouseDevice, nullptr)))
	{
		return false;
	}

	if (FAILED(m_pMouseDevice->SetDataFormat(&c_dfDIMouse2)))
	{
		return false;
	}

	// 조이스틱
	//if (FAILED(m_pInput->CreateDevice(GUID_Joystick, &m_pJoyStick, nullptr)))
	//{
	//	return false;
	//}

	//if (FAILED(m_pJoyStick->SetDataFormat(&c_dfDIJoystick)))
	//{
	//	return false;
	//}

	// 얘 안쓰는골 ㅇㅁㅇ 이거쓰려면 겁나 ㅇㅁ 많이 추가해야됨
	//AddActionKey("MouseRButtron", DIK_RBUTTON, 0, MB_RBUTTON);

	return true;
}

void InputManager::Update(float _fTime)
{
	// 해당 장치들에 대해서 눌렸는지 읽어온다.
	ReadKeyboard();
	ReadMouse();
	ReadJoyStick();

	// 읽은 후에 각 함수를 호출할지 검사 -> 호출  or 마우스 좌표 등 검사
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

// Axis찾기 string 키값으로
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
	// 키보드 상태를 m_Key에 저장함
	// GetDeviceState( 사이즈(or크기), 해당키상태를 넣을 변수)
	// 해당상태키는 눌러졌는지 아닌지를 저장한다.
	// 안눌렀으면 모두 null값인듯
	HRESULT result = m_pKeyboard->GetDeviceState(256, m_Key);
	
	// 키보드 상태를 읽는데 실패한 경우
	if (FAILED(result))
	{
		// 아래는 즉 키보드가 포커스를 잃거나 취득 불가 상태인 경우를 말한다.
		// DIERR_INPUTLOST : 입력처리를 잃어버린 경우
		// DIERR_NOTACQUIRED : 입력처리를 제어하지 못한경우
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
		{
			// SetCooperativeLevel (협력레벨)
			// : 이 장치가 무엇을 하는지, 어떻게 사용될 것인지 결정  -> 아래의 두개씩... 둘중 하나는 무조건 설정해야한다. 
			// DISCL_EXCLUSIVE : 다른 프로그램들과 공유하지 않는다. (배제 상태)
			// DISCL_NONEXCLUSIVE : 다른 프로그램들에서도 입력 접근 가능
			// DISCL_BACKGROUND  : 응용프로그램이 활성화중이거나 비활성중이든 뭐든 입력장치 사용 가능
			// DISCL_FOREGROUND  : 응용프로그램이 활성화 중일때만 입력장치 사용 가능

			if (FAILED(m_pKeyboard->SetCooperativeLevel(WINDOWHANDLE, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
			{
				return false;
			}

			// 단순히 연결처리가 안된것이므로 연결해준다.    
			//	Acquire는 해당 장치(현재는 키보드)에 접근이 가능하게 하는 함수
			m_pKeyboard->Acquire();
		}
		else
		{
			// 연결이 안된것 외에는 아에 실패한 경우이다.
			return false;
		}
	}

	return true;
}

bool InputManager::ReadMouse()
{
	HRESULT result = m_pMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &m_tMouseState);

	// 키보드 상태를 읽는데 실패한 경우
	if (FAILED(result))
	{
		// 아래는 즉 키보드가 포커스를 잃거나 취득 불가 상태인 경우를 말한다.
		// DIERR_INPUTLOST : 입력처리를 잃어버린 경우
		// DIERR_NOTACQUIRED : 입력처리를 제어하지 못한경우
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
		{
			if (FAILED(m_pMouseDevice->SetCooperativeLevel(WINDOWHANDLE, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
			{
				return false;
			}

			// 단순히 연결처리가 안된것이므로 연결해준다.
			m_pMouseDevice->Acquire();
		}
		else
		{
			// 연결이 안된것 외에는 아에 실패한 경우이다.
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
	// 등록된 키 개수만큼만 돌린다. ( ㅇㅅㅇ 더빠름!!) 
	for (int i = 0; i < m_iAddKeySize; ++i)
	{
		// 1000 0000 0x80 -> Down
		unsigned char cKey = m_AddKeyList[i];		// 해당 키 가져오기

		if (m_Key[cKey] & 0x80)
		{
			if (false == m_tKeyInfo[cKey].bPress && false == m_tKeyInfo[cKey].bPush)
			{
				// 키는 눌렀는데 만약 상태가 누른 상태, 또는 눌러지고 있는 상태가 아니라면 바꿔준다.
				// 이때 상황은 한번 눌러진 상태임
				m_tKeyInfo[cKey].bPress = true;
				m_tKeyInfo[cKey].bPush = true;
			}
			else
			{
				// 누르고 있는 상태 -> 이전프레임에도 누르고 있고 
				m_tKeyInfo[cKey].bPress = false;
			}
		}
		else if (true == m_tKeyInfo[cKey].bPress || true == m_tKeyInfo[cKey].bPush)
		{
			// 뗀상태
			m_tKeyInfo[cKey].bPress = false;
			m_tKeyInfo[cKey].bPush = false;
			m_tKeyInfo[cKey].bRelease = true;
		}
		else if (true == m_tKeyInfo[cKey].bRelease)
		{
			// 손떼고있는 상태
			m_tKeyInfo[cKey].bRelease = false;
		}
	}
}

void InputManager::UpdateMouse(float _fTime)
{
	// DIMOUSESTATE2 는 8개의 마우스 입력 버튼이 존재한다. (요즘 마우스꺼)
	for (int i = 0; i < 8; ++i)
	{
		if (m_tMouseState.rgbButtons[i] & 0x80)
		{
			if (false == m_tMouseInfo[i].bPress && false == m_tMouseInfo[i].bPush)
			{
				// 처음 눌렀을때
				m_tMouseInfo[i].bPress = true;
				m_tMouseInfo[i].bPush = true;
			}
			else 
			{
				// 누르고 있는 상태
				m_tMouseInfo[i].bPress = false;
			}
		}
		else if (true == m_tMouseInfo[i].bPress || true == m_tMouseInfo[i].bPush)
		{
			// 막 뗀 상태
			m_tMouseInfo[i].bPress = false;
			m_tMouseInfo[i].bPush = false;
			m_tMouseInfo[i].bRelease = true;
		}
		else if ( true == m_tMouseInfo[i].bRelease)
		{
			m_tMouseInfo[i].bRelease = false;
		}
	}

	// 마우스 좌표 구하기
	POINT	ptMouse;
	GetCursorPos(&ptMouse);						// 전체화면을 기준으로 좌표를 받아옴 
	ScreenToClient(WINDOWHANDLE, &ptMouse);		// 전제화면의 좌표를 현재 클라이언트 기준으로 좌표를 받아옴

	Vector2 vMousePos = {};
	vMousePos.x = (float)(ptMouse.x);
	vMousePos.y = (float)(ptMouse.y);

	Vector2 vUIMousePos = {};
	vUIMousePos.x = (float)(ptMouse.x);
	vUIMousePos.y = (float)(_RESOLUTION.iHeight) - (float)(ptMouse.y);

	m_vWindowMouseMove = vMousePos - m_vWindowMousePos;
	m_vWindowMousePos = vMousePos;

	Vector2 vRatio = GET_SINGLETON(Device)->GetWindowToViewportRatio();

	// 뷰포트용이니까 비율을 곱해준다.( 맞춰주려고, 안맞춰주면 일반 클라이언트상태와 똑같으니까 안맞음)
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

			// 해당키를 눌렀다면
			if (m_Key[(*StartList)->cKey] & 0x80)
			{
				fScale = (*StartList)->fScale;
			}

			// 연결된 함수가 있다면 다음을 실행한다.
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
	// FK : Function Key (기능키(조합) = 컨트롤 , 쉬프트, 알트 )
	const  unsigned char cFKKey[FK_END] = { DIK_LCONTROL, DIK_LALT, DIK_LSHIFT };
	bool bFKKeyState[FK_END] = {};			// 키 상태는 일단 0 으로 초기화

	// 조합키가 눌렸는지 검사 ( 조합키들만 검사하면 된다. )
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

	// 등록된 조합키에 대한 정보들 검사.. (함수호출여부)
	for (; ActionStart != ActionEnd; ++ActionStart)
	{
		// 조합키 검사 (눌렸는지 )
		std::list<PActionKeyInfo>::iterator ActionKeyListStart = ActionStart->second->KeyList.begin();
		std::list<PActionKeyInfo>::iterator ActionKeyListEnd = ActionStart->second->KeyList.end();

		bool bFunctionCall[KS_END] = {};			// 해당 키상태에 따른 함수 호출 여부

		for (; ActionKeyListStart != ActionKeyListEnd; ++ActionKeyListStart)
		{
			bool bFKeyEnable = true;

			for (int i = 0; i < FK_END; ++i)
			{
				// 조합키가 존재하는 경우
				if (true == (*ActionKeyListStart)->bFKKey[i])
				{
					// 조합키가 안눌렸을 떄
					if (false == bFKKeyState[i])
					{
						bFKeyEnable = false;
						break;
					}
				}
				else			// 해당 조합키가 존재하지 않을 경우..
				{
					// 조합키가 없는데 .. 조합키를 눌렀을 경우)
					if (true == bFKKeyState[i])
					{
						bFKeyEnable = false;
						break;
					}
				}
			}

			// 마우스 조합이 설정 X 경우 ( 순수하게 조합키로만 ..이뤄진애들만 함수호출 )
			if (MB_NONE == (*ActionKeyListStart)->eMouseType)
			{
				if (true == m_tKeyInfo[(*ActionKeyListStart)->cKey].bPress && true == bFKeyEnable)
				{
					// 해당키를 눌렀고, 조합키도 누른 경우 -> Press O  (처음 누른 상태임)
					(*ActionKeyListStart)->tKeyInfo.bPress = true;
					(*ActionKeyListStart)->tKeyInfo.bPush = true;

					bFunctionCall[KS_PRESS] = true;						// 조합키를 눌렀을때의 함수호출 O
				}
				else if (true == m_tKeyInfo[(*ActionKeyListStart)->cKey].bPush && true == bFKeyEnable)
				{
					// 해당키를 계속 누르고 있는 상태 -> Push O
					(*ActionKeyListStart)->tKeyInfo.bPress = false;
					(*ActionKeyListStart)->tKeyInfo.bPush = true;
					bFunctionCall[KS_PUSH] = true;
				}
				else if (true == (*ActionKeyListStart)->tKeyInfo.bPush &&
					(false == bFKeyEnable || true == m_tKeyInfo[(*ActionKeyListStart)->cKey].bRelease))
				{
					// 해당 조합키를 눌렀다가 뗀 상태 -> Release
					(*ActionKeyListStart)->tKeyInfo.bPush = false;
					(*ActionKeyListStart)->tKeyInfo.bRelease = true;
					bFunctionCall[KS_RELEASE] = true;
				}
				else if (true ==  (*ActionKeyListStart)->tKeyInfo.bRelease )
				{
					// 그냉 떼고있는상태 (아무상태 X)
					(*ActionKeyListStart)->tKeyInfo.bRelease = false;
				}
			}
		}

		// 연결된 함수 호출하기
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


// 키 등록
void InputManager::AddAxisKey(const std::string & _strName, 
	unsigned char _cKey, float _fScale)
{
	PBindAxisInfo pBind = FindAxis(_strName);

	// 없으면 만들어서 등록
	if (nullptr == pBind)
	{
		pBind = new BindAxisInfo;
		pBind->strName = _strName;
		m_mapAxis.insert(std::make_pair(_strName, pBind));
	}

	// 해당 PBindAxisInfo에 키가 이미 등록되어있는지 검사한다. (있으면 등록할 필요 없음)
	std::list<PKeyScale>::iterator StartIter	 = pBind->KeyList.begin();
	std::list<PKeyScale>::iterator EndIter		 = pBind->KeyList.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		// 이미 해당 키가 등록되어있으면 return
		if ((*StartIter)->cKey == _cKey)
		{
			return;
		}
	}

	PKeyScale pKeyScale = new KeyScale;
	pKeyScale->cKey = _cKey;
	pKeyScale->fScale = _fScale;

	// 해당 키가 이미 전체 키 리스트에 등록되어있는지 판단
	bool bFind = false;

	for (int i = 0; i < m_iAddKeySize; ++i)
	{
		if (m_AddKeyList[i] == _cKey)
		{
			bFind = true;
			break;
		}
	}

	// 등록이 안되어있다면 추가
	if (false == bFind)
	{
		m_AddKeyList[m_iAddKeySize] = _cKey;
		++m_iAddKeySize;
	}

	pBind->KeyList.push_back(pKeyScale);
}

// 얘는 마우스까지 ㅇㅁㅇ (조합키)  // _iFKey얘 조합키임
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

	// 조합키 -> 존재여부..(enum에 들어있는것만 처리)
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

	// 현재 키가 이미 등록된 키인지 판단 ( 있으면 더 추가할 필요가 없으니까)
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
