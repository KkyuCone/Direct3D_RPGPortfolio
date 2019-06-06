#pragma once

#include "..\Engine.h"
#include "fbxsdk.h"

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "libfbxsdk-md_Debugx64")
#else
#pragma comment(lib, "libfbxsdk-md_Releasex64")
#endif // _DEBUG
#else
#ifdef _DEBUG
#pragma comment(lib, "libfbxsdk-md_Debugx32")
#else
#pragma comment(lib, "libfbxsdk-md_Releasex32")
#endif // _DEBUG
#endif // _WIN64

ENGINE_BEGIN

// Bone
typedef struct ENGINE_DLL _tagFbxBone
{
	std::string	strName;			// 본 이름
	int		iDepth;					// 트리의 차수 (Tree Level, 본 노드가 몇번째 레벨인지 ..)
	int		iParentIndex;		
	FbxAMatrix	matOffset;			// 메쉬의 행렬 정보를 가져오면 그 원본 본행렬을 3D max상의 뼈대 공간이다. 그 본 공간의 행렬을 오브젝트 공간 즉 DirectX에서 사용할 수 있게.. 변환해줄 행렬이다.
	FbxAMatrix	matBone;			// 해당 원본 본 행렬
}FBXBONE, *PFBXBONE;

// Key Frame
typedef	struct ENGINE_DLL _tagFbxKeyFrame
{
	FbxAMatrix	matTransform;			// 애니메이션 행렬 ( DirectX 상의..)
	double		dTime;					// 시간
}FBXKEYFRAME, *PFBXKEYFRAME;

// Bone Key Frame
typedef	struct ENGINE_DLL _tagFbxBoneKeyFrame
{
	int		iBoneIndex;								// 애니메이션 본 인덱스
	std::vector<FBXKEYFRAME>	vecKeyFrame;		// 애니메이션 키프레임
}FBXBONEKEYFRAME, *PFBXBONEKEYFRAME;

// Fbx Animation Clip
typedef struct ENGINE_DLL _tagFbxAnimationClip
{
	std::string		strName;		// 애니메이션 클립 이름
	FbxTime			tStart;			// 시작
	FbxTime			tEnd;			// 끝
	FbxLongLong		lTimeLength;	// 애니메이션 시간 (모션 시간 길이)
	FbxTime::EMode	eTimeMode;		// 타임모드 ( 보통 30프레임 )
	std::vector<FBXBONEKEYFRAME>	vecBoneKeyFrame;		// 애니메이션 본키프레임
}FBXANIMATIONCLIP, *PFBXANIMATIONCLIP;

typedef struct ENGINE_DLL _tagFbxWeight
{
	int		iIndex;			// 본 인덱스
	double	dWeight;
}FBXWEIGHT, *PFBXWEIGHT;

typedef struct ENGINE_DLL _tagFbxMaterial
{
	Vector4		vDiffuse;
	Vector4		vAmbient;
	Vector4		vSpecular;
	Vector4		vEmissive;
	float		fSpecularPower;
	float		fTransparencyFactor;
	float		fShininess;
	std::string		strDifTex;
	std::string		strBumpTex;
	std::string		strSpcTex;
}FBXMATERIAL, *PFBXMATERIAL;

typedef struct ENGINE_DLL _tagFBXMeshContainer
{
	std::vector<Vector3>	vecPosition;
	std::vector<Vector3>	vecNormal;
	std::vector<Vector2>	vecUV;
	std::vector<Vector3>	vecTangent;
	std::vector<Vector3>	vecBinormal;
	std::vector<Vector4>	vecBlendWeight;
	std::vector<Vector4>	vecBlendIndex;
	std::vector<std::vector<UINT>>	vecIndices;
	std::unordered_map<int, std::vector<FBXWEIGHT>>	mapWeights;
	bool				bBump;
	bool				bAnimation;

	_tagFBXMeshContainer()
	{
		bBump = false;
		bAnimation = false;
	}
}FBXMESHCONTAINER, *PFBXMESHCONTAINER;

enum ENGINE_DLL FBX_LOAD_TYPE
{
	FLT_MESH,			// 메쉬 정보만..
	FLT_ALL				// 메쉬정보 + 애니메이션 정보
};


