#include "Scene.h"
#include "SceneComponent.h"
#include "Layer.h"
#include "..\GameObject.h"
#include "..\Component\Camera.h"
#include "..\Component\Transform.h"
#include "..\Component\Renderer.h"
#include "..\Component\Material.h"
#include "..\Device.h"
#include "..\Component\Light.h"
#include "..\SoundManager.h"

ENGINE_USING

Scene::Scene() 
	: m_pMainCameraObject(nullptr)
	, m_pMainCamera(nullptr)
	, m_pMainCameraeTransform(nullptr)
	, m_pSkyObject(nullptr)
	, m_pSkyMaterial(nullptr)
	, m_pUICameraObject(nullptr)
	, m_pUICamera(nullptr)
	, m_pUICameraeTransform(nullptr)
{
}


Scene::~Scene()
{
	GET_SINGLETON(SoundManager)->DeleteSound(this);			// �ش� ���� ���� �����

	SAFE_RELEASE(m_pSkyMaterial);
	SAFE_RELEASE(m_pSkyObject);

	// ���� ī�޶� �����, ī�޶������
	Safe_Release_Map(m_CameraMap);
	SAFE_RELEASE(m_pMainCameraObject);
	SAFE_RELEASE(m_pMainCamera);
	SAFE_RELEASE(m_pMainCameraeTransform);

	// UI ī�޶�
	SAFE_RELEASE(m_pUICameraObject);
	SAFE_RELEASE(m_pUICamera);
	SAFE_RELEASE(m_pUICameraeTransform);


	// �ش���� ������Ÿ�� �����
	GameObject::RemoveProtoType(this);

	// SceneComponentList ����
	std::list<class SceneComponent*>::iterator SceneComStart = m_SceneComponentList.begin();
	std::list<class SceneComponent*>::iterator SceneComEnd = m_SceneComponentList.end();

	for (; SceneComStart != SceneComEnd; ++SceneComStart)
	{
		SAFE_DELETE((*SceneComStart));
	}

	m_SceneComponentList.clear();


	// LayerList ����
	std::list<class Layer*>::iterator LayerStart = m_LayerList.begin();
	std::list<class Layer*>::iterator LayerEnd = m_LayerList.end();

	for (; LayerStart != LayerEnd; ++LayerStart)
	{
		SAFE_DELETE((*LayerStart));
	}

	m_LayerList.clear();
}

#pragma region Camera Get Fucntion : ����ī�޶�
GameObject * Scene::GetMainCameraObject() const
{
	m_pMainCameraObject->AddReference();
	return m_pMainCameraObject;
}

Camera * Scene::GetMainCamera() const
{
	m_pMainCamera->AddReference();
	return m_pMainCamera;
}

Transform * Scene::GetMainCameraTransform() const
{
	m_pMainCameraeTransform->AddReference();
	return m_pMainCameraeTransform;
}

Matrix Scene::GetViewMatrix() const
{
	return m_pMainCamera->GetViewMatrix();
}
Matrix Scene::GetProjMatrix() const
{
	return m_pMainCamera->GetProjectionMatrix();
}
Vector3 Scene::GetMainCameraPos() const
{
	return m_pMainCameraeTransform->GetWorldPosition();
}

#pragma endregion

#pragma region UI ī�޶�
GameObject * Scene::GetUICameraObject() const
{
	m_pUICameraObject->AddReference();
	return m_pUICameraObject;
}
Camera * Scene::GetUICamera() const
{
	m_pUICamera->AddReference();
	return m_pUICamera;
}
Transform * Scene::GetUICameraTransform() const
{
	m_pUICameraeTransform->AddReference();
	return m_pUICameraeTransform;
}
Matrix Scene::GetUIViewMatrix() const
{
	return m_pUICamera->GetViewMatrix();
}
Matrix Scene::GetUIProjMatrix() const
{
	return m_pUICamera->GetProjectionMatrix();
}
Vector3 Scene::GetUICameraPos() const
{
	return m_pUICameraeTransform->GetWorldPosition();
}
#pragma endregion


