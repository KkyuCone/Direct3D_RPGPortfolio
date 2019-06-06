#pragma once
#include "Flag.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"

ENGINE_BEGIN

// 해상도
typedef struct ENGINE_DLL _tagResolution
{
	int	iWidth;
	int	iHeight;
}Resolution, *PResolution;

// Color Vertex Buffer
typedef struct ENGINE_DLL _tagVertexColor
{
	Vector3	vPos;
	Vector4 vColor;

	_tagVertexColor()
	{

	}

	_tagVertexColor(const _tagVertexColor& _Vtx)
	{
		*this = _Vtx;
	}

	_tagVertexColor(const Vector3& _vPos, const Vector4& _vColor) : vPos(_vPos), vColor(_vColor)
	{

	}
}VertexColor, *PVertexColor;

// Color Normal Vertex Buffer
typedef struct ENGINE_DLL _tagVertexNormalColor
{
	Vector3	vPos;
	Vector3 vNormal;
	Vector4 vColor;

	_tagVertexNormalColor()
	{

	}

	_tagVertexNormalColor(const _tagVertexNormalColor& _Vtx)
	{
		*this = _Vtx;
	}

	_tagVertexNormalColor(const Vector3& _vPos, const Vector3& _vNormal, const Vector4& _vColor) 
		: vPos(_vPos), vNormal(_vNormal), vColor(_vColor)
	{

	}
}VertexNormalColor, *PVertexNormalColor;

// Vertex UV
typedef struct ENGINE_DLL _tagVertexUV
{
	Vector3	vPos;
	Vector2 vUV;

	_tagVertexUV()
	{

	}

	_tagVertexUV(const _tagVertexUV& _Vertex)
	{
		*this = _Vertex;
	}

	_tagVertexUV(const Vector3& _vPos, const Vector2& _vUV) 
		: vPos(_vPos), vUV(_vUV)
	{

	}
}VertexUV, *PVertexUV;

// 디폴트 파티클 구조체  ( 기하셰이더를 이용해서 점 -> 사각형 메쉬로 확장해서 렌더할 것이다. )
// -> 메쉬 렌더링, 빌보드이용해서 파티클 형성할거임
typedef struct ENGINE_DLL _tagVertexDefulatParticle
{
	Vector3	vPos;
	Vector2	vSize;

	_tagVertexDefulatParticle()
	{

	}

	_tagVertexDefulatParticle(const _tagVertexDefulatParticle& _Vertex)
	{
		*this = _Vertex;
	}

	_tagVertexDefulatParticle(const Vector3& _vPos, const Vector2& _vSize) : 
		vPos(_vPos), vSize(_vSize)
	{

	}
}VertexDefaultParticle, *PVertexDefaultParticle;

// 파티클 구조체 -> 얘 cbuffer(b11)로 쓸거 ㅇㅅㅇ (쌤은 10)
typedef struct ENGINE_DLL _tagParticleCBuffer
{
	Vector3	vParticlePos;				// 파티클 위치 정보 ( 월드공간의.., 그래서 후에 VP가 필요)
	float	fSizeX;						// 가로 사이즈
	Vector3 vAxisX;						// X축
	float	fSizeY;						// 세로 사이즈
	Vector3	vAxisY;						// Y축
	float	fEmpty;						// 얘는 걍 16바이트씩 맞춰줄려고 ㅇㅅㅇ
}ParticleCBuffer, *PParticleCBuffer;

// Constant Buffer
typedef struct ENGINE_DLL _tagConstBuffer
{
	std::string	strName;
	ID3D11Buffer*	pBuffer;
	void*	pData;
	int		iSize;
	int		iRegisterSlot;				// 레지스터 번호
	int		iConstant;					// 버텍스셰이더, 픽셀셰이더, 기하셰이더 중 어디 셰이더에서 사용할건지
}ConstBuffer, *PConstBuffer;

