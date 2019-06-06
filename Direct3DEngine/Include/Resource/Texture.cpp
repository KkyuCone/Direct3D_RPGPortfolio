#include "Texture.h"
#include "..\PathManager.h"
#include "..\Device.h"

ENGINE_USING

Texture::Texture() : m_pShaderResourceView(nullptr)
{
}


Texture::~Texture()
{
	for (size_t i = 0; i < m_vecFullPath.size(); ++i)
	{
		SAFE_DELETE_ARRAY(m_vecFullPath[i]);
	}

	m_vecFullPath.clear();

	Safe_Delete_VectorList(m_vecImage);
	SAFE_RELEASE(m_pShaderResourceView);
}

const std::vector<TCHAR*>* Texture::GetFullPath() const
{
	return &m_vecFullPath;
}

// �ؽ�ó �ε��ϱ�( �ؽ�ó �̸�, �н� �̿��ؼ�) -  Texture
bool Texture::LoadTexture(const std::string & _strName, const TCHAR * _pFileName, const std::string & _strPathKey)
{
	const TCHAR* pPath = GET_SINGLETON(PathManager)->FindPath(_strPathKey);
	TCHAR strPath[MAX_PATH] = {};

	// �ش� �н��� �̸� �ε尡 �Ǿ������ʴٸ� �ε����ش�.
	// ������ �ε带 ���ִ°� �ƴϰ� �ε尡 �ȵǾ��ִµ� �ε��ϰ� �����ϱ� ���ڿ��� ä���ִ°�
	if (nullptr != pPath)
	{
		// lstrcpy( A,B) : B���ڿ��� A�� �����Ѵ�.
		lstrcpy(strPath, pPath);
	}
	lstrcat(strPath, _pFileName);

	return LoadTextureFromFullPath(_strName, strPath);
}

// new�������ϱ� ����������� delete�� ���������������? ������ (���߿� �ڵ� Ȯ��) -  Texture
bool Texture::LoadTextureFromFullPath(const std::string & _strName, const TCHAR * _pFullPath)
{
	SetTag(_strName.c_str());

	ScratchImage* pImage = new ScratchImage;
	TCHAR	strExtension[_MAX_EXT] = {};		// Ȯ����

	// SplitPath(��ü���, ����̺��� ���� ���ڿ�, ����̺��� ũ��, ������� ���� ���ڿ�, ũ��, ���ϸ�, ũ��)
	// Ȯ���ڴ� .����!  ex ) .png, 
	SplitPath(_pFullPath, nullptr, 0, nullptr, 0, nullptr, 0, strExtension, _MAX_EXT);

	char _strExtension[_MAX_EXT] = {};

	// �����ڵ带 ��Ƽ����Ʈ�� �ٲ۴�.
	WideCharToMultiByte(CP_ACP, 0, strExtension, -1, _strExtension, lstrlen(strExtension), 0, 0);

	// �ش� ���ڿ��� �빮�ڷ� �ٲ۴�.
	_strupr_s(_strExtension);

	// strcmp�� �� ���ڿ��� ������ 0�� ��ȯ�Ѵ�.
	if (0 == strcmp(_strExtension, ".DDS"))
	{
		// �ؽ�ó�� �ε��Ѵ�.
		if (FAILED(LoadFromDDSFile(_pFullPath, DDS_FLAGS_NONE, nullptr, *pImage)))
		{
			delete pImage;
			return false;
		}
	}
	else if (0 == strcmp(_strExtension, ".TGA"))
	{
		if (FAILED(LoadFromTGAFile(_pFullPath, nullptr, *pImage)))
		{
			delete pImage;
			return false;
		}
	}
	else
	{
		// DDS, TGA���� ���� Ȯ���ڵ� (PNG, JPGE..��)
		if (FAILED(LoadFromWICFile(_pFullPath, WIC_FLAGS_NONE, nullptr, *pImage)))
		{
			delete pImage;
			return false;
		}
	}
	
	// ������ ��� (�ش� �ؽ�ó(�̹���)�� �ִ� ���)
	TCHAR* pSavePath = new TCHAR[MAX_PATH];

	memset(pSavePath, 0, sizeof(TCHAR) * MAX_PATH);

	lstrcpy(pSavePath, _pFullPath);

	m_vecFullPath.push_back(pSavePath);

	// �ε忡 �����ߴٸ� m_vecImage�� �־��ش�. ( ���̹��� )
	m_vecImage.push_back(pImage);

	return 	CreateShaderResourceView();
}