bool Scene::Init()
{
	// ���̾� �����
	AddLayer("Default", 0);
	AddLayer("UI", INT_MAX - 10);			// UI�� ���� �������� �����ؾ��ϱ� ����

	// ���� �ּ��� ī�޶� �ϳ� �����ؾ��Ѵ�. (�׷��� ����)
	// �׷��� ��ó���� �־��� ī�޶� ����ī�޶�� �����Ѵ�. ���Ŀ� �ٲ� �� ����
	//AddCamera("MainCamera", Vector3(0.0f, 0.0f, -5.0f), Vector3::Zero, CAM_PERSPECTIVE,
	//	_RESOLUTION.iWidth, _RESOLUTION.iHeight, 90.0f, 0.03f, 1000.0f);

	AddCamera("MainCamera", Vector3(0.0f, 15.0f, -50.0f), Vector3::Zero, CAM_PERSPECTIVE,
		_RESOLUTION.iWidth, _RESOLUTION.iHeight, 90.0f, 0.03f, 1000.0f);

	// UI ī�޶� - ���� ����
	AddCamera("UICamera", Vector3(0.0f, 0.0f, 0.0f), Vector3::Zero, CAM_ORTHOGONAL,
		_RESOLUTION.iWidth, _RESOLUTION.iHeight, 90.0f, 0.0f, 1000.0f);

	m_pUICameraObject = FindCamera("UICamera");
	m_pUICamera = m_pUICameraObject->FindComponentFromType<Camera>(CT_CAMERA);
	m_pUICameraeTransform = m_pUICameraObject->GetTransform();

	// ��ī��
	m_pSkyObject = GameObject::CreateObject("SkyObject");
	m_pSkyObject->SetScene(this);

	Transform* pSkyTransform = m_pSkyObject->GetTransform();
	pSkyTransform->SetWorldScale(100000.0f, 100000.0f, 100000.0f);
	pSkyTransform->LateUpdate(0.0f);			// ��� ������ ���� �س��� ���ؼ� LateUpdateȣ��
	SAFE_RELEASE(pSkyTransform);

	Renderer* pSkyRenderer = m_pSkyObject->AddComponent<Renderer>("SkyRenderer");
	pSkyRenderer->SetMesh("Sphere");
	pSkyRenderer->SetRenderState("CullNone");
	pSkyRenderer->SetRenderState("LessEqual");
	SAFE_RELEASE(pSkyRenderer);

	m_pSkyMaterial = m_pSkyObject->AddComponent<Material>("SkyMaterial");
	m_pSkyMaterial->AddTextureSet(0, 0, 10, "EngineSky", TEXT("Sky.dds"));

	m_pSkyObject->EraseComponentFromTag("PickSphere");		// ��ī�� �ڽ��� ��ŷ�� �ݶ��̴� �ʿ����
	m_pSkyObject->Start();


	// ���� ���� -  Global Light
	// Spot Light
	Layer* pDefaultLayer = FindLayer("Default");

	//GameObject* pGLSpotObject = GameObject::CreateObject("GlobalLight", pDefaultLayer);

	//Transform* pGLSpotTransform = pGLSpotObject->GetTransform();
	//pGLSpotTransform->SetWorldRotateX(90.0f);
	//pGLSpotTransform->SetWorldPosition(0.0f, 2.0f, 0.0f);

	//Light* pGLSpotight = pGLSpotObject->AddComponent<Light>("GlobalLight");
	////pGlobalLight->SetLightType(LT_DIR);
	//pGLSpotight->SetLightType(LT_SPOT);
	//pGLSpotight->SetLightDistance(10.0f);
	//pGLSpotight->SetLightAngle(60.0f, 90.0f);
	//pGLSpotight->SetLightColor(Vector4::Red, Vector4::Red * 0.2f,
	//	Vector4::Red);

	//SAFE_RELEASE(pGLSpotight);
	//SAFE_RELEASE(pGLSpotTransform);
	//SAFE_RELEASE(pGLSpotObject);

	//// ������ ��쾾�� �˷��ع��
	////LightInfo CurLightInfo = pGLSpotight->GetLightInfo();
	////float SpotDistance = CurLightInfo.fDistance;

	////float HOutAngleValue = pGLSpotight->GetOutAngle() / 2;

	////Vector3 LightDirNorm = pGLSpotTransform->GetWorldRotation();

	////Vector3 OutAngleDirNorm = LightDirNorm;
	////OutAngleDirNorm.z += HOutAngleValue;

	////LightDirNorm.Normalize();
	////OutAngleDirNorm.Normalize();

	////Vector3 LenVector = LightDirNorm - OutAngleDirNorm;

	////float dddddd = LenVector.Length();

	////dddddd *= SpotDistance;

	////pGLSpotTransform->SetWorldScale(dddddd, SpotDistance, dddddd);


	//////////////////////////////////////////////////////

	//// ��������Ʈ ũ�⿡���� �����  �̰��̰�
	////LightInfo CurLightInfo = pGLSpotight->GetLightInfo();

	////float SpotDistance = CurLightInfo.fDistance;
	////float OutAngleH = pGLSpotight->GetOutAngle() / 2.0f;
	////float nAngle = 90.0f - OutAngleH;
	////float TangentAngleValue = tanf(DegreeToRadian(nAngle));
	////float SpotRadianValue = (SpotDistance / TangentAngleValue)* 2.0f;


	////pGLSpotTransform->SetWorldScaleX(SpotRadianValue);
	////pGLSpotTransform->SetWorldScaleY(CurLightInfo.fDistance);
	////pGLSpotTransform->SetWorldScaleZ(SpotRadianValue);

	////SAFE_RELEASE(pGLSpotTransform);
	////SAFE_RELEASE(pGLSpotight);
	////SAFE_RELEASE(pGLSpotObject);


	//// Point Light
	//GameObject* pGLPointObject= GameObject::CreateObject("GlobalLight", pDefaultLayer);
	//Transform* pGLPointTransform = pGLPointObject->GetTransform();
	//pGLPointTransform->SetWorldPosition(0.0f, -1.0f, 0.0f);

	//SAFE_RELEASE(pGLPointTransform);

	//Light* pGLPointLight = pGLPointObject->AddComponent<Light>("GlobalLight");
	//pGLPointLight->SetLightType(LT_POINT);
	//pGLPointLight->SetLightDistance(10.0f);

	//SAFE_RELEASE(pGLPointLight);
	//SAFE_RELEASE(pGLPointObject);


	// Directioanl Light
	GameObject* pGLDirObject = GameObject::CreateObject("GlobalLight", pDefaultLayer);
	Transform* pGLDirTransform = pGLDirObject->GetTransform();
	pGLDirTransform->SetWorldPosition(-40.0f, 70.0f, 65.0f);
	pGLDirTransform->SetWorldRotateX(45.0f);
	pGLDirTransform->SetWorldRotateY(90.0f);

	SAFE_RELEASE(pGLDirTransform);

	Light* pGLDirLight = pGLDirObject->AddComponent<Light>("GlobalLight");
	pGLDirLight->SetLightType(LT_DIR);

	SAFE_RELEASE(pGLDirLight);
	SAFE_RELEASE(pGLDirObject);

	return true;
}

