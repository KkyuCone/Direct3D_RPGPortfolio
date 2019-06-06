#include "Collider.h"
#include "../CollisionManager.h"

#ifdef _DEBUG
#include "..\Render\Shader.h"
#include "..\Render\ShaderManager.h"
#include "..\Resource\Mesh.h"
#include "..\Render\RenderState.h"
#include "..\Render\RenderManager.h"
#include "..\Device.h"

#endif // _DEBUG


ENGINE_USING

// m_iSerialNumber�� �����ڿ��� UINT�� �ִ����� �����ϴ� ������ ���� �ʱⰪ�� �����ϴ� ����.
// �Ŀ� �ݶ��̴� ���� �ĺ� ��ȣ�� ������ �� ����.
// �갡 �ִ��� ���� �ĺ� ��ȣ�� �ο����� ���� ���̱� ������ �� ���¿��� �ݶ��̴��� ����ؼ� �ȵȴ�.
// �� ���� �ĺ� ��ȣ�� ����������Ѵ�. 
Collider::Collider() :
	//m_iSerialNumber(UINT_MAX),
	m_pPrevNumber(nullptr),
	m_pCollisionList(nullptr),
	m_bPick(false),
	m_bAddCollisionManager(true)
{
	m_eComponentType = CT_COLLIDER;			// ������Ʈ Ÿ���� �ݶ��̴���.(�浹ü�� ����)
	memset(m_bCallBack, 0, sizeof(bool) * CCBS_END);

#ifdef _DEBUG

	m_vColor = Vector4::Green;

	m_pShader = GET_SINGLETON(ShaderManager)->FindShader(SHADER_COLLIDER);
	m_pLayout = GET_SINGLETON(ShaderManager)->FindInputLayout(LAYOUT_POSITION);
	m_pMesh = nullptr;
	m_pWireFrame = GET_SINGLETON(RenderManager)->FindRenderState("WireFrame");
#endif // _DEBUG

	// WorldStatic
	m_pProfile = GET_SINGLETON(CollisionManager)->FindProfile("WorldStatic");		// �⺻��, ���� ������ �� ����
	m_iChannelIndex = 0;

}


Collider::Collider(const Collider & _Com) : Component(_Com)
{
	*this = _Com;
	m_iReferenceCount = 1;

	memset(m_bCallBack, 0, sizeof(bool) * CCBS_END);

#ifdef _DEBUG
	if (nullptr != m_pShader)
	{
		m_pShader->AddReference();
	}

	if (nullptr != m_pMesh)
	{
		m_pMesh->AddReference();
	}

	if (nullptr != m_pWireFrame)
	{
		m_pWireFrame->AddReference();
	}

#endif // _DEBUG


}

Collider::~Collider()
{
#ifdef _DEBUG
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_pMesh);
	SAFE_RELEASE(m_pWireFrame);
#endif // _DEBUG

	GET_SINGLETON(CollisionManager)->DeletePrevMouse(this);

	// ���� �浹 ��ϰ��� ���踦 �����ش�.  ( �� �ݶ��̴��� �������ϱ� !)
	ClearState();

	SAFE_DELETE_ARRAY(m_pCollisionList);
	SAFE_DELETE_ARRAY(m_pPrevNumber);

	// �ݶ��̴��� �����Ǹ�
	// �����ĺ���ȣ�� �ݳ�(?)�ϴ� �������ϴ� �Լ��� ȣ���Ѵ�. -> AddValidSerialNumber 
	// ( ������ �ݶ��̴����� �����ĺ���ȣ�� ��Ƶ�, �׸��� ���� �ݶ��̴��� �����ɶ� ���ŵ� �����ĺ���ȣ���� �ο�����. �̴¸޸� ���� ���̴� ������ ����Ѵ�.)
	if (m_iSerialNumber != UINT_MAX)
	{
		GET_SINGLETON(CollisionManager)->AddValidSerialNumber(m_iSerialNumber);
	}
}

//////////////////////////////////////////////////////

COLLIDER_TYPE Collider::GetColliderType() const
{
	return m_eCollType;
}

//unsigned int Collider::GetSerialNumber() const
//{
//	return m_iSerialNumber;
//}

Vector3 Collider::GetSectionMin() const
{
	return m_vSectionMin;
}

Vector3 Collider::GetSectionMax() const
{
	return m_vSectionMax;
}

