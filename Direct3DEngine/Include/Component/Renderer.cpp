#include "Renderer.h"
#include "..\Render\Shader.h"
#include "..\Render\ShaderManager.h"
#include "..\Resource\Mesh.h"
#include "..\Resource\ResourcesManager.h"
#include "..\Device.h"
#include "Transform.h"
#include "Material.h"
#include "..\GameObject.h"
#include "..\Render\RenderManager.h"
#include "..\Render\RenderState.h"
#include "Light.h"
#include "Animation.h"
#include "ColliderSphere.h"
#include "LandScape.h"
#include "..\Render\ShaderManager.h"

ENGINE_USING


Renderer::Renderer()
	: m_pShader(nullptr)
	, m_pMesh(nullptr)
	, m_pInputLayout(nullptr)
	, m_pMaterial(nullptr)
{
	tLimRenderGroup = {};
	memset(m_pRenderState, 0, sizeof(RenderState*) * RS_END);
	m_eComponentType = CT_RENDERER;
}

Renderer::Renderer(const Renderer & _Component) : Component(_Component)
{
	// ���������
	*this = _Component;
	m_iReferenceCount = 1;

	if (nullptr != m_pShader)
	{
		m_pShader->AddReference();
	}

	if (nullptr != m_pMesh)
	{
		m_pMesh->AddReference();
	}


	// ����������Ʈ�� �־��ֱ�
	for (int i = 0; i < RS_END; ++i)
	{
		if (nullptr != m_pRenderState[i])
		{
			m_pRenderState[i]->AddReference();
		}
	}
}


Renderer::~Renderer()
{
	for (int i = 0; i < RS_END; ++i)
	{
		SAFE_RELEASE(m_pRenderState[i]);
	}

	SAFE_RELEASE(m_pMaterial);
	SAFE_RELEASE(m_pMesh);
	SAFE_RELEASE(m_pShader);
}

Mesh * Renderer::GetMesh() const
{
	if (nullptr != m_pMesh)
	{
		m_pMesh->AddReference();
	}

	return m_pMesh;
}

void Renderer::SetMesh(const std::string & _strKey)
{
	// ������ �ִ� �޽��� ��ü�� ��쿡 �����Ÿ� �����ؾ��� ������
	SAFE_RELEASE(m_pMesh);

	m_pMesh = GET_SINGLETON(ResourcesManager)->FindMesh(_strKey);

	if (nullptr != m_pMesh)
	{
		SetShader(m_pMesh->GetShaderKey());
		SetInputLayout(m_pMesh->GetInputLayoutKey());

		// �޽��� �ִ� ���  ��ŷ�� �ݶ��̴��� �߰��Ѵ�.
		ColliderSphere* m_pPickSphere = FindComponentFromTag<ColliderSphere>("PickSphere");
		m_pPickSphere->SetSphereInfo(m_pMesh->GetCenter(), m_pMesh->GetRadiuse());
		m_pPickSphere->PickEnable(true);
		SAFE_RELEASE(m_pPickSphere);

	}

	// ��
	m_pTransform->SetLocalRelativeView(m_pMesh->GetView());
}