int Scene::Input(float _fTime)
{
	std::list<SceneComponent*>::iterator SceneComStart = m_SceneComponentList.begin();
	std::list<SceneComponent*>::iterator SceneComEnd = m_SceneComponentList.end();

	for (; SceneComStart != SceneComEnd;)
	{
		// Active�� �ش� ��ü�� ����ִ����� ���� (Death���� ����)
		// Enable�� �ش� ��ü�� Ȱ��ȭ/��Ȱ��ȭ ���� (Update�� ���ϴ°��� ������)
		if (false == (*SceneComStart)->IsActive())
		{
			SAFE_DELETE((*SceneComStart));
			SceneComStart = m_SceneComponentList.erase(SceneComStart);
			continue;
		}
		else if (false == (*SceneComStart)->IsEnable())
		{
			++SceneComStart;
			continue;
		}

		(*SceneComStart)->Input(_fTime);

		++SceneComStart;
	}


	std::list<class Layer*>::iterator LayerStart = m_LayerList.begin();
	std::list<class Layer*>::iterator LayerEnd = m_LayerList.end();

	for (; LayerStart != LayerEnd; )
	{
		if (false == (*LayerStart)->IsActive())
		{
			SAFE_DELETE(*LayerStart);
			LayerStart = m_LayerList.erase(LayerStart);
			continue;

		}
		else if (false == (*LayerStart)->IsEnable())
		{
			++LayerStart;
			continue;
		}

		(*LayerStart)->Input(_fTime);

		++LayerStart;
	}

	return 0;
}