// 얜 Transform.h, cpp에서 셋해준다.
typedef struct ENGINE_DLL _tagTransformConstBuffer
{
	Matrix	WorldRotationMatrix;		// 월드 회전행렬 
	Matrix	WorldViewRotationMatrix;	// 월드뷰회전 행렬 (노멀벡터 이용)
	Matrix	WorldMatrix;				// 월드 행렬
	Matrix	ViewMatrix;					// 뷰 행렬
	Matrix  InversViewMatrix;
	Matrix	ProjectionMatrix;			// 프로젝션 행렬
	Matrix	InversProjectionMatrix;		// 프로젝션 역행렬
	Matrix	WorldViewMatrix;			// 월드 뷰 행렬
	Matrix	WorldViewProjectionMatrix;		// 월드 뷰ㅠ 프로젝션 행렬
	Matrix	ViewProjectionMatrix;			// 뷰 프로젝션 행렬   -> 빌보드할때 필요
}TransformConstBuffer, *PTransformConstBuffer;

typedef struct ENGINE_DLL _tagMaterialCBuffer
{
	Vector4 vDiffuse;
	Vector4 vAmbient;
	Vector4 vSpecular;
	Vector4 vEmissive;
	int		iNormal;			// 노말값이 있는지 여부 (디퓨즈는 필수사항 그 외는 선택사항임)
	int		iSpeuclar;			// Sepcular텍스처 유무
	int		iSkinning;			// 애니메이션 있는지 여부
	float	fEmpty;

	_tagMaterialCBuffer()
		: vDiffuse(Vector4::White), 
		vAmbient(Vector4::White), 
		vSpecular(Vector4::Black), 
		vEmissive(Vector4::Black),
		iNormal(0),
		iSpeuclar(0),
		iSkinning(0),
		fEmpty(0.0f)
	{

	}
}MaterialCBuffer, *PMaterialCBuffer;

// 3D 
typedef struct ENGINE_DLL _tagVertexTextur3D
{
	Vector3 vPosition;
	Vector3 vNormal;
	Vector2 vUV;
	Vector3 vTangent;
	Vector3 vBinormal;
	Vector4 vBlendWeights;
	Vector4 vBlendIndices;
}Vertex3D, *PVertex3D;

// 라이트 정보
typedef struct ENGINE_DLL _tagLightInfo
{
	Matrix matShadowLightVP;
	Matrix matInvCamView;
	Vector4	vDiffuse;		// 디퓨즈 라이트 색상
	Vector4	vAmbient;
	Vector4	vSpecular;
	Vector3	vPosition;
	int		iType;			// 라이트 타입 (포인트, 디렉셔널, 스폿)
	Vector3 vDirection;		// 라이트 방향
	float	fDistance;		// 라이트 거리
	float	fInAngle;		// 스폿라이트 같은 안쪽 각도
	float	fOutAngle;		// 바깥쪽 각도
	Vector2	vEmpty;			// 그냥 상수버퍼 16바이트로 맞춰주기 위해서 사용한 변수 (의미 없는 변수)

	_tagLightInfo() :
		vDiffuse(Vector4::White),
		vAmbient(Vector4(0.2f, 0.2f, 0.2f, 1.0f)),
		vSpecular(Vector4::White),
		iType(LT_DIR),
		fDistance(0.0f),
		fInAngle(0.0f),
		fOutAngle(0.0f),
		vEmpty(Vector2::Zero)
	{
		matShadowLightVP.Identity();
	}
}LightInfo, *PLightInfo;

// 렌더모드를 알려줄 상수버퍼용 구조체
typedef struct ENGINE_DLL _tagRenderCBuffer
{
	int	iRenderMode;
	int	bSSAO;
	int	bShadow;
	int	bPostEffect;
	int	bFog;
	int bRain;
	Vector2 vEmpty;
	_tagRenderCBuffer()
	{
		bSSAO = 0;
		bShadow = 0;
		bPostEffect = 0;
		bFog = 0;
		bRain = false;
	}

}RenderCbuffer, *PRenderCBuffer;

// 렌더타겟 디버그에 사용할 월드행렬
typedef struct ENGINE_DLL _tagDebugCBuffer
{
	Matrix WVPMatrix;
}DebugCBuffer, *PDebugCBuffer;


#pragma region 충돌 관련 구조체

// 구 충돌
typedef struct ENGINE_DLL _tagSphereInfo
{
	Vector3 vCenter;
	float fRadius;

	_tagSphereInfo() : fRadius(0.0f)
	{
	}
}SphereInfo, *PSphereInfo;

