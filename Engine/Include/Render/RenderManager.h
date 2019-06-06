#pragma once
#include "..\GameObject.h"

ENGINE_BEGIN

#define INSTNACING_BUFFER_SIZE 4096
#define INSTANCING_COUNT		5				// 인스턴싱으로 바꿀 최소 개수

// 라이트를 들고 있는 오브젝트들은 따로 목록을 빼서
// 조명효과를 줄거얌 ㅇㅅㅇ
// STL은 느리니까 **를 이용해서 사용할거임 ㅇㅅㅇ ( 그냥 따로 만들어주는거 )
typedef struct ENGINE_DLL _tagRenderList
{
	GameObject**	pObjectList;
	int				iSize;				// 하나 크기
	int				iCapacity;			// 용량 ( 총 개수 )

	_tagRenderList()
	{
		iSize = 0;
		iCapacity = 100;			//일단 시작은 100으로 시작하고 부족하면 추가하는 형식으로 한다.
		pObjectList = new GameObject*[iCapacity];
		ZeroMemory(pObjectList, sizeof(GameObject*)*iCapacity);
	}

	~_tagRenderList()
	{
		SAFE_DELETE_ARRAY(pObjectList);
	}

}RenderList, *PRenderList;

// MRT (Multi-RenderTarget)
// MRT는 새로 만드는 것이 아닌 기존의 렌더매니저 클래스의 RenderTartget모음에서 
// MRT로 사용할 렌더타겟들만 빼온다.
// 즉 새로만드는 것이 아닌 기존의 것을 활용하는 방식으로 사용한다.
// 렌더타겟들을 묶어주는 (MRT로 만들..) 구조체
// 깊이정보를 사용할 수 있고 깊이값을 교체가 가능하도록 한다.
// AddMRT(), AddDepth(), FindMRT()
typedef struct ENGINE_DLL _tagMRT
{
	std::vector<class RenderTarget*>		RenderTargetVec;		// MRT의 렌더타겟 모음
	std::vector<ID3D11RenderTargetView*>	OldRenderTargetVec;		// 월드 렌더 타겟
	ID3D11DepthStencilView*					pDepth;					// 깊이정보
	ID3D11DepthStencilView*					pOldDepth;				// 기존의, 예전 뎁스뷰

	_tagMRT() :
		pDepth(nullptr),
		pOldDepth(nullptr)
	{

	}

}MRT, *PMRT;


class ENGINE_DLL RenderManager
{
private:
	ShadowBiasCBuffer m_ShadowBiasCB;

private:
	std::unordered_map<std::string, class RenderState*>		m_RenderStateMap;	// 렌더 스테이트 모음 ( 래스터라이저, 뎁스스텐실, 블렌드 ) 여부
	std::unordered_map<std::string, class RenderTarget*>	m_RenderTargetMap;	// 렌더 타겟모음 ( 디퍼드 렌더링을 하기 위해 필요.. MRT용도)

	std::unordered_map<std::string, PMRT>					m_MRTMap;			// MRT

	RenderList		m_tLightList;				// 라이트 리스트 (라이트 뭐쓰는지..), 조명모음
	RenderList		m_tRenderGroup[RG_END];		// 렌더 그룹별 모음 (렌더 그룹별로 렌더링 해줄거임)

	RENDER_MODE		m_eMode;					// 렌더모드 (포워드, 디퍼드, 기본은 디퍼드)
	RenderCbuffer	m_tRenderCBuffer;			// 렌더모드를 셰이더에 넘겨줄 상수버퍼용 구조체
	DepthFogCBuffer	m_tDepthFogCBuffer;

	// 이제 렌더매니저가 샘플러를 알 필요가 없다. ( 리소스 매니저에서 생성후 셰이더로 바로 넣어줬음 셰이더에서 원하는 샘플러 불러서 사용하면됨)
	//class Sampler*	m_pLinearSampler;
	//class Sampler*	m_pPointSampler;

