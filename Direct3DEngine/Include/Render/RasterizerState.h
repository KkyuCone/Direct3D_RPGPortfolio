#pragma once
#include "RenderState.h"

ENGINE_BEGIN

class ENGINE_DLL RasterizerState : public RenderState
{
private:
	friend class RenderManager;

private:
	RasterizerState();
	~RasterizerState();

public:
	bool CreateState(const std::string& _strName, D3D11_FILL_MODE _eFill = D3D11_FILL_SOLID,
		D3D11_CULL_MODE _eCull = D3D11_CULL_BACK);

public:
	virtual void SetState();
	virtual void ResetState();
};

ENGINE_END