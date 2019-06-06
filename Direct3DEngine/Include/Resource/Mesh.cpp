#include "Mesh.h"
#include "..\Device.h"
#include "..\PathManager.h"
#include "FBXLoader.h"
#include "..\Component\Material.h"
#include "..\Component\Animation.h"

ENGINE_USING

Mesh::Mesh() :
	m_pMaterial(nullptr),
	m_pAnimation(nullptr)
{
	IndexCount = 0;
	m_vView = Vector3::Axis[AXIS_Z];
}

Mesh::~Mesh()
{
	for (size_t MeshConCount = 0; MeshConCount < m_MeshContainerVec.size(); ++MeshConCount)
	{
		// 인덱스 버퍼들 지우기
		for (size_t Index = 0; Index < m_MeshContainerVec[MeshConCount]->vecIB.size(); ++Index)
		{
			SAFE_DELETE_ARRAY(m_MeshContainerVec[MeshConCount]->vecIB[Index]->pData);
			SAFE_RELEASE(m_MeshContainerVec[MeshConCount]->vecIB[Index]->pBuffer);
			SAFE_DELETE(m_MeshContainerVec[MeshConCount]->vecIB[Index]);
		}

		// 버텍스 지우기
		SAFE_DELETE_ARRAY(m_MeshContainerVec[MeshConCount]->tVB.pData);
		SAFE_RELEASE(m_MeshContainerVec[MeshConCount]->tVB.pBuffer);
		SAFE_DELETE(m_MeshContainerVec[MeshConCount]);
	}

	m_MeshContainerVec.clear();

	SAFE_RELEASE(m_pAnimation);
	SAFE_RELEASE(m_pMaterial);
}

// Get Set 함수

std::string Mesh::GetShaderKey() const
{
	return m_strShaderKey;
}

std::string Mesh::GetInputLayoutKey() const
{
	return m_strInputLayoutKey;
}

size_t Mesh::GetContainerCount() const
{
	return m_MeshContainerVec.size();
}

size_t Mesh::GetSubSetCount(int _iCotainerNumber /*= 0*/) const
{
	return m_MeshContainerVec[_iCotainerNumber]->vecIB.size();
}
Vector3 Mesh::GetView() const
{
	return m_vView;
}
Vector3 Mesh::GetCenter() const
{
	return m_vCenter;
}
Vector3 Mesh::GetMin() const
{
	return m_vMin;
}
Vector3 Mesh::GetMax() const
{
	return m_vMax;
}
float Mesh::GetRadiuse() const
{
	return m_fRadius;
}
UINT Mesh::GetIndexCount() const
{
	return 0;
}
void Mesh::SetView(const Vector3 & _vView)
{
	m_vView = _vView;
}
// 

// 메쉬 생성
bool Mesh::CreateMesh(const std::string& _strMeshName, const std::string& _strShaderKey, const std::string& _strInputLayoutKey, int _iVtxSize, int _iVtxCount,
	D3D11_USAGE _eVtxUsage, D3D11_PRIMITIVE_TOPOLOGY _ePrimitive,
	void* _pVtx, int _iIdxSize /*= 0*/, int _iIdxCount/* = 0*/,
	D3D11_USAGE _eIdxUSage/* = D3D11_USAGE_DEFAULT*/,
	DXGI_FORMAT _eFormat/* = DXGI_FORMAT_UNKNOWN*/,
	void* _pIdx /*= nullptr*/)
{
	m_strTag = _strMeshName;				// 찾기 편하려고 메쉬 이름 설정
	m_strShaderKey = _strShaderKey;
	m_strInputLayoutKey = _strInputLayoutKey;

	// 메쉬컨테이너 생성후 넣어주기
	PMeshContainer pNewMeshContainer = new MeshContainer;
	m_MeshContainerVec.push_back(pNewMeshContainer);

	// 버텍스 버퍼 생성
	if (false == CreateVertexBuffer(_iVtxSize, _iVtxCount, _eVtxUsage, _ePrimitive, _pVtx))
	{
		return false;	// 실패
	}

	// 인덱스 버퍼 생성  -> 인덱스가 있을시 만들어주기
	if (nullptr != _pIdx)
	{
		if (false == CreateIndexBuffer(_iIdxSize, _iIdxCount, _eIdxUSage, _eFormat, _pIdx))
		{
			return false;	// 실패
		}
	}

	return true;

}

