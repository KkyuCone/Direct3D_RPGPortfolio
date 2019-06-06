#include "FBXLoader.h"
#include <algorithm>

ENGINE_USING

FBXLoader::FBXLoader() :
	m_pManager(nullptr),
	m_pScene(nullptr),
	m_bMixamo(false),
	m_iLoadType(FLT_ALL)
{
}


FBXLoader::~FBXLoader()
{
	for (size_t i = 0; i < m_vecMaterials.size(); ++i)
	{
		for (size_t j = 0; j < m_vecMaterials[i].size(); ++j)
		{
			SAFE_DELETE(m_vecMaterials[i][j]);
		}

		m_vecMaterials[i].clear();
	}

	m_vecMaterials.clear();


	int NameCount = m_NameArr.GetCount();

	for (int i = 0; i < NameCount; ++i)
	{
		delete m_NameArr[i];
	}

	Safe_Delete_VectorList(m_vecMeshContainer);
	Safe_Delete_VectorList(m_vecBones);
	Safe_Delete_VectorList(m_vecClip);
	/*std::vector<PFBXANIMATIONCLIP>::iterator	iter = m_vecClip.begin();
	std::vector<PFBXANIMATIONCLIP>::iterator	iterEnd = m_vecClip.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE((*iter));

	}
	m_vecClip.clear();*/

	m_pScene->Clear();
	m_pScene->Destroy();
	m_pManager->Destroy();
}

const std::vector<PFBXMESHCONTAINER>* FBXLoader::GetMeshContainers() const
{
	return &m_vecMeshContainer;
}

const std::vector<std::vector<PFBXMATERIAL>>* FBXLoader::GetMaterials() const
{
	return &m_vecMaterials;
}

const std::vector<PFBXBONE>* FBXLoader::GetBones() const
{
	return &m_vecBones;
}

const std::vector<PFBXANIMATIONCLIP>* FBXLoader::GetClips() const
{
	return &m_vecClip;
}

bool FBXLoader::LoadFbx(const char * pFullPath)
{
	// FBX SDK의 대부분의 객체들은 FbxManager 싱글 톤을 사용하여 생성되고 관리됨

	// FbxManager 객체를 생성한다.
	m_pManager = FbxManager::Create();

	// IOSetting을 생성한다.
#pragma region I/O 설정 객체 만들기
	// FbxIOSettings 클래스는 씬의 요소를 파일에서 가져오거나 파일로 내보낼지 여부를 지정한다.
	// 이러한 요소엔 Camera, Light, Mesh, Texture, Material, Animation, 사용자 정의 속성 등이 포함됨.
	// FbxIOSettings 객체는 FbxImporter 또는 FbxExporter 객체에 전달되기 전에 인스턴스화 하고 구성되어야 함.
#pragma endregion
	FbxIOSettings*	pIos = FbxIOSettings::Create(m_pManager, IOSROOT);

	// FbxManager에 지정한다.
	m_pManager->SetIOSettings(pIos);

	// FbxScene을 생성한다.
	// Importer 초기화되면 파일에서 씬을 로드하기 위해선 씬 컨테이너를 만들어야함
	m_pScene = FbxScene::Create(m_pManager, "");

	// FbxImporter 를 이용해서 fbx 파일로부터 메쉬 정보를 읽어온다.
	FbxImporter*	pImporter = FbxImporter::Create(m_pManager, "");

	// 해당 경로에 있는 fbx 파일을 읽어오고 정보를 만들어낸다.
	// pImporter->Initialize ( 가져오려는 Fbx 파일명, -1은 FBX, 셋팅되어있는..(현재는 읽어오는거임))
	pImporter->Initialize(pFullPath, -1, m_pManager->GetIOSettings());

	// 위에서 만들어낸 정보를 FbxScene에 노드를 구성한다.
	pImporter->Import(m_pScene);

	// Global Scene Settings
	// 씬의 축 시스템, 주변 조명 및 시간 설정은 해당 FbxGlobalSettings 객체에 정의됨
	// 이 객체는 FbxScene::GetGlobalSettings()를 통해 접근함
	if (m_pScene->GetGlobalSettings().GetAxisSystem() != FbxAxisSystem::Max)
		m_pScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::Max);

	m_pScene->FillAnimStackNameArray(m_NameArr);

	if (m_NameArr.GetCount() > 0)
	{
		// 애니메이션 클립 정보를 읽어온다.
		LoadAnimationClip();

		// Bone 정보를 읽어온다.
		LoadBone(m_pScene->GetRootNode());

		// 애니메이션 클립이 가지고 있는 프레임을 본 수만큼 resize해준다.
		for (size_t i = 0; i < m_vecClip.size(); ++i)
		{
			m_vecClip[i]->vecBoneKeyFrame.resize(m_vecBones.size());
		}
	}

	// 삼각화
	Triangulate(m_pScene->GetRootNode());

	LoadMesh(m_pScene->GetRootNode());

	pImporter->Destroy();

	return true;
}

