#include "RenderManager.h"
#include "ShaderManager.h"
#include "..\Scene\SceneManager.h"
#include "..\Scene\Scene.h"
#include "RasterizerState.h"
#include "DepthStencilState.h"
#include "BlendState.h"
#include "..\Component\Light.h"
#include "RenderTarget.h"
#include "..\Device.h"
#include "..\Resource\ResourcesManager.h"
#include "..\Resource\Sampler.h"
#include "Shader.h"
#include "..\Resource\Mesh.h"
#include "..\CollisionManager.h"
#include "..\Component\Material.h"
#include "..\Component\Renderer.h"
#include "..\Component\Transform.h"
#include "..\Component\Camera.h"
#include "..\Component\TPCamera.h"
#include "..\Scene\Scene.h"
#include "..\Component\AnimationFrame.h"
#include "..\Component\Particle.h"
#include "..\Component\UI.h"
#include "SSAOManager.h"
#include "..\Scene\Layer.h"

ENGINE_USING

DEFINITION_SINGLETON(RenderManager)


RenderManager::RenderManager()
{
	m_bLimLightCBuffer = {};
	// �⺻�� ���۵� ���
	m_eMode = RM_DEFERRED;
	m_tRenderCBuffer = {};
	m_tDepthFogCBuffer = {};
	m_tRenderCBuffer.iRenderMode = m_eMode;
	//m_pLinearSampler = nullptr;
	//m_pPointSampler = nullptr;

	m_pDynamicShadowShader = nullptr;
	m_pStaticShadowShader = nullptr;

	m_pDepthDisable = nullptr;
	m_pAccBlend = nullptr;
	m_pAlphaBlend = nullptr;
	m_pCullNone = nullptr;

	m_pLightAccDirShader = nullptr;
	m_pLightAccPointShader = nullptr;
	m_pLightAccSpotShader = nullptr;
	m_pLightBlendShader = nullptr;
	m_pLightBlendRenderShader = nullptr;

	m_pLightPointVolume = nullptr;
	m_pLightSpotVolume = nullptr;

	m_pLightPointLayout = nullptr;
	m_pLightSpotLayout = nullptr;

	// �ν��Ͻ̿�
	m_pStaticInstancing = nullptr;
	m_pAnimFrameInstancing = nullptr;
	m_pAnimInstancing = nullptr;
	m_pColliderInstancing = nullptr;
	m_pLightInstancing = nullptr;
	m_pStaticInstancingShader = nullptr;
	m_pAnimFrameInstancingShader = nullptr;
	m_pAnimInstancingShader = nullptr;
	m_pUIButtonInstancingShader = nullptr;
	m_pUIImageInstancingShader = nullptr;
	m_pUIBarInstancingShader = nullptr;
	m_pStaticInstancingLayout = nullptr;
	m_pAnimFrameInstancingLayout = nullptr;
	m_pAnimInstancingLayout = nullptr;
	m_pUIInstancingLayout = nullptr;

	m_bShadowCompute = false;

	// ����Ʈ ����Ʈ
	m_bPostEffect = false;
	m_bBloom = false;
	m_pPostEffectShader = false;
	m_pDownScaleFirstPassShader = false;
	m_pDownScaleSecondPassShader = false;

	m_fMiddleGrey = 0.0f;
	m_fWhite = 0.0f;
	m_uiWidth = 0;
	m_uiHeight = 0;
	m_uiDomain = 0;
	m_uiDownScaleGroups = 0;
	m_fAdatation = 0.0f;
	m_fBloomThreshold = 0.0f;
	m_fBloomScale = 0.0f;

	m_fDOFFarStart = 0.0f;
	m_fDOFFarRange = 0.0f;

	m_pDownScaleBuffer = nullptr;
	m_pDownScaleUAV = nullptr;
	m_pDownScaleSRV = nullptr;
	m_pAvgLumBuffer = nullptr;
	m_pAvgLumUAV = nullptr;
	m_pAvgLumSRV = nullptr;

	m_pPrevAdaptionBuffer = nullptr;
	m_pPrevAdaptionUAV = nullptr;
	m_pPrevAdaptionSRV = nullptr;

	m_pDownScaleSceneRT = nullptr;
	m_pDownScaleSceneUAV = nullptr;
	m_pDownScaleSceneSRV = nullptr;

	m_pTempRT[0] = nullptr;
	m_pTempRT[1] = nullptr;

	m_pTempUAV[0] = nullptr;
	m_pTempUAV[1] = nullptr;

	m_pTempSRV[0] = nullptr;
	m_pTempSRV[1] = nullptr;

	m_pBloomRT = nullptr;
	m_pBloomUAV = nullptr;
	m_pBloomSRV = nullptr;

	m_pBloomBritePassShader = nullptr;
	m_pBloomBlurVerticalShader = nullptr;
	m_pBloomBlurHorizonShader = nullptr;

	m_ShadowBiasCB.fShadowBias = 0.0f;

	m_bSSAO = false;
	m_bDepthFog = false;
	m_bRain = false;
	m_bShadowCompute = false;
	
}


RenderManager::~RenderManager()
{
	// �ν��Ͻ�
	if (m_pStaticInstancing)
	{
		SAFE_DELETE_ARRAY(m_pStaticInstancing->pData);
		SAFE_RELEASE(m_pStaticInstancing->pBuffer);
		SAFE_DELETE(m_pStaticInstancing);
	}
	if (m_pAnimFrameInstancing)
	{
		SAFE_DELETE_ARRAY(m_pAnimFrameInstancing->pData);
		SAFE_RELEASE(m_pAnimFrameInstancing->pBuffer);
		SAFE_DELETE(m_pAnimFrameInstancing);
	}
	if (m_pAnimInstancing)
	{
		SAFE_DELETE_ARRAY(m_pAnimInstancing->pData);
		SAFE_RELEASE(m_pAnimInstancing->pBuffer);
		SAFE_DELETE(m_pAnimInstancing);
	}
	if (m_pColliderInstancing)
	{
		SAFE_DELETE_ARRAY(m_pColliderInstancing->pData);
		SAFE_RELEASE(m_pColliderInstancing->pBuffer);
		SAFE_DELETE(m_pColliderInstancing);
	}
	if (m_pLightInstancing)
	{
		SAFE_DELETE_ARRAY(m_pLightInstancing->pData);
		SAFE_RELEASE(m_pLightInstancing->pBuffer);
		SAFE_DELETE(m_pLightInstancing);
	}

	SAFE_RELEASE(m_pStaticInstancingShader);
	SAFE_RELEASE(m_pAnimFrameInstancingShader);
	SAFE_RELEASE(m_pAnimInstancingShader);
	SAFE_RELEASE(m_pUIButtonInstancingShader);
	SAFE_RELEASE(m_pUIImageInstancingShader);
	SAFE_RELEASE(m_pUIBarInstancingShader);

	Safe_Delete_Map(m_mapInstancingGeometry);

	// ����Ʈ ����Ʈ ����
	// HDR
	SAFE_RELEASE(m_pDownScaleBuffer);
	SAFE_RELEASE(m_pDownScaleUAV);
	SAFE_RELEASE(m_pDownScaleSRV);

	SAFE_RELEASE(m_pAvgLumBuffer);
	SAFE_RELEASE(m_pAvgLumUAV);
	SAFE_RELEASE(m_pAvgLumSRV);

	SAFE_RELEASE(m_pPostEffectShader);
	SAFE_RELEASE(m_pDownScaleFirstPassShader);
	SAFE_RELEASE(m_pDownScaleSecondPassShader);

	// ����
	SAFE_RELEASE(m_pPrevAdaptionBuffer);
	SAFE_RELEASE(m_pPrevAdaptionUAV);
	SAFE_RELEASE(m_pPrevAdaptionSRV);

	// ���
	SAFE_RELEASE(m_pDownScaleSceneRT);
	SAFE_RELEASE(m_pDownScaleSceneUAV);
	SAFE_RELEASE(m_pDownScaleSceneSRV);

	SAFE_RELEASE(m_pTempRT[0]);
	SAFE_RELEASE(m_pTempRT[1]);

	SAFE_RELEASE(m_pTempUAV[0]);
	SAFE_RELEASE(m_pTempUAV[1]);

	SAFE_RELEASE(m_pTempSRV[0]);
	SAFE_RELEASE(m_pTempSRV[1]);

	SAFE_RELEASE(m_pBloomRT);
	SAFE_RELEASE(m_pBloomUAV);
	SAFE_RELEASE(m_pBloomSRV);

	SAFE_RELEASE(m_pBloomBritePassShader);
	SAFE_RELEASE(m_pBloomBlurVerticalShader);
	SAFE_RELEASE(m_pBloomBlurHorizonShader);

	// ������
	SAFE_RELEASE(m_pCullNone);
	SAFE_RELEASE(m_pAlphaBlend);
	SAFE_RELEASE(m_pAccBlend);
	SAFE_RELEASE(m_pDepthDisable);

	SAFE_RELEASE(m_pLightBlendRenderShader);
	SAFE_RELEASE(m_pLightBlendShader);
	SAFE_RELEASE(m_pLightAccDirShader);
	SAFE_RELEASE(m_pLightAccPointShader);
	SAFE_RELEASE(m_pLightAccSpotShader);

	SAFE_RELEASE(m_pLightPointVolume);
	SAFE_RELEASE(m_pLightSpotVolume);

	//SAFE_RELEASE(m_pLinearSampler);
	//SAFE_RELEASE(m_pPointSampler);

	SAFE_RELEASE(m_pDynamicShadowShader);
	SAFE_RELEASE(m_pStaticShadowShader);

	Safe_Delete_Map(m_MRTMap);
	Safe_Release_Map(m_RenderStateMap);

	DESTROY_SINGLETON(ShaderManager);

	std::unordered_map<std::string, RenderTarget*>::iterator StartIter = m_RenderTargetMap.begin();
	std::unordered_map<std::string, RenderTarget*>::iterator EndIter = m_RenderTargetMap.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		SAFE_DELETE(StartIter->second);
	}

}

///////////////////////////////////////////////////////////////// Get()

PInstancingGeometry RenderManager::FindInstancingGeometry(unsigned __int64 _iKey)
{
	std::unordered_map<unsigned __int64, PInstancingGeometry>::iterator FindIter = m_mapInstancingGeometry.find(_iKey);

	if (FindIter == m_mapInstancingGeometry.end())
	{

		return nullptr;
	}

	return FindIter->second;
}

PInstancingBuffer RenderManager::CreateInstancingBuffer(int _iSize, int _iCount)
{
	PInstancingBuffer	pBuffer = new InstancingBuffer;

	pBuffer->iSize = _iSize;
	pBuffer->iCount = _iCount;
	pBuffer->eUsage = D3D11_USAGE_DYNAMIC;
	pBuffer->pData = new char[_iSize * _iCount];

	D3D11_BUFFER_DESC	tDesc = {};
	tDesc.ByteWidth = _iSize * _iCount;
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tDesc.Usage = D3D11_USAGE_DYNAMIC;						// �ν��Ͻ��� ���÷� ����������ϱ⶧���� ���̳���
	tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(_DEVICE->CreateBuffer(&tDesc, nullptr, &pBuffer->pBuffer)))
		return nullptr;

	return pBuffer;
}

void RenderManager::ResizeInstancingBuffer(PInstancingBuffer _pBuffer, int _iCount)
{
	SAFE_DELETE_ARRAY(_pBuffer->pData);
	SAFE_RELEASE(_pBuffer->pBuffer);
	_pBuffer->iCount = _iCount;
	_pBuffer->pData = new char[_pBuffer->iSize * _iCount];

	D3D11_BUFFER_DESC	tDesc = {};
	tDesc.ByteWidth = _pBuffer->iSize * _pBuffer->iCount;
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tDesc.Usage = D3D11_USAGE_DYNAMIC;
	tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(_DEVICE->CreateBuffer(&tDesc, nullptr, &_pBuffer->pBuffer)))
		return;
}

void RenderManager::AddInstancingData(PInstancingBuffer _pBuffer, int _iPos, void * _pData)
{
	char* pBufferData = (char*)_pBuffer->pData;
	memcpy(pBufferData + (_iPos * _pBuffer->iSize), _pData,
		_pBuffer->iSize);

	InstancingAnimFrameBuffer dddd = *(InstancingAnimFrameBuffer*)(_pData);
}

