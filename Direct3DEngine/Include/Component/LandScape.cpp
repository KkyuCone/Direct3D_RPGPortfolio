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
	_strupr_s(strExt);		// 대문자로 변환

	char strFullPath[MAX_PATH] = {};

	const char* pPath = GET_SINGLETON(PathManager)->FindPathMultibyte(_strPathName);		// 기본 경로 가져오기

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

#pragma region 비트맵 파일 포맷
	// 비트맵 파일은 바이너리 형식으로 되어 있다.
	// 비트맵 파일은 픽셀 하나를 몇 비트로 저장하는지에 따라 구조가 달라진다. ( 주로 24비트 사용 )

	// 1, 2, 4 비트		: 비트맵 파일 헤더 (14 Btye), 비트맵 정보 헤더(DIB헤더, 40 Byte), 색상 테이블 (가변), 픽셀 데이터 (가변)
	// 16, 24, 32 비트	: 비트맵 파일 헤더 ( 14 ),    비트맵 정보 헤더( 40 ),        픽셀 데이터(가변)   -> 얘는 색상테이블 없음

	// 1. 비트맵 파일 헤더		: 비트맵 파일 식별 정보, 파일크기, 데이터 위치 등
	// 2. 정보 헤더				: 가로, 세로 크기, 해상도, 픽셀의 비트 수 등
	// 3. 픽셀 데이터			: 그림 파일의 실제 색상 정보 ( 픽셀 색상 수가 16비트 미만일때는 색상 테이블에 따로 저장하고 픽셀 데이터데는 인덱스 저장 )


	// 24 비트 비트맵은 픽셀을 BGR 순서로 저장하고 색상의 크기는 1바이트이다. ( 각 픽셀당 3바이트 사용 )

	// 그리고 비트맵의 실제 메모리에 저장된건 이미지가 상하 반전으로 저장되어 있다.
	// 출력시에 상하반전을 원래대로 되돌리고 알아서 출력함 ㅇㅅㅇ 
	// 그리고 해당 정보들은 API에서 구조체를 제공한다.