void FBXLoader::Triangulate(FbxNode * pNode)
{
	// FbxNodeAttribute : FbxNode 속성을 나타내는 인터페이스이다.
	// 노드로부터 얻어올 수 있다.
	FbxNodeAttribute*	pAttr = pNode->GetNodeAttribute();

	// Nurbs : Non-Uniform Rational B-Spline (NURBS)
	if (pAttr && (pAttr->GetAttributeType() == FbxNodeAttribute::eMesh ||
		pAttr->GetAttributeType() == FbxNodeAttribute::eNurbs ||
		pAttr->GetAttributeType() == FbxNodeAttribute::eNurbsSurface))
	{
		FbxGeometryConverter	converter(m_pManager);

		// 씬 내에서 삼각형와 할 수 있는 모든 노드들을 삼각형화 시킨다.
		converter.Triangulate(pAttr, true);
	}

	// 재질정보를 읽는다.
	int	iMtrlCount = pNode->GetMaterialCount();

	// 재질 정보가 있다면
	if (iMtrlCount > 0)
	{
		std::vector<PFBXMATERIAL>	vecMtrl;

		m_vecMaterials.push_back(vecMtrl);

		for (int i = 0; i < iMtrlCount; ++i)
		{
			// FbxSurfaceMaterial : Fbx 가 가지고 있는 재질 정보를
			// 담고있다.
			FbxSurfaceMaterial*	pMtrl = pNode->GetMaterial(i);

			if (!pMtrl)
				continue;

			LoadMaterial(pMtrl);
		}
	}

	// 현재 노드의 자식노드 수를 얻어온다.
	int	iChildCount = pNode->GetChildCount();

	for (int i = 0; i < iChildCount; ++i)
	{
		Triangulate(pNode->GetChild(i));
	}
}

void FBXLoader::LoadMaterial(FbxSurfaceMaterial * pMtrl)
{
	// 재질 정보를 저장할 구조체를 생성한다.
	PFBXMATERIAL	pMtrlInfo = new FBXMATERIAL;

	// 가장 마지막 컨테이너에 재질 정보를 추가한다.
	m_vecMaterials[m_vecMaterials.size() - 1].push_back(pMtrlInfo);

	// Diffuse 정보를 읽어온다.
	pMtrlInfo->vDiffuse = GetMaterialColor(pMtrl, FbxSurfaceMaterial::sDiffuse,
		FbxSurfaceMaterial::sDiffuseFactor);

	pMtrlInfo->vAmbient = GetMaterialColor(pMtrl, FbxSurfaceMaterial::sAmbient,
		FbxSurfaceMaterial::sAmbientFactor);

	pMtrlInfo->vSpecular = GetMaterialColor(pMtrl, FbxSurfaceMaterial::sSpecular,
		FbxSurfaceMaterial::sSpecularFactor);

	pMtrlInfo->vEmissive = GetMaterialColor(pMtrl, FbxSurfaceMaterial::sEmissive,
		FbxSurfaceMaterial::sEmissiveFactor);

	pMtrlInfo->fSpecularPower = (float)(GetMaterialFactor(pMtrl, FbxSurfaceMaterial::sSpecularFactor));
	pMtrlInfo->fShininess = (float)(GetMaterialFactor(pMtrl, FbxSurfaceMaterial::sShininess));
	pMtrlInfo->fTransparencyFactor = (float)(GetMaterialFactor(pMtrl, FbxSurfaceMaterial::sTransparencyFactor));

	// 텍스처 이름 가져옴
	pMtrlInfo->strDifTex = GetMaterialTexture(pMtrl, FbxSurfaceMaterial::sDiffuse);
	pMtrlInfo->strBumpTex = GetMaterialTexture(pMtrl, FbxSurfaceMaterial::sNormalMap);

	if (pMtrlInfo->strBumpTex.empty())
		pMtrlInfo->strBumpTex = GetMaterialTexture(pMtrl, FbxSurfaceMaterial::sBump);

	pMtrlInfo->strSpcTex = GetMaterialTexture(pMtrl, FbxSurfaceMaterial::sSpecular);
}

