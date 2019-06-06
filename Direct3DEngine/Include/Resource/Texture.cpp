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

// 텍스처 로드하기( 텍스처 이름, 패스 이용해서) -  Texture
bool Texture::LoadTexture(const std::string & _strName, const TCHAR * _pFileName, const std::string & _strPathKey)
{
	const TCHAR* pPath = GET_SINGLETON(PathManager)->FindPath(_strPathKey);
	TCHAR strPath[MAX_PATH] = {};

	// 해당 패스가 미리 로드가 되어있지않다면 로드해준다.
	// ㅇㅁㅇ 로드를 해주는게 아니고 로드가 안되어있는데 로드하고 싶으니까 문자열을 채워주는것
	if (nullptr != pPath)
	{
		// lstrcpy( A,B) : B문자열을 A에 복사한다.
		lstrcpy(strPath, pPath);
	}
	lstrcat(strPath, _pFileName);

	return LoadTextureFromFullPath(_strName, strPath);
}

// new해줬으니까 실패했을경우 delete를 해줘야하지않을까? ㅇㅅㅇ (나중에 코드 확인) -  Texture
bool Texture::LoadTextureFromFullPath(const std::string & _strName, const TCHAR * _pFullPath)
{
	SetTag(_strName.c_str());

	ScratchImage* pImage = new ScratchImage;
	TCHAR	strExtension[_MAX_EXT] = {};		// 확장자

	// SplitPath(전체경로, 드라이브경로 넣을 문자열, 드라이브경로 크기, 폴더경로 넣을 문자열, 크기, 파일명, 크기)
	// 확장자는 .까지!  ex ) .png, 
	SplitPath(_pFullPath, nullptr, 0, nullptr, 0, nullptr, 0, strExtension, _MAX_EXT);

	char _strExtension[_MAX_EXT] = {};

	// 유니코드를 멀티바이트로 바꾼다.
	WideCharToMultiByte(CP_ACP, 0, strExtension, -1, _strExtension, lstrlen(strExtension), 0, 0);

	// 해당 문자열을 대문자로 바꾼다.
	_strupr_s(_strExtension);

	// strcmp은 두 문자열이 같을때 0을 반환한다.
	if (0 == strcmp(_strExtension, ".DDS"))
	{
		// 텍스처를 로드한다.
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
		// DDS, TGA파일 외의 확장자들 (PNG, JPGE..등)
		if (FAILED(LoadFromWICFile(_pFullPath, WIC_FLAGS_NONE, nullptr, *pImage)))
		{
			delete pImage;
			return false;
		}
	}
	
	// 저장할 경로 (해당 텍스처(이미지)가 있는 경로)
	TCHAR* pSavePath = new TCHAR[MAX_PATH];

	memset(pSavePath, 0, sizeof(TCHAR) * MAX_PATH);

	lstrcpy(pSavePath, _pFullPath);

	m_vecFullPath.push_back(pSavePath);

	// 로드에 성공했다면 m_vecImage에 넣어준다. ( 얜이미지 )
	m_vecImage.push_back(pImage);

	return 	CreateShaderResourceView();
}