_tagCollisionProfile * Collider::GetCollisionProfile() const
{
	return m_pProfile;
}

unsigned int Collider::GetCollisionChannelIndex() const
{
	return m_iChannelIndex;
}

bool Collider::GetPickEnable() const
{
	return m_bPick;
}

void Collider::SetSerialNumber(unsigned int _iSerialNumber)
{
	m_iSerialNumber = _iSerialNumber;
}

void Collider::SetChannel(const std::string & _strChannel)
{
	m_iChannelIndex = GET_SINGLETON(CollisionManager)->GetChannelIndex(_strChannel);
}

void Collider::SetProfile(const std::string & _strProfile)
{
	m_pProfile = GET_SINGLETON(CollisionManager)->FindProfile(_strProfile);
}

void Collider::PickEnable(bool _Value)
{
	m_bPick = _Value;
}

void Collider::CollisionManagerEnable(bool _Value)
{
	m_bAddCollisionManager = _Value;
}

void Collider::ClearState()
{
	// �̹� �������ų� ���¾ָ� return
	if (UINT_MAX == m_iSerialNumber)
	{
		return;
	}

	// �̹� �浹�� �ֵ� �����ֱ�
	for (unsigned int i = 0; i < m_iPrevSize; ++i)
	{
		GET_SINGLETON(CollisionManager)->DeletePrevCollider(m_iSerialNumber, m_pPrevNumber[i]);

		Collider* pDest = GET_SINGLETON(CollisionManager)->FindCollider(m_pPrevNumber[i]);

		if (nullptr != pDest)
		{
			pDest->DeletePrevSerialNumber(m_iSerialNumber);
		}

		m_pPrevNumber[i] = UINT_MAX;
	}

	m_iPrevSize = 0;
}

void Collider::AddPrevSerialNumber(unsigned int _iSerialNumber)
{
	if (m_iPrevSize == m_iPrevCapacity)
	{
		m_iPrevCapacity *= 2;
		unsigned int* pList = new unsigned int[m_iPrevCapacity];
		memcpy(pList, m_pPrevNumber, sizeof(unsigned int) * m_iPrevSize);

		SAFE_DELETE_ARRAY(m_pPrevNumber);		// ����� ���� ����pList�� �־��ش�.

		m_pPrevNumber = pList;
	}

	m_pPrevNumber[m_iPrevSize] = _iSerialNumber;
	++m_iPrevSize;
}

void Collider::DeletePrevSerialNumber(unsigned int _iSerialNumber)
{
	for (unsigned int i = 0; i < m_iPrevSize; ++i)
	{
		if (m_pPrevNumber[i] == _iSerialNumber)
		{
			--m_iPrevSize;
			for (unsigned int j = i; j < m_iPrevSize; ++j)
			{
				m_pPrevNumber[j] = m_pPrevNumber[j + 1];
			}

			m_pPrevNumber[m_iPrevSize] = UINT_MAX;
			break;
		}
	}
}

void Collider::AddCollisionList(unsigned int _iSerialNumber)
{
	if (m_iCollisionSize == m_iCollisionCapacity)
	{
		m_iCollisionCapacity *= 2;

		unsigned int* pList = new unsigned int[m_iCollisionCapacity];
		memcpy(pList, m_pCollisionList, sizeof(unsigned int) * m_iCollisionSize);

		SAFE_DELETE_ARRAY(m_pCollisionList);

		m_pCollisionList = pList;
	}

	m_pCollisionList[m_iCollisionSize] = _iSerialNumber;
	++m_iCollisionSize;
}

void Collider::DeleteCollisionList(unsigned int _ISerialNumber)
{
	for (unsigned int i = 0; i < m_iCollisionSize; ++i)
	{
		if (m_pCollisionList[i] == _ISerialNumber)
		{
			--m_iCollisionSize;
			for (unsigned int j = i; j < m_iCollisionSize; ++j)
			{
				m_pCollisionList[j] = m_pCollisionList[j + 1];
			}

			m_pCollisionList[m_iCollisionSize] = UINT_MAX;
			break;
		}
	}
}

bool Collider::CheckCollisionList(unsigned int _iSerialNumber)
{
	for (unsigned int i = 0; i < m_iCollisionSize; ++i)
	{
		if (m_pCollisionList[i] == _iSerialNumber)
			return true;
	}

	return false;
}

