#include "ResourcesManager.h"
#include "Mesh.h"
#include "Texture.h"
#include "Sampler.h"
#include "..\SerialNumber.h"

ENGINE_USING

DEFINITION_SINGLETON(ResourcesManager)

ResourcesManager::ResourcesManager()
{
	m_pMeshSerialNumber = new SerialNumber;
	m_pTextureSerialNumber = new SerialNumber;
}

ResourcesManager::~ResourcesManager()
{
	SAFE_DELETE(m_pTextureSerialNumber);
	SAFE_DELETE(m_pMeshSerialNumber);
	Safe_Release_Map(m_SamplerMap);
	Safe_Release_Map(m_mapTexture);
	Safe_Release_Map(m_mapMesh);
}

bool ResourcesManager::Init()
{
	// 버텍스, 인덱스 정보 만들기 (구)
	CreateSphereInfo(1.0f, 5);

	// 원뿔만들기
	CreateConeSide(0.5f, 1.0f, 10, 5);

#pragma region 삼각형 메쉬만들기

	// 삼각형 메쉬 만들기
	VertexColor tColorTriangle[3] =
	{
		VertexColor(Vector3(0.0f, 0.5f, 0.0f), Vector4::Green),
		VertexColor(Vector3(0.5f, -0.5f, 0.0f), Vector4::Red),
		VertexColor(Vector3(-0.5f, -0.5f, 0.0f), Vector4::Blue),
	};

	int iIndex[3] = { 0, 1, 2 };

	// 메쉬 생성
	CreateMesh("ColorTriangle", SHADER_STANDARD_COLOR, LAYOUT_POSITION_COLOR,
		Vector3::Axis[AXIS_Y],
		sizeof(VertexColor), 3, D3D11_USAGE_DEFAULT,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, tColorTriangle,
		4, 3, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT, iIndex);

#pragma endregion

#pragma region 사각형 메쉬 만들기
	VertexUV	tRectangle[4] =
	{
		VertexUV(Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f, 0.0f)),
		VertexUV(Vector3(1.0f, 1.0f, 0.0f), Vector2(1.0f, 0.0f)),
		VertexUV(Vector3(0.0f, 0.0f, 0.0f), Vector2(0.0f, 1.0f)),
		VertexUV(Vector3(1.0f, 0.0f, 0.0f), Vector2(1.0f, 1.0f)),
	};

	int iRectangleIndex[6] = { 0, 1, 3, 0, 3, 2 };

	CreateMesh("Rectangle", SHADER_DEBUG,
		LAYOUT_POSITION_UV, Vector3::Axis[AXIS_Y],
		sizeof(VertexUV), 4, D3D11_USAGE_DEFAULT,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, tRectangle,
		4, 6, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT, iRectangleIndex);

#pragma endregion


#pragma region 피라미드 메쉬 만들기 (사각뿔)

	// 정점 정보
	Vector3 vPyramidPosition[5] =
	{
		Vector3(0.0f, 0.5f, 0.0f),
		Vector3(-0.5f, -0.5f, 0.5f),
		Vector3(0.5f, -0.5f, 0.5f),
		Vector3(-0.5f, -0.5f, -0.5f),
		Vector3(0.5f, -0.5f, -0.5f)
	};

	Vector3 vPyramidFaceNormal[4];
	Vector3 vEdge[2];

	// 왼쪽 삼각형
	vEdge[0] = vPyramidPosition[3] - vPyramidPosition[0];
	vEdge[1] = vPyramidPosition[1] - vPyramidPosition[0];
	vEdge[0].Normalize();
	vEdge[1].Normalize();
	vPyramidFaceNormal[0] = vEdge[0].Cross(vEdge[1]);		//  ( 면법선계산)
	vPyramidFaceNormal[0].Normalize();

	// 뒤쪽 삼각형
	vEdge[0] = vPyramidPosition[1] - vPyramidPosition[0];
	vEdge[1] = vPyramidPosition[2] - vPyramidPosition[0];
	vEdge[0].Normalize();
	vEdge[1].Normalize();
	vPyramidFaceNormal[1] = vEdge[0].Cross(vEdge[1]);
	vPyramidFaceNormal[1].Normalize();

	// 오른쪽 삼각형
	vEdge[0] = vPyramidPosition[2] - vPyramidPosition[0];
	vEdge[1] = vPyramidPosition[4] - vPyramidPosition[0];
	vEdge[0].Normalize();
	vEdge[1].Normalize();
	vPyramidFaceNormal[2] = vEdge[0].Cross(vEdge[1]);
	vPyramidFaceNormal[2].Normalize();

	// 앞쪽 삼각형  ( s내가 해당 피라미드를 봤을때의 면)
	vEdge[0] = vPyramidPosition[4] - vPyramidPosition[0];
	vEdge[1] = vPyramidPosition[3] - vPyramidPosition[0];
	vEdge[0].Normalize();
	vEdge[1].Normalize();
	vPyramidFaceNormal[3] = vEdge[0].Cross(vEdge[1]);
	vPyramidFaceNormal[3].Normalize();

	// 피라미드 아랫면쪽면 (정점ㅇ..)
	Vector3 vPyramidNormal[4];

	vPyramidNormal[0] = vPyramidFaceNormal[0] + vPyramidFaceNormal[1];
	vPyramidNormal[0].Normalize();

	vPyramidNormal[1] = vPyramidFaceNormal[2] + vPyramidFaceNormal[1];
	vPyramidNormal[1].Normalize();

	vPyramidNormal[2] = vPyramidFaceNormal[0] + vPyramidFaceNormal[3];
	vPyramidNormal[2].Normalize();

	vPyramidNormal[3] = vPyramidFaceNormal[2] + vPyramidFaceNormal[3];
	vPyramidNormal[3].Normalize();

	// 전체 정점에따른 색상값
	VertexNormalColor	tPyramid[9] =
	{
		VertexNormalColor(vPyramidPosition[0], Vector3(0.f, 1.f, 0.f), Vector4::Green),
		VertexNormalColor(vPyramidPosition[1], vPyramidNormal[0], Vector4::Red),
		VertexNormalColor(vPyramidPosition[2], vPyramidNormal[1], Vector4::Blue),
		VertexNormalColor(vPyramidPosition[3], vPyramidNormal[2], Vector4::Yellow),
		VertexNormalColor(vPyramidPosition[4], vPyramidNormal[3], Vector4::Magenta),
		VertexNormalColor(vPyramidPosition[1], Vector3(0.f, -1.f, 0.f), Vector4::Red),
		VertexNormalColor(vPyramidPosition[2], Vector3(0.f, -1.f, 0.f), Vector4::Blue),
		VertexNormalColor(vPyramidPosition[3], Vector3(0.f, -1.f, 0.f), Vector4::Yellow),
		VertexNormalColor(vPyramidPosition[4], Vector3(0.f, -1.f, 0.f), Vector4::Magenta)
	};

	int iPyramidIndex[18] = { 0, 3, 1, 0, 1, 2, 0, 2, 4, 0, 4, 3, 7, 8, 6, 7, 6, 5 };

	CreateMesh("Pyramid", SHADER_STANDARD_NORMAL_COLOR,
		LAYOUT_POSITION_NORMAL_COLOR, Vector3::Axis[AXIS_Y],
		sizeof(VertexNormalColor), 9, D3D11_USAGE_DEFAULT,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, tPyramid,
		4, 18, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT, iPyramidIndex);