void RenderManager::CopyInstancingData(PInstancingBuffer _pBuffer, int _iCount)
{
	D3D11_MAPPED_SUBRESOURCE	tMap = {};

	InstancingAnimFrameBuffer* ddd = (InstancingAnimFrameBuffer*)(_pBuffer->pData);

	for (int i = 0; i < 5; ++i)
	{
		InstancingAnimFrameBuffer dddd = ddd[i];
		int a = 0;
	}

	_CONTEXT->Map(_pBuffer->pBuffer, 0, D3D11_MAP_WRITE_DISCARD,
		0, &tMap);

	memcpy(tMap.pData, _pBuffer->pData, _pBuffer->iSize * _iCount);

	_CONTEXT->Unmap(_pBuffer->pBuffer, 0);
}

void RenderManager::SetShadowCompute(bool _Value)
{
	m_bShadowCompute = _Value;
}

bool RenderManager::GetShadowCompute()
{
	return m_bShadowCompute;
}

void RenderManager::PostEffectInit()
{
	//m_fMiddleGrey = 0.863f;
	//m_fWhite = 1.2f;

	//m_fMiddleGrey = 3.5f;
	//m_fWhite = 6.0f;

	m_fMiddleGrey = 1.2f;
	m_fWhite = 5.0f;

	m_uiWidth = _RESOLUTION.iWidth;
	m_uiHeight = _RESOLUTION.iHeight;
	m_uiDomain = (UINT)((float)(m_uiWidth * m_uiHeight / 16));
	m_uiDownScaleGroups = (UINT)((float)(m_uiWidth * m_uiHeight / 16) / 1024.0f);
	m_fAdatation = 5.0f;

	// ù��°
	D3D11_BUFFER_DESC tDesc = {};
	tDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	tDesc.StructureByteStride = 4;				// Size of float
	tDesc.ByteWidth = 4 *( (_RESOLUTION.iWidth * _RESOLUTION.iHeight) / (16 * 1024));
	tDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	// ���� ����
	if (FAILED(_DEVICE->CreateBuffer(&tDesc, NULL, &m_pDownScaleBuffer)))
	{
		return;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDesc = {};
	tUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	tUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	tUAVDesc.Buffer.NumElements = (_RESOLUTION.iWidth * _RESOLUTION.iHeight) / (16 * 1024);

	// ���� ���� ���� �� ����
	if (FAILED(_DEVICE->CreateUnorderedAccessView(m_pDownScaleBuffer, &tUAVDesc, &m_pDownScaleUAV)))
	{
		return;
	}


	D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};
	tSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	tSRVDesc.Buffer.NumElements = (_RESOLUTION.iWidth * _RESOLUTION.iHeight) / (16 * 1024);

	// ���̴� ���ҽ� �� ����
	if (FAILED(_DEVICE->CreateShaderResourceView(m_pDownScaleBuffer, &tSRVDesc, &m_pDownScaleSRV)))
	{
		return;
	}

	// �ι�°
	D3D11_BUFFER_DESC tDescLA = {};
	tDescLA.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	tDescLA.StructureByteStride = 4;
	tDescLA.ByteWidth = 4;
	tDescLA.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	// ���� ����
	if (FAILED(_DEVICE->CreateBuffer(&tDescLA, NULL, &m_pAvgLumBuffer)))
	{
		return;
	}


	D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDescLA = {};
	tUAVDescLA.Format = DXGI_FORMAT_UNKNOWN;
	tUAVDescLA.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	tUAVDescLA.Buffer.NumElements = 1;

	// ���� ���� ���� �� ����
	if (FAILED(_DEVICE->CreateUnorderedAccessView(m_pAvgLumBuffer, &tUAVDescLA, &m_pAvgLumUAV)))
	{
		return;
	}


	D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDescLA = {};
	tSRVDescLA.Format = DXGI_FORMAT_UNKNOWN;
	tSRVDescLA.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	tSRVDescLA.Buffer.NumElements = 1;

	// ���̴� ���ҽ� �� ����
	if (FAILED(_DEVICE->CreateShaderResourceView(m_pAvgLumBuffer, &tSRVDescLA, &m_pAvgLumSRV)))
	{
		return;
	}

#pragma region ����
	// ����
	D3D11_BUFFER_DESC tDescAdap = {};
	tDescAdap.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	tDescAdap.StructureByteStride = 4;
	tDescAdap.ByteWidth = 4;
	tDescAdap.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	// ���� ����
	if (FAILED(_DEVICE->CreateBuffer(&tDescAdap, NULL, &m_pPrevAdaptionBuffer)))
	{
		return;
	}


	D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDescAdap = {};
	tUAVDescAdap.Format = DXGI_FORMAT_UNKNOWN;
	tUAVDescAdap.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	tUAVDescAdap.Buffer.NumElements = 1;

	// ���� ���� ���� �� ����
	if (FAILED(_DEVICE->CreateUnorderedAccessView(m_pPrevAdaptionBuffer, &tUAVDescAdap, &m_pPrevAdaptionUAV)))
	{
		return;
	}


	D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDescAdap = {};
	tSRVDescAdap.Format = DXGI_FORMAT_UNKNOWN;
	tSRVDescAdap.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	tSRVDescAdap.Buffer.NumElements = 1;

	// ���̴� ���ҽ� �� ����
	if (FAILED(_DEVICE->CreateShaderResourceView(m_pPrevAdaptionBuffer, &tSRVDescAdap, &m_pPrevAdaptionSRV)))
	{
		return;
	}
#pragma endregion


#pragma region ���

	//m_fBloomThreshold = 2.0f;
	//m_fBloomScale = 0.1f;
	m_fBloomThreshold = 2.0f;
	m_fBloomScale = 0.2f;

	D3D11_TEXTURE2D_DESC tTexDesc = {};
	tTexDesc.Width = m_uiWidth / 4;
	tTexDesc.Height = m_uiHeight / 4;
	tTexDesc.MipLevels = 1;
	tTexDesc.ArraySize = 1;
	tTexDesc.Format = DXGI_FORMAT_R32G32B32A32_TYPELESS;
	tTexDesc.SampleDesc.Count = 1;
	tTexDesc.SampleDesc.Quality = 0;
	tTexDesc.Usage = D3D11_USAGE_DEFAULT;
	tTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	tTexDesc.CPUAccessFlags = 0;
	tTexDesc.MiscFlags = 0;

	if (FAILED(_DEVICE->CreateTexture2D(&tTexDesc, NULL, &m_pDownScaleSceneRT)))
	{
		return;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDescDS = {};
	tSRVDescDS.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tSRVDescDS.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	tSRVDescDS.Texture2D.MipLevels = 1;

	// ���̴� ���ҽ� �� ����
	if (FAILED(_DEVICE->CreateShaderResourceView(m_pDownScaleSceneRT, &tSRVDescDS, &m_pDownScaleSceneSRV)))
	{
		return;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDescDS = {};
	tUAVDescDS.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tUAVDescDS.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	tUAVDescDS.Buffer.FirstElement = 0;
	tUAVDescDS.Buffer.NumElements = m_uiWidth * m_uiHeight / 16;

	// ���� ���� ���� �� ����
	if (FAILED(_DEVICE->CreateUnorderedAccessView(m_pDownScaleSceneRT, &tUAVDescDS, &m_pDownScaleSceneUAV)))
	{
		return;
	}


	// �ӽ� �ؽ�ó
	if (FAILED(_DEVICE->CreateTexture2D(&tTexDesc, NULL, &m_pTempRT[0])))
	{
		return;
	}
	if (FAILED(_DEVICE->CreateShaderResourceView(m_pTempRT[0], &tSRVDescDS, &m_pTempSRV[0])))
	{
		return;
	}
	if (FAILED(_DEVICE->CreateUnorderedAccessView(m_pTempRT[0], &tUAVDescDS, &m_pTempUAV[0])))
	{
		return;
	}

	if (FAILED(_DEVICE->CreateTexture2D(&tTexDesc, NULL, &m_pTempRT[1])))
	{
		return;
	}
	if (FAILED(_DEVICE->CreateShaderResourceView(m_pTempRT[1], &tSRVDescDS, &m_pTempSRV[1])))
	{
		return;
	}
	if (FAILED(_DEVICE->CreateUnorderedAccessView(m_pTempRT[1], &tUAVDescDS, &m_pTempUAV[1])))
	{
		return;
	}

	// ��� Ÿ��
	if (FAILED(_DEVICE->CreateTexture2D(&tTexDesc, NULL, &m_pBloomRT)))
	{
		return;
	}
	if (FAILED(_DEVICE->CreateShaderResourceView(m_pBloomRT, &tSRVDescDS, &m_pBloomSRV)))
	{
		return;
	}
	if (FAILED(_DEVICE->CreateUnorderedAccessView(m_pBloomRT, &tUAVDescDS, &m_pBloomUAV)))
	{
		return;
	}

#pragma endregion

#pragma region DOF
	m_fDOFFarStart = 30.0f;				// 40 ~ 400
	m_fDOFFarRange = 1.0f / std::fmaxf(60.0f, 0.001f);			// 80 -> 60 ~150
#pragma endregion


}

void RenderManager::SetSSAOEnable(bool _Enable)
{
	m_bSSAO = _Enable;
}

bool RenderManager::GetSSAOEnable()
{
	return m_bSSAO;
}

void RenderManager::SetDepthFogEnable(bool _Enable)
{
	m_bDepthFog = _Enable;
}

bool RenderManager::GetDepthFogEnable()
{
	return m_bDepthFog;
}

void RenderManager::SetRainEnable(bool _Enable)
{
	m_bRain = _Enable;
}

bool RenderManager::GetRainEnable()
{
	return m_bRain;
}

void RenderManager::SetPostEffectEnable(bool _Enable)
{
	m_bPostEffect = _Enable;
}

void RenderManager::SetBloomEnable(bool _Enable)
{
	m_bBloom = _Enable;
}

Light * RenderManager::GetFirstLight() const
{
	if (0 == m_tLightList.iSize)
	{
		return nullptr;
	}
	else if (RM_DEFERRED == m_eMode)
	{
		// ���۵� �������� ��� ��Ƽ� ����Ʈ����� �����ϱ� ������ nullptr�� �����ش�.
		// ������ �϶��� ����Ʈ�� �����ش�.
		return nullptr;
	}

	return m_tLightList.pObjectList[0]->FindComponentFromType<Light>(CT_LIGHT);
}

////////////////////////////////////////////////////////////// Init()

bool RenderManager::Init()
{
	if (false == (GET_SINGLETON(ShaderManager)->Init()))
	{
		return false;
	}

	// �ν��Ͻ̹��� �ʱ�ȭ �� �ν��Ͻ̿� ���̴� �־��ֱ�
	m_pStaticInstancing = CreateInstancingBuffer(sizeof(InstancingStaticBuffer));
	m_pAnimFrameInstancing = CreateInstancingBuffer(sizeof(_tagInstancingAnimFrameBuffer));
	m_pAnimInstancing = CreateInstancingBuffer(sizeof(InstancingStaticBuffer));

	ShaderLyaoutInit();			// ���̴��� ���̾ƿ� �ʱ�ȭ
	PostEffectInit();			// ����Ʈ����Ʈ �ʱ�ȭ
	RenderStateInit();			// ����������Ʈ �ʱ�ȭ
	RenderTargetInit();			// ����Ÿ�� �ʱ�ȭ
	SSAOInit();					// SSAO �ʱ�ȭ
	DepthFogInit();

#pragma region ���÷��� �־��ֱ� ������ -> ���ҽ� �Ŵ������� �ٷ� ��������⋚���� ���̴����� ���ϴ� ���÷� ��� ����ϸ��
	//m_pLinearSampler = GET_SINGLETON(ResourcesManager)->FindSampler(SAMPLER_LINEAR);
	//m_pPointSampler = GET_SINGLETON(ResourcesManager)->FindSampler(SAMPLER_POINT);
#pragma endregion

	return true;
}

void RenderManager::ShaderLyaoutInit()
{
	// �ν��Ͻ�
	m_pStaticInstancingShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_STANDARD_3D_INSTANCING);
	m_pStaticInstancingLayout = GET_SINGLETON(ShaderManager)->FindInputLayout(LAYOUT_VERTEX3D_STATIC_INSTANCING);

	m_pAnimFrameInstancingShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_PARTICLE_INSTNACING);
	m_pAnimFrameInstancingLayout = GET_SINGLETON(ShaderManager)->FindInputLayout(LAYOUT_PARTICLE_INSTANCING);

	m_pUIButtonInstancingShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_UI_BUTTON_INSTANCING);
	m_pUIImageInstancingShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_UI_IMAGE_INSTANCING);
	m_pUIBarInstancingShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_UI_BAR_INSTANCING);
	m_pUIInstancingLayout = GET_SINGLETON(ShaderManager)->FindInputLayout(LAYOUT_UI_INSTANCING);

	// HDR + ���� ����
	m_pPostEffectShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_POSTEFFECT);
	m_pDownScaleFirstPassShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_POSTEFFECT_DOWNSCALE_FIRST);
	m_pDownScaleSecondPassShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_POSTEFFECT_DOWNSCALE_SECOND);

	// ���
	m_pBloomBritePassShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_POSTEFFECT_BLOOM_BRITEPASS);
	m_pBloomBlurVerticalShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_POSTEFFECT_BLOOM_BLURFILTER_VERTICAL);
	m_pBloomBlurHorizonShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_POSTEFFECT_BLOOM_BLURFILTER_HORIZON);

	m_pLightAccDirShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_LIGHTACC_DIR);
	m_pLightAccPointShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_LIGHTACC_POINT);
	m_pLightAccSpotShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_LIGHTACC_SPOT);
	m_pLightBlendShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_LIGHT_BLEND);
	m_pLightBlendRenderShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_LIGHT_BLEND_RENDER);

	m_pLightPointLayout = GET_SINGLETON(ShaderManager)->FindInputLayout(LAYOUT_POSITION);
	m_pLightSpotLayout = GET_SINGLETON(ShaderManager)->FindInputLayout(LAYOUT_POSITION);
	m_pLightPointVolume = GET_SINGLETON(ResourcesManager)->FindMesh("Sphere");
	m_pLightSpotVolume = GET_SINGLETON(ResourcesManager)->FindMesh("Cone");

	// ������
	m_pDynamicShadowShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_STANDARD_3D_DYANMIC_DEPTH);
	m_pStaticShadowShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_STANDARD_3D_STATIC_DEPTH);
}

