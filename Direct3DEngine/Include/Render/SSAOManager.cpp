#include "SSAOManager.h"
#include "..\Device.h"
#include "Shader.h"
#include "ShaderManager.h"
#include "RenderTarget.h"
#include "..\Component\Camera.h"
#include "..\Scene\Scene.h"
#include "..\Scene\SceneManager.h"

ENGINE_USING

DEFINITION_SINGLETON(SSAOManager);

SSAOManager::SSAOManager()
{
	m_pQuarDepthBuffer = nullptr;
	m_pQuarDpethUAV = nullptr;
	m_pQuarDepthSRV = nullptr;

	m_pAmbientOcclusionTexture = nullptr;
	m_pAmbientOcclusionTextureUAV = nullptr;
	m_pAmbientOcclusionTextureSRV = nullptr;

	m_pSharedDepthBuffer = nullptr;
	m_pSharedDepthUAV = nullptr;
	m_pSharedDepthSRV = nullptr;

	m_pTempTex[0] = nullptr;
	m_pTempTex[1] = nullptr;

	m_pTempUAV[0] = nullptr;
	m_pTempUAV[1] = nullptr;

	m_pTempSRV[0] = nullptr;
	m_pTempSRV[1] = nullptr;

	m_pDownScaleShader = nullptr;
	m_pComputeSSAOShader = nullptr;
	m_pBlurVerticalShader = nullptr;
	m_pBlurHorizonShader = nullptr;

	m_SSAODownScaleCB = {};
	m_BlurCBuffer = {};
}


SSAOManager::~SSAOManager()
{
	SAFE_RELEASE(m_pQuarDepthBuffer);
	SAFE_RELEASE(m_pQuarDpethUAV);
	SAFE_RELEASE(m_pQuarDepthSRV);

	SAFE_RELEASE(m_pAmbientOcclusionTexture);
	SAFE_RELEASE(m_pAmbientOcclusionTextureUAV);
	SAFE_RELEASE(m_pAmbientOcclusionTextureSRV);

	SAFE_RELEASE(m_pSharedDepthBuffer);
	SAFE_RELEASE(m_pSharedDepthUAV);
	SAFE_RELEASE(m_pSharedDepthSRV);

	SAFE_RELEASE(m_pTempTex[0]);
	SAFE_RELEASE(m_pTempTex[1]);

	SAFE_RELEASE(m_pTempUAV[0]);
	SAFE_RELEASE(m_pTempUAV[1]);

	SAFE_RELEASE(m_pTempSRV[0]);
	SAFE_RELEASE(m_pTempSRV[1]);

	SAFE_RELEASE(m_pDownScaleShader);
	SAFE_RELEASE(m_pComputeSSAOShader);
	SAFE_RELEASE(m_pBlurVerticalShader);
	SAFE_RELEASE(m_pBlurHorizonShader);
}


