#pragma once
#include "UI.h"

ENGINE_BEGIN

// 버튼 상태
enum BUTTON_STATE
{
	BS_DISABLE,		// 비활성화..?
	BS_NORMAL,
	BS_MOUSEON,
	BS_CLICK,
	BS_END
};

class ENGINE_DLL UIButton :
	public UI
{
private:
	friend class GameObject;

protected:
	UIButton();
	UIButton(const UIButton& _Com);
	virtual ~UIButton();

protected:
	BUTTON_STATE	m_ePrevState;							// 마우스 이전 상태
	BUTTON_STATE	m_eState;								// 마우스 상태
	//bool			m_bStartTextureChange;					// 텍스쳐 체인지 여부 (텍스처 일 경우 처리 )
	std::function<void(float)> m_CallBackFunc[BS_END];			// 마우스 상태에 따른 함수 모음

	class Texture*		m_pTexture[BS_END];					// 마우스 상태에 따른 텍스처
	Vector4				m_vColor[BS_END];					// 마우스 상태에 따른 색상값
	class Material*		m_pMaterial;						// 머티리얼
	ButtonCBuffer		m_tButtonCBuffer;					// 색상값 상수버퍼로 업뎃해야함

	class Renderer*		m_pRenderer;
	
public:
	// 버튼 상태에 따른 색상 셋팅
	void SetStateColor(BUTTON_STATE _eState, const Vector4& _vColor);

	// 버튼 상태에 따른 텍스처 셋팅
	void SetStateTexture(BUTTON_STATE _eState, const std::string& _strName,
		const TCHAR* _pFileName, const std::string& _strPathName = PATH_TEXTURE);

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출, 즉 레이어에서 호출해준다.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// 렌더를 하기 직전에 호출해주는 함수ㅔㄱㅁ
	virtual int Render(float _fTime);
	virtual UIButton* Clone()	const;

public:
	void SetCallBackFunc(BUTTON_STATE _eState, void(*_pFunc)(float));

	template <typename FuncType>
	void SetCallBackFunc(BUTTON_STATE _eState, FuncType* _pObject, void(FuncType::*_pFunc)(float))
	{
		m_CallBackFunc[_eState] = std::bind(_pFunc, _pObject, std::placeholders::_1);
	}
	
};

ENGINE_END
