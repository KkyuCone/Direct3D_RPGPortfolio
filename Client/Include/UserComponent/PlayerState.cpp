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


void Player::Idle(float _fTime)
{
	AnimationChange("Wait");
	AnimationPivotMove();
}


void Player::Run(float _fTime)
{
	AnimationChange("AdvLeap");
	AnimationPivotMove();
}

void Player::Wait(float _fTime)
{
	AnimationChange("Wait");
	AnimationPivotMove();
}

void Player::Death(float _fTime)
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

void Player::DeathWait(float _fTime)
{
	return;
}

void Player::NockBack(float _fTime)
{
	// �˹��� �ִϸ��̼��� 3���� �̷���
	AnimationChange("DeathWait");
}

void Player::Groggy(float _fTime)
{
	AnimationChange("Groggy1");
}

void Player::Tumbling(float _fTime)
{
	AnimationChange("Tumbling");

}

///////////////////////////////////////////////////////// ��ų
// ��ų���� �� �ִϸ��̼� �������ӿ� ���� ���� ���
// ��ġ�� �̵� �Ǿ��� ��� ĳ���͸� �̵� ��Ų �Ŀ� Idle ���·� �ٲ۴�.

void Player::Skill01(float _fTime)
{
	// CuttingSlash
	// �������� -> Idle�� ��ȯ
	ChangeSkillToIdle();

}

void Player::Skill02(float _fTime)
{
	// StingerBlade
	ChangeSkillToIdle();

}

void Player::Skill03(float _fTime)
{
	// RisingAttack
	ChangeSkillToIdle();

}

void Player::Skill04(float _fTime)
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

void Player::Skill05(float _fTime)
{
	// WindCutter3
	ChangeSkillToIdle();

}

void Player::Combo1(float _fTime)
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
		// �����Ҷ����� �����ϴ��� �ƴϸ� ����Ű �������� �Ǵ��ؼ� Combo2, 3�� ���� �Ǵ� �̰� ���� �����Ҷ� �ٽ� �����ϱ�
		ChangeSkillToIdle();
	}
	else
	{
		AnimationChange("Combo1R");
	}

}

void Player::Combo2(float _fTime)
{
	// Combo2R -> Combo2
	AnimationChange("Combo2R");

}

void Player::Combo3(float _fTime)
{
	// Combo3R -> Combo3
	AnimationChange("Combo3R");

}

void Player::Combo4(float _fTime)
{
	// Combo4
	AnimationChange("Combo4");

}