bool SSAOManager::Init()
{
	// 1/4 깊이 버퍼 - 순서없는 접근뷰, 셰이더리소스 생성
	D3D11_BUFFER_DESC tDesc = {};
	tDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	tDesc.StructureByteStride = 4 * sizeof(float);			// sizeof(float)
	tDesc.ByteWidth = _RESOLUTION.iWidth * _RESOLUTION.iHeight * tDesc.StructureByteStride;
	tDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;			// 리소스를 구조화된 버퍼로 사용함

	if (FAILED(_DEVICE->CreateBuffer(&tDesc, NULL, &m_pQuarDepthBuffer)))
	{
		return false;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDesc = {};
	tUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	tUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;			// 해당 리소스를 버퍼로 본다는 뜻
	tUAVDesc.Buffer.FirstElement = 0;
	tUAVDesc.Buffer.NumElements = (_RESOLUTION.iWidth * _RESOLUTION.iHeight);

	if (FAILED(_DEVICE->CreateUnorderedAccessView(m_pQuarDepthBuffer, &tUAVDesc, &m_pQuarDpethUAV)))
	{
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};
	tSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	tSRVDesc.Buffer.FirstElement = 0;
	tSRVDesc.Buffer.NumElements = (_RESOLUTION.iWidth * _RESOLUTION.iHeight);

	if (FAILED(_DEVICE->CreateShaderResourceView(m_pQuarDepthBuffer, &tSRVDesc, &m_pQuarDepthSRV)))
	{
		return false;
	}


	// 앰비언트 오클루션 맵을 저장하기 위한 텍스처 2개
	D3D11_TEXTURE2D_DESC tTexDesc = {};
	tTexDesc.Width = _RESOLUTION.iWidth / 2;
	tTexDesc.Height = _RESOLUTION.iHeight / 2;
	tTexDesc.MipLevels = 1;
	tTexDesc.ArraySize = 1;
	tTexDesc.Format = DXGI_FORMAT_R32_FLOAT;
	tTexDesc.SampleDesc.Count = 1;
	tTexDesc.SampleDesc.Quality = 0;
	tTexDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(_DEVICE->CreateTexture2D(&tTexDesc, NULL, &m_pAmbientOcclusionTexture)))
	{
		return false;
	}

	tUAVDesc = {};
	tUAVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	tUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;			// 해당 리소스를 버퍼로 본다는 뜻
	tUAVDesc.Buffer.NumElements = (_RESOLUTION.iWidth * _RESOLUTION.iHeight) / 4;

	if (FAILED(_DEVICE->CreateUnorderedAccessView(m_pAmbientOcclusionTexture, &tUAVDesc, &m_pAmbientOcclusionTextureUAV)))
	{
		return false;
	}

	tSRVDesc = {};
	tSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	tSRVDesc.Buffer.NumElements = (_RESOLUTION.iWidth * _RESOLUTION.iHeight) / 4;
	tSRVDesc.Texture2D.MipLevels = 1;

	if (FAILED(_DEVICE->CreateShaderResourceView(m_pAmbientOcclusionTexture, &tSRVDesc, &m_pAmbientOcclusionTextureSRV)))
	{
		return false;
	}

	// 블러처리를 도울 텍스처들

	if (FAILED(_DEVICE->CreateTexture2D(&tTexDesc, NULL, &m_pTempTex[0])))
	{
		return false;
	}

	tUAVDesc = {};
	tUAVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	tUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;			// 해당 리소스를 버퍼로 본다는 뜻
	tUAVDesc.Buffer.NumElements = (_RESOLUTION.iWidth * _RESOLUTION.iHeight) / 4;

	if (FAILED(_DEVICE->CreateUnorderedAccessView(m_pTempTex[0], &tUAVDesc, &m_pTempUAV[0])))
	{
		return false;
	}

	tSRVDesc = {};
	tSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	tSRVDesc.Buffer.NumElements = (_RESOLUTION.iWidth * _RESOLUTION.iHeight) / 4;
	tSRVDesc.Texture2D.MipLevels = 1;

	if (FAILED(_DEVICE->CreateShaderResourceView(m_pTempTex[0], &tSRVDesc, &m_pTempSRV[0])))
	{
		return false;
	}

	if (FAILED(_DEVICE->CreateTexture2D(&tTexDesc, NULL, &m_pTempTex[1])))
	{
		return false;
	}

	tUAVDesc = {};
	tUAVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	tUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;			// 해당 리소스를 버퍼로 본다는 뜻
	tUAVDesc.Buffer.NumElements = (_RESOLUTION.iWidth * _RESOLUTION.iHeight) / 4;

	if (FAILED(_DEVICE->CreateUnorderedAccessView(m_pTempTex[1], &tUAVDesc, &m_pTempUAV[1])))
	{
		return false;
	}

	tSRVDesc = {};
	tSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	tSRVDesc.Buffer.NumElements = (_RESOLUTION.iWidth * _RESOLUTION.iHeight) / 4;
	tSRVDesc.Texture2D.MipLevels = 1;

	if (FAILED(_DEVICE->CreateShaderResourceView(m_pTempTex[1], &tSRVDesc, &m_pTempSRV[1])))
	{
		return false;
	}


	// 공유 메모리 대체용
	tDesc = {};
	tDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	tDesc.StructureByteStride = 4;			// sizeof(float)
	tDesc.ByteWidth = _RESOLUTION.iWidth * _RESOLUTION.iHeight;
	tDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;			// 리소스를 구조화된 버퍼로 사용함

	if (FAILED(_DEVICE->CreateBuffer(&tDesc, NULL, &m_pSharedDepthBuffer)))
	{
		return false;
	}

	tUAVDesc = {};
	tUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	tUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;			// 해당 리소스를 버퍼로 본다는 뜻
	tUAVDesc.Buffer.NumElements = (_RESOLUTION.iWidth * _RESOLUTION.iHeight) / 4;

	if (FAILED(_DEVICE->CreateUnorderedAccessView(m_pSharedDepthBuffer, &tUAVDesc, &m_pSharedDepthUAV)))
	{
		return false;
	}

	tSRVDesc = {};
	tSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	tSRVDesc.Buffer.NumElements = (_RESOLUTION.iWidth * _RESOLUTION.iHeight) / 4;

	if (FAILED(_DEVICE->CreateShaderResourceView(m_pSharedDepthBuffer, &tSRVDesc, &m_pSharedDepthSRV)))
	{
		return false;
	}


	m_pDownScaleShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_SSAO_DOWNSCALE);
	m_pComputeSSAOShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_SSAO_COMPUTESSAO);
	m_pBlurVerticalShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_POSTEFFECT_BLOOM_BLURFILTER_VERTICAL);
	m_pBlurHorizonShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_POSTEFFECT_BLOOM_BLURFILTER_HORIZON);

	return true;
}

