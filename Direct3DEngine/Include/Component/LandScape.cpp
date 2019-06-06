#include "LandScape.h"
#include "..\PathManager.h"
#include "..\Resource\Mesh.h"
#include "..\Resource\ResourcesManager.h"
#include "..\Resource\Texture.h"
#include "Material.h"
#include "Renderer.h"
#include "..\GameObject.h"
#include "..\Render\ShaderManager.h"
#include "..\Navigation\NavigationManager.h"
#include "..\Navigation\NavigationMesh.h"

ENGINE_USING

LandScape::LandScape() :
	m_iNumX(0), m_iNumZ(0),
	m_pMaterial(nullptr),
	m_pSplatDiffTex(nullptr),
	m_pSplatNormTex(nullptr),
	m_pSplatSpecTex(nullptr),
	m_pSplatAlphaTex(nullptr)
{
	m_eComponentType = CT_LANDSCAPE;
	SetTag("LandScape");
	SetDetailLevel(30.0f, 30.0f);
	m_tCBuffer.iSplatCount = 0;
}

LandScape::LandScape(const LandScape & _Com) : Component(_Com)
{
	m_iNumX = _Com.m_iNumX;
	m_iNumZ = _Com.m_iNumZ;

	m_tCBuffer = _Com.m_tCBuffer;
	m_pMaterial = nullptr;
	m_pSplatDiffTex = _Com.m_pSplatDiffTex;
	m_pSplatNormTex = _Com.m_pSplatNormTex;
	m_pSplatSpecTex = _Com.m_pSplatSpecTex;
	m_pSplatAlphaTex = _Com.m_pSplatAlphaTex;

	if (nullptr != m_pSplatDiffTex)
	{
		m_pSplatDiffTex->AddReference();
	}

	if (nullptr != m_pSplatNormTex)
	{
		m_pSplatNormTex->AddReference();
	}

	if (nullptr != m_pSplatSpecTex)
	{
		m_pSplatSpecTex->AddReference();
	}

	if (nullptr != m_pSplatAlphaTex)
	{
		m_pSplatAlphaTex->AddReference();
	}


}


LandScape::~LandScape()
{
	SAFE_RELEASE(m_pMaterial);
	SAFE_RELEASE(m_pSplatDiffTex);
	SAFE_RELEASE(m_pSplatNormTex);
	SAFE_RELEASE(m_pSplatSpecTex);
	SAFE_RELEASE(m_pSplatAlphaTex);
}

