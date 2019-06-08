#pragma once
#include "Component.h"

ENGINE_BEGIN

// 클래스를 동적할당하면 4바이트 단위로 처리하게된다.
// 그런데 Transform같은 경우는 매트릭스 즉 16바이트 단위의 멤버변수를 갖고있는데
// __declspec(align(16))을 하지 않는 상태로 사용하게된다면
// 4바이트 단위로 처리하게되니까 ㅇㅅㅇ 안된다!
// 그러니까 16바이트로 처리하기 위해서 __declspec(align(16))를 사용한다.
class ENGINE_DLL __declspec(align(16)) Transform : public Component
{
private:
	friend class GameObject;

private:
	Transform();
	Transform(const Transform& _Component);
	~Transform();

private:
	bool	m_bStatic;			// 움직이는 오브젝트인지 아닌지 판별하기 위한 변수 (false는 움직이는 오브젝트, true는 움직이지 않는 오브젝트)
	bool	m_bUpdate;			// 무조건 한번은 Update해야한다. m_bUpdate는 최종 행렬을 다시 계산할건지의 유무이다. Transform의 경우 위치, 크기, 회전이 바뀔때마다 행렬을 다시 계산해야한다.
	bool	m_bUI;				// UI 오브젝트 여부
	TransformConstBuffer	m_tConstBuffer;		// 상수버퍼 ( 월드, 뷰, 프로젝션 행렬을 갖고있음 )
	TransformConstBuffer	m_tShadowCBuffer;

public:
	bool GetIsStatic()	const;
	bool GetIsUpdate()	const;
	bool GetUIEnable()	const;

public:
	void SetUIEnable(bool _Enable);

	/////////////////////////////////////////////////////////////////// 로컬
private:
	Vector3 m_vLocalPosition;
	Vector3 m_vLocalRotation;
	Vector3	m_vLocalSacle;

	// 월드 회전시 로컬뷰에도 영향이 미친다.( 축이 바뀜 )
	Vector3 m_vLocalRelativeView;
	Vector3 m_vLocalView;

public:
	Vector3 GetLocalPosition()	const;
	Vector3	GetLocalRotation()	const;
	Vector3	GetLocalScale()		const;
	Vector3 GetLocalView()		const;

public:
	void SetLocalPosition(float _x, float _y, float _z);
	void SetLocalPosition(const Vector3& _vPosition);

	void SetLocalRotation(float _x, float _y, float _z);
	void SetLocalRotation(const Vector3& _vRotation);
	void SetLocalRotateX(float _x);
	void SetLocalRotateY(float _y);
	void SetLocalRotateZ(float _z);

	void SetLocalScale(float _x, float _y, float _z);
	void SetLocalScale(const Vector3& _vScale);

	void SetLocalRelativeView(float _x, float _y, float _z);
	void SetLocalRelativeView(const Vector3& _vView);

private:
	Matrix	m_MatrixLocalPosition;
	Matrix	m_MatrixLocalRotation;
	Matrix	m_MatrixLocalScale;
	Matrix	m_MatrixLocal;

public:
	Matrix GetLocalRotationMatrix() const;
	Matrix GetLocalPositionMatrix() const;
	Matrix GetLocalMatrix()	const;

	///////////////////////////////////////////////////////////////// 월드
private:
	Vector3 m_vWorldPosition;
	Vector3 m_vWorldRotation;
	Vector3 m_vWorldScale;

	// 각 오브젝트들은 자신만의 고유의 축을 갖고있다. (Axis) 월드축이아닌 자신의 고유의 축임
	// 이축을 통해 캐릭터가 이동, 회전이 편하도록함 (앞으로 간다던지, 뒤로간다던지 등..)
	Vector3 m_vWorldAxis[AXIS_END];
	Vector3 m_vWorldView;
	Vector3 m_vMove;
	bool m_bLookAt;
	Transform* m_pLookAt;			// 바라보는 방향
	LOOKAT_AXIS m_eLookAxis;

public:
	void SetLookAtActive(bool _Active);

public:
	Vector3 GetWorldPosition()	const;
	Vector3 GetPrevWorldPosiitonAtMatrix() const;
	Vector3 GetWorldPositionAtMatrix() const;
	Vector3 GetWorldRotation()	const;
	Vector3 GetWorldScale()		const;
	Vector3 GetWorldAxis(AXIS _Axis)	const;
	Vector3 GetWorldView()	const;
	Vector3 GetMove()	const;

public:
	void SetWorldPosition(float _x, float _y, float _z);
	void SetWorldPosition(const Vector3& _Position);

	void SetWorldRotation(float _x, float _y, float _z);
	void SetWorldRotation(const Vector3& _Rotation);
	void SetWorldRotateX(float _x);
	void SetWorldRotateY(float _y);
	void SetWorldRotateZ(float _z);

	void SetWorldScale(float _x, float _y, float _z);
	void SetWorldScaleX(float _x);
	void SetWorldScaleY(float _y);
	void SetWorldScaleZ(float _z);
	void SetWorldScale(const Vector3& _Scale);
	
	void SetLookAtAxis(LOOKAT_AXIS _eAxis);

private:
	// 크자(회전)이공부
	// 공전행렬을 뺀이유는 부모에서 월드를 계산할때 넣어줄거기때문 ㅇㅅㅇ
	Matrix	m_MatrixWorldScale;				// 크기
	Matrix	m_MatrixWorldRotation;			// 회전
	Matrix	m_MatrixWorldPosition;			// 이동(위치)
	Matrix	m_MatrixWorldParent;			// 부모

	Matrix m_PrevMatrixWolrd;
	Matrix m_MatrixWorld;					// 최종행렬

public:
	void SetParentMatrix(const Matrix& _MatrixParent);

public:
	Matrix GetWorldRotationMatrix() const;
	Matrix GetWorldPositionMatrix() const;
	Matrix GetWorldMatrix()	const;

public:
	void Move(AXIS _eAxis, float _fSpeed, float _fTime);
	void Move(const Vector3& _vDir, float _fSpeed, float _fTime);
	void Move(const Vector3& _vMove);
	void MoveBack();

	void Rotate(const Vector3& _vRot, float _fTime);
	void Rotate(const Vector3& _vRot);
	void RotateX(float _fSpeed, float _fTime);
	void RotateX(float _fSpeed);
	void RotateY(float _fSpeed, float _fTime);
	void RotateY(float _fSpeed);
	void RotateZ(float _fSpeed, float _fTime);
	void RotateZ(float _fSpeed);

	void LookAt(class GameObject* _pObject);
	void LookAt(Component* _pComponent);
	void RemoveLookAt();
	void RotationLookAt();
	void RotationLookAt(Vector3 _vLookAt);
private:
	// 뷰회전
	void ComputeAxis();

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// 렌더를 하기 직전에 호출해주는 함수ㅔㄱㅁ
	virtual int PrevShadowRender(Matrix _matLightView, Matrix _matLightProj, float _fTime);
	virtual int Render(float _fTime);
	virtual Component* Clone()	const;

};

ENGINE_END
