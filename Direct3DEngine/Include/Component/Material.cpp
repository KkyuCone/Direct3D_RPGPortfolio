#include "Material.h"
#include "..\Resource\ResourcesManager.h"
#include "..\Resource\Texture.h"
#include "..\Resource\Sampler.h"
#include "..\Render\ShaderManager.h"
#include "..\PathManager.h"

ENGINE_USING

Material::Material()
{
	m_eComponentType = CT_MATERIAL;
}

Material::Material(const Material & _Component) : Component(_Component)
{
	m_vecMaterialContainer.clear();

	// �����̳ʸ�ŭ
	for (size_t i = 0; i < _Component.m_vecMaterialContainer.size(); ++i)
	{
		PMaterialContainer pContainer = new MaterialContainer;

		// ����¸�ŭ
		for (size_t j = 0; j < _Component.m_vecMaterialContainer[i]->vecSubSet.size(); ++j)
		{
			PMaterialSubSet pSubSet = new MaterialSubSet;

			pSubSet->tMaterialData = _Component.m_vecMaterialContainer[i]->vecSubSet[j]->tMaterialData;

			// ������ diffuse, specular, normal �� ���� �־���µ� ���� �Ѳ����� �����ϴϱ� �׳� �־��ش�!
			std::unordered_map<std::string, PTextureSet>::const_iterator StartIter = 
				_Component.m_vecMaterialContainer[i]->vecSubSet[j]->mapTextureSet.begin();
			std::unordered_map<std::string, PTextureSet>::const_iterator EndIter =
				_Component.m_vecMaterialContainer[i]->vecSubSet[j]->mapTextureSet.end();

			// �������ش� (����)
			for (; StartIter != EndIter; ++StartIter)
			{
				// ���� ����
				PTextureSet pTextureSet = new TextureSet;
				*pTextureSet = *(StartIter->second);

				if (nullptr != pTextureSet->pTexture)
				{
					// �ؽ�ó�� �ִٸ�, �����ϴϱ� ���۷���ī���͸� ������Ų��.
					pTextureSet->pTexture->AddReference();
				}

				pSubSet->mapTextureSet.insert(std::make_pair(StartIter->first, pTextureSet));
			}

			pContainer->vecSubSet.push_back(pSubSet);
		}
		m_vecMaterialContainer.push_back(pContainer);
	}
}

Material::~Material()
{
	for (size_t i = 0; i < m_vecMaterialContainer.size(); ++i)
	{
		PMaterialContainer pContainer = m_vecMaterialContainer[i];

		for (size_t j = 0; j < pContainer->vecSubSet.size(); ++j)
		{
			std::unordered_map<std::string, PTextureSet>::iterator StartIter =
				pContainer->vecSubSet[j]->mapTextureSet.begin();
			std::unordered_map<std::string, PTextureSet>::iterator EndIter =
				pContainer->vecSubSet[j]->mapTextureSet.end();

			for (; StartIter != EndIter; ++StartIter)
			{
				SAFE_RELEASE(StartIter->second->pTexture);
				SAFE_DELETE(StartIter->second);
			}

			SAFE_DELETE(pContainer->vecSubSet[j]);
		}

		SAFE_DELETE(pContainer);
	}
}

void Material::SetColor(const Vector4 & _vDiffuse, const Vector4 & _vAmbient
	, const Vector4 & _vSpecualr, const Vector4 & _vEmissive
	, float _fSpecularPower, UINT _iContainer, UINT _iSubSet)
{
	if (_iContainer >= m_vecMaterialContainer.size())
	{
		PMaterialContainer pContainer = new MaterialContainer;
		m_vecMaterialContainer.push_back(pContainer);
	}

	if (_iSubSet >= m_vecMaterialContainer[_iContainer]->vecSubSet.size())
	{
		PMaterialSubSet pSubSet = new MaterialSubSet;
		m_vecMaterialContainer[_iContainer]->vecSubSet.push_back(pSubSet);
	}

	PMaterialContainer pContainer = m_vecMaterialContainer[_iContainer];
	PMaterialSubSet pSubSet = pContainer->vecSubSet[_iSubSet];

	memcpy(&pSubSet->tMaterialData.vDiffuse, &_vDiffuse, sizeof(Vector4));
	memcpy(&pSubSet->tMaterialData.vAmbient, &_vAmbient, sizeof(Vector4));
	memcpy(&pSubSet->tMaterialData.vSpecular, &_vSpecualr, sizeof(Vector4));
	memcpy(&pSubSet->tMaterialData.vEmissive, &_vEmissive, sizeof(Vector4));

	pSubSet->tMaterialData.vSpecular.w = _fSpecularPower;
}

// ���� �츮�ڵ忡�� ���̴����� Convert�۾��Ҷ� 0~1���� �־��ְ� �Ǿ� �����Ƿ�
// Emissive���� ��ǻ� 0~1�� �־��� �� �ִ�.
// �� �̻� �ְ� ������ Emissive�� 0~1���� ���� float���� ������ �۾����ָ�ȴ�.
void Material::SetEmissiveColor(float _fEmissive)
{
	// 
	for (size_t i = 0; i < m_vecMaterialContainer.size(); ++i)
	{
		for (size_t j = 0; j < m_vecMaterialContainer[i]->vecSubSet.size(); ++j)
		{
			m_vecMaterialContainer[i]->vecSubSet[j]->tMaterialData.vEmissive.x = _fEmissive;
			m_vecMaterialContainer[i]->vecSubSet[j]->tMaterialData.vEmissive.y = _fEmissive;
			m_vecMaterialContainer[i]->vecSubSet[j]->tMaterialData.vEmissive.z = _fEmissive;
			m_vecMaterialContainer[i]->vecSubSet[j]->tMaterialData.vEmissive.w = 1.0f;
		}
	}
}