bool LandScape::CreateLandScape(const std::string & _strName,
	const char * _pFileName, const std::string & _strPathName)
{
	char	strExt[_MAX_EXT] = {};

	_splitpath_s(_pFileName, 0, 0, 0, 0, 0, 0, strExt, _MAX_EXT);
	_strupr_s(strExt);		// �빮�ڷ� ��ȯ

	char strFullPath[MAX_PATH] = {};

	const char* pPath = GET_SINGLETON(PathManager)->FindPathMultibyte(_strPathName);		// �⺻ ��� ��������

	if (nullptr != pPath)
	{
		strcpy_s(strFullPath, pPath);
	}
	else
	{
		return false;
	}

	strcat_s(strFullPath, _pFileName);

	FILE*	pFile = nullptr;
	fopen_s(&pFile, strFullPath, "rb");

	if (nullptr == pFile)
	{
		return false;
	}

#pragma region ��Ʈ�� ���� ����
	// ��Ʈ�� ������ ���̳ʸ� �������� �Ǿ� �ִ�.
	// ��Ʈ�� ������ �ȼ� �ϳ��� �� ��Ʈ�� �����ϴ����� ���� ������ �޶�����. ( �ַ� 24��Ʈ ��� )

	// 1, 2, 4 ��Ʈ		: ��Ʈ�� ���� ��� (14 Btye), ��Ʈ�� ���� ���(DIB���, 40 Byte), ���� ���̺� (����), �ȼ� ������ (����)
	// 16, 24, 32 ��Ʈ	: ��Ʈ�� ���� ��� ( 14 ),    ��Ʈ�� ���� ���( 40 ),        �ȼ� ������(����)   -> ��� �������̺� ����

	// 1. ��Ʈ�� ���� ���		: ��Ʈ�� ���� �ĺ� ����, ����ũ��, ������ ��ġ ��
	// 2. ���� ���				: ����, ���� ũ��, �ػ�, �ȼ��� ��Ʈ �� ��
	// 3. �ȼ� ������			: �׸� ������ ���� ���� ���� ( �ȼ� ���� ���� 16��Ʈ �̸��϶��� ���� ���̺� ���� �����ϰ� �ȼ� �����͵��� �ε��� ���� )


	// 24 ��Ʈ ��Ʈ���� �ȼ��� BGR ������ �����ϰ� ������ ũ��� 1����Ʈ�̴�. ( �� �ȼ��� 3����Ʈ ��� )

	// �׸��� ��Ʈ���� ���� �޸𸮿� ����Ȱ� �̹����� ���� �������� ����Ǿ� �ִ�.
	// ��½ÿ� ���Ϲ����� ������� �ǵ����� �˾Ƽ� ����� ������ 
	// �׸��� �ش� �������� API���� ����ü�� �����Ѵ�.
#pragma endregion

	if (0 == strcmp(strExt, ".BMP"))
	{
		BITMAPFILEHEADER	FileH;			// ��Ʈ�� ���� ���
		BITMAPINFOHEADER	InfoH;			// ��Ʈ�� ���� ���

		fread(&FileH, sizeof(FileH), 1, pFile);
		fread(&InfoH, sizeof(InfoH), 1, pFile);

		// bfOffBits�� ���� ����� ��������� ����� ���� ���̴�.
		int iPixelLength = FileH.bfSize - FileH.bfOffBits;		// �̷��� ������ �ȼ� ������ ���̸� �� �� �ִ�.

		m_iNumX = InfoH.biWidth;
		m_iNumZ = InfoH.biHeight;

		// ��ü �ȼ��� ũ�⸸ŭ �����Ҵ��Ѵ�.
		unsigned char* pPixel = new unsigned char[iPixelLength];

		// ��ü �ȼ� ������ �о�´�.
		fread(pPixel, 1, iPixelLength, pFile);

		int iPixelByte = 0;

		// biBitCount : �� �ȼ��� �� ��Ʈ�� �Ǿ��ִ���..
		switch (InfoH.biBitCount)
		{
		case 8:
			iPixelByte = 1;
			break;
		case 24:
			iPixelByte = 3;
			break;
		case 32:
			iPixelByte = 4;
			break;
		default:
			assert(false);
			break;
		}


		// �̹��� ���� ũ��(����)�� �о ��Ʈ�̹��� ������Ų��.
		// ���� ���� , �Ʒ����� -> ���� �ٲٱ� 
		// ��Ʈ���� ���� �������� ���ͼ� �̰� �ٽ� �ٲ�����ϱ⋚�� ������(�������)
		unsigned char* pLine = new unsigned char[m_iNumX * iPixelByte];	
		//memcpy(pLine, 0, sizeof(char) * m_iNumX * iPixelByte);

		for (int i = 0; i < (m_iNumZ / 2); ++i)
		{

			memcpy(pLine, &pPixel[i * m_iNumX * iPixelByte], 
				m_iNumX * iPixelByte);		// ���� ���ٰ��� ��������  temp

			memcpy(&pPixel[i * m_iNumX * iPixelByte], 
				&pPixel[(m_iNumZ - 1 - i) * m_iNumX * iPixelByte], 
				m_iNumX * iPixelByte);				// ���� ���ٰ��� �Ʒ��� ���ٰ����� �ٲٱ�
			
			memcpy(&pPixel[(m_iNumZ - 1 - i) * m_iNumX * iPixelByte], 
				pLine, m_iNumX * iPixelByte);		// ���ٿ� ���ٰ����� �־��ֱ�	
		}

		SAFE_DELETE_ARRAY(pLine);

		// �̸� �޸𸮸� �Ҵ��� ���´�.
		m_vecVertex.reserve(m_iNumX * m_iNumZ);

		// �������� �ֱ�
		for (int i = 0; i < m_iNumZ; ++i)
		{
			for (int j = 0; j < m_iNumX; ++j)
			{
				Vertex3D tVertex = {};
				int Index = i * m_iNumX * iPixelByte + j * iPixelByte;		// �ȼ� �ε��� 

				// 40.0f, 20.0f�� ������ ���� (������ �ٲ㼭 ���� ������ ������ )
				float fY = pPixel[Index] / 20.0f;		// ����, �츮�� ���� ���, ����, ȸ�� �̷��� ���̸� ǥ���Ұǵ� ���� ���� �����ϴϱ� R���� �����ͼ� ����ϱ���Ѵ�.
				
				tVertex.vPosition = Vector3((float)j, fY, float(m_iNumZ - i - 1));
				tVertex.vNormal = Vector3(0.0f, 0.0f, 0.0f);		// �ϴ� 0���� �ʱ�ȭ�ϰ� Normal���� ���� ���Ұ���. merge�� ���ؽ����� �� ����� �ﰢ�������� �ش� ���ؽ����� �븻������ ���ؼ� ��հ����� ����ؾߵ�
				tVertex.vUV = Vector2((float)j / (m_iNumX - 1.0f), (float)i / (m_iNumZ - 1.0f));
				//tVertex.vUV = Vector2(j, i);
				tVertex.vTangent = Vector3(1.0f, 0.0f, 0.0f);
				tVertex.vBinormal = Vector3(0.0f, 0.0f, -1.0f);

				m_vecVertex.push_back(tVertex);
			}
		}

		// �ε���
		m_vecIndex.reserve((m_iNumX - 1) * (m_iNumZ - 1) * 6);

		// �����
		m_vecFaceNormal.resize((m_iNumX - 1) * (m_iNumZ - 1) * 2);

		// �׺� �޽� ����
		NavigationMesh* pNavMesh = GET_SINGLETON(NavigationManager)->CreateNavMesh(m_pScene, m_strTag);

		int iTriIndex = 0;			// �ﰢ�� �ε���(��°..)
		Vector3 vEdge1, vEdge2;		// �̰� ���� 2����(ź��Ʈ, ����ź��Ʈ) �븻����(������ ������̶� ����)�����
		Vector3	vCellPos[3];		// �� ��ġ(�׺���̼�)

		// �ε��� ���� �ֱ� ( -1�� �� ���������� for�� ����Ǵϱ� ������)
		for (int i = 0; i < m_iNumZ - 1; ++i)
		{
			for (int j = 0; j < m_iNumX - 1; ++j)
			{
				int Index = i * m_iNumX + j;

				// ���� �ﰢ�� �߰�
				m_vecIndex.push_back(Index);
				m_vecIndex.push_back(Index + 1);
				m_vecIndex.push_back(Index + m_iNumX + 1);

				// ����޽� �ﰢ�� �߰�
				vCellPos[0] = m_vecVertex[Index].vPosition;
				vCellPos[1] = m_vecVertex[Index + 1].vPosition;
				vCellPos[2] = m_vecVertex[Index + m_iNumX + 1].vPosition;
				pNavMesh->AddCell(vCellPos);

				// ���� �ﰢ�� �麤�� ����� �߰�
				// UV��ǥ�� ��ĭ�� ���� �ִ� ���̰��� ����� �̿��� �� ���� ���͸� ���� �� �� ���͵��� ���� ���ϱ�
				vEdge1 = m_vecVertex[Index + 1].vPosition - m_vecVertex[Index].vPosition;
				vEdge2 = m_vecVertex[Index + m_iNumX + 1].vPosition - m_vecVertex[Index].vPosition;

				vEdge1.Normalize();
				vEdge2.Normalize();

				m_vecFaceNormal[iTriIndex] = vEdge1.Cross(vEdge2);		// ����
				m_vecFaceNormal[iTriIndex].Normalize();					// �ش� ����� ����
				++iTriIndex;

				// ���ϴ� �ﰢ�� �߰�
				m_vecIndex.push_back(Index);
				m_vecIndex.push_back(Index + m_iNumX + 1);
				m_vecIndex.push_back(Index + m_iNumX);

				// ����޽� �ﰢ�� �߰�(���ϴ�)
				vCellPos[0] = m_vecVertex[Index].vPosition;
				vCellPos[1] = m_vecVertex[Index + m_iNumX + 1].vPosition;
				vCellPos[2] = m_vecVertex[Index + m_iNumX].vPosition;
				pNavMesh->AddCell(vCellPos);

				// ���ϴ� �ﰢ�� �麤�� ����� �߰�
				vEdge1 = m_vecVertex[Index + m_iNumX + 1].vPosition - m_vecVertex[Index].vPosition;
				vEdge2 = m_vecVertex[Index + m_iNumX].vPosition - m_vecVertex[Index].vPosition;

				vEdge1.Normalize();
				vEdge2.Normalize();

				m_vecFaceNormal[iTriIndex] = vEdge1.Cross(vEdge2);		// ����
				m_vecFaceNormal[iTriIndex].Normalize();					// �ش� ����� ����
				++iTriIndex;
			}
		}

		SAFE_DELETE_ARRAY(pPixel);

		fclose(pFile);

		// �ش� �麤�͵��� �������ϱ�
		// �� ������ �븻���� ź��Ʈ, ����ź��Ʈ ���� ����� �ִ´�. �븻���� ��ġ�� �κ��� ���� �븻������ ��հ��� �־��ش�.
		ComputeNormal();
		ComputeTangent();

		// �׺�޽�
		pNavMesh->CreateGridMapAdj(m_iNumX - 1);

		// ���������� �ε��������� ��������� ���� �޽��� ������ ~
		GET_SINGLETON(ResourcesManager)->CreateMesh(_strName, SHADER_LANDSCAPE,
			LAYOUT_VERTEX3D, Vector3(0.0f, 1.0f, 0.0f),
			sizeof(Vertex3D), (int)(m_vecVertex.size()), D3D11_USAGE_DEFAULT,
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, &m_vecVertex[0],
			4, (int)(m_vecIndex.size()), D3D11_USAGE_DEFAULT,
			DXGI_FORMAT_R32_UINT, &m_vecIndex[0]);

		Renderer* pRenderer = FindComponentFromType<Renderer>(CT_RENDERER);

		if (nullptr == pRenderer)
		{
			// ������ ������ֱ�
			pRenderer = AddComponent<Renderer>("LandScapeRenderer");
		}

		pRenderer->SetMesh(_strName);

		SAFE_RELEASE(pRenderer);

		m_pMaterial = FindComponentFromType<Material>(CT_MATERIAL);

		if (nullptr == m_pMaterial)
		{
			m_pMaterial = AddComponent<Material>("LandScapeMaterial");
		}

		m_pMaterial->AddTextureSet(0, 0, 0, "LandScapeDiffuse",
			TEXT("LandScape/BD_Terrain_Cave_01.dds"));
		m_pMaterial->AddTextureSet(0, 0, 1, "LandScapeNormal",
			TEXT("LandScape/BD_Terrain_Cave_01_NRM.bmp"));
		m_pMaterial->AddTextureSet(0, 0, 2, "LandScapeSpecular",
			TEXT("LandScape/BD_Terrain_Cave_01_SPC.bmp"));

		m_pMaterial->BumpTextureEnable(0, 0);
		m_pMaterial->SepcularTextureEnable(0, 0);

		return true;
	}

	fclose(pFile);
	return true;
}