void Renderer::SetMesh(const std::string & _strKey, const TCHAR * _pFileName, 
	const Vector3 & _vView, const std::string & _strPathKey)
{
	SAFE_RELEASE(m_pMesh);

	GET_SINGLETON(ResourcesManager)->LoadMesh(_strKey, _pFileName, _vView, _strPathKey);

	m_pMesh = GET_SINGLETON(ResourcesManager)->FindMesh(_strKey);

	if (nullptr != m_pMesh)
	{
		SetShader(m_pMesh->GetShaderKey());
		SetInputLayout(m_pMesh->GetInputLayoutKey());

		// �޽��� �ִ� ���  ��ŷ�� �ݶ��̴��� �߰��Ѵ�.
		ColliderSphere* m_pPickSphere = FindComponentFromTag<ColliderSphere>("PickSphere");
		m_pPickSphere->SetSphereInfo(m_pMesh->GetCenter(), m_pMesh->GetRadiuse());
		m_pPickSphere->PickEnable(true);
		SAFE_RELEASE(m_pPickSphere);
	

		// ���� ��Ƽ������ �ִٸ� ����� �־��ش�.
		Material*	pMaterial = m_pMesh->CloneMaterial();

		if (nullptr != pMaterial)
		{
			EraseComponentFromType(CT_MATERIAL);
			m_pGameObject->AddComponent(pMaterial);
			m_pMaterial = pMaterial;
		}

		// �̹� �ִϸ��̼� ������Ʈ�� �ִٸ� (FBX�ε���..)
		// �װ� ����� ���� �־��ش�.
		// ������ �ִϸ��̼� ������Ʈ�� �ʼ��� �־����
		// �ϴ� Ȥ�� ��.. �������� ���� �� �ֱ⶧���� 
		// �װ� ����� ���� �޾ƿ¾ָ� �־��ִ°�
		// �ִϸ��̼� ������Ʈ�� ���ٸ� �ش� �޽��� ����ƽ �޽���
		Animation* pAnimation = m_pMesh->CloneAnimation();

		if (nullptr != pAnimation)
		{
			EraseComponentFromType(CT_ANIMATION);
			m_pGameObject->AddComponent(pAnimation);
			SAFE_RELEASE(pAnimation);		// �������� �ִϸ��̼��� ���� ���� �ʿ䰡 ���⋚���� �����ش٤�.
		}
	}

	m_pTransform->SetLocalRelativeView(m_pMesh->GetView());
}

void Renderer::SetMeshFromFullPath(const std::string & _strKey, const TCHAR * _pFileName, const Vector3 & _vView)
{
	SAFE_RELEASE(m_pMesh);
	GET_SINGLETON(ResourcesManager)->LoadMeshFromFullPath(_strKey, _pFileName, _vView);
	m_pMesh = GET_SINGLETON(ResourcesManager)->FindMesh(_strKey);

	if (nullptr != m_pMesh)
	{
		SetShader(m_pMesh->GetShaderKey());
		SetInputLayout(m_pMesh->GetInputLayoutKey());

		// �޽��� �ִ� ���  ��ŷ�� �ݶ��̴��� �߰��Ѵ�.
		ColliderSphere* m_pPickSphere = FindComponentFromTag<ColliderSphere>("PickSphere");
		m_pPickSphere->SetSphereInfo(m_pMesh->GetCenter(), m_pMesh->GetRadiuse());
		m_pPickSphere->PickEnable(true);
		SAFE_RELEASE(m_pPickSphere);

		Material*	pMaterial = m_pMesh->CloneMaterial();			// ��Ƽ������ �ִٸ� �� ��Ƽ���� ���纻�� �ѱ�

		if (nullptr != pMaterial)
		{
			EraseComponentFromType(CT_MATERIAL);
			m_pGameObject->AddComponent(pMaterial);
			m_pMaterial = pMaterial;
		}

		Animation*	pAnimation = m_pMesh->CloneAnimation();			// �굵 ���纻 �ѱ�

		if (nullptr != pAnimation)
		{
			EraseComponentFromType(CT_ANIMATION);
			m_pGameObject->AddComponent(pAnimation);
			SAFE_RELEASE(pAnimation);
		}
	}  

	m_pTransform->SetLocalRelativeView(m_pMesh->GetView());
}

void Renderer::SetShader(const std::string & _strKey)
{
	// ���̴� �������ֱ� ( Ű������ � ���̴��� ����Ұ������� ���� ����)
	// �����Ŵ� ��� ����!
	SAFE_RELEASE(m_pShader);
	m_pShader = GET_SINGLETON(ShaderManager)->FindShader(_strKey);
}

void Renderer::SetInputLayout(const std::string & _strKey)
{
	// �� ������ �ʿ���� COM �������̽��� ����ϴϱ� ������
	m_pInputLayout = GET_SINGLETON(ShaderManager)->FindInputLayout(_strKey);
}

void Renderer::SetRenderState(const std::string & _strName)
{
	RenderState*	pState = GET_SINGLETON(RenderManager)->FindRenderState(_strName);

	if (nullptr == pState)
	{
		return;
	}

	RENDER_STATE	eState = pState->GetRenderStateType();

	SAFE_RELEASE(m_pRenderState[eState]);
	m_pRenderState[eState] = pState;
}

