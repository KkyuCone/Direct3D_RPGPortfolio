#pragma once
#include "Component.h"

ENGINE_BEGIN

#pragma region 2DAnimation을 위한 구조체들

// 애니메이션을 위한 텍스처 UV 좌표
typedef struct ENGINE_DLL _tagTextureCoord
{
	Vector2 vStart;
	Vector2 vEnd;
}TextureCoord, *PTextureCoord;


// 애니메이션 프레임 클립
typedef struct ENGINE_DLL _tagAnimationFrameClip
{
	std::string					strName;
	ANIMATION_FRAME_TYPE		eType;
	ANIMATION_OPTION			eOption;
	int							iFrame;
	Vector2						vTextureSize;
	std::vector<TextureCoord>	vecCoord;
	float						fPlayTime;
	float						fPlayLimitTime;
	float						fFrameTime;
	class Texture*				pTexture;
	int							iRegister;

}AnimationFrameClip, *PAnimationFramClip;

#pragma endregion


// 클래스
class ENGINE_DLL AnimationFrame : public Component
{
private:
	friend class GameObject;

protected:
	AnimationFrame();
	AnimationFrame(const AnimationFrame& _Com);
	~AnimationFrame();

private:
	std::unordered_map<std::string, PAnimationFramClip>	m_mapClip;			// 애니메이션들 모음
	PAnimationFramClip		m_pCurClip;						// 현재 애니메이션 클릭
	PAnimationFramClip		m_pDefaultClip;					// 기본 클립
	AnimationFrameCBuffer	m_tCBuffer;						// 애니메이션 상수버퍼
	class Material*			m_pMaterial;

public:
	// 아틀라스 텍스처 만들기
	bool CreateClip(const std::string& _strName,				// 애니메이션 클립 이름
		ANIMATION_OPTION _eOption,								// 애니메이션 옵션
		const Vector2& _vTextureSize,							// 텍스처 사이즈
		const std::vector<TextureCoord>& _vecCoord,				// 텍스처 UV 좌표
		float _fLimitTime,										// 제한시간
		int _iTextureRegister,									// 해당 텍스처 레지스터(셰이더로 넘겨줄..)
		const std::string& _strTextureName,						// 사용할 아틀라스 텍스처이름
		const TCHAR*	_pFileName,								// 파일 이름
		const std::string& _strPathName = PATH_TEXTURE);		// 해당 파일 경로

	// 프레임 텍스처 ( 여러개의 텍스처로 된 애들 ) 만들기
	bool CreateClip(const std::string& _strName,				// 애니메이션 클립 이름
		ANIMATION_OPTION _eOption,								// 애니메이션 옵션
		const Vector2& _vTextureSize,							// 텍스처 사이즈
		const std::vector<TextureCoord>& _vecCoord,				// 텍스처 UV 좌표
		float _fLimitTime,										// 제한시간
		int _iTextureRegister,									// 해당 텍스처 레지스터(셰이더로 넘겨줄..)
		const std::string& _strTextureName,						// 사용할 프레임 텍스처이름 -> 텍스처에 로드된 애들
		const std::vector<TCHAR*>&	_vecFileName,				// 파일 이름들.. - 벡터
		const std::string& _strPathName = PATH_TEXTURE);		// 해당 파일 경로

public:
	bool SetCurrentClip(const std::string& _strName);
	bool SetDefaultClip(const std::string& _strName);
	bool ChangeClip(const std::string& _strName);

public:
	PAnimationFramClip GetCurClip();

private:
	PAnimationFramClip FindClip(const std::string& _strName);

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출, 즉 레이어에서 호출해준다.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// 렌더를 하기 직전에 호출해주는 함수ㅔㄱㅁ
	virtual int Render(float _fTime);
	virtual Component* Clone()	const;
};

ENGINE_END