Vector4 FBXLoader::GetMaterialColor(FbxSurfaceMaterial * pMtrl, const char * pPropertyName, const char * pPropertyFactorName)
{
	FbxDouble3	vResult(0, 0, 0);
	double		dFactor = 0;

	// 인자로 들어온 재질에서 해당 이름을 가진 프로퍼티가 있는지를 판단한다음
	// 얻어온다. 마찬가지로 해당 이름의 프로퍼티 팩터가 있는지를 판단한다음
	// 얻어온다.
	FbxProperty	tProperty = pMtrl->FindProperty(pPropertyName);
	FbxProperty	tPropertyFactor = pMtrl->FindProperty(pPropertyFactorName);

	// 유효한지 체크한다.
	if (tProperty.IsValid() && tPropertyFactor.IsValid())
	{
		vResult = tProperty.Get<FbxDouble3>();
		dFactor = tPropertyFactor.Get<FbxDouble>();

		// Factor가 1이 아닐 경우 일종의 비율값으로 사용하여
		// 얻어온 vResult 에 모두 곱해준다.
		if (dFactor != 1)
		{
			vResult[0] *= dFactor;
			vResult[1] *= dFactor;
			vResult[2] *= dFactor;
		}
	}

	return Vector4((float)vResult[0], (float)vResult[1], (float)vResult[2], (float)dFactor);
}

double FBXLoader::GetMaterialFactor(FbxSurfaceMaterial * pMtrl, const char * pPropertyName)
{
	FbxProperty	tProperty = pMtrl->FindProperty(pPropertyName);
	double	dFactor = 0.0;

	if (tProperty.IsValid())
		dFactor = tProperty.Get<FbxDouble>();

	return dFactor;
}

std::string FBXLoader::GetMaterialTexture(FbxSurfaceMaterial * pMtrl, const char * pPropertyName)
{
	FbxProperty	tProperty = pMtrl->FindProperty(pPropertyName);

	std::string	str = "";

	if (tProperty.IsValid())
	{
		// FbxFileTexture 이 타입이 몇개가 있는지를 얻어오는 함수이다.
		int	iTexCount = tProperty.GetSrcObjectCount<FbxFileTexture>();

		if (iTexCount > 0)
		{
			FbxFileTexture*	pFileTex = tProperty.GetSrcObject<FbxFileTexture>(0);

			if (pFileTex)
				str = pFileTex->GetFileName();
		}
	}

	return str;
}

void FBXLoader::LoadMesh(FbxNode * pNode)
{
	FbxNodeAttribute*	pAttr = pNode->GetNodeAttribute();

	if (pAttr && pAttr->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		FbxMesh*	pMesh = pNode->GetMesh();

		if (nullptr != pMesh)
			LoadMesh(pMesh);
	}

	int	iChildCount = pNode->GetChildCount();

	for (int i = 0; i < iChildCount; ++i)
	{
		LoadMesh(pNode->GetChild(i));
	}
}