// 메쉬 렌더 (전부 렌더)
void Mesh::Render()
{

	for (size_t i = 0; i < m_MeshContainerVec.size(); ++i)
	{
		PMeshContainer pCurMeshContainer = m_MeshContainerVec[i];

		UINT	iSize = pCurMeshContainer->tVB.iSize;
		UINT	iOffset = 0;

		_CONTEXT->IASetPrimitiveTopology(pCurMeshContainer->tVB.ePrimitive);
		_CONTEXT->IASetVertexBuffers(0, 1, &pCurMeshContainer->tVB.pBuffer,
			&iSize, &iOffset);

		if (pCurMeshContainer->vecIB.empty())
		{
			_CONTEXT->Draw(pCurMeshContainer->tVB.iCount, 0);
		}
		else
		{
			for (size_t j = 0; j < pCurMeshContainer->vecIB.size(); ++j)
			{
				PIndexBuffer	pSubSet = pCurMeshContainer->vecIB[j];

				_CONTEXT->IASetIndexBuffer(pSubSet->pBuffer, pSubSet->eFormat, 0);
				_CONTEXT->DrawIndexed(pSubSet->iCount, 0, 0);
			}
		}
	}
}

bool Mesh::LoadMesh(const std::string & _strName, const TCHAR * _pFileName, const Vector3 & _vView, const std::string & _strPathName)
{
	const TCHAR*		pPath = GET_SINGLETON(PathManager)->FindPath(_strPathName);

	TCHAR		strFullPath[MAX_PATH] = {};

	if (nullptr != pPath)
	{
		lstrcpy(strFullPath, pPath);
	}

	// lstrcat : 해당 문자열 끝에 다른 문자열을 결합하는 함수
	// strFullPath + _pFileName
	lstrcat(strFullPath, _pFileName);

	return LoadMeshFromFullPath(_strName, strFullPath);
}

bool Mesh::LoadMesh(const std::string & _strName, const char * _pFileName, const Vector3 & _vView, const std::string & _strPathName)
{
	const char*		pPath = GET_SINGLETON(PathManager)->FindPathMultibyte(_strPathName);

	char		strFullPath[MAX_PATH] = {};

	if (nullptr != pPath)
	{
		strcpy_s(strFullPath, pPath);
	}

	// lstrcat : 해당 문자열 끝에 다른 문자열을 결합하는 함수
	// strFullPath + _pFileName
	strcat_s(strFullPath, _pFileName);

	return LoadMeshFromFullPath(_strName, strFullPath);

	return true;
}

bool Mesh::LoadMeshFromFullPath(const std::string & _strName, const TCHAR * _pFullPath, const Vector3 & _vView)
{
	char strFullPath[MAX_PATH] = {};

	// WideCharToMultiByte : 유니코드 ->멀티바이트
	// -1은 문자열 전체를 지정
	WideCharToMultiByte(CP_UTF8, 0, _pFullPath, -1,
		strFullPath, lstrlen(_pFullPath), 0, 0);

	return LoadMeshFromFullPath(_strName, strFullPath);
}

bool Mesh::LoadMeshFromFullPath(const std::string & _strName, const char * _pFullPath, const Vector3 & _vView)
{
	SetTag(_strName);
	m_vView = _vView;

	char strExtention[_MAX_EXT] = {};		// 확장자 명

	// 문자열 자르기
	_splitpath_s(_pFullPath, nullptr, 0, nullptr, 0, nullptr, 0, strExtention, _MAX_EXT);

	// 문자열 대문자로 변환
	_strupr_s(strExtention);

	if (0 == strcmp(strExtention, ".FBX"))
	{
		FBXLoader Loader;

		Loader.LoadFbx(_pFullPath);

		return ConvertFbx(&Loader, _pFullPath);
	}

	return LoadFromFullPath(_pFullPath);
}

// 서브셋에 따른 렌더
void Mesh::Render(int _iContainer, int _iSubSet /*= 0*/)
{
	PMeshContainer pContainer = m_MeshContainerVec[_iContainer];

	UINT iSize = pContainer->tVB.iSize;
	UINT iOffset = 0;

	_CONTEXT->IASetPrimitiveTopology(pContainer->tVB.ePrimitive);
	_CONTEXT->IASetVertexBuffers(0, 1, &pContainer->tVB.pBuffer,
		&iSize, &iOffset);

	if (pContainer->vecIB.empty())
	{
		_CONTEXT->Draw(pContainer->tVB.iCount, 0);
	}
	else
	{
		PIndexBuffer	pSubSet = pContainer->vecIB[_iSubSet];

		_CONTEXT->IASetIndexBuffer(pSubSet->pBuffer, pSubSet->eFormat, 0);
		_CONTEXT->DrawIndexed(pSubSet->iCount, 0, 0);
	}
}

