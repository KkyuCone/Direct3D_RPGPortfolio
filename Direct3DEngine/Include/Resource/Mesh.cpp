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
		// �ε��� ���۵� �����
		for (size_t Index = 0; Index < m_MeshContainerVec[MeshConCount]->vecIB.size(); ++Index)
		{
			SAFE_DELETE_ARRAY(m_MeshContainerVec[MeshConCount]->vecIB[Index]->pData);
			SAFE_RELEASE(m_MeshContainerVec[MeshConCount]->vecIB[Index]->pBuffer);
			SAFE_DELETE(m_MeshContainerVec[MeshConCount]->vecIB[Index]);
		}

		// ���ؽ� �����
		SAFE_DELETE_ARRAY(m_MeshContainerVec[MeshConCount]->tVB.pData);
		SAFE_RELEASE(m_MeshContainerVec[MeshConCount]->tVB.pBuffer);
		SAFE_DELETE(m_MeshContainerVec[MeshConCount]);
	}

	m_MeshContainerVec.clear();

	SAFE_RELEASE(m_pAnimation);
	SAFE_RELEASE(m_pMaterial);
}

// Get Set �Լ�

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

// �޽� ����
bool Mesh::CreateMesh(const std::string& _strMeshName, const std::string& _strShaderKey, const std::string& _strInputLayoutKey, int _iVtxSize, int _iVtxCount,
	D3D11_USAGE _eVtxUsage, D3D11_PRIMITIVE_TOPOLOGY _ePrimitive,
	void* _pVtx, int _iIdxSize /*= 0*/, int _iIdxCount/* = 0*/,
	D3D11_USAGE _eIdxUSage/* = D3D11_USAGE_DEFAULT*/,
	DXGI_FORMAT _eFormat/* = DXGI_FORMAT_UNKNOWN*/,
	void* _pIdx /*= nullptr*/)
{
	m_strTag = _strMeshName;				// ã�� ���Ϸ��� �޽� �̸� ����
	m_strShaderKey = _strShaderKey;
	m_strInputLayoutKey = _strInputLayoutKey;

	// �޽������̳� ������ �־��ֱ�
	PMeshContainer pNewMeshContainer = new MeshContainer;
	m_MeshContainerVec.push_back(pNewMeshContainer);

	// ���ؽ� ���� ����
	if (false == CreateVertexBuffer(_iVtxSize, _iVtxCount, _eVtxUsage, _ePrimitive, _pVtx))
	{
		return false;	// ����
	}

	// �ε��� ���� ����  -> �ε����� ������ ������ֱ�
	if (nullptr != _pIdx)
	{
		if (false == CreateIndexBuffer(_iIdxSize, _iIdxCount, _eIdxUSage, _eFormat, _pIdx))
		{
			return false;	// ����
		}
	}

	return true;

}

// �޽� ���� (���� ����)
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

	// lstrcat : �ش� ���ڿ� ���� �ٸ� ���ڿ��� �����ϴ� �Լ�
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

	// lstrcat : �ش� ���ڿ� ���� �ٸ� ���ڿ��� �����ϴ� �Լ�
	// strFullPath + _pFileName
	strcat_s(strFullPath, _pFileName);

	return LoadMeshFromFullPath(_strName, strFullPath);

	return true;
}

bool Mesh::LoadMeshFromFullPath(const std::string & _strName, const TCHAR * _pFullPath, const Vector3 & _vView)
{
	char strFullPath[MAX_PATH] = {};

	// WideCharToMultiByte : �����ڵ� ->��Ƽ����Ʈ
	// -1�� ���ڿ� ��ü�� ����
	WideCharToMultiByte(CP_UTF8, 0, _pFullPath, -1,
		strFullPath, lstrlen(_pFullPath), 0, 0);

	return LoadMeshFromFullPath(_strName, strFullPath);
}

bool Mesh::LoadMeshFromFullPath(const std::string & _strName, const char * _pFullPath, const Vector3 & _vView)
{
	SetTag(_strName);
	m_vView = _vView;

	char strExtention[_MAX_EXT] = {};		// Ȯ���� ��

	// ���ڿ� �ڸ���
	_splitpath_s(_pFullPath, nullptr, 0, nullptr, 0, nullptr, 0, strExtention, _MAX_EXT);

	// ���ڿ� �빮�ڷ� ��ȯ
	_strupr_s(strExtention);

	if (0 == strcmp(strExtention, ".FBX"))
	{
		FBXLoader Loader;

		Loader.LoadFbx(_pFullPath);

		return ConvertFbx(&Loader, _pFullPath);
	}

	return LoadFromFullPath(_pFullPath);
}

