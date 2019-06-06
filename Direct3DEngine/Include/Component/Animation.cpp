#include "Animation.h"
#include "..\Device.h"
#include "..\Resource\FBXLoader.h"
#include "..\GameObject.h"
#include "..\PathManager.h"
#include "Transform.h"
#include "Camera.h"
#include "TPCamera.h"
#include "..\Scene\Scene.h"

ENGINE_USING

Animation::Animation() :
	m_pBoneTexture(nullptr),
	m_pBoneResourceView(nullptr),
	m_pDefaultClip(nullptr),
	m_pCurClip(nullptr),
	m_pNextClip(nullptr),
	m_bEnd(false),
	m_fAnimationGlobalTime(0.f),
	m_fClipProgress(0.f),
	m_fChangeTime(0.f),
	m_fChangeLimitTime(0.25f),
	m_bParentBone(false),
	m_iParentBoneNum(-1),
	m_bChangePivot(false),
	m_iChangePivotBoneNum(-1)
{
	m_eComponentType = CT_ANIMATION;
	m_strParentBone = "";
	m_strChangePivotBone = "";
}

Animation::Animation(const Animation & _Com) : Component(_Com)
{
	*this = _Com;
	//this = new Animation;
	m_iReferenceCount = 1;

	m_pDefaultClip = nullptr;
	m_pCurClip = nullptr;
	m_pNextClip = nullptr;
	m_bEnd = false;

	m_pBoneTexture = nullptr;
	m_pBoneResourceView = nullptr;

	size_t ComvecBoneSize = _Com.m_vecBones.size();

	m_vecBones.resize(ComvecBoneSize);

	for (size_t i = 0; i < _Com.m_vecBones.size(); ++i)
	{
		++(m_vecBones[i]->iRefCount);
	}

	CreateBoneTexture();

	m_vecBoneMatrix.clear();

	m_vecBoneMatrix.resize(_Com.m_vecBoneMatrix.size());

	for (size_t i = 0; i < m_vecBoneMatrix.size(); ++i)
	{
		m_vecBoneMatrix[i] = new Matrix;
	}

	std::unordered_map<std::string, PANIMATIONCLIP>::const_iterator	iter;
	std::unordered_map<std::string, PANIMATIONCLIP>::const_iterator	iterEnd = _Com.m_mapClip.end();

	m_mapClip.clear();

	for (iter = _Com.m_mapClip.begin(); iter != iterEnd; ++iter)
	{
		PANIMATIONCLIP	pClip = new ANIMATIONCLIP;

		*pClip = *iter->second;

		pClip->vecCallback.clear();

		// Ű������
		size_t KeyFrameCount = pClip->vecKeyFrame.size();

		for (size_t i = 0; i < KeyFrameCount; ++i)
		{
			++(pClip->vecKeyFrame[i]->iRefCount);
		}

		// �ݹ�


		if (_Com.m_pCurClip->strName == iter->first)
			m_pCurClip = pClip;

		if (_Com.m_pDefaultClip->strName == iter->first)
			m_pDefaultClip = pClip;

		m_mapClip.insert(make_pair(iter->first, pClip));
	}
}


Animation::~Animation()
{
	Safe_Delete_Map(m_mapClip);
	Safe_Delete_VectorList(m_vecBoneMatrix);

	SAFE_RELEASE(m_pBoneResourceView);
	SAFE_RELEASE(m_pBoneTexture);


	for (size_t i = 0; i < m_vecBones.size(); ++i)
	{
		--(m_vecBones[i]->iRefCount);

		if (m_vecBones[i]->iRefCount == 0)
		{
			//Safe_Release_VecList(m_vecBones[i]->SocketList);
			SAFE_DELETE(m_vecBones[i]->matBone);
			SAFE_DELETE(m_vecBones[i]->matOffset);
			SAFE_DELETE(m_vecBones[i]);
		}
	}

	m_vecBones.clear();
}

const std::list<std::string>* Animation::GetAddClipNameList() const
{
	return &m_AddClipNameList;
}

void Animation::AddBone(PBONE pBone)
{
	m_vecBones.push_back(pBone);
}

// �ʹ� ū ����ĵ��� ���θ� ���̴��� �־��� �� ����. ( �������Ϳ�.. )
// �׷��� Texture2D�� �־ �Ѱ��ֱ� ���ؼ�
// �ؽ�ó�� �����Ѵ�. (�ε��Ҷ� ������)
bool Animation::CreateBoneTexture()
{
	SAFE_RELEASE(m_pBoneTexture);
	D3D11_TEXTURE2D_DESC	tDesc = {};
	tDesc.ArraySize = 1;
	tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tDesc.Usage = D3D11_USAGE_DYNAMIC;						// CPU ������ �����ؾ���
	tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// ��� ��������.. 32(4����Ʈ)�� ���
	tDesc.Height = 1;										// 1�� 
	tDesc.Width = (UINT)(m_vecBones.size() * 4);
	tDesc.MipLevels = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.SampleDesc.Count = 1;

	if (FAILED(_DEVICE->CreateTexture2D(&tDesc, NULL, &m_pBoneTexture)))
		return false;

	if (FAILED(_DEVICE->CreateShaderResourceView(m_pBoneTexture, NULL, &m_pBoneResourceView)))
		return false;

	m_vecBlendInfo.resize(m_vecBones.size());

	return true;
}