#pragma endregion

	if (0 == strcmp(strExt, ".BMP"))
	{
		BITMAPFILEHEADER	FileH;			// 비트맵 파일 헤더
		BITMAPINFOHEADER	InfoH;			// 비트맵 정보 헤더

		fread(&FileH, sizeof(FileH), 1, pFile);
		fread(&InfoH, sizeof(InfoH), 1, pFile);

		// bfOffBits는 파일 헤더와 인포헤더의 사이즈를 합한 값이다.
		int iPixelLength = FileH.bfSize - FileH.bfOffBits;		// 이러면 나머지 픽셀 정보의 길이를 알 수 있다.

		m_iNumX = InfoH.biWidth;
		m_iNumZ = InfoH.biHeight;

		// 전체 픽셀의 크기만큼 동적할당한다.
		unsigned char* pPixel = new unsigned char[iPixelLength];

		// 전체 픽셀 정보를 읽어온다.
		fread(pPixel, 1, iPixelLength, pFile);

		int iPixelByte = 0;

		// biBitCount : 한 픽셀당 몇 비트로 되어있는지..
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


		// 이미지 한줄 크기(지형)씩 읽어서 비트이미지 반전시킨다.
		// 위에 한줄 , 아래한줄 -> 서로 바꾸기 
		// 비트맵은 상하 반전으로 나와서 이거 다시 바꿔줘야하기떄문 ㅇㅅㅇ(원래대로)
		unsigned char* pLine = new unsigned char[m_iNumX * iPixelByte];	
		//memcpy(pLine, 0, sizeof(char) * m_iNumX * iPixelByte);

		for (int i = 0; i < (m_iNumZ / 2); ++i)
		{

			memcpy(pLine, &pPixel[i * m_iNumX * iPixelByte], 
				m_iNumX * iPixelByte);		// 위에 한줄값을 가져오기  temp

			memcpy(&pPixel[i * m_iNumX * iPixelByte], 
				&pPixel[(m_iNumZ - 1 - i) * m_iNumX * iPixelByte], 
				m_iNumX * iPixelByte);				// 위의 한줄값을 아래의 한줄값으로 바꾸기
			
			memcpy(&pPixel[(m_iNumZ - 1 - i) * m_iNumX * iPixelByte], 
				pLine, m_iNumX * iPixelByte);		// 끝줄에 윗줄값들을 넣어주기	
		}

		SAFE_DELETE_ARRAY(pLine);

		// 미리 메모리를 할당해 놓는다.
		m_vecVertex.reserve(m_iNumX * m_iNumZ);

		// 정점정보 넣기
		for (int i = 0; i < m_iNumZ; ++i)
		{
			for (int j = 0; j < m_iNumX; ++j)
			{
				Vertex3D tVertex = {};
				int Index = i * m_iNumX * iPixelByte + j * iPixelByte;		// 픽셀 인덱스 

				// 40.0f, 20.0f은 임의의 수임 (변수로 바꿔서 내가 지정할 수있음 )
				float fY = pPixel[Index] / 20.0f;		// 높이, 우리는 현재 흰색, 검정, 회색 이렇게 높이를 표현할건데 전부 값이 동일하니까 R값만 가져와서 사용하기로한다.
				
				tVertex.vPosition = Vector3((float)j, fY, float(m_iNumZ - i - 1));
				tVertex.vNormal = Vector3(0.0f, 0.0f, 0.0f);		// 일단 0으로 초기화하고 Normal값을 따로 구할거임. merge된 버텍스들은 그 연결된 삼각형마다의 해당 버텍스들의 노말값들은 더해서 평균값으로 사용해야됨
				tVertex.vUV = Vector2((float)j / (m_iNumX - 1.0f), (float)i / (m_iNumZ - 1.0f));
				//tVertex.vUV = Vector2(j, i);
				tVertex.vTangent = Vector3(1.0f, 0.0f, 0.0f);
				tVertex.vBinormal = Vector3(0.0f, 0.0f, -1.0f);

				m_vecVertex.push_back(tVertex);
			}
		}

		// 인덱스
		m_vecIndex.reserve((m_iNumX - 1) * (m_iNumZ - 1) * 6);

		// 면법선
		m_vecFaceNormal.resize((m_iNumX - 1) * (m_iNumZ - 1) * 2);

		// 네비 메쉬 생성
		NavigationMesh* pNavMesh = GET_SINGLETON(NavigationManager)->CreateNavMesh(m_pScene, m_strTag);

		int iTriIndex = 0;			// 삼각형 인덱스(번째..)
		Vector3 vEdge1, vEdge2;		// 이거 엣지 2개로(탄젠트, 바이탄젠트) 노말벡터(지금은 면법선이랑 같음)만들거
		Vector3	vCellPos[3];		// 셀 위치(네비게이션)

		// 인덱스 정보 넣기 ( -1은 그 이전까지만 for문 돌면되니까 ㅇㅅㅇ)
		for (int i = 0; i < m_iNumZ - 1; ++i)
		{
			for (int j = 0; j < m_iNumX - 1; ++j)
			{
				int Index = i * m_iNumX + j;

				// 우상단 삼각형 추가
				m_vecIndex.push_back(Index);
				m_vecIndex.push_back(Index + 1);
				m_vecIndex.push_back(Index + m_iNumX + 1);

				// 내비메쉬 삼각형 추가
				vCellPos[0] = m_vecVertex[Index].vPosition;
				vCellPos[1] = m_vecVertex[Index + 1].vPosition;
				vCellPos[2] = m_vecVertex[Index + m_iNumX + 1].vPosition;
				pNavMesh->AddCell(vCellPos);

				// 우상단 삼각형 면벡터 계산후 추가
				// UV좌표의 한칸씩 옆에 있는 높이값의 평균을 이용해 두 개의 벡터를 만든 후 그 벡터들의 외적 구하기
				vEdge1 = m_vecVertex[Index + 1].vPosition - m_vecVertex[Index].vPosition;
				vEdge2 = m_vecVertex[Index + m_iNumX + 1].vPosition - m_vecVertex[Index].vPosition;

				vEdge1.Normalize();
				vEdge2.Normalize();

				m_vecFaceNormal[iTriIndex] = vEdge1.Cross(vEdge2);		// 외적
				m_vecFaceNormal[iTriIndex].Normalize();					// 해당 면법선 구함
				++iTriIndex;

				// 좌하단 삼각형 추가
				m_vecIndex.push_back(Index);
				m_vecIndex.push_back(Index + m_iNumX + 1);
				m_vecIndex.push_back(Index + m_iNumX);

				// 내비메쉬 삼각형 추가(좌하단)
				vCellPos[0] = m_vecVertex[Index].vPosition;
				vCellPos[1] = m_vecVertex[Index + m_iNumX + 1].vPosition;
				vCellPos[2] = m_vecVertex[Index + m_iNumX].vPosition;
				pNavMesh->AddCell(vCellPos);

				// 좌하단 삼각형 면벡터 계산후 추가
				vEdge1 = m_vecVertex[Index + m_iNumX + 1].vPosition - m_vecVertex[Index].vPosition;
				vEdge2 = m_vecVertex[Index + m_iNumX].vPosition - m_vecVertex[Index].vPosition;

				vEdge1.Normalize();
				vEdge2.Normalize();

				m_vecFaceNormal[iTriIndex] = vEdge1.Cross(vEdge2);		// 외적
				m_vecFaceNormal[iTriIndex].Normalize();					// 해당 면법선 구함
				++iTriIndex;
			}
		}

		SAFE_DELETE_ARRAY(pPixel);

		fclose(pFile);

		// 해당 면벡터들을 구했으니까
		// 각 정점의 노말값과 탄젠트, 바이탄젠트 값을 계산후 넣는다. 노말값은 겹치는 부분의 점의 노말갑스이 평균값을 넣어준다.
		ComputeNormal();
		ComputeTangent();

		// 네비메쉬
		pNavMesh->CreateGridMapAdj(m_iNumX - 1);

		// 정점정보와 인덱스정보를 만들었으니 이제 메쉬를 만들자 ~
		GET_SINGLETON(ResourcesManager)->CreateMesh(_strName, SHADER_LANDSCAPE,
			LAYOUT_VERTEX3D, Vector3(0.0f, 1.0f, 0.0f),
			sizeof(Vertex3D), (int)(m_vecVertex.size()), D3D11_USAGE_DEFAULT,
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, &m_vecVertex[0],
			4, (int)(m_vecIndex.size()), D3D11_USAGE_DEFAULT,
			DXGI_FORMAT_R32_UINT, &m_vecIndex[0]);

		Renderer* pRenderer = FindComponentFromType<Renderer>(CT_RENDERER);

		if (nullptr == pRenderer)
		{
			// 없으면 만들어주기
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
	// ㅇㅁㅇ 없넹~? return
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
	// 합쳐진..(공유하는) 정점들의 노말값들을 전부 더해서 (겹친애들만더함) 그들의 평균값을 넣어준다.
	for (size_t i = 0; i < m_vecFaceNormal.size(); ++i)
	{
		int Index0 = m_vecIndex[i * 3];
		int Index1 = m_vecIndex[i * 3 + 1];
		int Index2 = m_vecIndex[i * 3 + 2];

		// 겹쳐진 부분의 값들을 더한다.
		// 면하나당의 정점의 노말값들은 동일하므로 더해준다.( 삼각형을 이루는 정점은 3개다)
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
	// 탄젠트 벡터 계산
	for (size_t i = 0; i < m_vecFaceNormal.size(); ++i)
	{
		int Index0 = m_vecIndex[i * 3];
		int Index1 = m_vecIndex[i * 3 + 1];
		int Index2 = m_vecIndex[i * 3 + 2];

		// 탄젠트벡터와 바이노말을 구하려면
		// 탄젠트 공간에서 구해야한다.
		// 탄젠트 공간은 법선벡터가 Z축인 공간이다. (월드-> 탄젠트)공간으로 이동시켜야한다.
		float fVtx1[3], fVtx2[3];

		fVtx1[0] = m_vecVertex[Index1].vPosition.x - m_vecVertex[Index0].vPosition.x;
		fVtx1[1] = m_vecVertex[Index1].vPosition.y - m_vecVertex[Index0].vPosition.y;
		fVtx1[2] = m_vecVertex[Index1].vPosition.z - m_vecVertex[Index0].vPosition.z;

		fVtx2[0] = m_vecVertex[Index2].vPosition.x - m_vecVertex[Index0].vPosition.x;
		fVtx2[1] = m_vecVertex[Index2].vPosition.y - m_vecVertex[Index0].vPosition.y;
		fVtx2[2] = m_vecVertex[Index2].vPosition.z - m_vecVertex[Index0].vPosition.z;


		// 탄젠트 U , 바이탄젠트(노말) V
		float fTU[2], fTV[2];
		fTU[0] = m_vecVertex[Index1].vUV.x - m_vecVertex[Index0].vUV.x;
		fTV[0] = m_vecVertex[Index1].vUV.y - m_vecVertex[Index0].vUV.y;

		fTU[1] = m_vecVertex[Index2].vUV.x - m_vecVertex[Index0].vUV.x;
		fTV[1] = m_vecVertex[Index2].vUV.y - m_vecVertex[Index0].vUV.y;

		float	fDen = 1.f / (fTU[0] * fTV[1] - fTU[1] * fTV[0]);		// 미분

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
	m_pGameObject->EraseComponentFromTag("PickSphere");			// 지형같은 경우 피킹용 구가 필요없다.
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