#pragma endregion


#pragma region 충돌체용 육면체 메쉬

	Vector3 vBoxCollider[8] = 
	{
		Vector3(-1.0f, 1.0f, -1.0f),
		Vector3(1.0f, 1.0f, -1.0f),
		Vector3(-1.0f, -1.0f, -1.0f),
		Vector3(1.0f, -1.0f, -1.0f),

		Vector3(-1.0f, 1.0f, 1.0f),
		Vector3(1.0f, 1.0f, 1.0f),
		Vector3(-1.0f, -1.0f, 1.0f),
		Vector3(1.0f, -1.0f, 1.0f)
	};

	int iBoxIndex[24] = 
	{
		0, 1, 0, 
		2, 1, 3, 
		2, 3, 4,
		0, 5, 1,
		6, 2, 7,
		3, 4, 5, 
		5, 7, 6, 
		7, 4, 6
	};

	CreateMesh("Collider_Box", SHADER_COLLIDER,
		LAYOUT_POSITION, Vector3::Axis[AXIS_Y],
		sizeof(Vector3), 8, D3D11_USAGE_DEFAULT,
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST, vBoxCollider,
		4, 24, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT, iBoxIndex);

#pragma endregion

#pragma region 파티클 전용       -> 기하셰이더를 이용해서 Point Rendering을 할 점 생성 ( 이 점이 기하셰이더를 거치면 사각형 메쉬로 됨)

	//VertexDefaultParticle _ParticleVertex = {};
	Vector3	ParticlePos = {};

	CreateMesh("Particle", SHADER_PARTICLE,
		LAYOUT_POSITION, Vector3::Axis[AXIS_Y],
		sizeof(Vector3), 1, D3D11_USAGE_DEFAULT,
		D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &ParticlePos);

#pragma endregion


	CreateSphereMesh("Sphere", SHADER_SKY, LAYOUT_POSITION);
	CreateConeMesh("Cone", SHADER_TEST_CYLINDER, LAYOUT_TEST_CYLINDER_POSITION);


	// 샘플러 생성
	CreateSampler(SAMPLER_LINEAR);
	CreateSampler(SAMPLER_POINT, D3D11_FILTER_MIN_MAG_MIP_POINT);
	CreateSampler(SAMPLER_WRAP, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP);
	CreateSampler(SAMPLER_CLAMP, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP);
	CreateSampler(SAMPLER_SHADOW, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_COMPARISON_LESS);

	// 샘플러 찾고 셰이더에 셋팅해주기 (미리해주는거임 예전에는 렌더할때마다 맞는거 설정해줬음)
	Sampler* pSampler = FindSampler(SAMPLER_LINEAR);
	pSampler->VSSetShader(0);
	pSampler->PSSetShader(0);
	SAFE_RELEASE(pSampler);

	pSampler = FindSampler(SAMPLER_POINT);
	pSampler->VSSetShader(1);
	pSampler->PSSetShader(1);
	SAFE_RELEASE(pSampler);

	pSampler = FindSampler(SAMPLER_WRAP);
	pSampler->VSSetShader(2);
	pSampler->PSSetShader(2);
	SAFE_RELEASE(pSampler);

	pSampler = FindSampler(SAMPLER_CLAMP);
	pSampler->VSSetShader(3);
	pSampler->PSSetShader(3);
	SAFE_RELEASE(pSampler);

	pSampler = FindSampler(SAMPLER_SHADOW);
	pSampler->VSSetShader(4);
	pSampler->PSSetShader(4);
	SAFE_RELEASE(pSampler);

	return true;
}