// AABB 충돌
typedef struct ENGINE_DLL _tagAABBInfo
{
	Vector3 vMin;
	Vector3 vMax;

	_tagAABBInfo()
	{

	}
}AABBInfo, *PAABBInfo;

// OBB 충돌
typedef struct ENGINE_DLL _tagOBBInfo
{
	Vector3 vCenter;			// 물체의 중심위치
	Vector3 vAxis[AXIS_END];	// 축 ( 기본축 3개를 쓸거 x,y,z축 )
	Vector3 vHalfLength;		// 절반의 길이

	_tagOBBInfo()
	{
		for (int i = 0; i < AXIS_END; ++i)
		{
			vAxis[i] = Vector3::Axis[i];
			vHalfLength[i] = 1.0f;
		}
	}

}OBBInfo, *POBBInfo;

// Ray 충돌
typedef struct ENGINE_DLL _tagRayInfo
{
	Vector3 vOrigin;			// 시작점
	Vector3 vDir;				// 방향
	float fDist;				// 거리

	_tagRayInfo()
	{
		fDist = -1.0f;			// 음수는 Infinity 로 처리
	}

}RayInfo, *PRayInfo;

#pragma endregion

typedef struct ENGINE_DLL _tagAnimationFrameCBuffer
{
	int iAnimationFrameType;		// 애니메이션 타입 -> 아틀라스, 프레임 인지 
	int iAnimationOption;			// 애니메이션 옵션
	Vector2 vTextureSize;			// 텍스처 크기
	Vector2 vStart;
	Vector2 vEnd;
	int iFrame;
	Vector3 vEmpty;
}AnimationFrameCBuffer, *PAnimationFrameCBuffer;

// 지형 디테일 레벨, 스플래팅개수 구조체
typedef struct ENGINE_DLL _tagLandScaleCBuffer
{
	float fDetailLevelX;
	float fDetailLevelZ;
	int iSplatCount;
	float fEmpty;

	_tagLandScaleCBuffer()
	{
		fDetailLevelX = 1;
		fDetailLevelZ = 1;
		iSplatCount = 0;
		fEmpty = 0.0f;
	}
}LandScaleCBuffer, *PLandScaleCBuffer;

typedef struct ENGINE_DLL _tagButtonCBuffer
{
	Vector4 vColor;
}ButtonCBuffer, *PButtonCBuffer;


typedef struct ENGINE_DLL _tagImageCBuffer
{
	Vector4 vColor;
}ImageCBuffer, *PImageCBuffer;

typedef struct ENGINE_DLL _tagBarCBuffer
{
	Vector4 vColor;
	float fRatio;
	Vector3 vEmpty;
	
}BarCBuffer, *PBarCBuffer;

#pragma region 인스턴싱

// 인스턴싱 상수 버퍼
typedef struct ENGINE_DLL _tagInstancingStaticBuffer
{
	Matrix matWVP;			// WVP
	Matrix matWV;
	Matrix matWVRot;
}InstancingStaticBuffer, *PInstancingStaticBuffer;

// 인스턴싱 - 애니메이션 2D 버퍼
typedef struct ENGINE_DLL _tagInstancingAnimFrameBuffer
{
	Matrix matWVP;
	Matrix matVP;
	Vector3 vParticlePos;
	Vector2 vParticleScale;
	Vector2 vStart;
	Vector2 vEnd;
	int iFrame;

}InstancingAnimFrameBuffer, *PInstancingAnimFrameBuffer;

// 인스턴싱 버퍼
typedef struct ENGINE_DLL _tagInstancingBUffer
{
	ID3D11Buffer*	pBuffer;
	void*			pData;
	int				iSize;
	int				iCount;
	D3D11_USAGE		eUsage;
}InstancingBuffer, *PInstancingBuffer;

