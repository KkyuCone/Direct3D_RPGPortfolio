#include "Core.h"
#include "Device.h"
#include "Resource\ResourcesManager.h"
#include "Render\RenderManager.h"
#include "PathManager.h"
#include "Render\ShaderManager.h"
#include "Render\Shader.h"
#include "Resource\Mesh.h"
#include "Scene\SceneManager.h"
#include "TimerManager.h"
#include "Timer.h"
#include "InputManager.h"
#include "CollisionManager.h"
#include "Navigation\NavigationManager.h"
#include "FontManager.h"
#include "ExcelManager.h"
#include "SoundManager.h"
#include "Render\SSAOManager.h"

ENGINE_USING

DEFINITION_SINGLETON(Core)

bool Core::m_bLoop = true;

Core::Core()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(6386);
	m_bEditMode = false;
	memset(m_fClearColor, 0, sizeof(float) * 4);
}


Core::~Core()
{
	DESTROY_SINGLETON(SceneManager);

	DESTROY_SINGLETON(SoundManager);
	DESTROY_SINGLETON(FontManager);
	DESTROY_SINGLETON(NavigationManager);
	DESTROY_SINGLETON(InputManager);
	DESTROY_SINGLETON(CollisionManager);
	DESTROY_SINGLETON(TimerManager);
	DESTROY_SINGLETON(RenderManager);
	DESTROY_SINGLETON(ResourcesManager);
	DESTROY_SINGLETON(PathManager);
	DESTROY_SINGLETON(SSAOManager);

	DESTROY_SINGLETON(ExcelManager);

	DESTROY_SINGLETON(Device);
}

HINSTANCE Core::GetWindowInstance() const
{
	return m_hInst;
}

HWND Core::GetWindowHandle() const
{
	return m_hWnd;
}

bool Core::GetEditMode() const
{
	return m_bEditMode;
}

void Core::SetClearColor(unsigned char _r, unsigned char _g,
	unsigned char _b, unsigned char _a)
{
	m_fClearColor[0] = _r / 255.0f;
	m_fClearColor[1] = _g / 255.0f;
	m_fClearColor[2] = _b / 255.0f;
	m_fClearColor[3] = _a / 255.0f;
}

void Core::SetEditMode(bool _Value)
{
	m_bEditMode = _Value;
}


bool Core::Init(HINSTANCE _hInst, const TCHAR* _pTitle, const TCHAR* _pClassName,
	int _iWidth, int _iHeight, int _iIconID, int _iSmallIcon, 
	bool _bEditMode /*= false*/, bool _bWindowMode /*= true*/, int _iSamplerCount /*= 1*/)
{
	m_hInst = _hInst;
	m_tResolution.iWidth = _iWidth;
	m_tResolution.iHeight = _iHeight;

	Register(_pClassName, _iIconID, _iSmallIcon);
	CreateWnd(_pTitle, _pClassName);

	return Init(m_hInst, m_hWnd, _iWidth, _iHeight, _bEditMode, _bWindowMode, _iSamplerCount);
}

bool Core::Init(HINSTANCE _hInst, HWND _hWnd, int _iWidth, int _iHeight,
	bool _bEditMode /*= false*/, bool _bWindowMode /*= true*/, int _iSamplerCount/* = 1*/)
{
	m_hInst = _hInst;
	m_hWnd = _hWnd;
	m_bEditMode = _bEditMode;			// 에디트 모드 설정

	// 디바이스 초기화
	if (!GET_SINGLETON(Device)->Init(_hWnd, _iWidth, _iHeight, _bWindowMode, _iSamplerCount))
	{
		return false;
	}

	// 경로관리자 초기화
	if (false == GET_SINGLETON(PathManager)->Init())
	{
		return false;
	}

	// Font 관리자 초기화
	if (false == GET_SINGLETON(FontManager)->Init())
	{
		return false;
	}

	// 사운드 관리자 초기화
	if (false == GET_SINGLETON(SoundManager)->Init())
	{
		return false;
	}

	// 입력 관리자 초기화
	if (false == GET_SINGLETON(InputManager)->Init(_hInst, _hWnd))
	{
		return false;
	}

	// 리소스 관리자 초기화
	if (false == GET_SINGLETON(ResourcesManager)->Init())
	{
		return false;
	}

	// 렌더링 관리자 초기화
	if (false == GET_SINGLETON(RenderManager)->Init())
	{
		return false;
	}

	// 시간 관리자 초기화
	if (false == GET_SINGLETON(TimerManager)->Init())
	{
		return false;
	}

	// 충돌 관리자 초기화
	if (false == GET_SINGLETON(CollisionManager)->Init())
	{
		return false;
	}

	// 네비게이션 관리자
	if (false == GET_SINGLETON(NavigationManager)->Init())
	{
		return false;
	}

	// 씬 관리자 초기화
	if (false == GET_SINGLETON(SceneManager)->Init())
	{
		return false;
	}

	//if (false == GET_SINGLETON(ExcelManager)->Init())
	//{
	//	return false;
	//}

	return true;
}