bool ResourcesManager::CreateMesh(const std::string& _strMeshName, const std::string& _strShaderKey, const std::string& _strInputLayoutKey,
	const Vector3& _vView,
	int iVtxSize, int iVtxCount,
	D3D11_USAGE _eVtxUsage, D3D11_PRIMITIVE_TOPOLOGY _ePrimitive,
	void* _pVtx, int _iIdxSize /*= 0*/, int _iIdxCount /*= 0*/,
	D3D11_USAGE _eIdxUSage/* = D3D11_USAGE_DEFAULT*/,
	DXGI_FORMAT _eFormat /*= DXGI_FORMAT_UNKNOWN*/,
	void* _pIdx /*= nullptr*/)
{
	Mesh* pMesh = FindMesh(_strMeshName);

	// 이미 있는 메쉬를 생성하려고한다면
	if (nullptr != pMesh)
	{
		SAFE_RELEASE(pMesh);
	}

	pMesh = new Mesh;

	if (false == pMesh->CreateMesh(_strMeshName, _strShaderKey, _strInputLayoutKey,iVtxSize, iVtxCount, _eVtxUsage, _ePrimitive,
		_pVtx, _iIdxSize, _iIdxCount, _eIdxUSage, _eFormat, _pIdx))
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	pMesh->SetView(_vView);
	pMesh->SetSerialNumber(m_pMeshSerialNumber->GetSerialNumber());

	m_mapMesh.insert(std::make_pair(_strMeshName, pMesh));

	return true;
}

bool ResourcesManager::CreateSphereMesh(const std::string & _strName, const std::string & _strShaderKey, const std::string & _strInputLayoutKey)
{
	Mesh* pMesh = FindMesh(_strName);

	if (nullptr != pMesh) 
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	pMesh = new Mesh;

	std::vector<Vector3> PositionVec;
	PositionVec.resize(m_SphereVertexVec.size());

	for (size_t i = 0; i < m_SphereVertexVec.size(); ++i)
	{
		PositionVec[i] = m_SphereVertexVec[i].vPosition;
	}

	if (false == pMesh->CreateMesh(_strName, _strShaderKey, _strInputLayoutKey,
		12, (int)(PositionVec.size()), D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		&PositionVec[0], 4, (int)(m_SphereIndexVec.size()), D3D11_USAGE_DEFAULT,
		DXGI_FORMAT_R32_UINT, &m_SphereIndexVec[0]))
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	pMesh->SetView(Vector3(0.0f, 0.0f, 1.0f));
	pMesh->SetSerialNumber(m_pMeshSerialNumber->GetSerialNumber());

	m_mapMesh.insert(std::make_pair(_strName, pMesh));

	return true;
}

// 원기둥
bool ResourcesManager::CreateCylinderMesh(const std::string & _strName, const std::string & _strShaderKey, const std::string & _strInputLayoutKey)
{
	Mesh* pMesh = FindMesh(_strName);

	if (nullptr != pMesh)
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	pMesh = new Mesh;

	std::vector<Vector3> PositionVec;
	PositionVec.resize(m_CylinderVertexVec.size());

	for (size_t i = 0; i < m_CylinderVertexVec.size(); ++i)
	{
		PositionVec[i] = m_CylinderVertexVec[i].vPosition;
	}

	if (false == pMesh->CreateMesh(_strName, _strShaderKey, _strInputLayoutKey,
		12, (int)(PositionVec.size()), D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		&PositionVec[0], 4, (int)(m_CylinderIndexVec.size()), D3D11_USAGE_DEFAULT,
		DXGI_FORMAT_R32_UINT, &m_CylinderIndexVec[0]))
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	pMesh->SetView(Vector3(0.0f, 0.0f, 1.0f));
	pMesh->SetSerialNumber(m_pMeshSerialNumber->GetSerialNumber());

	m_mapMesh.insert(std::make_pair(_strName, pMesh));

	return true;
}

// 원뿔
bool ResourcesManager::CreateConeMesh(const std::string & _strName, const std::string & _strShaderKey, const std::string & _strInputLayoutKey)
{
	Mesh* pMesh = FindMesh(_strName);

	if (nullptr != pMesh)
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	pMesh = new Mesh;

	std::vector<Vector3> PositionVec;
	PositionVec.resize(m_ConeVertexVec.size());

	for (size_t i = 0; i < m_ConeVertexVec.size(); ++i)
	{
		PositionVec[i] = m_ConeVertexVec[i].vPosition;
	}

	if (false == pMesh->CreateMesh(_strName, _strShaderKey, _strInputLayoutKey,
		12, (int)(PositionVec.size()), D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		&PositionVec[0], 4, (int)(m_ConeIndexVec.size()), D3D11_USAGE_DEFAULT,
		DXGI_FORMAT_R32_UINT, &m_ConeIndexVec[0]))
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	pMesh->SetView(Vector3(0.0f, 0.0f, 1.0f));
	pMesh->SetSerialNumber(m_pMeshSerialNumber->GetSerialNumber());

	m_mapMesh.insert(std::make_pair(_strName, pMesh));

	return true;
}

class Mesh* ResourcesManager::FindMesh(const std::string& _strMeshName)
{
	std::unordered_map<std::string, Mesh*>::iterator FindIter = m_mapMesh.find(_strMeshName);

	if (m_mapMesh.end() == FindIter)
	{
		return nullptr;
	}

	FindIter->second->AddReference();			// 참조를 했으니 참조횟수 추가

	return FindIter->second;
}

bool ResourcesManager::LoadMesh(const std::string & _strName, const TCHAR * _pFileName,
	const Vector3 & _vView,
	const std::string & _strPathName)
{
	Mesh*	pMesh = FindMesh(_strName);

	// 해당 메쉬가 이미 있다면 로드하지 않는다.
	if (nullptr != pMesh)
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	pMesh = new Mesh;

	// 메쉬를 로드한다.
	if (false == pMesh->LoadMesh(_strName, _pFileName, _vView, _strPathName))
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	//pMesh->SetView(_vView);
	pMesh->SetSerialNumber(m_pMeshSerialNumber->GetSerialNumber());
	m_mapMesh.insert(std::make_pair(_strName, pMesh));

	return true;
}