// �κп� Emissive �ֱ�
void Material::SetEmissiveColor(int _iContainer, int _iSubSet, float _fEmissive)
{
	m_vecMaterialContainer[_iContainer]->vecSubSet[_iSubSet]->tMaterialData.vEmissive.x = _fEmissive;
	m_vecMaterialContainer[_iContainer]->vecSubSet[_iSubSet]->tMaterialData.vEmissive.y = _fEmissive;
	m_vecMaterialContainer[_iContainer]->vecSubSet[_iSubSet]->tMaterialData.vEmissive.z = _fEmissive;
	m_vecMaterialContainer[_iContainer]->vecSubSet[_iSubSet]->tMaterialData.vEmissive.w = 1.0f;
}

// �ؽ�ó �߰��ϱ�
void Material::AddTextureSet(int _iContainer, int _iSubSet
	, int _iTextureRegister, const std::string & _strTextureName
	, const TCHAR * _pFileName /*= nullptr*/
	, const std::string & _strPathName /*= PATH_TEXTURE*/)
{
	// �����̳� ��ȣ�� ������ ũ�⸦ ������ ���� ������ش�.
	if (_iContainer >= (int)(m_vecMaterialContainer.size()))
	{
		PMaterialContainer pContainer = new MaterialContainer;
		m_vecMaterialContainer.push_back(pContainer);
	}

	if (_iSubSet >= (int)(m_vecMaterialContainer[_iContainer]->vecSubSet.size()))
	{
		// ������� ��ȣ�� �ش� ��Ƽ���� �����̳��� ������� ������ ũ��
		PMaterialSubSet pSubSet = new MaterialSubSet;
		m_vecMaterialContainer[_iContainer]->vecSubSet.push_back(pSubSet);
	}

	// �ش� �ؽ�ó�� �̹� �ش� �����̳��� ����¿� �ִٸ� return -> �ٲٰ� ���� Change�ϼ�
	if (nullptr != FindTextureSet(_iContainer, _iSubSet, _strTextureName))
	{
		return;
	}

	PMaterialContainer pContainer = m_vecMaterialContainer[_iContainer];
	PMaterialSubSet pSubSet = pContainer->vecSubSet[_iSubSet];

	// ���ؽ�ó�� �����
	PTextureSet	pTextureSet = new TextureSet;

	// �ؽ�ó�� �ε��Ѵ�.
	if (nullptr != _pFileName) 
	{
		GET_SINGLETON(ResourcesManager)->LoadTexture(_strTextureName, _pFileName, _strPathName);
	}

	// �� �ؽ�ó�� �ε��� �ؽ�ó�� �־��ְ� ( �������� ��ȣ��.. )
	pTextureSet->pTexture = GET_SINGLETON(ResourcesManager)->FindTexture(_strTextureName);
	pTextureSet->iTextureRegister = _iTextureRegister;

	// ����¿� �߰��Ѵ�.
	pSubSet->mapTextureSet.insert(std::make_pair(_strTextureName, pTextureSet));

	// �ĺ���ȣ 
	if (1 == pSubSet->mapTextureSet.size())
	{
		m_iSerialNumber = pTextureSet->pTexture->GetSerialNumber();
	}
}

// AnimationFrame class
void Material::AddTextureSet(int _iContainer, int _iSubSet, 
	int _iTextureRegister, Texture * _pTexture)
{
	// �����̳� ��ȣ�� ������ ũ�⸦ ������ ���� ������ش�.
	if (_iContainer >= (int)(m_vecMaterialContainer.size()))
	{
		PMaterialContainer pContainer = new MaterialContainer;
		m_vecMaterialContainer.push_back(pContainer);
	}

	if (_iSubSet >= (int)(m_vecMaterialContainer[_iContainer]->vecSubSet.size()))
	{
		// ������� ��ȣ�� �ش� ��Ƽ���� �����̳��� ������� ������ ũ��
		PMaterialSubSet pSubSet = new MaterialSubSet;
		m_vecMaterialContainer[_iContainer]->vecSubSet.push_back(pSubSet);
	}

	// �ش� �ؽ�ó�� �̹� �ش� �����̳��� ����¿� �ִٸ� return -> �ٲٰ� ���� Change�ϼ�
	if (nullptr != FindTextureSet(_iContainer, _iSubSet, _pTexture->GetTag()))
	{
		return;
	}

	PMaterialContainer pContainer = m_vecMaterialContainer[_iContainer];
	PMaterialSubSet pSubSet = pContainer->vecSubSet[_iSubSet];

	// ���ؽ�ó�� �����
	PTextureSet pTextureSet = new TextureSet;

	_pTexture->AddReference();

	pTextureSet->pTexture = _pTexture;
	pTextureSet->iTextureRegister = _iTextureRegister;

	// ����¿� �߰��Ѵ�.
	pSubSet->mapTextureSet.insert(std::make_pair(_pTexture->GetTag(), pTextureSet));

	if (1 == pSubSet->mapTextureSet.size())
	{
		m_iSerialNumber = pTextureSet->pTexture->GetSerialNumber();
	}
}