int Core::Run()
{
	MSG Message;

	// 기본 메시지 루프입니다.
	while (m_bLoop)
	{
		// PeekMessage : 메세지가 없을때는 FALSE를 반환하면서 바로 빠져나온다.
		// 메세지가 있을 경우 TRUE를 반환하게 된다.
		// 이 메세지를 이용하면 윈도우의 데드타임을 이용해서 게임을 제작할 수 있다.
		if (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		else
		{
			// 실제 게임 구현부분은 여기에 들어와야 한다.
			GameLogic();
		}
	}

	return (int)Message.wParam;
}

void Core::GameLogic()
{
	// 일반 srand 시드설정
	srand((unsigned int)time(NULL));

	// random 위한 시드 설정
	std::random_device rn;
	std::mt19937_64 rnd(rn());

	Timer* pTimer = GET_SINGLETON(TimerManager)->FindTimer("MainThread");
	pTimer->Update();

	float fTime = pTimer->GetTime();

	GET_SINGLETON(SoundManager)->Update(fTime);

	if (Input(fTime) == SC_NEXT)
	{
		return;
	}

	if (Update(fTime) == SC_NEXT)
	{
		return;
	}

	if (LateUpdate(fTime) == SC_NEXT)
	{
		return;
	}
	Collision(fTime);
	Render(fTime);
}

//

int Core::Input(float _fTime)
{
	//GET_SINGLETON(SceneManager)->Input(_fTime);
	GET_SINGLETON(InputManager)->Update(_fTime);
	return SC_NONE;
}

int Core::Update(float _fTime)
{
	int SceneType = GET_SINGLETON(SceneManager)->Update(_fTime);
	return SceneType;
}

int Core::LateUpdate(float _fTime)
{
	int SceneType =  GET_SINGLETON(SceneManager)->LateUpdate(_fTime);
	return SceneType;
}

int Core::Collision(float _fTime)
{
	/*GET_SINGLETON(SceneManager)->Collision(_fTime);*/
	GET_SINGLETON(CollisionManager)->Collision(_fTime);
	return SC_NONE;
}

int Core::Render(float _fTime)
{
	// 매 프레임 시작부분에서 렌더타겟, 뎁스스텐실뷰 Clear 필요 ( 다시그려야하니깡)
	GET_SINGLETON(Device)->Clear(m_fClearColor);

	// 이제 씬매니저에서 Render돌릴 필요가 없음 RenderManager에서 돌려줄거임 ㅇㅅㅇ
	int SceneType = GET_SINGLETON(SceneManager)->Render(_fTime);

	// 인스턴싱할걸 미리 계산한 후에 렌더한다.
	GET_SINGLETON(RenderManager)->ComputeInstancing();


	GET_SINGLETON(RenderManager)->Render(_fTime);

#pragma region 숙제1. UpdateSbuResource()로 삼각형 움직이기 ㅇㅅㅇ
	//Shader* pShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_STANDARD_COLOR);
	//ID3D11InputLayout* pLayout = GET_SINGLETON(ShaderManager)->FindInputLayout(LAYOUT_POSITION_COLOR);
	//Mesh* pMesh = GET_SINGLETON(ResourcesManager)->FindMesh("ColorTriangle");

	//VertexBuffer TestBuffer = pMesh->m_MeshContainerVec.back()->tVB;

	//if (0x8000 & GetAsyncKeyState(VK_LEFT))
	//{
	//	TestVColor[0].vPos.x -= 0.001f;
	//	TestVColor[1].vPos.x -= 0.001f;
	//	TestVColor[2].vPos.x -= 0.001f;
	//}
	//else if (0x8000 & GetAsyncKeyState(VK_UP))
	//{
	//	TestVColor[0].vPos.y += 0.001f;
	//	TestVColor[1].vPos.y += 0.001f;
	//	TestVColor[2].vPos.y += 0.001f;
	//}
	//else if (0x8000 & GetAsyncKeyState(VK_RIGHT))
	//{
	//	TestVColor[0].vPos.x += 0.001f;
	//	TestVColor[1].vPos.x += 0.001f;
	//	TestVColor[2].vPos.x += 0.001f;
	//}
	//else if (0x8000 & GetAsyncKeyState(VK_DOWN))
	//{
	//	TestVColor[0].vPos.y -= 0.001f;
	//	TestVColor[1].vPos.y -= 0.001f;
	//	TestVColor[2].vPos.y -= 0.001f;
	//}

	//VertexBuffer ChangeBuffer = {};
	//ChangeBuffer.iSize = pMesh->m_MeshContainerVec.back()->tVB.iSize;
	//ChangeBuffer.iCount = pMesh->m_MeshContainerVec.back()->tVB.iCount;
	//ChangeBuffer.eUsage = pMesh->m_MeshContainerVec.back()->tVB.eUsage;
	//ChangeBuffer.ePrimitive = pMesh->m_MeshContainerVec.back()->tVB.ePrimitive;
	//ChangeBuffer.pData = new char[ChangeBuffer.iSize * ChangeBuffer.iCount];
	//memcpy(ChangeBuffer.pData, TestVColor, ChangeBuffer.iSize * ChangeBuffer.iCount);

	////memcpy(pMesh->m_MeshContainerVec.back()->tVB.pData, TestVColor, pMesh->m_MeshContainerVec.back()->tVB.iSize);

	////pMesh->m_MeshContainerVec.back()->tVB.pData = TestVColor;

	//_CONTEXT->IASetInputLayout(pLayout);

	//_CONTEXT->UpdateSubresource(pMesh->m_MeshContainerVec.back()->tVB.pBuffer, 0, NULL, &TestVColor, ChangeBuffer.iSize, 0);

	//pShader->SetShader();

	//pMesh->Render();

	//SAFE_RELEASE(pShader);
	//SAFE_RELEASE(pMesh);

	//delete[] ChangeBuffer.pData;
#pragma endregion

	GET_SINGLETON(InputManager)->Render(_fTime);

	GET_SINGLETON(Device)->Present();				// 후면 버퍼를 전면 버퍼와 교환 (화면에 그려주는거)

	return SceneType;
}

//

void Core::Register(const TCHAR* _pClassName, int _iIconID, int _iSmallIconID)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Core::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_hInst;
	wcex.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(_iIconID));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;// MAKEINTRESOURCEW(IDC_MY180629);
	wcex.lpszClassName = _pClassName;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(_iSmallIconID));

	RegisterClassEx(&wcex);
}

void Core::CreateWnd(const TCHAR* _pCTitle, const TCHAR* _pClassName)
{
	m_hWnd = CreateWindow(_pClassName, _pCTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, m_tResolution.iWidth, m_tResolution.iHeight,
		nullptr, nullptr, m_hInst, nullptr);

	if (!m_hWnd)
		return;

	RECT		rc = { 0, 0, m_tResolution.iWidth, m_tResolution.iHeight };

	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	SetWindowPos(m_hWnd, HWND_TOPMOST, 100, 100, rc.right - rc.left, rc.bottom - rc.top,
		SWP_NOMOVE | SWP_NOZORDER);

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);
}

LRESULT CALLBACK Core::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSEWHEEL:
		GET_SINGLETON(InputManager)->SetWheel(HIWORD(wParam));
		break;
	case WM_DESTROY:
		m_bLoop = false;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}