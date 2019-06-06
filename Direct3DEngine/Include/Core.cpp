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
	m_bEditMode = _bEditMode;			// ����Ʈ ��� ����

	// ����̽� �ʱ�ȭ
	if (!GET_SINGLETON(Device)->Init(_hWnd, _iWidth, _iHeight, _bWindowMode, _iSamplerCount))
	{
		return false;
	}

	// ��ΰ����� �ʱ�ȭ
	if (false == GET_SINGLETON(PathManager)->Init())
	{
		return false;
	}

	// Font ������ �ʱ�ȭ
	if (false == GET_SINGLETON(FontManager)->Init())
	{
		return false;
	}

	// ���� ������ �ʱ�ȭ
	if (false == GET_SINGLETON(SoundManager)->Init())
	{
		return false;
	}

	// �Է� ������ �ʱ�ȭ
	if (false == GET_SINGLETON(InputManager)->Init(_hInst, _hWnd))
	{
		return false;
	}

	// ���ҽ� ������ �ʱ�ȭ
	if (false == GET_SINGLETON(ResourcesManager)->Init())
	{
		return false;
	}

	// ������ ������ �ʱ�ȭ
	if (false == GET_SINGLETON(RenderManager)->Init())
	{
		return false;
	}

	// �ð� ������ �ʱ�ȭ
	if (false == GET_SINGLETON(TimerManager)->Init())
	{
		return false;
	}

	// �浹 ������ �ʱ�ȭ
	if (false == GET_SINGLETON(CollisionManager)->Init())
	{
		return false;
	}

	// �׺���̼� ������
	if (false == GET_SINGLETON(NavigationManager)->Init())
	{
		return false;
	}

	// �� ������ �ʱ�ȭ
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

	// �⺻ �޽��� �����Դϴ�.
	while (m_bLoop)
	{
		// PeekMessage : �޼����� �������� FALSE�� ��ȯ�ϸ鼭 �ٷ� �������´�.
		// �޼����� ���� ��� TRUE�� ��ȯ�ϰ� �ȴ�.
		// �� �޼����� �̿��ϸ� �������� ����Ÿ���� �̿��ؼ� ������ ������ �� �ִ�.
		if (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		else
		{
			// ���� ���� �����κ��� ���⿡ ���;� �Ѵ�.
			GameLogic();
		}
	}

	return (int)Message.wParam;
}

void Core::GameLogic()
{
	// �Ϲ� srand �õ弳��
	srand((unsigned int)time(NULL));

	// random ���� �õ� ����
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
	// �� ������ ���ۺκп��� ����Ÿ��, �������ٽǺ� Clear �ʿ� ( �ٽñ׷����ϴϱ�)
	GET_SINGLETON(Device)->Clear(m_fClearColor);

	// ���� ���Ŵ������� Render���� �ʿ䰡 ���� RenderManager���� �����ٰ��� ������
	int SceneType = GET_SINGLETON(SceneManager)->Render(_fTime);

	// �ν��Ͻ��Ұ� �̸� ����� �Ŀ� �����Ѵ�.
	GET_SINGLETON(RenderManager)->ComputeInstancing();


	GET_SINGLETON(RenderManager)->Render(_fTime);

#pragma region ����1. UpdateSbuResource()�� �ﰢ�� �����̱� ������
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

	GET_SINGLETON(Device)->Present();				// �ĸ� ���۸� ���� ���ۿ� ��ȯ (ȭ�鿡 �׷��ִ°�)

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