#include "RenderTarget.h"
#include "..\Device.h"
#include "..\Resource\Mesh.h"
#include "..\Resource\ResourcesManager.h"
#include "Shader.h"
#include "ShaderManager.h"

ENGINE_USING

RenderTarget::RenderTarget() :
	m_pRenderTargetView(nullptr),
	m_pRenderTargetTexture(nullptr),
	m_pRenderTargetSRV(nullptr),
	m_pDepthView(nullptr),
	m_pDepthTexture(nullptr),
	m_pOldRenderTargetView(nullptr),
	m_pOldDepthView(nullptr),
	m_bDebug(false),
	m_pMesh(nullptr),
	m_pShader(nullptr),
	m_pLayout(nullptr)
{
	m_pProj = new Matrix;
}


RenderTarget::~RenderTarget()
{
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_pMesh);
	SAFE_DELETE(m_pProj);
	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pRenderTargetTexture);
	SAFE_RELEASE(m_pRenderTargetSRV);
	SAFE_RELEASE(m_pDepthView);
	SAFE_RELEASE(m_pDepthTexture);
	SAFE_RELEASE(m_pOldRenderTargetView);
	SAFE_RELEASE(m_pOldDepthView);
}

ID3D11RenderTargetView * RenderTarget::GetRenderTargetView() const
{
	return m_pRenderTargetView;
}

ID3D11DepthStencilView * RenderTarget::GetDepthStencilView() const
{
	return m_pDepthView;
}

ID3D11ShaderResourceView * RenderTarget::GetShaderResourceView() const
{
	return m_pRenderTargetSRV;
}

bool RenderTarget::CreateRenderTarget(const std::string & _strName, 
	UINT _iWidth, UINT _iHeight, 
	DXGI_FORMAT _eFormat,
	float _fClearColor[4],  
	int _iSampleCount /*= 1*/,
	DXGI_FORMAT _eDepthFormat /*= DXGI_FORMAT_UNKNOWN*/)
{
	// 렌더타겟 이름 지어주기
	m_strName = _strName;

	// 클리어색상 넣어주기
	memcpy(m_fCelarColor, _fClearColor, sizeof(float) * 4);

	// 텍스처 구조체 값 넣어주기 ( 렌더타겟용 )
	D3D11_TEXTURE2D_DESC tDesc = {};

	tDesc.Width = _iWidth;			// 가로 크기
	tDesc.Height = _iHeight;		// 세로 크기
	tDesc.ArraySize = 1;			// 텍스처 개수 (한개임)
	tDesc.Format = _eFormat;		// 텍스처 포맷 넣어주기 (렌더타겟용)
	tDesc.MipLevels = 1;			// 밉레벨 1, 기본으로..
	tDesc.SampleDesc.Quality = 0;
	tDesc.SampleDesc.Count = _iSampleCount;
	tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;		// 셰이더 리소스 뷰, 렌더타겟용
	tDesc.Usage = D3D11_USAGE_DEFAULT;

	// 렌더타겟 텍스처를 생성해준다.
	if (FAILED(_DEVICE->CreateTexture2D(&tDesc, nullptr, &m_pRenderTargetTexture)))
	{
		return false;
	}

	// 셰이더 리소스뷰를 만들어준다.
	if (FAILED(_DEVICE->CreateShaderResourceView(m_pRenderTargetTexture, nullptr,
		&m_pRenderTargetSRV)))
	{
		return false;
	}

	// 렌더타겟뷰를 만들어준다. ( 텍스처 생성 -> 셰이더리소스뷰 생성 -> 렌더타겟뷰생성)
	if (FAILED(_DEVICE->CreateRenderTargetView(m_pRenderTargetTexture, nullptr,
		&m_pRenderTargetView)))
	{
		return false;
	}

	
	// 뎁스를 기본값으로 설정하지 않지만
	// 깊이값을 설정하는 경우를 처리해준다.
	if (_eDepthFormat != DXGI_FORMAT_UNKNOWN)
	{
		// 해당 텍스처 구조체의 Foramt값, Flag값 변경
		tDesc.Format = _eDepthFormat;
		tDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		// 깊이용 텍스처 만들기
		if (FAILED(_DEVICE->CreateTexture2D(&tDesc, nullptr, &m_pDepthTexture)))
		{
			return false;
		}

		// 깊이뷰 만들기
		if (FAILED(_DEVICE->CreateDepthStencilView(m_pDepthTexture, nullptr, &m_pDepthView)))
		{
			return false;
		}
	}

	// 투영행렬  ( 직교 )  -> 얜 디버그를용 렌더를 위해서 사용
	*m_pProj = XMMatrixOrthographicOffCenterLH(0.0f, (float)(_RESOLUTION.iWidth),
		0.0f, (float)(_RESOLUTION.iHeight), 0.0f, 1000.0f);

	return true;
}

