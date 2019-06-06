#include "Device.h"

ENGINE_USING

DEFINITION_SINGLETON(Device)

Device::Device() 
	: m_pDevice(nullptr),
	m_pContext(nullptr),
	m_pSwapChain(nullptr),
	m_pRenderTargetView(nullptr),
	m_pDepthStencilView(nullptr),
	m_p2DRenderTarget(nullptr),
	m_p2DFactory(nullptr)
{
}


Device::~Device()
{
	SAFE_RELEASE(m_p2DRenderTarget);
	SAFE_RELEASE(m_p2DFactory);

	SAFE_RELEASE(m_pDepthStencilView);
	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pSwapChain);

	m_pContext->ClearState();						// 상태들 전부 지워주기
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pDevice);
}

#pragma region Get, Set
// Get함수들
ID3D11Device* Device::GetDevice() const
{
	return  m_pDevice;
}

ID3D11DeviceContext* Device::GetContext() const
{
	return m_pContext;
}

IDXGISwapChain* Device::GetSwapChain() const
{
	return m_pSwapChain;
}

ID2D1RenderTarget* Device::Get2DRenderTarget() const
{
	return m_p2DRenderTarget;
}

Resolution Device::GetResolution() const
{
	return m_Resolution;
}

Vector2 Device::GetWindowToViewportRatio() const
{
	// GetWindowRect : 윈도우 화면 좌표값으로 위치 정보를 반환한다.
	// 윈도우 좌표 : 모니터 화면을 기준으로 하는 절대 좌표계
	RECT	rcWindow = {};
	GetClientRect(m_hWnd, &rcWindow);

	Vector2 vSize = {};
	vSize.x = float(rcWindow.right - rcWindow.left);
	vSize.y = float(rcWindow.bottom - rcWindow.top);

	Vector2 vViewport = {};
	vViewport.x = float(m_Resolution.iWidth);
	vViewport.y = float(m_Resolution.iHeight);

	return vViewport / vSize;			// 윈도우 화면에 대한 해상도 비율 얻기
}
#pragma endregion


bool Device::Init(HWND _hWnd, unsigned int _iWidth, unsigned int _iHeight, 
	bool _bWindowMode, int _iSamplerCount /*= 1*/)
{
	m_hWnd = _hWnd;
	unsigned int iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG


	m_Resolution.iWidth = _iWidth;
	m_Resolution.iHeight = _iHeight;

	// D3D11_CREATE_DEVICE_BGRA_SUPPORT -> 3D  출력상태에서 2D출력이 가능해지도록 한다.
	iFlag |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	D3D_FEATURE_LEVEL eFLevel = D3D_FEATURE_LEVEL_11_0;				// 기능수준
	D3D_FEATURE_LEVEL eFLevel1 = D3D_FEATURE_LEVEL_11_0;

	DXGI_SWAP_CHAIN_DESC tSwapChainDesc = {};

	tSwapChainDesc.BufferDesc.Width = _iWidth;
	tSwapChainDesc.BufferDesc.Height = _iHeight;
	tSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;					// UNORM 부호없는, 얘 덕분에 셰이더에서 0~1값으로 사용함 ㅇㅅㅇ 8비트가 아닌 다른비트면 달라짐
	tSwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;				// Scaling : 확대, 축소
	tSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	tSwapChainDesc.BufferDesc.RefreshRate.Numerator = 1;			// 분자
	tSwapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
	tSwapChainDesc.BufferCount = 1;
	tSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	tSwapChainDesc.OutputWindow = _hWnd;
	tSwapChainDesc.SampleDesc.Count = _iSamplerCount;							// 소프트 파티클의 알파값을 부드럽게 하기 위해서 ㅇㅅㅇ 1이아니라 8적용
	tSwapChainDesc.SampleDesc.Quality = 0;
	tSwapChainDesc.Windowed = _bWindowMode;							// 창모드, 전체화면모드 여부
	tSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// D3D11CreateDeviceAndSwapChain
	// : 디스플레이 어댑터와, 렌더링에 사용되는 스왑체인을 나타내는 장치르 ㄹ만듬 (여튼 디바이스, 컨텍스트, 스왑체인 다 한꺼번에 만드는거)
	if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, &eFLevel,
		1, D3D11_SDK_VERSION, &tSwapChainDesc,
		&m_pSwapChain, &m_pDevice, &eFLevel1, &m_pContext)))
		return false;

	// 2D 텍스처 인터페이스는 구조화 된 메모리 인 텍셀 데이터를 관리합니다.
	ID3D11Texture2D* pBackBuffer = nullptr;						// 후면버퍼 담당

	// COM 객체를 얻어오면 레퍼런스 카운트가 증가함
	// __uuidof연산자 : 식에 열결된 GUID를 검색함 ㅇㅅㅇ(가져오는듯)
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);			//  스왑 체인의 백버퍼중 하나에 액세스함

	m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);

	SAFE_RELEASE(pBackBuffer);


	// 깊이뷰 만들기
	D3D11_TEXTURE2D_DESC tDepthBuffer = {};
	tDepthBuffer.Width = _iWidth;
	tDepthBuffer.Height = _iHeight;
	tDepthBuffer.ArraySize = 1;
	tDepthBuffer.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tDepthBuffer.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tDepthBuffer.Usage = D3D11_USAGE_DEFAULT;
	tDepthBuffer.MipLevels = 1;
	tDepthBuffer.SampleDesc.Count = _iSamplerCount;
	tDepthBuffer.SampleDesc.Quality = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&tDepthBuffer, nullptr, &pBackBuffer)))
		return false;

	m_pDevice->CreateDepthStencilView(pBackBuffer, nullptr, &m_pDepthStencilView);

	SAFE_RELEASE(pBackBuffer);

	// 만들어준 타겟뷰(렌더타겟뷰)와 뎁스뷰를 렌더링 파이프라인에 묶어주기(아웃풋머저에 묶어줘야한다.)
	m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	// 뷰포트 설정하기
	D3D11_VIEWPORT tViewPort = {};
	tViewPort.Width = (float)_iWidth;
	tViewPort.Height = (float)_iHeight;
	tViewPort.MinDepth = 0.0f;
	tViewPort.MaxDepth = 1.0f;

	m_pContext->RSSetViewports(1, &tViewPort);

	// D2D Factory를 생성하기
	D2D1_FACTORY_OPTIONS tOption = {};
	tOption.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

	if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, tOption,
		&m_p2DFactory)))
		return false;

	// DirectX11 BackBuffer 타겟을 이용해서 2D렌더링 타겟을 설정해주기
	IDXGISurface* pBackSurface = nullptr;
	m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackSurface));

	// 2D 렌더타겟을 생성하기 위한 옵션을 설정하기
	D2D1_RENDER_TARGET_PROPERTIES PropertyOption = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_HARDWARE,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));

	if (FAILED(m_p2DFactory->CreateDxgiSurfaceRenderTarget(pBackSurface, PropertyOption,
		&m_p2DRenderTarget)))
		return false;

	SAFE_RELEASE(pBackSurface);
	return true;
}

void Device::Clear(float _fClearColor[4])
{
	m_pContext->ClearRenderTargetView(m_pRenderTargetView, _fClearColor);

	m_pContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Device::Present()
{
	// Present는 V싱크 신호를 받은 후에 그린다.
	m_pSwapChain->Present(0, 0);
}
