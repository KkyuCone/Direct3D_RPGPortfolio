#pragma once
#include "Component.h"

ENGINE_BEGIN

// 텍스처 스플래팅용 구조체
// 스플래팅과 관련된 텍스처나 관련 알파맵을 Splat이라고 한다.
typedef struct ENGINE_DLL _tagSplatName
{
	std::vector<TCHAR*> vecDiffuseName;
	std::vector<TCHAR*> vecNormalName;
	std::vector<TCHAR*> vecSpecularName;
	std::vector<TCHAR*> vecAlphaName;
	std::vector<std::string> vecPathName;

	~_tagSplatName()
	{
		Safe_Delete_Array_VectorList(vecDiffuseName);
		Safe_Delete_Array_VectorList(vecNormalName);
		Safe_Delete_Array_VectorList(vecSpecularName);
		Safe_Delete_Array_VectorList(vecAlphaName);
		vecPathName.clear();
	}
}SplatName, *PSplatName;

class ENGINE_DLL LandScape : public Component
{
private:
	friend class GameObject;

protected:
	LandScape();
	LandScape(const LandScape& _Com);
	~LandScape();

private:
	std::vector<Vertex3D>	m_vecVertex;		// 지형 정점
	std::vector<int>		m_vecIndex;			// 지형 인덱스

	std::vector<Vector3>	m_vecFaceNormal;	// 면법선벡터들 모아놓기

	// 타일맵처럼 만들거임 ( 격자맵, 높이맵 )
	// 해당 지형은 xz축으로 평행하도록 만들고 y값을 조절해서 높이를 정한다.
	int						m_iNumX;			// X축쪽 
	int						m_iNumZ;

	LandScaleCBuffer		m_tCBuffer;			// 디테일맵(타일링과 관련)과 스플래팅 카운터 상수버퍼

	class Material*			m_pMaterial;

	SplatName				m_tSplatName;		// 스플랫을 가지고 있는 구조체(적용할것들 -> 상수버퍼용)
	class Texture*			m_pSplatDiffTex;
	class Texture*			m_pSplatNormTex;
	class Texture*			m_pSplatSpecTex;
	class Texture*			m_pSplatAlphaTex;

public:
	bool CreateLandScape(const std::string& _strName, 
		const char* _pFileName, const std::string& _strPathName = PATH_TEXTURE);

	void SetDetailLevel(float _fLevelX, float _fLevelZ);

	void ChangeBaseTexture(const TCHAR* _pDiffuseName,
		const TCHAR* _pNormalName,
		const TCHAR* _pSpeuclarName,
		const std::string& _strPathName = PATH_TEXTURE);

	void AddSplatTexture(const TCHAR* _pDiffuseName,
		const TCHAR* _pNormalName,
		const TCHAR* _pSpecualrName,
		const TCHAR* _pAlphaName,
		const std::string& _strPathName = PATH_TEXTURE);

	bool CreateSplatTexture();

	
private:
	void ComputeNormal();	
	void ComputeTangent();

public:
	virtual void Start();		
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	
	virtual int Render(float _fTime);
	virtual LandScape* Clone()	const;

};

ENGINE_END


#pragma region 텍스처 스플래팅 (Texture Splatting)

 //- 텍스처 스플래팅이란 ?
	//- 서페이스 상의 텍스처들을 알파맵을 사용하여 서로 블렌딩 하는 방식 ( 검정색 부위는 그리지 X )
	//- 이때 텍스처나 관련 알파맵  -> 스플랫
	//- 터레인(지형)이 나뉘는 단위 -> 청크

 //- 레이어 형태
	//- 알파맵은 각 텍스처들 당 노이즈를 적용하여 하나의 알파맵으로 만들어야 함.
	//- <필요한것>
	//  1. 알파맵
	//  2. 렌더링 될 텍스처들
	//  3. 높이맵
	//- 픽셀셰이더에서 적용한다. 
	//-> 렌더링될 텍스처들을 하나로 묶는다(알파맵도)->ArrayTexture필요

// https://m.blog.naver.com/PostView.nhn?blogId=sorkelf&logNo=40151055590&proxyReferer=https%3A%2F%2Fwww.google.com%2F
#pragma endregion