bool ResourcesManager::LoadMesh(const std::string & _strName, const char * _pFileName,
	const Vector3 & _vView,
	const std::string & _strPathName)
{
	Mesh*	pMesh = FindMesh(_strName);

	if (nullptr != pMesh)
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	pMesh = new Mesh;

	// 메쉬를 로드한다.
	if (false == pMesh->LoadMesh(_strName, _pFileName, _vView, _strPathName))
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	//pMesh->SetView(_vView);
	pMesh->SetSerialNumber(m_pMeshSerialNumber->GetSerialNumber());
	m_mapMesh.insert(std::make_pair(_strName, pMesh));

	return true;
}

bool ResourcesManager::LoadMeshFromFullPath(const std::string & _strName, 
	const TCHAR * _pFullPath,
	const Vector3 & _vView)
{
	Mesh*	pMesh = FindMesh(_strName);

	if (nullptr != pMesh)
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	pMesh = new Mesh;

	if (false == pMesh->LoadMeshFromFullPath(_strName, _pFullPath, _vView))
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	//pMesh->SetView(_vView);
	pMesh->SetSerialNumber(m_pMeshSerialNumber->GetSerialNumber());
	m_mapMesh.insert(std::make_pair(_strName, pMesh));

	return true;
}

bool ResourcesManager::LoadMeshFromFullPath(const std::string & _strName, 
	const char * _pFullPath, 
	const Vector3 & _vView)
{
	Mesh*	pMesh = FindMesh(_strName);

	if (nullptr != pMesh)
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	pMesh = new Mesh;

	if (false == pMesh->LoadMeshFromFullPath(_strName, _pFullPath, _vView))
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	//pMesh->SetView(_vView);

	m_mapMesh.insert(std::make_pair(_strName, pMesh));

	return true;
}

bool ResourcesManager::DeleteMesh(const std::string & _strName)
{
	std::unordered_map<std::string, Mesh*>::iterator FIter = m_mapMesh.find(_strName);

	if (FIter == m_mapMesh.end())
	{
		return false;
	}

	SAFE_RELEASE(FIter->second);

	m_pMeshSerialNumber->AddValidNumber(FIter->second->GetSerialNumber());
	m_mapMesh.erase(FIter);

	return true;
}



bool ResourcesManager::LoadTexture(const std::string & _strName, const TCHAR * _pFileName, const std::string & _strPathKey)
{
	Texture*	pTexture = FindTexture(_strName);

	// 해당 텍스처가 이미 있다면 로드할 필요가 없음
	if (nullptr != pTexture)
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	pTexture = new Texture;

	// 로드할 수 없다면
	if (false == pTexture->LoadTexture(_strName, _pFileName, _strPathKey))
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	m_mapTexture.insert(std::make_pair(_strName, pTexture));
	pTexture->SetSerialNumber(m_pTextureSerialNumber->GetSerialNumber());
	return true;
}

Texture * ResourcesManager::FindTexture(const std::string & _strName)
{
	std::unordered_map<std::string, Texture*>::iterator FIter = m_mapTexture.find(_strName);

	if (FIter == m_mapTexture.end())
	{
		return nullptr;
	}

	FIter->second->AddReference();

	return FIter->second;
}

bool ResourcesManager::LoadTextureFromFullPath(const std::string & _strName
	, const TCHAR * _pFullPath)
{
	Texture*	pTexture = FindTexture(_strName);

	// 해당 텍스처가 이미 있다면 로드할 필요가 없음
	if (nullptr != pTexture)
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	pTexture = new Texture;

	// 로드할 수 없다면
	if (false == pTexture->LoadTextureFromFullPath(_strName, _pFullPath))
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	pTexture->SetSerialNumber(m_pTextureSerialNumber->GetSerialNumber());
	m_mapTexture.insert(std::make_pair(_strName, pTexture));

	return true;
}

// Texture Array
bool ResourcesManager::LoadTexture(const std::string & _strName, const std::vector<TCHAR*>& _vecFileName, const std::string & _strPathKey)
{
	Texture*	pTexture = FindTexture(_strName);

	// 해당 텍스처가 이미 있다면 로드할 필요가 없음
	if (nullptr != pTexture)
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	pTexture = new Texture;

	// 로드할 수 없다면
	if (false == pTexture->LoadTexture(_strName, _vecFileName, _strPathKey))
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	pTexture->SetSerialNumber(m_pTextureSerialNumber->GetSerialNumber());
	m_mapTexture.insert(std::make_pair(_strName, pTexture));

	return true;
}

// Texture Array
bool ResourcesManager::LoadTextureFromFullPath(const std::string & _strName, const std::vector<TCHAR*>& _vecFullPath)
{

	Texture*	pTexture = FindTexture(_strName);

	// 해당 텍스처가 이미 있다면 로드할 필요가 없음
	if (nullptr != pTexture)
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	pTexture = new Texture;

	// 로드할 수 없다면
	if (false == pTexture->LoadTextureFromFullPath(_strName, _vecFullPath))
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	pTexture->SetSerialNumber(m_pTextureSerialNumber->GetSerialNumber());
	m_mapTexture.insert(std::make_pair(_strName, pTexture));

	return true;
}

bool ResourcesManager::DeleteTexture(const std::string & _strName)
{
	Texture* pTexture = FindTexture(_strName);

	if (nullptr == pTexture)
	{
		return false;
	}

	if (1 == pTexture->m_iReferenceCount)
	{
		m_pTextureSerialNumber->AddValidNumber(pTexture->GetSerialNumber());
		m_mapTexture.erase(_strName);
	}

	SAFE_RELEASE(pTexture);

	return true;
}

