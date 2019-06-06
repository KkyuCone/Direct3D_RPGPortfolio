#include "Component.h"
#include "..\GameObject.h"
#include "Transform.h"

ENGINE_USING

// 생성자, 소멸자

Component::Component()
	: m_pScene(nullptr)
	, m_pLayer(nullptr)
	, m_pGameObject(nullptr)
	, m_pTransform(nullptr)
{
}

Component::Component(const Component & _Component)
{
	*this = _Component;
	m_iReferenceCount = 1;
}

Component::~Component()
{
}

// Get Set함수

Scene * Component::GetScene() const
{
	return m_pScene;
}

Layer * Component::GetLayer() const
{
	return m_pLayer;
}

GameObject * Component::GetGameObject() const
{
	// 참조를 하는거니까 추가해준다.
	m_pGameObject->AddReference();
	return m_pGameObject;
}

Transform * Component::GetTransform() const
{
	m_pTransform->AddReference();
	return m_pTransform;
}

void Component::SetTransform(Transform * _Transform)
{
	m_pTransform = _Transform;
}

void Component::SetScene(Scene * _pScene)
{
	m_pScene = _pScene;
}

void Component::SetLayer(Layer * _pLayer)
{
	m_pLayer = _pLayer;
}

void Component::SetGameObject(GameObject * _pGameObject)
{
	m_pGameObject = _pGameObject;
}

bool Component::IsObjectEnable() const
{
	return m_pGameObject->IsEnable();
}

bool Component::IsObjectActive() const
{
	return m_pGameObject->IsActive();
}

COMPONENT_TYPE Component::GetComponentType() const
{
	return m_eComponentType;
}

// Start~Update, clone함수

void Component::Start()
{
}

bool Component::Init()
{
	return true;
}

int Component::Input(float _fTime)
{
	return 0;
}

int Component::Update(float _fTime)
{
	return 0;
}

int Component::LateUpdate(float _fTime)
{
	return 0;
}

int Component::Collision(float _fTime)
{
	return 0;
}

int Component::PrevRender(float _fTime)
{
	return 0;
}

int Component::PrevShadowRender(Matrix _matLightView, Matrix _matLightProj, float _fTime)
{
	return 0;
}

int Component::Render(float _fTime)
{
	return 0;
}

int Component::RenderShadow(class Shader* _pShader, float _fTime)
{
	return 0;
}

Component * Component::Clone() const
{
	/*return new Component(*this);*/
	return nullptr;
}

void Component::EraseComponentFromTag(const std::string & _strTag)
{
	m_pGameObject->EraseComponentFromTag(_strTag);
}

void Component::EraseComponentFromType(COMPONENT_TYPE _eType)
{
	m_pGameObject->EraseComponentFromType(_eType);
}

void Component::EraseComponent(Component * _pComponent)
{
	m_pGameObject->EraseComponent(_pComponent);
}
