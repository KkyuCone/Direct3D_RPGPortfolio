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
	// ����Ÿ�� �̸� �����ֱ�
	m_strName = _strName;

	// Ŭ������� �־��ֱ�
	memcpy(m_fCelarColor, _fClearColor, sizeof(float) * 4);

	// �ؽ�ó ����ü �� �־��ֱ� ( ����Ÿ�ٿ� )
	D3D11_TEXTURE2D_DESC tDesc = {};

	tDesc.Width = _iWidth;			// ���� ũ��
	tDesc.Height = _iHeight;		// ���� ũ��
	tDesc.ArraySize = 1;			// �ؽ�ó ���� (�Ѱ���)
	tDesc.Format = _eFormat;		// �ؽ�ó ���� �־��ֱ� (����Ÿ�ٿ�)
	tDesc.MipLevels = 1;			// �ӷ��� 1, �⺻����..
	tDesc.SampleDesc.Quality = 0;
	tDesc.SampleDesc.Count = _iSampleCount;
	tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;		// ���̴� ���ҽ� ��, ����Ÿ�ٿ�
	tDesc.Usage = D3D11_USAGE_DEFAULT;

	// ����Ÿ�� �ؽ�ó�� �������ش�.
	if (FAILED(_DEVICE->CreateTexture2D(&tDesc, nullptr, &m_pRenderTargetTexture)))
	{
		return false;
	}

	// ���̴� ���ҽ��並 ������ش�.
	if (FAILED(_DEVICE->CreateShaderResourceView(m_pRenderTargetTexture, nullptr,
		&m_pRenderTargetSRV)))
	{
		return false;
	}

	// ����Ÿ�ٺ並 ������ش�. ( �ؽ�ó ���� -> ���̴����ҽ��� ���� -> ����Ÿ�ٺ����)
	if (FAILED(_DEVICE->CreateRenderTargetView(m_pRenderTargetTexture, nullptr,
		&m_pRenderTargetView)))
	{
		return false;
	}

	
	// ������ �⺻������ �������� ������
	// ���̰��� �����ϴ� ��츦 ó�����ش�.
	if (_eDepthFormat != DXGI_FORMAT_UNKNOWN)
	{
		// �ش� �ؽ�ó ����ü�� Foramt��, Flag�� ����
		tDesc.Format = _eDepthFormat;
		tDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		// ���̿� �ؽ�ó �����
		if (FAILED(_DEVICE->CreateTexture2D(&tDesc, nullptr, &m_pDepthTexture)))
		{
			return false;
		}

		// ���̺� �����
		if (FAILED(_DEVICE->CreateDepthStencilView(m_pDepthTexture, nullptr, &m_pDepthView)))
		{
			return false;
		}
	}

	// �������  ( ���� )  -> �� ����׸��� ������ ���ؼ� ���
	*m_pProj = XMMatrixOrthographicOffCenterLH(0.0f, (float)(_RESOLUTION.iWidth),
		0.0f, (float)(_RESOLUTION.iHeight), 0.0f, 1000.0f);

	return true;
}

void RenderTarget::OnDebug(const Vector3 & _vPos, const Vector3 & _vScale, bool _bDebug/* = true*/)
{
	// ����׿� �������� ���ְų� ���ְų�
	m_bDebug = _bDebug;
	m_vPosition = _vPos;
	m_vScale = _vScale;

	// ����Ÿ���� ������ �޽��� ������ ã�Ƽ� �־��ֱ�
	if (nullptr == m_pMesh)
	{
		m_pMesh = GET_SINGLETON(ResourcesManager)->FindMesh("Rectangle");
		m_pShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_DEBUG);
		m_pLayout = GET_SINGLETON(ShaderManager)->FindInputLayout(LAYOUT_POSITION_UV);
	}
}

