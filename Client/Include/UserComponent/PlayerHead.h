#pragma once
#include "Component\UserComponent.h"
#include "Component\Renderer.h"
#include "Component\Transform.h"
#include "Component\Material.h"
#include "Component\Animation.h"

ENGINE_USING

class PlayerHead : public UserComponent
{
private:
	// 얼굴
	Transform*	m_pFaceTransform;
	Renderer*	m_pFaceRenderer;
	Material*	m_pFaceMaterial;
	Animation*	m_pFaceAnimation;

public:
	PlayerHead();
	PlayerHead(const PlayerHead& _Face);
	~PlayerHead();

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출, 즉 레이어에서 호출해준다.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// 렌더를 하기 직전에 호출해주는 함수ㅔㄱㅁ
	virtual int Render(float _fTime);
	virtual PlayerHead* Clone()	const;

private:
	// Init 함수들
	void ComponentInit();

public:
	void AnimationChange(std::string _strName);

};

