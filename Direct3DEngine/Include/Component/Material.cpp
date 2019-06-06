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

	// 컨테이너만큼
	for (size_t i = 0; i < _Component.m_vecMaterialContainer.size(); ++i)
	{
		PMaterialContainer pContainer = new MaterialContainer;

		// 서브셋만큼
		for (size_t j = 0; j < _Component.m_vecMaterialContainer[i]->vecSubSet.size(); ++j)
		{
			PMaterialSubSet pSubSet = new MaterialSubSet;

			pSubSet->tMaterialData = _Component.m_vecMaterialContainer[i]->vecSubSet[j]->tMaterialData;

			// 예전엔 diffuse, specular, normal 다 따로 넣어줬는데 이젠 한꺼번에 관리하니까 그냥 넣어준다!
			std::unordered_map<std::string, PTextureSet>::const_iterator StartIter = 
				_Component.m_vecMaterialContainer[i]->vecSubSet[j]->mapTextureSet.begin();
			std::unordered_map<std::string, PTextureSet>::const_iterator EndIter =
				_Component.m_vecMaterialContainer[i]->vecSubSet[j]->mapTextureSet.end();

			// 셋팅해준다 (복사)
			for (; StartIter != EndIter; ++StartIter)
			{
				// 얇은 복사
				PTextureSet pTextureSet = new TextureSet;
				*pTextureSet = *(StartIter->second);

				if (nullptr != pTextureSet->pTexture)
				{
					// 텍스처가 있다면, 복사하니까 레퍼런스카운터를 증가시킨다.
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

// 현재 우리코드에선 셰이더에서 Convert작업할때 0~1값만 넣어주게 되어 있으므로
// Emissive값은 사실상 0~1만 넣어줄 수 있다.
// 그 이상 넣고 싶으면 Emissive만 0~1말고 따로 float으로 빼내서 작업해주면된다.
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

// 부분에 Emissive 주기
void Material::SetEmissiveColor(int _iContainer, int _iSubSet, float _fEmissive)
{
	m_vecMaterialContainer[_iContainer]->vecSubSet[_iSubSet]->tMaterialData.vEmissive.x = _fEmissive;
	m_vecMaterialContainer[_iContainer]->vecSubSet[_iSubSet]->tMaterialData.vEmissive.y = _fEmissive;
	m_vecMaterialContainer[_iContainer]->vecSubSet[_iSubSet]->tMaterialData.vEmissive.z = _fEmissive;
	m_vecMaterialContainer[_iContainer]->vecSubSet[_iSubSet]->tMaterialData.vEmissive.w = 1.0f;
}

// 텍스처 추가하기
void Material::AddTextureSet(int _iContainer, int _iSubSet
	, int _iTextureRegister, const std::string & _strTextureName
	, const TCHAR * _pFileName /*= nullptr*/
	, const std::string & _strPathName /*= PATH_TEXTURE*/)
{
	// 컨테이너 번호가 벡터의 크기를 넘으면 새로 만들어준다.
	if (_iContainer >= (int)(m_vecMaterialContainer.size()))
	{
		PMaterialContainer pContainer = new MaterialContainer;
		m_vecMaterialContainer.push_back(pContainer);
	}

	if (_iSubSet >= (int)(m_vecMaterialContainer[_iContainer]->vecSubSet.size()))
	{
		// 서브셋의 번호가 해당 머티리얼 컨테이너의 서브셋의 수보다 크면
		PMaterialSubSet pSubSet = new MaterialSubSet;
		m_vecMaterialContainer[_iContainer]->vecSubSet.push_back(pSubSet);
	}

	// 해당 텍스처가 이미 해당 컨테이너의 서브셋에 있다면 return -> 바꾸고 싶음 Change하셈
	if (nullptr != FindTextureSet(_iContainer, _iSubSet, _strTextureName))
	{
		return;
	}

	PMaterialContainer pContainer = m_vecMaterialContainer[_iContainer];
	PMaterialSubSet pSubSet = pContainer->vecSubSet[_iSubSet];

	// 빈텍스처를 만들고
	PTextureSet	pTextureSet = new TextureSet;

	// 텍스처를 로드한다.
	if (nullptr != _pFileName) 
	{
		GET_SINGLETON(ResourcesManager)->LoadTexture(_strTextureName, _pFileName, _strPathName);
	}

	// 빈 텍스처에 로드한 텍스처를 넣어주고 ( 레지스터 번호도.. )
	pTextureSet->pTexture = GET_SINGLETON(ResourcesManager)->FindTexture(_strTextureName);
	pTextureSet->iTextureRegister = _iTextureRegister;

	// 서브셋에 추가한다.
	pSubSet->mapTextureSet.insert(std::make_pair(_strTextureName, pTextureSet));

	// 식별번호 
	if (1 == pSubSet->mapTextureSet.size())
	{
		m_iSerialNumber = pTextureSet->pTexture->GetSerialNumber();
	}
}

// AnimationFrame class
void Material::AddTextureSet(int _iContainer, int _iSubSet, 
	int _iTextureRegister, Texture * _pTexture)
{
	// 컨테이너 번호가 벡터의 크기를 넘으면 새로 만들어준다.
	if (_iContainer >= (int)(m_vecMaterialContainer.size()))
	{
		PMaterialContainer pContainer = new MaterialContainer;
		m_vecMaterialContainer.push_back(pContainer);
	}

	if (_iSubSet >= (int)(m_vecMaterialContainer[_iContainer]->vecSubSet.size()))
	{
		// 서브셋의 번호가 해당 머티리얼 컨테이너의 서브셋의 수보다 크면
		PMaterialSubSet pSubSet = new MaterialSubSet;
		m_vecMaterialContainer[_iContainer]->vecSubSet.push_back(pSubSet);
	}

	// 해당 텍스처가 이미 해당 컨테이너의 서브셋에 있다면 return -> 바꾸고 싶음 Change하셈
	if (nullptr != FindTextureSet(_iContainer, _iSubSet, _pTexture->GetTag()))
	{
		return;
	}

	PMaterialContainer pContainer = m_vecMaterialContainer[_iContainer];
	PMaterialSubSet pSubSet = pContainer->vecSubSet[_iSubSet];

	// 빈텍스처를 만들고
	PTextureSet pTextureSet = new TextureSet;

	_pTexture->AddReference();

	pTextureSet->pTexture = _pTexture;
	pTextureSet->iTextureRegister = _iTextureRegister;

	// 서브셋에 추가한다.
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
	// 컨테이너 번호가 벡터의 크기를 넘으면 새로 만들어준다.
	if (_iContainer >= (int)(m_vecMaterialContainer.size()))
	{
		PMaterialContainer pContainer = new MaterialContainer;
		m_vecMaterialContainer.push_back(pContainer);
	}

	if (_iSubSet >= (int)(m_vecMaterialContainer[_iContainer]->vecSubSet.size()))
	{
		// 서브셋의 번호가 해당 머티리얼 컨테이너의 서브셋의 수보다 크면
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

// 텍스처 셋팅한거 삭제하기(해당 컨테이너와 서브셋을 찾아서 삭제 ㅇㅅㅇ)
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

	// 없으면
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
	// 전부 다지운다.
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
	// 컨테이너 번호가 벡터의 크기를 넘으면 새로 만들어준다. -> 체[인지니까 만들지말자
	if (_iContainer >= (int)(m_vecMaterialContainer.size()))
	{
		assert(false);
		return;
	}

	if (_iSubSet >= (int)(m_vecMaterialContainer[_iContainer]->vecSubSet.size()))
	{
		// 서브셋의 번호가 해당 머티리얼 컨테이너의 서브셋의 수보다 크면
		assert(false);
		return;
	}

	PMaterialContainer pContainer = m_vecMaterialContainer[_iContainer];
	PMaterialSubSet pSubSet = pContainer->vecSubSet[_iSubSet];

	// 바꿔질애..(기존꺼)
	PTextureSet pOriginSet = FindTextureSet(_iContainer, _iSubSet, _strOriTextuerName);

	// 이미 존재한다면 지워준다.
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

	// 바꿀 텍스처 로드
	if (nullptr != _pFileName)
	{
		GET_SINGLETON(ResourcesManager)->LoadTexture(_strChangeTextureName, _pFileName, _strPathName);
	}

	pOriginSet->pTexture = GET_SINGLETON(ResourcesManager)->FindTexture(_strChangeTextureName);

	// 체인지 될 텍스처가 없으면 터트리자
	if (nullptr == pOriginSet->pTexture)
	{
		SAFE_RELEASE(pOriginSet->pTexture);
		SAFE_DELETE(pOriginSet);
		return;
	}

	// 키 교체
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
	// 컨테이너 번호가 벡터의 크기를 넘으면 새로 만들어준다. -> 체[인지니까 만들지말자
	if (_iContainer >= (int)(m_vecMaterialContainer.size()))
	{
		assert(false);
		return;
	}

	if (_iSubSet >= (int)(m_vecMaterialContainer[_iContainer]->vecSubSet.size()))
	{
		// 서브셋의 번호가 해당 머티리얼 컨테이너의 서브셋의 수보다 크면
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

	// 키 교체
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
	// 컨테이너 번호가 벡터의 크기를 넘으면 새로 만들어준다.
	if (_iContainer >= (int)(m_vecMaterialContainer.size()))
	{
		assert(false);
		return;
	}

	if (_iSubSet >= (int)(m_vecMaterialContainer[_iContainer]->vecSubSet.size()))
	{
		// 서브셋의 번호가 해당 머티리얼 컨테이너의 서브셋의 수보다 크면
		assert(false);
		return;
	}

	PMaterialContainer pContainer = m_vecMaterialContainer[_iContainer];
	PMaterialSubSet pSubSet = pContainer->vecSubSet[_iSubSet];

	// 바꿔질애..(기존꺼)
	PTextureSet pOriginSet = FindTextureSet(_iContainer, _iSubSet, _strOriTextuerName);

	// 이미 존재한다면 지워주고 
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

	// 체인지 될 텍스처가 없으면 터트리자
	if (nullptr == pOriginSet->pTexture)
	{
		assert(false);
		return;
	}

	// 키 교체
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

	// 이미 존재한다면 지워주고 
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

	// 체인지하는데 ..?텍스처 없으면 터트리자
	if (nullptr == pOriginSet->pTexture)
	{
		assert(false);
		return;
	}

	// 키를 교체한다.
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

	// 이미 존재한다면 지워주고 
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

	// 체인지하는데 ..?텍스처 없으면 터트리자
	if (nullptr == pOriginSet->pTexture)
	{
		assert(false);
		return;
	}

	// 키를 교체한다. ( 기존의 텍스처를 지우고 바꿀 텍스처로 바꾼다. )
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

	pSubset->tMaterialData.iNormal = 1;			// 노말이 없으면 0 있으면 1
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

	pSubset->tMaterialData.iSpeuclar = 1;		// spec이 없으면 0 있으면 1
}

// 텍스처 찾기
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

// 스키닝
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

	pSubSet->tMaterialData.iSkinning = 1;			// 애니메이션이 있다고 알려준다. 
	
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

	// 노말맵, 디퓨즈 등등 원래 다 갖고 있었는데 이제 map으로 관리하니까
	// 서브셋이 map으로 다 갖고 있다. 원래는 각각 픽셀셰이더로 넣어줬음
	// 현재는 텍스처레지스터 번호로 디퓨즈인지 다른거인지 셰이더로 알려준다. ㅇㅅㅇ
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

		// 텍스처 이름 저장 (키값)
		std::string strTextureKey = _pTexture->pTexture->GetTag();
		size_t		iKeyLength = strTextureKey.length();
		fwrite(&iKeyLength, sizeof(size_t), 1, _pFile);
		fwrite(strTextureKey.c_str(), 1, iKeyLength, _pFile);

		// Texture 경로를 얻어온다. 
		const std::vector<TCHAR*>* pPathList = _pTexture->pTexture->GetFullPath();

		size_t iPathCount = pPathList->size();
		fwrite(&iPathCount, sizeof(size_t), 1, _pFile);

		const char* pRootPath = GET_SINGLETON(PathManager)->FindPathMultibyte(PATH_ROOT);
		char strRoot[MAX_PATH] = {};

		int iRootLength = (int)(strlen(pRootPath));

		// 2를 뺀 이유는 맨뒤에 \\나 / 를 제외시키기 위함임.
		// 경로를 뒤에서부터 읽어온다.
		for (int i = iRootLength - 2; i >= 0; --i)
		{
			// 루트 경로를 가져오기..
			if ('\\' == pRootPath[i] || '/' == pRootPath[i])
			{
				memcpy(strRoot, &pRootPath[i + 1], sizeof(char) * (iRootLength - (i + 2)));
				break;
			}
		}

		// 해당 경로를 거꾸로 뒤집는다.
		// 위에서 뒤에서부터 읽어오니까 뒤집어줘야함
		iRootLength = (int)(strlen(strRoot));

		char strReverse[MAX_PATH] = {};		// 뒤집힌걸 저장할 변수 ( 맞게 뒤집힌거)

		for (int i = 0; i < iRootLength; ++i)
		{
			strReverse[i] = strRoot[iRootLength - (i + 1)];
		}

		// 다시 경로 넣어주기 (제대로)
		memcpy(strRoot, strReverse, MAX_PATH);

		// 대문자로 바꾸기
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
		// 텍스처가 없는 경우
		fwrite(&bTextureEnable, sizeof(bool), 1, _pFile);
	}
}

void Material::LoadTextureSet(FILE * _pFile, PTextureSet * _ppTexture)
{
	bool bTextureEnable = false;
	fread(&bTextureEnable, sizeof(bool), 1, _pFile);

	// 텍스처가 있다면
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

			// 텍스처 로드하기
			GET_SINGLETON(ResourcesManager)->LoadTexture(strTextureKey, strLoadPath, PATH_ROOT);
			(*_ppTexture)->pTexture = GET_SINGLETON(ResourcesManager)->FindTexture(strTextureKey);
		}
		else
		{
			// 텍스처가 없다면
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
