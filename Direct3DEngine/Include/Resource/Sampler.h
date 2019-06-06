#pragma once
#include "..\Reference.h"

ENGINE_BEGIN

class ENGINE_DLL Sampler : public Reference
{
private:
	friend class GameObject;

public:
	Sampler();
	~Sampler();

private:
	ID3D11SamplerState* m_pSampler;

public:
	// default : D3D11_FILTER_MIN_MAG_MIP_LINEAR
	bool CreateSamapler(const std::string& _strName, D3D11_FILTER _eFilter,
		D3D11_TEXTURE_ADDRESS_MODE _eAddressU,
		D3D11_TEXTURE_ADDRESS_MODE _eAddressV,
		D3D11_TEXTURE_ADDRESS_MODE _eAddressW,
		D3D11_COMPARISON_FUNC _CamparisonFunc);

	void VSSetShader(int _iRegister);
	void PSSetShader(int _iRegister);
};

ENGINE_END