	class RenderState*	m_pDepthDisable;		// 뎁스 사용 X
	class RenderState*	m_pAccBlend;			// 누적용 블렌드
	class RenderState*	m_pAlphaBlend;			// 알파 블렌드
	class RenderState*	m_pCullNone;			// CullNone 모드 (Rasterizer)

	class Shader*		m_pLightAccDirShader;		// 조명누적용 셰이더 - Directinal Light
	class Shader*		m_pLightAccPointShader;		// 조명누적용 셰이더 - Point Light
	class Shader*		m_pLightAccSpotShader;		// 조명누적용 셰이더 - Spot Light
	class Shader*		m_pLightBlendShader;		// 라이트에 블렌드 적용한 셰이더
	class Shader*		m_pLightBlendRenderShader;	// 라이트 블렌드 렌더 셰이더

	class Mesh*			m_pLightPointVolume;		// 볼륨메쉬 - Point
	class Mesh*			m_pLightSpotVolume;			// 볼륨메쉬 - Spot

	ID3D11InputLayout*	m_pLightPointLayout;		// 볼륨메쉬레이아웃 - Point
	ID3D11InputLayout*	m_pLightSpotLayout;			// 볼륨메쉬레이아웃 - Spot

private:
	// 인스턴싱 관련
	// 첫번째 4바이트 : 메쉬 , 두번째 4바이트 : 머티리얼 -> __int64
	std::unordered_map<unsigned __int64, PInstancingGeometry>	m_mapInstancingGeometry;

	std::list<PInstancingGeometry> m_InstancingList[RG_END];

	PInstancingBuffer	m_pStaticInstancing;
	PInstancingBuffer	m_pAnimFrameInstancing;
	PInstancingBuffer	m_pAnimInstancing;
	PInstancingBuffer	m_pColliderInstancing;			// 위처럼 얘도 추가해주면 된다.
	PInstancingBuffer	m_pLightInstancing;				// 얘도 ㅇㅅㅇ

	class Shader*		m_pStaticInstancingShader;
	class Shader*		m_pAnimFrameInstancingShader;
	class Shader*		m_pAnimInstancingShader;

	ID3D11InputLayout*		m_pStaticInstancingLayout;
	ID3D11InputLayout*		m_pAnimFrameInstancingLayout;
	ID3D11InputLayout*		m_pAnimInstancingLayout;

private:
	// 인스턴싱 관련
	PInstancingGeometry FindInstancingGeometry(unsigned __int64 _iKey);
	PInstancingBuffer CreateInstancingBuffer(int _iSize, int _iCount = INSTNACING_BUFFER_SIZE);
	void ResizeInstancingBuffer(PInstancingBuffer _pBuffer, int _iCount);
	void AddInstancingData(PInstancingBuffer _pBuffer, int _iPos, void* _pData);
	void CopyInstancingData(PInstancingBuffer _pBuffer, int _iCount);

private:
	bool m_bShadowCompute;
	class Shader* m_pDynamicShadowShader;
	class Shader* m_pStaticShadowShader;

public:
	void SetShadowCompute(bool _Value);
	bool GetShadowCompute();

private:
	//포스트 이펙트 관련

	// HDR
	bool	m_bPostEffect;
	bool	m_bBloom;
	//bool	m_bBlur;

	UINT m_uiWidth;
	UINT m_uiHeight;
	UINT m_uiDomain;
	UINT m_uiDownScaleGroups;
	float m_fAdatation;
	float m_fBloomThreshold;
	float m_fBloomScale;

	float m_fMiddleGrey;
	float m_fWhite;

	float m_fDOFFarStart;
	float m_fDOFFarRange;

	ID3D11Buffer*				m_pDownScaleBuffer;			// 휘도값을 계산할 때 중간값을 저장할 버퍼
	ID3D11UnorderedAccessView*	m_pDownScaleUAV;			// 순서 없는 접근 뷰
	ID3D11ShaderResourceView*	m_pDownScaleSRV;			// 셰이더 리소스 뷰