void RenderTarget::OnDebug(const Vector3 & _vPos, const Vector3 & _vScale, bool _bDebug/* = true*/)
{
	// 디버그용 값에따라 켜주거나 꺼주거나
	m_bDebug = _bDebug;
	m_vPosition = _vPos;
	m_vScale = _vScale;

	// 렌더타겟을 렌더할 메쉬가 없으면 찾아서 넣어주기
	if (nullptr == m_pMesh)
	{
		m_pMesh = GET_SINGLETON(ResourcesManager)->FindMesh("Rectangle");
		m_pShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_DEBUG);
		m_pLayout = GET_SINGLETON(ShaderManager)->FindInputLayout(LAYOUT_POSITION_UV);
	}
}

void RenderTarget::ClearRenderTarget()
{
	// 해당 타겟 지워주기
	// 렌더한 후에는 그 타겟이 필요없으므로 지워준다. ( 그리고 다시 사용할거임 ㅇㅅㅇ )
	_CONTEXT->ClearRenderTargetView(m_pRenderTargetView, m_fCelarColor);

	// 뎁스가 존재한다면 (깊이가 있다면) 깊이도 지워준다.
	if (nullptr != m_pDepthView)
	{
		_CONTEXT->ClearDepthStencilView(m_pDepthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}

void RenderTarget::SetRenderTarget()
{
	// 렌더타겟을 셋팅해준다...
	// 일단 기존의 지정된 타겟을 얻어온다.
	// 기존의 타겟을 얻어올경우 레퍼런스 카운터가 증가하게된다. ( 그러므로 다 사용한 다음에는 Release를 해줘야함 )
	_CONTEXT->OMGetRenderTargets(1, &m_pOldRenderTargetView, &m_pOldDepthView);

	// 만약 DepthView가 잘 만들어졌다면 해당 DepthView로 지정하고
	// 없다면 기존에 있는 Depth로 지정해준다.
	ID3D11DepthStencilView*	pDepth = m_pDepthView;

	if (nullptr == pDepth)
	{
		pDepth = m_pOldDepthView;
	}

	_CONTEXT->OMSetRenderTargets(1, &m_pRenderTargetView, pDepth);
}

void RenderTarget::ResetRenderTarget()
{
	// 얻어온 타겟들을 원래대로 되돌려준다.
	_CONTEXT->OMSetRenderTargets(1, &m_pOldRenderTargetView, m_pOldDepthView);
	
	// 레퍼런스 카운터를 감소시킨다.
	SAFE_RELEASE(m_pOldRenderTargetView);
	SAFE_RELEASE(m_pOldDepthView);
}

void RenderTarget::SetShader(int _iRegister)
{
	// 디버그용 렌더타겟을 위해서 셰이더를 셋팅해준다. (화면 출력을 위해서 필요함 ㅇㅅㅇ)
	// 셰이더 리소스 배열을 픽셀 셰이더 쪽에 바인딩해줌 그냥 연결해줌
	// 셰이더코드에서 사용해야하니까 ㅇㅇ
	_CONTEXT->PSSetShaderResources(_iRegister, 1, &m_pRenderTargetSRV);
}

void RenderTarget::SetCShader(int _iRegister)
{
	_CONTEXT->CSSetShaderResources(_iRegister, 1, &m_pRenderTargetSRV);
}

void RenderTarget::ResetShader(int _iRegister)
{
	// 다쓰고나서는 반드시 nullptr을 넣어줘야하는데 
	// 이때 그대로 nullptr을 넣을 경우 메모리 크러쉬가 발생한다.
	// 고로 ID3D11ShdaerResourceVeiw을 nullptr로 초기화 한후에 넣어준다.
	ID3D11ShaderResourceView* pSRV = nullptr;
	_CONTEXT->PSSetShaderResources(_iRegister, 1, &pSRV);
}

void RenderTarget::ResetCShader(int _iRegister)
{
	ID3D11ShaderResourceView* pSRV = nullptr;
	_CONTEXT->CSSetShaderResources(_iRegister, 1, &pSRV);
}

void RenderTarget::Render()
{
	// 렌더타겟들을 렌더해주기 ㅇㅅㅇ (디버그용이다.)
	if (false == m_bDebug)
	{
		return;
	}

	m_pShader->SetShader();

	Matrix ScaleMat, TranslateMat;
	ScaleMat.Scaling(m_vScale);
	TranslateMat.Translation(m_vPosition);

	// 디버그용 셰이더의 상수버퍼로 월드행렬을 넣어줘야한다.
	// 셰이더에서 계산 ㄴㄴ.. 될수 있으면 셰이더에선 계산하는걸 피해줘야함
	DebugCBuffer	tCBuffer = {};
	tCBuffer.WVPMatrix = ScaleMat * TranslateMat * (*m_pProj);
	tCBuffer.WVPMatrix.Transpose();

	// 상수버퍼 업데이트
	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("Debug", &tCBuffer);

	SetShader(0);		// 상수버퍼 슬롯이 0 번째임 (셋팅)

	_CONTEXT->IASetInputLayout(m_pLayout);

	m_pMesh->Render();

	ResetShader(0);		// 렌더를 해준뒤 꼭 리셋 해줘야한다.
}
