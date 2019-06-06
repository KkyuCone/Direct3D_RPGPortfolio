#pragma once

#include "Engine.h"
#include "resource.h"


#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "Direct3DEngine_Debugx64")
#else
#pragma comment(lib, "Direct3DEngine_Releasex64")
#endif // _DEBUG
#else
#ifdef _DEBUG
#pragma comment(lib, "Direct3DEngine_Debugx32")
#else
#pragma comment(lib, "Direct3DEngine_Releasex32")
#endif // _DEBUG
#endif // _WIN64