// fbx�ε��Ҷ� ���
void Animation::AddClip(ANIMATION_OPTION _eOption, _tagFbxAnimationClip * _pClip)
{
	PANIMATIONCLIP	pAnimClip = FindClip(_pClip->strName);

	// �ش� �ִϸ��̼��� ������ return (������ �߰��� �ʿ䰡 �����ϱ�)
	if (nullptr != pAnimClip)
		return;

	pAnimClip = new ANIMATIONCLIP;

	// ���ڷ� ���� �ִϸ��̼� �ɼ������� �����Ѵ�.
	pAnimClip->eOption = _eOption;
	pAnimClip->strName = _pClip->strName;

	pAnimClip->iChangeFrame = 0;

	// FBXANIMATIONCLIP�� �ִ� starttime �� endtime �� �̿��Ͽ� keyframe �� ���´�.
	pAnimClip->iStartFrame = (int)(_pClip->tStart.GetFrameCount(_pClip->eTimeMode));
	pAnimClip->iEndFrame = (int)(_pClip->tEnd.GetFrameCount(_pClip->eTimeMode));
	pAnimClip->iFrameLength = pAnimClip->iEndFrame - pAnimClip->iStartFrame;

	// �ð� ������ �������ش�.
	pAnimClip->fStartTime = 0.0f;
	pAnimClip->fEndTime = (float)(pAnimClip->fPlayTime);
	pAnimClip->fTimeLength = (float)(pAnimClip->fPlayTime);

	pAnimClip->fFrameTime = pAnimClip->fPlayTime / pAnimClip->iFrameLength;

	// Ű ������ ����ŭ �ݺ��ϸ� ������ �������� ������ ��� ������ ��ġ, ũ��, ȸ��������
	// �̾ƿ´�.
	for (size_t i = 0; i < _pClip->vecBoneKeyFrame.size(); ++i)
	{
		PBONEKEYFRAME	pBoneKeyFrame = new BONEKEYFRAME;

		pBoneKeyFrame->iBoneIndex = _pClip->vecBoneKeyFrame[i].iBoneIndex;

		pAnimClip->vecKeyFrame.push_back(pBoneKeyFrame);

		// �Ʒ����� Ű������ ������ �����Ѵ�.
		pBoneKeyFrame->vecKeyFrame.reserve(_pClip->vecBoneKeyFrame[i].vecKeyFrame.size());

		for (size_t j = 0; j < _pClip->vecBoneKeyFrame[i].vecKeyFrame.size(); ++j)
		{
			PKEYFRAME	pKeyFrame = new KEYFRAME;

			pKeyFrame->dTime = j * pAnimClip->fFrameTime;

			// ���� ���� Ű �����ӿ� �ش��ϴ� ��� ������ ���´�.
			FbxAMatrix	mat = _pClip->vecBoneKeyFrame[i].vecKeyFrame[j].matTransform;

			FbxVector4	vPos, vScale;
			FbxQuaternion	qRot;

			// ��ķκ��� ��ġ, ũ��, ȸ�� ������ ���´�.
			vPos = mat.GetT();
			vScale = mat.GetS();
			qRot = mat.GetQ();

			pKeyFrame->vScale = Vector3((float)vScale.mData[0], (float)vScale.mData[1],
				(float)vScale.mData[2]);
			pKeyFrame->vPos = Vector3((float)vPos.mData[0], (float)vPos.mData[1],
				(float)vPos.mData[2]);
			pKeyFrame->vRot = Vector4((float)qRot.mData[0], (float)qRot.mData[1],
				(float)qRot.mData[2], (float)qRot.mData[3]);

			pBoneKeyFrame->vecKeyFrame.push_back(pKeyFrame);
		}
	}

	switch (_pClip->eTimeMode)
	{
	case FbxTime::eFrames24:
		pAnimClip->iFrameMode = 24;
		break;
	case FbxTime::eFrames30:
		pAnimClip->iFrameMode = 30;
		break;
	case FbxTime::eFrames60:
		pAnimClip->iFrameMode = 60;
		break;
	}

	if (m_mapClip.empty())
	{
		m_pDefaultClip = pAnimClip;
		m_pCurClip = pAnimClip;
	}

	m_mapClip.insert(make_pair(pAnimClip->strName, pAnimClip));

	m_AddClipNameList.clear();				//fbx �ε��Ҷ��ϱ� ��ü ����Ʈ�� clear�ϰ� ���� �־��ش�.

	m_AddClipNameList.push_back(pAnimClip->strName);
}

// ���� ������ Ŭ���� �߰��Ҷ� ȣ���ϴ� �Լ� 
// ���� ��� �����������
// ��� ���� �ʿ��ϴ� ( Ŭ�� �̸�, Ŭ�� �ɼ�, �ִϸ��̼� ���� ������,  ��������, �÷��� �ð�, (����)�־��� Ű������ ���� �� ������ Ű������ ����)  
void Animation::AddClip(const std::string & _strName,
	ANIMATION_OPTION _eOption, 
	int _iStartFrame, 
	int _iEndFrame, 
	float _fPlayTime,
	const std::vector<PBONEKEYFRAME>& _vecFrame)
{
	PANIMATIONCLIP	pAnimClip = FindClip(_strName);

	// ����� �ش� Ŭ���̸��� �̹� �����Ѵٸ� ����� X
	if (nullptr != pAnimClip)
		return;

	pAnimClip = new ANIMATIONCLIP;

	// ���ڷ� ���� �ִϸ��̼� �ɼ������� �����Ѵ�.
	pAnimClip->eOption = _eOption;
	pAnimClip->strName = _strName;

	pAnimClip->iChangeFrame = 0;

	int	iLength = _iEndFrame - _iStartFrame;

	// FBXANIMATIONCLIP�� �ִ� starttime �� endtime �� �̿��Ͽ� keyframe �� ���´�.
	pAnimClip->iStartFrame = 0;
	pAnimClip->iEndFrame = iLength;
	pAnimClip->iFrameLength = iLength;

	// �ð� ������ �������ش�.
	pAnimClip->fStartTime = 0.f;
	pAnimClip->fEndTime = _fPlayTime;
	pAnimClip->fTimeLength = _fPlayTime;
	pAnimClip->fPlayTime = _fPlayTime;

	pAnimClip->fFrameTime = pAnimClip->fPlayTime / pAnimClip->iFrameLength;

	// Ű ������ ����ŭ �ݺ��ϸ� ������ �������� ������ ��� ������ ��ġ, ũ��, ȸ��������
	// �̾ƿ´�.
	for (size_t i = 0; i < _vecFrame.size(); ++i)
	{
		PBONEKEYFRAME	pBoneFrame = new BONEKEYFRAME;
		pAnimClip->vecKeyFrame.push_back(pBoneFrame);

		PBONEKEYFRAME	pClipFrame = _vecFrame[i];
		pBoneFrame->iBoneIndex = pClipFrame->iBoneIndex;
		pBoneFrame->iRefCount = 1;

		// �ش� Ű�������� ������.. (Ű�����Ӱ��� �־��ش�.)
		if (false == pClipFrame->vecKeyFrame.empty())
		{
			for (size_t j = _iStartFrame; j <= _iEndFrame; ++j)
			{
				PKEYFRAME	pFrame = new KEYFRAME;

				pFrame->dTime = (j - _iStartFrame) * pAnimClip->fFrameTime;
				pFrame->vPos = pClipFrame->vecKeyFrame[j]->vPos;
				pFrame->vScale = pClipFrame->vecKeyFrame[j]->vScale;
				pFrame->vRot = pClipFrame->vecKeyFrame[j]->vRot;

				pBoneFrame->vecKeyFrame.push_back(pFrame);
			}
		}
	}

	// �ִϸ��̼� Ŭ���� �߰��ϴµ� 
	// ������Ʈ�� �����ִ� �ִϸ��̼��� �ƹ��� �ִϸ��̼� Ŭ������ ���ٸ�
	// ó�� �־��ؾְ� DefaultClip���� �����ϰ�, ����Ŭ���� ���� ó������ �߰��� Ŭ������ �����Ѵ�.
	if (true == m_mapClip.empty())
	{
		m_pDefaultClip = pAnimClip;
		m_pCurClip = pAnimClip;
	}

	// �ִϸ��̼� Ŭ��map�� ������� �ִϸ��̼� Ŭ���� �־��ش�.( �ִϸ��̼� Ŭ�� �̸�, ������� �ִϸ��̼� Ŭ�� )
	m_mapClip.insert(std::make_pair(pAnimClip->strName, pAnimClip));

	
	m_AddClipNameList.clear();

	m_AddClipNameList.push_back(pAnimClip->strName);
}

