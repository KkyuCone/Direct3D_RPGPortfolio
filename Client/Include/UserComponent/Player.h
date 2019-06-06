#pragma once
#include "Component\UserComponent.h"
#include "Component\Renderer.h"
#include "Component\Transform.h"
#include "Component\Material.h"
#include "Component\Animation.h"
#include "Component\Collider.h"
#include "Component\ColliderRay.h"
#include "Base.h"

ENGINE_USING

class Player : public Base
{
private:
	BaseState		m_ePlayerState;					// 애니메이션 등 상태 관련
	BaseUIState		m_tPlayerUIState;				// HP, MP, EXP 관련
	float			m_fSpeed;

private:
	// 스킬 쿨타임 관련
	bool			m_bComputeCoolTime[5];
	float			m_fArrSkillCollTime[5];
	float			m_fArrCurCoolTime[5];
	BaseState		m_iBeforeSkill;					// 스킬 쿨타임 - 이전 스킬 체크용
	BaseState		m_iCurSKillNumber;				// 스킬 쿨타임 - 현재 스킬 체크용


private:
	// 몸
	Transform* m_pBodyTransform;
	Renderer* m_pBodyRenderer;
	Material* m_pBodyMaterial;
	Animation*	m_pBodyAnimation;
	ColliderRay* m_pMouseRay;				// 마우스광선, 이동할때 필요

public:
	void ReNavLateUpdate(float _fTime);


private:
	class GameObject*	m_pHeadObject;
	class GameObject*	m_pWeaponObject;

	class PlayerHead*	m_pHeadScript;
	class Weapon*		m_pWeaponScript;

	// 이동하는 애니메이션일 때 필요한 자식 오브젝트
	class GameObject*			m_pMoveAniChildObj;
	class PlayerCameraPivot*	m_pMoveAniChildScript;
	Transform*					m_pMoveAniChildTR;

	bool m_bMoveAni;
	Vector3 m_vMoveAniChild;

public:
	bool GetMoveAni();


public:
	Player();
	Player(const Player& _Player);
	~Player();

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출, 즉 레이어에서 호출해준다.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// 렌더를 하기 직전에 호출해주는 함수ㅔㄱㅁ
	virtual int Render(float _fTime);
	virtual Player* Clone()	const;

public:
	void SetHeadObject(class GameObject* _Object);
	void SetWeaponObject(class GameObject* _Object);

	void SetHeadScript(class PlayerHead* _Script);
	void SetWeaponScript(class Weapon* _Script);

	void SetMoveAniChildObject(class GameObject* _Object);
	void SetMoveAniChildScript(class PlayerCameraPivot* _Script);

public:
	BaseState GetCurrentState();

private:
	// Init 함수들
	void InputInit();
	void ComponentInit();

public:
	void MoveFront(float _fTime);
	void MoveBack(float _fTime);
	void MoveNone(float _fTime);
	void MoveRight(float _fScale, float _fTime);
	void Rotate(float _fScale, float _fTime);
	void Fire(float _fTime);
	void Test(float _fTime);

public:
	// 오직 캐릭터 관련 함수 (이동, 상태 등)
	void AnimationChange(std::string _strName);

public:
	void HDR_ON(float _fScale, float _fTime);
	void HDR_OFF(float _fScale, float _fTime);

	void SSAO_ON(float _fScale, float _fTime);
	void SSAO_OFF(float _fScale, float _fTime);

	void DepthFog_ON(float _fScale, float _fTime);
	void DepthFog_OFF(float _fScale, float _fTime);

	void Rain_ON(float _fScale, float _fTime);
	void Rain_OFF(float _fScale, float _fTime);


private:
	void TestHitEnter(Collider* _pSrc, Collider* _pDest, float _fTime);
	void TestHitStay(Collider* _pSrc, Collider* _pDest, float _fTime);
	void TestHitExit(Collider* _pSrc, Collider* _pDest, float _fTime);

	void HitPick(Collider* _pSrc, Collider* _pDest, float _fTime);
	void HitPickLeave(Collider* _pSrc, Collider* _pDest, float _fTime);

private:
	// 상태 바꿀 함수
	void StateChangeIdle(float _fTime);
	void StateChangeRun(float _fTime);
	void StateChangeWait(float _fTime);
	void StateChangeDeath(float _fTime);
	void StateChangeDeathWait(float _fTime);
	void StateChangeNockBack(float _fTime);
	void StateChangeGroggy(float _fTime);
	void StateChangeTumbling(float _fTime);

	void StateChangeSkill01(float _fTime);
	void StateChangeSkill02(float _fTime);
	void StateChangeSkill03(float _fTime);
	void StateChangeSkill04(float _fTime);
	void StateChangeSkill05(float _fTime);

	void StateChangeCombo1(float _fTime);
	void StateChangeCombo2(float _fTime);
	void StateChangeCombo3(float _fTime);
	void StateChangeCombo4(float _fTime);

private:
	bool ChangeableSkill(float _fTime);				// 스킬 상태를 바꿀수 있는지 여부 판단
	bool ComputeSkillCoolTime(float _fTime);		// 스킬 쿨타임 계산
	void ChangeSkillToIdle();

	bool CheckChangeSkill(BaseState _State);
	bool UseSkill();

private:
	// 상태에 따른 함수
	void Idle();
	void Run();
	void Wait();
	void Death();
	void DeathWait();
	void NockBack();
	void Groggy();
	void Tumbling();

	void Skill01();
	void Skill02();
	void Skill03();
	void Skill04();
	void Skill05();

	void Combo1();
	void Combo2();
	void Combo3();
	void Combo4();

	// 피봇 변화에 따른 3인칭 카메라 위치 이동 (마지막 프레임일때 이동 )
	void AnimationPivotMove();

	public:
	// 테스트용함수
	void DecHP(float _fTime);
};

