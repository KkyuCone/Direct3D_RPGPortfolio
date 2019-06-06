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

		// 키프레임
		size_t KeyFrameCount = pClip->vecKeyFrame.size();

		for (size_t i = 0; i < KeyFrameCount; ++i)
		{
			++(pClip->vecKeyFrame[i]->iRefCount);
		}

		// 콜백


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

// 너무 큰 본행렬들을 전부를 셰이더에 넣어줄 수 없다. ( 레지스터에.. )
// 그래서 Texture2D로 넣어서 넘겨주기 위해서
// 텍스처를 생성한다. (로드할때 생성함)
bool Animation::CreateBoneTexture()
{
	SAFE_RELEASE(m_pBoneTexture);
	D3D11_TEXTURE2D_DESC	tDesc = {};
	tDesc.ArraySize = 1;
	tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tDesc.Usage = D3D11_USAGE_DYNAMIC;						// CPU 접근이 가능해야함
	tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// 행렬 넣을려구.. 32(4바이트)씩 사용
	tDesc.Height = 1;										// 1개 
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

// fbx로드할때 사용
void Animation::AddClip(ANIMATION_OPTION _eOption, _tagFbxAnimationClip * _pClip)
{
	PANIMATIONCLIP	pAnimClip = FindClip(_pClip->strName);

	// 해당 애니메이션이 있으면 return (있으면 추가할 필요가 없으니까)
	if (nullptr != pAnimClip)
		return;

	pAnimClip = new ANIMATIONCLIP;

	// 인자로 들어온 애니메이션 옵션정보를 설정한다.
	pAnimClip->eOption = _eOption;
	pAnimClip->strName = _pClip->strName;

	pAnimClip->iChangeFrame = 0;

	// FBXANIMATIONCLIP에 있는 starttime 과 endtime 을 이용하여 keyframe 을 얻어온다.
	pAnimClip->iStartFrame = (int)(_pClip->tStart.GetFrameCount(_pClip->eTimeMode));
	pAnimClip->iEndFrame = (int)(_pClip->tEnd.GetFrameCount(_pClip->eTimeMode));
	pAnimClip->iFrameLength = pAnimClip->iEndFrame - pAnimClip->iStartFrame;

	// 시간 정보를 저장해준다.
	pAnimClip->fStartTime = 0.0f;
	pAnimClip->fEndTime = (float)(pAnimClip->fPlayTime);
	pAnimClip->fTimeLength = (float)(pAnimClip->fPlayTime);

	pAnimClip->fFrameTime = pAnimClip->fPlayTime / pAnimClip->iFrameLength;

	// 키 프레임 수만큼 반복하며 각각의 프레임을 보간할 행렬 정보를 위치, 크기, 회전정보로
	// 뽑아온다.
	for (size_t i = 0; i < _pClip->vecBoneKeyFrame.size(); ++i)
	{
		PBONEKEYFRAME	pBoneKeyFrame = new BONEKEYFRAME;

		pBoneKeyFrame->iBoneIndex = _pClip->vecBoneKeyFrame[i].iBoneIndex;

		pAnimClip->vecKeyFrame.push_back(pBoneKeyFrame);

		// 아래부터 키프레임 정보를 저장한다.
		pBoneKeyFrame->vecKeyFrame.reserve(_pClip->vecBoneKeyFrame[i].vecKeyFrame.size());

		for (size_t j = 0; j < _pClip->vecBoneKeyFrame[i].vecKeyFrame.size(); ++j)
		{
			PKEYFRAME	pKeyFrame = new KEYFRAME;

			pKeyFrame->dTime = j * pAnimClip->fFrameTime;

			// 현재 본의 키 프레임에 해당하는 행렬 정보를 얻어온다.
			FbxAMatrix	mat = _pClip->vecBoneKeyFrame[i].vecKeyFrame[j].matTransform;

			FbxVector4	vPos, vScale;
			FbxQuaternion	qRot;

			// 행렬로부터 위치, 크기, 회전 정보를 얻어온다.
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

	m_AddClipNameList.clear();				//fbx 로드할때니까 전체 리스트를 clear하고 새로 넣어준다.

	m_AddClipNameList.push_back(pAnimClip->strName);
}

// 내가 툴에서 클립을 추가할때 호출하는 함수 
// 내가 모두 지정해줘야함
// 모든 값이 필요하다 ( 클립 이름, 클립 옵션, 애니메이션 시작 프레임,  끝프레임, 플레이 시간, (내가)넣어줄 키프레임 벡터 즉 툴전용 키프레임 벡터)  
void Animation::AddClip(const std::string & _strName,
	ANIMATION_OPTION _eOption, 
	int _iStartFrame, 
	int _iEndFrame, 
	float _fPlayTime,
	const std::vector<PBONEKEYFRAME>& _vecFrame)
{
	PANIMATIONCLIP	pAnimClip = FindClip(_strName);

	// 얘또한 해당 클립이름이 이미 존재한다면 만들기 X
	if (nullptr != pAnimClip)
		return;

	pAnimClip = new ANIMATIONCLIP;

	// 인자로 들어온 애니메이션 옵션정보를 설정한다.
	pAnimClip->eOption = _eOption;
	pAnimClip->strName = _strName;

	pAnimClip->iChangeFrame = 0;

	int	iLength = _iEndFrame - _iStartFrame;

	// FBXANIMATIONCLIP에 있는 starttime 과 endtime 을 이용하여 keyframe 을 얻어온다.
	pAnimClip->iStartFrame = 0;
	pAnimClip->iEndFrame = iLength;
	pAnimClip->iFrameLength = iLength;

	// 시간 정보를 저장해준다.
	pAnimClip->fStartTime = 0.f;
	pAnimClip->fEndTime = _fPlayTime;
	pAnimClip->fTimeLength = _fPlayTime;
	pAnimClip->fPlayTime = _fPlayTime;

	pAnimClip->fFrameTime = pAnimClip->fPlayTime / pAnimClip->iFrameLength;

	// 키 프레임 수만큼 반복하며 각각의 프레임을 보간할 행렬 정보를 위치, 크기, 회전정보로
	// 뽑아온다.
	for (size_t i = 0; i < _vecFrame.size(); ++i)
	{
		PBONEKEYFRAME	pBoneFrame = new BONEKEYFRAME;
		pAnimClip->vecKeyFrame.push_back(pBoneFrame);

		PBONEKEYFRAME	pClipFrame = _vecFrame[i];
		pBoneFrame->iBoneIndex = pClipFrame->iBoneIndex;
		pBoneFrame->iRefCount = 1;

		// 해당 키프레임이 있으면.. (키프레임값들 넣어준다.)
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

	// 애니메이션 클립을 추가하는데 
	// 오브젝트가 갖고있는 애니메이션이 아무런 애니메이션 클립들이 없다면
	// 처음 넣어준애가 DefaultClip으로 설정하고, 현재클립도 내가 처음으로 추가한 클립으로 설정한다.
	if (true == m_mapClip.empty())
	{
		m_pDefaultClip = pAnimClip;
		m_pCurClip = pAnimClip;
	}

	// 애니메이션 클립map에 만들어준 애니메이션 클립을 넣어준다.( 애니메이션 클립 이름, 만들어준 애니메이션 클립 )
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

// 매개변수로 들어오는 _vecKeyFrame에 현재 키프레임을 넣어준다.
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
	// strOrigin : 기존 애니메이션 
	// strChange : 바꿀 애니메이션
	std::unordered_map<std::string, PANIMATIONCLIP>::iterator	iter = m_mapClip.find(strOrigin);

	// 애니메이션이 없다면 return
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

// 본인덱스 ( 이름으로 찾기)
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

	// 해당 본이 없다면
	if (iBone == -1)
		return Matrix();

	return *m_vecBoneMatrix[iBone];
}

// 클립 바꾸기 ( 현재 애니메이션 -> 다른애니메이션으로 )
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

// 실제로 해당 경로에 바이너리파일로 저장하는 함수
bool Animation::SaveFromFullPath(const char * pFullPath)
{
	FILE*	pFile = NULL;

	fopen_s(&pFile, pFullPath, "wb");

	if (!pFile)
		return false;

	fwrite(&m_fChangeLimitTime, sizeof(float), 1, pFile);

	// 애니메이션 클립정보를 저장한다.
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

		// 애니메이션 클립 키를 저장한다.
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

	// 애니메이션 클립정보를 저장한다.
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

		// 애니메이션 클립 키를 저장한다.
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

	// ===================== 본 정보 저장 =====================
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

	// ===================== 본 정보 읽기 =====================
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


	// 삭제한 클립이 Default 클립과 같다면
	// 클립들 모음에서 맨 첫번째에 있는 클립을 Default클립으로 설정해준다.
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

	// 삭제한 클립이 현재 클립이라면
	// 현재 애니메이션을 Dfault클립으로 바꿔준다.
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

	// 애니메이션 클립을 추가한다.
	const std::vector<PFBXANIMATIONCLIP>* pvecClip = loader.GetClips();

	// 클립을 읽어온다.
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
	// 애니메이션 적용 이전 위치
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

	// 부모본이 있을 경우
	if (true == m_pGameObject->ParentEnable() && true == m_bParentBone)
	{
		GameObject* pParent = m_pGameObject->GetParent();
		Animation* pAni = pParent->FindComponentFromType<Animation>(CT_ANIMATION);
		PBONE matParBoneNum = pAni->FindBone(m_iParentBoneNum);
		matParent = *(matParBoneNum->matBone);

		SAFE_RELEASE(pAni);
	}

	// 모션이 변할때
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

		// 본 수만큼 반복한다.
		for (size_t i = 0; i < m_vecBones.size(); ++i)
		{
			// 키프레임이 없을 경우
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

			// 보간
			XMVECTOR	vS = XMVectorLerp(m_vecBlendInfo[i].vScale.Convert(),
				pNextKey->vScale.Convert(), fPercent);
			XMVECTOR	vT = XMVectorLerp(m_vecBlendInfo[i].vPos.Convert(),
				pNextKey->vPos.Convert(), fPercent);
			XMVECTOR	vR = XMQuaternionSlerp(m_vecBlendInfo[i].vRot.Convert(),
				pNextKey->vRot.Convert(), fPercent);

			XMVECTOR	vZero = XMVectorSet(0.f, 0.f, 0.f, 1.f);

			// XMMatrixAffineTransformation( 크기, 회전중심을 식별하는 점, 회전, 위치 )
			Matrix	matBone = XMMatrixAffineTransformation(vS, vZero,
				vR, vT);

			//Vector4 vTMove = vT.m128_f32;


			// 부모본이 있을 경우
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

	// 기존 모션이 계속 동작될때
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
		// iFrameIndex : 시간값을 키프레임으로 갖고오기
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

			// 본 수만큼 반복한다.
			for (size_t i = 0; i < m_vecBones.size(); ++i)
			{
				// 키프레임이 없을 경우
				if (m_pCurClip->vecKeyFrame[i]->vecKeyFrame.empty())
				{
					*m_vecBoneMatrix[i] = *m_vecBones[i]->matBone;
					continue;
				}

				const PKEYFRAME pCurKey = m_pCurClip->vecKeyFrame[i]->vecKeyFrame[iFrameIndex];
				const PKEYFRAME pNextKey = m_pCurClip->vecKeyFrame[i]->vecKeyFrame[iNextFrameIndex];

				// 현재 프레임의 시간을 얻어온다.
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


				// 부모본이 있을 경우
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
	// 렌더하기 전에 애니메이션 정보들을 셰이더로 넘겨준다.
	_CONTEXT->VSSetShaderResources(3, 1, &m_pBoneResourceView);

	return 0;
}

int Animation::PrevShadowRender(Matrix _matLightView, Matrix _matLightProj, float _fTime)
{
	// 렌더하기 전에 애니메이션 정보들을 셰이더로 넘겨준다.
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