void Material::AddTextureSetFromFullPath(int _iContainer, int _iSubSet,
	int _iTextureRegister, const std::string & _strTextureName, 
	const TCHAR * _pFullPath)
{
	if (_iContainer >= (int)(m_vecMaterialContainer.size()))
	{
		PMaterialContainer	pContainer = new MaterialContainer;
		m_vecMaterialContainer.push_back(pContainer);
	}

	if (_iSubSet >= (int)(m_vecMaterialContainer[_iContainer]->vecSubSet.size()))
	{
		PMaterialSubSet	pSubSet = new MaterialSubSet;
		m_vecMaterialContainer[_iContainer]->vecSubSet.push_back(pSubSet);
	}

	if (nullptr != FindTextureSet(_iContainer, _iSubSet, _strTextureName))
	{
		return;
	}

	PMaterialContainer	pContainer = m_vecMaterialContainer[_iContainer];
	PMaterialSubSet		pSubSet = pContainer->vecSubSet[_iSubSet];

	PTextureSet pTextureSet = new TextureSet;

	if (nullptr != _pFullPath)
	{
		GET_SINGLETON(ResourcesManager)->LoadTextureFromFullPath(_strTextureName, _pFullPath);
	}

	pTextureSet->pTexture = GET_SINGLETON(ResourcesManager)->FindTexture(_strTextureName);
	pTextureSet->iTextureRegister = _iTextureRegister;

	pSubSet->mapTextureSet.insert(std::make_pair(_strTextureName, pTextureSet));

	if (1 == pSubSet->mapTextureSet.size())
	{
		m_iSerialNumber = pTextureSet->pTexture->GetSerialNumber();
	}

}

void Material::AddTextureSetArray(int _iContainer, int _iSubSet,
	int _iTextureRegister, const std::string & _strTextureName,
	const std::vector<TCHAR*>* _vecFileName, 
	const std::string & _strPathName)
{
	// �����̳� ��ȣ�� ������ ũ�⸦ ������ ���� ������ش�.
	if (_iContainer >= (int)(m_vecMaterialContainer.size()))
	{
		PMaterialContainer pContainer = new MaterialContainer;
		m_vecMaterialContainer.push_back(pContainer);
	}

	if (_iSubSet >= (int)(m_vecMaterialContainer[_iContainer]->vecSubSet.size()))
	{
		// ������� ��ȣ�� �ش� ��Ƽ���� �����̳��� ������� ������ ũ��
		PMaterialSubSet pSubSet = new MaterialSubSet;
		m_vecMaterialContainer[_iContainer]->vecSubSet.push_back(pSubSet);
	}

	if (nullptr != FindTextureSet(_iContainer, _iSubSet, _strTextureName))
	{
		return;
	}

	PMaterialContainer pContainer = m_vecMaterialContainer[_iContainer];
	PMaterialSubSet pSubSet = pContainer->vecSubSet[_iSubSet];

	PTextureSet pTextureSet = new TextureSet;
	
	if (nullptr != _vecFileName)
	{
		GET_SINGLETON(ResourcesManager)->LoadTexture(_strTextureName, *_vecFileName, _strPathName);
	}

	pTextureSet->pTexture = GET_SINGLETON(ResourcesManager)->FindTexture(_strTextureName);
	pTextureSet->iTextureRegister = _iTextureRegister;

	pSubSet->mapTextureSet.insert(std::make_pair(_strTextureName, pTextureSet));

	if (1 == pSubSet->mapTextureSet.size())
	{
		m_iSerialNumber = pTextureSet->pTexture->GetSerialNumber();
	}
}

void Material::AddTextureSetArrayFromFullPath(int _iContainer, int _iSubSet,
	int _iTextureRegister, const std::string & _strTextureName, 
	const std::vector<TCHAR*>* _vecFullPath)
{
	if (_iContainer >= (int)(m_vecMaterialContainer.size()))
	{
		PMaterialContainer	pContainer = new MaterialContainer;
		m_vecMaterialContainer.push_back(pContainer);
	}

	if (_iSubSet >= (int)(m_vecMaterialContainer[_iContainer]->vecSubSet.size()))
	{
		PMaterialSubSet	pSubSet = new MaterialSubSet;
		m_vecMaterialContainer[_iContainer]->vecSubSet.push_back(pSubSet);
	}

	if (nullptr != FindTextureSet(_iContainer, _iSubSet, _strTextureName))
	{
		return;
	}

	PMaterialContainer	pContainer = m_vecMaterialContainer[_iContainer];
	PMaterialSubSet		pSubSet = pContainer->vecSubSet[_iSubSet];

	PTextureSet pTextureSet = new TextureSet;

	if (nullptr != _vecFullPath)
	{
		GET_SINGLETON(ResourcesManager)->LoadTextureFromFullPath(_strTextureName, *_vecFullPath);
	}

	pTextureSet->pTexture = GET_SINGLETON(ResourcesManager)->FindTexture(_strTextureName);
	pTextureSet->iTextureRegister = _iTextureRegister;

	pSubSet->mapTextureSet.insert(std::make_pair(_strTextureName, pTextureSet));

	if (1 == pSubSet->mapTextureSet.size())
	{
		m_iSerialNumber = pTextureSet->pTexture->GetSerialNumber();
	}
}

