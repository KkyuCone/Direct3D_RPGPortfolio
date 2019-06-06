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
// ���̴� ����Ʈ�ڵ���� (��������)

const void* Shader::GetVSCode() const
{
	return m_pVSBlob->GetBufferPointer();
}

size_t Shader::GetVSCodeSize() const
{
	return m_pVSBlob->GetBufferSize();
}

// ���̴� �ε��ϱ�
bool Shader::LoadShader(const std::string& _strName, const TCHAR* _pFileName
	, std::string _pEntry[ST_END], const std::string& _strPathKey /*= PATH_SHADER*/)
{
	SetTag(_strName.c_str());

	// ��ü ��� �����
	const TCHAR* pPath = GET_SINGLETON(PathManager)->FindPath(_strPathKey);

	TCHAR strPath[MAX_PATH] = {};

	if (nullptr != pPath)
	{
		lstrcpy(strPath, pPath);
	}

	lstrcat(strPath, _pFileName);

	// ���ؽ� ���̴� �ε忡 �������� ���
	if (false == _pEntry[ST_VERTEX].empty())
	{
		if (false == LoadVertexShader(strPath, _pEntry[ST_VERTEX].c_str()))
		{
			return false;
		}
	}

	// �ȼ����̴� �ε忡 �������� ���
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

	// ��ǻƮ ���̴�
	if (false == _pEntry[ST_COMPUTE].empty())
	{
		if (false == LoadComputeShader(strPath, _pEntry[ST_COMPUTE].c_str()))
			return false;
	}

	return true;
}

// ���̴� �����ϱ� ( �ȼ����̴��� �ʼ������̾ƴϴ�. ���� ��쿡�� �������ش�. )
// ��Ʈ�� ��ºκп��� �ȼ����̴��� �ʿ����.
void Shader::SetShader()
{
	_CONTEXT->VSSetShader(m_pVS, nullptr, 0);
	_CONTEXT->PSSetShader(m_pPS, nullptr, 0);
	_CONTEXT->GSSetShader(m_pGS, nullptr, 0);
	_CONTEXT->CSSetShader(m_pCS, nullptr, 0);
}

// ���ؽ� ���̴� �ε�
bool Shader::LoadVertexShader(const TCHAR* _pFullPath, const char* _pEntry)
{
	UINT iFlag = 0;			// �⺻��

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG;				// ����׿�, ��ǻ�Ͱ� �˾Ƽ� ����ȭó�����ش�.
#endif // _DEBUG

	ID3DBlob* pError = nullptr;				// �����޼�����

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

// �ȼ� ���̴� �ε�
bool Shader::LoadPixelShader(const TCHAR* _pFullPath, const char* _pEntry)
{
	UINT iFlag = 0;			// �⺻��

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG;				// ����׿�, ��ǻ�Ͱ� �˾Ƽ� ����ȭó�����ش�.
#endif // _DEBUG

	ID3DBlob* pError = nullptr;				// �����޼�����

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
	UINT iFlag = 0;			// �⺻��

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG;				// ����׿�, ��ǻ�Ͱ� �˾Ƽ� ����ȭó�����ش�.
#endif // _DEBUG

	ID3DBlob* pError = nullptr;				// �����޼�����

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
	UINT iFlag = 0;			// �⺻��

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG;				// ����׿�, ��ǻ�Ͱ� �˾Ƽ� ����ȭó�����ش�.
#endif // _DEBUG

	ID3DBlob* pError = nullptr;				// �����޼�����

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
