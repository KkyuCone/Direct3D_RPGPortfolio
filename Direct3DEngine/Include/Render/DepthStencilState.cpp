#include "DepthStencilState.h"
#include "..\Device.h"

ENGINE_USING

DepthStencilState::DepthStencilState()
{
	m_iCurStencilRef = 0xffffffff;
	m_eType = RS_DEPTH;
}


DepthStencilState::~DepthStencilState()
{
}

bool DepthStencilState::CreateState(const std::string & _strName, BOOL _bEnable/* = TRUE*/
	, D3D11_DEPTH_WRITE_MASK _eWriteMask/* = D3D11_DEPTH_WRITE_MASK_ALL*/
	, D3D11_COMPARISON_FUNC _eDepthFunc /*= D3D11_COMPARISON_LESS*/)
{
	SetTag(_strName.c_str());

	D3D11_DEPTH_STENCIL_DESC	tDesc = {};

	tDesc.DepthEnable = _bEnable;
	tDesc.DepthWriteMask = _eWriteMask;
	tDesc.DepthFunc = _eDepthFunc;
	
	tDesc.StencilEnable = FALSE;
	tDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	tDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	tDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	tDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	tDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	tDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	tDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	tDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	tDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	tDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	if (FAILED(_DEVICE->CreateDepthStencilState(&tDesc,
		(ID3D11DepthStencilState**)&m_pCurState)))
	{
		return false;
	}

	return true;
}

void DepthStencilState::SetState()
{
	_CONTEXT->OMGetDepthStencilState((ID3D11DepthStencilState**)&m_pOldState,
		&m_iOldStencilRef);

	_CONTEXT->OMSetDepthStencilState((ID3D11DepthStencilState*)m_pCurState,
		m_iCurStencilRef);
}

void DepthStencilState::ResetState()
{
	_CONTEXT->OMSetDepthStencilState((ID3D11DepthStencilState*)m_pOldState,
		m_iOldStencilRef);
	SAFE_RELEASE(m_pOldState);
}
