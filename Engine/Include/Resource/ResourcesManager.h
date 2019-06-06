#pragma once

#include "../Engine.h"

ENGINE_BEGIN

class ENGINE_DLL ResourcesManager
{
private:
	std::unordered_map<std::string, class Mesh*> m_mapMesh;
	std::unordered_map<std::string, class Texture*> m_mapTexture;
	std::unordered_map<std::string, class Sampler*> m_SamplerMap;

private:
	// �� ���� ������ ������ ���� ( ���ؽ�, �ε���)
	std::vector<Vertex3D>	m_SphereVertexVec;
	std::vector<UINT>		m_SphereIndexVec;

	// ����� ���� ����
	std::vector<Vertex3D>	m_CylinderVertexVec;
	std::vector<UINT>		m_CylinderIndexVec;

	// ���� ���� ����
	std::vector<Vertex3D>	m_ConeVertexVec;
	std::vector<UINT>		m_ConeIndexVec;

	// ����  = ����, ��ġ���� ����ִ� ����
	std::vector<VertexColor> m_ConePosColorVertexVec;
	std::vector<UINT>		m_ConePosColorIndexVec;

private:
	class SerialNumber*		m_pMeshSerialNumber;			// �޽� �ĺ���ȣ
	class SerialNumber*		m_pTextureSerialNumber;			// �ؽ�ó �ĺ� ��ȣ

public:
	bool Init();


#pragma region  �޽� ����

public:
	// �޽� ����
	bool CreateMesh(const std::string& _strMeshName, const std::string& _strShaderKey, const std::string& _strInputLayoutKey,
		const Vector3& _vView,
		int iVtxSize, int iVtxCount,
		D3D11_USAGE _eVtxUsage, D3D11_PRIMITIVE_TOPOLOGY _ePrimitive,
		void* _pVtx, int iIdxSize = 0, int iIdxCount = 0,
		D3D11_USAGE _eIdxUSage = D3D11_USAGE_DEFAULT,
		DXGI_FORMAT _eFormat = DXGI_FORMAT_UNKNOWN,
		void* _pIdx = nullptr);

	// �� �޽� ����
	bool CreateSphereMesh(const std::string& _strName, const std::string& _strShaderKey,
		const std::string& _strInputLayoutKey);

	// ����� �޽� ����
	bool CreateCylinderMesh(const std::string& _strName, const std::string& _strShaderKey,
		const std::string& _strInputLayoutKey);

	// ���� �޽� ����
	bool CreateConeMesh(const std::string& _strName, const std::string& _strShaderKey,
		const std::string& _strInputLayoutKey);

	class Mesh* FindMesh(const std::string& _strMeshName);

#pragma endregion

#pragma region �޽� �ε� �� ����
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
		const Vector3& vView = Vector3::Axis[AXIS_Z]);

	bool DeleteMesh(const std::string& _strName);
#pragma endregion
	
#pragma region �ؽ�ó ã��, �ε� (1��, array)

public:
	class Texture* FindTexture(const std::string& _strName);

	bool LoadTexture(const std::string& _strName, const TCHAR* _pFileName, const std::string& _strPathKey);
	bool LoadTextureFromFullPath(const std::string& _strName, const TCHAR* _pFullPath);

	bool LoadTexture(const std::string& _strName, const std::vector<TCHAR*>& _vecFileName, const std::string& _strPathKey);
	bool LoadTextureFromFullPath(const std::string& _strName, const std::vector<TCHAR*>& _vecFullPath);

	bool DeleteTexture(const std::string& _strName);
#pragma endregion

#pragma region ���÷� �ε�
public:
	bool CreateSampler(const std::string& _strName, D3D11_FILTER _eFileter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_MODE _eAddressU = D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_MODE _eAddressV = D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_MODE _eAddressW = D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_COMPARISON_FUNC _ComparisonFunc = D3D11_COMPARISON_NEVER);

		class Sampler* FindSampler(const std::string& _strName);
#pragma endregion

private:
	bool CreateSphereInfo(float _fRadian, unsigned int _iSubDivision);
	void SubDivide();
	float AngleFromXY(float x, float y);

private:
	//����� ����
	void CreateCylinderSide(float _BottomRadiuse, float _TopRadiuse,
		float _Height, UINT _SliceCount,
		UINT _StackCount);

	// ����� ���� - �ֻ��
	void CreateCylinderTopCap(float _BottomRaidus, float _TopRadiuse, float _Height,
		UINT _SliceCount, UINT _StackCount);

	// ����� ���� - ���ϴ�
	void CreateCylinderBottomCap(float _BottomRaidus, float _TopRadiuse, float _Height,
		UINT _SliceCount, UINT _StackCount);

private:
	// ���� ����
	void CreateConeSide(float _BottomRadiuse,
		float _Height, UINT _SliceCount,
		UINT _StackCount);

	// ���� �ϴ� ����
	void CreateConeBottomCap(float _BottomRaidus, float _Height,
		UINT _SliceCount, UINT _StackCount);

	// �̱��� �����
	DECLARE_SINGLETON(ResourcesManager)
};

ENGINE_END