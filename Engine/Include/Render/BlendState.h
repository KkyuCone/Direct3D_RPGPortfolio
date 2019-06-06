#pragma once
#include "RenderState.h"

ENGINE_BEGIN

class ENGINE_DLL BlendState : public RenderState
{
private:
	friend class RenderManager;

public:
	BlendState();
	~BlendState();

private:
	// 블렌드스테이트 생성시 필요한 구조체 모음
	std::vector<D3D11_RENDER_TARGET_BLEND_DESC>	m_BlendDescVec;
	
	float	m_fBlendFactor[4];			// Blend Factor
	UINT	m_iSampleMask;
	float	m_fOldBlendFactor[4];
	UINT	m_iOldSampleMask;

public:
	// BlendState 생성
	bool CreateState(const std::string& _strName, BOOL _bAlphaToCoverage = FALSE,
		BOOL _bIndependentBlend = FALSE);

	// BlendState 생성시 필요한 구조체 만들기 
	void AddRenderTargetBlendDesc(BOOL _bEnable = FALSE,
		D3D11_BLEND _eSrcBlend = D3D11_BLEND_ONE,
		D3D11_BLEND _eDestBlend = D3D11_BLEND_ZERO,
		D3D11_BLEND_OP _eOp = D3D11_BLEND_OP_ADD,
		D3D11_BLEND _eSrcBlendAlpha = D3D11_BLEND_ONE,
		D3D11_BLEND _eDestBlendAlpha = D3D11_BLEND_ZERO,
		D3D11_BLEND_OP _eAlphaOp = D3D11_BLEND_OP_ADD,
		UINT _iWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL);

public:
	virtual void SetState();
	virtual void ResetState();
};

ENGINE_END