bool Collider::CheckCollisionList()
{
	return m_iCollisionSize != 0;			// ����Ʈ�� �Ѱ��� ������.. �ִ��� ����(�浹ü��)
}

void Collider::ClearCollisionList()
{
	m_iCollisionSize = 0;
}

void Collider::Call(COLLISION_CALLBACK_STATE _eState, Collider * pDest, float _fTime)
{
	if (true == m_bCallBack[_eState])
	{
		m_CallBackFunc[_eState](this, pDest, _fTime);
	}
}

void Collider::Start()
{
	// �ݶ��̴��� ������ ����
	// Start()�Լ����� (�ѹ��� �����ϴϱ�..)
	// CollisionManager���� ���� �ĺ� ��ȣ (SerialNumber)�� �ο� �޴´�.
	m_iSerialNumber = GET_SINGLETON(CollisionManager)->GetSerialNumber();		//  �ø��� �ѹ� �ޱ�

	// �־���Ҷ��� �ִ´�.
	if (true == m_bAddCollisionManager)
	{
		GET_SINGLETON(CollisionManager)->AddCollider(this);
	}

	m_iPrevSize = 0;
	m_iPrevCapacity = 10;
	m_pPrevNumber = new unsigned int[m_iPrevCapacity];

	m_iCollisionSize = 0;
	m_iCollisionCapacity = 10;
	m_pCollisionList = new unsigned int[m_iCollisionCapacity];
}

bool Collider::Init()
{
	return true;
}

int Collider::Input(float _fTime)
{
	return 0;
}

int Collider::Update(float _fTime)
{
	return 0;
}

int Collider::LateUpdate(float _fTime)
{
	return 0;
}

int Collider::Collision(float _fTime)
{
	return 0;
}

int Collider::PrevRender(float _fTime)
{
	return 0;
}


int Collider::Render(float _fTime)
{
#ifdef _DEBUG
	if (0 < m_iPrevSize)
	{
		// �浹�� ������ ������~
		m_vColor = Vector4::Red;
	}
	else
	{
		// �浹 �������� �ʷϻ�~
		m_vColor = Vector4::Green;
	}

	m_pWireFrame->SetState();

	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("Transform", &m_tCBuffer);
	GET_SINGLETON(ShaderManager)->UpdateConstBuffer("Collider", &m_vColor);

	m_pShader->SetShader();
	_CONTEXT->IASetInputLayout(m_pLayout);

	m_pMesh->Render();

	m_pWireFrame->ResetState();
#endif // _DEBUG

	m_iCollisionSize = 0;

	return 0;
}

Collider * Collider::Clone() const
{
	return nullptr;
}

bool Collider::Collision(Collider * _pDest)
{
	return true;
}

bool Collider::CollisionSphereToSphere(const SphereInfo & _tSrc, const SphereInfo & _tDest)
{
	float	fDist = _tSrc.vCenter.Distance(_tDest.vCenter);
	return fDist <= _tSrc.fRadius + _tDest.fRadius;
}

bool Collider::CollisionSphereToRay(const SphereInfo & _tSrc, const RayInfo & _tDest)
{
	Vector3 vM =  _tDest.vOrigin - _tSrc.vCenter;
	
	float B = 2.0f * vM.Dot(_tDest.vDir);
	float C = vM.Dot(vM) - (_tSrc.fRadius * _tSrc.fRadius);

	// �Ǻ����� ���� �浹 ���� �Ǵ�
	float fDiscriminant = B * B - 4.0f * C;

	// �Ǻ����� ������ ��� �浹 X 
	if (fDiscriminant < 0)
	{
		return false;
	}

	fDiscriminant = sqrtf(fDiscriminant);

	float T0, T1;
	T0 = (-B + fDiscriminant) / 2.0f;
	T1 = (-B - fDiscriminant) / 2.0f;

	// ������ ��� �浹 X, ������ �� �ۿ� ������.
	if (T0 < 0 && T1 < 0)
	{
		return false;
	}

	return true;
}