void Animation::AddClip(const TCHAR * _pFullPath)
{
	char	strFullPath[MAX_PATH] = {};

#ifdef UNICODE
	WideCharToMultiByte(CP_UTF8, 0, _pFullPath, -1, strFullPath, lstrlen(_pFullPath),
		0, 0);
#else
	strcpy_s(strFullPath, pFullPath);
#endif // UNICODE

	AddClipFromMultibyte(strFullPath);
}

void Animation::AddClipFromMultibyte(const char * _pFullPath)
{
	char	strExt[_MAX_EXT] = {};

	_splitpath_s(_pFullPath, 0, 0, 0, 0, 0, 0, strExt, _MAX_EXT);

	_strupr_s(strExt);

	if (strcmp(strExt, ".FBX") == 0)
	{
		LoadFBXAnimation(_pFullPath);
	}

	else
	{
		LoadFromFullPath(_pFullPath);
	}
}

PANIMATIONCLIP Animation::FindClip(const std::string & strName)
{
	std::unordered_map<std::string, PANIMATIONCLIP>::iterator	iter = m_mapClip.find(strName);

	if (iter == m_mapClip.end())
		return nullptr;

	return iter->second;
}

Matrix Animation::FindIndexBoneMatrix(int _Index)
{
	Matrix returnMat = *(m_vecBoneMatrix[_Index]);
	return returnMat;
}

PANIMATIONCLIP Animation::GetCurrentClip() const
{
	return m_pCurClip;
}

// �Ű������� ������ _vecKeyFrame�� ���� Ű�������� �־��ش�.
void Animation::GetCurrentKeyFrame(std::vector<PBONEKEYFRAME>& _vecKeyFrame)
{
	for (size_t i = 0; i < m_pCurClip->vecKeyFrame.size(); ++i)
	{
		PBONEKEYFRAME	pBoneFrame = new BONEKEYFRAME;
		_vecKeyFrame.push_back(pBoneFrame);

		PBONEKEYFRAME	pClipFrame = m_pCurClip->vecKeyFrame[i];
		pBoneFrame->iBoneIndex = pClipFrame->iBoneIndex;
		pBoneFrame->iRefCount = 1;

		for (size_t j = 0; j < pClipFrame->vecKeyFrame.size(); ++j)
		{
			PKEYFRAME	pFrame = new KEYFRAME;

			pFrame->dTime = pClipFrame->vecKeyFrame[j]->dTime;
			pFrame->vPos = pClipFrame->vecKeyFrame[j]->vPos;
			pFrame->vScale = pClipFrame->vecKeyFrame[j]->vScale;
			pFrame->vRot = pClipFrame->vecKeyFrame[j]->vRot;

			pBoneFrame->vecKeyFrame.push_back(pFrame);
		}
	}
}

const std::unordered_map<std::string, PANIMATIONCLIP>* Animation::GetClips() const
{
	return &m_mapClip;
}

bool Animation::IsAnimationEnd() const
{
	return m_bEnd;
}

void Animation::ChangeClipKey(const std::string & strOrigin, const std::string & strChange)
{
	// strOrigin : ���� �ִϸ��̼� 
	// strChange : �ٲ� �ִϸ��̼�
	std::unordered_map<std::string, PANIMATIONCLIP>::iterator	iter = m_mapClip.find(strOrigin);

	// �ִϸ��̼��� ���ٸ� return
	if (iter == m_mapClip.end())
		return;

	PANIMATIONCLIP	pClip = iter->second;

	pClip->strName = strChange;

	m_mapClip.erase(iter);

	m_mapClip.insert(std::make_pair(strChange, pClip));
}

PBONE Animation::FindBone(const std::string & strBoneName)
{
	for (size_t i = 0; i < m_vecBones.size(); ++i)
	{
		if (m_vecBones[i]->strName == strBoneName)
			return m_vecBones[i];
	}

	return NULL;
}

PBONE Animation::FindBone(int _Index)
{
	if (_Index >= m_vecBones.size())
		return NULL;

	return m_vecBones[_Index];
}

// ���ε��� ( �̸����� ã��)
int Animation::FindBoneIndex(const std::string & strBoneName)
{
	for (size_t i = 0; i < m_vecBones.size(); ++i)
	{
		if (m_vecBones[i]->strName == strBoneName)
			return (int)i;
	}

	return -1;
}

Matrix Animation::GetBoneMatrix(const std::string & strBoneName)
{
	int	iBone = FindBoneIndex(strBoneName);

	// �ش� ���� ���ٸ�
	if (iBone == -1)
		return Matrix();

	return *m_vecBoneMatrix[iBone];
}

