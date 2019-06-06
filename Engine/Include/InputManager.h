#pragma once
#include "Engine.h"

ENGINE_BEGIN

// 마우스 관련 DIK는 없으므로.. DIK를 임시로 만들어줌.
#define DIK_LBUTTON 0xfd
#define DIK_RBUTTON 0xfe
#define DIK_MBUTTON	0xff

typedef struct ENGINE_DLL _tagKeyInfo
{
	bool	bPress;		// 키를 누르고 있는지
	bool	bPush;		// 키를 눌렀는지
	bool	bRelease;	// 떼어냈는지 (안누르는거)

	_tagKeyInfo()
	{
		bPress = false;
		bPush = false;
		bRelease = false;
	}
}KeyInfo, *PKeyInfo;

// Key와 관련된 클래스는 Call Back 함수 형태로 만들거임 ㅇㅅㅇ
// Client에서 해당 키를 누르면 그 키에 연결된 함수를 호출할 것이다. ( event 함수처럼 생각하면됨 )
// Axis는 누르고 있는 경우 (계속 누르고 있는경우) function<시간값, 스케일값> , ex ) 이동키
// Action은 눌렀을 경우, 놓을때 , function<시간값>		ex) 스킬키 등

// void로 받는 이유는어떤 함수가 들어올지 모르기때문이다 ㅇㅅㅇ
// 기존 Axis에 입력받을 키랑 함수가 바인드되어있는 여부를 추가해준다.

typedef struct ENGINE_DLL _tagKeyScale
{
	unsigned char cKey;		// 모든키는 char로 받을 수있다. ( 키는 256개, 0~255)
	float fScale;
}KeyScale, *PKeyScale;

typedef struct ENGINE_DLL _tagBindAxisInfo
{
	std::string strName;
	bool bFunctionBind;									// 함수와 연결되어있는지 여부
	std::function<void(float, float)>	Func;			// 연결된 함수
	std::list<PKeyScale> KeyList;
	bool bGlobal;				// 다른씬들에서도 공용으로 쓸건지 여부

	_tagBindAxisInfo()
	{
		bFunctionBind = false;
		bGlobal = false;
	}

	~_tagBindAxisInfo()
	{
		// 동적할당 된거 삭제해주기 ㅇㅅㅇ
		Safe_Delete_VectorList(KeyList);
	}

}BindAxisInfo, *PBindAxisInfo;

// 액션 정보 (조합키 정보)
typedef struct ENGINE_DLL _tagBindActionKeyInfo
{
	MOUSE_BUTTON	eMouseType;					// 마우스 값(왼쪽, 오른쪽, 가운데)
	unsigned char	cKey;
	bool			bFKKey[FK_END];				// 조합키가 있는지 여부 
	KeyInfo			tKeyInfo;
	//bool			bKeyState[KS_END];			// 키상태
	//bool			bFKKeyState[FK_END][KS_END];	// 조합키 상태 [조합키][키상태]

	_tagBindActionKeyInfo()
	{
		cKey = 0;
		memset(bFKKey, 0, sizeof(bool) * FK_END);
		eMouseType = MB_NONE;
		/*memset(bKeyState, 0, sizeof(bool) * KS_END);
		memset(bFKKeyState, 0, sizeof(bool) * KS_END * FK_END);*/
	}
}ActionKeyInfo, *PActionKeyInfo;

typedef struct ENGINE_DLL _tagBindActionInfo
{
	std::string strName;
	bool bFunctionBind[KS_END];				// 함수가 묶여있는지 여부
	std::function<void(float)>		Func[KS_END];			// 얘도 각 함수들이있음 (Press, Push, Release 일때의 불러올 함수들 )
	std::list<PActionKeyInfo> KeyList;
	bool bGlobal;

	_tagBindActionInfo()
	{
		memset(bFunctionBind, 0, sizeof(bool) * KS_END);
		bGlobal = false;
	}

}BindActionInfo, *PBindActionInfo;


class ENGINE_DLL InputManager
{
private:
	IDirectInput8*	m_pInput;
	IDirectInputDevice8*	m_pKeyboard;
	IDirectInputDevice8*	m_pMouseDevice;
	//IDirectInputDevice8*	m_pJoyStick;

	unsigned char			m_Key[256];
	unsigned char			m_AddKeyList[256];				// 등록된 키 목록
	int						m_iAddKeySize;					// 등록된 키 개수
	KeyInfo					m_tKeyInfo[256];				// 모든키 배열.. 눌려진 키 값에 눌렀는지 아닌지 표시하면된다.
	
	short					m_sWheel;						// 마우스 휠 관련, 휠에대한 이벤트가 들어올때 사용 2바이트인 이유는-> 입력받을때 반반씩
	DIMOUSESTATE2			m_tMouseState;					// 마우스 상태( 요즘은 마우스에 대한 버튼이 많아져서 ㅇㅁㅇ..그거에 대한거까지 반영하려면 2를 사용)
	KeyInfo					m_tMouseInfo[8];
	
	//DIJOYSTATE2				m_tJoyStickState;				// 조이스틱 상태

	// 마우스 좌표 관련
	Vector2					m_vWindowMousePos;
	Vector2					m_vWindowMouseMove;
	Vector2					m_vViewportMousePos;
	Vector2					m_vViewportMouseMove;