void LandScape::SetDetailLevel(float _fLevelX, float _fLevelZ)
{
	m_tCBuffer.fDetailLevelX = _fLevelX;
	m_tCBuffer.fDetailLevelZ = _fLevelZ;
}

void LandScape::ChangeBaseTexture(const TCHAR * _pDiffuseName, const TCHAR * _pNormalName, const TCHAR * _pSpeuclarName, const std::string & _strPathName)
{
	m_pMaterial->ChangeTextureSet(0, 0, 0, "LandScapeDiffuse",
		"LandScapeDiffuse", _pDiffuseName, _strPathName);
	m_pMaterial->ChangeTextureSet(0, 0, 1, "LandScapeNormal",
		"LandScapeNormal", _pNormalName, _strPathName);
	m_pMaterial->ChangeTextureSet(0, 0, 2, "LandScapeSpecular",
		"LandScapeSpecular", _pSpeuclarName, _strPathName);
	return;
}

void LandScape::AddSplatTexture(const TCHAR * _pDiffuseName, 
	const TCHAR * _pNormalName, 
	const TCHAR * _pSpecualrName, 
	const TCHAR * _pAlphaName, 
	const std::string & _strPathName)
{
	// Diffuse
	TCHAR* pName = new TCHAR[MAX_PATH];
	memset(pName, 0, sizeof(TCHAR) * MAX_PATH);
	lstrcpy(pName, _pDiffuseName);
	m_tSplatName.vecDiffuseName.push_back(pName);

	// Normal
	pName = new TCHAR[MAX_PATH];
	memset(pName, 0, sizeof(TCHAR) * MAX_PATH);
	lstrcpy(pName, _pNormalName);
	m_tSplatName.vecNormalName.push_back(pName);

	// Specualr
	pName = new TCHAR[MAX_PATH];
	memset(pName, 0, sizeof(TCHAR) * MAX_PATH);
	lstrcpy(pName, _pSpecualrName);
	m_tSplatName.vecSpecularName.push_back(pName);

	// Alpha
	pName = new TCHAR[MAX_PATH];
	memset(pName, 0, sizeof(TCHAR) * MAX_PATH);
	lstrcpy(pName, _pAlphaName);
	m_tSplatName.vecAlphaName.push_back(pName);

	m_tSplatName.vecPathName.push_back(_strPathName);

	return;
}