// Ŭ�� �ٲٱ� ( ���� �ִϸ��̼� -> �ٸ��ִϸ��̼����� )
bool Animation::ChangeClip(const std::string & strClip)
{
	if (m_pCurClip->strName == strClip)
		return false;

	m_pNextClip = FindClip(strClip);

	if (!m_pNextClip)
		return false;

	return true;
}

ID3D11ShaderResourceView ** Animation::GetBoneTexture()
{
	return &m_pBoneResourceView;
}

bool Animation::Save(const TCHAR * pFileName, const std::string & strPathKey)
{
	char	strFileName[MAX_PATH] = {};

#ifdef UNICODE
	WideCharToMultiByte(CP_UTF8, 0, pFileName, -1, strFileName, lstrlen(pFileName),
		NULL, NULL);
#else
	strcpy_s(strFileName, pFileName);
#endif // UNICODE

	return Save(strFileName, strPathKey);
}

bool Animation::Save(const char * pFileName, const std::string & strPathKey)
{
	const char*	pPath = GET_SINGLETON(PathManager)->FindPathMultibyte(strPathKey);

	std::string	strFullPath;

	if (pPath)
		strFullPath = pPath;

	strFullPath += pFileName;

	return SaveFromFullPath(strFullPath.c_str());
}

bool Animation::SaveFromFullPath(const TCHAR * pFullPath)
{
	char	strFileName[MAX_PATH] = {};

#ifdef UNICODE
	WideCharToMultiByte(CP_UTF8, 0, pFullPath, -1, strFileName, lstrlen(pFullPath),
		NULL, NULL);
#else
	strcpy_s(strFileName, pFileName);
#endif // UNICODE

	return SaveFromFullPath(strFileName);
}

// ������ �ش� ��ο� ���̳ʸ����Ϸ� �����ϴ� �Լ�
bool Animation::SaveFromFullPath(const char * pFullPath)
{
	FILE*	pFile = NULL;

	fopen_s(&pFile, pFullPath, "wb");

	if (!pFile)
		return false;

	fwrite(&m_fChangeLimitTime, sizeof(float), 1, pFile);

	// �ִϸ��̼� Ŭ�������� �����Ѵ�.
	size_t iCount = m_mapClip.size();

	fwrite(&iCount, sizeof(size_t), 1, pFile);

	size_t iLength = m_pDefaultClip->strName.length();
	fwrite(&iLength, sizeof(size_t), 1, pFile);
	fwrite(m_pDefaultClip->strName.c_str(), sizeof(char),
		iLength, pFile);

	iLength = m_pCurClip->strName.length();
	fwrite(&iLength, sizeof(size_t), 1, pFile);
	fwrite(m_pCurClip->strName.c_str(), sizeof(char), iLength, pFile);

	std::unordered_map<std::string, PANIMATIONCLIP>::iterator	iter;
	std::unordered_map<std::string, PANIMATIONCLIP>::iterator	iterEnd = m_mapClip.end();

	for (iter = m_mapClip.begin(); iter != iterEnd; ++iter)
	{
		PANIMATIONCLIP	pClip = iter->second;

		// �ִϸ��̼� Ŭ�� Ű�� �����Ѵ�.
		iLength = pClip->strName.length();
		fwrite(&iLength, sizeof(size_t), 1, pFile);
		fwrite(pClip->strName.c_str(), sizeof(char), iLength, pFile);

		fwrite(&pClip->eOption, sizeof(ANIMATION_OPTION), 1, pFile);

		fwrite(&pClip->fStartTime, sizeof(float), 1, pFile);
		fwrite(&pClip->fEndTime, sizeof(float), 1, pFile);
		fwrite(&pClip->fTimeLength, sizeof(float), 1, pFile);
		fwrite(&pClip->fFrameTime, sizeof(float), 1, pFile);

		fwrite(&pClip->iFrameMode, sizeof(int), 1, pFile);
		fwrite(&pClip->iStartFrame, sizeof(int), 1, pFile);
		fwrite(&pClip->iEndFrame, sizeof(int), 1, pFile);
		fwrite(&pClip->iFrameLength, sizeof(int), 1, pFile);
		fwrite(&pClip->fPlayTime, sizeof(float), 1, pFile);

		size_t	iCount = pClip->vecKeyFrame.size();

		fwrite(&iCount, sizeof(size_t), 1, pFile);

		for (size_t i = 0; i < iCount; ++i)
		{
			fwrite(&pClip->vecKeyFrame[i]->iBoneIndex, sizeof(int), 1,
				pFile);

			size_t	iFrameCount = pClip->vecKeyFrame[i]->vecKeyFrame.size();

			fwrite(&iFrameCount, sizeof(size_t), 1, pFile);

			for (size_t j = 0; j < iFrameCount; ++j)
			{
				fwrite(&pClip->vecKeyFrame[i]->vecKeyFrame[j]->dTime, sizeof(double), 1, pFile);
				fwrite(&pClip->vecKeyFrame[i]->vecKeyFrame[j]->vPos, sizeof(Vector3), 1, pFile);
				fwrite(&pClip->vecKeyFrame[i]->vecKeyFrame[j]->vScale, sizeof(Vector3), 1, pFile);
				fwrite(&pClip->vecKeyFrame[i]->vecKeyFrame[j]->vRot, sizeof(Vector4), 1, pFile);
			}
		}
	}

	fclose(pFile);

	return true;
}

bool Animation::Load(const TCHAR * pFileName, const std::string & strPathKey)
{
	char	strFileName[MAX_PATH] = {};

#ifdef UNICODE
	WideCharToMultiByte(CP_UTF8, 0, pFileName, -1, strFileName, lstrlen(pFileName),
		NULL, NULL);
#else
	strcpy_s(strFileName, pFileName);
#endif // UNICODE

	return Load(strFileName, strPathKey);
}

bool Animation::Load(const char * pFileName, const std::string & strPathKey)
{
	const char*	pPath = GET_SINGLETON(PathManager)->FindPathMultibyte(strPathKey);

	std::string	strFullPath;

	if (pPath)
		strFullPath = pPath;

	strFullPath += pFileName;

	return LoadFromFullPath(strFullPath.c_str());
}

