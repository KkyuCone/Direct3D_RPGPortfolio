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
	tDesc.AntialiasedLineEnable = FALSE;					// DirectX���� �����ϴ� ��Ƽ�ٸ������ ������� �ʴ´�. ���̴����� ����ؼ� ó���ϴ°ɷ� �Ұ���.

	// �ٿ�ĳ�����ؾ��� -> (ID3D11RasterizerState**)&m_pCurState
	if (FAILED(_DEVICE->CreateRasterizerState(&tDesc,
		(ID3D11RasterizerState**)&m_pCurState)))
	{
		return false;
	}

	return true;
}

void RasterizerState::SetState()
{
	// ������ �����Ǿ� �־��� ���� ��������
	_CONTEXT->RSGetState((ID3D11RasterizerState**)&m_pOldState);

	// ���ο� ���¸� �����Ѵ�.
	_CONTEXT->RSSetState((ID3D11RasterizerState*)m_pCurState);
}

void RasterizerState::ResetState()
{
	// ���� ���·� ������ 
	_CONTEXT->RSSetState((ID3D11RasterizerState*)m_pOldState);

	// Get�� �� ��� ���۷��� ī���Ͱ� �����Ѵ�.
	// �׷��� ���۷��� ī���͸� ���ҽ�Ű�� ���� Release�� ����Ѵ�.
	SAFE_RELEASE(m_pOldState);
}