bool RenderManager::SSAOInit()
{
	if (false == GET_SINGLETON(SSAOManager)->Init())
	{
		return false;
	}
	return  true;
}

bool RenderManager::DepthFogInit()
{ 
	m_tDepthFogCBuffer.vFogColor = Vector3(0.2f, 0.2f, 0.2f);
	m_tDepthFogCBuffer.fFogStartDepth = 20.0f;
	m_tDepthFogCBuffer.vFogHightLightColor = Vector3(0.2f, 0.2f, 0.2f);
	m_tDepthFogCBuffer.fFogGlobalDensity = 0.7f;									// �ִ� 2.0f,, �⺻ 1.5f
	m_tDepthFogCBuffer.vFogSunDir = Vector3::Zero;										// Dir����Ʈ���� ������
	m_tDepthFogCBuffer.fFogHeightFalloff = 0.2f;

	return true;
}

bool RenderManager::RenderStateInit()
{
#pragma region RasterizerState, DepthState, BlendState ����� �� �� �־��ֱ�

	//  Create Rasterozer
	CreateRasterizerState("CullNone", D3D11_FILL_SOLID,
		D3D11_CULL_NONE);

	CreateRasterizerState("WireFrame", D3D11_FILL_WIREFRAME);

	//  Create DepthState
	CreateDepthState("LessEqual", TRUE, D3D11_DEPTH_WRITE_MASK_ALL,
		D3D11_COMPARISON_LESS_EQUAL);
	CreateDepthState("DepthDisable", FALSE);	// ���� ��� X ( �������� �⺻������ )

	
	// Create BlendState - ����
	AddTargetBlendDesc("AlphaBlend", TRUE, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA);			// AlphaBelnd
	CreateBlendState("AlphaBlend", TRUE, FALSE);

	AddTargetBlendDesc("AccBlend", TRUE, D3D11_BLEND_ONE, D3D11_BLEND_ONE);	// Acc Blend
	CreateBlendState("AccBlend", TRUE, FALSE);

	// ���� ��� - ���伥ó��..
	AddTargetBlendDesc(BLEND_NORMAL, TRUE, D3D11_BLEND_SRC_COLOR, D3D11_BLEND_DEST_COLOR, D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD);
	AddTargetBlendDesc(BLEND_DARKEN, TRUE, D3D11_BLEND_SRC_COLOR, D3D11_BLEND_DEST_COLOR, D3D11_BLEND_OP_MIN,
		D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_MIN);
	AddTargetBlendDesc(BLEND_MULTIPLY, TRUE, D3D11_BLEND_SRC_COLOR, D3D11_BLEND_DEST_COLOR, D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ZERO, D3D11_BLEND_SRC_COLOR, D3D11_BLEND_OP_ADD);
	AddTargetBlendDesc(BLEND_LIGHTEN, TRUE, D3D11_BLEND_SRC_COLOR, D3D11_BLEND_DEST_COLOR, D3D11_BLEND_OP_MAX,
		D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_MAX);
	AddTargetBlendDesc(BLEND_SCREEN, TRUE, D3D11_BLEND_SRC_COLOR, D3D11_BLEND_DEST_COLOR, D3D11_BLEND_OP_ADD,
		D3D11_BLEND_INV_DEST_COLOR, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD);
	AddTargetBlendDesc(BLEND_LINEAR_DODGE, TRUE, D3D11_BLEND_SRC_COLOR, D3D11_BLEND_DEST_COLOR, D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD);
	AddTargetBlendDesc(BLEND_SUBTRACT, TRUE, D3D11_BLEND_SRC_COLOR, D3D11_BLEND_DEST_COLOR, D3D11_BLEND_OP_REV_SUBTRACT,
		D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_REV_SUBTRACT);


	// Value Init
	m_pCullNone = FindRenderState("CullNone");
	m_pDepthDisable = FindRenderState("DepthDisable");
	m_pAccBlend = FindRenderState("AccBlend");
	m_pAlphaBlend = FindRenderState("AlphaBlend");

#pragma endregion

	return true;
}

bool RenderManager::RenderTargetInit()
{
	// ClearColor ���� (����Ÿ�� ����¿뵵)
	float fClearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};


#pragma region Deferred RenderTarget �����

	// Color ����Ÿ��
	if (false == CreateRenderTarget("Albedo", _RESOLUTION.iWidth, _RESOLUTION.iHeight,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		fClearColor, 1))
	{
		return false;
	}

	OnDebugRenderTarget("Albedo", Vector3(0.0f, 0.0f, 0.0f), Vector3(100.0f, 100.0f, 1.0f));


	// Normal(����) ����Ÿ��
	if (false == CreateRenderTarget("Normal", _RESOLUTION.iWidth, _RESOLUTION.iHeight,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		fClearColor, 1))
	{
		return false;
	}

	OnDebugRenderTarget("Normal", Vector3(0.0f, 100.0f, 0.0f), Vector3(100.0f, 100.0f, 1.0f));


	// Depth(����) ���� Ÿ��
	if (false == CreateRenderTarget("Depth", _RESOLUTION.iWidth, _RESOLUTION.iHeight,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		fClearColor, 1))
	{
		return false;
	}

	OnDebugRenderTarget("Depth", Vector3(0.0f, 200.0f, 0.0f), Vector3(100.0f, 100.0f, 1.0f));


	// Material(����) ����Ÿ�� 
	// ���� ����Ÿ���� �� ���ҿ�..
	// R = Diffuse, G = Normal, B = Specular, A = Emissive �̷��� �����ؼ� �־��ٰ���(���̴��ʿ���)
	if (false == CreateRenderTarget("Material", _RESOLUTION.iWidth, _RESOLUTION.iHeight,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		fClearColor, 1))
	{
		return false;
	}

	OnDebugRenderTarget("Material", Vector3(0.0f, 300.0f, 0.0f), Vector3(100.0f, 100.0f, 1.0f));

	//
	if (false == CreateRenderTarget("RenderGroup", _RESOLUTION.iWidth, _RESOLUTION.iHeight,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		fClearColor, 1))
	{
		return false;
	}

	OnDebugRenderTarget("RenderGroup", Vector3(0.0f, 400.0f, 0.0f), Vector3(100.0f, 100.0f, 1.0f));
#pragma endregion

#pragma region LightAcc RenderTarget �����

	// Light Diffuse RenderTarget
	if (false == CreateRenderTarget("LightDiffuse", _RESOLUTION.iWidth, _RESOLUTION.iHeight,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		fClearColor, 1))
	{
		return false;
	}

	OnDebugRenderTarget("LightDiffuse", Vector3(100.0f, 0.0f, 0.0f), Vector3(100.0f, 100.0f, 1.0f));

	// Light Specular RenderTarget
	if (false == CreateRenderTarget("LightSpecular", _RESOLUTION.iWidth, _RESOLUTION.iHeight,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		fClearColor, 1))
	{
		return false;
	}

	OnDebugRenderTarget("LightSpecular", Vector3(100.0f, 100.0f, 0.0f), Vector3(100.0f, 100.0f, 1.0f));

	// Light Blend RenderTarget
	if (false == CreateRenderTarget("LightBlend", _RESOLUTION.iWidth, _RESOLUTION.iHeight,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		fClearColor, 1))
	{
		return false;
	}

	OnDebugRenderTarget("LightBlend", Vector3(100.0f, 200.0f, 0.0f), Vector3(100.0f, 100.0f, 1.0f));
#pragma endregion

#pragma region  ����Ʈ ����Ʈ ����Ÿ��

	if (false == CreateRenderTarget("HDR", _RESOLUTION.iWidth, _RESOLUTION.iHeight,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		fClearColor, 1))
	{
		return false;
	}

	OnDebugRenderTarget("HDR", Vector3(200.0f, 0.0f, 0.0f), Vector3(100.0f, 100.0f, 1.0f));

#pragma endregion

	// �׸��� ����Ÿ��
	if (false == CreateRenderTarget("Shadow", _RESOLUTION.iWidth * 4, _RESOLUTION.iHeight * 4,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		fClearColor, 1, DXGI_FORMAT_D24_UNORM_S8_UINT))
	{
		return false;
	}

	OnDebugRenderTarget("Shadow", Vector3(200.0f, 100.0f, 0.0f), Vector3(100.0f, 100.0f, 1.0f));

	// SSAO
	if (false == CreateRenderTarget("SSAO", _RESOLUTION.iWidth, _RESOLUTION.iHeight,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		fClearColor, 1, DXGI_FORMAT_D24_UNORM_S8_UINT))
	{
		return false;
	}

	OnDebugRenderTarget("SSAO", Vector3(200.0f, 200.0f, 0.0f), Vector3(100.0f, 100.0f, 1.0f));

#pragma region MRT ����� ( ������ ����Ÿ�ٿ� �ִ°��߿��� ���� �ʿ��Ѱ͸� ������)
	AddMRT("GBuffer", "Albedo");
	AddMRT("GBuffer", "Normal");
	AddMRT("GBuffer", "Depth");
	AddMRT("GBuffer", "Material");
	AddMRT("GBuffer", "RenderGroup");

	AddMRT("LightAcc", "LightDiffuse");
	AddMRT("LightAcc", "LightSpecular");
	AddMRT("LightAcc", "LightBlend");

	AddMRT("PostEffect", "HDR");
	//AddMRT("PostEffect", "LDR");

	AddMRT("ShadowRender", "Shadow");
	AddMRT("ShadowRender", "ShadowStatic");
#pragma endregion

	return true;
}

