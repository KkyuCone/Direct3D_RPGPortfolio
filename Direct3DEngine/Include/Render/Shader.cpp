#include "Shader.h"
#include "../PathManager.h"
#include "../Device.h"

ENGINE_USING

Shader::Shader() :
	m_pVS(nullptr), m_pVSBlob(nullptr),
	m_pPS(nullptr), m_pPSBlob(nullptr),
	m_pGS(nullptr), m_pGSBlob(nullptr),
	m_pCS(nullptr), m_pCSBlob(nullptr)
{
}


Shader::~Shader()
{
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pVSBlob);
	SAFE_RELEASE(m_pPS);
	SAFE_RELEASE(m_pPSBlob);
	SAFE_RELEASE(m_pGS);
	SAFE_RELEASE(m_pGSBlob);
	SAFE_RELEASE(m_pCS);
	SAFE_RELEASE(m_pCSBlob);
}
// 셰이더 바이트코드관련 (가져오기)

const void* Shader::GetVSCode() const
{
	return m_pVSBlob->GetBufferPointer();
}

size_t Shader::GetVSCodeSize() const
{
	return m_pVSBlob->GetBufferSize();
}

// 셰이더 로드하기
bool Shader::LoadShader(const std::string& _strName, const TCHAR* _pFileName
	, std::string _pEntry[ST_END], const std::string& _strPathKey /*= PATH_SHADER*/)
{
	SetTag(_strName.c_str());

	// 전체 경로 만들기
	const TCHAR* pPath = GET_SINGLETON(PathManager)->FindPath(_strPathKey);

	TCHAR strPath[MAX_PATH] = {};

	if (nullptr != pPath)
	{
		lstrcpy(strPath, pPath);
	}

	lstrcat(strPath, _pFileName);

	// 버텍스 셰이더 로드에 실패했을 경우
	if (false == _pEntry[ST_VERTEX].empty())
	{
		if (false == LoadVertexShader(strPath, _pEntry[ST_VERTEX].c_str()))
		{
			return false;
		}
	}

	// 픽셀셰이더 로드에 실패했을 경우
	if (false == _pEntry[ST_PIXEL].empty())
	{
		if (false == LoadPixelShader(strPath, _pEntry[ST_PIXEL].c_str()))
		{
			return false;
		}
	}


	//
	if (false == _pEntry[ST_GEOMETRY].empty())
	{
		if (!LoadGeometryShader(strPath, _pEntry[ST_GEOMETRY].c_str()))
			return false;
	}

	// 컴퓨트 셰이더
	if (false == _pEntry[ST_COMPUTE].empty())
	{
		if (false == LoadComputeShader(strPath, _pEntry[ST_COMPUTE].c_str()))
			return false;
	}

	return true;
}

// 셰이더 셋팅하기 ( 픽셀셰이더는 필수사항이아니다. 있을 경우에만 셋팅해준다. )
// 스트림 출력부분에선 픽셀셰이더가 필요없다.
void Shader::SetShader()
{
	_CONTEXT->VSSetShader(m_pVS, nullptr, 0);
	_CONTEXT->PSSetShader(m_pPS, nullptr, 0);
	_CONTEXT->GSSetShader(m_pGS, nullptr, 0);
	_CONTEXT->CSSetShader(m_pCS, nullptr, 0);
}

// 버텍스 셰이더 로드
bool Shader::LoadVertexShader(const TCHAR* _pFullPath, const char* _pEntry)
{
	UINT iFlag = 0;			// 기본값

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG;				// 디버그용, 컴퓨터가 알아서 최적화처리해준다.
#endif // _DEBUG

	ID3DBlob* pError = nullptr;				// 에러메세지용

	if (FAILED(D3DCompileFromFile(_pFullPath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		_pEntry, "vs_5_0", iFlag, 0, &m_pVSBlob, &pError)))
	{
		OutputDebugStringA((char*)pError->GetBufferPointer());
		int a = 0;
		return false;
	}

	if (FAILED(_DEVICE->CreateVertexShader(m_pVSBlob->GetBufferPointer(),
		m_pVSBlob->GetBufferSize(), nullptr, &m_pVS)))
	{
		return false;
	}

	return true;
}

// 픽셀 셰이더 로드
bool Shader::LoadPixelShader(const TCHAR* _pFullPath, const char* _pEntry)
{
	UINT iFlag = 0;			// 기본값

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG;				// 디버그용, 컴퓨터가 알아서 최적화처리해준다.
#endif // _DEBUG

	ID3DBlob* pError = nullptr;				// 에러메세지용

	if (FAILED(D3DCompileFromFile(_pFullPath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		_pEntry, "ps_5_0", iFlag, 0, &m_pPSBlob, &pError)))
	{
		OutputDebugStringA((char*)pError->GetBufferPointer());
		return false;
	}

	if (FAILED(_DEVICE->CreatePixelShader(m_pPSBlob->GetBufferPointer(),
		m_pPSBlob->GetBufferSize(), nullptr, &m_pPS)))
	{
		return false;
	}

	return true;
}

bool Shader::LoadGeometryShader(const TCHAR * _pFullPath, const char * _pEntry)
{
	UINT iFlag = 0;			// 기본값

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG;				// 디버그용, 컴퓨터가 알아서 최적화처리해준다.
#endif // _DEBUG

	ID3DBlob* pError = nullptr;				// 에러메세지용

	if (FAILED(D3DCompileFromFile(_pFullPath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		_pEntry, "gs_5_0", iFlag, 0, &m_pGSBlob, &pError)))
	{
		OutputDebugStringA((char*)pError->GetBufferPointer());
		return false;
	}

	if (FAILED(_DEVICE->CreateGeometryShader(m_pGSBlob->GetBufferPointer(),
		m_pGSBlob->GetBufferSize(), nullptr, &m_pGS)))
	{
		return false;
	}

	return true;
}

bool Shader::LoadComputeShader(const TCHAR * _pFullPath, const char * _pEntry)
{
	UINT iFlag = 0;			// 기본값

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG;				// 디버그용, 컴퓨터가 알아서 최적화처리해준다.
#endif // _DEBUG

	ID3DBlob* pError = nullptr;				// 에러메세지용

	if (FAILED(D3DCompileFromFile(_pFullPath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		_pEntry, "cs_5_0", iFlag, 0, &m_pCSBlob, &pError)))
	{
		OutputDebugStringA((char*)pError->GetBufferPointer());
		return false;
	}

	if (FAILED(_DEVICE->CreateComputeShader(m_pCSBlob->GetBufferPointer(),
		m_pCSBlob->GetBufferSize(), nullptr, &m_pCS)))
	{
		return false;
	}

	return true;
}
