#pragma once
#include "Component.h"

ENGINE_BEGIN

class ENGINE_DLL Collider :
	public Component
{
private:
	friend class GameObject;

protected:
	Collider();
	Collider(const Collider& _Com);
	virtual ~Collider() = 0;

	// 디버그 모드일때는 화면에 모형을 출력해주기 위해 메쉬, 셰이더, 레이아웃을 넣어준다.
#ifdef _DEBUG
protected:
	class Mesh*				m_pMesh;			// 디버깅용 메쉬
	class Shader*			m_pShader;			// 셰이더
	ID3D11InputLayout*		m_pLayout;			// 레이아웃
	class RenderState*		m_pWireFrame;		// 면이아닌 겉 테두리만 출력하기 위해서 RenderState필요
	Vector4					m_vColor;			// 띄울 테두리 색상
	TransformConstBuffer	m_tCBuffer;			// 상수버퍼
#endif // _DEBUG

protected:
	// 고유 식별번호는 CollisionManager class에서 관리한다.
	// CollisionManager에 해당 콜라이더 목록들을 전부 넣어줌
	// 게임오브젝트에 Collider 생성시 고유 식별 번호를 부여한다.
	COLLIDER_TYPE		m_eCollType;			// 콜라이더 타입 (모형말함)
	//unsigned int		m_iSerialNumber;		// 해당 콜라이더의 고유 식별 번호

	Vector3				m_vSectionMin;			// 자신이 속한 분할된 공간의(공간들중에 1개) 최솟값을 알아내기 위한 자신만의 공간의 최솟값
	Vector3				m_vSectionMax;			// 자신이 속한 공간의 최댓값  ( 후에 자신이 분할공간의 어디에 속할것인지 알기 위한 자신만의 공간의 최댓값)
	struct _tagCollisionProfile*	m_pProfile;	// 자신이 속한 공간의 프로파일(각 공간들은 프로파일이 한개씩 존재하는데 프로파일은 채널 정보를 갖고 있음)
	unsigned int		m_iChannelIndex;		// 채널 인덱스

	unsigned int*		m_pPrevNumber;			// 충돌된 콜라이더의 모음(식별번호들의 모음)
	unsigned int		m_iPrevSize;
	unsigned int		m_iPrevCapacity;

	unsigned int*		m_pCollisionList;		// 충돌된 콜라이더 모음 ( 콜라이더 그 자체를 모아둠 )
	unsigned int		m_iCollisionSize;
	unsigned int		m_iCollisionCapacity;

	std::function<void(Collider*, Collider*, float)>		m_CallBackFunc[CCBS_END];		// 콜백 함수 넣을거 ㅇㅅㅇ
	bool			m_bCallBack[CCBS_END];				// 콜백 상태
	bool			m_bPick;							// 피킹용 콜라이더 여부
	bool			m_bAddCollisionManager;				// 콜리전매니저에 넣을건지 여부( 공간에 충돌체 한개만 존재할경우 충돌함수를 호출하지 X )


public:
	COLLIDER_TYPE	GetColliderType()		const;
	//unsigned int	GetSerialNumber()		const;			// CollisionManager에서 고유식별번호 받아오기

	Vector3 GetSectionMin()	const;
	Vector3 GetSectionMax()	const;

	struct _tagCollisionProfile* GetCollisionProfile()	const;
	unsigned int GetCollisionChannelIndex()	const;

	bool GetPickEnable()	const;

public:
	void SetSerialNumber(unsigned int _iSerialNumber);		// 콜라이더들은 고유식별번호를 갖는데 이걸 Set해주는 함수
	void SetChannel(const std::string& _strChannel);
	void SetProfile(const std::string& _strProfile);
	void PickEnable(bool _Value);
	void CollisionManagerEnable(bool _Value);

public:
	void ClearState();

	void AddPrevSerialNumber(unsigned int _iSerialNumber);			// 충돌한 고유식별번호를 추가하기
	void DeletePrevSerialNumber(unsigned int _iSerialNumber);		// 충돌한 고유식별번호 삭제

	void AddCollisionList(unsigned int _iSerialNumber);
	void DeleteCollisionList(unsigned int _ISerialNumber);
	bool CheckCollisionList(unsigned int _iSerialNumber);
	bool CheckCollisionList();
	void ClearCollisionList();

	void Call(COLLISION_CALLBACK_STATE _eState, Collider* pDest, float _fTime);			// 콜백함수

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출, 즉 레이어에서 호출해준다.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// 렌더를 하기 직전에 호출해주는 함수
	virtual int Render(float _fTime);
	virtual Collider* Clone()	const;
	virtual bool Collision(Collider* _pDest) = 0;

public:
	// 충돌 함수 
	bool CollisionSphereToSphere(const SphereInfo& _tSrc, const SphereInfo& _tDest);		// Sphere to Sphere
	bool CollisionSphereToRay(const SphereInfo& _tSrc, const RayInfo& _tDest);				// Sphere to Ray
	bool CollisionSphereToOBB(const SphereInfo& _tSrc, const OBBInfo& _tDest);				// Sphere to OBB
	bool CollisionOBBToOBB(const OBBInfo& _tSrc, const OBBInfo& _tDest);					// OBB to OBB
	bool CollisionRayToOBB(const RayInfo& _tSrc, const OBBInfo& _tDest);					// Ray to OBB
public:
	// 콜백함수 셋팅해주기 ( 충돌체 상태에 따라.. Enter, Stay, Exit )
	void SetCallback(COLLISION_CALLBACK_STATE _eState,
		void(*pFunc)(Collider*, Collider*, float));

	template <typename T>
	void SetCallback(COLLISION_CALLBACK_STATE _eState,
		T* _pObject, void(T::*pFunc)(Collider*, Collider*, float))
	{
		m_bCallBack[_eState] = true;
		m_CallBackFunc[_eState] = bind(pFunc, _pObject, std::placeholders::_1, 
			std::placeholders::_2, std::placeholders::_3);
	}
};

ENGINE_END
