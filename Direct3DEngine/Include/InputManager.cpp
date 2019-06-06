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

	// ��ŷ�� ���콺������Ʈ �����
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

	// Axis, Action map �����
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


	// ���ﶧ�� �� Unacquire�����ϰ� Release�Ѵ�.
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
	// ���� �׻� -120 or 120���� ���´�. (�� ����(+), �Ʒ���(-) )
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
	// Direct Input �ʱ�ȭ ( �ϴ� Ű���常����)

	// DirectInput8Create()�� �̿��ؼ� DirectInput �������̽��� �ʱ�ȭ��.
	// Direct Input ��ü�� ��ԵǸ� �ٸ� �Է� ��ġ���� �ʱ�ȭ�� �� ����
	if (FAILED(DirectInput8Create(_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&m_pInput, nullptr)))
	{
		return false;
	}

	// Ű���� �ʱ�ȭ�ϱ�
	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
	{
		return false;
	}

	// SetDataFormat�� �ش� ��ġ�� ���� �����ͳֱ�
	if (FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		return false;
	}

	// ���콺
	if (FAILED(m_pInput->CreateDevice(GUID_SysMouse, &m_pMouseDevice, nullptr)))
	{
		return false;
	}

	if (FAILED(m_pMouseDevice->SetDataFormat(&c_dfDIMouse2)))
	{
		return false;
	}

	// ���̽�ƽ
	//if (FAILED(m_pInput->CreateDevice(GUID_Joystick, &m_pJoyStick, nullptr)))
	//{
	//	return false;
	//}

	//if (FAILED(m_pJoyStick->SetDataFormat(&c_dfDIJoystick)))
	//{
	//	return false;
	//}

	// �� �Ⱦ��°� ������ �̰ž����� �̳� ���� ���� �߰��ؾߵ�
	//AddActionKey("MouseRButtron", DIK_RBUTTON, 0, MB_RBUTTON);

	return true;
}