bool Animation::LoadFromFullPath(const TCHAR * pFullPath)
{
	char	strFileName[MAX_PATH] = {};

#ifdef UNICODE
	WideCharToMultiByte(CP_UTF8, 0, pFullPath, -1, strFileName, lstrlen(pFullPath),
		NULL, NULL);
#else
	strcpy_s(strFileName, pFileName);
#endif // UNICODE

	return LoadFromFullPath(strFileName);
}

bool Animation::LoadFromFullPath(const char * pFullPath)
{
	FILE*	pFile = NULL;

	fopen_s(&pFile, pFullPath, "rb");

	if (!pFile)
		return false;

	fread(&m_fChangeLimitTime, sizeof(float), 1, pFile);

	// �ִϸ��̼� Ŭ�������� �����Ѵ�.
	size_t iCount = 0, iLength = 0;
	fread(&iCount, sizeof(size_t), 1, pFile);

	char	strDefaultClip[256] = {};
	fread(&iLength, sizeof(size_t), 1, pFile);
	fread(strDefaultClip, sizeof(char),
		iLength, pFile);

	char	strCurClip[256] = {};
	fread(&iLength, sizeof(size_t), 1, pFile);
	fread(strCurClip, sizeof(char), iLength, pFile);

	m_AddClipNameList.clear();

	for (int l = 0; l < iCount; ++l)
	{
		PANIMATIONCLIP	pClip = new ANIMATIONCLIP;

		// �ִϸ��̼� Ŭ�� Ű�� �����Ѵ�.
		char	strClipName[256] = {};
		fread(&iLength, sizeof(size_t), 1, pFile);
		fread(strClipName, sizeof(char), iLength, pFile);

		m_mapClip.insert(std::make_pair(strClipName, pClip));

		pClip->strName = strClipName;
		pClip->iChangeFrame = 0;

		m_AddClipNameList.push_back(strClipName);

		fread(&pClip->eOption, sizeof(ANIMATION_OPTION), 1, pFile);

		fread(&pClip->fStartTime, sizeof(float), 1, pFile);
		fread(&pClip->fEndTime, sizeof(float), 1, pFile);
		fread(&pClip->fTimeLength, sizeof(float), 1, pFile);
		fread(&pClip->fFrameTime, sizeof(float), 1, pFile);

		fread(&pClip->iFrameMode, sizeof(int), 1, pFile);
		fread(&pClip->iStartFrame, sizeof(int), 1, pFile);
		fread(&pClip->iEndFrame, sizeof(int), 1, pFile);
		fread(&pClip->iFrameLength, sizeof(int), 1, pFile);
		fread(&pClip->fPlayTime, sizeof(float), 1, pFile);

		size_t	iFrameCount = 0;

		fread(&iFrameCount, sizeof(size_t), 1, pFile);

		for (size_t i = 0; i < iFrameCount; ++i)
		{
			PBONEKEYFRAME	pBoneKeyFrame = new BONEKEYFRAME;
			pClip->vecKeyFrame.push_back(pBoneKeyFrame);

			fread(&pBoneKeyFrame->iBoneIndex, sizeof(int), 1,
				pFile);

			size_t	iBoneFrameCount = 0;

			fread(&iBoneFrameCount, sizeof(size_t), 1, pFile);

			for (size_t j = 0; j < iBoneFrameCount; ++j)
			{
				PKEYFRAME	pKeyFrame = new KEYFRAME;
				pBoneKeyFrame->vecKeyFrame.push_back(pKeyFrame);

				fread(&pKeyFrame->dTime, sizeof(double), 1, pFile);
				fread(&pKeyFrame->vPos, sizeof(Vector3), 1, pFile);
				fread(&pKeyFrame->vScale, sizeof(Vector3), 1, pFile);
				fread(&pKeyFrame->vRot, sizeof(Vector4), 1, pFile);
			}
		}
	}

	m_pCurClip = FindClip(strCurClip);
	m_pDefaultClip = FindClip(strDefaultClip);

	fclose(pFile);

	return true;
}

bool Animation::SaveBone(const TCHAR * pFileName, const std::string & strPathKey)
{
	char	strFileName[MAX_PATH] = {};

#ifdef UNICODE
	WideCharToMultiByte(CP_UTF8, 0, pFileName, -1, strFileName, lstrlen(pFileName),
		NULL, NULL);
#else
	strcpy_s(strFileName, pFileName);
#endif // UNICODE

	return SaveBone(strFileName, strPathKey);
}

bool Animation::SaveBone(const char * pFileName, const std::string & strPathKey)
{
	const char*	pPath = GET_SINGLETON(PathManager)->FindPathMultibyte(strPathKey);

	std::string	strFullPath;

	if (pPath)
		strFullPath = pPath;

	strFullPath += pFileName;

	return SaveBoneFromFullPath(strFullPath.c_str());
}

bool Animation::SaveBoneFromFullPath(const TCHAR * pFullPath)
{
	char	strFileName[MAX_PATH] = {};

#ifdef UNICODE
	WideCharToMultiByte(CP_UTF8, 0, pFullPath, -1, strFileName, lstrlen(pFullPath),
		NULL, NULL);
#else
	strcpy_s(strFileName, pFileName);
#endif // UNICODE

	return SaveBoneFromFullPath(strFileName);
}

bool Animation::SaveBoneFromFullPath(const char * pFullPath)
{
	FILE*	pFile = NULL;

	fopen_s(&pFile, pFullPath, "wb");

	if (!pFile)
		return false;

	// ===================== �� ���� ���� =====================
	size_t	iCount = m_vecBones.size();

	fwrite(&iCount, sizeof(size_t), 1, pFile);

	size_t	iLength = 0;

	for (size_t i = 0; i < iCount; ++i)
	{
		iLength = m_vecBones[i]->strName.length();
		fwrite(&iLength, sizeof(size_t), 1, pFile);
		fwrite(m_vecBones[i]->strName.c_str(), sizeof(char), iLength, pFile);

		fwrite(&m_vecBones[i]->iDepth, sizeof(int), 1, pFile);
		fwrite(&m_vecBones[i]->iParentIndex, sizeof(int), 1, pFile);
		fwrite(&m_vecBones[i]->matOffset->m, sizeof(XMMATRIX), 1, pFile);
		fwrite(&m_vecBones[i]->matBone->m, sizeof(XMMATRIX), 1, pFile);
	}

	fclose(pFile);

	return true;
}