bool SSAOManager::Compute(ID3D11ShaderResourceView * _DepthSRV, ID3D11ShaderResourceView * _NormalSRV)
{
	if (false == DownScaleDepth(_DepthSRV, _NormalSRV))
		return false;

	if (false == SSAOCompute())
		return false;

	if (false == Blur())
		return false;

	return true;
}

bool SSAOManager::DownScaleDepth(ID3D11ShaderResourceView* _DepthSRV, ID3D11ShaderResourceView* _NormalSRV)
{
	// 셰이더 셋팅
	m_pDownScaleShader->SetShader();

	// 상수버퍼 업데이트
	SetCBuffer();
	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("SSAOCBuffer", &m_SSAODownScaleCB);

	//리소스들 넣어주기
	_CONTEXT->CSSetUnorderedAccessViews(0, 1, &m_pQuarDpethUAV, NULL);		// 저장용도..
	_CONTEXT->CSSetShaderResources(0, 1, &_DepthSRV);						// 뎁스리소스
	_CONTEXT->CSSetShaderResources(1, 1, &_NormalSRV);						// 노말
	_CONTEXT->Dispatch((UINT)std::ceil((float)(_RESOLUTION.iWidth * _RESOLUTION.iHeight) / 1024.0f), 1, 1);

	// 리셋
	ID3D11UnorderedAccessView* pNullUAV = nullptr;
	ID3D11ShaderResourceView* pNullSRV = nullptr;

	_CONTEXT->CSSetUnorderedAccessViews(0, 1, &pNullUAV, NULL);		// 저장용도..
	_CONTEXT->CSSetShaderResources(0, 1, &pNullSRV);						// 뎁스리소스
	_CONTEXT->CSSetShaderResources(1, 1, &pNullSRV);						// 노말

	return true;
}

bool SSAOManager::SSAOCompute()
{
	m_pComputeSSAOShader->SetShader();
	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("SSAOCBuffer", &m_SSAODownScaleCB);
	_CONTEXT->CSSetUnorderedAccessViews(0, 1, &m_pTempUAV[0], NULL);
	_CONTEXT->CSSetUnorderedAccessViews(1, 1, &m_pSharedDepthUAV, NULL);
	_CONTEXT->CSSetShaderResources(0, 1, &m_pQuarDepthSRV);					
	_CONTEXT->Dispatch((UINT)std::ceil((float)(_RESOLUTION.iWidth * _RESOLUTION.iHeight) / 1024.0f), 1, 1);

	ID3D11UnorderedAccessView* pNullUAV = nullptr;
	ID3D11ShaderResourceView* pNullSRV = nullptr;

	_CONTEXT->CSSetUnorderedAccessViews(0, 1, &pNullUAV, NULL);	
	_CONTEXT->CSSetUnorderedAccessViews(1, 1, &pNullUAV, NULL);
	_CONTEXT->CSSetShaderResources(0, 1, &pNullSRV);
	return true;
}