////////////////////////////////////////////////////////////// ������Ʈ�� �����׷캰�� �־��ֱ�
void RenderManager::AddRenderObject(GameObject* _pObject)
{
	// ����Ʈ ����Ʈ�� �־��ֱ�
	if (true == _pObject->CheckComponentFromType(CT_LIGHT))
	{
		if (m_tLightList.iSize == m_tLightList.iCapacity)
		{
			m_tLightList.iCapacity *= 2;

			GameObject** pList = new GameObject*[m_tLightList.iCapacity];
			memcpy(pList, m_tLightList.pObjectList, sizeof(GameObject*) * m_tLightList.iSize);

			SAFE_DELETE_ARRAY(m_tLightList.pObjectList);

			m_tLightList.pObjectList = pList;
		}

		m_tLightList.pObjectList[m_tLightList.iSize] = _pObject;
		++m_tLightList.iSize;
	}


	if (false == _pObject->CheckComponentFromType(CT_RENDERER))
	{
		// ������ ������Ʈ�� ������� ������ �ʿ䰡 �����Ƿ� return + �ؽ�Ʈ��
		if (false == _pObject->CheckComponentFromType(CT_TEXT))
		{
			return;
		}
	}
#pragma region ����
	//else if (true == _pObject->CheckComponentFromType(CT_LANDSCAPE))
	//{
	//	RG = RG_LANDSCAPE;
	//}
	//else if (true == _pObject->CheckComponentFromType(CT_UI))
	//{
	//	// UI
	//	RG = RG_UI;
	//}
	//else
	//{
	//	// �⺻
	//	RG = RG_DEFAULT;
	//}
#pragma endregion

	// �����׷� �����ϱ� , �� ���ӿ�����Ʈ���� �ڽ��� ���� �����׷쿡 ���ϴ���  �˰� �ִ�.
	// ������ ������Ʈ Ÿ������.. (UI����..�ٸ� �������� ���� �����׷��� ��������µ� ������ �׳� �ڱⰡ � �׷쿡 ���ϴ��� ������)
	RENDER_GROUP	RG = _pObject->GetRenderGroup();

	Renderer* pRenderer = _pObject->FindComponentFromType<Renderer>(CT_RENDERER);		// �ش� ������Ʈ ������ ��������

	if (nullptr != pRenderer)
	{
		Mesh* pMesh = pRenderer->GetMesh();
		Material* pMaterial = _pObject->FindComponentFromType<Material>(CT_MATERIAL);

		// ����ü �ø� �Ǵ�
		// UI�� ���� ó���ؾ��Ѵ�. (�̰� ���� �˾Ƽ� �ؾߵ� )
		if (RG <= RG_ALPHA && RG_LANDSCAPE < RG)
		{
			Vector3 vCenter = pMesh->GetCenter();
			float fRadius = pMesh->GetRadiuse();

			// Center�� Radius�� ���� �������� ��ȯ�Ѵ�.
			Transform* pTr = _pObject->GetTransform();
			Vector3 vPos = pTr->GetWorldPosition();
			Vector3 vScale = pTr->GetWorldScale();

			SAFE_RELEASE(pTr);

			vCenter *= vScale;
			vCenter += vPos;		// ������� ��ġ�� �̵� (����)

			// ���� ū���� ���������� �Ѵ�. (����)
			float fScale = vScale.x;
			fScale = fScale < vScale.y ? vScale.y : fScale;
			fScale = fScale < vScale.z ? vScale.z : fScale;

			fRadius *= fScale;		// ������ ���� ���ؾ��Ѵ�. ( ũ�Ⱚ ���� )

			GameObject* pMainCamObj = _pObject->GetScene()->GetMainCameraObject();
			Camera* pMainCamera = _pObject->GetScene()->GetMainCamera();
			TPCamera* pTPCamera = pMainCamObj->FindComponentFromType<TPCamera>(CT_TPCAMERA);
			GameObject* pTPParent = pTPCamera->GetTargetObject()->GetParent();

			if (pTPParent == _pObject)
			{
				// �ø� X
				_pObject->SetFrustumCulling(false);
				SAFE_RELEASE(pMainCamObj);
				SAFE_RELEASE(pMainCamera);
				SAFE_RELEASE(pTPCamera);
			}
			else
			{
				// �θ� ���� ��� 
				if (true == _pObject->ParentEnable())
				{
					Transform* pParentTr = _pObject->GetParent()->GetTransform();
					vCenter += pParentTr->GetWorldPosition();

					float fParentScale = 1.0f;

					Vector3 vParentScale = pParentTr->GetWorldScale();
					fParentScale = (fParentScale < vParentScale.x ? vParentScale.x : fParentScale);
					fParentScale = (fParentScale < vParentScale.y ? vParentScale.y : fParentScale);
					fParentScale = (fParentScale < vParentScale.z ? vParentScale.z : fParentScale);

					fRadius *= fParentScale;

					SAFE_RELEASE(pParentTr);
				}

				bool bFrustumCull = pMainCamera->FrustumInSphere(vCenter, fRadius);

				SAFE_RELEASE(pMainCamera);
				SAFE_RELEASE(pMainCamObj);
				SAFE_RELEASE(pTPCamera);

				_pObject->SetFrustumCulling(!bFrustumCull);			// �������� ���� ����

				if (false == bFrustumCull)
				{
					SAFE_RELEASE(pMesh);
					SAFE_RELEASE(pMaterial);
					SAFE_RELEASE(pRenderer);
					return;
				}
			}
		}

		if (true == _pObject->GetUpdateInstancing())
		{
			unsigned int iMeshNumber = pMesh->GetSerialNumber();			// �޽� �ĺ���ȣ (�ν��Ͻ�)
			unsigned int iMaterialNumber = pMaterial->GetSerialNumber();	// ��Ƽ���� �ĺ���ȣ (�ν��Ͻ�)

																			// __int64 �� �־��ֱ� �޽� / ��Ƽ����
			unsigned __int64 iKey = iMeshNumber;
			iKey <<= 32;
			iKey |= iMaterialNumber;

			PInstancingGeometry pGeometry = FindInstancingGeometry(iKey);		// �̹� �ִ��� �˻�

																				// ���ٸ� ���� �־��ش�.
			if (nullptr == pGeometry)
			{
				pGeometry = new InstancingGeometry;
				m_mapInstancingGeometry.insert(std::make_pair(iKey, pGeometry));
			}

			// 2D �ִϸ��̼� ���� (������ �ִϸ��̼�)
			if (false != _pObject->CheckComponentFromType(CT_ANIMATIONFRAME))
			{
				if (false != _pObject->CheckComponentFromType(CT_PARTICLE))
				{
					pGeometry->bAnimation2D = true;
				}
				else
				{
					pGeometry->bAnimation2D = false;
				}
			}
			else if (false != _pObject->CheckComponentFromType(CT_ANIMATION))
			{
				pGeometry->bAnimation3D = true;
			}
			else
			{
				pGeometry->bAnimation2D = false;
				pGeometry->bAnimation3D = false;
			}

			pGeometry->Add(_pObject);		// ������Ʈ �־��ֱ�
		}
		SAFE_RELEASE(pMesh);
		SAFE_RELEASE(pMaterial);
		SAFE_RELEASE(pRenderer);
	}


	// ���� �׷캰�� ������Ʈ ����Ʈ�� �־��ֱ� ������
	if (m_tRenderGroup[RG].iSize == m_tRenderGroup[RG].iCapacity)
	{
		m_tRenderGroup[RG].iCapacity *= 2;

		GameObject**	pGameObjectList = new GameObject*[m_tRenderGroup[RG].iCapacity];

		memcpy(pGameObjectList, m_tRenderGroup[RG].pObjectList, sizeof(GameObject*) * m_tRenderGroup[RG].iSize);

		SAFE_DELETE_ARRAY(m_tRenderGroup[RG].pObjectList);

		m_tRenderGroup[RG].pObjectList = pGameObjectList;
	}

	// ������Ʈ �߰��ϰ� ������ �÷��ش�.
	m_tRenderGroup[RG].pObjectList[m_tRenderGroup[RG].iSize] = _pObject;
	++m_tRenderGroup[RG].iSize;

}

////////////////////////////////////////////////////////////// �����Ͷ����� ������Ʈ �����
bool RenderManager::CreateRasterizerState(const std::string & _strName
	, D3D11_FILL_MODE _eFill /*= D3D11_FILL_SOLID*/
	, D3D11_CULL_MODE _eCull /*= D3D11_CULL_BACK*/)
{
	RasterizerState*	pState = (RasterizerState*)FindRenderState(_strName);

	if (nullptr != pState)
	{
		SAFE_RELEASE(pState);
		return false;
	}

	pState = new RasterizerState;

	if (false == pState->CreateState(_strName, _eFill, _eCull))
	{
		SAFE_RELEASE(pState);
		return false;
	}

	m_RenderStateMap.insert(std::make_pair(_strName, pState));

	return true;
}

//////////////////////////////////////////////////////////////  ����������Ʈ �����
bool RenderManager::CreateDepthState(const std::string & _strName, BOOL _bEnable /*= TRUE*/
	, D3D11_DEPTH_WRITE_MASK _eWirteMaske/* = D3D11_DEPTH_WRITE_MASK_ALL*/
	, D3D11_COMPARISON_FUNC _eDpethFunc /*= D3D11_COMPARISON_LESS*/)
{
	DepthStencilState* pState = (DepthStencilState*)FindRenderState(_strName);

	if (nullptr != pState)
	{
		SAFE_RELEASE(pState);
		return false;
	}

	pState = new DepthStencilState;

	if (false == pState->CreateState(_strName, _bEnable, _eWirteMaske, _eDpethFunc))
	{
		SAFE_RELEASE(pState);
		return false;
	}

	m_RenderStateMap.insert(std::make_pair(_strName, pState));

	return true;
}

////////////////////////////////////////////////////////////// Create Blend Desc
bool RenderManager::AddTargetBlendDesc(const std::string & _strName,
	BOOL _bEnable /*= FALSE*/,
	D3D11_BLEND _eSrcBlend /*= D3D11_BLEND_ONE*/,
	D3D11_BLEND _eDestBlend /*= D3D11_BLEND_ZERO*/,
	D3D11_BLEND_OP _eOp /*= D3D11_BLEND_OP_ADD*/,
	D3D11_BLEND _eSrcBlendAlpha /*= D3D11_BLEND_ONE*/,
	D3D11_BLEND _eDestBlendAlpha /*= D3D11_BLEND_ZERO*/,
	D3D11_BLEND_OP _eAlphaOp /*= D3D11_BLEND_OP_ADD*/,
	UINT _iWriteMask /*= D3D11_COLOR_WRITE_ENABLE_ALL*/)
{
	BlendState* pState = (BlendState*)FindRenderState(_strName);

	// �ش� ����������Ʈ�� ���ٸ� ������ֱ�
	if (nullptr == pState)
	{
		pState = new BlendState;
		pState->AddReference();
		m_RenderStateMap.insert(std::make_pair(_strName, pState));
	}

	pState->AddRenderTargetBlendDesc(_bEnable, _eSrcBlend, _eDestBlend, _eOp,
		_eSrcBlendAlpha, _eDestBlendAlpha, _eAlphaOp, _iWriteMask);

	SAFE_RELEASE(pState);

	return true;
}

////////////////////////////////////////////////////////////// Create BlendState
bool RenderManager::CreateBlendState(const std::string & _strName,
	BOOL _bAlphaToCoverage /* = FALSE*/,
	BOOL _bIndependentBlend /*= FALSE*/)
{
	BlendState* pState = (BlendState*)(FindRenderState(_strName));

	if (nullptr == pState)
	{
		return false;
	}

	// �����ϴµ� ������ ��� �����ش�. (���۷��� ī���Ͱ� ���������� ���� ��Ű�� �߰��Ȱ͵� �����ش�.
	if (false == pState->CreateState(_strName, _bAlphaToCoverage, _bIndependentBlend))
	{
		pState->Release();

		std::unordered_map<std::string, RenderState*>::iterator FIter = m_RenderStateMap.find(_strName);

		m_RenderStateMap.erase(FIter);

		SAFE_RELEASE(pState);
		return false;
	}

	SAFE_RELEASE(pState);

	return true;
}

//////////////////////////////////////////////////////////////  Create RenderTarget
bool RenderManager::CreateRenderTarget(const std::string & _strName
	, UINT _iWidth, UINT _iHeight, DXGI_FORMAT _eFormat
	, float _fClearColor[4]
	, int _iSamplerCount /*= 1*/
	, DXGI_FORMAT _eDepthFormat/* = DXGI_FORMAT_UNKNOWN*/)
{
	// �ش� ����Ÿ���� �ִ�������..
	RenderTarget*	pRenderTarget = (RenderTarget*)FindRenderTarget(_strName);

	// �̸� �ߺ� or �̹� ���� ��� ���� �ʿ����
	if (nullptr != pRenderTarget)
	{
		return false;
	}

	pRenderTarget = new RenderTarget;

	// ����Ÿ�� �����, ���н� ����� return
	if (false == pRenderTarget->CreateRenderTarget(_strName, _iWidth, _iHeight, _eFormat,
		_fClearColor, _iSamplerCount, _eDepthFormat))
	{
		SAFE_DELETE(pRenderTarget);
		return false;
	}

	m_RenderTargetMap.insert(std::make_pair(_strName, pRenderTarget));

	return true;
}

