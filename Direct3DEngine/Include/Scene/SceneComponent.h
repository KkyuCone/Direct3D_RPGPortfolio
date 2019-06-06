#pragma once
#include "../Reference.h"

ENGINE_BEGIN

// 가상 클래스 -> 객체로 안만들어진다.
// 자신을 업데이트하는 씬정보를 갖고 있다. 
// (AddComponent로 추가)
class ENGINE_DLL SceneComponent : public Reference
{
private:
	friend class Scene;


protected:
	SceneComponent();
	virtual ~SceneComponent() = 0;

protected:
	class Scene*	m_pScene;			// 컴포넌트들은 자신이 어떤 씬에 있는지 알고있음

public:
	virtual bool Init() = 0;
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int Render(float _fTime);
};

ENGINE_END