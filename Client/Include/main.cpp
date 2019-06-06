#include "Client.h"
#include "Core.h"
#include "Scene\Scene.h"
#include "Scene\SceneManager.h"
#include "SceneComponent\MainScene.h"
#include "SceneComponent\TitleScene.h"


#pragma comment(lib, "Direct3DEngine_Debugx64")

ENGINE_USING
// 1280
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow){
	if (!GET_SINGLETON(Core)->Init(hInstance, TEXT("Client"), TEXT("Client"),
		1920, 1080, IDI_ICON1, IDI_ICON1, true, true, 1))
	{
		DESTROY_SINGLETON(Core);
		return 0;
	}
	
	GET_SINGLETON(SceneManager)->AddSceneComponent<TitleScene>();

	int	iResult = GET_SINGLETON(Core)->Run();

	DESTROY_SINGLETON(Core);

	return iResult;
}