// �ؽ�ó �����Ѱ� �����ϱ�(�ش� �����̳ʿ� ������� ã�Ƽ� ���� ������)
void Material::DeleteTextureSet(int _iContainer, int _iSubSet, const std::string & _strName)
{
	if (true == _iContainer >= (int)(m_vecMaterialContainer.size()))
	{
		return;
	}
	else if (true == _iSubSet >= (int)(m_vecMaterialContainer[_iContainer]->vecSubSet.size()))
	{
		return;
	}

	PMaterialContainer pContainer = m_vecMaterialContainer[_iContainer];
	PMaterialSubSet pSubSet = m_vecMaterialContainer[_iContainer]->vecSubSet[_iSubSet];

	std::unordered_map<std::string, PTextureSet>::iterator FindIter = pSubSet->mapTextureSet.find(_strName);

	// ������
	if (FindIter == pSubSet->mapTextureSet.end())
	{
		return;
	}

	SAFE_RELEASE(FindIter->second->pTexture);
	SAFE_DELETE(FindIter->second);

	pSubSet->mapTextureSet.erase(FindIter);
}

void Material::DeleteTextureSet(int _iContainer, int _iSubSet)
{
	// ���� �������.
	if (true == _iContainer >= (int)(m_vecMaterialContainer.size()))
	{
		return;
	}
	else if (true == _iSubSet >= (int)(m_vecMaterialContainer[_iContainer]->vecSubSet.size()))
	{
		return;
	}

	PMaterialContainer pContainer = m_vecMaterialContainer[_iContainer];
	PMaterialSubSet pSubSet = m_vecMaterialContainer[_iContainer]->vecSubSet[_iSubSet];

	std::unordered_map<std::string, PTextureSet>::iterator StartIter = pSubSet->mapTextureSet.begin();
	std::unordered_map<std::string, PTextureSet>::iterator EndIter = pSubSet->mapTextureSet.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		SAFE_RELEASE(StartIter->second->pTexture);
		SAFE_DELETE(StartIter->second);
	}

	pSubSet->mapTextureSet.clear();

}

void Material::ChangeTextureSet(int _iContainer, int _iSubSet,
	int _iTextureRegister, const std::string& _strOriTextuerName,
	const std::string& _strChangeTextureName,
	const TCHAR* _pFileName/* = nullptr*/,
	const std::string& _strPathName /*= PATH_TEXTURE*/)
{
	// �����̳� ��ȣ�� ������ ũ�⸦ ������ ���� ������ش�. -> ü[�����ϱ� ����������
	if (_iContainer >= (int)(m_vecMaterialContainer.size()))
	{
		assert(false);
		return;
	}

	if (_iSubSet >= (int)(m_vecMaterialContainer[_iContainer]->vecSubSet.size()))
	{
		// ������� ��ȣ�� �ش� ��Ƽ���� �����̳��� ������� ������ ũ��
		assert(false);
		return;
	}

	PMaterialContainer pContainer = m_vecMaterialContainer[_iContainer];
	PMaterialSubSet pSubSet = pContainer->vecSubSet[_iSubSet];

	// �ٲ�����..(������)
	PTextureSet pOriginSet = FindTextureSet(_iContainer, _iSubSet, _strOriTextuerName);

	// �̹� �����Ѵٸ� �����ش�.
	if (nullptr != pOriginSet) 
	{
		std::string strName = pOriginSet->pTexture->GetTag();
		SAFE_RELEASE(pOriginSet->pTexture);
		GET_SINGLETON(ResourcesManager)->DeleteTexture(strName);
	}
	else
	{
		pOriginSet = new TextureSet;
	}

	pOriginSet->iTextureRegister = _iTextureRegister;

	// �ٲ� �ؽ�ó �ε�
	if (nullptr != _pFileName)
	{
		GET_SINGLETON(ResourcesManager)->LoadTexture(_strChangeTextureName, _pFileName, _strPathName);
	}

	pOriginSet->pTexture = GET_SINGLETON(ResourcesManager)->FindTexture(_strChangeTextureName);

	// ü���� �� �ؽ�ó�� ������ ��Ʈ����
	if (nullptr == pOriginSet->pTexture)
	{
		SAFE_RELEASE(pOriginSet->pTexture);
		SAFE_DELETE(pOriginSet);
		return;
	}

	// Ű ��ü
	pSubSet->mapTextureSet.erase(_strOriTextuerName);
	pSubSet->mapTextureSet.insert(std::make_pair(_strChangeTextureName, pOriginSet));

	if (1 == pSubSet->mapTextureSet.size())
	{
		m_iSerialNumber = pOriginSet->pTexture->GetSerialNumber();
	}
}

void Material::ChangeTextureSet(int _iContainer, int _iSubSet, 
	int _iTextureRegister, 
	const std::string & _strOriTextuerName, 
	Texture * _pTexture)
{
	// �����̳� ��ȣ�� ������ ũ�⸦ ������ ���� ������ش�. -> ü[�����ϱ� ����������
	if (_iContainer >= (int)(m_vecMaterialContainer.size()))
	{
		assert(false);
		return;
	}

	if (_iSubSet >= (int)(m_vecMaterialContainer[_iContainer]->vecSubSet.size()))
	{
		// ������� ��ȣ�� �ش� ��Ƽ���� �����̳��� ������� ������ ũ��
		assert(false);
		return;
	}

	PMaterialContainer pContainer = m_vecMaterialContainer[_iContainer];
	PMaterialSubSet pSubSet = pContainer->vecSubSet[_iSubSet];

	PTextureSet pOriginSet = FindTextureSet(_iContainer, _iSubSet, _strOriTextuerName);

	if (nullptr != pOriginSet)
	{
		std::string strName = pOriginSet->pTexture->GetTag();
		SAFE_RELEASE(pOriginSet->pTexture);
		GET_SINGLETON(ResourcesManager)->DeleteTexture(strName);
	}
	else
	{
		pOriginSet = new TextureSet;
	}

	pOriginSet->iTextureRegister = _iTextureRegister;
	_pTexture->AddReference();
	pOriginSet->pTexture = _pTexture;

	// Ű ��ü
	pSubSet->mapTextureSet.erase(_strOriTextuerName);
	pSubSet->mapTextureSet.insert(std::make_pair(_pTexture->GetTag(), pOriginSet));

	if (1 == pSubSet->mapTextureSet.size())
	{
		m_iSerialNumber = pOriginSet->pTexture->GetSerialNumber();
	}
}

