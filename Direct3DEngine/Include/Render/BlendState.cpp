#include "BlendState.h"
#include "..\Device.h"

ENGINE_USING

BlendState::BlendState()
{
	m_eType = RS_BLEND;

	m_iSampleMask = 0xffffffff;						// 기본값 
	memset(m_fBlendFactor, 0, sizeof(float) * 4);	// 블렌드 팩터는 블렌드 구조체 만들때 해당되는 옵션값이 있을때만 사용함

	//m_iOldSampleMask = 0xffffffff;
	//memset(m_fOldBlendFactor, 0, sizeof(float) * 4);
}


BlendState::~BlendState()
{
}

bool BlendState::CreateState(const std::string & _strName, 
	BOOL _bAlphaToCoverage, BOOL _bIndependentBlend)
{
	SetTag(_strName.c_str());

	D3D11_BLEND_DESC	tDesc = {};

	tDesc.AlphaToCoverageEnable = _bAlphaToCoverage;
	tDesc.IndependentBlendEnable = _bIndependentBlend;

	for (size_t i = 0; i < m_BlendDescVec.size(); ++i)
	{
		tDesc.RenderTarget[i] = m_BlendDescVec[i];
	}

	if (FAILED(_DEVICE->CreateBlendState(&tDesc, (ID3D11BlendState**)&m_pCurState)))
	{
		return false;
	}

	return true;
}

void BlendState::AddRenderTargetBlendDesc(BOOL _bEnable /*= FALSE*/,
	D3D11_BLEND _eSrcBlend /*= D3D11_BLEND_ONE*/,
	D3D11_BLEND _eDestBlend /*= D3D11_BLEND_ZERO*/,
	D3D11_BLEND_OP _eOp /*= D3D11_BLEND_OP_ADD*/,
	D3D11_BLEND _eSrcBlendAlpha /*= D3D11_BLEND_ONE*/,
	D3D11_BLEND _eDestBlendAlpha /*= D3D11_BLEND_ZERO*/,
	D3D11_BLEND_OP _eAlphaOp /*= D3D11_BLEND_OP_ADD*/,
	UINT _iWriteMask/* = D3D11_COLOR_WRITE_ENABLE_ALL*/)
{
	// DirectX11의 최대 렌더타겟 개수는 8개다.
	if (8 == m_BlendDescVec.size())
	{
		return;
	}

	D3D11_RENDER_TARGET_BLEND_DESC tDesc = {};

	tDesc.BlendEnable = _bEnable;				// 블렌드 사용 여부
	tDesc.SrcBlend = _eSrcBlend;				// source Factor(원본) RGB
	tDesc.DestBlend = _eDestBlend;				// Dest Factor(백버퍼에 쓰여진, 대상) Dest
	tDesc.BlendOp = _eOp;						// RGB 블렌드 연산할 이항연산자 설정
	tDesc.SrcBlendAlpha = _eSrcBlendAlpha;		// source Alpha Factor 
	tDesc.DestBlendAlpha = _eDestBlendAlpha;	// Dest Alpha Factor
	tDesc.BlendOpAlpha = _eAlphaOp;				// Alhpha 블렌드 연산할 이항연산자 설정
	tDesc.RenderTargetWriteMask = _iWriteMask;	// 위의 계산한 픽셀중에 어떤값을 쓸건지 설정 (기본값은 전부임)

	m_BlendDescVec.push_back(tDesc);
}

void BlendState::SetState()
{
	_CONTEXT->OMGetBlendState((ID3D11BlendState**)&m_pOldState
		, m_fOldBlendFactor, &m_iOldSampleMask);

	_CONTEXT->OMSetBlendState((ID3D11BlendState*)m_pCurState,
		m_fBlendFactor, m_iSampleMask);
}

void BlendState::ResetState()
{
	_CONTEXT->OMSetBlendState((ID3D11BlendState*)m_pOldState,
		m_fOldBlendFactor, m_iOldSampleMask);

	SAFE_RELEASE(m_pOldState);
}