bool LandScape::CreateSplatTexture()
{
	// ������ ����~? return
	if (true == m_tSplatName.vecDiffuseName.empty())
	{
		return false;
	}

	SAFE_RELEASE(m_pSplatDiffTex);
	SAFE_RELEASE(m_pSplatNormTex);
	SAFE_RELEASE(m_pSplatSpecTex);
	SAFE_RELEASE(m_pSplatAlphaTex);

	m_pMaterial->ChangeTextureSetArray(0, 0, 10, "SplatDiffuse", "SplatDiffuse",
		&m_tSplatName.vecDiffuseName, m_tSplatName.vecPathName[0]);

	m_pMaterial->ChangeTextureSetArray(0, 0, 11, "SplatNormal", "SplatNormal",
		&m_tSplatName.vecNormalName, m_tSplatName.vecPathName[0]);

	m_pMaterial->ChangeTextureSetArray(0, 0, 12, "SplatSpecular", "SplatSpecular",
		&m_tSplatName.vecSpecularName, m_tSplatName.vecPathName[0]);

	m_pMaterial->ChangeTextureSetArray(0, 0, 13, "SplatAlpha", "SplatAlpha",
		&m_tSplatName.vecAlphaName, m_tSplatName.vecPathName[0]);

	m_tCBuffer.iSplatCount = (int)(m_tSplatName.vecDiffuseName.size());

	return true;
}

