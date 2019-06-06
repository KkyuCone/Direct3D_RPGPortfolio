#pragma once
#include "..\Reference.h"

ENGINE_BEGIN

class ENGINE_DLL RenderState : public Reference
{ 
	//Raterizer, DepthStencil, Blend 모두 DeviceChild를 상속 받는다.
	//( 다운캐스팅 해줄거임 ㅇㅅㅇ, 이거 하나로만 관리할거)
	//세개 모두 셋팅하는 방법이 조금씩 다르기 때문에 소멸자에 virtual을 추가하지 않고
	//셋팅하는 함수 부분에 virtual을 추가한다. + 필수로 셋팅해야하기 때문에 순수가상함수를 사용함
private:
	friend class RenderManager;

public:
	RenderState();
	~RenderState();

protected:
	ID3D11DeviceChild*	m_pCurState;			// 현재 렌더 상태
	ID3D11DeviceChild*	m_pOldState;			// 이전의 렌더 상태 ( 상태를 잠시 바꿔줄때 이전 상태를 알고 있어야 하기 때문에 추가해준거)
	RENDER_STATE		m_eType;

public:
	RENDER_STATE GetRenderStateType()	const;

public:
	virtual void SetState() = 0;		// 셋팅해줄 스테이트(바꿔줄스테이트)
	virtual void ResetState() = 0;		// 이전 스테이트로 돌아감

};

ENGINE_END

