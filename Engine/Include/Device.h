#pragma once
#include "Engine.h"

ENGINE_BEGIN

class ENGINE_DLL Device
{
private:
	HWND					m_hWnd;
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pContext;
	IDXGISwapChain*			m_pSwapChain;				// 렌더링 된 데이터를 출력하기 전에 저장하기위한 하나 이상의 서페이스 를 구현합니다.
	ID3D11RenderTargetView*	m_pRenderTargetView;
	ID3D11DepthStencilView*	m_pDepthStencilView;
	Resolution				m_Resolution;

private:
	// 2D Interface, 2D라이브러리(출력용)
	ID2D1RenderTarget*	m_p2DRenderTarget;
	ID2D1Factory*		m_p2DFactory;

public:
	// Get함수
	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetContext() const;
	IDXGISwapChain* GetSwapChain() const;
	ID2D1RenderTarget* Get2DRenderTarget() const;
	Resolution GetResolution() const;
	Vector2 GetWindowToViewportRatio()	const;			// 마우스 좌표를 알아내기 위해서 사용하는 윈도우 크기와 뷰포트에 대한 비율

public:
	bool Init(HWND _hWnd, unsigned int _iWidth, unsigned int _iHeight, bool _bWindowMode, int _iSamplerCount/* = 1*/);
	void Clear(float _fCrearColor[4]);
	void Present();

	// 싱글톤을 ㅗ만들기
	DECLARE_SINGLETON(Device)
};

ENGINE_END