// 인스턴싱인 애들 렌더 ->인스턴싱인 애들은 버퍼를 2개 만든다.
void Mesh::RenderInstancing(unsigned int _iContainer, unsigned int _iSubSet,
	PInstancingBuffer _pInstancingBuffer, 
	int _iInstancingCount)
{
	PMeshContainer pContainer = m_MeshContainerVec[_iContainer];

	UINT iSize[2] = { (UINT)(pContainer->tVB.iSize), (UINT)(_pInstancingBuffer->iSize) };
	UINT iOffset[2] = { 0, 0 };

	ID3D11Buffer* pBuffer[2] = { pContainer->tVB.pBuffer, _pInstancingBuffer->pBuffer };

	_CONTEXT->IASetPrimitiveTopology(pContainer->tVB.ePrimitive);
	_CONTEXT->IASetVertexBuffers(0, 2, pBuffer,
		iSize, iOffset);

	if (pContainer->vecIB.empty())
	{
		_CONTEXT->DrawInstanced(pContainer->tVB.iCount, _iInstancingCount, 0, 0);
	}
	else
	{
		PIndexBuffer	pSubSet = pContainer->vecIB[_iSubSet];

		_CONTEXT->IASetIndexBuffer(pSubSet->pBuffer, pSubSet->eFormat, 0);
		_CONTEXT->DrawIndexedInstanced(pSubSet->iCount, _iInstancingCount, 0, 0, 0);
	}
}

// 버텍스 버퍼 생성
bool Mesh::CreateVertexBuffer(int _iSize, int _iCount, D3D11_USAGE _eUsage,
	D3D11_PRIMITIVE_TOPOLOGY _ePrimitive, void* _pData)
{
	PMeshContainer pFindMeshContainer = m_MeshContainerVec.back();

	D3D11_BUFFER_DESC	tDesc = {};						// 정점 버퍼를 서술하는 구조체

	tDesc.ByteWidth = _iSize * _iCount;					// 생성할 정점 버퍼의 크기(바이트 단위)
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;			// 버퍼의 플래그 설정
	tDesc.Usage = _eUsage;								// 버퍼가 쓰이는 방식

	switch (_eUsage)
	{
	case D3D11_USAGE_DEFAULT:
		break;
	case D3D11_USAGE_IMMUTABLE:
		break;
	case D3D11_USAGE_DYNAMIC:
		tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;			// 쓰기 접근
		break;
	case D3D11_USAGE_STAGING:
		tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;		// 읽기, 쓰기
		break;
	default:
		break;
	}

	pFindMeshContainer->tVB.iSize = _iSize;
	pFindMeshContainer->tVB.iCount = _iCount;
	pFindMeshContainer->tVB.eUsage = _eUsage;
	pFindMeshContainer->tVB.ePrimitive = _ePrimitive;
	pFindMeshContainer->tVB.pData = new char[_iSize * _iCount];


	memcpy(pFindMeshContainer->tVB.pData, _pData, _iSize * _iCount);

	D3D11_SUBRESOURCE_DATA tData = {};
	tData.pSysMem = pFindMeshContainer->tVB.pData;			// 정점 버퍼를 초기화할 자료를 담은 시스템 메모리 배열을 가리키는 포인터


	if (FAILED(_DEVICE->CreateBuffer(&tDesc, &tData, &pFindMeshContainer->tVB.pBuffer)))
	{
		return false;
	}

	char*	pVertices = (char*)_pData;
	Vector3	vPos;
	memcpy(&vPos, pVertices, sizeof(Vector3));

	m_vMin = vPos;
	m_vMax = vPos;

	for (int i = 1; i < _iCount; ++i)
	{
		memcpy(&vPos, pVertices + _iSize * i, sizeof(Vector3));
		if (m_vMin.x > vPos.x)
			m_vMin.x = vPos.x;

		if (m_vMin.y > vPos.y)
			m_vMin.y = vPos.y;

		if (m_vMin.z > vPos.z)
			m_vMin.z = vPos.z;

		if (m_vMax.x < vPos.x)
			m_vMax.x = vPos.x;

		if (m_vMax.y < vPos.y)
			m_vMax.y = vPos.y;

		if (m_vMax.z < vPos.z)
			m_vMax.z = vPos.z;
	}

	m_vCenter = (m_vMin + m_vMax) / 2.f;
	m_vLength = m_vMax - m_vMin;
	m_fRadius = m_vLength.Length() / 2.f;

	return true;
}

// 인덱스 버퍼 생성
bool Mesh::CreateIndexBuffer(int _iSize, int _iCount, D3D11_USAGE _eUsage,
	DXGI_FORMAT _eFormat, void* _pData)
{
	PMeshContainer	pFindContainer = m_MeshContainerVec.back();			// 생성하고 바로 버텍스, 인덱스를 생성하니까 뒤에꺼 가져온다
	PIndexBuffer	pIB = new IndexBuffer;

	pFindContainer->vecIB.push_back(pIB);
	IndexCount = _iCount;
	D3D11_BUFFER_DESC tDesc = {};

	tDesc.ByteWidth = _iSize * _iCount;
	tDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	tDesc.Usage = _eUsage;

	switch (_eUsage)
	{
	case D3D11_USAGE_DEFAULT:
		break;
	case D3D11_USAGE_IMMUTABLE:
		break;
	case D3D11_USAGE_DYNAMIC:
		tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		break;
	case D3D11_USAGE_STAGING:
		tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		break;
	default:
		break;
	}

	pIB->iSize = _iSize;
	pIB->iCount = _iCount;
	pIB->eUsage = _eUsage;
	pIB->eFormat = _eFormat;
	pIB->pData = new char[_iSize * _iCount];			// 1바이트씩 읽으려고

	memcpy(pIB->pData, _pData, _iSize * _iCount);

	D3D11_SUBRESOURCE_DATA tData = {};
	tData.pSysMem = pIB->pData;

	if (FAILED(_DEVICE->CreateBuffer(&tDesc, &tData, &pIB->pBuffer)))
	{
		return false;
	}

	return true;
}


