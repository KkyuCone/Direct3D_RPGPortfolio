#pragma once
#include "Component.h"

ENGINE_BEGIN

// 본
typedef struct ENGINE_DLL _tagBone
{
	std::string		strName;			// 본 이름
	int				iDepth;				// 본 차수(레벨, 트리구조임)
	int				iParentIndex;		// 부모 인덱스
	PMatrix			matOffset;			// 오프셋 행렬
	PMatrix			matBone;			// 원본 행렬 (변환 전의 3D맥스상의 공간 본행렬_
	std::list<class CBoneSocket*>	SocketList;	// 추후에 무기같은걸 들고 있을 경우 필요
	int				iRefCount;			// 여러 몬스터 객체가 있을 경우 본은 하나만 공유한다. 대신 각 객체마다 들고 있어야하므로 카운터를 세어준다.

	_tagBone() :
		iRefCount(1)
	{
	}
}BONE, *PBONE;

// 키프레임
typedef struct ENGINE_DLL _tagKeyFrame
{
	double	dTime;				// 각 키프레임에 해당하는 시간값
	Vector3	vPos;
	Vector3	vScale;
	Vector4	vRot;
}KEYFRAME, *PKEYFRAME;

// 애니메이션 키프레임
typedef struct ENGINE_DLL _tagBoneKeyFrame
{
	int		iBoneIndex;
	std::vector<PKEYFRAME>	vecKeyFrame;
	int			iRefCount;

	_tagBoneKeyFrame() :
		iRefCount(1)
	{
	}

	~_tagBoneKeyFrame()
	{
		Safe_Delete_VectorList(vecKeyFrame);
	}
}BONEKEYFRAME, *PBONEKEYFRAME;

typedef struct ENGINE_DLL _tagAnimationCallback
{
	int		iAnimationProgress;
	float	fAnimationProgress;
	std::function<void(float)> func;
	bool	bCall;
}ANIMATIONCALLBACK, *PANIMATIONCALLBACK;

typedef struct ENGINE_DLL _tagAnimationClip
{
	ANIMATION_OPTION	eOption;
	std::string				strName;
	float				fStartTime;
	float				fEndTime;
	float				fTimeLength;			// 애니메이션 구간(하나의 모션 Idle..Run같은..) 시간 길이 정보
	float				fFrameTime;				// 몇초마다 애니메이션 프레임이 갱신되어야 하는지에 대한 시간값
	float				fPlayTime;
	int					iStartFrame;
	int					iEndFrame;
	int					iFrameLength;
	int					iFrameMode;				// 보통 30프레임
	int					iChangeFrame;			// 다른 애니메이션으로 바꾸기 바로 직전의 현재 애니메이션 프레임
	std::vector<PBONEKEYFRAME>		vecKeyFrame;
	std::vector<PANIMATIONCALLBACK>	vecCallback;

	_tagAnimationClip() :
		eOption(AO_LOOP),
		strName(""),
		iFrameMode(0),
		fStartTime(0),
		fEndTime(0),
		fTimeLength(0),
		iStartFrame(0),
		iEndFrame(0),
		iFrameLength(0),
		fPlayTime(1.0f)
	{
	}

	~_tagAnimationClip()
	{
		for (size_t i = 0; i < vecKeyFrame.size(); ++i)
		{
			--(vecKeyFrame[i]->iRefCount);

			if (vecKeyFrame[i]->iRefCount == 0)
			{
				SAFE_DELETE(vecKeyFrame[i]);
			}
		}

		vecKeyFrame.clear();
		//Safe_Delete_VecList(vecCallback);
	}
}ANIMATIONCLIP, *PANIMATIONCLIP;

typedef struct ENGINE_DLL _tagBlendInfo
{
	Vector3	vScale;
	Vector3	vPos;
	Vector4	vRot;
}BlendInfo, *PBlendInfo;

//
class ENGINE_DLL Animation : public Component
{
private:
	friend class GameObject;
	friend class Mesh;

protected:
	Animation();
	Animation(const Animation& _Com);
	virtual ~Animation();

private:
	// 셰이더의 상수 레지스터는 수가 한계가 있다.
	// 즉, 너무 많은 본들을 셰이더에 넣을 수가 없다.
	// 이를 보완하기 위해 2가지 방법이 있는데
	// 1. 본정보들을 반씩 쪼개서 넣는 방법
	// 2. VTF(Vertex Texture Fatch)
	// 우리는 이중에서 VTF를 사용할건데, 본정보들을 텍스처에 넣고
	// 버텍스셰이더에서 픽셀 정보를 가져와서 애니메이션 본 정보들을 처리할 것이다.
	// 이는 셰이더 3.0부터 지원하며, 텍스처를 버퍼처럼 사용할 것이기 때문에
	// Texture2D와 ShaderResourceView가 필요하다. (셰이더에 넘겨줄려고)
	std::vector<PBONE>				m_vecBones;					// 본들 모음
	ID3D11Texture2D*				m_pBoneTexture;				// 본행렬들 정보를 갖고 있는 텍스처, 이걸 셰이더로 넘겨줘서 Skinning할때 사용할 거임	
	ID3D11ShaderResourceView*		m_pBoneResourceView;		// 위의 텍스처를 셰이더로 넘길 Shader Resource View
	std::vector<PMatrix>			m_vecBoneMatrix;			// 본행렬들 모음
	std::unordered_map<std::string, PANIMATIONCLIP>	m_mapClip;	// 애니메이션 클립들 모아둠. 해쉬처럼 바로빼서 사용할거기때문에 unordered_map사용

