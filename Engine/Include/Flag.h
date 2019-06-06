#pragma once
#include "Macro.h"

ENGINE_BEGIN

// 씬 체인지용 
enum SCENE_CHANGE
{
	SC_NONE,
	SC_NEXT				// 다음씬
};

// 셰이더 타입 ( 버텍스 / 픽셀 )
enum SHADER_TYPE
{
	ST_VERTEX = 0,
	ST_PIXEL,
	ST_GEOMETRY,
	ST_COMPUTE,
	ST_END
};

// 컴포넌트 타입
enum COMPONENT_TYPE
{
	CT_TRANSFORM = 0,
	CT_RENDERER,
	CT_USERCOMPONENT,				// 유저컴포넌트
	CT_CAMERA,						// 일반 카메라
	CT_OPCAMERA,					// 1인칭 카메라
	CT_TPCAMERA,					// 3인칭 카메라
	CT_MATERIAL,
	CT_COLLIDER,
	CT_LIGHT,
	CT_UI,
	CT_LANDSCAPE,					// 풍경 (지형용도로 쓸듯)
	CT_ANIMATION,
	CT_ANIMATIONFRAME,				// 프레임 애니메이션 -> 2D애니메이션
	CT_PARTICLE,
	CT_NAVIGATION,					// 네비게이션
	CT_SOUND,
	CT_TEXT
};

// 축
enum AXIS
{
	AXIS_X = 0,
	AXIS_Y,
	AXIS_Z,
	AXIS_END
};

// 
enum CONSTANT_SHADER
{
	CS_VERTEX = 0x1,
	CS_PIXEL = 0x2,
	CS_GEOMETRY = 0x4,
	CS_COMPUTE = 0x8,
};

// 키상태
enum KEY_STATE
{
	KS_PRESS = 0,
	KS_PUSH,
	KS_RELEASE,
	KS_END
};

// 키 상태 (눌렀는지 비트로 판단)
enum KEY_STATE_FALG
{
	KSF_PRESS = 0x1,
	KSF_PUSH = 0x2,
	KSF_RELEASE = 0x4
};

// 조합키(기능키 : 컨트롤 쉬프트 알트 )
enum FUNCTION_KEY
{
	FK_CTRL = 0,
	FK_ALT,
	FK_SHIFT,
	FK_END
};

enum FUNCTION_KEY_ENABLE
{
	FKE_CTRL = 0x1,
	FKE_ALT = 0x2,
	FKE_SHIFT = 0x4
};


// LOOKAT 축,  ALL은 상관없음/ 각 축들을 설정해서 지우는거임
enum LOOKAT_AXIS
{
	LOOK_X = 0,
	LOOK_Y,
	LOOK_Z,
	LOOK_ALL,
	LOOK_END
};

// 카메라타입
enum CAMERA_TYPE
{
	CAM_PERSPECTIVE = 0,			// 원근투영
	CAM_ORTHOGONAL,					// 직교투영
	CAM_END
};

// 렌더 상태 타입 ( 래스터라이저, 뎁스, 블렌드 )
enum RENDER_STATE
{
	RS_RASTERIZER,
	RS_DEPTH,
	RS_BLEND,
	RS_END
};

// 라이트타입
enum LIGHT_TYPE
{
	LT_DIR = 0,					// Directional Light (임의의 방향)
	LT_POINT,				// Point Light
	LT_SPOT,				// Spot Light
	LT_END
};

// 렌더 그룹 ( 렌더 그룹별로 렌더링해줄거임 ㅇㅅㅇ )
enum RENDER_GROUP
{
	RG_LANDSCAPE = 0,
	RG_STATIC,
	RG_DEFAULT,
	RG_ALPHA,
	RG_HUD,					// Head up display
	RG_UI,
	RG_END
};

// 렌더링 모드 ( 포워드, 디퍼드), 기본값은 디퍼드
enum RENDER_MODE
{
	RM_FORWARD = 0,
	RM_DEFERRED
};

// 애니메이션 옵션 ( loop or 한번만 돌고 지울지 ..)
enum ANIMATION_OPTION
{
	AO_LOOP,
	AO_ONCE_DESTROY
};

// 콜라이더 타입 (구, 캡슐, OBB 등 )
enum COLLIDER_TYPE
{
	COL_SPHERE,
	COL_CAPSULE,
	COL_OBB,
	COL_POINT,
	COL_RAY,
	COL_AABB,
	COL_LAND,
};

// 콜라이더 채널 상태 -> 콜라이더의 상태라고 보면 될듯
enum COLLISION_CHANNEL_STATE
{
	CCS_IGNORE,			// 무시
	CCS_OVERLAP,		// 겹쳤을때
	CCS_BLOCK			// 부딪혔을때
};

// 충돌체 콜백함수를 호출하기 위한 충돌상태 표시 -> 처음충돌, 충돌중, 충돌끝
// 충돌상태로 보면 될듯
enum COLLISION_CALLBACK_STATE
{
	CCBS_ENTER,					// 충돌 시작
	CCBS_STAY,					// 충돌 중
	CCBS_EXIT,					// 충돌을 끝냈을 때
	CCBS_END
};

// 애니메이션 프레임 타입
enum ANIMATION_FRAME_TYPE
{
	AFT_ATLAS,				// 아틀라스
	AFT_FRAME,				// 프레임별로.. (예전 노가다처럼 ㅇㅅㅇ)
	AFT_END
};


// 마우스 버튼 값
enum MOUSE_BUTTON
{
	MB_NONE = -1,
	MB_LBUTTON,			// 왼쪽 버튼
	MB_RBUTTON,			// 오른쪽 버튼
	MB_MBUTTON,			// 가운데버튼
	MB_END
};

ENGINE_END