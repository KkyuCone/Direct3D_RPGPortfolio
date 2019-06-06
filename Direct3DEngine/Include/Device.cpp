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

	m_pContext->ClearState();						// 雌殿級 穿採 走趨爽奄
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pDevice);
}

#pragma region Get, Set
// Get敗呪級
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
	// GetWindowRect : 制亀酔 鉢檎 疎妊葵生稽 是帖 舛左研 鋼発廃陥.
	// 制亀酔 疎妊 : 乞艦斗 鉢檎聖 奄層生稽 馬澗 箭企 疎妊域
	RECT	rcWindow = {};
	GetClientRect(m_hWnd, &rcWindow);

	Vector2 vSize = {};
	vSize.x = float(rcWindow.right - rcWindow.left);
	vSize.y = float(rcWindow.bottom - rcWindow.top);

	Vector2 vViewport = {};
	vViewport.x = float(m_Resolution.iWidth);
	vViewport.y = float(m_Resolution.iHeight);

	return vViewport / vSize;			// 制亀酔 鉢檎拭 企廃 背雌亀 搾晴 条奄
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

	// D3D11_CREATE_DEVICE_BGRA_SUPPORT -> 3D  窒径雌殿拭辞 2D窒径戚 亜管背走亀系 廃陥.
	iFlag |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	D3D_FEATURE_LEVEL eFLevel = D3D_FEATURE_LEVEL_11_0;				// 奄管呪層
	D3D_FEATURE_LEVEL eFLevel1 = D3D_FEATURE_LEVEL_11_0;

	DXGI_SWAP_CHAIN_DESC tSwapChainDesc = {};

	tSwapChainDesc.BufferDesc.Width = _iWidth;
	tSwapChainDesc.BufferDesc.Height = _iHeight;
	tSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;					// UNORM 採硲蒸澗, 剰 幾歳拭 捨戚希拭辞 0~1葵生稽 紫遂敗 しさし 8搾闘亜 焼観 陥献搾闘檎 含虞像
	tSwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;				// Scaling : 溌企, 逐社
	tSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	tSwapChainDesc.BufferDesc.RefreshRate.Numerator = 1;			// 歳切
	tSwapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
	tSwapChainDesc.BufferCount = 1;
	tSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	tSwapChainDesc.OutputWindow = _hWnd;
	tSwapChainDesc.SampleDesc.Count = _iSamplerCount;							// 社覗闘 督銅適税 硝督葵聖 採球郡惟 馬奄 是背辞 しさし 1戚焼艦虞 8旋遂
	tSwapChainDesc.SampleDesc.Quality = 0;
	tSwapChainDesc.Windowed = _bWindowMode;							// 但乞球, 穿端鉢檎乞球 食採
	tSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// D3D11CreateDeviceAndSwapChain
	// : 巨什巴傾戚 嬢基斗人, 兄希元拭 紫遂鞠澗 什尋端昔聖 蟹展鎧澗 舌帖牽 ぉ幻給 (食動 巨郊戚什, 珍努什闘, 什尋端昔 陥 廃襖腰拭 幻球澗暗)
	if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, &eFLevel,
		1, D3D11_SDK_VERSION, &tSwapChainDesc,
		&m_pSwapChain, &m_pDevice, &eFLevel1, &m_pContext)))
		return false;

	// 2D 努什坦 昔斗凪戚什澗 姥繕鉢 吉 五乞軒 昔 努漆 汽戚斗研 淫軒杯艦陥.
	ID3D11Texture2D* pBackBuffer = nullptr;						// 板檎獄遁 眼雁

	// COM 梓端研 条嬢神檎 傾遁訓什 朝錘闘亜 装亜敗
	// __uuidof尻至切 : 縦拭 伸衣吉 GUID研 伊事敗 しさし(亜閃神澗牛)
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);			//  什尋 端昔税 拷獄遁掻 馬蟹拭 衝室什敗

	m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);

	SAFE_RELEASE(pBackBuffer);


	// 燕戚坂 幻級奄
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

	// 幻級嬢層 展為坂(兄希展為坂)人 紀什坂研 兄希元 督戚覗虞昔拭 広嬢爽奄(焼数撚袴煽拭 広嬢操醤廃陥.)
	m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	// 坂匂闘 竺舛馬奄
	D3D11_VIEWPORT tViewPort = {};
	tViewPort.Width = (float)_iWidth;
	tViewPort.Height = (float)_iHeight;
	tViewPort.MinDepth = 0.0f;
	tViewPort.MaxDepth = 1.0f;

	m_pContext->RSSetViewports(1, &tViewPort);

	// D2D Factory研 持失馬奄
	D2D1_FACTORY_OPTIONS tOption = {};
	tOption.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

	if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, tOption,
		&m_p2DFactory)))
		return false;

	// DirectX11 BackBuffer 展為聖 戚遂背辞 2D兄希元 展為聖 竺舛背爽奄
	IDXGISurface* pBackSurface = nullptr;
	m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackSurface));

	// 2D 兄希展為聖 持失馬奄 是廃 辛芝聖 竺舛馬奄
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
	// Present澗 V縮滴 重硲研 閤精 板拭 益鍵陥.
	m_pSwapChain->Present(0, 0);
}
