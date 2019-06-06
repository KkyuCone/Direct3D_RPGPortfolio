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
	// FBX SDK�� ��κ��� ��ü���� FbxManager �̱� ���� ����Ͽ� �����ǰ� ������

	// FbxManager ��ü�� �����Ѵ�.
	m_pManager = FbxManager::Create();

	// IOSetting�� �����Ѵ�.
#pragma region I/O ���� ��ü �����
	// FbxIOSettings Ŭ������ ���� ��Ҹ� ���Ͽ��� �������ų� ���Ϸ� �������� ���θ� �����Ѵ�.
	// �̷��� ��ҿ� Camera, Light, Mesh, Texture, Material, Animation, ����� ���� �Ӽ� ���� ���Ե�.
	// FbxIOSettings ��ü�� FbxImporter �Ǵ� FbxExporter ��ü�� ���޵Ǳ� ���� �ν��Ͻ�ȭ �ϰ� �����Ǿ�� ��.
#pragma endregion
	FbxIOSettings*	pIos = FbxIOSettings::Create(m_pManager, IOSROOT);

	// FbxManager�� �����Ѵ�.
	m_pManager->SetIOSettings(pIos);

	// FbxScene�� �����Ѵ�.
	// Importer �ʱ�ȭ�Ǹ� ���Ͽ��� ���� �ε��ϱ� ���ؼ� �� �����̳ʸ� ��������
	m_pScene = FbxScene::Create(m_pManager, "");

	// FbxImporter �� �̿��ؼ� fbx ���Ϸκ��� �޽� ������ �о�´�.
	FbxImporter*	pImporter = FbxImporter::Create(m_pManager, "");

	// �ش� ��ο� �ִ� fbx ������ �о���� ������ ������.
	// pImporter->Initialize ( ���������� Fbx ���ϸ�, -1�� FBX, ���õǾ��ִ�..(����� �о���°���))
	pImporter->Initialize(pFullPath, -1, m_pManager->GetIOSettings());

	// ������ ���� ������ FbxScene�� ��带 �����Ѵ�.
	pImporter->Import(m_pScene);

	// Global Scene Settings
	// ���� �� �ý���, �ֺ� ���� �� �ð� ������ �ش� FbxGlobalSettings ��ü�� ���ǵ�
	// �� ��ü�� FbxScene::GetGlobalSettings()�� ���� ������
	if (m_pScene->GetGlobalSettings().GetAxisSystem() != FbxAxisSystem::Max)
		m_pScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::Max);

	m_pScene->FillAnimStackNameArray(m_NameArr);

	if (m_NameArr.GetCount() > 0)
	{
		// �ִϸ��̼� Ŭ�� ������ �о�´�.
		LoadAnimationClip();

		// Bone ������ �о�´�.
		LoadBone(m_pScene->GetRootNode());

		// �ִϸ��̼� Ŭ���� ������ �ִ� �������� �� ����ŭ resize���ش�.
		for (size_t i = 0; i < m_vecClip.size(); ++i)
		{
			m_vecClip[i]->vecBoneKeyFrame.resize(m_vecBones.size());
		}
	}

	// �ﰢȭ
	Triangulate(m_pScene->GetRootNode());

	LoadMesh(m_pScene->GetRootNode());

	pImporter->Destroy();

	return true;
}

void FBXLoader::Triangulate(FbxNode * pNode)
{
	// FbxNodeAttribute : FbxNode �Ӽ��� ��Ÿ���� �������̽��̴�.
	// ���κ��� ���� �� �ִ�.
	FbxNodeAttribute*	pAttr = pNode->GetNodeAttribute();

	// Nurbs : Non-Uniform Rational B-Spline (NURBS)
	if (pAttr && (pAttr->GetAttributeType() == FbxNodeAttribute::eMesh ||
		pAttr->GetAttributeType() == FbxNodeAttribute::eNurbs ||
		pAttr->GetAttributeType() == FbxNodeAttribute::eNurbsSurface))
	{
		FbxGeometryConverter	converter(m_pManager);

		// �� ������ �ﰢ���� �� �� �ִ� ��� ������ �ﰢ��ȭ ��Ų��.
		converter.Triangulate(pAttr, true);
	}

	// ���������� �д´�.
	int	iMtrlCount = pNode->GetMaterialCount();

	// ���� ������ �ִٸ�
	if (iMtrlCount > 0)
	{
		std::vector<PFBXMATERIAL>	vecMtrl;

		m_vecMaterials.push_back(vecMtrl);

		for (int i = 0; i < iMtrlCount; ++i)
		{
			// FbxSurfaceMaterial : Fbx �� ������ �ִ� ���� ������
			// ����ִ�.
			FbxSurfaceMaterial*	pMtrl = pNode->GetMaterial(i);

			if (!pMtrl)
				continue;

			LoadMaterial(pMtrl);
		}
	}

	// ���� ����� �ڽĳ�� ���� ���´�.
	int	iChildCount = pNode->GetChildCount();

	for (int i = 0; i < iChildCount; ++i)
	{
		Triangulate(pNode->GetChild(i));
	}
}