int Scene::Update(float _fTime)
{
	std::list<class SceneComponent*>::iterator SceneComStart = m_SceneComponentList.begin();
	std::list<class SceneComponent*>::iterator SceneComEnd = m_SceneComponentList.end();

	for (; SceneComStart != SceneComEnd;)
	{
		if (false == (*SceneComStart)->IsActive())
		{
			SAFE_DELETE((*SceneComStart));
			SceneComStart = m_SceneComponentList.erase(SceneComStart);
			continue;
		}
		else if (false == (*SceneComStart)->IsEnable())
		{
			++SceneComStart;
			continue;
		}

		(*SceneComStart)->Update(_fTime);

		++SceneComStart;
	}


	std::list<class Layer*>::iterator LayerStart = m_LayerList.begin();
	std::list<class Layer*>::iterator LayerEnd = m_LayerList.end();

	for (; LayerStart != LayerEnd; )
	{
		if (false == (*LayerStart)->IsActive())
		{
			SAFE_DELETE(*LayerStart);
			LayerStart = m_LayerList.erase(LayerStart);
			continue;

		}
		else if (false == (*LayerStart)->IsEnable())
		{
			++LayerStart;
			continue;
		}

		(*LayerStart)->Update(_fTime);

		++LayerStart;
	}

	m_pMainCameraObject->Update(_fTime);

	return 0;
}

int Scene::LateUpdate(float _fTime)
{
	std::list<class SceneComponent*>::iterator SceneComStart = m_SceneComponentList.begin();
	std::list<class SceneComponent*>::iterator SceneComEnd = m_SceneComponentList.end();

	for (; SceneComStart != SceneComEnd;)
	{
		if (false == (*SceneComStart)->IsActive())
		{
			SAFE_DELETE((*SceneComStart));
			SceneComStart = m_SceneComponentList.erase(SceneComStart);
			continue;
		}
		else if (false == (*SceneComStart)->IsEnable())
		{
			++SceneComStart;
			continue;
		}

		(*SceneComStart)->LateUpdate(_fTime);

		++SceneComStart;
	}


	std::list<class Layer*>::iterator LayerStart = m_LayerList.begin();
	std::list<class Layer*>::iterator LayerEnd = m_LayerList.end();

	for (; LayerStart != LayerEnd; )
	{
		if (false == (*LayerStart)->IsActive())
		{
			SAFE_DELETE(*LayerStart);
			LayerStart = m_LayerList.erase(LayerStart);
			continue;

		}
		else if (false == (*LayerStart)->IsEnable())
		{
			++LayerStart;
			continue;
		}

		(*LayerStart)->LateUpdate(_fTime);

		++LayerStart;
	}

	m_pMainCameraObject->LateUpdate(_fTime);
	m_pMainCameraObject->Render(_fTime);

	return 0;
}

