#pragma once
#include "..\GameObject.h"

ENGINE_BEGIN

class ENGINE_DLL SSAOManager
{
private:
	SSAODownScaleCBuffer m_SSAODownScaleCB;

private:
	// 1/4 깊이 버퍼
	ID3D11Buffer* m_pQuarDepthBuffer;
	ID3D11UnorderedAccessView* m_pQuarDpethUAV;
	ID3D11ShaderResourceView* m_pQuarDepthSRV;

	// 오클루션 맵을 저장하기 위한 1/4 해상도 텍스처 2개
	ID3D11Texture2D* m_pAmbientOcclusionTexture;
	ID3D11UnorderedAccessView* m_pAmbientOcclusionTextureUAV;
	ID3D11ShaderResourceView* m_pAmbientOcclusionTextureSRV;

	// 공유메모리 대체용 버퍼
	ID3D11Buffer* m_pSharedDepthBuffer;
	ID3D11UnorderedAccessView* m_pSharedDepthUAV;
	ID3D11ShaderResourceView* m_pSharedDepthSRV;

	// 블러처리를 위한 텍스처 2개
	ID3D11Texture2D*			m_pTempTex[2];				// 임시 텍스처 (영역)
	ID3D11UnorderedAccessView*	m_pTempUAV[2];
	ID3D11ShaderResourceView*	m_pTempSRV[2];

	TDownScaleCBuffer m_BlurCBuffer;

private:
	class Shader* m_pDownScaleShader;
	class Shader* m_pComputeSSAOShader;
	class Shader* m_pBlurVerticalShader;
	class Shader* m_pBlurHorizonShader;

private:
	void SetCBuffer();

public:
	bool Init();
	bool Compute(ID3D11ShaderResourceView* _DepthSRV, ID3D11ShaderResourceView* _NormalSRV);
	bool DownScaleDepth(ID3D11ShaderResourceView* _DepthSRV, ID3D11ShaderResourceView* _NormalSRV);
	bool SSAOCompute();
	bool Blur();

public:
	void SetSSAOTexRSV(UINT _SlotNumber);
	void ResetSSAOTexRSV(UINT _SlotNumber);

public:

	DECLARE_SINGLETON(SSAOManager)
};

ENGINE_END