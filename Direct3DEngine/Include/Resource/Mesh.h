#pragma once

#include "../Reference.h"

ENGINE_BEGIN

// ���ؽ� ����
typedef struct ENGINE_DLL _tagVertexBuffer
{
	ID3D11Buffer*	pBuffer;
	void*			pData;
	int				iSize;
	int				iCount;
	D3D11_USAGE		eUsage;
	D3D11_PRIMITIVE_TOPOLOGY	ePrimitive;
}VertexBuffer, *PVertexBuffer;

// �ε�������
typedef struct ENGINE_DLL _tagIndexBuffer
{
	ID3D11Buffer*	pBuffer;
	void*			pData;
	int				iSize;
	int				iCount;
	D3D11_USAGE		eUsage;
	DXGI_FORMAT		eFormat;
}IndexBuffer, *PIndexBuffer;

// �޽� �����̳�
typedef struct ENGINE_DLL _tagMeshContainer
{
	VertexBuffer	tVB;
	std::vector<PIndexBuffer>	vecIB;
}MeshContainer, *PMeshContainer;

// �޽� Ŭ����
class ENGINE_DLL Mesh : public Reference
{
	friend class ResourcesManager;

private:
	Mesh();
	~Mesh();

private:
	std::vector<PMeshContainer>	m_MeshContainerVec;			// �޽������̳� ( ���ؽ�, �ε��� ���۵� )
	std::string m_strShaderKey;								// ���̴� Ű�� �������� (���� ���̴��������� ���� Ű��)
	std::string m_strInputLayoutKey;						// ���̾ƿ� Ű���� �������� ������ ���� ����

	Vector3 m_vView;
	Vector3 m_vMin;			// Min��
	Vector3 m_vMax;			// Max��
	Vector3 m_vLength;
	Vector3 m_vCenter;
	
	float	m_fRadius;										// m_vLength / 2.0f
	
	class Material*		m_pMaterial;						// �ش� �޽��� ���� ��Ƽ������ ����� ���� ���� �ִ�. (FBX)
	class Animation*	m_pAnimation;						// �޽��� �ִϸ��̼��� �ִٸ� �� �ִϸ��̼��� ���� �ִ�.


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
	// �޽� ����
	bool CreateMesh(const std::string& _strMeshName, const std::string& _strShaderKey, const std::string& _strInputLayoutKey
		, int iVtxSize, int iVtxCount, D3D11_USAGE _eVtxUsage, D3D11_PRIMITIVE_TOPOLOGY _ePrimitive, void* _pVtx, int iIdxSize = 0, int iIdxCount = 0,
		D3D11_USAGE _eIdxUSage = D3D11_USAGE_DEFAULT, DXGI_FORMAT _eFormat = DXGI_FORMAT_UNKNOWN, void* _pIdx = nullptr);


	// �޽� �ε��ϱ�
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

	// �޽� ���� (���� �����ϴ°�)
	void Render();

	// �޽� ���� (����¸�ŭ �����ϴ°�)
	void Render(int _iContainer, int _iSubSet = 0);

	// �ν��Ͻ��� �ֵ� ����
	void RenderInstancing(unsigned int _iContainer, unsigned int _iSubSet,
		PInstancingBuffer _pInstancingBuffer,
		int _iInstancingCount);

private:
	// ���ؽ� ���� ����
	bool CreateVertexBuffer(int _iSize, int iCount, D3D11_USAGE _eUsage,
		D3D11_PRIMITIVE_TOPOLOGY _ePrimitive, void* pData);

	// �ε��� ���� ����
	bool CreateIndexBuffer(int _iSize, int iCount, D3D11_USAGE _eUsage,
		DXGI_FORMAT _eFormat, void* _pData);

	// ��� ���� ����
	bool CreateConstBuffer(UINT _Size, bool dynamic, bool _CPUUpdates, D3D11_SUBRESOURCE_DATA* _pData);

private:
	// ����Ʈ
	bool ConvertFbx(class FBXLoader* _pLoader,
		const char* _pFullPath);

public:
	// Mesh Save, Load
	bool Save(const char* _pFileName, const std::string& _strPathKey = PATH_MESH);
	bool SaveFromFullPath(const char* _pFullPath);

	bool Load(const char* _pFileName, const std::string& _strPathKey = PATH_MESH);
	bool LoadFromFullPath(const char* _pFullPath);

public:
	// �������� ����뵵
	class Material* CloneMaterial();
	
	// �ִϸ��̼� ����뵵
	class Animation* CloneAnimation();
};

ENGINE_END