bool ResourcesManager::CreateSampler(const std::string & _strName, D3D11_FILTER _eFileter/* = D3D11_FILTER_MIN_MAG_MIP_LINEAR*/,
	D3D11_TEXTURE_ADDRESS_MODE _eAddressU /*= D3D11_TEXTURE_ADDRESS_WRAP*/,
	D3D11_TEXTURE_ADDRESS_MODE _eAddressV /*= D3D11_TEXTURE_ADDRESS_WRAP*/,
	D3D11_TEXTURE_ADDRESS_MODE _eAddressW /*= D3D11_TEXTURE_ADDRESS_WRAP*/,
	D3D11_COMPARISON_FUNC _ComparisonFunc /*= D3D11_COMPARISON_NEVER*/)
{
	Sampler* pSampler = FindSampler(_strName);

	// 이미 해당 샘플러가 있다면 만들 필요가없다.
	if (nullptr != pSampler)
	{
		SAFE_RELEASE(pSampler);
		return false;
	}

	pSampler = new Sampler;

	if (false == pSampler->CreateSamapler(_strName, _eFileter, _eAddressU, _eAddressV, _eAddressW, _ComparisonFunc))
	{
		SAFE_RELEASE(pSampler);
		return false;
	}

	m_SamplerMap.insert(std::make_pair(_strName, pSampler));

	return true;
}

Sampler * ResourcesManager::FindSampler(const std::string & _strName)
{
	std::unordered_map<std::string, Sampler*>::iterator FIter = m_SamplerMap.find(_strName);

	if (FIter == m_SamplerMap.end())
	{
		return nullptr;
	}

	FIter->second->AddReference();

	return FIter->second;
}

// 구 정점정보, 인덱스 정보를 만들어 주는 함수
bool ResourcesManager::CreateSphereInfo(float _fRadian, unsigned int _iSubDivision)
{
	// Put a cap on the number of subdivisions.
	_iSubDivision = min(_iSubDivision, 5u);

	// Approximate a sphere by tessellating an icosahedron.
	const float X = 0.525731f;
	const float Z = 0.850651f;

	Vector3 pos[12] =
	{
		Vector3(-X, 0.0f, Z),  Vector3(X, 0.0f, Z),
		Vector3(-X, 0.0f, -Z), Vector3(X, 0.0f, -Z),
		Vector3(0.0f, Z, X),   Vector3(0.0f, Z, -X),
		Vector3(0.0f, -Z, X),  Vector3(0.0f, -Z, -X),
		Vector3(Z, X, 0.0f),   Vector3(-Z, X, 0.0f),
		Vector3(Z, -X, 0.0f),  Vector3(-Z, -X, 0.0f)
	};

	DWORD k[60] =
	{
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
	};

	m_SphereVertexVec.resize(12);
	m_SphereIndexVec.resize(60);

	for (UINT i = 0; i < 12; ++i)
		m_SphereVertexVec[i].vPosition = pos[i];

	for (UINT i = 0; i < 60; ++i)
		m_SphereIndexVec[i] = k[i];

	for (UINT i = 0; i < _iSubDivision; ++i)
		SubDivide();

	// Project vertices onto sphere and scale.
	for (UINT i = 0; i < m_SphereVertexVec.size(); ++i)
	{
		// Project onto unit sphere.
		Vector3	vN = m_SphereVertexVec[i].vPosition;
		vN.Normalize();

		// Project onto sphere.
		Vector3 p = vN * _fRadian;

		m_SphereVertexVec[i].vPosition = p;
		// Normal이 있을 경우 따로 저장한다.
		m_SphereVertexVec[i].vNormal = vN;

		// Derive texture coordinates from spherical coordinates.
		float theta = AngleFromXY(
			m_SphereVertexVec[i].vPosition.x,
			m_SphereVertexVec[i].vPosition.z);

		float phi = acosf(m_SphereVertexVec[i].vPosition.y / _fRadian);

		m_SphereVertexVec[i].vUV.x = theta / XM_2PI;
		m_SphereVertexVec[i].vUV.y = phi / XM_PI;

		// Partial derivative of P with respect to theta
		m_SphereVertexVec[i].vTangent.x = -_fRadian*sinf(phi)*sinf(theta);
		m_SphereVertexVec[i].vTangent.y = 0.0f;
		m_SphereVertexVec[i].vTangent.z = +_fRadian*sinf(phi)*cosf(theta);

		m_SphereVertexVec[i].vBinormal = m_SphereVertexVec[i].vNormal.Cross(m_SphereVertexVec[i].vTangent);

		//XMVECTOR T = XMLoadFloat3(&vecVertices[i].TangentU);
		//XMStoreFloat3(&meshData.Vertices[i].TangentU, XMVector3Normalize(T));
	}


	return false;
}