bool RenderManager::OnDebugRenderTarget(const std::string & _strName, const Vector3 & _vPos, const Vector3 & _vScale)
{
	// ����ã�� (����׿뵵)
	RenderTarget*	pRenderTarget = (RenderTarget*)FindRenderTarget(_strName);

	if (nullptr == pRenderTarget)
	{
		return false;
	}

	pRenderTarget->OnDebug(_vPos, _vScale);

	return true;
}

//////////////////////////////////////////////////////////////// MRT ���� �Լ�

// MRT �߰��ϱ�
bool RenderManager::AddMRT(const std::string & _strMRTName, const std::string & _strTarget)
{
	PMRT	pMRT = FindMRT(_strMRTName);

	if (nullptr == pMRT)
	{
		pMRT = new MRT;
		m_MRTMap.insert(std::make_pair(_strMRTName, pMRT));
	}

	// �ϳ��� ����Ÿ�ٿ��� ������ �ִ� 8�� ������ ���� �� �ִ�.
	// �� �̻��� ���� ���ϴϱ� return
	if (8 == pMRT->RenderTargetVec.size())
	{
		return false;
	}

	RenderTarget* pRenderTarget = FindRenderTarget(_strTarget);

	// �ش� ����Ÿ���� ������ �־��ָ� �ȵȴ�.
	if (nullptr == pRenderTarget)
	{
		return false;
	}

	// �ش� ����Ÿ���� ������ MRT�� �־��ֱ�
	pMRT->RenderTargetVec.push_back(pRenderTarget);
	return true;
}

// MRT ���� ���� �־��ֱ�
bool RenderManager::AddDepth(const std::string & _strMRTName, const std::string & _strTargetName)
{
	PMRT	pMRT = FindMRT(_strMRTName);

	// �ش� MRT�� �������������� ���� ���� �־��ش�.
	if (nullptr == pMRT)
	{
		pMRT = new MRT;
		m_MRTMap.insert(std::make_pair(_strMRTName, pMRT));
	}

	RenderTarget*	pRenderTarget = FindRenderTarget(_strTargetName);

	if (nullptr == pRenderTarget)
	{
		return false;
	}

	pMRT->pDepth = pRenderTarget->GetDepthStencilView();

	return true;
}

void RenderManager::ClearMRT(const std::string & _strMRTName)
{
	// �ش� ����Ÿ���� �����ش�. (Ŭ�����������)
	PMRT pMRT = FindMRT(_strMRTName);

	if (nullptr == pMRT)
	{
		return;
	}

	// ����� ������
	for (size_t i = 0; i < pMRT->RenderTargetVec.size(); ++i)
	{
		pMRT->RenderTargetVec[i]->ClearRenderTarget();
	}
}

void RenderManager::SetMRT(const std::string & _strMRTName)
{
	PMRT	pMRT = FindMRT(_strMRTName);

	if (nullptr == pMRT)
	{
		return;
	}

	// ����(����)�� ����Ÿ�� �������� ���� ����Ÿ�� ������ ������  ������ ����!!
	// ���� ����Ÿ�ٰ����� ���ƾ��Ѵ�! 
	if (pMRT->OldRenderTargetVec.size() < pMRT->RenderTargetVec.size())
	{
		pMRT->OldRenderTargetVec.clear();
		pMRT->OldRenderTargetVec.resize(pMRT->RenderTargetVec.size());
	}

	// ����Ÿ�� ���� ������ŭ ������ �������� ( ��������), �����ϰ��� Reset�Լ����� release �������
	_CONTEXT->OMGetRenderTargets((UINT)(pMRT->RenderTargetVec.size()),
		&pMRT->OldRenderTargetVec[0], &pMRT->pOldDepth);

	// �������� ����Ÿ�ٵ� �������� �����´�.
	std::vector<ID3D11RenderTargetView*> RenderTargetVec;
	RenderTargetVec.resize(pMRT->RenderTargetVec.size());

	for (size_t i = 0; i < RenderTargetVec.size(); ++i)
	{
		RenderTargetVec[i] = pMRT->RenderTargetVec[i]->GetRenderTargetView();
	}

	ID3D11DepthStencilView*	pDepth = pMRT->pDepth;

	// ���� ������ ���ٸ� ������ ������ �־��ش�.
	if (nullptr == pDepth)
	{
		pDepth = pMRT->pOldDepth;
	}

	// �ٲ��� ����Ÿ������ �����ֱ� ������
	_CONTEXT->OMSetRenderTargets((UINT)(pMRT->RenderTargetVec.size()),
		&RenderTargetVec[0], pDepth);
}

void RenderManager::ResetMRT(const std::string & _strMRTName)
{
	PMRT	pMRT = FindMRT(_strMRTName);

	// �������� MRT�� ���ٸ� return
	if (nullptr == pMRT)
	{
		return;
	}

	_CONTEXT->OMSetRenderTargets((UINT)(pMRT->RenderTargetVec.size()),
		&pMRT->OldRenderTargetVec[0], pMRT->pOldDepth);

	SAFE_RELEASE(pMRT->pOldDepth);

	// Get�ؼ� ���۷��� ī���� �����Ѱ� Release�� �����ش�.(ī���Ͱ���)
	for (size_t i = 0; i < pMRT->RenderTargetVec.size(); ++i)
	{
		SAFE_RELEASE(pMRT->OldRenderTargetVec[i]);
	}
}

//////////////////////////////////////////////////////////////  RenderState, RenderTarget Find�Լ�

RenderState * RenderManager::FindRenderState(const std::string & _strName)
{
	std::unordered_map<std::string, RenderState*>::iterator FIter = m_RenderStateMap.find(_strName);

	if (FIter == m_RenderStateMap.end())
	{
		return nullptr;
	}

	FIter->second->AddReference();

	return FIter->second;
}

RenderTarget * RenderManager::FindRenderTarget(const std::string & _strName)
{
	// ����Ÿ��ã��
	std::unordered_map<std::string, RenderTarget*>::iterator FIter = m_RenderTargetMap.find(_strName);


	// �ش� ����Ÿ���� ������ nullptr��ȯ
	if (FIter == m_RenderTargetMap.end())
	{
		return nullptr;
	}

	return FIter->second;
}

PMRT RenderManager::FindMRT(const std::string & _strName)
{
	std::unordered_map<std::string, PMRT>::iterator FIter = m_MRTMap.find(_strName);

	if (FIter == m_MRTMap.end())
	{
		return nullptr;
	}

	return FIter->second;
}

void RenderManager::ComputeInstancing()
{
	for (int i = 0; i < RG_END; ++i)
	{
		m_InstancingList[i].clear();
	}

	std::unordered_map<unsigned __int64, PInstancingGeometry>::iterator	iterIns = m_mapInstancingGeometry.begin();
	std::unordered_map<unsigned __int64, PInstancingGeometry>::iterator	iterInsEnd = m_mapInstancingGeometry.end();

	for (; iterIns != iterInsEnd; ++iterIns)
	{
		PInstancingGeometry	pGeometry = iterIns->second;

		// �����Ȱ� 5�� �̻��϶� �ν��Ͻ�ó��
		if (pGeometry->iSize >= INSTANCING_COUNT)
		{
			RENDER_GROUP	rg = pGeometry->pGameObjectList[0]->GetRenderGroup();

			if (true == pGeometry->bAnimation3D)
			{
				// 3D�ִϸ��̼�
				if (pGeometry->iSize > m_pAnimInstancing->iCount)
				{
					ResizeInstancingBuffer(m_pAnimInstancing,
						pGeometry->iSize);
				}
			}
			else if (true == pGeometry->bAnimation2D)
			{
				// 2D�ִϸ��̼�(��ƼŬ����)
				if (pGeometry->iSize > m_pAnimFrameInstancing->iCount)
				{
					ResizeInstancingBuffer(m_pAnimFrameInstancing,
						pGeometry->iSize);
				}
			}
			else
			{
				// ����ƽ �޽�, UI ó���ؾߵ�
				if (pGeometry->iSize > m_pStaticInstancing->iCount)
				{
					ResizeInstancingBuffer(m_pStaticInstancing, pGeometry->iSize);
				}
			}

			for (int i = 0; i < pGeometry->iSize; ++i)
			{
				pGeometry->pGameObjectList[i]->SetInstancingEnable(true);
			}

			m_InstancingList[rg].push_back(pGeometry);
		}
	}
}

//////////////////////////////////////////////////////////////  Render�ϱ�
void RenderManager::Render(float _fTime)
{
	// �����ϱ� ���� ������۸� ������Ʈ���ش�. (RenderCBuffer -> MRT)
	m_tRenderCBuffer.bSSAO = (m_bSSAO != false ? 1 : 0);
	m_tRenderCBuffer.bShadow = (m_bShadowCompute != false ? 1 : 0);
	m_tRenderCBuffer.bFog = (m_bDepthFog != false ? 1 : 0);
	m_tRenderCBuffer.bRain = (m_bRain != false ? 1 : 0);
	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("Rendering", &m_tRenderCBuffer);

	//m_pPointSampler->PSSetShader(7);			// 7�� ���÷� ����Ұ�

	// ������� ���� ������ ���ش�.
	switch (m_eMode)
	{
	case Engine::RM_FORWARD:		// ������
		RenderForward(_fTime);
		break;
	case Engine::RM_DEFERRED:		// ���۵�
		RenderDeferred(_fTime);
		break;
	default:
		break;
	}

	// ���÷� �����ϱ�
	//m_pLinearSampler->PSSetShader(0);

	m_pDepthDisable->SetState();

	std::unordered_map<std::string, RenderTarget*>::iterator StartIter = m_RenderTargetMap.begin();
	std::unordered_map<std::string, RenderTarget*>::iterator EndIter = m_RenderTargetMap.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		StartIter->second->Render();
	}

	m_pDepthDisable->ResetState();			// ����ϰ� ���ڿ��� Reset������� ������ ���� ����ߴ��ɷ� ���ư����ϴϱ�

	// ������ ���ϸ� �ٽ� 0���� �ʱ�ȭ���ش�. (����Ʈ�κ�)
	m_tLightList.iSize = 0;

	// ���� �������ϱ� �굵 clear�Ѵ�.
	std::unordered_map<unsigned __int64, PInstancingGeometry>::iterator	iterIns = m_mapInstancingGeometry.begin();
	std::unordered_map<unsigned __int64, PInstancingGeometry>::iterator	iterInsEnd = m_mapInstancingGeometry.end();

	for (; iterIns != iterInsEnd; ++iterIns)
	{
		iterIns->second->Clear();
	}
}

////////////////////////////////////////////////////////////// ������, ���۵� ����

void RenderManager::RenderForward(float _fTime)
{
	// ������ ����
	for (int i = 0; i < RG_END; ++i)
	{
		for (int j = 0; j < m_tRenderGroup[i].iSize; ++j)
		{
			m_tRenderGroup[i].pObjectList[j]->Render(_fTime);
		}

		if (RG_DEFAULT == i)
		{
			GET_SINGLETON(CollisionManager)->Render(_fTime);
		}

		m_tRenderGroup[i].iSize = 0;
	}
}

void RenderManager::RenderDeferred(float _fTime)
{
	// �׸���
	RenderShadow(_fTime);

	// ����
	if (true == m_bRain)
	{
		//GET_SINGLETON(RainManager)->BeginHeightMap(_fTime);
		//GET_SINGLETON(RainManager)->PrevRender(_fTime);
	}

	// GBuffer�� �׷��ش�.
	RenderGBuffer(_fTime);

	// SSAO
	RenderSSAO(_fTime);

	// ���� ���� ���۸� �����.
	RenderLightAcc(_fTime);

	// ���� �������ۿ� Albedo�� ���ļ� ���� ����ó���� Ÿ���� �����.
	RenderLightBlend(_fTime);

	// ������ �ռ��� ���� Ÿ���� ȭ�鿡 �׷��ش�.
	RenderLightBlendRender(_fTime);

	// ����
	//RenderRainSet(_fTime);
	//RenderRain(_fTime);

	// ����Ʈ ����Ʈ ����
	RenderPostProcessing(_fTime);

	// �ݶ��̴��� ���� �������ش�.
	GET_SINGLETON(CollisionManager)->Render(_fTime);

	RenderTarget* pDepthTarget = FindRenderTarget("Depth");
	pDepthTarget->SetShader(11);

	RenderInstancingAlpha(_fTime);			// ���������ϴ� �ֵ� �ν��Ͻ� ���� ó��

	// ����ó���Ҿֵ� ���� ó��( ��ƼŬ ���Ե�  ->  +2DAnimation) -> ������� ó��
	for (int i = 0; i < m_tRenderGroup[RG_ALPHA].iSize; ++i)
	{
		m_tRenderGroup[RG_ALPHA].pObjectList[i]->Render(_fTime);
	}

	pDepthTarget->ResetShader(11);


	// UI ����
	RenderUI(_fTime);

	// �׷� ������ 0����..
	for (int i = 0; i < RG_END; ++i)
	{
		m_tRenderGroup[i].iSize = 0;
	}
}