// ����¿� ���� ����
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

// �ν��Ͻ��� �ֵ� ���� ->�ν��Ͻ��� �ֵ��� ���۸� 2�� �����.
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

// ���ؽ� ���� ����
bool Mesh::CreateVertexBuffer(int _iSize, int _iCount, D3D11_USAGE _eUsage,
	D3D11_PRIMITIVE_TOPOLOGY _ePrimitive, void* _pData)
{
	PMeshContainer pFindMeshContainer = m_MeshContainerVec.back();

	D3D11_BUFFER_DESC	tDesc = {};						// ���� ���۸� �����ϴ� ����ü

	tDesc.ByteWidth = _iSize * _iCount;					// ������ ���� ������ ũ��(����Ʈ ����)
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;			// ������ �÷��� ����
	tDesc.Usage = _eUsage;								// ���۰� ���̴� ���

	switch (_eUsage)
	{
	case D3D11_USAGE_DEFAULT:
		break;
	case D3D11_USAGE_IMMUTABLE:
		break;
	case D3D11_USAGE_DYNAMIC:
		tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;			// ���� ����
		break;
	case D3D11_USAGE_STAGING:
		tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;		// �б�, ����
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
	tData.pSysMem = pFindMeshContainer->tVB.pData;			// ���� ���۸� �ʱ�ȭ�� �ڷḦ ���� �ý��� �޸� �迭�� ����Ű�� ������


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

// �ε��� ���� ����
bool Mesh::CreateIndexBuffer(int _iSize, int _iCount, D3D11_USAGE _eUsage,
	DXGI_FORMAT _eFormat, void* _pData)
{
	PMeshContainer	pFindContainer = m_MeshContainerVec.back();			// �����ϰ� �ٷ� ���ؽ�, �ε����� �����ϴϱ� �ڿ��� �����´�
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
	pIB->pData = new char[_iSize * _iCount];			// 1����Ʈ�� ��������

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
	// ����ִ� �ε���, ������� ���µ� �ִٰ� �Ǿ��ִ� �͵� ���� (�����̳ʰ� �������� ������)
	std::vector<bool>	vecEmptyIndex;
	bool				bAnimation = false;

	for (; iter != iterEnd; ++iter)
	{
		PMeshContainer	pContainer = new MeshContainer;

		m_strInputLayoutKey = LAYOUT_VERTEX3D;

		m_MeshContainerVec.push_back(pContainer);

		int	iVtxSize = 0;

		// ������ ���� ���
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

		// ������ ���� ���
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

		m_strShaderKey = SHADER_STANDARD_3D;				// �⺻�ؽ�ó��

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

		// �ε������� ����
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

	// ���� ������ �о�´�.
	const std::vector<std::vector<PFBXMATERIAL>>*	pMaterials = _pLoader->GetMaterials();

	std::vector<std::vector<PFBXMATERIAL>>::const_iterator	iterM = pMaterials->begin();
	std::vector<std::vector<PFBXMATERIAL>>::const_iterator	iterMEnd = pMaterials->end();

	if (false == pMaterials->empty())
	{
		// ���� ����� ���� Ŭ������ �����Ѵ�.
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
			// �ε��� ���۰� ������� ��쿡�� ������ �߰����� �ʴ´�.
			if (false == vecEmptyIndex[i])
				continue;

			// ���� ������ ���´�.
			PFBXMATERIAL	pMtrl = (*iterM)[i];

			m_pMaterial->SetColor(pMtrl->vDiffuse, pMtrl->vAmbient,
				pMtrl->vSpecular, pMtrl->vEmissive, pMtrl->fShininess, iContainer, (UINT)(i));

			// �ִϸ��̼��� �����Ҷ�
			if (true == bAnimation)
			{
				// ��Ű�� ����
				m_pMaterial->Skinning(iContainer, (int)i);
			}

			// �̸��� �ҷ��´�.
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

			// ���÷��� �ؽ�ó ���� �����Ѵ�.
			m_pMaterial->AddTextureSetFromFullPath(iContainer, (int)i,
				0, strName,
				strPath);


			// ������ �ִٸ�(�븻)
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

				// ���÷�(X������ �ƴ� ���̴����� ó���ϱ����) �ؽ�ó ���� �����Ѵ�.
				m_pMaterial->AddTextureSetFromFullPath(iContainer, (int)i, 1, strName, strPath);
				m_pMaterial->BumpTextureEnable(iContainer, (int)i);
			}


			// sepcular �ؽ�ó
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

				// ���÷��� �ؽ�ó ���� �����Ѵ�.
				m_pMaterial->AddTextureSetFromFullPath(iContainer, (int)i,  2, strName, strPath);
				m_pMaterial->SepcularTextureEnable(iContainer, (int)i);
			}
		}
	}
#pragma region ����
	// �ؽ��İ� ����� �������� Ű�� �����Ѵ�.
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

	// Mesh\\ ������ ��θ� �����Ѵ�.
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

	// �ִϸ��̼� ó��
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

	//// �� ����ŭ �ݺ��Ѵ�.
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

	// �ִϸ��̼� Ŭ���� �߰��Ѵ�.
	const std::vector<PFBXANIMATIONCLIP>* pvecClip = _pLoader->GetClips();

	// Ŭ���� �о�´�.
	std::vector<PFBXANIMATIONCLIP>::const_iterator	iterC = pvecClip->begin();
	std::vector<PFBXANIMATIONCLIP>::const_iterator	iterCEnd = pvecClip->end();

	for (; iterC != iterCEnd; ++iterC)
	{
		m_pAnimation->AddClip(AO_LOOP, *iterC);
	}

	// �ִϸ��̼��� �ִ� ��� �������� �ִϸ��̼� ������ �����´�.
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
	// ��ü����
	FILE*	pFile = nullptr;

	// write
	fopen_s(&pFile, _pFullPath, "wb");

	if (nullptr == pFile)
	{
		return false;
	}

	int iLength = (int)(m_strTag.length());

	// Tag ���̸� �����Ѵ�.
	fwrite(&iLength, sizeof(int), 1, pFile);
	fwrite(m_strTag.c_str(), 1, iLength, pFile);

	// SahderName ���̸� �����Ѵ�.
	iLength = (int)(m_strShaderKey.length());
	fwrite(&iLength, sizeof(int), 1, pFile);
	fwrite(m_strShaderKey.c_str(), 1, iLength, pFile);

	// �Է·��̾ƿ� �̸� ���̸� �����Ѵ�.
	iLength = (int)(m_strInputLayoutKey.length());
	fwrite(&iLength, sizeof(int), 1, pFile);
	fwrite(m_strInputLayoutKey.c_str(), 1, iLength, pFile);

	// ������ ����
	fwrite(&m_vCenter, sizeof(Vector3), 1, pFile);
	fwrite(&m_fRadius, sizeof(float), 1, pFile);
	fwrite(&m_vMin, sizeof(Vector3), 1, pFile);
	fwrite(&m_vMax, sizeof(Vector3), 1, pFile);
	fwrite(&m_vLength, sizeof(Vector3), 1, pFile);

	// �޽������� �����ϱ� ( �����̳� -> ������� ���ؽ����������� �ε��������� )
	size_t iContainer = m_MeshContainerVec.size();		// �޽� �����̳� ��
	fwrite(&iContainer, sizeof(size_t), 1, pFile);

	for (size_t i = 0; i < iContainer; ++i)
	{
		PMeshContainer pContainer = m_MeshContainerVec[i];

		// ���ؽ�
		fwrite(&pContainer->tVB.ePrimitive, sizeof(D3D11_PRIMITIVE_TOPOLOGY), 1, pFile);		// ���� �⺻���� ����
		
		fwrite(&pContainer->tVB.iSize, sizeof(int), 1, pFile);									// ���ؽ� �ϳ� ������
		fwrite(&pContainer->tVB.iCount, sizeof(int), 1, pFile);									// ���ؽ� ����
		fwrite(&pContainer->tVB.eUsage, sizeof(D3D11_USAGE), 1, pFile);							// �뵵
		fwrite(pContainer->tVB.pData, pContainer->tVB.iSize, pContainer->tVB.iCount, pFile);	// ������ ����

		// �ε���
		size_t iIdxCount = pContainer->vecIB.size();
		fwrite(&iIdxCount, sizeof(size_t), 1, pFile);

		for (size_t j = 0; j < iIdxCount; ++j)
		{
			fwrite(&pContainer->vecIB[j]->eFormat, sizeof(DXGI_FORMAT), 1, pFile);				// �ε��� ���� 
			fwrite(&pContainer->vecIB[j]->iSize, sizeof(int), 1, pFile);						// �ε��� ������
			fwrite(&pContainer->vecIB[j]->iCount, sizeof(int), 1, pFile);						// �ε��� ����
			fwrite(&pContainer->vecIB[j]->eUsage, sizeof(D3D11_USAGE), 1, pFile);				// �뵵
			fwrite(pContainer->vecIB[j]->pData, pContainer->vecIB[j]->iSize, pContainer->vecIB[j]->iCount, pFile);	// ������ ����
		}
	}
	
	bool	bMaterial = false;

	if (nullptr != m_pMaterial)
	{
		// ���� ������ �ִ� ���, ���������� ����
		bMaterial = true;
		fwrite(&bMaterial, sizeof(bool), 1, pFile);

		m_pMaterial->Save(pFile);
	}
	else
	{
		// ���� ������ ���� ���
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

	// Tag ���̸� �о �����Ѵ�.
	fread(&iLength, sizeof(int), 1, pFile);
	char	strTag[256] = {};
	fread(strTag, 1, iLength, pFile);
	m_strTag = strTag;

	// ShaderName ���̸� �����Ѵ�. ( + ���̴�Ű�̸���!)
	iLength = 0;
	fread(&iLength, sizeof(int), 1, pFile);
	char	strShaderKey[256] = {};
	fread(strShaderKey, 1, iLength, pFile);
	m_strShaderKey = strShaderKey;

	// �Է� ���̾ƿ� �̸� ���̸� �����Ѵ� (+Key����)
	iLength = 0;
	fread(&iLength, sizeof(int), 1, pFile);
	char	strLayoutKey[256] = {};
	fread(strLayoutKey, 1, iLength, pFile);
	m_strInputLayoutKey = strLayoutKey;

	// ������ ���� �о���� + ����
	fread(&m_vCenter, sizeof(Vector3), 1, pFile);
	fread(&m_fRadius, sizeof(float), 1, pFile);
	fread(&m_vMin, sizeof(Vector3), 1, pFile);
	fread(&m_vMax, sizeof(Vector3), 1, pFile);
	fread(&m_vLength, sizeof(Vector3), 1, pFile);

	// �޽� �����̳� �о����
	size_t	iContainer = 0;

	fread(&iContainer, sizeof(size_t), 1, pFile);

	for (size_t i = 0; i < iContainer; ++i)
	{
		// �޽� �����̳� �о����
		PMeshContainer	pContainer = new MeshContainer;
		m_MeshContainerVec.push_back(pContainer);

		int iVtxSize = 0;
		int iVtxCount = 0;
		D3D11_USAGE eUsage;

		fread(&pContainer->tVB.ePrimitive, sizeof(D3D11_PRIMITIVE_TOPOLOGY), 1, pFile);
		fread(&iVtxSize, sizeof(int), 1, pFile);
		fread(&iVtxCount, sizeof(int), 1, pFile);
		fread(&eUsage, sizeof(D3D11_USAGE), 1, pFile);

		// ���ؽ� ������ �о���� + �����ϱ�
		char* pData = new char[iVtxSize * iVtxCount];
		fread(pData, iVtxSize, iVtxCount, pFile);

		CreateVertexBuffer(iVtxSize, iVtxCount, eUsage, 
			pContainer->tVB.ePrimitive, pData);

		SAFE_DELETE_ARRAY(pData);

		// �ε��� ������ �о���� + �����ϱ�
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

	// ������ �����ľ��ϱ� ���� ����
	bool bMaterial = false; 
	fread(&bMaterial, sizeof(bool), 1, pFile);

	if (true == bMaterial)
	{
		// ������ �ִ� ���
		SAFE_RELEASE(m_pMaterial);
		bMaterial = true;

		m_pMaterial = new Material;

		m_pMaterial->Init();
		m_pMaterial->Load(pFile);
	}

	// ������ �����ٸ� �� �ݾƾ��Ѵ�.
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
