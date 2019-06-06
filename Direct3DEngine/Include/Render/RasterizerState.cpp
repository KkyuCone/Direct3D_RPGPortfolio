#include "RasterizerState.h"
#include "..\Device.h"

ENGINE_USING

RasterizerState::RasterizerState()
{
	m_eType = RS_RASTERIZER;
}


RasterizerState::~RasterizerState()
{
}

bool RasterizerState::CreateState(const std::string & _strName
	, D3D11_FILL_MODE _eFill /*= D3D11_FILL_SOLID*/
	, D3D11_CULL_MODE _eCull /*= D3D11_CULL_BACK*/)
{
	SetTag(_strName.c_str());

	D3D11_RASTERIZER_DESC	tDesc = {};

	tDesc.FillMode = _eFill;
	tDesc.CullMode = _eCull;
	tDesc.FrontCounterClockwise = FALSE;
	tDesc.DepthBias = 0;
	tDesc.SlopeScaledDepthBias = 0.0f;
	tDesc.DepthBiasClamp = 0.0f;
	tDesc.DepthClipEnable = TRUE;
	tDesc.ScissorEnable = FALSE;
	tDesc.MultisampleEnable = FALSE;
	tDesc.AntialiasedLineEnable = FALSE;					// DirectX에서 제공하는 안티앨리어싱은 사용하지 않는다. 셰이더에서 계산해서 처리하는걸로 할거임.

	// 다운캐스팅해야함 -> (ID3D11RasterizerState**)&m_pCurState
	if (FAILED(_DEVICE->CreateRasterizerState(&tDesc,
		(ID3D11RasterizerState**)&m_pCurState)))
	{
		return false;
	}

	return true;
}

void RasterizerState::SetState()
{
	// 기존에 지정되어 있었던 상태 가져오기
	_CONTEXT->RSGetState((ID3D11RasterizerState**)&m_pOldState);

	// 새로운 상태를 지정한다.
	_CONTEXT->RSSetState((ID3D11RasterizerState*)m_pCurState);
}

void RasterizerState::ResetState()
{
	// 원래 상태로 돌리기 
	_CONTEXT->RSSetState((ID3D11RasterizerState*)m_pOldState);

	// Get을 할 경우 레퍼런스 카운터가 증가한다.
	// 그래서 레퍼런스 카운터를 감소시키기 위해 Release를 사용한다.
	SAFE_RELEASE(m_pOldState);
}