bool Animation::LoadBone(const TCHAR * pFileName, const std::string & strPathKey)
{
	char	strFileName[MAX_PATH] = {};

#ifdef UNICODE
	WideCharToMultiByte(CP_UTF8, 0, pFileName, -1, strFileName, lstrlen(pFileName),
		NULL, NULL);
#else
	strcpy_s(strFileName, pFileName);
#endif // UNICODE

	return LoadBone(strFileName, strPathKey);
}

bool Animation::LoadBone(const char * pFileName, const std::string & strPathKey)
{
	const char*	pPath = GET_SINGLETON(PathManager)->FindPathMultibyte(strPathKey);

	std::string	strFullPath;

	if (pPath)
		strFullPath = pPath;

	strFullPath += pFileName;

	return LoadBoneFromFullPath(strFullPath.c_str());
}

bool Animation::LoadBoneFromFullPath(const TCHAR * pFullPath)
{
	char	strFileName[MAX_PATH] = {};

#ifdef UNICODE
	WideCharToMultiByte(CP_UTF8, 0, pFullPath, -1, strFileName, lstrlen(pFullPath),
		NULL, NULL);
#else
	strcpy_s(strFileName, pFileName);
#endif // UNICODE

	return LoadBoneFromFullPath(strFileName);
}

bool Animation::LoadBoneFromFullPath(const char * pFullPath)
{
	FILE*	pFile = NULL;

	fopen_s(&pFile, pFullPath, "rb");

	if (!pFile)
		return false;

	// ===================== �� ���� �б� =====================
	size_t	iCount = 0;

	fread(&iCount, sizeof(size_t), 1, pFile);

	size_t	iLength = 0;

	for (size_t i = 0; i < iCount; ++i)
	{
		PBONE	pBone = new BONE;
		m_vecBones.push_back(pBone);

		pBone->matBone = new Matrix;
		pBone->matOffset = new Matrix;

		char	strBoneName[256] = {};
		fread(&iLength, sizeof(size_t), 1, pFile);
		fread(strBoneName, sizeof(char), iLength, pFile);
		pBone->strName = strBoneName;

		fread(&pBone->iDepth, sizeof(int), 1, pFile);
		fread(&pBone->iParentIndex, sizeof(int), 1, pFile);
		fread(&pBone->matOffset->m, sizeof(XMMATRIX), 1, pFile);
		fread(&pBone->matBone->m, sizeof(XMMATRIX), 1, pFile);
	}

	fclose(pFile);

	CreateBoneTexture();

	return true;
}

bool Animation::ModifyClip(const std::string & _strKey,
	const std::string & _strChangeKey,
	ANIMATION_OPTION _eOption, 
	int _iStartFrame, 
	int _iEndFrame, 
	float _fPlayTime, 
	const std::vector<PBONEKEYFRAME>& _vecFrame)
{
	PANIMATIONCLIP	pClip = FindClip(_strKey);

	if (nullptr == pClip)
		return false;

	m_mapClip.erase(_strKey);

	int	iLength = _iEndFrame - _iStartFrame;

	pClip->eOption = _eOption;
	pClip->strName = _strChangeKey;
	pClip->iStartFrame = 0;
	pClip->iEndFrame = iLength;
	pClip->iFrameLength = iLength;
	pClip->fStartTime = 0.f;
	pClip->fEndTime = _fPlayTime / (float)pClip->iFrameMode;
	pClip->fTimeLength = _fPlayTime;
	pClip->fPlayTime = _fPlayTime;

	pClip->fFrameTime = pClip->fPlayTime / iLength;

	m_mapClip.insert(std::make_pair(_strChangeKey, pClip));

	Safe_Delete_VectorList(pClip->vecKeyFrame);

	for (size_t i = 0; i < _vecFrame.size(); ++i)
	{
		PBONEKEYFRAME	pBoneFrame = new BONEKEYFRAME;
		pClip->vecKeyFrame.push_back(pBoneFrame);

		PBONEKEYFRAME	pClipFrame = _vecFrame[i];
		pBoneFrame->iBoneIndex = pClipFrame->iBoneIndex;
		pBoneFrame->iRefCount = 1;

		if (!pClipFrame->vecKeyFrame.empty())
		{
			for (size_t j = _iStartFrame; j <= _iEndFrame; ++j)
			{
				PKEYFRAME	pFrame = new KEYFRAME;

				pFrame->dTime = j * pClip->fFrameTime;
				pFrame->vPos = pClipFrame->vecKeyFrame[j]->vPos;
				pFrame->vScale = pClipFrame->vecKeyFrame[j]->vScale;
				pFrame->vRot = pClipFrame->vecKeyFrame[j]->vRot;

				pBoneFrame->vecKeyFrame.push_back(pFrame);
			}
		}
	}

	return true;
}

bool Animation::DeleteClip(const std::string & _strKey)
{
	std::unordered_map<std::string, PANIMATIONCLIP>::iterator	iter = m_mapClip.find(_strKey);

	if (iter == m_mapClip.end())
		return false;


	// ������ Ŭ���� Default Ŭ���� ���ٸ�
	// Ŭ���� �������� �� ù��°�� �ִ� Ŭ���� DefaultŬ������ �������ش�.
	if (_strKey == m_pDefaultClip->strName)
	{
		if (!m_mapClip.empty())
		{
			std::unordered_map<std::string, PANIMATIONCLIP>::iterator	Citer = m_mapClip.begin();

			while (Citer != m_mapClip.end() && Citer->first == _strKey)
			{
				++Citer;
			}

			m_pDefaultClip = FindClip(Citer->first);
		}
	}

	// ������ Ŭ���� ���� Ŭ���̶��
	// ���� �ִϸ��̼��� DfaultŬ������ �ٲ��ش�.
	if (_strKey == m_pCurClip->strName)
	{
		m_pCurClip = m_pDefaultClip;
	}

	SAFE_DELETE(iter->second);

	m_mapClip.erase(iter);

	return true;
}