void RenderTarget::ClearRenderTarget()
{
	// �ش� Ÿ�� �����ֱ�
	// ������ �Ŀ��� �� Ÿ���� �ʿ�����Ƿ� �����ش�. ( �׸��� �ٽ� ����Ұ��� ������ )
	_CONTEXT->ClearRenderTargetView(m_pRenderTargetView, m_fCelarColor);

	// ������ �����Ѵٸ� (���̰� �ִٸ�) ���̵� �����ش�.
	if (nullptr != m_pDepthView)
	{
		_CONTEXT->ClearDepthStencilView(m_pDepthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}

void RenderTarget::SetRenderTarget()
{
	// ����Ÿ���� �������ش�...
	// �ϴ� ������ ������ Ÿ���� ���´�.
	// ������ Ÿ���� ���ð�� ���۷��� ī���Ͱ� �����ϰԵȴ�. ( �׷��Ƿ� �� ����� �������� Release�� ������� )
	_CONTEXT->OMGetRenderTargets(1, &m_pOldRenderTargetView, &m_pOldDepthView);

	// ���� DepthView�� �� ��������ٸ� �ش� DepthView�� �����ϰ�
	// ���ٸ� ������ �ִ� Depth�� �������ش�.
	ID3D11DepthStencilView*	pDepth = m_pDepthView;

	if (nullptr == pDepth)
	{
		pDepth = m_pOldDepthView;
	}

	_CONTEXT->OMSetRenderTargets(1, &m_pRenderTargetView, pDepth);
}

void RenderTarget::ResetRenderTarget()
{
	// ���� Ÿ�ٵ��� ������� �ǵ����ش�.
	_CONTEXT->OMSetRenderTargets(1, &m_pOldRenderTargetView, m_pOldDepthView);
	
	// ���۷��� ī���͸� ���ҽ�Ų��.
	SAFE_RELEASE(m_pOldRenderTargetView);
	SAFE_RELEASE(m_pOldDepthView);
}

void RenderTarget::SetShader(int _iRegister)
{
	// ����׿� ����Ÿ���� ���ؼ� ���̴��� �������ش�. (ȭ�� ����� ���ؼ� �ʿ��� ������)
	// ���̴� ���ҽ� �迭�� �ȼ� ���̴� �ʿ� ���ε����� �׳� ��������
	// ���̴��ڵ忡�� ����ؾ��ϴϱ� ����
	_CONTEXT->PSSetShaderResources(_iRegister, 1, &m_pRenderTargetSRV);
}

void RenderTarget::SetCShader(int _iRegister)
{
	_CONTEXT->CSSetShaderResources(_iRegister, 1, &m_pRenderTargetSRV);
}

void RenderTarget::ResetShader(int _iRegister)
{
	// �پ������� �ݵ�� nullptr�� �־�����ϴµ� 
	// �̶� �״�� nullptr�� ���� ��� �޸� ũ������ �߻��Ѵ�.
	// ��� ID3D11ShdaerResourceVeiw�� nullptr�� �ʱ�ȭ ���Ŀ� �־��ش�.
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
	// ����Ÿ�ٵ��� �������ֱ� ������ (����׿��̴�.)
	if (false == m_bDebug)
	{
		return;
	}

	m_pShader->SetShader();

	Matrix ScaleMat, TranslateMat;
	ScaleMat.Scaling(m_vScale);
	TranslateMat.Translation(m_vPosition);

	// ����׿� ���̴��� ������۷� ��������� �־�����Ѵ�.
	// ���̴����� ��� ����.. �ɼ� ������ ���̴����� ����ϴ°� ���������
	DebugCBuffer	tCBuffer = {};
	tCBuffer.WVPMatrix = ScaleMat * TranslateMat * (*m_pProj);
	tCBuffer.WVPMatrix.Transpose();

	// ������� ������Ʈ
	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("Debug", &tCBuffer);

	SetShader(0);		// ������� ������ 0 ��°�� (����)

	_CONTEXT->IASetInputLayout(m_pLayout);

	m_pMesh->Render();

	ResetShader(0);		// ������ ���ص� �� ���� ������Ѵ�.
}