void FBXLoader::LoadMesh(FbxMesh * pMesh)
{
	// MeshContainer를 만들고 컨테이너별 정점과 컨테이너&서브셋 별
	// 인덱스 정보를 얻어와서 메쉬를 구성해야 한다.
	PFBXMESHCONTAINER	pContainer = new FBXMESHCONTAINER;

	// 범프는 일단 false를 한다.
	// 노말맵이 있는경우에 true로 변경해줄거임
	pContainer->bBump = false;

	m_vecMeshContainer.push_back(pContainer);

	// ControlPoint 는 위치정보를 담고 있는 배열이다. ( 정점이라고 생각하면 됨 )
	// 이 배열의 개수는 곧 정점의 개수가 된다.
	int	iVtxCount = pMesh->GetControlPointsCount();

	// 내부적으로 FbxVector4타입의 배열로 저장하고 있기 때문에 배열의 
	// 시작주소를 얻어온다.
	FbxVector4*	pVtxPos = pMesh->GetControlPoints();

	// 컨테이너가 가지고 있는 정점 정보들을 정점수만큼 resize 해준다.
	pContainer->vecPosition.resize(iVtxCount);
	pContainer->vecNormal.resize(iVtxCount);
	pContainer->vecUV.resize(iVtxCount);
	pContainer->vecTangent.resize(iVtxCount);
	pContainer->vecBinormal.resize(iVtxCount);

	for (int i = 0; i < iVtxCount; ++i)
	{
		// y와 z축이 바뀌어 있기 때문에 변경해준다.
		pContainer->vecPosition[i].x = (float)(pVtxPos[i].mData[0]);
		pContainer->vecPosition[i].y = (float)(pVtxPos[i].mData[2]);
		pContainer->vecPosition[i].z = (float)(pVtxPos[i].mData[1]);
	}

	// 폴리곤 수를 얻어온다.
	int	iPolygonCount = pMesh->GetPolygonCount();

	UINT	iVtxID = 0;

	// 재질 수를 얻어온다.
	int	iMtrlCount = pMesh->GetNode()->GetMaterialCount();

	// 재질 수는 곧 서브셋 수이기 때문에 재질 수만큼 resize 한다.
	pContainer->vecIndices.resize(iMtrlCount);

	// 재질 정보를 얻어온다.
	FbxGeometryElementMaterial*	pMaterial = pMesh->GetElementMaterial();
	int iCount = pMesh->GetElementMaterialCount();
	// 삼각형 수만큼 반복한다.
	for (int i = 0; i < iPolygonCount; ++i)
	{
		// 이 폴리곤을 구성하는 정점의 수를 얻어온다.
		// 삼각형일 경우 3개를 얻어온다.
		int	iPolygonSize = pMesh->GetPolygonSize(i);			// 3개.. 4개이면 ㅇㅅㅇ 이상한거

		// 삼각형이니까 인덱스가 총 3개임 ㅇㅅㅇ
		int	iIdx[3] = {};

		for (int j = 0; j < iPolygonSize; ++j)
		{
			// 현재 삼각형을 구성하고 있는 버텍스정보 내에서의 인덱스를
			// 구한다.
			int	iControlIndex = pMesh->GetPolygonVertex(i, j);

			iIdx[j] = iControlIndex;

			LoadNormal(pMesh, pContainer, iVtxID, iControlIndex);

			LoadUV(pMesh, pContainer, pMesh->GetTextureUVIndex(i, j),
				iControlIndex);

			LoadTangent(pMesh, pContainer, iVtxID, iControlIndex);

			LoadBinormal(pMesh, pContainer, iVtxID, iControlIndex);

			++iVtxID;
		}

		int	iMtrlID = pMaterial->GetIndexArray().GetAt(i);

		pContainer->vecIndices[iMtrlID].push_back(iIdx[0]);
		pContainer->vecIndices[iMtrlID].push_back(iIdx[2]);
		pContainer->vecIndices[iMtrlID].push_back(iIdx[1]);
	}

	// 애니메이션 로드하기
	LoadAnimation(pMesh, pContainer);
}

void FBXLoader::LoadNormal(FbxMesh * pMesh, PFBXMESHCONTAINER pContainer, int iVtxID, int iControlIndex)
{
	// 메쉬로부터 ElementNormal 정보를 얻어온다.
	FbxGeometryElementNormal*	pNormal = pMesh->GetElementNormal();

	int	iNormalIndex = iVtxID;

	// MappingMode와 ReferenceMode에 따라서 인덱스로 사용할 정보가 달라진다.
	// Mapping mode 는 index 를 구하는 방법을 가르쳐 주고
	// Reference Mode 는 data 가 들어 있는 곳을 가르쳐 줌
	if (pNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		switch (pNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eIndexToDirect:
			iNormalIndex = pNormal->GetIndexArray().GetAt(iVtxID);
			break;
		}
	}

	else if (pNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		switch (pNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			iNormalIndex = iControlIndex;
			break;
		case FbxGeometryElement::eIndexToDirect:
			iNormalIndex = pNormal->GetIndexArray().GetAt(iControlIndex);
			break;
		}
	}

	FbxVector4	vNormal = pNormal->GetDirectArray().GetAt(iNormalIndex);

	pContainer->vecNormal[iControlIndex].x = (float)(vNormal.mData[0]);
	pContainer->vecNormal[iControlIndex].y = (float)(vNormal.mData[2]);
	pContainer->vecNormal[iControlIndex].z = (float)(vNormal.mData[1]);
}

