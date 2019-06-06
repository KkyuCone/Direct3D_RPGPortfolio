#include "Player.h"
#include "Scene\Scene.h"
#include "Scene\Layer.h"
#include "Time.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Bullet.h"
#include "Component\ColliderSphere.h"
#include "Component\ColliderOBB.h"
#include "Component\Navigation.h"
#include "Navigation\NavigationManager.h"
#include "Navigation\NavigationMesh.h"
#include "Render\RenderManager.h"
#include "PlayerHead.h"
#include "Weapon.h"
#include "Component\Camera.h"
#include "Component\TPCamera.h"


void Player::Idle()
{
	AnimationChange("Wait");
	AnimationPivotMove();
}


void Player::Run()
{
	AnimationChange("AdvLeap");
	AnimationPivotMove();
}

void Player::Wait()
{
	AnimationChange("Wait");
	AnimationPivotMove();
}

void Player::Death()
{
	PANIMATIONCLIP CurClip = m_pBodyAnimation->GetCurrentClip();

	if ("Death" != CurClip->strName)
	{
		AnimationChange("Death");
	}

	if (true == m_pBodyAnimation->IsAnimationEnd())
	{
		AnimationPivotMove();
		m_ePlayerState = BaseState::DEATHWAIT;
		AnimationChange("DeathWait");
	}
}

void Player::DeathWait()
{
	return;
}

void Player::NockBack()
{
	// 넉백은 애니메이션이 3개로 이뤄짐
	AnimationChange("DeathWait");
}

void Player::Groggy()
{
	AnimationChange("Groggy1");
}

void Player::Tumbling()
{
	AnimationChange("Tumbling");

}

///////////////////////////////////////////////////////// 스킬
// 스킬들은 각 애니메이션 끝프레임에 도달 했을 경우
// 위치가 이동 되었을 경우 캐릭터를 이동 시킨 후에 Idle 상태로 바꾼다.

void Player::Skill01()
{
	// CuttingSlash

	// 끝프레임 -> Idle로 전환
	ChangeSkillToIdle();

}

void Player::Skill02()
{
	// StingerBlade
	ChangeSkillToIdle();

}

void Player::Skill03()
{
	// RisingAttack
	ChangeSkillToIdle();

}

void Player::Skill04()
{
	// WindCutter1R -> WindCutter1
	PANIMATIONCLIP pCurClip = m_pBodyAnimation->GetCurrentClip();

	if ("WindCutter1R" == pCurClip->strName)
	{
		if (true == m_pBodyAnimation->IsAnimationEnd())
		{
			AnimationChange("WindCutter1");
		}
	}
	else if ("WindCutter1" == pCurClip->strName)
	{
		ChangeSkillToIdle();
	}
}

void Player::Skill05()
{
	// WindCutter3
	ChangeSkillToIdle();

}

void Player::Combo1()
{
	// Combo1R -> Combo1 
	PANIMATIONCLIP pCurClip = m_pBodyAnimation->GetCurrentClip();

	if (BaseState::COMBO1R == m_ePlayerState)
	{
		if ("Combo1R" == pCurClip->strName)
		{
			if (true == m_pBodyAnimation->IsAnimationEnd())
			{
				m_ePlayerState = BaseState::COMBO1;
				AnimationChange("Combo1");
			}
		}
	}
	else if (BaseState::COMBO1 == m_ePlayerState)
	{
		// 공격할때마다 랜덤하던가 아니면 연속키 눌렀을때 판단해서 Combo2, 3로 갈지 판단 이건 전투 구현할때 다시 생각하기
		ChangeSkillToIdle();
	}
	else
	{
		AnimationChange("Combo1R");
	}

}

void Player::Combo2()
{
	// Combo2R -> Combo2
	AnimationChange("Combo2R");

}

void Player::Combo3()
{
	// Combo3R -> Combo3
	AnimationChange("Combo3R");

}

void Player::Combo4()
{
	// Combo4
	AnimationChange("Combo4");

}