bool Mesh::CreateConstBuffer(UINT _Size, bool _dynamic, bool _CPUUpdates, D3D11_SUBRESOURCE_DATA* _pData)
{
	D3D11_BUFFER_DESC	tDesc = {};
	tDesc.ByteWidth = _Size;
	tDesc.MiscFlags = 0;
	tDesc.StructureByteStride = 0;
	tDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	if (_dynamic && _CPUUpdates)
	{
		tDesc.Usage = D3D11_USAGE_DYNAMIC;
		tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if (_dynamic && !_CPUUpdates)
	{
		tDesc.Usage = D3D11_USAGE_DEFAULT;
		tDesc.CPUAccessFlags = 0;
	}
	else
	{
		tDesc.Usage = D3D11_USAGE_IMMUTABLE;
		tDesc.CPUAccessFlags = 0;
	}

	ID3D11Buffer* pBuffer = 0;

	if (FAILED(_DEVICE->CreateBuffer(&tDesc, _pData, &pBuffer)))
	{
		return false;
	}

	return true;
}

bool Mesh::ConvertFbx(FBXLoader * _pLoader, const char * _pFullPath)
{
	const std::vector<PFBXMESHCONTAINER>*	pvecContainer = _pLoader->GetMeshContainers();
	const std::vector<std::vector<PFBXMATERIAL>>*	pvecMaterials = _pLoader->GetMaterials();

	std::vector<PFBXMESHCONTAINER>::const_iterator	iter = pvecContainer->begin();
	std::vector<PFBXMESHCONTAINER>::const_iterator	iterEnd = pvecContainer->end();

	// vecEmptyIndex
	// 비어있는 인덱스, 서브셋이 없는데 있다고 되어있는 것들 모음 (디자이너가 만들어놓고 사용안함)
	std::vector<bool>	vecEmptyIndex;
	bool				bAnimation = false;

	for (; iter != iterEnd; ++iter)
	{
		PMeshContainer	pContainer = new MeshContainer;

		m_strInputLayoutKey = LAYOUT_VERTEX3D;

		m_MeshContainerVec.push_back(pContainer);

		int	iVtxSize = 0;

		// 범프가 있을 경우
		if ((*iter)->bBump)
		{
			if ((*iter)->bAnimation)
			{
				bAnimation = true;
				//m_strShaderKey = STANDARD_BUMP_ANIM_SHADER;
			}

			else
			{
				m_strShaderKey = SHADER_STANDARD_BUMP;
			}
		}

		// 범프가 없을 경우
		else
		{
			if ((*iter)->bAnimation)
			{
				bAnimation = true;
				//m_strShaderKey = STANDARD_TEX_NORMAL_ANIM_SHADER;
			}

			else
			{
				m_strShaderKey = SHADER_STANDARD_TEXTURE_NORMAL;
			}
		}

		m_strShaderKey = SHADER_STANDARD_3D;				// 기본텍스처들

		std::vector<Vertex3D>	vecVtx;
		iVtxSize = sizeof(Vertex3D);

		for (size_t i = 0; i < (*iter)->vecPosition.size(); ++i)
		{
			Vertex3D	tVtx = {};

			tVtx.vPosition = (*iter)->vecPosition[i];
			tVtx.vNormal = (*iter)->vecNormal[i];
			tVtx.vUV = (*iter)->vecUV[i];

			if (false == (*iter)->vecTangent.empty())
				tVtx.vTangent = (*iter)->vecTangent[i];

			if (false == (*iter)->vecBinormal.empty())
				tVtx.vBinormal = (*iter)->vecBinormal[i];

			if (false == (*iter)->vecBlendWeight.empty())
			{
				tVtx.vBlendWeights = (*iter)->vecBlendWeight[i];
				tVtx.vBlendIndices = (*iter)->vecBlendIndex[i];
			}

			vecVtx.push_back(tVtx);
		}

		pContainer->tVB.ePrimitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		if (!CreateVertexBuffer(iVtxSize, (int)(vecVtx.size()),
			D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			&vecVtx[0]))
		{
			return false;
		}

		// 인덱스버퍼 생성
		for (size_t i = 0; i < (*iter)->vecIndices.size(); ++i)
		{
			if ((*iter)->vecIndices[i].empty())
			{
				vecEmptyIndex.push_back(false);
				continue;
			}

			vecEmptyIndex.push_back(true);

			if (!CreateIndexBuffer(4, (int)((*iter)->vecIndices[i].size()),
				D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT,
				&(*iter)->vecIndices[i][0]))
				return false;
		}
	}

	// 재질 정보를 읽어온다.
	const std::vector<std::vector<PFBXMATERIAL>>*	pMaterials = _pLoader->GetMaterials();

	std::vector<std::vector<PFBXMATERIAL>>::const_iterator	iterM = pMaterials->begin();
	std::vector<std::vector<PFBXMATERIAL>>::const_iterator	iterMEnd = pMaterials->end();

	if (false == pMaterials->empty())
	{
		// 실제 사용할 재질 클래스를 생성한다.
		m_pMaterial = new Material;

		if (!m_pMaterial->Init())
		{
			SAFE_RELEASE(m_pMaterial);
			return NULL;
		}
	}

	int	iContainer = 0;
	for (; iterM != iterMEnd; ++iterM, ++iContainer)
	{
		for (size_t i = 0; i < (*iterM).size(); ++i)
		{
			// 인덱스 버퍼가 비어있을 경우에는 재질을 추가하지 않는다.
			if (false == vecEmptyIndex[i])
				continue;

			// 재질 정보를 얻어온다.
			PFBXMATERIAL	pMtrl = (*iterM)[i];

			m_pMaterial->SetColor(pMtrl->vDiffuse, pMtrl->vAmbient,
				pMtrl->vSpecular, pMtrl->vEmissive, pMtrl->fShininess, iContainer, (UINT)(i));

			// 애니메이션이 존재할때
			if (true == bAnimation)
			{
				// 스키닝 적용
				m_pMaterial->Skinning(iContainer, (int)i);
			}

			// 이름을 불러온다.
			char	strName[MAX_PATH] = {};
			_splitpath_s(pMtrl->strDifTex.c_str(), NULL, 0, NULL, 0,
				strName, MAX_PATH, NULL, 0);

			TCHAR	strPath[MAX_PATH] = {};

#ifdef UNICODE
			MultiByteToWideChar(CP_ACP, 0, pMtrl->strDifTex.c_str(),
				-1, strPath, (int)(pMtrl->strDifTex.length()));
#else
			strcpy_s(strPath, pMtrl->strDifTex.c_str());
#endif // UNICODE

			// 샘플러랑 텍스처 같이 셋팅한다.
			m_pMaterial->AddTextureSetFromFullPath(iContainer, (int)i,
				0, strName,
				strPath);


			// 범프가 있다면(노말)
			if (false == pMtrl->strBumpTex.empty())
			{
				memset(strName, 0, MAX_PATH);
				_splitpath_s(pMtrl->strBumpTex.c_str(), NULL, 0, NULL, 0,
				strName, MAX_PATH, NULL, 0);

				memset(strPath, 0, sizeof(wchar_t) * MAX_PATH);


#ifdef UNICODE
				MultiByteToWideChar(CP_ACP, 0, pMtrl->strBumpTex.c_str(),
					-1, strPath, (int)(pMtrl->strBumpTex.length()));
#else
				strcpy_s(strPath, pMtrl->strBumpTex.c_str());

#endif // UNICODE

				// 샘플러(X지금은 아님 셰이더에서 처리하기로함) 텍스처 같이 셋팅한다.
				m_pMaterial->AddTextureSetFromFullPath(iContainer, (int)i, 1, strName, strPath);
				m_pMaterial->BumpTextureEnable(iContainer, (int)i);
			}


			// sepcular 텍스처
			if (false == pMtrl->strSpcTex.empty())
			{
				memset(strName, 0, MAX_PATH);
				_splitpath_s(pMtrl->strSpcTex.c_str(), NULL, 0, NULL, 0,
				strName, MAX_PATH, NULL, 0);

				memset(strPath, 0, sizeof(wchar_t) * MAX_PATH);

#ifdef UNICODE
				MultiByteToWideChar(CP_ACP, 0, pMtrl->strSpcTex.c_str(),
					-1, strPath, (int)(pMtrl->strSpcTex.length()));
#else
				strcpy_s(strPath, pMtrl->strSpcTex.c_str());
#endif // UNICODE

				// 샘플러랑 텍스처 같이 셋팅한다.
				m_pMaterial->AddTextureSetFromFullPath(iContainer, (int)i,  2, strName, strPath);
				m_pMaterial->SepcularTextureEnable(iContainer, (int)i);
			}
		}
	}
#pragma region 예전
	// 텍스쳐가 저장된 폴더명을 키로 변경한다.
	//char	strFullName[MAX_PATH] = {};
	//iterM = pMaterials->begin();
	//strcpy_s(strFullName, (*iterM)[0]->strDifTex.c_str());

	//int	iLength = strlen(strFullName);
	//for (int i = iLength - 1; i >= 0; --i)
	//{
	//	if (strFullName[i] == '\\' || strFullName[i] == '/')
	//	{
	//		memset(strFullName + (i + 1), 0, sizeof(char) * (iLength - (i + 1)));
	//		strFullName[i] = '\\';
	//		//strFullName[i] = 0;
	//		break;
	//	}
	//}

	//char	strChange[MAX_PATH] = {};
	//strcpy_s(strChange, strFullName);
	//iLength = strlen(strChange);
	//for (int i = iLength - 2; i >= 0; --i)
	//{
	//	if (strChange[i] == '\\' || strChange[i] == '/')
	//	{
	//		memset(strChange + (i + 1), 0, sizeof(char) * (iLength - (i + 1)));
	//		break;
	//	}
	//}

	//strcat_s(strChange, m_strTag.c_str());
	//strcat_s(strChange, "\\");

	//MoveFileA(strFullName, strChange);

	// Mesh\\ 까지의 경로를 제거한다.
	/*iLength = strlen(strChange);
	for (int i = iLength - 2; i >= 0; --i)
	{
	char	cText[5] = {};
	memcpy(cText, &strChange[i - 4], 4);
	_strupr_s(cText);

	if (strcmp(cText, "MESH") == 0)
	{
	memset(strChange, 0, sizeof(char) * (i + 1));
	memcpy(strChange, &strChange[i + 1], sizeof(char) * (iLength - (i + 1)));
	memset(strChange + (i + 1), 0, sizeof(char) * (iLength - (i + 1)));
	break;
	}
	}*/

	/*for (size_t i = 0; i < m_vecMeshContainer.size(); ++i)
	{
	PMESHCONTAINER	pContainer = m_vecMeshContainer[i];

	for (size_t j = 0; j < pContainer->vecMaterial.size(); ++j)
	{
	pContainer->vecMaterial[j]->SetTexturePathKey(MESH_PATH);
	pContainer->vecMaterial[j]->ChangeTexturePath(strChange);
	}
	}*/
#pragma endregion


	m_vLength = m_vMax - m_vMin;

	m_vCenter = (m_vMax + m_vMin) / 2.f;
	m_fRadius = m_vLength.Length() / 2.f;

	char	strFullPath[MAX_PATH] = {};
	strcpy_s(strFullPath, _pFullPath);
	int	iPathLength = (int)(strlen(strFullPath));
	memcpy(&strFullPath[iPathLength - 3], "msh", 3);

	SaveFromFullPath(strFullPath);;

	// 애니메이션 처리
	const std::vector<PFBXBONE>*	pvecBone = _pLoader->GetBones();

	if (pvecBone->empty())
		return true;

	if (nullptr != m_pAnimation)
	{
		SAFE_RELEASE(m_pAnimation);
	}

	m_pAnimation = new Animation;

	if (!m_pAnimation->Init())
	{
		SAFE_RELEASE(m_pAnimation);
		return false;
	}

	//// 본 수만큼 반복한다.
	std::vector<PFBXBONE>::const_iterator	iterB;
	std::vector<PFBXBONE>::const_iterator	iterBEnd = pvecBone->end();

	for (iterB = pvecBone->begin(); iterB != iterBEnd; ++iterB)
	{
		PBONE	pBone = new BONE;

		pBone->strName = (*iterB)->strName;
		pBone->iDepth = (*iterB)->iDepth;
		pBone->iParentIndex = (*iterB)->iParentIndex;

		float	fMat[4][4];

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				fMat[i][j] = (float)((*iterB)->matOffset.mData[i].mData[j]);
			}
		}

		pBone->matOffset = new Matrix;
		*pBone->matOffset = fMat;

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				fMat[i][j] = (float)((*iterB)->matBone.mData[i].mData[j]);
			}
		}

		pBone->matBone = new Matrix;
		*pBone->matBone = fMat;

		m_pAnimation->AddBone(pBone);
	}

	m_pAnimation->CreateBoneTexture();

	// 애니메이션 클립을 추가한다.
	const std::vector<PFBXANIMATIONCLIP>* pvecClip = _pLoader->GetClips();

	// 클립을 읽어온다.
	std::vector<PFBXANIMATIONCLIP>::const_iterator	iterC = pvecClip->begin();
	std::vector<PFBXANIMATIONCLIP>::const_iterator	iterCEnd = pvecClip->end();

	for (; iterC != iterCEnd; ++iterC)
	{
		m_pAnimation->AddClip(AO_LOOP, *iterC);
	}

	// 애니메이션이 있는 경우 본정보와 애니메이션 정보를 가져온다.
	if (m_pAnimation)
	{
		memcpy(&strFullPath[iPathLength - 3], "bne", 3);
		m_pAnimation->SaveBoneFromFullPath(strFullPath);

		memcpy(&strFullPath[iPathLength - 3], "anm", 3);
		m_pAnimation->SaveFromFullPath(strFullPath);
	}

	return true;
}