void Renderer::Start()
{
	// Start()�������� ���۷���ī���Ͱ� �����ϴµ�  -> FindComponentFromType
	// �׷� �������� Start()�� ȣ���Ҷ����� ���۷���ī���Ͱ� ��� �����ϴϱ� .. ( ���ӿ�����Ʈ���� �̹� ���� �ִ°� �����Ϸ��� ȣ��� Add��)
	// Start()���� ��������Find()ȣ������ Release�� ���ش�.
	SAFE_RELEASE(m_pMaterial)
	m_pMaterial = FindComponentFromType<Material>(CT_MATERIAL);
}

bool Renderer::Init()
{
	// ��ŷ�� �ݶ��̴� ����
	ColliderSphere* m_pPickSphere = AddComponent<ColliderSphere>("PickSphere");
	m_pPickSphere->PickEnable(true);
	SAFE_RELEASE(m_pPickSphere);

	return true;
}

int Renderer::Input(float _fTime)
{
	return 0;
}

int Renderer::Update(float _fTime)
{
	return 0;
}

int Renderer::LateUpdate(float _fTime)
{
	return 0;
}

int Renderer::Collision(float _fTime)
{
	return 0;
}

int Renderer::PrevRender(float _fTime)
{
	Light* pLight = GET_SINGLETON(RenderManager)->GetFirstLight();

	if (nullptr != pLight)
	{
		pLight->SetShader();
		SAFE_RELEASE(pLight);
	}

	return 0;
}

int Renderer::PrevShadowRender(Matrix _matLightView, Matrix _matLightProj, float _fTime)
{
	return 0;
}

int Renderer::Render(float _fTime)
{
	LandScape* pLandScape = m_pGameObject->FindComponentFromType<LandScape>(CT_LANDSCAPE);

	if (nullptr != pLandScape)
	{
		tLimRenderGroup.bLimlight = 5;
	}
	else
	{
		tLimRenderGroup.bLimlight = 0;
	}

	SAFE_RELEASE(pLandScape);

	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("BLimLightCBuffer", &tLimRenderGroup);

	for (int i = 0; i < RS_END; ++i)
	{
		if (nullptr != m_pRenderState[i])
		{
			m_pRenderState[i]->SetState();
		}
	}

	_CONTEXT->IASetInputLayout(m_pInputLayout);
	m_pShader->SetShader();

	size_t iContainer = m_pMesh->GetContainerCount();

	for (size_t i = 0; i < iContainer; ++i)
	{
		size_t iSubSet = m_pMesh->GetSubSetCount((int)i);

		if (0 < iSubSet )
		{
			// ������� �ִ°��
			for (size_t j = 0; j < iSubSet; ++j)
			{
				if (nullptr != m_pMaterial)
				{
					m_pMaterial->SetShader((int)i, (int)j);
				}

				m_pMesh->Render((int)i, (int)j);
			}
		}
		else
		{
			// ������� ���� ���
			if (nullptr != m_pMaterial)
			{
				m_pMaterial->SetShader((int)i, 0);
			}
			m_pMesh->Render((int)i, 0);
		}
	}

	for (int i = 0; i < RS_END; ++i)
	{
		if (nullptr != m_pRenderState[i])
		{
			m_pRenderState[i]->ResetState();
		}
	}

	return 0;
}

int Renderer::RenderShadow(class Shader* _pShader, float _fTime)
{
	for (int i = 0; i < RS_END; ++i)
	{
		if (nullptr != m_pRenderState[i])
		{
			m_pRenderState[i]->SetState();
		}
	}

	_CONTEXT->IASetInputLayout(m_pInputLayout);
	_pShader->SetShader();

	size_t iContainer = m_pMesh->GetContainerCount();

	for (size_t i = 0; i < iContainer; ++i)
	{
		size_t iSubSet = m_pMesh->GetSubSetCount((int)i);

		if (0 < iSubSet)
		{
			// ������� �ִ°��
			for (size_t j = 0; j < iSubSet; ++j)
			{
				if (nullptr != m_pMaterial)
				{
					m_pMaterial->SetShader((int)i, (int)j);
				}

				m_pMesh->Render((int)i, (int)j);
			}
		}
		else
		{
			// ������� ���� ���
			if (nullptr != m_pMaterial)
			{
				m_pMaterial->SetShader((int)i, 0);
			}
			m_pMesh->Render((int)i, 0);
		}
	}

	for (int i = 0; i < RS_END; ++i)
	{
		if (nullptr != m_pRenderState[i])
		{
			m_pRenderState[i]->ResetState();
		}
	}
	return 0;
}

