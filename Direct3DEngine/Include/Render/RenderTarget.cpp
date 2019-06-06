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
	// 兄希展為 戚硯 走嬢爽奄
	m_strName = _strName;

	// 適軒嬢事雌 隔嬢爽奄
	memcpy(m_fCelarColor, _fClearColor, sizeof(float) * 4);

	// 努什坦 姥繕端 葵 隔嬢爽奄 ( 兄希展為遂 )
	D3D11_TEXTURE2D_DESC tDesc = {};

	tDesc.Width = _iWidth;			// 亜稽 滴奄
	tDesc.Height = _iHeight;		// 室稽 滴奄
	tDesc.ArraySize = 1;			// 努什坦 鯵呪 (廃鯵績)
	tDesc.Format = _eFormat;		// 努什坦 匂庫 隔嬢爽奄 (兄希展為遂)
	tDesc.MipLevels = 1;			// 荒傾婚 1, 奄沙生稽..
	tDesc.SampleDesc.Quality = 0;
	tDesc.SampleDesc.Count = _iSampleCount;
	tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;		// 捨戚希 軒社什 坂, 兄希展為遂
	tDesc.Usage = D3D11_USAGE_DEFAULT;

	// 兄希展為 努什坦研 持失背層陥.
	if (FAILED(_DEVICE->CreateTexture2D(&tDesc, nullptr, &m_pRenderTargetTexture)))
	{
		return false;
	}

	// 捨戚希 軒社什坂研 幻級嬢層陥.
	if (FAILED(_DEVICE->CreateShaderResourceView(m_pRenderTargetTexture, nullptr,
		&m_pRenderTargetSRV)))
	{
		return false;
	}

	// 兄希展為坂研 幻級嬢層陥. ( 努什坦 持失 -> 捨戚希軒社什坂 持失 -> 兄希展為坂持失)
	if (FAILED(_DEVICE->CreateRenderTargetView(m_pRenderTargetTexture, nullptr,
		&m_pRenderTargetView)))
	{
		return false;
	}

	
	// 紀什研 奄沙葵生稽 竺舛馬走 省走幻
	// 燕戚葵聖 竺舛馬澗 井酔研 坦軒背層陥.
	if (_eDepthFormat != DXGI_FORMAT_UNKNOWN)
	{
		// 背雁 努什坦 姥繕端税 Foramt葵, Flag葵 痕井
		tDesc.Format = _eDepthFormat;
		tDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		// 燕戚遂 努什坦 幻級奄
		if (FAILED(_DEVICE->CreateTexture2D(&tDesc, nullptr, &m_pDepthTexture)))
		{
			return false;
		}

		// 燕戚坂 幻級奄
		if (FAILED(_DEVICE->CreateDepthStencilView(m_pDepthTexture, nullptr, &m_pDepthView)))
		{
			return false;
		}
	}

	// 燈慎楳慶  ( 送嘘 )  -> 城 巨獄益研遂 兄希研 是背辞 紫遂
	*m_pProj = XMMatrixOrthographicOffCenterLH(0.0f, (float)(_RESOLUTION.iWidth),
		0.0f, (float)(_RESOLUTION.iHeight), 0.0f, 1000.0f);

	return true;
}

void RenderTarget::OnDebug(const Vector3 & _vPos, const Vector3 & _vScale, bool _bDebug/* = true*/)
{
	// 巨獄益遂 葵拭魚虞 佃爽暗蟹 襖爽暗蟹
	m_bDebug = _bDebug;
	m_vPosition = _vPos;
	m_vScale = _vScale;

	// 兄希展為聖 兄希拝 五習亜 蒸生檎 達焼辞 隔嬢爽奄
	if (nullptr == m_pMesh)
	{
		m_pMesh = GET_SINGLETON(ResourcesManager)->FindMesh("Rectangle");
		m_pShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_DEBUG);
		m_pLayout = GET_SINGLETON(ShaderManager)->FindInputLayout(LAYOUT_POSITION_UV);
	}
}