//////////////////////////////////////////////////////////////  GBuffer
void RenderManager::RenderGBuffer(float _fTime)
{
	// GBuffer Target�� Clear�Ѵ�. (������ ����, delete�� �ƴ�)
	ClearMRT("GBuffer");

	// MRT ��ü
	SetMRT("GBuffer");

	//
	for (int i = RG_LANDSCAPE; i <= RG_DEFAULT; ++i)
	{
		std::list<PInstancingGeometry>::iterator	iter = m_InstancingList[i].begin();
		std::list<PInstancingGeometry>::iterator	iterEnd = m_InstancingList[i].end();

		for (; iter != iterEnd; ++iter)
		{
			Shader*	pShader = nullptr;
			ID3D11InputLayout*	pLayout = nullptr;

			// �ν��Ͻ� ���۸� ä���ش�.
			PInstancingGeometry	pGeometry = *iter;
			for (int j = 0; j < pGeometry->iSize; ++j)
			{
				GameObject*	pObj = pGeometry->pGameObjectList[j];

				Transform*	pTransform = pObj->GetTransform();
				Scene*	pScene = pObj->GetScene();
				Camera*	pCamera = pScene->GetMainCamera();

				if (true == pGeometry->bAnimation3D)
				{
					// 3D�ִϸ��̼�
				}
				else if (true == pGeometry->bAnimation2D)
				{
					// 2D�ִϸ��̼� 

				}
				else
				{
					// ����ƽ�޽�

					Matrix	matWVP, matWV, matWVRot;
					matWV = pTransform->GetLocalMatrix() * pTransform->GetWorldMatrix();
					matWV *= pCamera->GetViewMatrix();
					matWVP = matWV * pCamera->GetProjectionMatrix();

					matWVRot = pTransform->GetLocalRotationMatrix() * pTransform->GetWorldRotationMatrix();
					matWVRot *= pCamera->GetViewMatrix();

					matWV.Transpose();
					matWVP.Transpose();
					matWVRot.Transpose();

					InstancingStaticBuffer	tBuffer;
					tBuffer.matWV = matWV;
					tBuffer.matWVP = matWVP;
					tBuffer.matWVRot = matWVRot;

					AddInstancingData(m_pStaticInstancing,
						j, &tBuffer);

					pShader = m_pStaticInstancingShader;
					pLayout = m_pStaticInstancingLayout;
				}

				SAFE_RELEASE(pCamera);
				SAFE_RELEASE(pTransform);
			}

			PInstancingBuffer	pBuffer = nullptr;

			if (true == pGeometry->bAnimation3D)
			{
				// 3D �ִϸ��̼�
				pBuffer = m_pAnimInstancing;
			}
			else if (true == pGeometry->bAnimation2D)
			{
				// 2D�ִϸ��̼�
				//pBuffer = m_pAnimFrameInstancing;
			}
			else
			{
				// ����ƽ �޽�
				pBuffer = m_pStaticInstancing;
			}

			// �߰��� �����͸� ���ؽ����ۿ� �����Ѵ�.
			CopyInstancingData(pBuffer,
				pGeometry->iSize);

			// �ν��Ͻ� ��ü�� �׷�����.
			pGeometry->pGameObjectList[0]->PrevRender(_fTime);

			Renderer*	pRenderer = pGeometry->pGameObjectList[0]->FindComponentFromType<Renderer>(CT_RENDERER);

			pRenderer->RenderInstancing(pBuffer, pShader, pLayout,
				pGeometry->iSize, _fTime);

			SAFE_RELEASE(pRenderer);
		}
	}

	//�ϴ� LandScape�� Default�� ���ش�.
	// �ν��Ͻ� �ƴѾֵ�
	for (int i = RG_LANDSCAPE; i <= RG_DEFAULT; ++i)
	{

		for (int j = 0; j < m_tRenderGroup[i].iSize; ++j)
		{
			m_tRenderGroup[i].pObjectList[j]->Render(_fTime);
		}
	}

	// MRT�� ������� ������. ( ��ü�Ѱ� ��ü�ϱ� ������..)
	ResetMRT("GBuffer");
}

////////////////////////////////////////////////////////////// LightAcc
void RenderManager::RenderLightAcc(float _fTime)
{
	// LightAcc Target�� Clear�Ѵ�. 
	ClearMRT("LightAcc");

	// MRT ��ü
	SetMRT("LightAcc");

	RenderDepthFog(_fTime);

	m_pDepthDisable->SetState();
	m_pAccBlend->SetState();

	PMRT	pGBuffer = FindMRT("GBuffer");

	//m_pPointSampler->PSSetShader(10);

	// ���� ����� �ؾ��ϴϱ� 
	pGBuffer->RenderTargetVec[1]->SetShader(11);
	pGBuffer->RenderTargetVec[2]->SetShader(12);
	pGBuffer->RenderTargetVec[3]->SetShader(13);
	pGBuffer->RenderTargetVec[4]->SetShader(6);

	RenderTarget* pShadowRenderTarget = FindRenderTarget("Shadow");
	pShadowRenderTarget->SetShader(9);

	if (true == m_bSSAO)
	{
		GET_SINGLETON(SSAOManager)->SetSSAOTexRSV(7);
	}

	for (int i = 0; i < m_tLightList.iSize; ++i)
	{
		Light*	pLight = m_tLightList.pObjectList[i]->FindComponentFromType<Light>(CT_LIGHT);
		
		Transform* pLightTR = m_tLightList.pObjectList[i]->GetTransform();
		pLightTR->PrevRender(_fTime);

		switch (pLight->GetLightInfo().iType)
		{
		case LT_DIR:
			RenderLightDir(_fTime, pLight);
			break;
		case LT_POINT:
			RenderLightPoint(_fTime, pLight);
			break;
		case LT_SPOT:
			RenderLightSpot(_fTime, pLight);
			break;
		default:
			break;
		}

		SAFE_RELEASE(pLightTR);
		SAFE_RELEASE(pLight);
	}

	// �ٽ����� Reset
	pGBuffer->RenderTargetVec[1]->ResetShader(11);
	pGBuffer->RenderTargetVec[2]->ResetShader(12);
	pGBuffer->RenderTargetVec[3]->ResetShader(13);
	pGBuffer->RenderTargetVec[4]->ResetShader(6);
	pShadowRenderTarget->ResetShader(9);

	if (true == m_bSSAO)
	{
		GET_SINGLETON(SSAOManager)->ResetSSAOTexRSV(7);
	}

	m_pDepthDisable->ResetState();
	m_pAccBlend->ResetState();

	// MRT�� ������� �ǵ����ֱ�
	ResetMRT("LightAcc");
}

void RenderManager::RenderLightDir(float _fTime, Light * _pLight)
{
	// Directional Light
	m_pLightAccDirShader->SetShader();

	// ���������� ������ۿ� �Ѱ��ֱ� 
	_pLight->SetShader();

	// NULL Buffer�� ��üȭ�� ũ���� �簢���� ����ϱ�
	// NULL Buffer�� ���̾ƿ�, ���ؽ� ,�ε��� ���� null�̴�.
	// �׸��� NULL Buffer�� ��� ����� �̹� ������ �ִ�.  + ��üȭ�� ũ��(���� �������� (������))
	_CONTEXT->IASetInputLayout(nullptr);

	UINT iOffSet = 0;
	_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);		// �� ��Ʈ���̿�����
	_CONTEXT->IASetVertexBuffers(0, 0, nullptr, 0, &iOffSet);
	_CONTEXT->IASetIndexBuffer(0, DXGI_FORMAT_UNKNOWN, 0);
	_CONTEXT->Draw(4, 0);
}

void RenderManager::RenderLightPoint(float _fTime, Light * _pLight)
{
	// Point Light
	m_pLightAccPointShader->SetShader();

	// ���������� ������ۿ� �Ѱ��ֱ� 
	// ����Ʈ����Ʈ�� X
	_pLight->SetShader();

	_CONTEXT->IASetInputLayout(m_pLightPointLayout);

	m_pCullNone->SetState();

	m_pLightPointVolume->Render();

	m_pCullNone->ResetState();

}

void RenderManager::RenderLightSpot(float _fTime, Light * _pLight)
{
	// Spot Light
	m_pLightAccSpotShader->SetShader();

	// ���������� ������ۿ� �Ѱ��ֱ� 
	_pLight->SetShader();

	// NULL Buffer�� ��üȭ�� ũ���� �簢���� ����ϱ�
	// NULL Buffer�� ���̾ƿ�, ���ؽ� ,�ε��� ���� null�̴�.
	// �׸��� NULL Buffer�� ��� ����� �̹� ������ �ִ�.  + ��üȭ�� ũ��(���� �������� (������))
	_CONTEXT->IASetInputLayout(m_pLightSpotLayout);

	m_pCullNone->SetState();

	m_pLightSpotVolume->Render();

	m_pCullNone->ResetState();

}

void RenderManager::RenderLightBlend(float _fTime)
{
	RenderTarget*	pLightBlendTarget = FindRenderTarget("LightBlend");

	pLightBlendTarget->ClearRenderTarget();
	pLightBlendTarget->SetRenderTarget();

	m_pLightBlendShader->SetShader();

	m_pDepthDisable->SetState();

	PMRT	pLightAcc = FindMRT("LightAcc");
	RenderTarget*	pAlbedo = FindRenderTarget("Albedo");

	//m_pPointSampler->PSSetShader(10);
	pAlbedo->SetShader(10);

	pLightAcc->RenderTargetVec[0]->SetShader(14);
	pLightAcc->RenderTargetVec[1]->SetShader(15);

	// NULL Buffer�� ��ü ȭ��ũ���� �簢���� ����Ѵ�.
	_CONTEXT->IASetInputLayout(nullptr);

	UINT iOffSet = 0;
	_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	_CONTEXT->IASetVertexBuffers(0, 0, nullptr, 0, &iOffSet);
	_CONTEXT->IASetIndexBuffer(0, DXGI_FORMAT_UNKNOWN, 0);
	_CONTEXT->Draw(4, 0);

	pAlbedo->ResetShader(10);
	pLightAcc->RenderTargetVec[0]->ResetShader(14);
	pLightAcc->RenderTargetVec[1]->ResetShader(15);

	m_pDepthDisable->ResetState();

	pLightBlendTarget->ResetRenderTarget();
}

void RenderManager::RenderLightBlendRender(float _fTime)
{
	m_pLightBlendRenderShader->SetShader();

	m_pDepthDisable->SetState();
	m_pAlphaBlend->SetState();

	RenderTarget*	pLightBlend = FindRenderTarget("LightBlend");

	//m_pPointSampler->PSSetShader(0);
	pLightBlend->SetShader(0);

	// NULL Buffer�� ��ü ȭ��ũ���� �簢���� ����Ѵ�.
	_CONTEXT->IASetInputLayout(nullptr);

	UINT iOffSet = 0;
	_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	_CONTEXT->IASetVertexBuffers(0, 0, nullptr, 0, &iOffSet);
	_CONTEXT->IASetIndexBuffer(0, DXGI_FORMAT_UNKNOWN, 0);
	_CONTEXT->Draw(4, 0);

	pLightBlend->ResetShader(0);
	m_pAlphaBlend->ResetState();
	m_pDepthDisable->ResetState();
}