	ID3D11Buffer*				m_pAvgLumBuffer;			// 평균 휘도 값을 저장할 버퍼
	ID3D11UnorderedAccessView*	m_pAvgLumUAV;				// 순서 없는 접근 뷰
	ID3D11ShaderResourceView*	m_pAvgLumSRV;				// 셰이더 리소스 뷰

	TDownScaleCBuffer m_DownScaleCBuffer;
	TFinalPassCBuffer m_FinalPassCBuffer;

	class Shader*	m_pPostEffectShader;
	class Shader*	m_pDownScaleFirstPassShader;
	class Shader*	m_pDownScaleSecondPassShader;

	// 적응
	// 각 프레임 렌더링을 할 때마다 현재 평균 휘도값을 버퍼에 저장하고 직전 프레임의 평균 휘도 값을 
	// 다른 버퍼에 저장..
	ID3D11Buffer*				m_pPrevAdaptionBuffer;			// 다음 프레임의 직전 평균 휘도값을 저장할 버퍼
	ID3D11UnorderedAccessView*	m_pPrevAdaptionUAV;
	ID3D11ShaderResourceView*	m_pPrevAdaptionSRV;

	// 블룸
	// 밝은 픽셀에서 주변의 어두운 픽셀 쪽으로 빛을 흘리는 것
	// 1. 다운 스케일된 HDR 텍스처
	// 2. 필터링되기 전의 블룸 값을 임시로 저장하기 위한 영역
	// 3. 최종 블룸 값
	ID3D11Texture2D*			m_pDownScaleSceneRT;		// 다운 스케일된 HDR 텍스처
	ID3D11UnorderedAccessView*	m_pDownScaleSceneUAV;
	ID3D11ShaderResourceView*	m_pDownScaleSceneSRV;

	ID3D11Texture2D*			m_pTempRT[2];				// 임시 텍스처 (영역)
	ID3D11UnorderedAccessView*	m_pTempUAV[2];
	ID3D11ShaderResourceView*	m_pTempSRV[2];

	ID3D11Texture2D*			m_pBloomRT;
	ID3D11UnorderedAccessView*	m_pBloomUAV;
	ID3D11ShaderResourceView*	m_pBloomSRV;

	class Shader*	m_pBloomBritePassShader;
	class Shader*	m_pBloomBlurVerticalShader;
	class Shader*	m_pBloomBlurHorizonShader;

private:
	void PostEffectInit();
	void DownScale(RenderTarget* _pHDRSRV);				// HDR
	void TonMapping(RenderTarget* _pHDRSRV);			// HDR
	void Adaptation(float _fTime);						// 적응
	void Bloom();										// 블룸
	void Blur();										// 블러

private:
	BLimLightCBuffer m_bLimLightCBuffer;

private:
	bool m_bSSAO;										// SSAO적용 여부
	bool m_bDepthFog;									// 깊이 기반 안개 적용 여부 
	bool m_bRain;										// 강우 효과 적용 여부

public:
	void SetSSAOEnable(bool _Enable);
	bool GetSSAOEnable();

	void SetDepthFogEnable(bool _Enable);
	bool GetDepthFogEnable();

	void SetRainEnable(bool _Enable);					// 강우 효과 적용 여부
	bool GetRainEnable();

public:
	void SetPostEffectEnable(bool _Enable);
	void SetBloomEnable(bool _Enable);

public:
	class Light* GetFirstLight()	const;

public:
	bool Init();
	void ShaderLyaoutInit();
	bool SSAOInit();
	bool DepthFogInit();
	bool RenderStateInit();
	bool RenderTargetInit();
	void AddRenderObject(GameObject* _pObject);			// 오브젝트들 렌더그룹별로 넣어주기

public:
	// Rasterizer 만들기 
	bool CreateRasterizerState(const std::string& _strName, D3D11_FILL_MODE _eFill = D3D11_FILL_SOLID,
		D3D11_CULL_MODE _eCull = D3D11_CULL_BACK);