void Material::ChangeTextureSetFromFullPath(int _iContainer, int _iSubSet,
	int _iTextureRegister, const std::string & _strOriTextuerName,
	const std::string& _strChangeTextureName,
	const TCHAR * _pFullPath /*= nullptr*/)
{
	// �����̳� ��ȣ�� ������ ũ�⸦ ������ ���� ������ش�.
	if (_iContainer >= (int)(m_vecMaterialContainer.size()))
	{
		assert(false);
		return;
	}

	if (_iSubSet >= (int)(m_vecMaterialContainer[_iContainer]->vecSubSet.size()))
	{
		// ������� ��ȣ�� �ش� ��Ƽ���� �����̳��� ������� ������ ũ��
		assert(false);
		return;
	}

	PMaterialContainer pContainer = m_vecMaterialContainer[_iContainer];
	PMaterialSubSet pSubSet = pContainer->vecSubSet[_iSubSet];

	// �ٲ�����..(������)
	PTextureSet pOriginSet = FindTextureSet(_iContainer, _iSubSet, _strOriTextuerName);

	// �̹� �����Ѵٸ� �����ְ� 
	if (nullptr != pOriginSet)
	{
		std::string strName = pOriginSet->pTexture->GetTag();
		SAFE_RELEASE(pOriginSet->pTexture);
		GET_SINGLETON(ResourcesManager)->DeleteTexture(strName);
	}
	else
	{
		pOriginSet = new TextureSet;
	}


	if (nullptr != _pFullPath)
	{
		GET_SINGLETON(ResourcesManager)->LoadTextureFromFullPath(_strChangeTextureName, _pFullPath);
	}

	pOriginSet->iTextureRegister = _iTextureRegister;
	pOriginSet->pTexture = GET_SINGLETON(ResourcesManager)->FindTexture(_strChangeTextureName);

	// ü���� �� �ؽ�ó�� ������ ��Ʈ����
	if (nullptr == pOriginSet->pTexture)
	{
		assert(false);
		return;
	}

	// Ű ��ü
	pSubSet->mapTextureSet.erase(_strOriTextuerName);
	pSubSet->mapTextureSet.insert(std::make_pair(_strChangeTextureName, pOriginSet));

	if (1 == pSubSet->mapTextureSet.size())
	{
		m_iSerialNumber = pOriginSet->pTexture->GetSerialNumber();
	}
}

void Material::ChangeTextureSetArray(int _iContainer, int _iSubSet,
	int _iTextureRegister, const std::string & _strOriTextuerName,
	const std::string& _strChangeTextureName,
	const std::vector<TCHAR*>* _vecFullPath /*= nullptr*/,
	const std::string& _strPathname /*= PATH_TEXTURE*/)
{
	if (_iContainer >= (int)(m_vecMaterialContainer.size()))
	{
		assert(false);
		return;
	}

	if (_iSubSet >= (int)(m_vecMaterialContainer[_iContainer]->vecSubSet.size()))
	{
		assert(false);
		return;
	}

	PMaterialContainer	pContainer = m_vecMaterialContainer[_iContainer];
	PMaterialSubSet	pSubset = pContainer->vecSubSet[_iSubSet];

	PTextureSet	pOriginSet = FindTextureSet(_iContainer, _iSubSet, _strOriTextuerName);

	// �̹� �����Ѵٸ� �����ְ� 
	if (nullptr != pOriginSet)
	{
		std::string strName = pOriginSet->pTexture->GetTag();
		SAFE_RELEASE(pOriginSet->pTexture);
		GET_SINGLETON(ResourcesManager)->DeleteTexture(strName);
	}
	else
	{
		pOriginSet = new TextureSet;
	}

	if (nullptr != _vecFullPath)
	{
		GET_SINGLETON(ResourcesManager)->LoadTexture(_strChangeTextureName, *_vecFullPath, _strPathname);
	}

	pOriginSet->iTextureRegister = _iTextureRegister;
	pOriginSet->pTexture = GET_SINGLETON(ResourcesManager)->FindTexture(_strChangeTextureName);

	// ü�����ϴµ� ..?�ؽ�ó ������ ��Ʈ����
	if (nullptr == pOriginSet->pTexture)
	{
		assert(false);
		return;
	}

	// Ű�� ��ü�Ѵ�.
	pSubset->mapTextureSet.erase(_strOriTextuerName);
	pSubset->mapTextureSet.insert(std::make_pair(_strChangeTextureName, pOriginSet));

	if (1 == pSubset->mapTextureSet.size())
	{
		m_iSerialNumber = pOriginSet->pTexture->GetSerialNumber();
	}
}

