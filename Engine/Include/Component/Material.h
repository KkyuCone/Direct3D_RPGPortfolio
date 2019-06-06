#pragma once
#include "Component.h"

ENGINE_BEGIN
#pragma region TextureSet
// 픽셀 색상을 가져올 때 샘플러를 사용하는데 이때 색상을 가져오는 정보에 대해 넣어준다.
// 점으로 가져올지 선형으로 가져올지 비등방성으로 가져올지..
// 선형필터링으로 계단현상(앨리어싱)을 완화할 수 있다.  ( 안티앨리어싱)
// 비등방성으로는 부드럽게 할 수 있지만 연산량이 증가함(안티앨리어싱). 점은 그냥 그대로 가져오는것
typedef struct ENGINE_DLL _tagTextureSet
{
	class Texture*	pTexture;
	int				iTextureRegister;
	//class Sampler*	pSampler;				// 리소스매니저에서 넣어줬기때문에 안갖고있고 셰이더에서 골라서 쓰셈
	//int				iSamplerRegister;

	_tagTextureSet()
		: pTexture(nullptr)
		, iTextureRegister(0)
	{

	}
}TextureSet, *PTextureSet;

#pragma endregion

#pragma region MaterialSet

typedef struct ENGINE_DLL _tagMaterialSubSet
{
	MaterialCBuffer	tMaterialData;
	std::unordered_map<std::string, PTextureSet>	mapTextureSet;			// 묶어서 사용할거임
	//PTextureSet	pDiffuse;
	//PTextureSet pNormal;
	//PTextureSet pSpecular;
}MaterialSubSet, *PMaterialSubSet;

#pragma endregion

#pragma region Material Container

typedef struct ENGINE_DLL _tagMaterialContainer
{
	std::vector<PMaterialSubSet> vecSubSet;
}MaterialContainer, *PMaterialContainer;

#pragma endregion

// 재질(Material) : 빛이 물체의 표면과 상호작용하는 방식을 결정하는 속성들의 집합
class ENGINE_DLL Material : public Component
{
private:
	friend class GameObject;
	friend class Mesh;

public:
	Material();
	Material(const Material& _Component);
	virtual ~Material();

private:
	std::vector<PMaterialContainer>	m_vecMaterialContainer;

public:
	void SetColor(const Vector4& _vDiffuse, const Vector4& _vAmbient
		, const Vector4& _vSpecualr, const Vector4& _vEmissive, float _fSpecularPower, UINT _iContainer = 0, UINT _iSubSet = 0);

	// EmissiveColor 셋팅
	void SetEmissiveColor(float _fEmissive);		// 강도(세기)
	void SetEmissiveColor(int _iContainer, int _iSubSet, float _fEmissive);

	// 텍스처 셋팅 -  Texture, ArrayTexture
	void AddTextureSet(int _iContainer, int _iSubSet,
		int _iTextureRegister, const std::string& _strTextureName, 
		const TCHAR* _pFileName = nullptr,
		const std::string& _strPathName = PATH_TEXTURE);

	void AddTextureSet(int _iContainer, int _iSubSet,
		int _iTextureRegister,class Texture* _pTexture);			//애니메이션 관련

	void AddTextureSetFromFullPath(int _iContainer, int _iSubSet,
		int _iTextureRegister, const std::string& _strTextureName,
		const TCHAR* _pFullPath = nullptr);

	void AddTextureSetArray(int _iContainer, int _iSubSet,
		int _iTextureRegister, const std::string& _strTextureName,
		const std::vector<TCHAR*>* _vecFileName = nullptr,
		const std::string& _strPathName = PATH_TEXTURE);

	void AddTextureSetArrayFromFullPath(int _iContainer, int _iSubSet,
		int _iTextureRegister, const std::string& _strTextureName,
		const std::vector<TCHAR*>* _vecFullPath = nullptr);

	// 삭제
	void DeleteTextureSet(int _iContainer, int _iSubSet, const std::string& _strName);
	void DeleteTextureSet(int _iContainer, int _iSubSet);			// 애니메이션관련  -> 전체 다지움

	// 텍스처 바꾸기 -  Texture, ArrayTexture
	void ChangeTextureSet(int _iContainer, int _iSubSet,
		int _iTextureRegister, const std::string& _strOriTextuerName, 
		const std::string& _strChangeTextureName,
		const TCHAR* _pFileName = nullptr,
		const std::string& _strPathName = PATH_TEXTURE);

	void ChangeTextureSet(int _iContainer, int _iSubSet,
		int _iTextureRegister, const std::string& _strOriTextuerName,
		class Texture* _pTexture);						// 애니메이션 관련

	void ChangeTextureSetFromFullPath(int _iContainer, int _iSubSet,
		int _iTextureRegister, const std::string& _strOriTextuerName,
		const std::string& _strChangeTextureName,
		const TCHAR* _pFullPath = nullptr);

	void ChangeTextureSetArray(int _iContainer, int _iSubSet,
		int _iTextureRegister, const std::string& _strOriTextuerName,
		const std::string& _strChangeTextureName,
		const std::vector<TCHAR*>* _vecFullPath = nullptr,
		const std::string& _strPathname = PATH_TEXTURE);

	void ChangeTextureSetArrayFromFullPath(int _iContainer, int _iSubSet,
		int _iTextureRegister, const std::string& _strOriTextuerName,
		const std::string& _strChangeTextureName,
		const std::vector<TCHAR*>* _vecFullPath = nullptr);


	// Bump(Normal) 텍스처, Sepcular텍스처 
	void BumpTextureEnable(int _iContainer, int _iSubSet);
	void SepcularTextureEnable(int _iContainer, int _iSubSet);

private:
	PTextureSet FindTextureSet(int _iConatienr, int _iSubSet,
		const std::string& _strName);

public:
	// 스키닝
	void Skinning(int _iContainer, int _iSubSet);

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출, 즉 레이어에서 호출해준다.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// 렌더를 하기 직전에 호출해주는 함수ㅔㄱㅁ
	virtual int Render(float _fTime);
	virtual int RenderShadow(float _fTime);
	virtual Material* Clone()	const;

public:
	// 머티리얼에서 셰이더 셋팅하기
	void SetShader(int _iContainer, int _iSubSet);

	virtual void Save(FILE* _pFile);
	virtual void Load(FILE* _pFile);
	virtual void SaveFromPath(const char* _pFileName, const std::string& _strPathKey = PATH_DATA);
	virtual void LoadFromPath(const char* _pFileName, const std::string& _strPathKey = PATH_DATA);

private:
	void SaveTextureSet(FILE* _pFile, PTextureSet _pTexture);
	void LoadTextureSet(FILE* _pFile, PTextureSet* _ppTexture);
};

ENGINE_END