// FBX Loader

class ENGINE_DLL FBXLoader
{
private:
	friend class Mesh;
	friend class Animation;

public:
	FBXLoader();
	~FBXLoader();

private:
#pragma region FbxManager 설명
	// FbxManager
	// 씬 그래프, 각 장면(Scene) 오브젝트들의 노드들을 그래프 형태로 표현한것
	// FBX는 노드가 트리구조로 되어 있으며, 메쉬, 애니메이션 정보뿐만아니라 해당 3D프로그램에서 사용한 조명, 카메라도 가질 수 있다.
#pragma endregion
	FbxManager*		m_pManager;
	FbxScene*		m_pScene;

private:
	std::vector<std::vector<PFBXMATERIAL>>	m_vecMaterials;			// 머티리얼을 담고 있는 벡터
	std::vector<PFBXMESHCONTAINER>		m_vecMeshContainer;			// 메쉬 컨테이너를 담고 있는 벡터	
	std::vector<PFBXBONE>				m_vecBones;					// 메쉬의 본들을 갖고 있는 벡터
	FbxArray<FbxString*>				m_NameArr;					// 애니메이션 이름들을 갖고 있는 벡터
	std::vector<PFBXANIMATIONCLIP>		m_vecClip;					// 애니메이션 클립을 갖고 있는 벡터
	bool								m_bMixamo;					// 믹사모 여부
	int									m_iLoadType;				// 로드타입(스태틱 메쉬 / 애니메이션까지 있는 메쉬) 여부

public:
	const std::vector<PFBXMESHCONTAINER>* GetMeshContainers()	const;
	const std::vector<std::vector<PFBXMATERIAL>>* GetMaterials()	const;
	const std::vector<PFBXBONE>* GetBones()	const;
	const std::vector<PFBXANIMATIONCLIP>* GetClips()	const;

public:
	bool LoadFbx(const char* pFullPath);

private:
	void Triangulate(FbxNode* pNode);
	void LoadMaterial(FbxSurfaceMaterial * pMtrl);
	Vector4 GetMaterialColor(FbxSurfaceMaterial * pMtrl,
		const char * pPropertyName, const char * pPropertyFactorName);
	double GetMaterialFactor(FbxSurfaceMaterial * pMtrl,
		const char * pPropertyName);
	std::string GetMaterialTexture(FbxSurfaceMaterial * pMtrl, const char * pPropertyName);

	void LoadMesh(FbxNode* pNode);
	void LoadMesh(FbxMesh* pMesh);

	void LoadNormal(FbxMesh * pMesh, PFBXMESHCONTAINER pContainer, int iVtxID,
		int iControlIndex);
	void LoadUV(FbxMesh * pMesh, PFBXMESHCONTAINER pContainer,
		int iUVID, int iControlIndex);
	void LoadTangent(FbxMesh * pMesh,
		PFBXMESHCONTAINER pContainer, int iVtxID, int iControlIndex);
	void LoadBinormal(FbxMesh * pMesh, PFBXMESHCONTAINER pContainer,
		int iVtxID, int iControlIndex);

private:
	// 애니메이션 관련
	void LoadAnimationClip();
	void LoadBone(FbxNode* pNode);
	void LoadBoneRecursive(FbxNode* pNode, int iDepth, int iIndex, int iParent);
	void LoadAnimation(FbxMesh* pMesh, PFBXMESHCONTAINER pContainer);
	FbxAMatrix GetTransform(FbxNode * pNode);
	int FindBoneFromName(const std::string & strName);
	void LoadWeightAndIndex(FbxCluster * pCluster,
		int iBoneIndex, PFBXMESHCONTAINER pContainer);
	void LoadOffsetMatrix(FbxCluster * pCluster,
		const FbxAMatrix & matTransform, int iBoneIndex,
		PFBXMESHCONTAINER pContainer);
	void LoadTimeTransform(FbxNode * pNode,
		FbxCluster * pCluster, const FbxAMatrix & matTransform,
		int iBoneIndex);
	void ChangeWeightAndIndices(PFBXMESHCONTAINER pContainer);
};

ENGINE_END