void FBXLoader::LoadMaterial(FbxSurfaceMaterial * pMtrl)
{
	// ���� ������ ������ ����ü�� �����Ѵ�.
	PFBXMATERIAL	pMtrlInfo = new FBXMATERIAL;

	// ���� ������ �����̳ʿ� ���� ������ �߰��Ѵ�.
	m_vecMaterials[m_vecMaterials.size() - 1].push_back(pMtrlInfo);

	// Diffuse ������ �о�´�.
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

	// �ؽ�ó �̸� ������
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

	// ���ڷ� ���� �������� �ش� �̸��� ���� ������Ƽ�� �ִ����� �Ǵ��Ѵ���
	// ���´�. ���������� �ش� �̸��� ������Ƽ ���Ͱ� �ִ����� �Ǵ��Ѵ���
	// ���´�.
	FbxProperty	tProperty = pMtrl->FindProperty(pPropertyName);
	FbxProperty	tPropertyFactor = pMtrl->FindProperty(pPropertyFactorName);

	// ��ȿ���� üũ�Ѵ�.
	if (tProperty.IsValid() && tPropertyFactor.IsValid())
	{
		vResult = tProperty.Get<FbxDouble3>();
		dFactor = tPropertyFactor.Get<FbxDouble>();

		// Factor�� 1�� �ƴ� ��� ������ ���������� ����Ͽ�
		// ���� vResult �� ��� �����ش�.
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
		// FbxFileTexture �� Ÿ���� ��� �ִ����� ������ �Լ��̴�.
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
	// MeshContainer�� ����� �����̳ʺ� ������ �����̳�&����� ��
	// �ε��� ������ ���ͼ� �޽��� �����ؾ� �Ѵ�.
	PFBXMESHCONTAINER	pContainer = new FBXMESHCONTAINER;

	// ������ �ϴ� false�� �Ѵ�.
	// �븻���� �ִ°�쿡 true�� �������ٰ���
	pContainer->bBump = false;

	m_vecMeshContainer.push_back(pContainer);

	// ControlPoint �� ��ġ������ ��� �ִ� �迭�̴�. ( �����̶�� �����ϸ� �� )
	// �� �迭�� ������ �� ������ ������ �ȴ�.
	int	iVtxCount = pMesh->GetControlPointsCount();

	// ���������� FbxVector4Ÿ���� �迭�� �����ϰ� �ֱ� ������ �迭�� 
	// �����ּҸ� ���´�.
	FbxVector4*	pVtxPos = pMesh->GetControlPoints();

	// �����̳ʰ� ������ �ִ� ���� �������� ��������ŭ resize ���ش�.
	pContainer->vecPosition.resize(iVtxCount);
	pContainer->vecNormal.resize(iVtxCount);
	pContainer->vecUV.resize(iVtxCount);
	pContainer->vecTangent.resize(iVtxCount);
	pContainer->vecBinormal.resize(iVtxCount);

	for (int i = 0; i < iVtxCount; ++i)
	{
		// y�� z���� �ٲ�� �ֱ� ������ �������ش�.
		pContainer->vecPosition[i].x = (float)(pVtxPos[i].mData[0]);
		pContainer->vecPosition[i].y = (float)(pVtxPos[i].mData[2]);
		pContainer->vecPosition[i].z = (float)(pVtxPos[i].mData[1]);
	}

	// ������ ���� ���´�.
	int	iPolygonCount = pMesh->GetPolygonCount();

	UINT	iVtxID = 0;

	// ���� ���� ���´�.
	int	iMtrlCount = pMesh->GetNode()->GetMaterialCount();

	// ���� ���� �� ����� ���̱� ������ ���� ����ŭ resize �Ѵ�.
	pContainer->vecIndices.resize(iMtrlCount);

	// ���� ������ ���´�.
	FbxGeometryElementMaterial*	pMaterial = pMesh->GetElementMaterial();
	int iCount = pMesh->GetElementMaterialCount();
	// �ﰢ�� ����ŭ �ݺ��Ѵ�.
	for (int i = 0; i < iPolygonCount; ++i)
	{
		// �� �������� �����ϴ� ������ ���� ���´�.
		// �ﰢ���� ��� 3���� ���´�.
		int	iPolygonSize = pMesh->GetPolygonSize(i);			// 3��.. 4���̸� ������ �̻��Ѱ�

		// �ﰢ���̴ϱ� �ε����� �� 3���� ������
		int	iIdx[3] = {};

		for (int j = 0; j < iPolygonSize; ++j)
		{
			// ���� �ﰢ���� �����ϰ� �ִ� ���ؽ����� �������� �ε�����
			// ���Ѵ�.
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

	// �ִϸ��̼� �ε��ϱ�
	LoadAnimation(pMesh, pContainer);
}

void FBXLoader::LoadNormal(FbxMesh * pMesh, PFBXMESHCONTAINER pContainer, int iVtxID, int iControlIndex)
{
	// �޽��κ��� ElementNormal ������ ���´�.
	FbxGeometryElementNormal*	pNormal = pMesh->GetElementNormal();

	int	iNormalIndex = iVtxID;

	// MappingMode�� ReferenceMode�� ���� �ε����� ����� ������ �޶�����.
	// Mapping mode �� index �� ���ϴ� ����� ������ �ְ�
	// Reference Mode �� data �� ��� �ִ� ���� ������ ��
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

	// ���� UV���� 0~1�̾ƴ� 1�� �ʰ��� ���� �����µ�..
	// DirectX UV�� ���μ��� 0~1�ε� FBX���� �������� ���� 
	// Y���� 1~0����.. (�� ����)���� �Ǿ��ִ�.
	// �׷��� y���� ���Ҷ� 1.0f�� ���� ������ �� ���������̴�.
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
	int	iCount = m_NameArr.GetCount();				// �ִϸ��̼� �̸� ����

	FbxTime::EMode	eTimeMode = m_pScene->GetGlobalSettings().GetTimeMode();		// ���� 30��������

	for (int i = 0; i < iCount; ++i)
	{
		// m_NameArr�� ����� �̸����� Scene���κ��� FbxAnimStack ��ü�� ���´�.
		FbxAnimStack*	pAnimStack = m_pScene->FindMember<FbxAnimStack>(m_NameArr[i]->Buffer());

		if (!pAnimStack)
			continue;

		PFBXANIMATIONCLIP	pClip = new FBXANIMATIONCLIP;

		pClip->strName = pAnimStack->GetName();

		if (pClip->strName == "mixamo.com")
			m_bMixamo = true;

		// FbxTakeInfo : �ִϸ��̼� Ŭ������
		FbxTakeInfo*	pTake = m_pScene->GetTakeInfo(pClip->strName.c_str());

		pClip->tStart = pTake->mLocalTimeSpan.GetStart();
		pClip->tEnd = pTake->mLocalTimeSpan.GetStop();
		// GetFrameCount �Լ��� ȣ���ϰ�  time��带 �־��ָ� �ð��� ����������
		// ��ȯ���ش�. �������� ¥�� �ִϸ��̼� ������ �����ش�.
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
	// Deformer : Ÿ��
	// ���⼱ ��Ų Ÿ���� �����´�.
	int	iSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);		// ��Ų���� ��������

	// ��Ų�� ���ٸ� �ִϸ��̼� �ε��� �ʿ� X
	if (iSkinCount <= 0)
	{
		return;
	}

	// �޽��� ���� ���� ���´�.
	// ControlPoints�� �����̶�� �����ϸ� �ȴ�.
	int	iCPCount = pMesh->GetControlPointsCount();

	// ������ ����ġ ������ ���ε��� ������ ���� ����ŭ
	// ��������� �Ѵ�.
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
			// Cluster : ������ �ǹ��Ѵ�.
			int	iClusterCount = pSkin->GetClusterCount();

			for (int j = 0; j < iClusterCount; ++j)
			{
				FbxCluster*	pCluster = pSkin->GetCluster(j);

				if (!pCluster->GetLink())
					continue;

				// �ش� �� �̸� �������� ( �ش� ���� ������ ����Ǿ� �ִ�. )
				std::string	strBoneName = pCluster->GetLink()->GetName();

				if("R_Sword" == strBoneName)
				{
					int a = 0;
				}

				if ("Skirt_B_02" == strBoneName)
				{
					int b = 0;
				}
				
				// �ͻ�� ����Ʈ���� �ִϸ��̼��� �̾ƿ��� ���
				// ��纻 �̸� �տ� mixamo.com�� �پ..
				// �����ش�.
				if (m_bMixamo)
					strBoneName.erase(0, 10);

				// �ش� ���� �ε��� ��������
				int	iBoneIndex = FindBoneFromName(strBoneName);


				// ����ġ�� Indecies �ε��Ѵ�.
				LoadWeightAndIndex(pCluster, iBoneIndex, pContainer);

				// ������ ����� �ε��Ѵ�. ( ������ ����� �ʼ���. )
				LoadOffsetMatrix(pCluster, matTransform, iBoneIndex, pContainer);

				// �ش� �� ��� �־��ֱ� (��������� ���߿� ������ ����� �����ٰ���)
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
	// ��忡 �´� ��� ��������
	const FbxVector4	vT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4	vR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4	vS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(vT, vR, vS);
}

int FBXLoader::FindBoneFromName(const std::string & strName)
{
	// �� �̸��� �´� ���� ã���� �ش� ���� �ε����� �ѱ��.
	for (size_t i = 0; i < m_vecBones.size(); ++i)
	{
		if (m_vecBones[i]->strName == strName)
			return (int)i;
	}
	// �ε����� ������ -1����
	return -1;
}

void FBXLoader::LoadWeightAndIndex(FbxCluster * pCluster,
	int iBoneIndex, PFBXMESHCONTAINER pContainer)
{
	// Control Point(����)�� �ε��������� �����´�.
	// �ֳĸ� �������� ����ġ�� �����ϱ� ������
	int	iControlIndicesCount = pCluster->GetControlPointIndicesCount();

	for (int i = 0; i < iControlIndicesCount; ++i)
	{
		FBXWEIGHT	tWeight = {};

		tWeight.iIndex = iBoneIndex;			// �� �ε��� ( ������ ����� �� �ε����ε� )
		tWeight.dWeight = pCluster->GetControlPointWeights()[i];	// ����ġ

		int	iClusterIndex = pCluster->GetControlPointIndices()[i];

		// map�� Ư¡ : Ű�� �̿��� �ε���ó�� ������ ��� �ش� Ű�� ���ٸ�
		// ������ش�.
		pContainer->mapWeights[iClusterIndex].push_back(tWeight);
	}
}

void FBXLoader::LoadOffsetMatrix(FbxCluster * pCluster, const FbxAMatrix & matTransform, int iBoneIndex, PFBXMESHCONTAINER pContainer)
{
	FbxAMatrix	matCluster;
	FbxAMatrix	matClusterLink;

	pCluster->GetTransformMatrix(matCluster);			// Ŭ������ ���
	pCluster->GetTransformLinkMatrix(matClusterLink);	// ������ ����� ���

	// ������ ���(3D Max -> DirectX ���� ����ϴ� ���)�� ����� ���ؼ�
	// Reflect����� �ʿ��ϴ�.
	// �ش� Ŭ������ ����� �������� 3D Max���� �������� ���������� ����� �����µ�
	// DirectX���� ����� �� �ְԲ� ���� ����� Y��� Z���� �ٲٱ� ���� �����ش�.(Reflect�����)
	// �� Reflect����� YZ���� �ٲٱ� ���� ����̴�.
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
	matOffset = matClusterLink.Inverse() * matCluster * matTransform;		// ������ ����� ���ϴ� ������ ( Autodesk���� �̷��� ������ )
	matOffset = matReflect * matOffset * matReflect;

	m_vecBones[iBoneIndex]->matOffset = matOffset;
}

void FBXLoader::LoadTimeTransform(FbxNode * pNode, FbxCluster * pCluster, const FbxAMatrix & matTransform, int iBoneIndex)
{
	// Reflect ���
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
		// �ִϸ��̼��� �ͻ��� ���� �ִϸ��̼��� ���
		// mixamo.com�� �Ⱥٿ����͵��� �����.
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

		// ��ü ������ ����ŭ �ݺ��Ѵ�.
		for (FbxLongLong j = Start; j <= End; ++j)
		{
			FbxTime	tTime = {};

			// ���� �����ӿ� �ش��ϴ� FbxTime�� ������.
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
		// ����ġ�� �ִ� 4���� �����Ѵ�. (Vector4��..)  ������� 1�� ���� �ʴ´�.
		// ���� 4���� �ʰ��� ��� ������������ ������ �� �ڿ��� �ڸ���. ������
		if (iter->second.size() > 4)
		{
			// ����ġ ���� ���� �������� �����Ѵ�.
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

			// 4���� �ʰ��� ��� �� ������ �͵���� �����ؼ� �����Ѵ�.
			std::vector<FBXWEIGHT>::iterator	iterErase = iter->second.begin() + 4;

			iter->second.erase(iterErase, iter->second.end());		// 4�� �ʰ��ϴ¾ֵ��� ����
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