bool Collider::CollisionOBBToOBB(const OBBInfo & _tSrc, const OBBInfo & _tDest)
{
	// OBB -> �������� � �࿡ �����ؼ� �и��� �Ǿ����� �ľ��ؼ� �浹 ���θ� �˾Ƴ� ����
	// 15�� ������ �˻�

	Vector3 vDir = _tDest.vCenter - _tSrc.vCenter;		// ���� ���͸� ���ϰ�..

	float fAxisDot[3][3] = {};			// �� 3���� �࿡ ���� ������
	float fAbsAxisDot[3][3] = {};		// ���� ���������� ���밪���� ǥ�� ( ������ �ʿ���� ) 
	float fDirDot[3] = {};
	float r, r1, r2;					// _tSrc�� _tDest�� ������ �Ÿ� ( �и��� ������ �ִ��� Ȯ���� �߿��� �ֵ� )
	bool bAxis = false;					// �и� ���� �ִ����� ����,  �и� ���� ����� �� OBB�� �浹 X    -> true = �浹 X, false = �浹 O

	for (int i = 0; i < 3; ++i)
	{
		fAxisDot[AXIS_X][i] = _tSrc.vAxis[AXIS_X].Dot(_tDest.vAxis[i]);			// _Src�� X�࿡ ���� _Dest�� X,Y,Z�� ���������� �־��ش�.
		fAbsAxisDot[AXIS_X][i] = abs(fAxisDot[AXIS_X][i]);						// ���� ���������� ���밪���� �ٲ��ֱ�.. (�����ʿ� X)

		if (fAbsAxisDot[AXIS_X][i] > 0.99999f)
			bAxis = true;
	}

	// Center Dir�� Src�� X�࿡ �����Ѵ�. ( ������ ���� ���̸� ���� )
	fDirDot[0] = vDir.Dot(_tSrc.vAxis[AXIS_X]);
	r = abs(fDirDot[0]);
	r1 = _tSrc.vHalfLength.x;
	r2 = _tDest.vHalfLength.x * fAbsAxisDot[AXIS_X][AXIS_X] +
		_tDest.vHalfLength.y * fAbsAxisDot[AXIS_X][AXIS_Y] +
		_tDest.vHalfLength.z * fAbsAxisDot[AXIS_X][AXIS_Z];

	if (r > r1 + r2)
		return false;

	for (int i = 0; i < 3; ++i)
	{
		fAxisDot[AXIS_Y][i] = _tSrc.vAxis[AXIS_Y].Dot(_tDest.vAxis[i]);
		fAbsAxisDot[AXIS_Y][i] = abs(fAxisDot[AXIS_Y][i]);

		if (fAbsAxisDot[AXIS_Y][i] > 0.99999f)
			bAxis = true;
	}

	// Center Dir�� Src�� Y�࿡ �����Ѵ�.
	fDirDot[1] = vDir.Dot(_tSrc.vAxis[AXIS_Y]);
	r = abs(fDirDot[1]);
	r1 = _tSrc.vHalfLength.y;
	r2 = _tDest.vHalfLength.x * fAbsAxisDot[AXIS_Y][AXIS_X] +
		_tDest.vHalfLength.y * fAbsAxisDot[AXIS_Y][AXIS_Y] +
		_tDest.vHalfLength.z * fAbsAxisDot[AXIS_Y][AXIS_Z];

	if (r > r1 + r2)
		return false;

	for (int i = 0; i < 3; ++i)
	{
		fAxisDot[AXIS_Z][i] = _tSrc.vAxis[AXIS_Z].Dot(_tDest.vAxis[i]);
		fAbsAxisDot[AXIS_Z][i] = abs(fAxisDot[AXIS_Z][i]);

		if (fAbsAxisDot[AXIS_Z][i] > 0.99999f)
			bAxis = true;
	}

	// Center Dir�� Src�� Z�࿡ �����Ѵ�.
	fDirDot[2] = vDir.Dot(_tSrc.vAxis[AXIS_Z]);
	r = abs(fDirDot[2]);
	r1 = _tSrc.vHalfLength.z;
	r2 = _tDest.vHalfLength.x * fAbsAxisDot[AXIS_Z][AXIS_X] +
		_tDest.vHalfLength.y * fAbsAxisDot[AXIS_Z][AXIS_Y] +
		_tDest.vHalfLength.z * fAbsAxisDot[AXIS_Z][AXIS_Z];

	if (r > r1 + r2)
		return false;

	// Dest�� X�࿡ Center Dir�� �����Ѵ�.
	r = abs(vDir.Dot(_tDest.vAxis[AXIS_X]));
	r1 = _tSrc.vHalfLength.x * fAbsAxisDot[AXIS_X][AXIS_X] +
		_tSrc.vHalfLength.y * fAbsAxisDot[AXIS_Y][AXIS_X] +
		_tSrc.vHalfLength.z * fAbsAxisDot[AXIS_Z][AXIS_X];
	r2 = _tDest.vHalfLength.x;

	if (r > r1 + r2)
		return false;

	// Dest�� Y�࿡ Center Dir�� �����Ѵ�.
	r = abs(vDir.Dot(_tDest.vAxis[AXIS_Y]));
	r1 = _tSrc.vHalfLength.x * fAbsAxisDot[AXIS_X][AXIS_Y] +
		_tSrc.vHalfLength.y * fAbsAxisDot[AXIS_Y][AXIS_Y] +
		_tSrc.vHalfLength.z * fAbsAxisDot[AXIS_Z][AXIS_Y];
	r2 = _tDest.vHalfLength.y;

	if (r > r1 + r2)
		return false;

	// Dest�� Z�࿡ Center Dir�� �����Ѵ�.
	r = abs(vDir.Dot(_tDest.vAxis[AXIS_Z]));
	r1 = _tSrc.vHalfLength.x * fAbsAxisDot[AXIS_X][AXIS_Z] +
		_tSrc.vHalfLength.y * fAbsAxisDot[AXIS_Y][AXIS_Z] +
		_tSrc.vHalfLength.z * fAbsAxisDot[AXIS_Z][AXIS_Z];
	r2 = _tDest.vHalfLength.z;

	if (r > r1 + r2)
		return false;

	if (bAxis)
		return true;

	r = abs(fDirDot[AXIS_Z] * fAxisDot[AXIS_Y][AXIS_X] -
		fDirDot[AXIS_Y] * fAxisDot[AXIS_Z][AXIS_X]);
	r1 = _tSrc.vHalfLength.y * fAbsAxisDot[AXIS_Z][AXIS_X] +
		_tSrc.vHalfLength.z * fAbsAxisDot[AXIS_Y][AXIS_X];
	r2 = _tDest.vHalfLength.y * fAbsAxisDot[AXIS_X][AXIS_Z] +
		_tDest.vHalfLength.z * fAbsAxisDot[AXIS_X][AXIS_Y];

	if (r > r1 + r2)
		return false;

	r = abs(fDirDot[AXIS_Z] * fAxisDot[AXIS_Y][AXIS_Y] -
		fDirDot[AXIS_Y] * fAxisDot[AXIS_Z][AXIS_Y]);
	r1 = _tSrc.vHalfLength.y * fAbsAxisDot[AXIS_Z][AXIS_Y] +
		_tSrc.vHalfLength.z * fAbsAxisDot[AXIS_Y][AXIS_Y];
	r2 = _tDest.vHalfLength.x * fAbsAxisDot[AXIS_X][AXIS_Z] +
		_tDest.vHalfLength.z * fAbsAxisDot[AXIS_X][AXIS_X];

	if (r > r1 + r2)
		return false;

	r = abs(fDirDot[AXIS_Z] * fAxisDot[AXIS_Y][AXIS_Z] -
		fDirDot[AXIS_Y] * fAxisDot[AXIS_Z][AXIS_Z]);
	r1 = _tSrc.vHalfLength.y * fAbsAxisDot[AXIS_Z][AXIS_Z] +
		_tSrc.vHalfLength.z * fAbsAxisDot[AXIS_Y][AXIS_Z];
	r2 = _tDest.vHalfLength.x * fAbsAxisDot[AXIS_X][AXIS_Y] +
		_tDest.vHalfLength.y * fAbsAxisDot[AXIS_X][AXIS_X];

	if (r > r1 + r2)
		return false;

	r = abs(fDirDot[AXIS_X] * fAxisDot[AXIS_Z][AXIS_X] -
		fDirDot[AXIS_Y] * fAxisDot[AXIS_Z][AXIS_Z]);
	r1 = _tSrc.vHalfLength.x * fAbsAxisDot[AXIS_Z][AXIS_X] +
		_tSrc.vHalfLength.z * fAbsAxisDot[AXIS_X][AXIS_X];
	r2 = _tDest.vHalfLength.y * fAbsAxisDot[AXIS_Y][AXIS_Z] +
		_tDest.vHalfLength.z * fAbsAxisDot[AXIS_Y][AXIS_Y];

	if (r > r1 + r2)
		return false;

	r = abs(fDirDot[AXIS_X] * fAxisDot[AXIS_Z][AXIS_Y] -
		fDirDot[AXIS_Z] * fAxisDot[AXIS_X][AXIS_Y]);
	r1 = _tSrc.vHalfLength.x * fAbsAxisDot[AXIS_Z][AXIS_Y] +
		_tSrc.vHalfLength.z * fAbsAxisDot[AXIS_X][AXIS_Y];
	r2 = _tDest.vHalfLength.x * fAbsAxisDot[AXIS_Y][AXIS_Z] +
		_tDest.vHalfLength.z * fAbsAxisDot[AXIS_Y][AXIS_X];

	if (r > r1 + r2)
		return false;

	r = abs(fDirDot[AXIS_X] * fAxisDot[AXIS_Z][AXIS_Z] -
		fDirDot[AXIS_Z] * fAxisDot[AXIS_X][AXIS_Z]);
	r1 = _tSrc.vHalfLength.x * fAbsAxisDot[AXIS_Z][AXIS_Z] +
		_tSrc.vHalfLength.z * fAbsAxisDot[AXIS_X][AXIS_Z];
	r2 = _tDest.vHalfLength.x * fAbsAxisDot[AXIS_Y][AXIS_Y] +
		_tDest.vHalfLength.y * fAbsAxisDot[AXIS_Y][AXIS_X];

	if (r > r1 + r2)
		return false;

	r = abs(fDirDot[AXIS_Y] * fAxisDot[AXIS_X][AXIS_X] -
		fDirDot[AXIS_X] * fAxisDot[AXIS_Y][AXIS_X]);
	r1 = _tSrc.vHalfLength.x * fAbsAxisDot[AXIS_Y][AXIS_X] +
		_tSrc.vHalfLength.y * fAbsAxisDot[AXIS_X][AXIS_X];
	r2 = _tDest.vHalfLength.y * fAbsAxisDot[AXIS_Z][AXIS_Z] +
		_tDest.vHalfLength.z * fAbsAxisDot[AXIS_Z][AXIS_Y];

	if (r > r1 + r2)
		return false;

	r = abs(fDirDot[AXIS_Y] * fAxisDot[AXIS_X][AXIS_Y] -
		fDirDot[AXIS_X] * fAxisDot[AXIS_Y][AXIS_Y]);
	r1 = _tSrc.vHalfLength.x * fAbsAxisDot[AXIS_Y][AXIS_Y] +
		_tSrc.vHalfLength.y * fAbsAxisDot[AXIS_X][AXIS_Y];
	r2 = _tDest.vHalfLength.x * fAbsAxisDot[AXIS_Z][AXIS_Z] +
		_tDest.vHalfLength.z * fAbsAxisDot[AXIS_Z][AXIS_X];

	if (r > r1 + r2)
		return false;

	r = abs(fDirDot[AXIS_Y] * fAxisDot[AXIS_X][AXIS_Z] -
		fDirDot[AXIS_X] * fAxisDot[AXIS_Y][AXIS_Z]);
	r1 = _tSrc.vHalfLength.x * fAbsAxisDot[AXIS_Y][AXIS_Z] +
		_tSrc.vHalfLength.y * fAbsAxisDot[AXIS_X][AXIS_Z];
	r2 = _tDest.vHalfLength.x * fAbsAxisDot[AXIS_Z][AXIS_Y] +
		_tDest.vHalfLength.y * fAbsAxisDot[AXIS_Z][AXIS_X];

	if (r > r1 + r2)
		return false;

	return true;				// ��� ��쿡 �ش���� ���� ��� ( �и����� ���� ���) �浹 O
}