void LandScape::ComputeNormal()
{
	// ������..(�����ϴ�) �������� �븻������ ���� ���ؼ� (��ģ�ֵ鸸����) �׵��� ��հ��� �־��ش�.
	for (size_t i = 0; i < m_vecFaceNormal.size(); ++i)
	{
		int Index0 = m_vecIndex[i * 3];
		int Index1 = m_vecIndex[i * 3 + 1];
		int Index2 = m_vecIndex[i * 3 + 2];

		// ������ �κ��� ������ ���Ѵ�.
		// ���ϳ����� ������ �븻������ �����ϹǷ� �����ش�.( �ﰢ���� �̷�� ������ 3����)
		m_vecVertex[Index0].vNormal += m_vecFaceNormal[i];
		m_vecVertex[Index1].vNormal += m_vecFaceNormal[i];
		m_vecVertex[Index2].vNormal += m_vecFaceNormal[i];
	}

	for (size_t i = 0; i < m_vecVertex.size(); ++i)
	{
		m_vecVertex[i].vNormal.Normalize();
	}
}

void LandScape::ComputeTangent()
{
	// ź��Ʈ ���� ���
	for (size_t i = 0; i < m_vecFaceNormal.size(); ++i)
	{
		int Index0 = m_vecIndex[i * 3];
		int Index1 = m_vecIndex[i * 3 + 1];
		int Index2 = m_vecIndex[i * 3 + 2];

		// ź��Ʈ���Ϳ� ���̳븻�� ���Ϸ���
		// ź��Ʈ �������� ���ؾ��Ѵ�.
		// ź��Ʈ ������ �������Ͱ� Z���� �����̴�. (����-> ź��Ʈ)�������� �̵����Ѿ��Ѵ�.
		float fVtx1[3], fVtx2[3];

		fVtx1[0] = m_vecVertex[Index1].vPosition.x - m_vecVertex[Index0].vPosition.x;
		fVtx1[1] = m_vecVertex[Index1].vPosition.y - m_vecVertex[Index0].vPosition.y;
		fVtx1[2] = m_vecVertex[Index1].vPosition.z - m_vecVertex[Index0].vPosition.z;

		fVtx2[0] = m_vecVertex[Index2].vPosition.x - m_vecVertex[Index0].vPosition.x;
		fVtx2[1] = m_vecVertex[Index2].vPosition.y - m_vecVertex[Index0].vPosition.y;
		fVtx2[2] = m_vecVertex[Index2].vPosition.z - m_vecVertex[Index0].vPosition.z;


		// ź��Ʈ U , ����ź��Ʈ(�븻) V
		float fTU[2], fTV[2];
		fTU[0] = m_vecVertex[Index1].vUV.x - m_vecVertex[Index0].vUV.x;
		fTV[0] = m_vecVertex[Index1].vUV.y - m_vecVertex[Index0].vUV.y;

		fTU[1] = m_vecVertex[Index2].vUV.x - m_vecVertex[Index0].vUV.x;
		fTV[1] = m_vecVertex[Index2].vUV.y - m_vecVertex[Index0].vUV.y;

		float	fDen = 1.f / (fTU[0] * fTV[1] - fTU[1] * fTV[0]);		// �̺�

		Vector3	vTangent;
		vTangent.x = (fTV[1] * fVtx1[0] - fTV[0] * fVtx2[0]) * fDen;
		vTangent.y = (fTV[1] * fVtx1[1] - fTV[0] * fVtx2[1]) * fDen;
		vTangent.z = (fTV[1] * fVtx1[2] - fTV[0] * fVtx2[2]) * fDen;

		vTangent.Normalize();

		m_vecVertex[Index0].vTangent = vTangent;
		m_vecVertex[Index1].vTangent = vTangent;
		m_vecVertex[Index2].vTangent = vTangent;

		m_vecVertex[Index0].vBinormal = m_vecVertex[Index0].vNormal.Cross(vTangent);
		m_vecVertex[Index1].vBinormal = m_vecVertex[Index1].vNormal.Cross(vTangent);
		m_vecVertex[Index2].vBinormal = m_vecVertex[Index2].vNormal.Cross(vTangent);

		m_vecVertex[Index0].vBinormal.Normalize();
		m_vecVertex[Index1].vBinormal.Normalize();
		m_vecVertex[Index2].vBinormal.Normalize();
	}

}

void LandScape::Start()
{
	SAFE_RELEASE(m_pMaterial);
	m_pMaterial = FindComponentFromType<Material>(CT_MATERIAL);
	m_pGameObject->EraseComponentFromTag("PickSphere");			// �������� ��� ��ŷ�� ���� �ʿ����.
}

bool LandScape::Init()
{
	return true;
}

int LandScape::Input(float _fTime)
{
	return 0;
}

int LandScape::Update(float _fTime)
{
	return 0;
}

int LandScape::LateUpdate(float _fTime)
{
	return 0;
}

int LandScape::Collision(float _fTime)
{
	return 0;
}

int LandScape::PrevRender(float _fTime)
{
	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("LandScape", &m_tCBuffer);
	return 0;
}

int LandScape::Render(float _fTime)
{
	return 0;
}

LandScape * LandScape::Clone() const
{
	return new LandScape(*this);
}
