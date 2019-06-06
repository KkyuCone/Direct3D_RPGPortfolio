#pragma once
//export일때랑 import일때
#ifdef ENGINE_EXPORT
#define ENGINE_DLL __declspec(dllexport)
#else
#define ENGINE_DLL __declspec(dllimport)
#endif // ENGINE_EXPORT

#define ENGINE_BEGIN namespace Engine {
#define ENGINE_END }
#define ENGINE_USING using namespace Engine;

#define SAFE_DELETE(p)		if(p)	{ delete p; p = nullptr; }
#define SAFE_DELETE_ARRAY(p) if(p)	{ delete[] p; p = nullptr; }
#define SAFE_RELEASE(p)		if(p)	{ p->Release(); p = nullptr; }


 //싱글톤
 //DECLARE_SINGLETON -> 싱글톤 선언하기
#define	DECLARE_SINGLETON(Type)	\
private:\
	static Type* m_pInst;\
public:\
	static Type* GetInst()\
	{\
		if(!m_pInst)\
			m_pInst = new Type;\
		return m_pInst;\
	}\
	static void DestroyInst()\
	{\
		SAFE_DELETE(m_pInst);\
	}\
private:\
	Type();\
	~Type();

#define DEFINITION_SINGLETON(Type) Type* Type::m_pInst = nullptr;

#define GET_SINGLETON(Type)			Type::GetInst()
#define DESTROY_SINGLETON(Type)		Type::DestroyInst()

#define _DEVICE	GET_SINGLETON(Device)->GetDevice()
#define _CONTEXT GET_SINGLETON(Device)->GetContext()
#define _RESOLUTION GET_SINGLETON(Device)->GetResolution()

// 윈도우 인스턴스, 핸들
#define WINDOWINSTANCE	GET_SINGLETON(Core)->GetWindowInstance()
#define WINDOWHANDLE	GET_SINGLETON(Core)->GetWindowHandle()
#define	RENDERTARGET2D	GET_SINGLETON(Device)->Get2DRenderTarget()

// Split함수는 Path를 잘라준다. -> 확장자, 폴더명, 드라이브 등
// 유니코드랑 멀티바이트 나눠서 'ㅅ'
// 같은 확장자여도 대소문자에 따라 다른 문자열로 인식된다.
// 그래서 모두 대문자로 바꾼뒤에 문자열을 검사하도록 한다.
#ifdef UNICODE
#define	SplitPath	_wsplitpath_s
#else
#define	SplitPath	_splitpath_s
#endif // UNICODE