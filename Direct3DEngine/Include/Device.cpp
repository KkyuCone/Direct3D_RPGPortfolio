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

	m_pContext->ClearState();						// ���µ� ���� �����ֱ�
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pDevice);
}

#pragma region Get, Set
// Get�Լ���
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
	// GetWindowRect : ������ ȭ�� ��ǥ������ ��ġ ������ ��ȯ�Ѵ�.
	// ������ ��ǥ : ����� ȭ���� �������� �ϴ� ���� ��ǥ��
	RECT	rcWindow = {};
	GetClientRect(m_hWnd, &rcWindow);

	Vector2 vSize = {};
	vSize.x = float(rcWindow.right - rcWindow.left);
	vSize.y = float(rcWindow.bottom - rcWindow.top);

	Vector2 vViewport = {};
	vViewport.x = float(m_Resolution.iWidth);
	vViewport.y = float(m_Resolution.iHeight);

	return vViewport / vSize;			// ������ ȭ�鿡 ���� �ػ� ���� ���
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

	// D3D11_CREATE_DEVICE_BGRA_SUPPORT -> 3D  ��»��¿��� 2D����� ������������ �Ѵ�.
	iFlag |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	D3D_FEATURE_LEVEL eFLevel = D3D_FEATURE_LEVEL_11_0;				// ��ɼ���
	D3D_FEATURE_LEVEL eFLevel1 = D3D_FEATURE_LEVEL_11_0;

	DXGI_SWAP_CHAIN_DESC tSwapChainDesc = {};

	tSwapChainDesc.BufferDesc.Width = _iWidth;
	tSwapChainDesc.BufferDesc.Height = _iHeight;
	tSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;					// UNORM ��ȣ����, �� ���п� ���̴����� 0~1������ ����� ������ 8��Ʈ�� �ƴ� �ٸ���Ʈ�� �޶���
	tSwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;				// Scaling : Ȯ��, ���
	tSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	tSwapChainDesc.BufferDesc.RefreshRate.Numerator = 1;			// ����
	tSwapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
	tSwapChainDesc.BufferCount = 1;
	tSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	tSwapChainDesc.OutputWindow = _hWnd;
	tSwapChainDesc.SampleDesc.Count = _iSamplerCount;							// ����Ʈ ��ƼŬ�� ���İ��� �ε巴�� �ϱ� ���ؼ� ������ 1�̾ƴ϶� 8����
	tSwapChainDesc.SampleDesc.Quality = 0;
	tSwapChainDesc.Windowed = _bWindowMode;							// â���, ��üȭ���� ����
	tSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// D3D11CreateDeviceAndSwapChain
	// : ���÷��� ����Ϳ�, �������� ���Ǵ� ����ü���� ��Ÿ���� ��ġ�� ������ (��ư ����̽�, ���ؽ�Ʈ, ����ü�� �� �Ѳ����� ����°�)
	if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, &eFLevel,
		1, D3D11_SDK_VERSION, &tSwapChainDesc,
		&m_pSwapChain, &m_pDevice, &eFLevel1, &m_pContext)))
		return false;

	// 2D �ؽ�ó �������̽��� ����ȭ �� �޸� �� �ؼ� �����͸� �����մϴ�.
	ID3D11Texture2D* pBackBuffer = nullptr;						// �ĸ���� ���

	// COM ��ü�� ������ ���۷��� ī��Ʈ�� ������
	// __uuidof������ : �Ŀ� ����� GUID�� �˻��� ������(�������µ�)
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);			//  ���� ü���� ������� �ϳ��� �׼�����

	m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);

	SAFE_RELEASE(pBackBuffer);


	// ���̺� �����
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

	// ������� Ÿ�ٺ�(����Ÿ�ٺ�)�� �����並 ������ ���������ο� �����ֱ�(�ƿ�ǲ������ ��������Ѵ�.)
	m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	// ����Ʈ �����ϱ�
	D3D11_VIEWPORT tViewPort = {};
	tViewPort.Width = (float)_iWidth;
	tViewPort.Height = (float)_iHeight;
	tViewPort.MinDepth = 0.0f;
	tViewPort.MaxDepth = 1.0f;

	m_pContext->RSSetViewports(1, &tViewPort);

	// D2D Factory�� �����ϱ�
	D2D1_FACTORY_OPTIONS tOption = {};
	tOption.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

	if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, tOption,
		&m_p2DFactory)))
		return false;

	// DirectX11 BackBuffer Ÿ���� �̿��ؼ� 2D������ Ÿ���� �������ֱ�
	IDXGISurface* pBackSurface = nullptr;
	m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackSurface));

	// 2D ����Ÿ���� �����ϱ� ���� �ɼ��� �����ϱ�
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
	// Present�� V��ũ ��ȣ�� ���� �Ŀ� �׸���.
	m_pSwapChain->Present(0, 0);
}