// 구메쉬를 분할하는 함수
void ResourcesManager::SubDivide()
{
	// Save a copy of the input geometry.
	std::vector<Vertex3D>	vecCopyVertex = m_SphereVertexVec;
	std::vector<UINT>	vecCopyIndex = m_SphereIndexVec;


	m_SphereVertexVec.resize(0);
	m_SphereIndexVec.resize(0);

	//       v1
	//       *
	//      / \
				//     /   \
	//  m0*-----*m1
//   / \   / \
	//  /   \ /   \
	// *-----*-----*
// v0    m2     v2

	UINT numTris = (UINT)(vecCopyIndex.size()) / 3;
	for (UINT i = 0; i < numTris; ++i)
	{
		Vertex3D v0 = vecCopyVertex[vecCopyIndex[i * 3 + 0]];
		Vertex3D v1 = vecCopyVertex[vecCopyIndex[i * 3 + 1]];
		Vertex3D v2 = vecCopyVertex[vecCopyIndex[i * 3 + 2]];

		//
		// Generate the midpoints.
		//

		Vertex3D m0, m1, m2;

		// For subdivision, we just care about the position component.  We derive the other
		// vertex components in CreateGeosphere.

		m0.vPosition = Vector3(
			0.5f*(v0.vPosition.x + v1.vPosition.x),
			0.5f*(v0.vPosition.y + v1.vPosition.y),
			0.5f*(v0.vPosition.z + v1.vPosition.z));

		m1.vPosition = Vector3(
			0.5f*(v1.vPosition.x + v2.vPosition.x),
			0.5f*(v1.vPosition.y + v2.vPosition.y),
			0.5f*(v1.vPosition.z + v2.vPosition.z));

		m2.vPosition = Vector3(
			0.5f*(v0.vPosition.x + v2.vPosition.x),
			0.5f*(v0.vPosition.y + v2.vPosition.y),
			0.5f*(v0.vPosition.z + v2.vPosition.z));

		//
		// Add new geometry.
		//

		m_SphereVertexVec.push_back(v0); // 0
		m_SphereVertexVec.push_back(v1); // 1
		m_SphereVertexVec.push_back(v2); // 2
		m_SphereVertexVec.push_back(m0); // 3
		m_SphereVertexVec.push_back(m1); // 4
		m_SphereVertexVec.push_back(m2); // 5

		m_SphereIndexVec.push_back(i * 6 + 0);
		m_SphereIndexVec.push_back(i * 6 + 3);
		m_SphereIndexVec.push_back(i * 6 + 5);

		m_SphereIndexVec.push_back(i * 6 + 3);
		m_SphereIndexVec.push_back(i * 6 + 4);
		m_SphereIndexVec.push_back(i * 6 + 5);

		m_SphereIndexVec.push_back(i * 6 + 5);
		m_SphereIndexVec.push_back(i * 6 + 4);
		m_SphereIndexVec.push_back(i * 6 + 2);

		m_SphereIndexVec.push_back(i * 6 + 3);
		m_SphereIndexVec.push_back(i * 6 + 1);
		m_SphereIndexVec.push_back(i * 6 + 4);
	}
}

// 각도값, 탄젠트 이용
float ResourcesManager::AngleFromXY(float x, float y)
{
	float theta = 0.0f;

	// Quadrant I or IV
	if (x >= 0.0f)
	{
		// If x = 0, then atanf(y/x) = +pi/2 if y > 0
		//                atanf(y/x) = -pi/2 if y < 0
		theta = atanf(y / x); // in [-pi/2, +pi/2]

		if (theta < 0.0f)
			theta += 2.0f * ENGINE_PI; // in [0, 2*pi).
	}

	// Quadrant II or III
	else
		theta = atanf(y / x) + ENGINE_PI; // in [0, 2*pi).

	return theta;
}


#pragma region 원기둥만들기

// 원기둥 - 옆면
void ResourcesManager::CreateCylinderSide(float _BottomRadiuse, float _TopRadiuse,
	float _Height, UINT _SliceCount,
	UINT _StackCount)
{
	// 더미만들기
	float StackHeight = _Height / _StackCount;

	// 더미가 한단계씩 올라갈떄마다 반지름 변화량을 구한다ㅣ
	// 반지름 = (윗 반지름 - 아래반지름) / StackCount
	float RadiusStep = (_TopRadiuse - _BottomRadiuse) / _StackCount;

	// 총 고리개수 = 스택카운트 + 1
	UINT RingCount = _StackCount + 1;

	// 최하단 고리에서 최상단 고리까지의 각 고리의 정점들 계산하기
	for (UINT i = 0; i < RingCount; ++i)
	{
		float Y = -0.5f * _Height + i * StackHeight;
		float R = _BottomRadiuse + i * RadiusStep;

		// 현재 고리의 정점들
		float dTheta = 2.0f * XM_PI / _SliceCount;

		for (UINT j = 0; j <= _SliceCount; ++j)
		{
			Vertex3D vertex;

			float C = cosf(j * dTheta);
			float S = sinf(j * dTheta);

			vertex.vPosition = XMFLOAT3(R * C, Y, R * S);

			vertex.vUV.x = (float)j /_SliceCount;
			vertex.vUV.y = 1.0f - (float)i / _StackCount;

			vertex.vTangent = XMFLOAT3(-S, 0.0f, C);

			float DR = _BottomRadiuse - _TopRadiuse;		// 길이
			XMFLOAT3 bitangent(DR * C, -_Height, DR * S);

			XMFLOAT3 NewTangent = XMFLOAT3(vertex.vTangent.x, vertex.vTangent.y, vertex.vTangent.z);
			XMVECTOR T = XMLoadFloat3(&NewTangent);
			XMVECTOR B = XMLoadFloat3(&bitangent);
			XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));

			XMFLOAT3 ConvertN = XMFLOAT3(vertex.vNormal.x, vertex.vNormal.y, vertex.vNormal.z);
			XMStoreFloat3(&ConvertN, N);

			vertex.vNormal.x = ConvertN.x;
			vertex.vNormal.y = ConvertN.y;
			vertex.vNormal.z = ConvertN.z;

			m_CylinderVertexVec.push_back(vertex);
		}
	}

	// 인덱스계산
	// 원기둥에 텍스처를 제대로 입히려면 첫정점과 마지막 정점이 중복되어야 한다.
	// 이걸 위해서 정점 개수를 조각 개수 보다 하나 더 많게 한다.
	UINT RingVertexCount = _SliceCount + 1;

	// 각 더미마다 인덱스를 계산한다.
	for (UINT i = 0; i < _StackCount; ++i)
	{
		for (UINT j = 0; j < _SliceCount; ++j)
		{
			m_CylinderIndexVec.push_back(i * RingVertexCount + j);
			m_CylinderIndexVec.push_back((i + 1) * RingVertexCount + j);
			m_CylinderIndexVec.push_back((i + 1) * RingVertexCount + j + 1);

			m_CylinderIndexVec.push_back(i * RingVertexCount + j);
			m_CylinderIndexVec.push_back((i + 1) * RingVertexCount + j + 1);
			m_CylinderIndexVec.push_back(i * RingVertexCount + j + 1);
		}
	}

	// 윗면, 밑면 마개 기하 구조는 원을 최상단, 최하단 고리의 정점들로 근사한것임
	CreateCylinderTopCap(_BottomRadiuse, _TopRadiuse, _Height, _SliceCount, _StackCount);
	CreateCylinderBottomCap(_BottomRadiuse, _TopRadiuse, _Height, _SliceCount, _StackCount);
}