// Texture Array
bool Texture::LoadTexture(const std::string & _strName, const std::vector<TCHAR*>& _vecFileName, const std::string & _strPathKey)
{
	const TCHAR* pPath = GET_SINGLETON(PathManager)->FindPath(_strPathKey);

	std::vector<TCHAR*> vecFullPath;
	vecFullPath.reserve(_vecFileName.size());

	// 해당 패스가 미리 로드가 되어있지않다면 로드해준다.
	// ㅇㅁㅇ 로드를 해주는게 아니고 로드가 안되어있는데 로드하고 싶으니까 문자열을 채워주는것

	for (size_t i = 0; i < _vecFileName.size(); ++i)
	{
		TCHAR* pFullPath = new TCHAR[MAX_PATH];
		memset(pFullPath, 0, sizeof(TCHAR) * MAX_PATH);

		if (nullptr != pPath)
		{
			// lstrcpy( A,B) : B문자열을 A에 복사한다.
			lstrcpy(pFullPath, pPath);
		}
		lstrcat(pFullPath, _vecFileName[i]);

		vecFullPath.push_back(pFullPath);
	}

	if (false == LoadTextureFromFullPath(_strName, vecFullPath))
	{
		// 실패할 경우에도 지워준다.
		Safe_Delete_Array_VectorList(vecFullPath);
	}

	// 넣어준뒤에는 지워준당
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
		TCHAR	strExtension[_MAX_EXT] = {};		// 확장자

		// SplitPath(전체경로, 드라이브경로 넣을 문자열, 드라이브경로 크기, 폴더경로 넣을 문자열, 크기, 파일명, 크기)
		// 확장자는 .까지!  ex ) .png, 
		SplitPath(_vecFullPath[i], nullptr, 0, nullptr, 0, nullptr, 0, strExtension, _MAX_EXT);

		char _strExtension[_MAX_EXT] = {};

		// 유니코드를 멀티바이트로 바꾼다.
		WideCharToMultiByte(CP_ACP, 0, strExtension, -1, _strExtension, lstrlen(strExtension), 0, 0);

		// 해당 문자열을 대문자로 바꾼다.
		_strupr_s(_strExtension);

		// strcmp은 두 문자열이 같을때 0을 반환한다.
		if (0 == strcmp(_strExtension, ".DDS"))
		{
			// 텍스처를 로드한다.
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
			// DDS, TGA파일 외의 확장자들 (PNG, JPGE..등)
			if (FAILED(LoadFromWICFile(_vecFullPath[i], WIC_FLAGS_NONE, nullptr, *pImage)))
			{
				delete pImage;
				return false;
			}
		}

		// 저장할 경로 (해당 텍스처(이미지)가 있는 경로)
		TCHAR* pSavePath = new TCHAR[MAX_PATH];

		memset(pSavePath, 0, sizeof(TCHAR) * MAX_PATH);

		lstrcpy(pSavePath, _vecFullPath[i]);

		m_vecFullPath.push_back(pSavePath);

		// 로드에 성공했다면 m_vecImage에 넣어준다. ( 얜이미지 )
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

// Texture 1개일때
bool Texture::CreateShaderResourceView()
{
	// CreateShaderResourceView( 디바이스, ScratchImage 정보, 이미지 개수, 메타데이터, ShaderResourceView)
	if (FAILED(DirectX::CreateShaderResourceView(_DEVICE, m_vecImage[0]->GetImages()
		, m_vecImage[0]->GetImageCount(), m_vecImage[0]->GetMetadata(),
		&m_pShaderResourceView)))
	{
		return false;
	}

	return true;
}

// Texture가 여러장일때
bool Texture::CreateShaderResourceViewArray()
{
	// Array Texture는 생성하는 코드가 존재한다. ㅇㅅㅇ 그거 쓰면 됨
	
	// 각 텍스처 원소를 파일로부터 개별적으로 넣어야한다.
	// 이 텍스처들은 GPU가 사용하는 것이 아님. 
	// 그냥 파일에서 이미지 자료를 넣기 위한 것임
	// CPU에서 자원을 읽어야하니까 D3D11_USAGE_STAGING 설정
	std::vector<ID3D11Texture2D*>		vecTexture;

	for (size_t i = 0; i < m_vecImage.size(); ++i)
	{
		ID3D11Texture2D*		pTexture = NULL;

		// CreateTextureEx -> Ex가 붙으면 확장 함수라고 생각하면 된다.
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

	// 텍스처 배열을 생성한다.
	// 텍스처 배열의 모든 원소는 형식과 크기가 동일해야한다.
	D3D11_TEXTURE2D_DESC tTextureDesc = {};
	vecTexture[0]->GetDesc(&tTextureDesc);

	// Array Texture를 만들기 위한 Desc 구조체를 생성하기
	// Array Texture는 가로, 세로 길이가 똑같아야 한다. (그래야 사용가능)
	D3D11_TEXTURE2D_DESC tArrayTextureDesc = {};
	tArrayTextureDesc.Width = tTextureDesc.Width;
	tArrayTextureDesc.Height = tTextureDesc.Height;
	tArrayTextureDesc.MipLevels = tTextureDesc.MipLevels;
	tArrayTextureDesc.ArraySize = (UINT)(vecTexture.size());		// 배열 개수
	tArrayTextureDesc.Format = tTextureDesc.Format;					// 형식 지정
	tArrayTextureDesc.SampleDesc.Count = 1;
	tArrayTextureDesc.SampleDesc.Quality = 0;
	tArrayTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	tArrayTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	// 위에서 설정한 정보를 이용해서 빈 Array Texture를 생성한다.
	ID3D11Texture2D*	pArrayTexture = NULL;

	if (FAILED(_DEVICE->CreateTexture2D(&tArrayTextureDesc, NULL, &pArrayTexture)))
	{
		assert(false);
		return false;
	}

	// Array Texture의 픽셀 정보를 불러온 텍스처의 픽셀 정보를 채워준다.  -> 물방울 11장.3텍스처배열 보기
	// 개별 텍스처 원소를 텍스처 배열에 복사한다.

	// 각각의 텍스처 원소들에 대해..(개수)
	for (size_t i = 0; i < vecTexture.size(); ++i)
	{
		// 각각의 밉맵 수준에 대해서..
		for (size_t mip = 0; mip < tTextureDesc.MipLevels; ++mip)
		{
			// 텍스처를 Map 해준다.
			D3D11_MAPPED_SUBRESOURCE	tMap = {};
			
			// Map : 하위 리소스에 포함된 데이터에 대한 포인터를 가져오고 해당 하위 리소스에 대한 GPU 액세스를 거부한다.
			_CONTEXT->Map(vecTexture[i],			// ID3D11Resource 인터페이스의 포인터
				(UINT)mip,								// 하위리소스의 인덱스 번호
				D3D11_MAP_READ,						// ㅇㅅㅇ 읽을거니까! Read
				0,									// GPU가 사용중일때 CPU가 수행하는 작업을 지정하는 플래그, 여기선 안쓸거니까 0 지정
				&tMap);								// 매핑 된 서브 리소스 에 대한 D3D11_MAPPED_SUBRESOURCE 구조체에 대한 포인터

			// UpdateSubresource : CPU는 메모리에서 비 맵핑 메모리로 작성된 하위 리소스로 데이터를 복사
			// 한 부분자원에서 다른 부분자원으로 메모리 내용을 복사한다.
			// 2차원 텍스처일 경우 SrcDepthPItch는 필요 X, 3차원 텍스처에 사용됨.
			_CONTEXT->UpdateSubresource(pArrayTexture,					// pArrayTexture여기에 값들 넣어줄거임 , 대상자원 객체
				D3D11CalcSubresource((UINT)mip, (UINT)i, tTextureDesc.MipLevels),	// 대상 하위 리소스의 시작 인덱스 (D3D11CalcSubresource -> 텍스처의 서브 렌더링 인덱스를 계산함), 대상 자원에서 갱신할 부분자원을 가리키는 인덱스
				NULL,													// NULL -> Offset없이 대상 하위 리소스에 데이터 기록, 갱신할 대상 부분자원의 영역을 지정하는  D3D11_BOX 인스턴스를 가리키는 포인터, 부분자원 전체를 갱신하고 싶으면 NULL지정
				tMap.pData,												// 복사해서 넣어줄.. 메모리 소스 데이터에 대한 포인터, 원본 자료를 가리키는 포인터
				tMap.RowPitch,											// 소스 데이터의 한 행 크기, 원본 자료 한 행의 바이트 단위 크기(바이트개수)
				tMap.DepthPitch);										// 소스 데이터의 깊이 슬라이스 크기, 원본 자료의 깊이 조각(Depth slice) 하나의 바이트 단위 크기.

			_CONTEXT->Unmap(vecTexture[i], (UINT)mip);		// Map했으니까 Unmap으로 잠근다.
		}
	}

	// 셰이더 리소스 뷰
	// 텍스처 배열에 대한 자원 뷰를 생성한다.
	D3D11_SHADER_RESOURCE_VIEW_DESC	tViewDesc = {};
	tViewDesc.Format = tArrayTextureDesc.Format;
	tViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;			// 기본 자원의 자원 유형과 동일해야함
	tViewDesc.Texture2DArray.MostDetailedMip = 0;							// 사용할 가장 상세한 밉맵 레벨의 인덱스 (0 과 -1 (MipLevels -1 ))
	tViewDesc.Texture2DArray.MipLevels = tArrayTextureDesc.MipLevels;
	tViewDesc.Texture2DArray.ArraySize = (UINT)(vecTexture.size());					// 개수

	if (FAILED(_DEVICE->CreateShaderResourceView(pArrayTexture, &tViewDesc, &m_pShaderResourceView)))
	{
		assert(false);
		return false;
	}

	SAFE_RELEASE(pArrayTexture);			// 생성후 접근했으니까 레퍼런스카운터를 감소시킨다.

	Safe_Release_VectorList(vecTexture);	// 얘도 각각 레퍼런스 카운터를 감소시킨다.
	
	return true;
}
