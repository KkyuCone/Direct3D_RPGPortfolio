#include "Sampler.h"
#include "..\Device.h"

ENGINE_USING


Sampler::Sampler() : m_pSampler(nullptr)
{
}


Sampler::~Sampler()
{
	SAFE_RELEASE(m_pSampler);
}

bool Sampler::CreateSamapler(const std::string & _strName, D3D11_FILTER _eFilter,
	D3D11_TEXTURE_ADDRESS_MODE _eAddressU,
	D3D11_TEXTURE_ADDRESS_MODE _eAddressV,
	D3D11_TEXTURE_ADDRESS_MODE _eAddressW,
	D3D11_COMPARISON_FUNC _CamparisonFun)
{
	D3D11_SAMPLER_DESC tDesc = {};
	
	tDesc.Filter = _eFilter;				// ���͸� ( ��, ����, ���� )
	tDesc.AddressU = _eAddressU;			// ��ǥ������� UVW
	tDesc.AddressV = _eAddressV;
	tDesc.AddressW = _eAddressW;
	tDesc.MinLOD = -FLT_MAX;
	tDesc.MaxLOD = FLT_MAX;
	tDesc.MipLODBias = 0.0f;
	tDesc.MaxAnisotropy = 1;				// �ִ� ���浵�̴�. 1~16���� ��������. ���� �������� ���������� ������.
	tDesc.ComparisonFunc = _CamparisonFun;			// �񱳾��� ������
	memcpy(tDesc.BorderColor, &Vector4(1.0f, 1.0f, 1.0f, 1.0f), sizeof(Vector4));

	if (FAILED(_DEVICE->CreateSamplerState(&tDesc, &m_pSampler)))
	{
		return false;
	}

	SetTag(_strName.c_str());

	return true;
}

void Sampler::VSSetShader(int _iRegister)
{
	_CONTEXT->VSSetSamplers(_iRegister, 1, &m_pSampler);
}

void Sampler::PSSetShader(int _iRegister)
{
	_CONTEXT->PSSetSamplers(_iRegister, 1, &m_pSampler);
}