void RenderTarget::ClearRenderTarget()
{
	// 背雁 展為 走趨爽奄
	// 兄希廃 板拭澗 益 展為戚 琶推蒸生糠稽 走趨層陥. ( 益軒壱 陥獣 紫遂拝暗績 しさし )
	_CONTEXT->ClearRenderTargetView(m_pRenderTargetView, m_fCelarColor);

	// 紀什亜 糎仙廃陥檎 (燕戚亜 赤陥檎) 燕戚亀 走趨層陥.
	if (nullptr != m_pDepthView)
	{
		_CONTEXT->ClearDepthStencilView(m_pDepthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}

void RenderTarget::SetRenderTarget()
{
	// 兄希展為聖 実特背層陥...
	// 析舘 奄糎税 走舛吉 展為聖 条嬢紳陥.
	// 奄糎税 展為聖 条嬢臣井酔 傾遁訓什 朝錘斗亜 装亜馬惟吉陥. ( 益君糠稽 陥 紫遂廃 陥製拭澗 Release研 背操醤敗 )
	_CONTEXT->OMGetRenderTargets(1, &m_pOldRenderTargetView, &m_pOldDepthView);

	// 幻鉦 DepthView亜 設 幻級嬢然陥檎 背雁 DepthView稽 走舛馬壱
	// 蒸陥檎 奄糎拭 赤澗 Depth稽 走舛背層陥.
	ID3D11DepthStencilView*	pDepth = m_pDepthView;

	if (nullptr == pDepth)
	{
		pDepth = m_pOldDepthView;
	}

	_CONTEXT->OMSetRenderTargets(1, &m_pRenderTargetView, pDepth);
}

void RenderTarget::ResetRenderTarget()
{
	// 条嬢紳 展為級聖 据掘企稽 鞠宜形層陥.
	_CONTEXT->OMSetRenderTargets(1, &m_pOldRenderTargetView, m_pOldDepthView);
	
	// 傾遁訓什 朝錘斗研 姶社獣轍陥.
	SAFE_RELEASE(m_pOldRenderTargetView);
	SAFE_RELEASE(m_pOldDepthView);
}

void RenderTarget::SetShader(int _iRegister)
{
	// 巨獄益遂 兄希展為聖 是背辞 捨戚希研 実特背層陥. (鉢檎 窒径聖 是背辞 琶推敗 しさし)
	// 捨戚希 軒社什 壕伸聖 波漆 捨戚希 楕拭 郊昔漁背捜 益撹 尻衣背捜
	// 捨戚希坪球拭辞 紫遂背醤馬艦猿 しし
	_CONTEXT->PSSetShaderResources(_iRegister, 1, &m_pRenderTargetSRV);
}

void RenderTarget::SetCShader(int _iRegister)
{
	_CONTEXT->CSSetShaderResources(_iRegister, 1, &m_pRenderTargetSRV);
}

void RenderTarget::ResetShader(int _iRegister)
{
	// 陥床壱蟹辞澗 鋼球獣 nullptr聖 隔嬢操醤馬澗汽 
	// 戚凶 益企稽 nullptr聖 隔聖 井酔 五乞軒 滴君習亜 降持廃陥.
	// 壱稽 ID3D11ShdaerResourceVeiw聖 nullptr稽 段奄鉢 廃板拭 隔嬢層陥.
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
	// 兄希展為級聖 兄希背爽奄 しさし (巨獄益遂戚陥.)
	if (false == m_bDebug)
	{
		return;
	}

	m_pShader->SetShader();

	Matrix ScaleMat, TranslateMat;
	ScaleMat.Scaling(m_vScale);
	TranslateMat.Translation(m_vPosition);

	// 巨獄益遂 捨戚希税 雌呪獄遁稽 杉球楳慶聖 隔嬢操醤廃陥.
	// 捨戚希拭辞 域至 いい.. 吃呪 赤生檎 捨戚希拭識 域至馬澗杏 杷背操醤敗
	DebugCBuffer	tCBuffer = {};
	tCBuffer.WVPMatrix = ScaleMat * TranslateMat * (*m_pProj);
	tCBuffer.WVPMatrix.Transpose();

	// 雌呪獄遁 穣汽戚闘
	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("Debug", &tCBuffer);

	SetShader(0);		// 雌呪獄遁 十茎戚 0 腰属績 (実特)

	_CONTEXT->IASetInputLayout(m_pLayout);

	m_pMesh->Render();

	ResetShader(0);		// 兄希研 背層及 伽 軒実 背操醤廃陥.
}