int Scene::Collision(float _fTime)
{
	std::list<class SceneComponent*>::iterator SceneComStart = m_SceneComponentList.begin();
	std::list<class SceneComponent*>::iterator SceneComEnd = m_SceneComponentList.end();

	for (; SceneComStart != SceneComEnd;)
	{
		if (false == (*SceneComStart)->IsActive())
		{
			SAFE_DELETE((*SceneComStart));
			SceneComStart = m_SceneComponentList.erase(SceneComStart);
			continue;
		}
		else if (false == (*SceneComStart)->IsEnable())
		{
			++SceneComStart;
			continue;
		}

		(*SceneComStart)->Collision(_fTime);

		++SceneComStart;
	}


	std::list<class Layer*>::iterator LayerStart = m_LayerList.begin();
	std::list<class Layer*>::iterator LayerEnd = m_LayerList.end();

	for (; LayerStart != LayerEnd; )
	{
		if (false == (*LayerStart)->IsActive())
		{
			SAFE_DELETE(*LayerStart);
			LayerStart = m_LayerList.erase(LayerStart);
			continue;

		}
		else if (false == (*LayerStart)->IsEnable())
		{
			++LayerStart;
			continue;
		}

		(*LayerStart)->Collision(_fTime);

		++LayerStart;
	}

	m_pMainCameraObject->Collision(_fTime);

	return 0;
}

int Scene::Render(float _fTime)
{
	std::list<class SceneComponent*>::iterator SceneComStart = m_SceneComponentList.begin();
	std::list<class SceneComponent*>::iterator SceneComEnd = m_SceneComponentList.end();

	for (; SceneComStart != SceneComEnd;)
	{
		if (false == (*SceneComStart)->IsActive())
		{
			SAFE_DELETE((*SceneComStart));
			SceneComStart = m_SceneComponentList.erase(SceneComStart);
			continue;
		}
		else if (false == (*SceneComStart)->IsEnable())
		{
			++SceneComStart;
			continue;
		}

		(*SceneComStart)->Render(_fTime);

		++SceneComStart;
	}

	m_pSkyObject->Render(_fTime);

	std::list<class Layer*>::iterator LayerStart = m_LayerList.begin();
	std::list<class Layer*>::iterator LayerEnd = m_LayerList.end();

	for (; LayerStart != LayerEnd; )
	{
		if (false == (*LayerStart)->IsActive())
		{
			SAFE_DELETE(*LayerStart);
			LayerStart = m_LayerList.erase(LayerStart);
			continue;

		}
		else if (false == (*LayerStart)->IsEnable())
		{
			++LayerStart;
			continue;
		}

		(*LayerStart)->Render(_fTime);

		++LayerStart;
	}

	m_pMainCameraObject->Render(_fTime);

	return 0;
}

void Scene::AddLayer(const std::string & _strName, int _Order)
{
	Layer* pLayer = new Layer;

	pLayer->SetTag(_strName.c_str());
	pLayer->SetScene(this);

	if (false == pLayer->Init())
	{
		SAFE_DELETE(pLayer);
		return;
	}

	m_LayerList.push_back(pLayer);
	pLayer->SetOrder(_Order);
}

void Scene::SortLayer()
{
	// ���� ������� (��������)
	m_LayerList.sort(Scene::Sort);
}

// ���̾� ã��(Ű��)
Layer * Scene::FindLayer(const std::string & _strName)
{
	std::list<Layer*>::iterator StartIter = m_LayerList.begin();
	std::list<Layer*>::iterator EndIter = m_LayerList.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		if ((*StartIter)->GetTag() == _strName)
		{
			return *StartIter;
		}
	}
	return nullptr;
}

