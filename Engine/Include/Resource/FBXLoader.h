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
	std::string	strName;			// �� �̸�
	int		iDepth;					// Ʈ���� ���� (Tree Level, �� ��尡 ���° �������� ..)
	int		iParentIndex;		
	FbxAMatrix	matOffset;			// �޽��� ��� ������ �������� �� ���� ������� 3D max���� ���� �����̴�. �� �� ������ ����� ������Ʈ ���� �� DirectX���� ����� �� �ְ�.. ��ȯ���� ����̴�.
	FbxAMatrix	matBone;			// �ش� ���� �� ���
}FBXBONE, *PFBXBONE;

// Key Frame
typedef	struct ENGINE_DLL _tagFbxKeyFrame
{
	FbxAMatrix	matTransform;			// �ִϸ��̼� ��� ( DirectX ����..)
	double		dTime;					// �ð�
}FBXKEYFRAME, *PFBXKEYFRAME;

// Bone Key Frame
typedef	struct ENGINE_DLL _tagFbxBoneKeyFrame
{
	int		iBoneIndex;								// �ִϸ��̼� �� �ε���
	std::vector<FBXKEYFRAME>	vecKeyFrame;		// �ִϸ��̼� Ű������
}FBXBONEKEYFRAME, *PFBXBONEKEYFRAME;

// Fbx Animation Clip
typedef struct ENGINE_DLL _tagFbxAnimationClip
{
	std::string		strName;		// �ִϸ��̼� Ŭ�� �̸�
	FbxTime			tStart;			// ����
	FbxTime			tEnd;			// ��
	FbxLongLong		lTimeLength;	// �ִϸ��̼� �ð� (��� �ð� ����)
	FbxTime::EMode	eTimeMode;		// Ÿ�Ӹ�� ( ���� 30������ )
	std::vector<FBXBONEKEYFRAME>	vecBoneKeyFrame;		// �ִϸ��̼� ��Ű������
}FBXANIMATIONCLIP, *PFBXANIMATIONCLIP;

typedef struct ENGINE_DLL _tagFbxWeight
{
	int		iIndex;			// �� �ε���
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
	FLT_MESH,			// �޽� ������..
	FLT_ALL				// �޽����� + �ִϸ��̼� ����
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
#pragma region FbxManager ����
	// FbxManager
	// �� �׷���, �� ���(Scene) ������Ʈ���� ������ �׷��� ���·� ǥ���Ѱ�
	// FBX�� ��尡 Ʈ�������� �Ǿ� ������, �޽�, �ִϸ��̼� �����Ӹ��ƴ϶� �ش� 3D���α׷����� ����� ����, ī�޶� ���� �� �ִ�.
#pragma endregion
	FbxManager*		m_pManager;
	FbxScene*		m_pScene;

private:
	std::vector<std::vector<PFBXMATERIAL>>	m_vecMaterials;			// ��Ƽ������ ��� �ִ� ����
	std::vector<PFBXMESHCONTAINER>		m_vecMeshContainer;			// �޽� �����̳ʸ� ��� �ִ� ����	
	std::vector<PFBXBONE>				m_vecBones;					// �޽��� ������ ���� �ִ� ����
	FbxArray<FbxString*>				m_NameArr;					// �ִϸ��̼� �̸����� ���� �ִ� ����
	std::vector<PFBXANIMATIONCLIP>		m_vecClip;					// �ִϸ��̼� Ŭ���� ���� �ִ� ����
	bool								m_bMixamo;					// �ͻ�� ����
	int									m_iLoadType;				// �ε�Ÿ��(����ƽ �޽� / �ִϸ��̼Ǳ��� �ִ� �޽�) ����

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
	// �ִϸ��̼� ����
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