void Material::ChangeTextureSetArrayFromFullPath(int _iContainer, int _iSubSet,
	int _iTextureRegister,
	const std::string& _strOriTextuerName,
	const std::string& _strChangeTextureName,
	const std::vector<TCHAR*>* _vecFullPath /*= nullptr*/)
{
	if (_iContainer >= (int)(m_vecMaterialContainer.size()))
	{
		return;
	}

	if (_iSubSet >= (int)(m_vecMaterialContainer[_iContainer]->vecSubSet.size()))
	{
		return;
	}

	PMaterialContainer	pContainer = m_vecMaterialContainer[_iContainer];
	PMaterialSubSet	pSubset = pContainer->vecSubSet[_iSubSet];

	PTextureSet	pOriginSet = FindTextureSet(_iContainer, _iSubSet, _strOriTextuerName);

	// �̹� �����Ѵٸ� �����ְ� 
	if (nullptr != pOriginSet)
	{
		std::string strName = pOriginSet->pTexture->GetTag();
		SAFE_RELEASE(pOriginSet->pTexture);
		GET_SINGLETON(ResourcesManager)->DeleteTexture(strName);
	}
	else
	{
		pOriginSet = new TextureSet;
	}

	if (nullptr != _vecFullPath)
	{
		GET_SINGLETON(ResourcesManager)->LoadTextureFromFullPath(_strChangeTextureName, *_vecFullPath);
	}

	pOriginSet->iTextureRegister = _iTextureRegister;
	pOriginSet->pTexture = GET_SINGLETON(ResourcesManager)->FindTexture(_strChangeTextureName);

	// ü�����ϴµ� ..?�ؽ�ó ������ ��Ʈ����
	if (nullptr == pOriginSet->pTexture)
	{
		assert(false);
		return;
	}

	// Ű�� ��ü�Ѵ�. ( ������ �ؽ�ó�� ����� �ٲ� �ؽ�ó�� �ٲ۴�. )
	pSubset->mapTextureSet.erase(_strOriTextuerName);
	pSubset->mapTextureSet.insert(std::make_pair(_strChangeTextureName, pOriginSet));

	if (1 == pSubset->mapTextureSet.size())
	{
		m_iSerialNumber = pOriginSet->pTexture->GetSerialNumber();
	}
}

void Material::BumpTextureEnable(int _iContainer, int _iSubSet)
{
	if (_iContainer >= (int)(m_vecMaterialContainer.size()))
	{
		assert(false);
		return;
	}

	if (_iSubSet >= (int)(m_vecMaterialContainer[_iContainer]->vecSubSet.size()))
	{
		assert(false);
		return;
	}

	PMaterialContainer	pContainer = m_vecMaterialContainer[_iContainer];
	PMaterialSubSet	pSubset = pContainer->vecSubSet[_iSubSet];

	pSubset->tMaterialData.iNormal = 1;			// �븻�� ������ 0 ������ 1
}

void Material::SepcularTextureEnable(int _iContainer, int _iSubSet)
{
	if (_iContainer >= (int)(m_vecMaterialContainer.size()))
	{
		assert(false);
		return;
	}

	if (_iSubSet >= (int)(m_vecMaterialContainer[_iContainer]->vecSubSet.size()))
	{
		assert(false);
		return;
	}

	PMaterialContainer	pContainer = m_vecMaterialContainer[_iContainer];
	PMaterialSubSet	pSubset = pContainer->vecSubSet[_iSubSet];

	pSubset->tMaterialData.iSpeuclar = 1;		// spec�� ������ 0 ������ 1
}

// �ؽ�ó ã��
PTextureSet Material::FindTextureSet(int _iConatienr, int _iSubSet, const std::string & _strName)
{
	if (_iConatienr >= (int)(m_vecMaterialContainer.size()))
		return nullptr;

	else if (_iSubSet >= (int)(m_vecMaterialContainer[_iConatienr]->vecSubSet.size()))
		return nullptr;

	PMaterialContainer	pContainer = m_vecMaterialContainer[_iConatienr];
	PMaterialSubSet	pSubset = pContainer->vecSubSet[_iSubSet];

	std::unordered_map<std::string, PTextureSet>::iterator	FindIter = pSubset->mapTextureSet.find(_strName);

	if (FindIter == pSubset->mapTextureSet.end())
		return nullptr;

	return FindIter->second;
}

// ��Ű��
void Material::Skinning(int _iContainer, int _iSubSet)
{
	if (_iContainer >= (int)(m_vecMaterialContainer.size()))
	{
		PMaterialContainer pContainer = new MaterialContainer;
		m_vecMaterialContainer.push_back(pContainer);
	}

	if (_iSubSet >= (int)(m_vecMaterialContainer[_iContainer]->vecSubSet.size()))
	{
		PMaterialSubSet pSubSet = new MaterialSubSet;
		m_vecMaterialContainer[_iContainer]->vecSubSet.push_back(pSubSet);
	}

	PMaterialContainer pContatiner = m_vecMaterialContainer[_iContainer];
	PMaterialSubSet	pSubSet = m_vecMaterialContainer[_iContainer]->vecSubSet[_iSubSet];

	pSubSet->tMaterialData.iSkinning = 1;			// �ִϸ��̼��� �ִٰ� �˷��ش�. 
	
}


void Material::Start()
{
}

bool Material::Init()
{
	return true;
}

int Material::Input(float _fTime)
{
	return 0;
}

int Material::Update(float _fTime)
{
	return 0;
}

int Material::LateUpdate(float _fTime)
{
	return 0;
}