// 인스턴싱 구조체
typedef struct ENGINE_DLL _tagInstancingGeometry
{
	bool	bAnimation2D;				// 2D애니메이션 여부 (프레임 애니메이션)
	bool	bAnimation3D;				// 3D 애니메이션 여부
	class GameObject** pGameObjectList;		// 게임 오브젝트들 묶음 
	int		iSize;						// 개수
	int		iCapacity;

	// 공간 늘리기
	void Resize()
	{
		if (iSize == iCapacity)
		{
			iCapacity *= 2;
			class GameObject** pList = new GameObject*[iCapacity];

			memset(pList, 0, sizeof(GameObject*) * iCapacity);				// 0 으로 초기화
			memcpy(pList, pGameObjectList, sizeof(GameObject*) * iSize);	// 늘리기 이전까지의 ㅇ게임오브젝트 리스트 넣어주기

			SAFE_DELETE_ARRAY(pGameObjectList);
			pGameObjectList = pList;
		}
	}

	// 추가 
	void Add(class GameObject* _pObject)
	{
		Resize();
		pGameObjectList[iSize] = _pObject;
		++iSize;
	}

	void Clear()
	{
		iSize = 0;
	}

	_tagInstancingGeometry()
	{
		iSize = 0;
		iCapacity = 100;
		bAnimation2D = false;
		bAnimation3D = false;
		pGameObjectList = new GameObject*[iCapacity];
		memset(pGameObjectList, 0, sizeof(GameObject*) * iCapacity);
	}

	~_tagInstancingGeometry()
	{
		SAFE_DELETE_ARRAY(pGameObjectList);
	}
}InstancingGeometry, *PInstancingGeometry;


#pragma endregion


#pragma region 포스트 프로세싱 관련

typedef struct _tagTDownScaleCBuffer
{
	UINT nWidth;
	UINT nHeight;
	UINT nTotalPixels;
	UINT nGroupSize;
	float fAdaptation;
	float fBloomThreshold;
	Vector2 vEmpty;
} TDownScaleCBuffer, *PTDownScaleCBuffer;

typedef struct _tagTFinalPassCBuffer
{
	float fMiddleGrey;
	float fLumWhiteSqr;
	float fBloomScale;
	float vEmpty;
	Vector2 fProjValue;
	Vector2 fDOFFarRange;
} TFinalPassCBuffer, *PTFinalPassCBuffer;

#pragma endregion


#pragma region 그림자 관련

typedef struct _tagShadowMatCBuffer
{
	Matrix matWorld;
	Matrix matView;
	Matrix matProj;
	Matrix matLightView;
	Matrix matLightProj;

}ShadowMatCBuffer, *PShadowMatCBuffer;

typedef struct _tagShadowLightVSCBufferss
{
	Matrix fLightPosition;
	Matrix fEmpty;
}ShadowLightVSCBuffer, *PShadowLightVSCBuffer;

typedef struct _tagShadowLightPSBuffer
{
	Vector4 fAmbientColor;
	Vector4 fDiffuseColor;
}ShadowLightPSBuffer, *PShadowLightPSBuffer;

typedef struct _tagShadowBiasCBuffer
{
	float fShadowBias;
	Vector3 vEmpty;
}ShadowBiasCBuffer, *PShadowBiasCBuffer;

#pragma endregion

#pragma region SSOA

typedef struct _tagSSAODownScaleCBuffer
{
	UINT inWidth;
	UINT inHeight;
	float fHorResRcp;
	float fVerResRcp;
	Vector4 vProjParams;
	Matrix matView;
	float fOffsetRadius;
	float fRadius;
	float fMaxDepth;
	int iEmpty;
}SSAODownScaleCBuffer, *PSSAODownScaleCBuffer;

#pragma endregion

#pragma region DepthFog

typedef struct _tagDepthFogCBuffer
{
	Vector3 vFogColor;
	float	fFogStartDepth;
	Vector3	vFogHightLightColor;
	float	fFogGlobalDensity;
	Vector3	vFogSunDir;
	float	fFogHeightFalloff;
	Vector3  g_CamPos;
	float   fEmpty;
}DepthFogCBuffer, *PDepthFogCBuffer;

#pragma endregion

#pragma region bLimlight

typedef struct _tagbLimlightCBuffer
{
	UINT bLimlight;
	Vector3 vEmpty;
}BLimLightCBuffer, *PBLimLightCBuffer;
#pragma endregion


ENGINE_END