//GameObject * Scene::FindGameObejctToLayer(const std::string & _strName, const std::string & _Tag)
//{
//	Layer* pLayer = FindLayer(_strName);
//
//	if (nullptr == pLayer)
//		return nullptr;
//	else
//	{
//		GameObject* FGameObject = pLayer->FindGameObject(_Tag);
//
//		if (nullptr != FGameObject)
//			return FGameObject;
//	}
//	return nullptr;
//}
//
//GameObject * Scene::FindGameObjectFirst(const std::string & _Tag)
//{
//	// ���̾� ������� �ٷ� ���� ã�¾� ����
//	std::list<Layer*>::iterator StartIter = m_LayerList.begin();
//	std::list<Layer*>::iterator EndIter = m_LayerList.end();
//
//	for(; StartIter != EndIter;)
//	{
//		GameObject* FGameObject = (*StartIter)->FindGameObject(_Tag);
//
//		if (nullptr != FGameObject)
//		{
//			return FGameObject;
//		}
//		else
//		{
//			++StartIter;
//		}
//	}
//
//	return nullptr;
//}

// ī�޶� �߰��Լ�
bool Scene::AddCamera(const std::string & _strName, const Vector3 & _vPos, const Vector3 & _vRot, CAMERA_TYPE _eType, UINT _iWidth, UINT _iHeight, float _fAngle, float _fNear, float _fFar)
{
	// ī�޶� �߰��ϱ��� ī�޶� �ִ��� ã��
	GameObject* pCameraObject = FindCamera(_strName);

	// ������� �̸��� ī�޶� �̹� ������ false��ȯ (ī�޶��߰� ����)
	if (nullptr != pCameraObject)
	{
		SAFE_RELEASE(pCameraObject);
		return false;
	}

	// ī�޶� ������Ʈ �����
	pCameraObject = GameObject::CreateObject(_strName);
	pCameraObject->SetScene(this);

	// ī�޶� ������Ʈ Transform��������
	Transform* pTransform = pCameraObject->GetTransform();		// ���⼭ ���۷��� ī���� ����
	pTransform->SetWorldPosition(_vPos);
	pTransform->SetWorldRotation(_vRot);

	SAFE_RELEASE(pTransform);					// ���۷��� ī���� ���� (��������ϱ�)

	// ī�޶� ������Ʈ ����
	Camera* pCamera = pCameraObject->AddComponent<Camera>(_strName);
	pCamera->SetCameraInfo(_eType, _iWidth, _iHeight, _fAngle, _fNear, _fFar);

	// ���� ó���߰��Ǵ� ī�޶� MainCamera�� �����Ѵ�. 
	if (nullptr == m_pMainCameraObject)
	{
		pCameraObject->AddReference();
		m_pMainCameraObject = pCameraObject;

		pCamera->AddReference();
		m_pMainCamera = pCamera;

		m_pMainCameraeTransform = pCameraObject->GetTransform();
	}

	SAFE_RELEASE(pCamera);

	// ī�޶� Map���� �߰����ش�.
	m_CameraMap.insert(std::make_pair(_strName, pCameraObject));

	return true;
}

bool Scene::ChangeMainCamera(const std::string & _strName)
{
	// �ٲ� ī�޶� ī�޶� map�� �ִ� ã�´�.
	GameObject* pCameraObject = FindCamera(_strName);
	
	// �ٲ� ī�޶� ���¾ָ�
	if (nullptr != pCameraObject)
	{
		return false;
	}

	SAFE_RELEASE(m_pMainCameraObject);
	SAFE_RELEASE(m_pMainCamera);
	SAFE_RELEASE(m_pMainCameraeTransform);

	m_pMainCameraObject = pCameraObject;
	m_pMainCamera = pCameraObject->FindComponentFromType<Camera>(CT_CAMERA);
	m_pMainCameraeTransform = pCameraObject->GetTransform();

	return true;
}

GameObject * Scene::FindCamera(const std::string & _strName)
{
	std::unordered_map<std::string, GameObject*>::iterator FCamera = m_CameraMap.find(_strName);

	if (FCamera == m_CameraMap.end())
	{
		return nullptr;
	}

	FCamera->second->AddReference();			// ����ī���� �߰�
	return FCamera->second;
}

bool Scene::Sort(Layer * _pSrcc, Layer * _pDest)
{
	int iSrc = _pSrcc->GetOrder();
	int iDest = _pDest->GetOrder();

	return iSrc > iDest;
}