int Material::Collision(float _fTime)
{
	return 0;
}

int Material::PrevRender(float _fTime)
{
	return 0;
}

int Material::Render(float _fTime)
{
	return 0;
}

int Material::RenderShadow(float _fTime)
{
	return 0;
}

Material * Material::Clone() const
{
	return new Material(*this);
}

void Material::SetShader(int _iContainer, int _iSubSet)
{
	PMaterialSubSet pSubSet = m_vecMaterialContainer[_iContainer]->vecSubSet[_iSubSet];

	if (nullptr == pSubSet)
	{
		return;
	}

	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("Material", &pSubSet->tMaterialData);

	std::unordered_map<std::string, PTextureSet>::iterator StartIter = pSubSet->mapTextureSet.begin();
	std::unordered_map<std::string, PTextureSet>::iterator EndIter = pSubSet->mapTextureSet.end();

	// �븻��, ��ǻ�� ��� ���� �� ���� �־��µ� ���� map���� �����ϴϱ�
	// ������� map���� �� ���� �ִ�. ������ ���� �ȼ����̴��� �־�����
	// ����� �ؽ�ó�������� ��ȣ�� ��ǻ������ �ٸ������� ���̴��� �˷��ش�. ������
	// 0 : Diffse, 1 : Noraml, 2 : Sepcular
	for ( ; StartIter != EndIter; ++StartIter)
	{
		StartIter->second->pTexture->PSSetShader(StartIter->second->iTextureRegister);
	}
}

void Material::Save(FILE * _pFile)
{
	size_t iContainer = m_vecMaterialContainer.size();
	fwrite(&iContainer, sizeof(size_t), 1, _pFile);

	for (size_t i = 0; i < iContainer; ++i)
	{
		size_t iSubSet = m_vecMaterialContainer[i]->vecSubSet.size();
		fwrite(&iSubSet, sizeof(size_t), 1, _pFile);

		for (size_t j = 0; j < iSubSet; ++j)
		{
			PMaterialSubSet pSubSet = m_vecMaterialContainer[i]->vecSubSet[j];
			fwrite(&pSubSet->tMaterialData, sizeof(MaterialCBuffer), 1, _pFile);

			size_t iTextureCount = pSubSet->mapTextureSet.size();
			fwrite(&iTextureCount, sizeof(size_t), 1, _pFile);

			std::unordered_map<std::string, PTextureSet>::iterator StartIter = pSubSet->mapTextureSet.begin();
			std::unordered_map<std::string, PTextureSet>::iterator EndIter = pSubSet->mapTextureSet.end();

			for ( ; StartIter != EndIter; ++StartIter)
			{
				SaveTextureSet(_pFile, StartIter->second);
			}

		}
	}
}

void Material::Load(FILE * _pFile)
{
	m_vecMaterialContainer.clear();
	size_t iContainer = 0;
	fread(&iContainer, sizeof(size_t), 1, _pFile);

	for (size_t i = 0; i < iContainer; ++i)
	{
		size_t iSubSet = 0;
		fread(&iSubSet, sizeof(size_t), 1, _pFile);

		PMaterialContainer	pContainer = new MaterialContainer;

		m_vecMaterialContainer.push_back(pContainer);

		for (size_t j = 0; j < iSubSet; j++)
		{
			PMaterialSubSet	pSubSet = new MaterialSubSet;
			pContainer->vecSubSet.push_back(pSubSet);

			fread(&pSubSet->tMaterialData, sizeof(MaterialCBuffer), 1, _pFile);

			size_t iTextureCount = 0;
			fread(&iTextureCount, sizeof(size_t), 1, _pFile);

			for (int i = 0; i < (int)(iTextureCount); ++i)
			{
				PTextureSet pTexture = nullptr;
				LoadTextureSet(_pFile, &pTexture);
				pSubSet->mapTextureSet.insert(std::make_pair(pTexture->pTexture->GetTag(), pTexture));
			
				if (1 == pSubSet->mapTextureSet.size())
				{
					m_iSerialNumber = pTexture->pTexture->GetSerialNumber();
				}
			}
		}
	}
	
}

void Material::SaveFromPath(const char * _pFileName, const std::string & _strPathKey)
{
}

void Material::LoadFromPath(const char * _pFileName, const std::string & _strPathKey)
{
}