void FBXLoader::LoadUV(FbxMesh * pMesh, PFBXMESHCONTAINER pContainer, int iUVID, int iControlIndex)
{
	int iCount = pMesh->GetElementUVCount();
	FbxGeometryElementUV*	pUV = pMesh->GetElementUV(0);

	if (!pUV)
		return;

	int	iUVIndex = iUVID;

	if (pUV->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		switch (pUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			iUVIndex = iControlIndex;
			break;
		case FbxGeometryElement::eIndexToDirect:
			iUVIndex = pUV->GetIndexArray().GetAt(iControlIndex);
			break;
		}
	}

	else if (pUV->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		switch (pUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		case FbxGeometryElement::eIndexToDirect:
		{
			//iUVIndex = pUV->GetIndexArray().GetAt(iUVIndex);
		}
		break;
		default:
			break; // other reference modes not shown here!
		}
	}

	FbxVector2	vUV = pUV->GetDirectArray().GetAt(iUVIndex);

	// 가끔 UV값이 0~1이아닌 1를 초과한 값이 들어오는데..
	// DirectX UV는 가로세로 0~1인데 FBX에서 가져오는 값은 
	// Y축이 1~0으로.. (즉 반전)으로 되어있다.
	// 그래서 y값을 구할때 1.0f를 빼는 이유가 이 이유에서이다.
	pContainer->vecUV[iControlIndex].x = (float)(vUV.mData[0] - (int)vUV.mData[0]);
	pContainer->vecUV[iControlIndex].y = (float)(1.f - (vUV.mData[1] - (int)vUV.mData[1]));
}

void FBXLoader::LoadTangent(FbxMesh * pMesh, PFBXMESHCONTAINER pContainer, int iVtxID, int iControlIndex)
{
	FbxGeometryElementTangent*	pTangent = pMesh->GetElementTangent();

	if (!pTangent)
		return;

	pContainer->bBump = true;

	int	iTangentIndex = iVtxID;

	if (pTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		switch (pTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eIndexToDirect:
			iTangentIndex = pTangent->GetIndexArray().GetAt(iVtxID);
			break;
		}
	}

	else if (pTangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		switch (pTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			iTangentIndex = iControlIndex;
			break;
		case FbxGeometryElement::eIndexToDirect:
			iTangentIndex = pTangent->GetIndexArray().GetAt(iControlIndex);
			break;
		}
	}

	FbxVector4	vTangent = pTangent->GetDirectArray().GetAt(iTangentIndex);

	pContainer->vecTangent[iControlIndex].x = (float)(vTangent.mData[0]);
	pContainer->vecTangent[iControlIndex].y = (float)(vTangent.mData[2]);
	pContainer->vecTangent[iControlIndex].z = (float)(vTangent.mData[1]);
}

void FBXLoader::LoadBinormal(FbxMesh * pMesh, PFBXMESHCONTAINER pContainer, int iVtxID, int iControlIndex)
{
	FbxGeometryElementBinormal*	pBinormal = pMesh->GetElementBinormal();

	if (!pBinormal)
		return;

	pContainer->bBump = true;

	int	iBinormalIndex = iVtxID;

	if (pBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		switch (pBinormal->GetReferenceMode())
		{
		case FbxGeometryElement::eIndexToDirect:
			iBinormalIndex = pBinormal->GetIndexArray().GetAt(iVtxID);
			break;
		}
	}

	else if (pBinormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		switch (pBinormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			iBinormalIndex = iControlIndex;
			break;
		case FbxGeometryElement::eIndexToDirect:
			iBinormalIndex = pBinormal->GetIndexArray().GetAt(iControlIndex);
			break;
		}
	}

	FbxVector4	vBinormal = pBinormal->GetDirectArray().GetAt(iBinormalIndex);

	pContainer->vecBinormal[iControlIndex].x = (float)(vBinormal.mData[0]);
	pContainer->vecBinormal[iControlIndex].y = (float)(vBinormal.mData[2]);
	pContainer->vecBinormal[iControlIndex].z = (float)(vBinormal.mData[1]);
}

void FBXLoader::LoadAnimationClip()
{
	int	iCount = m_NameArr.GetCount();				// 애니메이션 이름 개수

	FbxTime::EMode	eTimeMode = m_pScene->GetGlobalSettings().GetTimeMode();		// 보통 30프레임임

	for (int i = 0; i < iCount; ++i)
	{
		// m_NameArr에 저장된 이름으로 Scene으로부터 FbxAnimStack 객체를 얻어온다.
		FbxAnimStack*	pAnimStack = m_pScene->FindMember<FbxAnimStack>(m_NameArr[i]->Buffer());

		if (!pAnimStack)
			continue;

		PFBXANIMATIONCLIP	pClip = new FBXANIMATIONCLIP;

		pClip->strName = pAnimStack->GetName();

		if (pClip->strName == "mixamo.com")
			m_bMixamo = true;

		// FbxTakeInfo : 애니메이션 클립정보
		FbxTakeInfo*	pTake = m_pScene->GetTakeInfo(pClip->strName.c_str());

		pClip->tStart = pTake->mLocalTimeSpan.GetStart();
		pClip->tEnd = pTake->mLocalTimeSpan.GetStop();
		// GetFrameCount 함수를 호출하고  time모드를 넣어주면 시간을 프레임으로
		// 변환해준다. 몇프레임 짜리 애니메이션 인지를 구해준다.
		pClip->lTimeLength = pClip->tEnd.GetFrameCount(eTimeMode) -
			pClip->tStart.GetFrameCount(eTimeMode);
		pClip->eTimeMode = eTimeMode;

		m_vecClip.push_back(pClip);
	}
}