// Texture Array
bool Texture::LoadTexture(const std::string & _strName, const std::vector<TCHAR*>& _vecFileName, const std::string & _strPathKey)
{
	const TCHAR* pPath = GET_SINGLETON(PathManager)->FindPath(_strPathKey);

	std::vector<TCHAR*> vecFullPath;
	vecFullPath.reserve(_vecFileName.size());

	// �ش� �н��� �̸� �ε尡 �Ǿ������ʴٸ� �ε����ش�.
	// ������ �ε带 ���ִ°� �ƴϰ� �ε尡 �ȵǾ��ִµ� �ε��ϰ� �����ϱ� ���ڿ��� ä���ִ°�

	for (size_t i = 0; i < _vecFileName.size(); ++i)
	{
		TCHAR* pFullPath = new TCHAR[MAX_PATH];
		memset(pFullPath, 0, sizeof(TCHAR) * MAX_PATH);

		if (nullptr != pPath)
		{
			// lstrcpy( A,B) : B���ڿ��� A�� �����Ѵ�.
			lstrcpy(pFullPath, pPath);
		}
		lstrcat(pFullPath, _vecFileName[i]);

		vecFullPath.push_back(pFullPath);
	}

	if (false == LoadTextureFromFullPath(_strName, vecFullPath))
	{
		// ������ ��쿡�� �����ش�.
		Safe_Delete_Array_VectorList(vecFullPath);
	}

	// �־��صڿ��� �����ش�
	Safe_Delete_Array_VectorList(vecFullPath);

	return true;
}

// Texture Array
bool Texture::LoadTextureFromFullPath(const std::string & _strName, const std::vector<TCHAR*>& _vecFullPath)
{
	SetTag(_strName.c_str());

	for (size_t i = 0; i < _vecFullPath.size(); ++i)
	{
		ScratchImage* pImage = new ScratchImage;
		TCHAR	strExtension[_MAX_EXT] = {};		// Ȯ����

		// SplitPath(��ü���, ����̺��� ���� ���ڿ�, ����̺��� ũ��, ������� ���� ���ڿ�, ũ��, ���ϸ�, ũ��)
		// Ȯ���ڴ� .����!  ex ) .png, 
		SplitPath(_vecFullPath[i], nullptr, 0, nullptr, 0, nullptr, 0, strExtension, _MAX_EXT);

		char _strExtension[_MAX_EXT] = {};

		// �����ڵ带 ��Ƽ����Ʈ�� �ٲ۴�.
		WideCharToMultiByte(CP_ACP, 0, strExtension, -1, _strExtension, lstrlen(strExtension), 0, 0);

		// �ش� ���ڿ��� �빮�ڷ� �ٲ۴�.
		_strupr_s(_strExtension);

		// strcmp�� �� ���ڿ��� ������ 0�� ��ȯ�Ѵ�.
		if (0 == strcmp(_strExtension, ".DDS"))
		{
			// �ؽ�ó�� �ε��Ѵ�.
			if (FAILED(LoadFromDDSFile(_vecFullPath[i], DDS_FLAGS_NONE, nullptr, *pImage)))
			{
				delete pImage;
				return false;
			}
		}
		else if (0 == strcmp(_strExtension, ".TGA"))
		{
			if (FAILED(LoadFromTGAFile(_vecFullPath[i], nullptr, *pImage)))
			{
				delete pImage;
				return false;
			}
		}
		else
		{
			// DDS, TGA���� ���� Ȯ���ڵ� (PNG, JPGE..��)
			if (FAILED(LoadFromWICFile(_vecFullPath[i], WIC_FLAGS_NONE, nullptr, *pImage)))
			{
				delete pImage;
				return false;
			}
		}

		// ������ ��� (�ش� �ؽ�ó(�̹���)�� �ִ� ���)
		TCHAR* pSavePath = new TCHAR[MAX_PATH];

		memset(pSavePath, 0, sizeof(TCHAR) * MAX_PATH);

		lstrcpy(pSavePath, _vecFullPath[i]);

		m_vecFullPath.push_back(pSavePath);

		// �ε忡 �����ߴٸ� m_vecImage�� �־��ش�. ( ���̹��� )
		m_vecImage.push_back(pImage);
	}

	return 	CreateShaderResourceViewArray();
}