void RenderManager::RenderInstancingAlpha(float _fTime)
{

	for (int i = RG_ALPHA; i <= RG_ALPHA; ++i)
	{
		if (0 == m_InstancingList[i].size())
			continue;

		std::list<PInstancingGeometry>::iterator	iter = m_InstancingList[i].begin();
		std::list<PInstancingGeometry>::iterator	iterEnd = m_InstancingList[i].end();

		for (; iter != iterEnd; ++iter)
		{
			Shader*	pShader = nullptr;
			ID3D11InputLayout*	pLayout = nullptr;

			// �ν��Ͻ� ���۸� ä���ش�.
			PInstancingGeometry	pGeometry = *iter;
			for (int j = 0; j < pGeometry->iSize; ++j)
			{
				GameObject*	pObj = pGeometry->pGameObjectList[j];

				Transform*	pTransform = pObj->GetTransform();
				Scene*	pScene = pObj->GetScene();
				Camera*	pCamera = pScene->GetMainCamera();

				if (true == pGeometry->bAnimation2D)
				{
					// 2D�ִϸ��̼� 
					Matrix matWVP, matVP, matW;

					matW = pTransform->GetLocalMatrix() * pTransform->GetWorldMatrix();
					matVP = pCamera->GetViewMatrix() * pCamera->GetProjectionMatrix();
					matWVP = matW * pCamera->GetViewMatrix() * pCamera->GetProjectionMatrix();

					//matVP.Transpose();
					//matWVP.Transpose();

					InstancingAnimFrameBuffer tBuffer;
					tBuffer.matVP = matVP;
					tBuffer.matWVP = matWVP;

					tBuffer.matVP.Transpose();
					tBuffer.matWVP.Transpose();

					// ��ƼŬ
					tBuffer.vParticlePos = pTransform->GetWorldPosition();
					tBuffer.vParticleScale.x = pTransform->GetWorldScale().x;
					tBuffer.vParticleScale.y = pTransform->GetWorldScale().y;

					// �ִϸ��̼ǰ��� 2D
					AnimationFrame* pAnimationFrame = pObj->FindComponentFromType<AnimationFrame>(CT_ANIMATIONFRAME);

					tBuffer.iFrame = pAnimationFrame->GetCurClip()->iFrame;

					tBuffer.vStart = pAnimationFrame->GetCurClip()->vecCoord[tBuffer.iFrame].vStart;
					tBuffer.vEnd = pAnimationFrame->GetCurClip()->vecCoord[tBuffer.iFrame].vEnd;

					AddInstancingData(m_pAnimFrameInstancing,
						j, &tBuffer);

					pShader = m_pAnimFrameInstancingShader;
					pLayout = m_pAnimFrameInstancingLayout;

					SAFE_RELEASE(pAnimationFrame);
					SAFE_RELEASE(pCamera);
					SAFE_RELEASE(pTransform);
				}
				else
				{
					SAFE_RELEASE(pCamera);
					SAFE_RELEASE(pTransform);
					continue;
				}
			}

			PInstancingBuffer	pBuffer = nullptr;

			pBuffer = m_pAnimFrameInstancing;


			// �߰��� �����͸� ���ؽ����ۿ� �����Ѵ�.
			CopyInstancingData(pBuffer,
				pGeometry->iSize);

			// �ν��Ͻ� ��ü�� �׷�����.
			pGeometry->pGameObjectList[0]->PrevRender(_fTime);

			Renderer*	pRenderer = pGeometry->pGameObjectList[0]->FindComponentFromType<Renderer>(CT_RENDERER);

			pRenderer->RenderInstancing(pBuffer, pShader, pLayout,
				pGeometry->iSize, _fTime);

			SAFE_RELEASE(pRenderer);
		}

	}

}


void RenderManager::RenderPostProcessing(float _fTime)
{
	if (true == m_bPostEffect)
	{
		// LightAcc Target�� Clear�Ѵ�. 
		//ClearMRT("PostEffect");

		//// MRT ��ü
		//SetMRT("PostEffect");

		m_pDepthDisable->SetState();
		m_pAlphaBlend->SetState();

		RenderTarget*	pLightBlendTarget = FindRenderTarget("LightBlend");

		DownScale(pLightBlendTarget);		// HDR - �ٿ� ������
		Bloom();// ���
		Blur();
		TonMapping(pLightBlendTarget);		// HDR - �����

		m_pAlphaBlend->ResetState();
		m_pDepthDisable->ResetState();

		// ����
		Adaptation(_fTime);

		//ResetMRT("PostEffect");
	}
}

void RenderManager::RenderShadow(float _fTime)
{
	Light* pDirLight = nullptr;

	for (int i = 0; i < m_tLightList.iSize; ++i)
	{
		Light*	pLight = m_tLightList.pObjectList[i]->FindComponentFromType<Light>(CT_LIGHT);

		if (LT_DIR == pLight->GetLightInfo().iType)
		{
			pDirLight = pLight;
			SAFE_RELEASE(pLight);
			break;
		}

		SAFE_RELEASE(pLight);
	}

	if (nullptr == pDirLight)
	{
		return;
	}

	//pDirLight->SetShader();

	D3D11_VIEWPORT tViewPort = {};
	tViewPort.Width = (float)(_RESOLUTION.iWidth) * 4.0f;
	tViewPort.Height = (float)(_RESOLUTION.iHeight) * 4.0f;
	tViewPort.MinDepth = 0.0f;
	tViewPort.MaxDepth = 1.0f;

	_CONTEXT->RSSetViewports(1, &tViewPort);

	// Light - View, Proj Matrix �����

	Transform* pLightTR = pDirLight->GetTransform();
	Matrix matLightView = {};
	Matrix matLightProj = {};

	matLightView.Identity();
	matLightProj.Identity();

	for (int i = 0; i < AXIS_END; ++i)
	{
		// ���� ȸ���� �־��ֱ� (12����Ʈ��ŭ(x,y,z))
		memcpy(&matLightView[i][0], &pLightTR->GetWorldAxis((AXIS)i), sizeof(Vector3));
	}
	
	matLightView.Transpose();

	for (int j = 0; j < AXIS_END; ++j)
	{
		Vector3 dddd = pLightTR->GetWorldPosition();
		matLightView[3][j] = (pLightTR->GetWorldPosition().Dot(pLightTR->GetWorldAxis((AXIS)j))) * -1.0f;
	}
	matLightProj = XMMatrixPerspectiveFovLH(DegreeToRadian(90.0f), (float)(_RESOLUTION.iWidth) / (float)(_RESOLUTION.iHeight), 0.3f, 1000.0f);

	// �Ϲ� ������Ʈ�� ���
	RenderShadowMovingMesh(matLightView, matLightProj, _fTime);

	SAFE_RELEASE(pLightTR);

	tViewPort.Width = (float)(_RESOLUTION.iWidth);
	tViewPort.Height = (float)(_RESOLUTION.iHeight);
	tViewPort.MinDepth = 0.0f;
	tViewPort.MaxDepth = 1.0f;

	_CONTEXT->RSSetViewports(1, &tViewPort);
}

void RenderManager::RenderShadowMovingMesh(Matrix _matView, Matrix _matProj, float _fTime)
{
	RenderTarget* pShadowRenderTarget = FindRenderTarget("Shadow");
	//RenderTarget* pStaticShadowRenderTarget = FindRenderTarget("ShadowStatic");
	//pStaticShadowRenderTarget->SetShader(8);

	pShadowRenderTarget->SetRenderTarget();
	pShadowRenderTarget->ClearRenderTarget();


	for (int i = RG_LANDSCAPE; i <= RG_STATIC; ++i)
	{
		m_ShadowBiasCB.fShadowBias = 0.00018f;
		GET_SINGLETON(ShaderManager)->UpdateConstBuffer("ShadowBias", &m_ShadowBiasCB);

		std::list<PInstancingGeometry>::iterator	iter = m_InstancingList[i].begin();
		std::list<PInstancingGeometry>::iterator	iterEnd = m_InstancingList[i].end();

		for (; iter != iterEnd; ++iter)
		{
			Shader*	pShader = nullptr;
			ID3D11InputLayout*	pLayout = nullptr;

			// �ν��Ͻ� ���۸� ä���ش�.
			PInstancingGeometry	pGeometry = *iter;
			for (int j = 0; j < pGeometry->iSize; ++j)
			{
				GameObject*	pObj = pGeometry->pGameObjectList[j];

				Transform*	pTransform = pObj->GetTransform();
				Scene*	pScene = pObj->GetScene();

				if (true == pGeometry->bAnimation3D)
				{
					// 3D�ִϸ��̼�
				}
				else if (true == pGeometry->bAnimation2D)
				{
					// 2D�ִϸ��̼� 
				}
				else
				{
					// ����ƽ�޽�
					Matrix	matWVP, matWV, matWVRot;
					matWV = pTransform->GetLocalMatrix() * pTransform->GetWorldMatrix();
					matWV *= _matView;
					matWVP = matWV * _matProj;

					matWVRot = pTransform->GetLocalRotationMatrix() * pTransform->GetWorldRotationMatrix();
					matWVRot *= _matView;

					matWV.Transpose();
					matWVP.Transpose();
					matWVRot.Transpose();

					InstancingStaticBuffer	tBuffer;
					tBuffer.matWV = matWV;
					tBuffer.matWVP = matWVP;
					tBuffer.matWVRot = matWVRot;

					AddInstancingData(m_pStaticInstancing,
						j, &tBuffer);

					pShader = m_pDynamicShadowShader;
					pLayout = m_pStaticInstancingLayout;
				}
				SAFE_RELEASE(pTransform);
			}

			PInstancingBuffer	pBuffer = nullptr;

			if (true == pGeometry->bAnimation3D)
			{
				// 3D �ִϸ��̼�
				pBuffer = m_pAnimInstancing;
			}
			else if (true == pGeometry->bAnimation2D)
			{
				// 2D�ִϸ��̼�
				//pBuffer = m_pAnimFrameInstancing;
			}
			else
			{
				// ����ƽ �޽�
				pBuffer = m_pStaticInstancing;
			}

			// �߰��� �����͸� ���ؽ����ۿ� �����Ѵ�.
			CopyInstancingData(pBuffer,
				pGeometry->iSize);

			// �ν��Ͻ� ��ü�� �׷�����.
			pGeometry->pGameObjectList[0]->PrevShadowRender(_matView, _matProj, _fTime);

			Renderer*	pRenderer = pGeometry->pGameObjectList[0]->FindComponentFromType<Renderer>(CT_RENDERER);

			pRenderer->RenderInstancingShadow(pBuffer, pShader, pLayout,
				pGeometry->iSize, _fTime);

			SAFE_RELEASE(pRenderer);
		}
	}

	//�ϴ� LandScape�� Default�� ���ش�.
	// �ν��Ͻ� �ƴѾֵ�
	for (int i = RG_LANDSCAPE; i <= RG_DEFAULT; ++i)
	{
		if (i == RG_DEFAULT)
		{
			m_ShadowBiasCB.fShadowBias = 0.000002f;
			GET_SINGLETON(ShaderManager)->UpdateConstBuffer("ShadowBias", &m_ShadowBiasCB);
		}
		else
		{
			m_ShadowBiasCB.fShadowBias = 0.00018f;
			GET_SINGLETON(ShaderManager)->UpdateConstBuffer("ShadowBias", &m_ShadowBiasCB);
		}

		for (int j = 0; j < m_tRenderGroup[i].iSize; ++j)
		{
			m_tRenderGroup[i].pObjectList[j]->RenderShadow(m_pDynamicShadowShader, _matView, _matProj, _fTime);
		}
	}

	pShadowRenderTarget->ResetRenderTarget();
	//pStaticShadowRenderTarget->ResetShader(8);
}

void RenderManager::RenderSSAO(float _fTime)
{
	if (true == m_bSSAO)
	{
		RenderTarget* pDepthRT = FindRenderTarget("Depth");                          
		RenderTarget* pNormalRT = FindRenderTarget("Normal");

		if (nullptr == pDepthRT || nullptr == pNormalRT)
			return;

		if (false == GET_SINGLETON(SSAOManager)->Compute(pDepthRT->GetShaderResourceView(), pNormalRT->GetShaderResourceView()))
			return;
	}
}

void RenderManager::RenderDepthFog(float _fTime)
{
	if (true == m_bDepthFog)
	{
		Scene* pCurScene = GET_SINGLETON(SceneManager)->GetScene();
		Layer* pLayer = pCurScene->FindLayer("Default");
		GameObject* pDirLight = pLayer->FindGameObject("GlobalLight");

		if (nullptr == pDirLight)
		{
			return;
		}

		Transform* pTransform = pDirLight->GetTransform();
		Matrix matDirLightDir = {};
		matDirLightDir.Identity();

		for (int i = 0; i < AXIS_END; ++i)
		{
			// ���� ȸ���� �־��ֱ� (12����Ʈ��ŭ(x,y,z))
			memcpy(&matDirLightDir[i][0], &pTransform->GetWorldAxis((AXIS)i), sizeof(Vector3));
		}

		Vector3 DirLightDir = matDirLightDir._41;
		m_tDepthFogCBuffer.vFogSunDir = (DirLightDir * -1.0f);

		Camera* pMainCam = pCurScene->GetMainCamera();
		Transform* pMainCamTR = pMainCam->GetTransform();
		m_tDepthFogCBuffer.g_CamPos = pMainCamTR->GetWorldPosition();
		GET_SINGLETON(ShaderManager)->UpdateConstBuffer("DepthFogCBuffer", &m_tDepthFogCBuffer);

		SAFE_RELEASE(pMainCamTR);
		SAFE_RELEASE(pMainCam);
		SAFE_RELEASE(pTransform);
	}
}