void ResourcesManager::CreateCylinderTopCap(float _BottomRaidus, float _TopRadiuse, float _Height, UINT _SliceCount, UINT _StackCount)
{
	UINT BaseIndex = (UINT)m_CylinderVertexVec.size();

	float y = 0.5f * _Height;
	float dTheta = 2.0f * XM_PI / _SliceCount;

	// 고리 정점들을 복제해서 마개 정점들을 만든다.
	// 텍스처 좌표와 법선이 달라서 이렇게 해야함 ㅇㅅㅇ 9for문
	for (UINT i = 0; i <= _SliceCount; ++i)
	{
		float x = _TopRadiuse * cosf(i * dTheta);
		float z = _TopRadiuse * sinf(i * dTheta);

		// 윗면 마개의 텍스처 좌표 면적이 밑면에 비례하도록
		// 텍스처 좌표를 높이에 따라 적절히 축소한다.
		float u = x / _Height + 0.5f;
		float v = z / _Height + 0.5f;

		Vertex3D pushData = {};
		pushData.vPosition = Vector3(x, y, z);
		pushData.vNormal = Vector3(0.0f, 1.0f, 0.0f);
		pushData.vTangent = Vector3(1.0f, 0.0f, 0.0f);
		pushData.vUV = Vector2(u, v);

		m_CylinderVertexVec.push_back(pushData);
	}

	// 마개의 중심 정점
	Vertex3D pushCenterVtxData = {};

	pushCenterVtxData.vPosition = Vector3(0.0f, y, 0.0f);
	pushCenterVtxData.vNormal = Vector3(0.0f, 1.0f, 0.0f);
	pushCenterVtxData.vTangent = Vector3(1.0f, 0.0f, 0.0f);
	pushCenterVtxData.vUV = Vector2(0.5f, 0.5f);

	m_CylinderVertexVec.push_back(pushCenterVtxData);

	// 중심 정점의 Index
	UINT CenterInderx = (UINT)m_CylinderVertexVec.size() - 1;

	for (UINT i = 0; i < _SliceCount; ++i)
	{
		m_CylinderIndexVec.push_back(CenterInderx);
		m_CylinderIndexVec.push_back(BaseIndex + i + 1);
		m_CylinderIndexVec.push_back(BaseIndex + i);
	}
}

void ResourcesManager::CreateCylinderBottomCap(float _BottomRaidus, float _TopRadiuse, float _Height, UINT _SliceCount, UINT _StackCount)
{
	UINT BaseIndex = (UINT)m_CylinderVertexVec.size();

	float y = 0.5f * -_Height;
	float dTheta = 2.0f * XM_PI / _SliceCount;

	// 고리 정점들을 복제해서 마개 정점들을 만든다.
	// 텍스처 좌표와 법선이 달라서 이렇게 해야함 ㅇㅅㅇ 9for문
	for (UINT i = 0; i <= _SliceCount; ++i)
	{
		float x = _BottomRaidus * cosf(i * dTheta);
		float z = _BottomRaidus * sinf(i * dTheta);

		// 윗면 마개의 텍스처 좌표 면적이 밑면에 비례하도록
		// 텍스처 좌표를 높이에 따라 적절히 축소한다.
		float u = x / _Height + 0.5f;
		float v = z / _Height + 0.5f;

		Vertex3D pushData = {};
		pushData.vPosition = Vector3(x, y, z);
		pushData.vNormal = Vector3(0.0f, 1.0f, 0.0f);
		pushData.vTangent = Vector3(1.0f, 0.0f, 0.0f);
		pushData.vUV = Vector2(0.5f, 0.5f);

		m_CylinderVertexVec.push_back(pushData);
	}

	// 마개의 중심 정점
	Vertex3D pushCenterVtxData = {};

	pushCenterVtxData.vPosition = Vector3(0.0f, y, 0.0f);
	pushCenterVtxData.vNormal = Vector3(0.0f, 1.0f, 0.0f);
	pushCenterVtxData.vTangent = Vector3(1.0f, 0.0f, 0.0f);
	pushCenterVtxData.vUV = Vector2(0.5f, 0.5f);

	m_CylinderVertexVec.push_back(pushCenterVtxData);

	// 중심 정점의 Index
	UINT CenterInderx = (UINT)m_CylinderVertexVec.size() - 1;

	for (UINT i = 0; i < _SliceCount; ++i)
	{
		m_CylinderIndexVec.push_back(CenterInderx);
		m_CylinderIndexVec.push_back(BaseIndex + i + 1);
		m_CylinderIndexVec.push_back(BaseIndex + i);
	}
}

#pragma endregion


