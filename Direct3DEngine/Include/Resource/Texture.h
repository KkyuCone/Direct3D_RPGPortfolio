#pragma once
#include "..\Reference.h"
#include "DirectXTex.h"

// #pragma comment�� ���� ������Ʈ��κ��� ������ ������ 
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
	// ScratchImage : �ȼ��� ���� ������ ���� (�̹�������)
	// �� ������ ������ ���� �ֱ⶧���� ���� ���̴� �̹����� �ƴϴ�.
	// �������̴� �̹����� �Ǳ� ���ؼ� ShaderResourceView�� ����ؾ��Ѵ�. (���̴��� �����ؾ���)
	std::vector<ScratchImage*>	m_vecImage;
	ID3D11ShaderResourceView*	m_pShaderResourceView;		// �̹����� ȭ�鿡 ����Ϸ��� ���̴���������Ѿ��Ѵ�.
	std::vector<TCHAR*>			m_vecFullPath;				// �ش� �ؽ�ó ��ü ���

public:
	const std::vector<TCHAR*>* GetFullPath() const;

public:
	// Texture 1��
	bool LoadTexture(const std::string& _strName, const TCHAR* _pFileName,
		const std::string& _strPathKey);
	bool LoadTextureFromFullPath(const std::string& _strName, const TCHAR* _pFullPath);

	// Texture ������
	bool LoadTexture(const std::string& _strName, const std::vector<TCHAR*>& _vecFileName,
		const std::string& _strPathKey);
	bool LoadTextureFromFullPath(const std::string& _strName, const std::vector<TCHAR*>& _vecFullPath);

	// �̹��� ������ � ���̴��� �����Ұ�����, �������͹�ȣ�� �� �Ѱܾ��Ѵ�.
	void VSSetShader(int _iRegister);
	void PSSetShader(int _iRegister);

public:
	ID3D11ShaderResourceView* GetTextureSRV();

private:
	bool CreateShaderResourceView();
	bool CreateShaderResourceViewArray();
};

ENGINE_END