void FBXLoader::LoadBone(FbxNode * pNode)
{
	int	iChildCount = pNode->GetChildCount();

	for (int i = 0; i < iChildCount; ++i)
	{
		LoadBoneRecursive(pNode->GetChild(i), 0, 0, -1);
	}
}

void FBXLoader::LoadBoneRecursive(FbxNode * pNode, int iDepth, int iIndex, int iParent)
{
	FbxNodeAttribute*	pAttr = pNode->GetNodeAttribute();

	if (pAttr && pAttr->GetAttributeType() ==
		FbxNodeAttribute::eSkeleton)
	{
		PFBXBONE	pBone = new FBXBONE;

		pBone->strName = pNode->GetName();
		if (m_bMixamo)
			pBone->strName.erase(0, 10);
		pBone->iDepth = iDepth;
		pBone->iParentIndex = iParent;

		m_vecBones.push_back(pBone);
	}

	int	iChildCount = pNode->GetChildCount();

	for (int i = 0; i < iChildCount; ++i)
	{
		LoadBoneRecursive(pNode->GetChild(i), iDepth + 1,
			(int)(m_vecBones.size()), iIndex);
	}
}

void FBXLoader::LoadAnimation(FbxMesh * pMesh, PFBXMESHCONTAINER pContainer)
{
	// Deformer : 타입
	// 여기선 스킨 타입을 가져온다.
	int	iSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);		// 스킨개수 가져오기

	// 스킨이 없다면 애니메이션 로드할 필요 X
	if (iSkinCount <= 0)
	{
		return;
	}

	// 메쉬의 정점 수를 얻어온다.
	// ControlPoints는 정점이라고 생각하면 된다.
	int	iCPCount = pMesh->GetControlPointsCount();

	// 정점의 가중치 정보와 본인덱스 정보는 정점 수만큼
	// 만들어져야 한다.
	pContainer->vecBlendWeight.resize(iCPCount);
	pContainer->vecBlendIndex.resize(iCPCount);

	pContainer->bAnimation = true;
	FbxAMatrix	matTransform = GetTransform(pMesh->GetNode());

	for (int i = 0; i < iSkinCount; ++i)
	{
		FbxSkin*	pSkin = (FbxSkin*)pMesh->GetDeformer(i, FbxDeformer::eSkin);

		if (!pSkin)
			continue;

		FbxSkin::EType	eSkinningType = pSkin->GetSkinningType();

		if (eSkinningType == FbxSkin::eRigid ||
			eSkinningType == FbxSkin::eLinear || eSkinningType == FbxSkin::eBlend)
		{
			// Cluster : 관절을 의미한다.
			int	iClusterCount = pSkin->GetClusterCount();

			for (int j = 0; j < iClusterCount; ++j)
			{
				FbxCluster*	pCluster = pSkin->GetCluster(j);

				if (!pCluster->GetLink())
					continue;

				// 해당 본 이름 가져오기 ( 해당 본은 관절에 연결되어 있다. )
				std::string	strBoneName = pCluster->GetLink()->GetName();

				if("R_Sword" == strBoneName)
				{
					int a = 0;
				}

				if ("Skirt_B_02" == strBoneName)
				{
					int b = 0;
				}
				
				// 믹사모 사이트에서 애니메이션을 뽑아왔을 경우
				// 모든본 이름 앞에 mixamo.com이 붙어서..
				// 지워준다.
				if (m_bMixamo)
					strBoneName.erase(0, 10);

				// 해당 본의 인덱스 가져오기
				int	iBoneIndex = FindBoneFromName(strBoneName);


				// 가중치와 Indecies 로드한다.
				LoadWeightAndIndex(pCluster, iBoneIndex, pContainer);

				// 오프셋 행렬을 로드한다. ( 오프셋 행렬을 필수다. )
				LoadOffsetMatrix(pCluster, matTransform, iBoneIndex, pContainer);

				// 해당 본 행렬 넣어주기 (원본행렬임 나중에 오프셋 행렬을 곱해줄거임)
				m_vecBones[iBoneIndex]->matBone = matTransform;

				LoadTimeTransform(pMesh->GetNode(), pCluster,
					matTransform, iBoneIndex);
			}
		}
		//else if (eSkinningType == FbxSkin::eDualQuaternion)
		//{

		//}
		//else if (eSkinningType == FbxSkin::eBlend)
		//{

		//}
	}

	ChangeWeightAndIndices(pContainer);
}