	PANIMATIONCLIP			m_pDefaultClip;						// Default 애니메이션 정보 ( 다른애니메이션이 끝난 후에 Default 애니메이션으로 돌아올 것임. (Idle같은..))
	PANIMATIONCLIP			m_pCurClip;							// 현재 애니메이션
	PANIMATIONCLIP			m_pNextClip;						// 다음 애니메이션

	std::list<std::string>	m_AddClipNameList;				// 애니메이션 클립 이름 목록 모음

	bool					m_bEnd;							// 애니메이션이 끝났는지 여부
	float					m_fAnimationGlobalTime;			// 애니메이션 누적시간
	float					m_fClipProgress;				// 현재 애니메이션 진행률 (어느정도 애니메이션이 진행되었는지)
	float					m_fChangeTime;					// 다른애니메이션으로 애니메이션을 바꿀때의 시간 ( 시간재는 변수 )
	float					m_fChangeLimitTime;				// 다른애니메이션으로 바꿀때의 제한시간 A애니메이션 -> B애니메이션으로 변경될때 몇초만에 변경해야하는지..

	// 모션 블렌딩
	std::vector<BlendInfo>		m_vecBlendInfo;

public:
	// Get 함수
	const std::list<std::string>*	GetAddClipNameList()		const;		// 애니메이션 클립 이름들을 모아둔 리스트 Get함수
	const std::unordered_map<std::string, PANIMATIONCLIP>* GetClips()	const;
	ID3D11ShaderResourceView** GetBoneTexture();
	Matrix GetBoneMatrix(const std::string& strBoneName);
	PANIMATIONCLIP GetCurrentClip()	const;					// 현재 애니메이션 클립 가져오기

public:
	// Find 함수
	PBONE			FindBone(const std::string& strBoneName);
	PBONE			FindBone(int _Index);
	int				FindBoneIndex(const std::string& strBoneName);
	PANIMATIONCLIP	FindClip(const std::string& strName);

	Matrix			FindIndexBoneMatrix(int _Index);

public:
	void AddBone(PBONE pBone);
	bool CreateBoneTexture();

	void AddClip(ANIMATION_OPTION eOption,
		struct _tagFbxAnimationClip* pClip);
	void AddClip(const std::string& _strName, ANIMATION_OPTION _eOption,
		int _iStartFrame, int _iEndFrame, float _fPlayTime,
		const std::vector<PBONEKEYFRAME>& _vecFrame);
	void AddClip(const TCHAR* _pFullPath);

	void AddClipFromMultibyte(const char* _pFullPath);

	void GetCurrentKeyFrame(std::vector<PBONEKEYFRAME>& _vecKeyFrame);

	bool IsAnimationEnd()	const;

public:
	void ChangeClipKey(const std::string& strOrigin, const std::string& strChange);
	bool ChangeClip(const std::string& strClip);

public:
	bool ModifyClip(const std::string& strKey, const std::string& strChangeKey,
		ANIMATION_OPTION eOption, int iStartFrame, int iEndFrame, float _fPlayTime,
		const std::vector<PBONEKEYFRAME>& _vecFrame);
	bool DeleteClip(const std::string& strKey);
	bool ReturnDefaultClip();

public:
	bool Save(const TCHAR* pFileName, const std::string& strPathKey = PATH_MESH);
	bool Save(const char* pFileName, const std::string& strPathKey = PATH_MESH);
	bool SaveFromFullPath(const TCHAR* pFullPath);
	bool SaveFromFullPath(const char* pFullPath);
	bool Load(const TCHAR* pFileName, const std::string& strPathKey = PATH_MESH);
	bool Load(const char* pFileName, const std::string& strPathKey = PATH_MESH);
	bool LoadFromFullPath(const TCHAR* pFullPath);
	bool LoadFromFullPath(const char* pFullPath);
	bool SaveBone(const TCHAR* pFileName, const std::string& strPathKey = PATH_MESH);
	bool SaveBone(const char* pFileName, const std::string& strPathKey = PATH_MESH);
	bool SaveBoneFromFullPath(const TCHAR* pFullPath);
	bool SaveBoneFromFullPath(const char* pFullPath);
	bool LoadBone(const TCHAR* pFileName, const std::string& strPathKey = PATH_MESH);
	bool LoadBone(const char* pFileName, const std::string& strPathKey = PATH_MESH);
	bool LoadBoneFromFullPath(const TCHAR* pFullPath);
	bool LoadBoneFromFullPath(const char* pFullPath);

private:
	void LoadFBXAnimation(const char* _pFullPath);

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int PrevShadowRender(Matrix _matLightView, Matrix _matLightProj, float _fTime);
	virtual int Render(float fTime);
	virtual Animation* Clone()	const;

	private:
		// 부모 적용
		bool m_bParentBone;
		int  m_iParentBoneNum;
		std::string m_strParentBone;

		// 피봇 위치변경때문에 필요한 본 
		bool m_bChangePivot;
		int m_iChangePivotBoneNum;
		std::string m_strChangePivotBone;

public:
	int GetParentBoneIndex();
	void SetParentBoneName(std::string _ParentBone);
	void DeleteParentBone();
	bool ParentBoneEnable();

	void SetChangePivotBone(std::string _PivotBone);
	void MovePivotBone();
	void DeleteChangePivotBone();

};

ENGINE_END