bool Animation::ReturnDefaultClip()
{
	return ChangeClip(m_pDefaultClip->strName);
}

void Animation::LoadFBXAnimation(const char * _pFullPath)
{
	FBXLoader	loader;

	loader.LoadFbx(_pFullPath);

	// �ִϸ��̼� Ŭ���� �߰��Ѵ�.
	const std::vector<PFBXANIMATIONCLIP>* pvecClip = loader.GetClips();

	// Ŭ���� �о�´�.
	std::vector<PFBXANIMATIONCLIP>::const_iterator	iterC;
	std::vector<PFBXANIMATIONCLIP>::const_iterator	iterCEnd = pvecClip->end();

	m_AddClipNameList.clear();


	for (iterC = pvecClip->begin(); iterC != iterCEnd; ++iterC)
	{
		AddClip(AO_LOOP, *iterC);

		m_AddClipNameList.push_back((*iterC)->strName);
	}
}

void Animation::Start()
{
}

bool Animation::Init()
{
	return true;
}

int Animation::Input(float fTime)
{
	return 0;
}

int Animation::Update(float fTime)
{
	// �ִϸ��̼� ���� ���� ��ġ
	Vector3 vPrevPos = m_pTransform->GetWorldPosition();

	if (m_mapClip.empty())
		return 0;

	if (m_vecBoneMatrix.empty())
	{
		m_vecBoneMatrix.resize(m_vecBones.size());

		for (size_t i = 0; i < m_vecBoneMatrix.size(); ++i)
		{
			m_vecBoneMatrix[i] = new Matrix;
		}
	}

	m_bEnd = false;

	Matrix	matParent;

	// �θ��� ���� ���
	if (true == m_pGameObject->ParentEnable() && true == m_bParentBone)
	{
		GameObject* pParent = m_pGameObject->GetParent();
		Animation* pAni = pParent->FindComponentFromType<Animation>(CT_ANIMATION);
		PBONE matParBoneNum = pAni->FindBone(m_iParentBoneNum);
		matParent = *(matParBoneNum->matBone);

		SAFE_RELEASE(pAni);
	}

	// ����� ���Ҷ�
	if (nullptr != m_pNextClip)
	{
		m_fChangeTime += fTime;

		bool	bChange = false;
		if (m_fChangeTime >= m_fChangeLimitTime)
		{
			m_fChangeTime = m_fChangeLimitTime;
			bChange = true;
		}

		float	fAnimationTime = m_fAnimationGlobalTime +
			m_pCurClip->fStartTime;

		// �� ����ŭ �ݺ��Ѵ�.
		for (size_t i = 0; i < m_vecBones.size(); ++i)
		{
			// Ű�������� ���� ���
			if (m_pCurClip->vecKeyFrame[i]->vecKeyFrame.empty())
			{
				*m_vecBoneMatrix[i] = *m_vecBones[i]->matBone;
				continue;
			}

			//int	iFrameIndex = (int)(fAnimationTime * m_iFrameMode);
			//int	iFrameIndex = m_pCurClip->iChangeFrame;
			int	iNextFrameIndex = m_pNextClip->iStartFrame;

			//const PKEYFRAME pCurKey = m_pCurClip->vecKeyFrame[i]->vecKeyFrame[iFrameIndex];
			const PKEYFRAME pNextKey = m_pNextClip->vecKeyFrame[i]->vecKeyFrame[iNextFrameIndex];

			float	fPercent = m_fChangeTime / m_fChangeLimitTime;

			// ����
			XMVECTOR	vS = XMVectorLerp(m_vecBlendInfo[i].vScale.Convert(),
				pNextKey->vScale.Convert(), fPercent);
			XMVECTOR	vT = XMVectorLerp(m_vecBlendInfo[i].vPos.Convert(),
				pNextKey->vPos.Convert(), fPercent);
			XMVECTOR	vR = XMQuaternionSlerp(m_vecBlendInfo[i].vRot.Convert(),
				pNextKey->vRot.Convert(), fPercent);

			XMVECTOR	vZero = XMVectorSet(0.f, 0.f, 0.f, 1.f);

			// XMMatrixAffineTransformation( ũ��, ȸ���߽��� �ĺ��ϴ� ��, ȸ��, ��ġ )
			Matrix	matBone = XMMatrixAffineTransformation(vS, vZero,
				vR, vT);

			//Vector4 vTMove = vT.m128_f32;


			// �θ��� ���� ���
			if (true == m_pGameObject->ParentEnable() && true == m_bParentBone)
			{
				matBone *= matParent;
			}

			*m_vecBones[i]->matBone = matBone;

			matBone = *m_vecBones[i]->matOffset *
				matBone;

			*m_vecBoneMatrix[i] = matBone;
		}

		if (bChange)
		{
			m_pCurClip = m_pNextClip;
			m_pNextClip = nullptr;
			m_fAnimationGlobalTime = 0.f;
			m_fChangeTime = 0.f;
		}
	}

	// ���� ����� ��� ���۵ɶ�
	else
	{
		m_fAnimationGlobalTime += fTime;
		m_fClipProgress = m_fAnimationGlobalTime / m_pCurClip->fPlayTime;

		while (m_fAnimationGlobalTime >= m_pCurClip->fPlayTime)
		{
			m_fAnimationGlobalTime -= m_pCurClip->fPlayTime;
			m_bEnd = true;

			/*for (size_t i = 0; i < m_vecChannel[m_iCurChannel].pClip->m_tInfo.vecCallback.size();
			++i)
			{
			m_vecChannel[m_iCurChannel].pClip->m_tInfo.vecCallback[i]->bCall = false;
			}*/
		}

		float	fAnimationTime = m_fAnimationGlobalTime +
			m_pCurClip->fStartTime;

		int	iStartFrame = m_pCurClip->iStartFrame;
		int	iEndFrame = m_pCurClip->iEndFrame;

		//int	iFrameIndex = (int)(fAnimationTime * m_pCurClip->iFrameMode);
		// iFrameIndex : �ð����� Ű���������� �������
		int	iFrameIndex = (int)(fAnimationTime / m_pCurClip->fFrameTime);

		if (m_bEnd)
		{
			switch (m_pCurClip->eOption)
			{
			case AO_LOOP:
				iFrameIndex = iStartFrame;
				break;
			case AO_ONCE_DESTROY:
				m_pGameObject->Active(false);
				break;
			}
		}

		else
		{
			int	iNextFrameIndex = iFrameIndex + 1;

			m_pCurClip->iChangeFrame = iFrameIndex;

			if (iNextFrameIndex > iEndFrame)
				iNextFrameIndex = iStartFrame;

			// �� ����ŭ �ݺ��Ѵ�.
			for (size_t i = 0; i < m_vecBones.size(); ++i)
			{
				// Ű�������� ���� ���
				if (m_pCurClip->vecKeyFrame[i]->vecKeyFrame.empty())
				{
					*m_vecBoneMatrix[i] = *m_vecBones[i]->matBone;
					continue;
				}

				const PKEYFRAME pCurKey = m_pCurClip->vecKeyFrame[i]->vecKeyFrame[iFrameIndex];
				const PKEYFRAME pNextKey = m_pCurClip->vecKeyFrame[i]->vecKeyFrame[iNextFrameIndex];

				// ���� �������� �ð��� ���´�.
				double	 dFrameTime = pCurKey->dTime;

				float	fPercent = (float)((fAnimationTime - dFrameTime) / m_pCurClip->fFrameTime);

				XMVECTOR	vS = XMVectorLerp(pCurKey->vScale.Convert(),
					pNextKey->vScale.Convert(), fPercent);
				XMVECTOR	vT = XMVectorLerp(pCurKey->vPos.Convert(),
					pNextKey->vPos.Convert(), fPercent);
				XMVECTOR	vR = XMQuaternionSlerp(pCurKey->vRot.Convert(),
					pNextKey->vRot.Convert(), fPercent);

		
				m_vecBlendInfo[i].vPos = vT;
				m_vecBlendInfo[i].vScale = vS;
				m_vecBlendInfo[i].vRot = vR;

				XMVECTOR	vZero = XMVectorSet(0.f, 0.f, 0.f, 1.f);

				Matrix	matBone = XMMatrixAffineTransformation(vS, vZero,
					vR, vT);


				// �θ��� ���� ���
				if (true == m_pGameObject->ParentEnable() && true == m_bParentBone)
				{
					matBone *= matParent;
				}


				*m_vecBones[i]->matBone = matBone;

				matBone = *m_vecBones[i]->matOffset * matBone;
				*m_vecBoneMatrix[i] = matBone;
			}
		}
	}

	if (!m_bEnd)
	{
		D3D11_MAPPED_SUBRESOURCE	tMap = {};
		_CONTEXT->Map(m_pBoneTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &tMap);

		PMatrix	pMatrix = (PMatrix)tMap.pData;

		for (size_t i = 0; i < m_vecBoneMatrix.size(); ++i)
		{
			pMatrix[i] = *m_vecBoneMatrix[i];
		}

		_CONTEXT->Unmap(m_pBoneTexture, 0);
	}

	return 0;
}