bool Mesh::Save(const char * _pFileName, const std::string & _strPathKey)
{
	char	strFullPath[MAX_PATH] = {};
	const char* pPath = GET_SINGLETON(PathManager)->FindPathMultibyte(_strPathKey);

	if (nullptr != pPath)
	{
		strcpy_s(strFullPath, pPath);
	}

	strcat_s(strFullPath, _pFileName);

	return SaveFromFullPath(strFullPath);
}

bool Mesh::SaveFromFullPath(const char * _pFullPath)
{
	// 자체포맷
	FILE*	pFile = nullptr;

	// write
	fopen_s(&pFile, _pFullPath, "wb");

	if (nullptr == pFile)
	{
		return false;
	}

	int iLength = (int)(m_strTag.length());

	// Tag 길이를 저장한다.
	fwrite(&iLength, sizeof(int), 1, pFile);
	fwrite(m_strTag.c_str(), 1, iLength, pFile);

	// SahderName 길이를 저장한다.
	iLength = (int)(m_strShaderKey.length());
	fwrite(&iLength, sizeof(int), 1, pFile);
	fwrite(m_strShaderKey.c_str(), 1, iLength, pFile);

	// 입력레이아웃 이름 길이를 저장한다.
	iLength = (int)(m_strInputLayoutKey.length());
	fwrite(&iLength, sizeof(int), 1, pFile);
	fwrite(m_strInputLayoutKey.c_str(), 1, iLength, pFile);

	// 나머지 저장
	fwrite(&m_vCenter, sizeof(Vector3), 1, pFile);
	fwrite(&m_fRadius, sizeof(float), 1, pFile);
	fwrite(&m_vMin, sizeof(Vector3), 1, pFile);
	fwrite(&m_vMax, sizeof(Vector3), 1, pFile);
	fwrite(&m_vLength, sizeof(Vector3), 1, pFile);

	// 메쉬정보들 저장하기 ( 컨테이너 -> 서브셋의 버텍스버퍼정보와 인덱스정보들 )
	size_t iContainer = m_MeshContainerVec.size();		// 메쉬 컨테이너 수
	fwrite(&iContainer, sizeof(size_t), 1, pFile);

	for (size_t i = 0; i < iContainer; ++i)
	{
		PMeshContainer pContainer = m_MeshContainerVec[i];

		// 버텍스
		fwrite(&pContainer->tVB.ePrimitive, sizeof(D3D11_PRIMITIVE_TOPOLOGY), 1, pFile);		// 도형 기본구조 저장
		
		fwrite(&pContainer->tVB.iSize, sizeof(int), 1, pFile);									// 버텍스 하나 사이즈
		fwrite(&pContainer->tVB.iCount, sizeof(int), 1, pFile);									// 버텍스 개수
		fwrite(&pContainer->tVB.eUsage, sizeof(D3D11_USAGE), 1, pFile);							// 용도
		fwrite(pContainer->tVB.pData, pContainer->tVB.iSize, pContainer->tVB.iCount, pFile);	// 데이터 저장

		// 인덱스
		size_t iIdxCount = pContainer->vecIB.size();
		fwrite(&iIdxCount, sizeof(size_t), 1, pFile);

		for (size_t j = 0; j < iIdxCount; ++j)
		{
			fwrite(&pContainer->vecIB[j]->eFormat, sizeof(DXGI_FORMAT), 1, pFile);				// 인덱스 포맷 
			fwrite(&pContainer->vecIB[j]->iSize, sizeof(int), 1, pFile);						// 인덱스 사이즈
			fwrite(&pContainer->vecIB[j]->iCount, sizeof(int), 1, pFile);						// 인덱스 개수
			fwrite(&pContainer->vecIB[j]->eUsage, sizeof(D3D11_USAGE), 1, pFile);				// 용도
			fwrite(pContainer->vecIB[j]->pData, pContainer->vecIB[j]->iSize, pContainer->vecIB[j]->iCount, pFile);	// 데이터 저장
		}
	}
	
	bool	bMaterial = false;

	if (nullptr != m_pMaterial)
	{
		// 재질 정보가 있는 경우, 재질정보도 저장
		bMaterial = true;
		fwrite(&bMaterial, sizeof(bool), 1, pFile);

		m_pMaterial->Save(pFile);
	}
	else
	{
		// 재질 정보가 없는 경우
		fwrite(&bMaterial, sizeof(bool), 1, pFile);
	}

	fclose(pFile);
	return true;
}

