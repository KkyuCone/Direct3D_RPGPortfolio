#pragma once
#include "../Reference.h"

ENGINE_BEGIN

// 실질적으로 오브젝트(또는 객체)가 배치되는곳은 Layer라고 보면 된다. ㅇㅅㅇ
// 레이어는 어떤 순서로 업데이트할 것인지가 중요하다.
// 여기서 순서는 m_iOrder로 분류하며 오름차순으로 업데이트함 ㅇㅅㅇ
class ENGINE_DLL Layer : public Reference
{
private:
	friend class Scene;

private:
	Layer();
	~Layer();

private:
	class Scene*	m_pScene;
	int				m_iOrder;			// 레이어 순서를 정하기 위한 변수 (오름차순으로 할거)

	// 각각의 레이어들은 자신들이 무슨 오브젝트가 있는지 알고있음.
	// vector를 사용하지 않고 list를 사용하는 이유는 보통 삽입 삭제가 빈번하게 (순서에 관계없이)일어날 경우 list를 사용함 ㅇㅅㅇ
	// 여기선 게임오브젝트들은 언제, 누가 삭제 될지 모르기 때문에 list로 사용
	std::list<class GameObject*>	m_GameObjectList;

	std::list<class GameObject*>	m_StartList;

	// + STL객체로 받을경우 (매개변수나..받는경우) 참조아님 포인터로 넣어야한다.
	// -> 이유는 만약 일반변수처럼 넣게되면 그때마다 값 복사가 아닌 메모리를 할당하기 때문이다. (메모리 낭비)

public:
	int GetOrder()	const;
	void SetOrder(int _Order);
	void Start();

	void SetScene(Scene* _pScene);
	Scene* GetScene();

public:
	bool Init();
	int Input(float _fTime);
	int Update(float _fTime);
	int LateUpdate(float _fTime);
	int Collision(float _fTime);
	int Render(float _fTime);

	void AddGameObject(class GameObject* _GameObject);
	class GameObject* FindGameObject(std::string _Tag);
	void DeleteGameObject(class GameObject* _Object);					// Delete는 아예 레이어에서 존재를 지우는것임
	void DeleteGameObject(std::string _Tag);

public:
	
};

ENGINE_END