void RenderManager::RenderUI(float _fTime)
{
	// UI�� �κ� ���� (������ �Ⱦ�����..)
	RenderInstancingUI(_fTime);

	for (int i = RG_HUD; i < RG_END; ++i)
	{
		// �ν��Ͻ� �ƴѾֵ�
		for (int j = 0; j < m_tRenderGroup[i].iSize; ++j)
		{
			m_tRenderGroup[i].pObjectList[j]->Render(_fTime);
		}
	}
}

void RenderManager::RenderInstancingUI(float _fTime)
{
	for (int i = RG_HUD; i < RG_END; ++i)
	{
		// �ν��Ͻ��� �ֵ� ����
		if (0 == m_InstancingList[i].size())
			continue;

		std::list<PInstancingGeometry>::iterator	iter = m_InstancingList[i].begin();
		std::list<PInstancingGeometry>::iterator	iterEnd = m_InstancingList[i].end();

		for (; iter != iterEnd; ++iter)
		{
			Shader*	pShader = nullptr;
			ID3D11InputLayout*	pLayout = nullptr;

			// �ν��Ͻ� ���۸� ä���ش�.
			PInstancingGeometry	pGeometry = *iter;
			for (int j = 0; j < pGeometry->iSize; ++j)
			{
				GameObject*	pObj = pGeometry->pGameObjectList[j];

				// ���̴��� ������ ��ư���� �̹������� �´� ���̴� �־��ֱ�
				UI* pUICom = pObj->FindComponentFromType<UI>(CT_UI);

				if (nullptr == pUICom)
				{
					SAFE_RELEASE(pUICom);
					continue;
				}

				Transform*	pTransform = pObj->GetTransform();
				Scene*	pScene = pObj->GetScene();
				Camera*	pCamera = pScene->GetUICamera();

				if (false == pGeometry->bAnimation3D && false == pGeometry->bAnimation2D)
				{
					Matrix	matWVP, matWV, matWVRot;
					matWV = pTransform->GetLocalMatrix() * pTransform->GetWorldMatrix();
					matWV *= pCamera->GetViewMatrix();
					matWVP = matWV * pCamera->GetProjectionMatrix();

					matWVRot = pTransform->GetLocalRotationMatrix() * pTransform->GetWorldRotationMatrix();
					matWVRot *= pCamera->GetViewMatrix();

					matWV.Transpose();
					matWVP.Transpose();
					matWVRot.Transpose();

					InstancingStaticBuffer	tBuffer;
					tBuffer.matWV = matWV;
					tBuffer.matWVP = matWVP;
					tBuffer.matWVRot = matWVRot;

					AddInstancingData(m_pStaticInstancing,
						j, &tBuffer);

					switch (pUICom->m_eUIType)
					{
					case UI_TYPE::UI_BUTTON:
						pShader = m_pUIButtonInstancingShader;
						break;
					case UI_TYPE::UI_IMAGE:
						pShader = m_pUIImageInstancingShader;
						break;
					case UI_TYPE::UI_BAR:
						pShader = m_pUIBarInstancingShader;
						break;
					default:
						SAFE_RELEASE(pUICom);
						SAFE_RELEASE(pCamera);
						SAFE_RELEASE(pTransform);
						continue;
					}
					pLayout = m_pUIInstancingLayout;
				}

				SAFE_RELEASE(pUICom);
				SAFE_RELEASE(pCamera);
				SAFE_RELEASE(pTransform);
			}

			PInstancingBuffer	pBuffer = nullptr;
			pBuffer = m_pStaticInstancing;

			// �߰��� �����͸� ���ؽ����ۿ� �����Ѵ�.
			CopyInstancingData(pBuffer,
				pGeometry->iSize);

			// �ν��Ͻ� ��ü�� �׷�����.
			pGeometry->pGameObjectList[0]->PrevRender(_fTime);

			Renderer*	pRenderer = pGeometry->pGameObjectList[0]->FindComponentFromType<Renderer>(CT_RENDERER);

			pRenderer->RenderInstancing(pBuffer, pShader, pLayout,
				pGeometry->iSize, _fTime);

			SAFE_RELEASE(pRenderer);
		}


	}
}

void RenderManager::DownScale(RenderTarget* _pHDRRenderTarget)
{
	// Output
	_CONTEXT->CSSetUnorderedAccessViews(0, 1, &m_pDownScaleUAV, nullptr);
	_CONTEXT->CSSetUnorderedAccessViews(3, 1, &m_pDownScaleSceneUAV, nullptr);

	// Input
	_pHDRRenderTarget->SetCShader(5);

	// Constants
	m_DownScaleCBuffer.nWidth = m_uiWidth / 4;				// ������� �뵵 - Res.x
	m_DownScaleCBuffer.nHeight = m_uiHeight / 4;			// ������� �뵵 - Res.y
	m_DownScaleCBuffer.nTotalPixels = m_uiDomain;			// ������� �뵵 - Domain
	m_DownScaleCBuffer.nGroupSize = m_uiDownScaleGroups;	// ������� �뵵 - GroupSize
	m_DownScaleCBuffer.fAdaptation = m_fAdatation;			// ������� �뵵 - Adatation (����)
	m_DownScaleCBuffer.fBloomThreshold = m_fBloomThreshold;	// ������� �뵵 - ���

	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("TDownScaleCBuffer", &m_DownScaleCBuffer);

	// Shader
	m_pDownScaleFirstPassShader->SetShader();

	_CONTEXT->Dispatch(m_uiDownScaleGroups, 1, 1);


	// ����
	ID3D11UnorderedAccessView* ResetUAV = nullptr;
	_CONTEXT->CSSetUnorderedAccessViews(0, 1, &ResetUAV, nullptr);
	_CONTEXT->CSSetUnorderedAccessViews(3, 1, &ResetUAV, nullptr);
	_pHDRRenderTarget->ResetCShader(5);



	// Input
	_CONTEXT->CSSetUnorderedAccessViews(0, 1, &m_pAvgLumUAV, nullptr);
	_CONTEXT->CSSetShaderResources(6, 1, &m_pDownScaleSRV);
	_CONTEXT->CSSetShaderResources(7, 1, &m_pPrevAdaptionSRV);

	// Constants
	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("TDownScaleCBuffer", &m_DownScaleCBuffer);

	// Shader
	m_pDownScaleSecondPassShader->SetShader();

	_CONTEXT->Dispatch(1, 1, 1);

	ResetUAV = nullptr;
	_CONTEXT->CSSetUnorderedAccessViews(0, 1, &ResetUAV, nullptr);

	ID3D11ShaderResourceView* pNullSRV = nullptr;
	_CONTEXT->CSSetShaderResources(6, 1, &pNullSRV);
	_CONTEXT->CSSetShaderResources(7, 1, &pNullSRV);
}

void RenderManager::TonMapping(RenderTarget* _pHDRRenderTarget)
{
	m_pPostEffectShader->SetShader();

	// DOF �߰�
	RenderTarget* pDepthRT = FindRenderTarget("Depth");
	pDepthRT->SetShader(10);
	_CONTEXT->PSSetShaderResources(9, 1, &m_pDownScaleSceneSRV);

	_pHDRRenderTarget->SetShader(5);
	_CONTEXT->PSSetShaderResources(6, 1, &m_pAvgLumSRV);
	_CONTEXT->PSSetShaderResources(8, 1, &m_pBloomSRV);

	// ���������
	m_FinalPassCBuffer.fMiddleGrey = m_fMiddleGrey;
	m_FinalPassCBuffer.fLumWhiteSqr = m_fWhite;
	m_FinalPassCBuffer.fLumWhiteSqr *= m_FinalPassCBuffer.fMiddleGrey;
	m_FinalPassCBuffer.fLumWhiteSqr *= m_FinalPassCBuffer.fLumWhiteSqr;
	m_FinalPassCBuffer.fBloomScale = m_fBloomScale;

	m_FinalPassCBuffer.fProjValue[0] = m_fDOFFarStart;

	m_FinalPassCBuffer.fDOFFarRange.x = m_fDOFFarStart;
	m_FinalPassCBuffer.fDOFFarRange.y = m_fDOFFarRange;

	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("TFinalPassCBuffer", &m_FinalPassCBuffer);

	_CONTEXT->IASetInputLayout(NULL);
	UINT iOffSet = 0;
	_CONTEXT->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	_CONTEXT->IASetVertexBuffers(0, 0, nullptr, 0, &iOffSet);
	_CONTEXT->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
	_CONTEXT->Draw(4, 0);

	ID3D11ShaderResourceView* pNullSRV = nullptr;
	pDepthRT->ResetShader(10);
	_pHDRRenderTarget->ResetShader(5);
	_CONTEXT->PSSetShaderResources(6, 1, &pNullSRV);
	_CONTEXT->PSSetShaderResources(8, 1, &pNullSRV);
	_CONTEXT->PSSetShaderResources(9, 1, &pNullSRV);

}

void RenderManager::Adaptation(float _fTime)
{
	// ���� ������ ��� �ֵ����� �������ִ� ���
	ID3D11Buffer* pTempBuffer = m_pAvgLumBuffer;
	ID3D11UnorderedAccessView* pTempUAV = m_pAvgLumUAV;
	ID3D11ShaderResourceView* p_TempSRV = m_pAvgLumSRV;
	m_pAvgLumBuffer = m_pPrevAdaptionBuffer;
	m_pAvgLumUAV = m_pPrevAdaptionUAV;
	m_pAvgLumSRV = m_pPrevAdaptionSRV;
	m_pPrevAdaptionBuffer = pTempBuffer;
	m_pPrevAdaptionUAV = pTempUAV;
	m_pPrevAdaptionSRV = p_TempSRV;


	// ������ ���
	float fElapsedTime = _fTime;
	m_fAdatation = std::fmin(m_fAdatation < 0.0001f ? 1.0f : fElapsedTime / m_fAdatation, 1.0f);
}

void RenderManager::Bloom()
{
	_CONTEXT->CSSetShaderResources(5, 1, &m_pDownScaleSceneSRV);
	_CONTEXT->CSSetShaderResources(6, 1, &m_pAvgLumSRV);
	_CONTEXT->CSSetUnorderedAccessViews(0, 1, &m_pTempUAV[0], NULL);

	m_pBloomBritePassShader->SetShader();

	_CONTEXT->Dispatch(m_uiDownScaleGroups, 1, 1);

	// ����
	ID3D11ShaderResourceView* pNullSRV = nullptr;
	ID3D11UnorderedAccessView* pNullUAV = nullptr;
	_CONTEXT->CSSetShaderResources(5, 1, &pNullSRV);
	_CONTEXT->CSSetShaderResources(6, 1, &pNullSRV);
	_CONTEXT->CSSetUnorderedAccessViews(0, 1, &pNullUAV, NULL);
}

void RenderManager::Blur()
{
	ID3D11ShaderResourceView* pNullSRV = nullptr;
	ID3D11UnorderedAccessView* pNullUAV = nullptr;

	_CONTEXT->CSSetUnorderedAccessViews(0, 1, &m_pTempUAV[1], NULL);
	_CONTEXT->CSSetShaderResources(0, 1, &m_pTempSRV[0]);					// �갡 BritePass�Ⱦ�

	m_pBloomBlurVerticalShader->SetShader();
	_CONTEXT->Dispatch(m_uiDownScaleGroups, 1, 1);

	_CONTEXT->CSSetShaderResources(0, 1, &pNullSRV);
	_CONTEXT->CSSetUnorderedAccessViews(0, 1, &pNullUAV, NULL);

	_CONTEXT->CSSetUnorderedAccessViews(1, 1, &m_pBloomUAV, NULL);
	_CONTEXT->CSSetShaderResources(1, 1, &m_pTempSRV[1]);

	m_pBloomBlurHorizonShader->SetShader();
	_CONTEXT->Dispatch(m_uiDownScaleGroups, 1, 1);

	_CONTEXT->CSSetShaderResources(1, 1, &pNullSRV);
	_CONTEXT->CSSetUnorderedAccessViews(1, 1, &pNullUAV, NULL);
}