void Texture::VSSetShader(int _iRegister)
{
	_CONTEXT->VSSetShaderResources(_iRegister, 1, &m_pShaderResourceView);
}

void Texture::PSSetShader(int _iRegister)
{
	_CONTEXT->PSSetShaderResources(_iRegister, 1, &m_pShaderResourceView);
}

ID3D11ShaderResourceView * Texture::GetTextureSRV()
{
	return m_pShaderResourceView;
}

// Texture 1���϶�
bool Texture::CreateShaderResourceView()
{
	// CreateShaderResourceView( ����̽�, ScratchImage ����, �̹��� ����, ��Ÿ������, ShaderResourceView)
	if (FAILED(DirectX::CreateShaderResourceView(_DEVICE, m_vecImage[0]->GetImages()
		, m_vecImage[0]->GetImageCount(), m_vecImage[0]->GetMetadata(),
		&m_pShaderResourceView)))
	{
		return false;
	}

	return true;
}

// Texture�� �������϶�
bool Texture::CreateShaderResourceViewArray()
{
	// Array Texture�� �����ϴ� �ڵ尡 �����Ѵ�. ������ �װ� ���� ��
	
	// �� �ؽ�ó ���Ҹ� ���Ϸκ��� ���������� �־���Ѵ�.
	// �� �ؽ�ó���� GPU�� ����ϴ� ���� �ƴ�. 
	// �׳� ���Ͽ��� �̹��� �ڷḦ �ֱ� ���� ����
	// CPU���� �ڿ��� �о���ϴϱ� D3D11_USAGE_STAGING ����
	std::vector<ID3D11Texture2D*>		vecTexture;

	for (size_t i = 0; i < m_vecImage.size(); ++i)
	{
		ID3D11Texture2D*		pTexture = NULL;

		// CreateTextureEx -> Ex�� ������ Ȯ�� �Լ���� �����ϸ� �ȴ�.
		if (FAILED(DirectX::CreateTextureEx(_DEVICE, m_vecImage[i]->GetImages(),
			m_vecImage[i]->GetImageCount(),
			m_vecImage[i]->GetMetadata(),
			D3D11_USAGE_STAGING,
			0,
			D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE, 0, FALSE,
			(ID3D11Resource**)&pTexture)))
		{
			assert(false);
			return false;
		}

		vecTexture.push_back(pTexture);
	}

	// �ؽ�ó �迭�� �����Ѵ�.
	// �ؽ�ó �迭�� ��� ���Ҵ� ���İ� ũ�Ⱑ �����ؾ��Ѵ�.
	D3D11_TEXTURE2D_DESC tTextureDesc = {};
	vecTexture[0]->GetDesc(&tTextureDesc);

	// Array Texture�� ����� ���� Desc ����ü�� �����ϱ�
	// Array Texture�� ����, ���� ���̰� �Ȱ��ƾ� �Ѵ�. (�׷��� ��밡��)
	D3D11_TEXTURE2D_DESC tArrayTextureDesc = {};
	tArrayTextureDesc.Width = tTextureDesc.Width;
	tArrayTextureDesc.Height = tTextureDesc.Height;
	tArrayTextureDesc.MipLevels = tTextureDesc.MipLevels;
	tArrayTextureDesc.ArraySize = (UINT)(vecTexture.size());		// �迭 ����
	tArrayTextureDesc.Format = tTextureDesc.Format;					// ���� ����
	tArrayTextureDesc.SampleDesc.Count = 1;
	tArrayTextureDesc.SampleDesc.Quality = 0;
	tArrayTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	tArrayTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	// ������ ������ ������ �̿��ؼ� �� Array Texture�� �����Ѵ�.
	ID3D11Texture2D*	pArrayTexture = NULL;

	if (FAILED(_DEVICE->CreateTexture2D(&tArrayTextureDesc, NULL, &pArrayTexture)))
	{
		assert(false);
		return false;
	}

	// Array Texture�� �ȼ� ������ �ҷ��� �ؽ�ó�� �ȼ� ������ ä���ش�.  -> ����� 11��.3�ؽ�ó�迭 ����
	// ���� �ؽ�ó ���Ҹ� �ؽ�ó �迭�� �����Ѵ�.

	// ������ �ؽ�ó ���ҵ鿡 ����..(����)
	for (size_t i = 0; i < vecTexture.size(); ++i)
	{
		// ������ �Ӹ� ���ؿ� ���ؼ�..
		for (size_t mip = 0; mip < tTextureDesc.MipLevels; ++mip)
		{
			// �ؽ�ó�� Map ���ش�.
			D3D11_MAPPED_SUBRESOURCE	tMap = {};
			
			// Map : ���� ���ҽ��� ���Ե� �����Ϳ� ���� �����͸� �������� �ش� ���� ���ҽ��� ���� GPU �׼����� �ź��Ѵ�.
			_CONTEXT->Map(vecTexture[i],			// ID3D11Resource �������̽��� ������
				(UINT)mip,								// �������ҽ��� �ε��� ��ȣ
				D3D11_MAP_READ,						// ������ �����Ŵϱ�! Read
				0,									// GPU�� ������϶� CPU�� �����ϴ� �۾��� �����ϴ� �÷���, ���⼱ �Ⱦ��Ŵϱ� 0 ����
				&tMap);								// ���� �� ���� ���ҽ� �� ���� D3D11_MAPPED_SUBRESOURCE ����ü�� ���� ������

			// UpdateSubresource : CPU�� �޸𸮿��� �� ���� �޸𸮷� �ۼ��� ���� ���ҽ��� �����͸� ����
			// �� �κ��ڿ����� �ٸ� �κ��ڿ����� �޸� ������ �����Ѵ�.
			// 2���� �ؽ�ó�� ��� SrcDepthPItch�� �ʿ� X, 3���� �ؽ�ó�� ����.
			_CONTEXT->UpdateSubresource(pArrayTexture,					// pArrayTexture���⿡ ���� �־��ٰ��� , ����ڿ� ��ü
				D3D11CalcSubresource((UINT)mip, (UINT)i, tTextureDesc.MipLevels),	// ��� ���� ���ҽ��� ���� �ε��� (D3D11CalcSubresource -> �ؽ�ó�� ���� ������ �ε����� �����), ��� �ڿ����� ������ �κ��ڿ��� ����Ű�� �ε���
				NULL,													// NULL -> Offset���� ��� ���� ���ҽ��� ������ ���, ������ ��� �κ��ڿ��� ������ �����ϴ�  D3D11_BOX �ν��Ͻ��� ����Ű�� ������, �κ��ڿ� ��ü�� �����ϰ� ������ NULL����
				tMap.pData,												// �����ؼ� �־���.. �޸� �ҽ� �����Ϳ� ���� ������, ���� �ڷḦ ����Ű�� ������
				tMap.RowPitch,											// �ҽ� �������� �� �� ũ��, ���� �ڷ� �� ���� ����Ʈ ���� ũ��(����Ʈ����)
				tMap.DepthPitch);										// �ҽ� �������� ���� �����̽� ũ��, ���� �ڷ��� ���� ����(Depth slice) �ϳ��� ����Ʈ ���� ũ��.

			_CONTEXT->Unmap(vecTexture[i], (UINT)mip);		// Map�����ϱ� Unmap���� ��ٴ�.
		}
	}

	// ���̴� ���ҽ� ��
	// �ؽ�ó �迭�� ���� �ڿ� �並 �����Ѵ�.
	D3D11_SHADER_RESOURCE_VIEW_DESC	tViewDesc = {};
	tViewDesc.Format = tArrayTextureDesc.Format;
	tViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;			// �⺻ �ڿ��� �ڿ� ������ �����ؾ���
	tViewDesc.Texture2DArray.MostDetailedMip = 0;							// ����� ���� ���� �Ӹ� ������ �ε��� (0 �� -1 (MipLevels -1 ))
	tViewDesc.Texture2DArray.MipLevels = tArrayTextureDesc.MipLevels;
	tViewDesc.Texture2DArray.ArraySize = (UINT)(vecTexture.size());					// ����

	if (FAILED(_DEVICE->CreateShaderResourceView(pArrayTexture, &tViewDesc, &m_pShaderResourceView)))
	{
		assert(false);
		return false;
	}

	SAFE_RELEASE(pArrayTexture);			// ������ ���������ϱ� ���۷���ī���͸� ���ҽ�Ų��.

	Safe_Release_VectorList(vecTexture);	// �굵 ���� ���۷��� ī���͸� ���ҽ�Ų��.
	
	return true;
}