void Material::SaveTextureSet(FILE * _pFile, PTextureSet _pTexture)
{
	bool	bTextureEnable = false;

	if (nullptr != _pTexture)
	{
		bTextureEnable = true;
		fwrite(&bTextureEnable, sizeof(bool), 1, _pFile);

		// �ؽ�ó �̸� ���� (Ű��)
		std::string strTextureKey = _pTexture->pTexture->GetTag();
		size_t		iKeyLength = strTextureKey.length();
		fwrite(&iKeyLength, sizeof(size_t), 1, _pFile);
		fwrite(strTextureKey.c_str(), 1, iKeyLength, _pFile);

		// Texture ��θ� ���´�. 
		const std::vector<TCHAR*>* pPathList = _pTexture->pTexture->GetFullPath();

		size_t iPathCount = pPathList->size();
		fwrite(&iPathCount, sizeof(size_t), 1, _pFile);

		const char* pRootPath = GET_SINGLETON(PathManager)->FindPathMultibyte(PATH_ROOT);
		char strRoot[MAX_PATH] = {};

		int iRootLength = (int)(strlen(pRootPath));

		// 2�� �� ������ �ǵڿ� \\�� / �� ���ܽ�Ű�� ������.
		// ��θ� �ڿ������� �о�´�.
		for (int i = iRootLength - 2; i >= 0; --i)
		{
			// ��Ʈ ��θ� ��������..
			if ('\\' == pRootPath[i] || '/' == pRootPath[i])
			{
				memcpy(strRoot, &pRootPath[i + 1], sizeof(char) * (iRootLength - (i + 2)));
				break;
			}
		}

		// �ش� ��θ� �Ųٷ� �����´�.
		// ������ �ڿ������� �о���ϱ� �����������
		iRootLength = (int)(strlen(strRoot));

		char strReverse[MAX_PATH] = {};		// �������� ������ ���� ( �°� ��������)

		for (int i = 0; i < iRootLength; ++i)
		{
			strReverse[i] = strRoot[iRootLength - (i + 1)];
		}

		// �ٽ� ��� �־��ֱ� (�����)
		memcpy(strRoot, strReverse, MAX_PATH);

		// �빮�ڷ� �ٲٱ�
		_strupr_s(strRoot);

		std::vector<TCHAR*>::const_iterator StartIter = pPathList->begin();
		std::vector<TCHAR*>::const_iterator EndIter = pPathList->end();

		for (; StartIter != EndIter; ++StartIter)
		{
			int iPathLength = lstrlen(*StartIter);
			char strPath[MAX_PATH] = {};

#ifdef UNICODE
			WideCharToMultiByte(CP_ACP, 0, *StartIter, -1, strPath, iPathLength, 0, 0);
#else
			strcpy_s(strPath, *StartIter);
#endif // UNICODE

			_strupr_s(strPath);

			for (int k = iPathLength - 1; k >= 0; --k)
			{
				if ('\\' == strPath[k] || '/' == strPath[k])
				{
					bool bEnable = true;
					
					for (int l = 1; l < iRootLength; ++l)
					{
						if (strPath[k - l] != strRoot[l - 1])
						{
							bEnable = false;
							break;
						}
					}

					if (true == bEnable)
					{
						char	strSavePath[MAX_PATH] = {};
						int iSaveCount = iPathLength - (k + 1);
						memcpy(strSavePath, &strPath[k + 1], sizeof(char) * iSaveCount);
						fwrite(&iSaveCount, sizeof(int), 1, _pFile);
						fwrite(strSavePath, sizeof(char), iSaveCount, _pFile);
						break;
					}
				}
			}
		}

		fwrite(&_pTexture->iTextureRegister, sizeof(int), 1, _pFile);
	}
	else
	{
		// �ؽ�ó�� ���� ���
		fwrite(&bTextureEnable, sizeof(bool), 1, _pFile);
	}
}

void Material::LoadTextureSet(FILE * _pFile, PTextureSet * _ppTexture)
{
	bool bTextureEnable = false;
	fread(&bTextureEnable, sizeof(bool), 1, _pFile);

	// �ؽ�ó�� �ִٸ�
	if (true == bTextureEnable)
	{
		char strTextureKey[256] = {};
		size_t	iKeyLength = 0;
		fread(&iKeyLength, sizeof(size_t), 1, _pFile);
		fread(strTextureKey, 1, iKeyLength, _pFile);

		size_t iPathCount = 0;
		fread(&iPathCount, sizeof(size_t), 1, _pFile);

		if (1 == iPathCount)
		{
			char	strPath[MAX_PATH] = {};
			int iSaveCount = 0;
			fread(&iSaveCount, sizeof(int), 1, _pFile);
			fread(strPath, sizeof(char), iSaveCount, _pFile);

			TCHAR	strLoadPath[MAX_PATH] = {};

#ifdef UNICODE
			MultiByteToWideChar(CP_ACP, 0, strPath, -1, strLoadPath, (int)(strlen(strPath) * 2));
#else
			strcpy_s(strLoadPath, strPath);
#endif // UNICODE

			*_ppTexture = new TextureSet;

			// �ؽ�ó �ε��ϱ�
			GET_SINGLETON(ResourcesManager)->LoadTexture(strTextureKey, strLoadPath, PATH_ROOT);
			(*_ppTexture)->pTexture = GET_SINGLETON(ResourcesManager)->FindTexture(strTextureKey);
		}
		else
		{
			// �ؽ�ó�� ���ٸ�
			std::vector<TCHAR*>		PathVec;

			for (size_t i = 0; i < iPathCount; ++i)
			{
				char	strPath[MAX_PATH] = {};
				int iSaveCount = 0;
				fread(&iSaveCount, sizeof(int), 1, _pFile);
				fread(strPath, sizeof(char), iSaveCount, _pFile);

				TCHAR* strLoadPath = new TCHAR[MAX_PATH];
				memset(strLoadPath, 0, sizeof(TCHAR) * MAX_PATH);

#ifdef UNICODE
				MultiByteToWideChar(CP_ACP, 0, strPath, -1, strLoadPath, (int)(strlen(strPath) * 2));
#else
				strcpy_s(strLoadPath, strPath);
#endif // UNICODE

				PathVec.push_back(strLoadPath);

			}
			*_ppTexture = new TextureSet;
			GET_SINGLETON(ResourcesManager)->LoadTexture(strTextureKey, PathVec, PATH_ROOT);
			(*_ppTexture)->pTexture = GET_SINGLETON(ResourcesManager)->FindTexture(strTextureKey);
		}

		fread(&(*_ppTexture)->iTextureRegister, sizeof(int), 1, _pFile);
	}
}