#pragma region 원뿔 
// 원뿔 옆면
void ResourcesManager::CreateConeSide(float _BottomRadiuse,
	float _Height, UINT _SliceCount,
	UINT _StackCount)
{
	// 더미만들기
	float StackHeight = _Height / _StackCount;

	// 더미가 한단계씩 올라갈떄마다 반지름 변화량을 구한다ㅣ
	// 반지름 = (윗 반지름 - 아래반지름) / StackCount
	float RadiusStep = -_BottomRadiuse / _StackCount;

	// 총 고리개수 = 스택카운트 + 1
	UINT RingCount = _StackCount + 1;

	// 최하단 고리에서 최상단 고리까지의 각 고리의 정점들 계산하기
	for (UINT i = 0; i < RingCount; ++i)
	{
		float Y = -0.5f * _Height + i * StackHeight;
		float R = _BottomRadiuse + i * RadiusStep;

		// 현재 고리의 정점들
		float dTheta = 2.0f * XM_PI / _SliceCount;

		for (UINT j = 0; j <= _SliceCount; ++j)
		{
			Vertex3D vertex;

			float C = cosf(j * dTheta);
			float S = sinf(j * dTheta);

			vertex.vPosition = XMFLOAT3(R * C, Y, R * S);

			vertex.vUV.x = (float)j / _SliceCount;
			vertex.vUV.y = 1.0f - (float)i / _StackCount;

			vertex.vTangent = XMFLOAT3(-S, 0.0f, C);

			float DR = _BottomRadiuse /*- _TopRadiuse*/;		// 길이
			XMFLOAT3 bitangent(DR * C, -_Height, DR * S);

			XMFLOAT3 NewTangent = XMFLOAT3(vertex.vTangent.x, vertex.vTangent.y, vertex.vTangent.z);
			XMVECTOR T = XMLoadFloat3(&NewTangent);
			XMVECTOR B = XMLoadFloat3(&bitangent);
			XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));

			XMFLOAT3 ConvertN = XMFLOAT3(vertex.vNormal.x, vertex.vNormal.y, vertex.vNormal.z);
			XMStoreFloat3(&ConvertN, N);

			vertex.vNormal.x = ConvertN.x;
			vertex.vNormal.y = ConvertN.y;
			vertex.vNormal.z = ConvertN.z;

			m_ConeVertexVec.push_back(vertex);
		}
	}

	// 인덱스계산
	// 원기둥에 텍스처를 제대로 입히려면 첫정점과 마지막 정점이 중복되어야 한다.
	// 이걸 위해서 정점 개수를 조각 개수 보다 하나 더 많게 한다.
	UINT RingVertexCount = _SliceCount + 1;

	// 각 더미마다 인덱스를 계산한다.
	for (UINT i = 0; i < _StackCount; ++i)
	{
		for (UINT j = 0; j < _SliceCount; ++j)
		{
			m_ConeIndexVec.push_back(i * RingVertexCount + j);
			m_ConeIndexVec.push_back((i + 1) * RingVertexCount + j);
			m_ConeIndexVec.push_back((i + 1) * RingVertexCount + j + 1);

			m_ConeIndexVec.push_back(i * RingVertexCount + j);
			m_ConeIndexVec.push_back((i + 1) * RingVertexCount + j + 1);
			m_ConeIndexVec.push_back(i * RingVertexCount + j + 1);
		}
	}

	// 윗면, 밑면 마개 기하 구조는 원을 최상단, 최하단 고리의 정점들로 근사한것임
	CreateConeBottomCap(_BottomRadiuse, _Height, _SliceCount, _StackCount);
}

// 원뿔 하단 마개
void ResourcesManager::CreateConeBottomCap(float _BottomRaidus, float _Height,
	UINT _SliceCount, UINT _StackCount)
{
	UINT BaseIndex = (UINT)m_ConeVertexVec.size();

	float y = 0.5f * -_Height;
	float dTheta = 2.0f * XM_PI / _SliceCount;

	// 고리 정점들을 복제해서 마개 정점들을 만든다.
	// 텍스처 좌표와 법선이 달라서 이렇게 해야함 ㅇㅅㅇ 9for문
	for (UINT i = 0; i <= _SliceCount; ++i)
	{
		float x = _BottomRaidus * cosf(i * dTheta);
		float z = _BottomRaidus * sinf(i * dTheta);

		// 윗면 마개의 텍스처 좌표 면적이 밑면에 비례하도록
		// 텍스처 좌표를 높이에 따라 적절히 축소한다.
		float u = x / _Height + 0.5f;
		float v = z / _Height + 0.5f;

		Vertex3D pushData = {};
		pushData.vPosition = Vector3(x, y, z);
		pushData.vNormal = Vector3(0.0f, 1.0f, 0.0f);
		pushData.vTangent = Vector3(1.0f, 0.0f, 0.0f);
		pushData.vUV = Vector2(0.5f, 0.5f);

		m_ConeVertexVec.push_back(pushData);
	}

	// 마개의 중심 정점
	Vertex3D pushCenterVtxData = {};

	pushCenterVtxData.vPosition = Vector3(0.0f, y, 0.0f);
	pushCenterVtxData.vNormal = Vector3(0.0f, 1.0f, 0.0f);
	pushCenterVtxData.vTangent = Vector3(1.0f, 0.0f, 0.0f);
	pushCenterVtxData.vUV = Vector2(0.5f, 0.5f);

	m_ConeVertexVec.push_back(pushCenterVtxData);

	// 중심 정점의 Index
	UINT CenterInderx = (UINT)m_ConeVertexVec.size() - 1;

	for (UINT i = 0; i < _SliceCount; ++i)
	{
		m_ConeIndexVec.push_back(CenterInderx);
		m_ConeIndexVec.push_back(BaseIndex + i + 1);
		m_ConeIndexVec.push_back(BaseIndex + i);
	}
}
#pragma endregion