	Vector2					m_vViewportUIMousePos;
	Vector2					m_vViewportUIMouseMove;

	// 마우스용 오브젝트
	class GameObject*		m_pMouseObj;
	class ColliderRay*		m_pMouseRay;


public:
	void SetWheel(short _sWheel);
	short GetWheelDir() const;
	bool MousePress(MOUSE_BUTTON _eMouseButton);
	bool MousePush(MOUSE_BUTTON _eMouseButton);
	bool MouseRelease(MOUSE_BUTTON _eMouseButton);

	Vector2 GetMouseWindowPos()	const;
	Vector2 GetMouseWindowMove() const;
	Vector2 GetMouseViewportPos() const;
	Vector2 GetMouseViewportMove() const;
	Vector2 GetUIMouseViewportPos() const;
	Vector2 GetUIMouseViewportMove() const;

	bool MouseRayEnable() const;
	class ColliderRay* GetMouseRay() const;

public:
	void ChangeMouseScene(class Scene* _pScene);

private:
	std::unordered_map<std::string, PBindAxisInfo> m_mapAxis;
	std::unordered_map<std::string, PBindActionInfo> m_mapAction;

public:
	bool Init(HINSTANCE _hInst, HWND _hWnd);
	void Update(float _fTime);
	void Render(float _fTime);
	PBindAxisInfo FindAxis(const std::string& _strName);
	PBindActionInfo FindAction(const std::string& _strName);

private:
	bool ReadKeyboard();
	bool ReadMouse();
	bool ReadJoyStick();

private:
	void UpdateKeyboard(float _fTime);
	void UpdateMouse(float _fTime);
	void UpdateAxis(float _fTime);			// 조합키가 아닌것들 눌렀는지 체크
	void UpdateAction(float _fTime);		// 등록된 조합키들이 눌렀는지 체크

public:
	// 키 등록 (단순히 키만 등록하는 것이다.)
	void AddAxisKey(const std::string& _strName, unsigned char _cKey, float _fScale);
	void AddActionKey(const std::string& _strName, unsigned char _cKey, int _iFKey = 0/*= FK_CTRL*/, MOUSE_BUTTON _eMouseButton = MB_NONE);

public:
	// 해당 키를 찾아서 전역키인지 셋팅하기
	void SetAxisGlobal(const std::string& _strName, bool _bGlobal)
	{
		PBindAxisInfo pBind = FindAxis(_strName);

		if (nullptr == pBind)
		{
			return;
		}

		pBind->bGlobal = _bGlobal;
	}

	// 키와 함수 연결해주기
	void BindAxis(const std::string& _strName, void(*_pFunc)(float, float))
	{
		PBindAxisInfo pBind = FindAxis(_strName);

		// 키가 없으면 새로 만들어준다.
		if (nullptr == pBind)
		{
			pBind = new BindAxisInfo;
			pBind->strName = _strName;
			m_mapAxis.insert(std::make_pair(_strName, pBind));
		}

		pBind->Func = std::bind(_pFunc, std::placeholders::_1, std::placeholders::_2);
		pBind->bFunctionBind = true;
	}

	template <typename T>
	void BindAxis(const std::string& _strName, T* _pObject, void(T::*_pFunc)(float, float))
	{
		PBindAxisInfo pBind = FindAxis(_strName);

		if (nullptr == pBind)
		{
			pBind = new BindAxisInfo;
			pBind->strName = _strName;
			m_mapAxis.insert(std::make_pair(_strName, pBind));
		}

		pBind->Func = std::bind(_pFunc, _pObject, std::placeholders::_1, std::placeholders::_2);
		pBind->bFunctionBind = true;
	}


public:
	// 조합키 관련
	void SetActionGlobal(const std::string& _strName, bool _bGlobla)
	{
		PBindActionInfo pBind = FindAction(_strName);

		if (nullptr == pBind)
		{
			return;
		}

		pBind->bGlobal = true;
	}

	void BindAction(const std::string& _strName, KEY_STATE _eKeyState, void(*_pFunc)(float))
	{
		PBindActionInfo pBind = FindAction(_strName);

		if (nullptr == pBind)
		{
			pBind = new BindActionInfo;
			pBind->strName = _strName;
			m_mapAction.insert(std::make_pair(_strName, pBind));
		}

		pBind->Func[_eKeyState] = std::bind(_pFunc, std::placeholders::_1);
		pBind->bFunctionBind[_eKeyState] = true;
	}

	template <typename T>
	void BindAction(const std::string& _strName, 
		KEY_STATE _eKeyState, T* _pObject, void(T::*_pFunc)(float))
	{
		PBindActionInfo pBind = FindAction(_strName);

		if (nullptr == pBind)
		{
			pBind = new BindActionInfo;
			pBind->strName = _strName;
			m_mapAction.insert(std::make_pair(_strName, pBind));
		}

		pBind->Func[_eKeyState] = std::bind(_pFunc, _pObject, std::placeholders::_1);
		pBind->bFunctionBind[_eKeyState] = true;
	}

public:
	DECLARE_SINGLETON(InputManager);
};

ENGINE_END

