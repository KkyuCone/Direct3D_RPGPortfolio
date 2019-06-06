#pragma once
#include "..\Reference.h"
#include "DirectXTex.h"

// #pragma comment는 현재 프로젝트경로부터 시작함 ㅇㅅㅇ 
#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "DirectXTex_Debugx64")
#else
#pragma comment(lib, "DirectXTex_Releasex64")
#endif // _DEBUG        
#else
#ifdef _DEBUG
#pragma comment(lib, "DirectXTex_Debugx32")
#else
#pragma comment(lib, "DirectXTex_Releasex32")
#endif // _DEBUG
#endif // _WIN64

ENGINE_BEGIN

class ENGINE_DLL Texture : public Reference
{
private:
	friend class ResourcesManager;

public:
	Texture();
	~Texture();

private:
	// ScratchImage : 픽셀에 대한 정보만 있음 (이미지구조)
	// 즉 데이터 정보만 갖고 있기때문에 눈에 보이는 이미지는 아니다.
	// 눈에보이는 이미지가 되기 위해선 ShaderResourceView를 사용해야한다. (셰이더에 연결해야함)
	std::vector<ScratchImage*>	m_vecImage;
	ID3D11ShaderResourceView*	m_pShaderResourceView;		// 이미지를 화면에 출력하려면 셰이더에연결시켜야한다.
	std::vector<TCHAR*>			m_vecFullPath;				// 해당 텍스처 전체 경로

public:
	const std::vector<TCHAR*>* GetFullPath() const;

public:
	// Texture 1개
	bool LoadTexture(const std::string& _strName, const TCHAR* _pFileName,
		const std::string& _strPathKey);
	bool LoadTextureFromFullPath(const std::string& _strName, const TCHAR* _pFullPath);

	// Texture 여러개
	bool LoadTexture(const std::string& _strName, const std::vector<TCHAR*>& _vecFileName,
		const std::string& _strPathKey);
	bool LoadTextureFromFullPath(const std::string& _strName, const std::vector<TCHAR*>& _vecFullPath);

	// 이미지 정보를 어떤 셰이더에 셋팅할것인지, 레지스터번호를 꼭 넘겨야한다.
	void VSSetShader(int _iRegister);
	void PSSetShader(int _iRegister);

public:
	ID3D11ShaderResourceView* GetTextureSRV();

private:
	bool CreateShaderResourceView();
	bool CreateShaderResourceViewArray();
};

ENGINE_END