FbxAMatrix FBXLoader::GetTransform(FbxNode * pNode)
{
	// 노드에 맞는 행렬 가져오기
	const FbxVector4	vT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4	vR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4	vS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(vT, vR, vS);
}

int FBXLoader::FindBoneFromName(const std::string & strName)
{
	// 본 이름에 맞는 본을 찾으면 해당 본의 인덱스를 넘긴다.
	for (size_t i = 0; i < m_vecBones.size(); ++i)
	{
		if (m_vecBones[i]->strName == strName)
			return (int)i;
	}
	// 인덱스가 없으면 -1리턴
	return -1;
}

void FBXLoader::LoadWeightAndIndex(FbxCluster * pCluster,
	int iBoneIndex, PFBXMESHCONTAINER pContainer)
{
	// Control Point(정점)의 인덱스개수를 가져온다.
	// 왜냐면 정점마다 가중치가 있으니까 ㅇㅅㅇ
	int	iControlIndicesCount = pCluster->GetControlPointIndicesCount();

	for (int i = 0; i < iControlIndicesCount; ++i)
	{
		FBXWEIGHT	tWeight = {};

		tWeight.iIndex = iBoneIndex;			// 본 인덱스 ( 관절과 연결된 본 인덱스인듯 )
		tWeight.dWeight = pCluster->GetControlPointWeights()[i];	// 가중치

		int	iClusterIndex = pCluster->GetControlPointIndices()[i];

		// map의 특징 : 키를 이용해 인덱스처럼 접근할 경우 해당 키가 없다면
		// 만들어준다.
		pContainer->mapWeights[iClusterIndex].push_back(tWeight);
	}
}

void FBXLoader::LoadOffsetMatrix(FbxCluster * pCluster, const FbxAMatrix & matTransform, int iBoneIndex, PFBXMESHCONTAINER pContainer)
{
	FbxAMatrix	matCluster;
	FbxAMatrix	matClusterLink;

	pCluster->GetTransformMatrix(matCluster);			// 클러스터 행렬
	pCluster->GetTransformLinkMatrix(matClusterLink);	// 관절과 연결된 행렬

	// 오프셋 행렬(3D Max -> DirectX 에서 사용하는 행렬)을 만들기 위해선
	// Reflect행렬이 필요하다.
	// 해당 클러스터 행렬을 가져오면 3D Max상의 기준축을 기준으로한 행렬이 나오는데
	// DirectX에서 사용할 수 있게끔 원본 행렬의 Y축과 Z축을 바꾸기 위해 곱해준다.(Reflect행렬을)
	// 즉 Reflect행렬은 YZ축을 바꾸기 위한 행렬이다.
	FbxVector4	v1 = { 1.0, 0.0, 0.0, 0.0 };
	FbxVector4	v2 = { 0.0, 0.0, 1.0, 0.0 };
	FbxVector4	v3 = { 0.0, 1.0, 0.0, 0.0 };
	FbxVector4	v4 = { 0.0, 0.0, 0.0, 1.0 };

	FbxAMatrix	matReflect;
	matReflect.mData[0] = v1;
	matReflect.mData[1] = v2;
	matReflect.mData[2] = v3;
	matReflect.mData[3] = v4;

	/*
	1 0 0 0   1 2 3 4   1 0 0 0
	0 0 1 0   5 6 7 8   0 0 1 0
	0 1 0 0 * 9 0 1 2 * 0 1 0 0
	0 0 0 1   3 4 5 6   0 0 0 1

	1 2 3 4   1 0 0 0
	9 0 1 2   0 0 1 0
	5 6 7 8 * 0 1 0 0
	3 4 5 6   0 0 0 1

	1 3 2 4
	9 1 0 2
	5 7 6 8
	3 5 4 6
	*/

	FbxAMatrix	matOffset;
	matOffset = matClusterLink.Inverse() * matCluster * matTransform;		// 오프셋 행렬을 구하는 공식임 ( Autodesk에서 이렇게 정했음 )
	matOffset = matReflect * matOffset * matReflect;

	m_vecBones[iBoneIndex]->matOffset = matOffset;
}