bool Mesh::Load(const char * _pFileName, const std::string & _strPathKey)
{
	char	strFullPath[MAX_PATH] = {};
	const char* pPath = GET_SINGLETON(PathManager)->FindPathMultibyte(_strPathKey);

	if (nullptr != pPath)
	{
		strcpy_s(strFullPath, pPath);
	}

	strcat_s(strFullPath, _pFileName);

	return LoadFromFullPath(strFullPath);
}

bool Mesh::LoadFromFullPath(const char * _pFullPath)
{
	FILE*	pFile = nullptr;

	fopen_s(&pFile, _pFullPath, "rb");

	if (nullptr == pFile)
	{
		return false;
	}

	int iLength = 0;

	// Tag 길이를 읽어서 저장한다.
	fread(&iLength, sizeof(int), 1, pFile);
	char	strTag[256] = {};
	fread(strTag, 1, iLength, pFile);
	m_strTag = strTag;

	// ShaderName 길이를 저장한다. ( + 셰이더키이름도!)
	iLength = 0;
	fread(&iLength, sizeof(int), 1, pFile);
	char	strShaderKey[256] = {};
	fread(strShaderKey, 1, iLength, pFile);
	m_strShaderKey = strShaderKey;

	// 입력 레이아웃 이름 길이를 저장한다 (+Key값도)
	iLength = 0;
	fread(&iLength, sizeof(int), 1, pFile);
	char	strLayoutKey[256] = {};
	fread(strLayoutKey, 1, iLength, pFile);
	m_strInputLayoutKey = strLayoutKey;

	// 나머지 값들 읽어오기 + 저장
	fread(&m_vCenter, sizeof(Vector3), 1, pFile);
	fread(&m_fRadius, sizeof(float), 1, pFile);
	fread(&m_vMin, sizeof(Vector3), 1, pFile);
	fread(&m_vMax, sizeof(Vector3), 1, pFile);
	fread(&m_vLength, sizeof(Vector3), 1, pFile);

	// 메쉬 컨테이너 읽어오기
	size_t	iContainer = 0;

	fread(&iContainer, sizeof(size_t), 1, pFile);

	for (size_t i = 0; i < iContainer; ++i)
	{
		// 메쉬 컨테이너 읽어오기
		PMeshContainer	pContainer = new MeshContainer;
		m_MeshContainerVec.push_back(pContainer);

		int iVtxSize = 0;
		int iVtxCount = 0;
		D3D11_USAGE eUsage;

		fread(&pContainer->tVB.ePrimitive, sizeof(D3D11_PRIMITIVE_TOPOLOGY), 1, pFile);
		fread(&iVtxSize, sizeof(int), 1, pFile);
		fread(&iVtxCount, sizeof(int), 1, pFile);
		fread(&eUsage, sizeof(D3D11_USAGE), 1, pFile);

		// 버텍스 데이터 읽어오기 + 생성하기
		char* pData = new char[iVtxSize * iVtxCount];
		fread(pData, iVtxSize, iVtxCount, pFile);

		CreateVertexBuffer(iVtxSize, iVtxCount, eUsage, 
			pContainer->tVB.ePrimitive, pData);

		SAFE_DELETE_ARRAY(pData);

		// 인덱스 데이터 읽어오기 + 생성하기
		size_t	iIdxCount = 0;
		fread(&iIdxCount, sizeof(size_t), 1, pFile);

		for (size_t j = 0; j < iIdxCount; ++j)
		{
			DXGI_FORMAT eFormat;
			int iIdxSize = 0;
			int iIdxCount = 0;

			fread(&eFormat, sizeof(DXGI_FORMAT), 1, pFile);
			fread(&iIdxSize, sizeof(int), 1, pFile);
			fread(&iIdxCount, sizeof(int), 1, pFile);
			fread(&eUsage, sizeof(D3D11_USAGE), 1, pFile);

			pData = new char[iIdxSize * iIdxCount];
			fread(pData, iIdxSize, iIdxCount, pFile);

			CreateIndexBuffer(iIdxSize, iIdxCount, eUsage, eFormat, pData);

			SAFE_DELETE_ARRAY(pData);
		}
	}

	// 재질의 유무파악하기 위한 변수
	bool bMaterial = false; 
	fread(&bMaterial, sizeof(bool), 1, pFile);

	if (true == bMaterial)
	{
		// 재질이 있는 경우
		SAFE_RELEASE(m_pMaterial);
		bMaterial = true;

		m_pMaterial = new Material;

		m_pMaterial->Init();
		m_pMaterial->Load(pFile);
	}

	// 파일을 열었다면 꼭 닫아야한다.
	fclose(pFile);

	return true;
}

Material * Mesh::CloneMaterial()
{
	if (nullptr == m_pMaterial)
	{
		return nullptr;
	}

	return m_pMaterial->Clone();
}

Animation * Mesh::CloneAnimation()
{
	if (nullptr == m_pAnimation)
	{
		return nullptr;
	}

	return m_pAnimation->Clone();
}
