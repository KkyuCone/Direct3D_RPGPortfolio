#pragma once
#include "Engine.h"

ENGINE_BEGIN

class ENGINE_DLL Core
{

#pragma region 숙제용도
//private:
//	VertexColor TestVColor[3] = {
//		VertexColor(Vector3(0.0f, 0.5f, 0.0f), Vector4::Green),
//		VertexColor(Vector3(0.5f, -0.5f, 0.0f), Vector4::Red),
//		VertexColor(Vector3(-0.5f, -0.5f, 0.0f), Vector4::Blue),
//	};
#pragma endregion

private:
	HINSTANCE	m_hInst;
	HWND		m_hWnd;
	Resolution	m_tResolution;				// 해상도( _tag, _t는 구조체 )
	static bool m_bLoop;
	float		m_fClearColor[4];
	bool		m_bEditMode;				// Edit 모드 설정 -> 피킹용도체크

public:
	// 윈도우 Instance, Handle Get함수
	HINSTANCE GetWindowInstance()	const;
	HWND GetWindowHandle()	const;
	bool GetEditMode() const;

public:
	void SetClearColor(unsigned char _r, unsigned char _g,
		unsigned char _b, unsigned char _a);
	void SetEditMode(bool _Value);

public:
	// 초기화
	// _bWindowMode = true는 창모드, false는 전체화면
	bool Init(HINSTANCE _hInst, const TCHAR* _pTitle, const TCHAR* _pClassName,
		int _iWidth, int _iHeight, int _iIconID, int _iSmallIcon, 
		bool _bEditMode = false, bool _bWindowMode = true, int _iSamplerCount = 1);

	// 오버로딩, 이미 초기화된(생성된) 윈도우가 있을 경우 추가할 때
	bool Init(HINSTANCE _hInst, HWND _hWnd, int _iWidth, int _iHeight,
		bool _bEditMode = false, bool _bWindowMode = true, int _iSamplerCount = 1);


	int Run();
	void GameLogic();

private:
	int Input(float _fTime);
	int Update(float _fTime);
	int LateUpdate(float _fTime);
	int Collision(float _fTime);
	int Render(float _fTime);

private:
	void Register(const TCHAR* _pClassName, int _iIconID, int _iSmallIconID);				// 등록
	void CreateWnd(const TCHAR* _pCTitle, const TCHAR* _pClassName);						// 윈도우 만들기
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_SINGLETON(Core)			// 싱글톤 선언하기
};

ENGINE_END