void InputManager::Update(float _fTime)
{
	// �ش� ��ġ�鿡 ���ؼ� ���ȴ��� �о�´�.
	ReadKeyboard();
	ReadMouse();
	ReadJoyStick();

	// ���� �Ŀ� �� �Լ��� ȣ������ �˻� -> ȣ��  or ���콺 ��ǥ �� �˻�
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

// Axisã�� string Ű������
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
	// Ű���� ���¸� m_Key�� ������
	// GetDeviceState( ������(orũ��), �ش�Ű���¸� ���� ����)
	// �ش����Ű�� ���������� �ƴ����� �����Ѵ�.
	// �ȴ������� ��� null���ε�
	HRESULT result = m_pKeyboard->GetDeviceState(256, m_Key);
	
	// Ű���� ���¸� �дµ� ������ ���
	if (FAILED(result))
	{
		// �Ʒ��� �� Ű���尡 ��Ŀ���� �Ұų� ��� �Ұ� ������ ��츦 ���Ѵ�.
		// DIERR_INPUTLOST : �Է�ó���� �Ҿ���� ���
		// DIERR_NOTACQUIRED : �Է�ó���� �������� ���Ѱ��
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
		{
			// SetCooperativeLevel (���·���)
			// : �� ��ġ�� ������ �ϴ���, ��� ���� ������ ����  -> �Ʒ��� �ΰ���... ���� �ϳ��� ������ �����ؾ��Ѵ�. 
			// DISCL_EXCLUSIVE : �ٸ� ���α׷���� �������� �ʴ´�. (���� ����)
			// DISCL_NONEXCLUSIVE : �ٸ� ���α׷��鿡���� �Է� ���� ����
			// DISCL_BACKGROUND  : �������α׷��� Ȱ��ȭ���̰ų� ��Ȱ�����̵� ���� �Է���ġ ��� ����
			// DISCL_FOREGROUND  : �������α׷��� Ȱ��ȭ ���϶��� �Է���ġ ��� ����

			if (FAILED(m_pKeyboard->SetCooperativeLevel(WINDOWHANDLE, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
			{
				return false;
			}

			// �ܼ��� ����ó���� �ȵȰ��̹Ƿ� �������ش�.    
			//	Acquire�� �ش� ��ġ(����� Ű����)�� ������ �����ϰ� �ϴ� �Լ�
			m_pKeyboard->Acquire();
		}
		else
		{
			// ������ �ȵȰ� �ܿ��� �ƿ� ������ ����̴�.
			return false;
		}
	}

	return true;
}

bool InputManager::ReadMouse()
{
	HRESULT result = m_pMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &m_tMouseState);

	// Ű���� ���¸� �дµ� ������ ���
	if (FAILED(result))
	{
		// �Ʒ��� �� Ű���尡 ��Ŀ���� �Ұų� ��� �Ұ� ������ ��츦 ���Ѵ�.
		// DIERR_INPUTLOST : �Է�ó���� �Ҿ���� ���
		// DIERR_NOTACQUIRED : �Է�ó���� �������� ���Ѱ��
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
		{
			if (FAILED(m_pMouseDevice->SetCooperativeLevel(WINDOWHANDLE, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
			{
				return false;
			}

			// �ܼ��� ����ó���� �ȵȰ��̹Ƿ� �������ش�.
			m_pMouseDevice->Acquire();
		}
		else
		{
			// ������ �ȵȰ� �ܿ��� �ƿ� ������ ����̴�.
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
	// ��ϵ� Ű ������ŭ�� ������. ( ������ ������!!) 
	for (int i = 0; i < m_iAddKeySize; ++i)
	{
		// 1000 0000 0x80 -> Down
		unsigned char cKey = m_AddKeyList[i];		// �ش� Ű ��������

		if (m_Key[cKey] & 0x80)
		{
			if (false == m_tKeyInfo[cKey].bPress && false == m_tKeyInfo[cKey].bPush)
			{
				// Ű�� �����µ� ���� ���°� ���� ����, �Ǵ� �������� �ִ� ���°� �ƴ϶�� �ٲ��ش�.
				// �̶� ��Ȳ�� �ѹ� ������ ������
				m_tKeyInfo[cKey].bPress = true;
				m_tKeyInfo[cKey].bPush = true;
			}
			else
			{
				// ������ �ִ� ���� -> ���������ӿ��� ������ �ְ� 
				m_tKeyInfo[cKey].bPress = false;
			}
		}
		else if (true == m_tKeyInfo[cKey].bPress || true == m_tKeyInfo[cKey].bPush)
		{
			// ������
			m_tKeyInfo[cKey].bPress = false;
			m_tKeyInfo[cKey].bPush = false;
			m_tKeyInfo[cKey].bRelease = true;
		}
		else if (true == m_tKeyInfo[cKey].bRelease)
		{
			// �ն����ִ� ����
			m_tKeyInfo[cKey].bRelease = false;
		}
	}
}

void InputManager::UpdateMouse(float _fTime)
{
	// DIMOUSESTATE2 �� 8���� ���콺 �Է� ��ư�� �����Ѵ�. (���� ���콺��)
	for (int i = 0; i < 8; ++i)
	{
		if (m_tMouseState.rgbButtons[i] & 0x80)
		{
			if (false == m_tMouseInfo[i].bPress && false == m_tMouseInfo[i].bPush)
			{
				// ó�� ��������
				m_tMouseInfo[i].bPress = true;
				m_tMouseInfo[i].bPush = true;
			}
			else 
			{
				// ������ �ִ� ����
				m_tMouseInfo[i].bPress = false;
			}
		}
		else if (true == m_tMouseInfo[i].bPress || true == m_tMouseInfo[i].bPush)
		{
			// �� �� ����
			m_tMouseInfo[i].bPress = false;
			m_tMouseInfo[i].bPush = false;
			m_tMouseInfo[i].bRelease = true;
		}
		else if ( true == m_tMouseInfo[i].bRelease)
		{
			m_tMouseInfo[i].bRelease = false;
		}
	}

	// ���콺 ��ǥ ���ϱ�
	POINT	ptMouse;
	GetCursorPos(&ptMouse);						// ��üȭ���� �������� ��ǥ�� �޾ƿ� 
	ScreenToClient(WINDOWHANDLE, &ptMouse);		// ����ȭ���� ��ǥ�� ���� Ŭ���̾�Ʈ �������� ��ǥ�� �޾ƿ�

	Vector2 vMousePos = {};
	vMousePos.x = (float)(ptMouse.x);
	vMousePos.y = (float)(ptMouse.y);

	Vector2 vUIMousePos = {};
	vUIMousePos.x = (float)(ptMouse.x);
	vUIMousePos.y = (float)(_RESOLUTION.iHeight) - (float)(ptMouse.y);

	m_vWindowMouseMove = vMousePos - m_vWindowMousePos;
	m_vWindowMousePos = vMousePos;

	Vector2 vRatio = GET_SINGLETON(Device)->GetWindowToViewportRatio();

	// ����Ʈ���̴ϱ� ������ �����ش�.( �����ַ���, �ȸ����ָ� �Ϲ� Ŭ���̾�Ʈ���¿� �Ȱ����ϱ� �ȸ���)
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

			// �ش�Ű�� �����ٸ�
			if (m_Key[(*StartList)->cKey] & 0x80)
			{
				fScale = (*StartList)->fScale;
			}

			// ����� �Լ��� �ִٸ� ������ �����Ѵ�.
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
	// FK : Function Key (���Ű(����) = ��Ʈ�� , ����Ʈ, ��Ʈ )
	const  unsigned char cFKKey[FK_END] = { DIK_LCONTROL, DIK_LALT, DIK_LSHIFT };
	bool bFKKeyState[FK_END] = {};			// Ű ���´� �ϴ� 0 ���� �ʱ�ȭ

	// ����Ű�� ���ȴ��� �˻� ( ����Ű�鸸 �˻��ϸ� �ȴ�. )
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

	// ��ϵ� ����Ű�� ���� ������ �˻�.. (�Լ�ȣ�⿩��)
	for (; ActionStart != ActionEnd; ++ActionStart)
	{
		// ����Ű �˻� (���ȴ��� )
		std::list<PActionKeyInfo>::iterator ActionKeyListStart = ActionStart->second->KeyList.begin();
		std::list<PActionKeyInfo>::iterator ActionKeyListEnd = ActionStart->second->KeyList.end();

		bool bFunctionCall[KS_END] = {};			// �ش� Ű���¿� ���� �Լ� ȣ�� ����

		for (; ActionKeyListStart != ActionKeyListEnd; ++ActionKeyListStart)
		{
			bool bFKeyEnable = true;

			for (int i = 0; i < FK_END; ++i)
			{
				// ����Ű�� �����ϴ� ���
				if (true == (*ActionKeyListStart)->bFKKey[i])
				{
					// ����Ű�� �ȴ����� ��
					if (false == bFKKeyState[i])
					{
						bFKeyEnable = false;
						break;
					}
				}
				else			// �ش� ����Ű�� �������� ���� ���..
				{
					// ����Ű�� ���µ� .. ����Ű�� ������ ���)
					if (true == bFKKeyState[i])
					{
						bFKeyEnable = false;
						break;
					}
				}
			}

			// ���콺 ������ ���� X ��� ( �����ϰ� ����Ű�θ� ..�̷����ֵ鸸 �Լ�ȣ�� )
			if (MB_NONE == (*ActionKeyListStart)->eMouseType)
			{
				if (true == m_tKeyInfo[(*ActionKeyListStart)->cKey].bPress && true == bFKeyEnable)
				{
					// �ش�Ű�� ������, ����Ű�� ���� ��� -> Press O  (ó�� ���� ������)
					(*ActionKeyListStart)->tKeyInfo.bPress = true;
					(*ActionKeyListStart)->tKeyInfo.bPush = true;

					bFunctionCall[KS_PRESS] = true;						// ����Ű�� ���������� �Լ�ȣ�� O
				}
				else if (true == m_tKeyInfo[(*ActionKeyListStart)->cKey].bPush && true == bFKeyEnable)
				{
					// �ش�Ű�� ��� ������ �ִ� ���� -> Push O
					(*ActionKeyListStart)->tKeyInfo.bPress = false;
					(*ActionKeyListStart)->tKeyInfo.bPush = true;
					bFunctionCall[KS_PUSH] = true;
				}
				else if (true == (*ActionKeyListStart)->tKeyInfo.bPush &&
					(false == bFKeyEnable || true == m_tKeyInfo[(*ActionKeyListStart)->cKey].bRelease))
				{
					// �ش� ����Ű�� �����ٰ� �� ���� -> Release
					(*ActionKeyListStart)->tKeyInfo.bPush = false;
					(*ActionKeyListStart)->tKeyInfo.bRelease = true;
					bFunctionCall[KS_RELEASE] = true;
				}
				else if (true ==  (*ActionKeyListStart)->tKeyInfo.bRelease )
				{
					// �׳� �����ִ»��� (�ƹ����� X)
					(*ActionKeyListStart)->tKeyInfo.bRelease = false;
				}
			}
		}

		// ����� �Լ� ȣ���ϱ�
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


// Ű ���
void InputManager::AddAxisKey(const std::string & _strName, 
	unsigned char _cKey, float _fScale)
{
	PBindAxisInfo pBind = FindAxis(_strName);

	// ������ ���� ���
	if (nullptr == pBind)
	{
		pBind = new BindAxisInfo;
		pBind->strName = _strName;
		m_mapAxis.insert(std::make_pair(_strName, pBind));
	}

	// �ش� PBindAxisInfo�� Ű�� �̹� ��ϵǾ��ִ��� �˻��Ѵ�. (������ ����� �ʿ� ����)
	std::list<PKeyScale>::iterator StartIter	 = pBind->KeyList.begin();
	std::list<PKeyScale>::iterator EndIter		 = pBind->KeyList.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		// �̹� �ش� Ű�� ��ϵǾ������� return
		if ((*StartIter)->cKey == _cKey)
		{
			return;
		}
	}

	PKeyScale pKeyScale = new KeyScale;
	pKeyScale->cKey = _cKey;
	pKeyScale->fScale = _fScale;

	// �ش� Ű�� �̹� ��ü Ű ����Ʈ�� ��ϵǾ��ִ��� �Ǵ�
	bool bFind = false;

	for (int i = 0; i < m_iAddKeySize; ++i)
	{
		if (m_AddKeyList[i] == _cKey)
		{
			bFind = true;
			break;
		}
	}

	// ����� �ȵǾ��ִٸ� �߰�
	if (false == bFind)
	{
		m_AddKeyList[m_iAddKeySize] = _cKey;
		++m_iAddKeySize;
	}

	pBind->KeyList.push_back(pKeyScale);
}

// ��� ���콺���� ������ (����Ű)  // _iFKey�� ����Ű��
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

	// ����Ű -> ���翩��..(enum�� ����ִ°͸� ó��)
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

	// ���� Ű�� �̹� ��ϵ� Ű���� �Ǵ� ( ������ �� �߰��� �ʿ䰡 �����ϱ�)
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