Renderer * Renderer::Clone() const
{
	return new Renderer(*this);
}

void Renderer::RenderInstancing(PInstancingBuffer _pBuffer, Shader * 
	_pShader, 
	ID3D11InputLayout * _pLayout, 
	int _iInstancingCount,
	float _fTime)
{
	_pShader->SetShader();
	_CONTEXT->IASetInputLayout(_pLayout);

	// ����������Ʈ�� ����
	for (int i = 0; i < RS_END; ++i)
	{
		if (nullptr != m_pRenderState[i])
		{
			m_pRenderState[i]->SetState();
		}
	}

	LandScape* pLandScape = m_pGameObject->FindComponentFromType<LandScape>(CT_LANDSCAPE);

	if (nullptr != pLandScape)
	{
		tLimRenderGroup.bLimlight = 5;
	}
	else
	{
		tLimRenderGroup.bLimlight = 0;
	}

	SAFE_RELEASE(pLandScape);
	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("BLimLightCBuffer", &tLimRenderGroup);

	size_t iContainer = m_pMesh->GetContainerCount();

	for (size_t i = 0; i < iContainer; ++i)
	{
		size_t iSubSet = m_pMesh->GetSubSetCount((int)i);

		if (0 < iSubSet)
		{
			// ������� �ִ°��
			for (size_t j = 0; j < iSubSet; ++j)
			{
				if (nullptr != m_pMaterial)
				{
					m_pMaterial->SetShader((int)i, (int)j);
				}

				m_pMesh->RenderInstancing((int)i, (int)j, _pBuffer,
					_iInstancingCount);
			}
		}
		else
		{
			// ������� ���� ���
			if (nullptr != m_pMaterial)
			{
				m_pMaterial->SetShader((int)i, 0);
			}
			m_pMesh->RenderInstancing((int)i, 0, _pBuffer,
				_iInstancingCount);
		}
	}

	for (int i = 0; i < RS_END; ++i)
	{
		if (nullptr != m_pRenderState[i])
		{
			m_pRenderState[i]->ResetState();
		}
	}

}

void Renderer::RenderInstancingShadow(PInstancingBuffer _pBuffer, Shader * _pShader, ID3D11InputLayout * _pLayout, int _iInstancingCount, float _fTime)
{
	_pShader->SetShader();
	_CONTEXT->IASetInputLayout(_pLayout);

	// ����������Ʈ�� ����
	for (int i = 0; i < RS_END; ++i)
	{
		if (nullptr != m_pRenderState[i])
		{
			m_pRenderState[i]->SetState();
		}
	}

	size_t iContainer = m_pMesh->GetContainerCount();

	for (size_t i = 0; i < iContainer; ++i)
	{
		size_t iSubSet = m_pMesh->GetSubSetCount((int)i);

		if (0 < iSubSet)
		{
			// ������� �ִ°��
			for (size_t j = 0; j < iSubSet; ++j)
			{
				if (nullptr != m_pMaterial)
				{
					m_pMaterial->SetShader((int)i, (int)j);
				}

				m_pMesh->RenderInstancing((int)i, (int)j, _pBuffer,
					_iInstancingCount);
			}
		}
		else
		{
			// ������� ���� ���
			if (nullptr != m_pMaterial)
			{
				m_pMaterial->SetShader((int)i, 0);
			}
			m_pMesh->RenderInstancing((int)i, 0, _pBuffer,
				_iInstancingCount);
		}
	}

	for (int i = 0; i < RS_END; ++i)
	{
		if (nullptr != m_pRenderState[i])
		{
			m_pRenderState[i]->ResetState();
		}
	}
}