bool Collider::CollisionRayToOBB(const RayInfo & _tSrc, const OBBInfo & _tDest)
{
	Vector3 vDir = _tDest.vCenter - _tSrc.vOrigin;
	Vector3 RayOrigin = _tSrc.vOrigin;

	float tMin = 0.0f;		
	float tMax = 100000.0f;


	Vector3 vSrcCenter =
	{	_tDest.vAxis[AXIS_X].Dot(vDir),
		_tDest.vAxis[AXIS_Y].Dot(vDir),
		_tDest.vAxis[AXIS_Z].Dot(vDir) 
	};

	Vector3 vDestCenter =
	{
		_tDest.vAxis[AXIS_X].Dot(_tSrc.vDir),
		_tDest.vAxis[AXIS_Y].Dot(_tSrc.vDir),
		_tDest.vAxis[AXIS_Z].Dot(_tSrc.vDir)
	};

	// x ��
	float AxisDotDir = vSrcCenter.x;
	float AxisDotRay = vDestCenter.x;

	// Beware, don't do the division if f is near 0 ! See full source code for details.
	float t1 = (AxisDotDir - _tDest.vHalfLength.x) / AxisDotRay;	// Intersection with the "left" plane
	float t2 = (AxisDotDir + _tDest.vHalfLength.x) / AxisDotRay;	// Intersection with the "right" plane


	if (t1 > t2)
	{
		float w = t1;
		t1 = t2;
		t2 = w;
	}

	if (t2 < tMax)
	{
		tMax = t2;
	}

	if (t1 > tMin)
	{
		tMin = t1;
	}

	if (tMax < tMin)
	{
		return false;
	}

	// y ��
	AxisDotDir = vSrcCenter.y;
	AxisDotRay = vDestCenter.y;

	t1 = (AxisDotDir - _tDest.vHalfLength.y) / AxisDotRay;
	t2 = (AxisDotDir + _tDest.vHalfLength.y) / AxisDotRay;

	if (t1 > t2)
	{
		float w = t1;
		t1 = t2;
		t2 = w;
	}

	if (t2 < tMax)
	{
		tMax = t2;
	}

	if (t1 > tMin)
	{
		tMin = t1;
	}

	if (tMax < tMin)
	{
		return false;
	}

	// z��

	AxisDotDir = vSrcCenter.z;
	AxisDotRay = vDestCenter.z;

	t1 = (AxisDotDir - _tDest.vHalfLength.z) / AxisDotRay;
	t2 = (AxisDotDir + _tDest.vHalfLength.z) / AxisDotRay;

	if (t1 > t2)
	{
		float w = t1;
		t1 = t2;
		t2 = w;
	}

	if (t2 < tMax)
	{
		tMax = t2;
	}

	if (t1 > tMin)
	{
		tMin = t1;
	}

	if (tMax < tMin)
	{
		return false;
	}

	return true;
}