void FBXLoader::LoadTimeTransform(FbxNode * pNode, FbxCluster * pCluster, const FbxAMatrix & matTransform, int iBoneIndex)
{
	// Reflect 행렬
	FbxVector4	v1 = { 1.0, 0.0, 0.0, 0.0 };
	FbxVector4	v2 = { 0.0, 0.0, 1.0, 0.0 };
	FbxVector4	v3 = { 0.0, 1.0, 0.0, 0.0 };
	FbxVector4	v4 = { 0.0, 0.0, 0.0, 1.0 };

	FbxAMatrix	matReflect;
	matReflect.mData[0] = v1;
	matReflect.mData[1] = v2;
	matReflect.mData[2] = v3;
	matReflect.mData[3] = v4;

	if (m_bMixamo)
	{
		// 애니메이션이 믹사모로 만든 애니메이션일 경우
		// mixamo.com이 안붙여진것들은 지운다.
		std::vector<PFBXANIMATIONCLIP>::iterator	iter = m_vecClip.begin();
		std::vector<PFBXANIMATIONCLIP>::iterator	iterEnd = m_vecClip.end();

		for (; iter != iterEnd;)
		{
			if ((*iter)->strName != "mixamo.com")
			{
				SAFE_DELETE((*iter));
				iter = m_vecClip.erase(iter);
				iterEnd = m_vecClip.end();
			}

			else
				++iter;
		}
	}

	for (size_t i = 0; i < m_vecClip.size(); ++i)
	{
		FbxLongLong	Start = m_vecClip[i]->tStart.GetFrameCount(m_vecClip[i]->eTimeMode);
		FbxLongLong	End = m_vecClip[i]->tEnd.GetFrameCount(m_vecClip[i]->eTimeMode);

		m_vecClip[i]->vecBoneKeyFrame[iBoneIndex].iBoneIndex = iBoneIndex;

		// 전체 프레임 수만큼 반복한다.
		for (FbxLongLong j = Start; j <= End; ++j)
		{
			FbxTime	tTime = {};

			// 현재 프레임에 해당하는 FbxTime을 만들어낸다.
			tTime.SetFrame(j, m_vecClip[i]->eTimeMode);

			// EvaluateGlobalTransform
			FbxAMatrix	matOffset = pNode->EvaluateGlobalTransform(tTime) * matTransform;
			FbxAMatrix	matCur = matOffset.Inverse() * pCluster->GetLink()->EvaluateGlobalTransform(tTime);

			matCur = matReflect * matCur * matReflect;

			FBXKEYFRAME	tKeyFrame = {};

			tKeyFrame.dTime = tTime.GetSecondDouble();
			tKeyFrame.matTransform = matCur;

			m_vecClip[i]->vecBoneKeyFrame[iBoneIndex].vecKeyFrame.push_back(tKeyFrame);
		}
	}
}

void FBXLoader::ChangeWeightAndIndices(PFBXMESHCONTAINER pContainer)
{
	std::unordered_map<int, std::vector<FBXWEIGHT>>::iterator	iter = pContainer->mapWeights.begin();
	std::unordered_map<int, std::vector<FBXWEIGHT>>::iterator	iterEnd = pContainer->mapWeights.end();

	for (; iter != iterEnd; ++iter)
	{
		// 가중치는 최대 4개를 갖게한다. (Vector4라서..)  모든합은 1을 넘지 않는다.
		// 만약 4개를 초과할 경우 내림차순으로 정렬한 뒤 뒤에껄 자른다. ㅇㅅㅇ
		if (iter->second.size() > 4)
		{
			// 가중치 값에 따라 내림차순 정렬한다.
			sort(iter->second.begin(), iter->second.end(), [](const FBXWEIGHT& lhs, const FBXWEIGHT& rhs)
			{
				return lhs.dWeight > rhs.dWeight;
			});

			double	dSum = 0.0;

			for (int i = 0; i < 4; ++i)
			{
				dSum += iter->second[i].dWeight;
			}

			double	dInterpolate = 1.f - dSum;

			// 4개를 초과할 경우 그 이후의 것들부터 시작해서 삭제한다.
			std::vector<FBXWEIGHT>::iterator	iterErase = iter->second.begin() + 4;

			iter->second.erase(iterErase, iter->second.end());		// 4개 초과하는애들은 삭제
			iter->second[0].dWeight += dInterpolate;
		}

		float	fWeight[4] = {};
		int		iIndex[4] = {};

		for (int i = 0; i < (int)(iter->second.size()); ++i)
		{
			fWeight[i] = (float)(iter->second[i].dWeight);
			iIndex[i] = (int)(iter->second[i].iIndex);
		}

		Vector4	vWeight = fWeight;
		Vector4	vIndex = iIndex;

		pContainer->vecBlendWeight[iter->first] = vWeight;
		pContainer->vecBlendIndex[iter->first] = vIndex;
	}
}