bool SSAOManager::Blur()
{
	//return true;
	// Constants
	m_BlurCBuffer.nWidth = _RESOLUTION.iWidth / 2;				// 상수버퍼 용도 - Res.x
	m_BlurCBuffer.nHeight = _RESOLUTION.iHeight / 2;			// 상수버퍼 용도 - Res.y
	m_BlurCBuffer.nTotalPixels = (UINT)((float)(_RESOLUTION.iWidth * _RESOLUTION.iWidth / 4));			// 상수버퍼 용도 - Domain
	m_BlurCBuffer.nGroupSize = (UINT)((float)(_RESOLUTION.iWidth * _RESOLUTION.iWidth / 4) / 1024.0f);	// 상수버퍼 용도 - GroupSize
	m_BlurCBuffer.fAdaptation = 5.0f;			// 상수버퍼 용도 - Adatation (적응)
	m_BlurCBuffer.fBloomThreshold = 2.0f;	// 상수버퍼 용도 - 블룸

	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("TDownScaleCBuffer", &m_BlurCBuffer);

	ID3D11ShaderResourceView* pNullSRV = nullptr;
	ID3D11UnorderedAccessView* pNullUAV = nullptr;

	// 
	_CONTEXT->CSSetUnorderedAccessViews(0, 1, &m_pTempUAV[1], NULL);
	_CONTEXT->CSSetShaderResources(0, 1, &m_pTempSRV[0]);

	m_pBlurVerticalShader->SetShader();
	_CONTEXT->Dispatch((UINT)std::ceil((float)(_RESOLUTION.iWidth * _RESOLUTION.iHeight) / 1024.0f), 1, 1);

	_CONTEXT->CSSetShaderResources(0, 1, &pNullSRV);
	_CONTEXT->CSSetUnorderedAccessViews(0, 1, &pNullUAV, NULL);

	//
	_CONTEXT->CSSetUnorderedAccessViews(1, 1, &m_pAmbientOcclusionTextureUAV, NULL);
	_CONTEXT->CSSetShaderResources(1, 1, &m_pTempSRV[1]);

	m_pBlurHorizonShader->SetShader();
	_CONTEXT->Dispatch((UINT)std::ceil((float)(_RESOLUTION.iWidth * _RESOLUTION.iHeight) / 1024.0f), 1, 1);

	_CONTEXT->CSSetShaderResources(1, 1, &pNullSRV);
	_CONTEXT->CSSetUnorderedAccessViews(1, 1, &pNullUAV, NULL);

	return true;
}

void SSAOManager::SetSSAOTexRSV(UINT _SlotNumber)
{
	_CONTEXT->PSSetShaderResources(_SlotNumber, 1, &m_pAmbientOcclusionTextureSRV);
}

void SSAOManager::ResetSSAOTexRSV(UINT _SlotNumber)
{
	ID3D11ShaderResourceView* pNullSRV = nullptr;
	_CONTEXT->PSSetShaderResources(_SlotNumber, 1, &pNullSRV);
}

void SSAOManager::SetCBuffer()
{
	Scene* pCurScene = GET_SINGLETON(SceneManager)->GetScene();
	Camera* pMainCamera = pCurScene->GetMainCamera();
	Matrix matProj = pMainCamera->GetProjectionMatrix();

	m_SSAODownScaleCB.inWidth = _RESOLUTION.iWidth;
	m_SSAODownScaleCB.inHeight = _RESOLUTION.iHeight;
	m_SSAODownScaleCB.fHorResRcp = 1.0f / (float)m_SSAODownScaleCB.inWidth;
	m_SSAODownScaleCB.fVerResRcp = 1.0f / (float)m_SSAODownScaleCB.inHeight;
	m_SSAODownScaleCB.vProjParams.x = 1.0f / matProj._11;
	m_SSAODownScaleCB.vProjParams.y = 1.0f / matProj._22;

	float fQ = pMainCamera->GetFar() / (pMainCamera->GetFar() - pMainCamera->GetNear());

	m_SSAODownScaleCB.vProjParams.z = -pMainCamera->GetNear() * fQ;
	m_SSAODownScaleCB.vProjParams.w = -fQ;

	m_SSAODownScaleCB.matView = pMainCamera->GetViewMatrix();
	m_SSAODownScaleCB.matView.Transpose();

	m_SSAODownScaleCB.fOffsetRadius = 6;
	m_SSAODownScaleCB.fRadius = 7;
	//m_SSAODownScaleCB.fOffsetRadius = 10;
	//m_SSAODownScaleCB.fRadius = 12;

	m_SSAODownScaleCB.fMaxDepth = pMainCamera->GetFar();

	SAFE_RELEASE(pMainCamera);
}