int Animation::LateUpdate(float fTime)
{
	return 0;
}

int Animation::Collision(float fTime)
{
	return 0;
}

int Animation::PrevRender(float fTime)
{
	// �����ϱ� ���� �ִϸ��̼� �������� ���̴��� �Ѱ��ش�.
	_CONTEXT->VSSetShaderResources(3, 1, &m_pBoneResourceView);

	return 0;
}

int Animation::PrevShadowRender(Matrix _matLightView, Matrix _matLightProj, float _fTime)
{
	// �����ϱ� ���� �ִϸ��̼� �������� ���̴��� �Ѱ��ش�.
	_CONTEXT->VSSetShaderResources(3, 1, &m_pBoneResourceView);
	return 0;
}

int Animation::Render(float fTime)
{
	return 0;
}

Animation * Animation::Clone() const
{
	return new Animation(*this);
}

int Animation::GetParentBoneIndex()
{
	return m_iParentBoneNum;
}

void Animation::SetParentBoneName(std::string _ParentBone)
{
	GameObject* pParent = m_pGameObject->GetParent();
	Animation* pParentAni = pParent->FindComponentFromType<Animation>(CT_ANIMATION);

	if (nullptr == pParent || nullptr == pParentAni || NULL == pParentAni->FindBone(_ParentBone))
	{
		SAFE_RELEASE(pParentAni);
		return;
	}

	m_strParentBone = _ParentBone;
	m_iParentBoneNum = pParentAni->FindBoneIndex(_ParentBone);
	m_bParentBone = true;
	SAFE_RELEASE(pParentAni);
}

void Animation::DeleteParentBone()
{
	m_strParentBone = "";
	m_iParentBoneNum = -1;
	m_bParentBone = false;
}

bool Animation::ParentBoneEnable()
{
	return m_bParentBone;
}

void Animation::SetChangePivotBone(std::string _PivotBone)
{
	m_iChangePivotBoneNum = FindBoneIndex(_PivotBone);

	if (-1 == m_iChangePivotBoneNum)
	{
		m_strChangePivotBone = "";
		m_bChangePivot = false;
		return;
	}

	m_strChangePivotBone = _PivotBone;
	m_bChangePivot = true;
}

void Animation::MovePivotBone()
{
	//if (-1 != m_iChangePivotBoneNum && true == m_bChangePivot)
	//{
	//	Vector3 vEndPos;

	//	Matrix aaaaaa = *(m_vecBoneMatrix[m_iChangePivotBoneNum]);
	//	vEndPos = Vector3(aaaaaa._41, aaaaaa._42, aaaaaa._43);
	//	Vector3 vCurPos = m_pTransform->GetWorldPosition();

	//	vCurPos += vEndPos;
	//	m_pTransform->SetWorldPosition(vCurPos);
	//	m_bChangePivot = false;
	//}
}

void Animation::DeleteChangePivotBone()
{
	m_strChangePivotBone = "";
	m_iChangePivotBoneNum = -1;
	m_bChangePivot = false;
}