bool Collider::CollisionSphereToOBB(const SphereInfo & _tSrc, const OBBInfo & _tDest)
{
	// ���� ����� ��(OBB ��������)
	Vector3 ClosePoint = {};

	Vector3 vDir = _tDest.vCenter - _tSrc.vCenter;

	Vector3 vSrcCenter = 
	{	_tDest.vAxis[AXIS_X].Dot(vDir),
		_tDest.vAxis[AXIS_Y].Dot(vDir), 
		_tDest.vAxis[AXIS_Z].Dot(vDir) };

	if (vSrcCenter.x < -_tDest.vHalfLength.x)
	{
		ClosePoint.x = -_tDest.vHalfLength.x;
	}
	else if (vSrcCenter.x > _tDest.vHalfLength.x)
	{
		ClosePoint.x = +_tDest.vHalfLength.x;
	}
	else
	{
		ClosePoint.x = vSrcCenter.x;
	}



	if (vSrcCenter.y < -_tDest.vHalfLength.y)
	{
		ClosePoint.y = -_tDest.vHalfLength.y;
	}
	else if (vSrcCenter.y > _tDest.vHalfLength.y)
	{
		ClosePoint.y = _tDest.vHalfLength.y;
	}
	else
	{
		ClosePoint.y = vSrcCenter.y;
	}


	if (vSrcCenter.z < -_tDest.vHalfLength.z)
	{
		ClosePoint.z = -_tDest.vHalfLength.z;
	}
	else if (vSrcCenter.z > _tDest.vHalfLength.z)
	{
		ClosePoint.z = _tDest.vHalfLength.z;
	}
	else
	{
		ClosePoint.z = vSrcCenter.z;
	}

	// ����� ���� ���� �߽��� �Ÿ�
	//float fDistance = (vSrcCenter - ClosePoint).Length();
	float fDistance = vSrcCenter.Distance(ClosePoint);

	if (fDistance > _tSrc.fRadius)
	{
		return false;
	}
	
	return true;

}

void Collider::SetCallback(COLLISION_CALLBACK_STATE _eState, void(*_pFunc)(Collider *, Collider *, float))
{
	m_bCallBack[_eState] = true;
	m_CallBackFunc[_eState] = bind(_pFunc, std::placeholders::_1,
		std::placeholders::_2, std::placeholders::_3);
}
