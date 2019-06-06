#pragma once
#include "RenderState.h"

ENGINE_BEGIN

class ENGINE_DLL DepthStencilState : public RenderState
{
private:
	friend class RenderManager;

public:
	DepthStencilState();
	~DepthStencilState();

private:
	UINT	m_iCurStencilRef;
	UINT	m_iOldStencilRef;

public:
	bool CreateState(const std::string& _strName, BOOL _bEnable = TRUE,
		D3D11_DEPTH_WRITE_MASK _eWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
		D3D11_COMPARISON_FUNC _eDepthFunc = D3D11_COMPARISON_LESS);

public:
	virtual void SetState();
	virtual void ResetState();
};

ENGINE_END

