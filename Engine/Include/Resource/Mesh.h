#pragma once

#include "../Reference.h"

ENGINE_BEGIN

// 버텍스 버퍼
typedef struct ENGINE_DLL _tagVertexBuffer
{
	ID3D11Buffer*	pBuffer;
	void*			pData;
	int				iSize;
	int				iCount;
	D3D11_USAGE		eUsage;
	D3D11_PRIMITIVE_TOPOLOGY	ePrimitive;
}VertexBuffer, *PVertexBuffer;

// 인덱스버퍼
typedef struct ENGINE_DLL _tagIndexBuffer
{
	ID3D11Buffer*	pBuffer;
	void*			pData;
	int				iSize;
	int				iCount;
	D3D11_USAGE		eUsage;
	DXGI_FORMAT		eFormat;
}IndexBuffer, *PIndexBuffer;

// 메쉬 컨테이너
typedef struct ENGINE_DLL _tagMeshContainer
{
	VertexBuffer	tVB;
	std::vector<PIndexBuffer>	vecIB;
}MeshContainer, *PMeshContainer;

// 메쉬 클래스
class ENGINE_DLL Mesh : public Reference
{
	friend class ResourcesManager;

private:
	Mesh();
	~Mesh();

private:
	std::vector<PMeshContainer>	m_MeshContainerVec;			// 메쉬컨테이너 ( 버텍스, 인덱스 버퍼들 )
	std::string m_strShaderKey;								// 셰이더 키를 갖고있음 (무슨 셰이더쓸건지에 대한 키값)
	std::string m_strInputLayoutKey;						// 레이아웃 키값을 갖고있음 이유는 위와 동일

	Vector3 m_vView;
	Vector3 m_vMin;			// Min값
	Vector3 m_vMax;			// Max값
	Vector3 m_vLength;
	Vector3 m_vCenter;
	
	float	m_fRadius;										// m_vLength / 2.0f
	
	class Material*		m_pMaterial;						// 해당 메쉬는 무슨 머티리얼을 사용할 건지 갖고 있다. (FBX)
	class Animation*	m_pAnimation;						// 메쉬는 애니메이션이 있다면 그 애니메이션을 갖고 있다.


public:
	std::string		GetShaderKey()			const;
	std::string		GetInputLayoutKey()		const;
	size_t			GetContainerCount()		const;
	size_t			GetSubSetCount(int _iCotainerNumber = 0)		const;
	Vector3			GetView()	const;

	Vector3			GetCenter() const;
	Vector3			GetMin() const;
	Vector3			GetMax() const;
	float			GetRadiuse() const;

	UINT	IndexCount;

	UINT GetIndexCount() const;

public:
	void SetView(const Vector3& _vView);

public:
	// 메쉬 생성
	bool CreateMesh(const std::string& _strMeshName, const std::string& _strShaderKey, const std::string& _strInputLayoutKey
		, int iVtxSize, int iVtxCount, D3D11_USAGE _eVtxUsage, D3D11_PRIMITIVE_TOPOLOGY _ePrimitive, void* _pVtx, int iIdxSize = 0, int iIdxCount = 0,
		D3D11_USAGE _eIdxUSage = D3D11_USAGE_DEFAULT, DXGI_FORMAT _eFormat = DXGI_FORMAT_UNKNOWN, void* _pIdx = nullptr);


	// 메쉬 로드하기
	bool LoadMesh(const std::string& _strName, const TCHAR* _pFileName,
		const Vector3& _vView = Vector3::Axis[AXIS_Z],
		const std::string& _strPathName = PATH_MESH);
	bool LoadMesh(const std::string& _strName, const char* _pFileName,
		const Vector3& _vView = Vector3::Axis[AXIS_Z],
		const std::string& _strPathName = PATH_MESH);

	bool LoadMeshFromFullPath(const std::string& _strName,
		const TCHAR* _pFullPath,
		const Vector3& _vView = Vector3::Axis[AXIS_Z]);
	bool LoadMeshFromFullPath(const std::string& _strName,
		const char* _pFullPath,
		const Vector3& _vView = Vector3::Axis[AXIS_Z]);

	// 메쉬 렌더 (전부 렌더하는거)
	void Render();

	// 메쉬 렌더 (서브셋만큼 렌더하는거)
	void Render(int _iContainer, int _iSubSet = 0);

	// 인스턴싱인 애들 렌더
	void RenderInstancing(unsigned int _iContainer, unsigned int _iSubSet,
		PInstancingBuffer _pInstancingBuffer,
		int _iInstancingCount);

private:
	// 버텍스 버퍼 생성
	bool CreateVertexBuffer(int _iSize, int iCount, D3D11_USAGE _eUsage,
		D3D11_PRIMITIVE_TOPOLOGY _ePrimitive, void* pData);

	// 인덱스 버퍼 생성
	bool CreateIndexBuffer(int _iSize, int iCount, D3D11_USAGE _eUsage,
		DXGI_FORMAT _eFormat, void* _pData);

	// 상수 버퍼 생성
	bool CreateConstBuffer(UINT _Size, bool dynamic, bool _CPUUpdates, D3D11_SUBRESOURCE_DATA* _pData);

private:
	// 컨버트
	bool ConvertFbx(class FBXLoader* _pLoader,
		const char* _pFullPath);

public:
	// Mesh Save, Load
	bool Save(const char* _pFileName, const std::string& _strPathKey = PATH_MESH);
	bool SaveFromFullPath(const char* _pFullPath);

	bool Load(const char* _pFileName, const std::string& _strPathKey = PATH_MESH);
	bool LoadFromFullPath(const char* _pFullPath);

public:
	// 재질정보 복사용도
	class Material* CloneMaterial();
	
	// 애니메이션 복사용도
	class Animation* CloneAnimation();
};

ENGINE_END