	// DepthState 만들기
	bool CreateDepthState(const std::string& _strName, BOOL _bEnable = TRUE,
		D3D11_DEPTH_WRITE_MASK _eWirteMaske = D3D11_DEPTH_WRITE_MASK_ALL,
		D3D11_COMPARISON_FUNC _eDpethFunc = D3D11_COMPARISON_LESS);

	// Blend Option sruct 만들기
	bool AddTargetBlendDesc(const std::string& _strName,
		BOOL _bEnable = FALSE,
		D3D11_BLEND _eSrcBlend = D3D11_BLEND_ONE,
		D3D11_BLEND _eDestBlend = D3D11_BLEND_ZERO,
		D3D11_BLEND_OP _eOp = D3D11_BLEND_OP_ADD,
		D3D11_BLEND _eSrcBlendAlpha = D3D11_BLEND_ONE,
		D3D11_BLEND _eDestBlendAlpha = D3D11_BLEND_ZERO,
		D3D11_BLEND_OP _eAlphaOp = D3D11_BLEND_OP_ADD,
		UINT _iWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL);

	// Blend State 만들기
	bool CreateBlendState(const std::string& _strName, BOOL _bAlphaToCoverage = FALSE,
		BOOL _bIndependentBlend = FALSE);

	// 렌더타겟 만들기 ( 이름, 가로, 세로, 렌더타겟 텍스처 포맷, 클리어색상, 뎁스 포맷)
	bool CreateRenderTarget(const std::string& _strName,
		UINT _iWidth, UINT _iHeight, DXGI_FORMAT _eFormat,
		float _fClearColor[4], int _iSamplerCount = 1,
		DXGI_FORMAT _eDepthFormat = DXGI_FORMAT_UNKNOWN);

	// 디버그용 렌더타겟
	bool OnDebugRenderTarget(const std::string& _strName, const Vector3& _vPos,
		const Vector3& _vScale);

public:
	// MRT용 함수
	bool AddMRT(const std::string& _strMRTName, const std::string& _strTarget);
	bool AddDepth(const std::string& _strMRTName, const std::string& _strTargetName);
	void ClearMRT(const std::string& _strMRTName);
	void SetMRT(const std::string& _strMRTName);
	void ResetMRT(const std::string& _strMRTName);

public:
	class RenderState* FindRenderState(const std::string& _strName);		// 렌더스테이트 찾기
	class RenderTarget* FindRenderTarget(const std::string& _strName);		// 렌더 타겟 찾기
	PMRT FindMRT(const std::string& _strName);								// MRT 찾기

public:
	void ComputeInstancing();					// 인스턴싱 할건지 아닌지 분류작업 함수
	void Render(float _fTime);

private:
	void RenderForward(float _fTime);			// 포워드 렌더링
	void RenderDeferred(float _fTime);			// 디퍼드 렌더링

	// GBuffer  ( G : Geometry )
	// 디퍼드는 종류별로 나눠줘야한다. 
	// GBuffer를 위한 렌더타겟을 만들어주기
	void RenderGBuffer(float _fTime);

	// 조명렌더 함수들
	void RenderLightAcc(float _fTime);									// 조명들 누적할 함수
	void RenderLightDir(float _fTime, class Light* _pLight);			// DirectionalLight Render
	void RenderLightPoint(float _fTime, class Light* _pLight);			// PointLight Render
	void RenderLightSpot(float _fTime, class Light* _pLight);			// SpotLight Render

	void RenderLightBlend(float _fTime);
	void RenderLightBlendRender(float _fTime);

	// 알파적용된 애들은 따로 인스턴싱 함수로 빼기
	void RenderInstancingAlpha(float _fTime);

	void RenderPostProcessing(float _fTime);

	void RenderShadow(float _fTime);
	void RenderShadowMovingMesh(Matrix _matView, Matrix _matProj, float _fTime);

	void RenderSSAO(float _fTime);

	void RenderDepthFog(float _fTime);

	DECLARE_SINGLETON(RenderManager)
};

ENGINE_END

