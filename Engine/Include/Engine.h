#pragma once
#include <Windows.h>
#include <list>
#include <vector>
#include <unordered_map>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dwrite.h>
#include <d2d1.h>
#include <dinput.h>				// Direct Input �������

#include <string>
#include <crtdbg.h>
#include <cstdlib>
#include <random>
#include <ctime>
#include <functional>
#include <stack>

//using namespace std;

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")

// Direct Input�� �����ϱ� ���ؼ� �Ʒ��� �� ���̺귯���� ��ũ�Ǿ����
#pragma comment(lib, "dinput8")
#pragma comment(lib, "dxguid")


#include "Types.h"

// Path key (��ΰ���)
#define PATH_ROOT	"Path_Root"
#define PATH_SHADER	"Path_Shader"

#define PATH_TEXTURE "Path_Texture"
#define PATH_TEST_TEXTURE "Path_TestTexture"
#define PATH_LANDSCAPE "Path_LandScape"

#define PATH_MESH			"Path_Mesh"
#define PATH_MESH_HOUSE		"Path_Mesh_House"
#define PATH_MESH_HUMAN		"Path_Mesh_Human"
#define PATH_MESH_MONSTER	"Path_Mesh_Monster"
#define PATH_MESH_ROCK		"Path_Mesh_Rock"
#define PATH_MESH_TREE		"Path_Mesh_Tree"
#define PATH_MESH_WALL		"Path_Mesh_Wall"
#define PATH_MESH_WEAPON	"Path_Mesh_Weapon"

#define PATH_DATA			"Path_Data"

#define PATH_UI				"Path_UI"
#define PATH_UI_BUTTON		"Path_UI_Button"
#define PATH_UI_IMAGE		"Path_UI_Image"
#define PATH_UI_BAR			"Path_UI_Bar"

#define PATH_EXCEL			"Path_Excel"

#define PATH_SOUND			"Path_Sound"

#define PATH_RAIN			"Path_Rain"

// Shader key
#define SHADER_STANDARD_COLOR			"Sahder_StandardColor"
#define SHADER_STANDARD_NORMAL_COLOR	"Shader_StandardNormalColor"
#define SHADER_STANDARD_TEXTURE_NORMAL	"Shader_StandardTextureNormal"		// �⺻ 
#define SHADER_STANDARD_BUMP			"Shader_StandardBump"				// �븻�� ���̴�
#define SHADER_STANDARD_3D				"Shader_Standard3D"					// �⺻�ؽ�ó ���̴�
#define SHADER_STANDARD_3D_DYANMIC_DEPTH	"Shader_Standard3D_Dynamic_Depth"
#define SHADER_STANDARD_3D_STATIC_DEPTH		"Shader_Standard3D_Static_Depth"

#define SHADER_STANDARD_3D_INSTANCING	"Shader_Standard3DInstancing"		// �ν��Ͻ̿���̴�
#define SHADER_STANDARD_3D_INSTANCING_DEPTH	"Shader_Standard3DInstancing_Depth"


#define SHADER_DEBUG					"Shader_Debug"

#define SHADER_LIGHTACC_DIR				"Shader_LightAccDir"
#define SHADER_LIGHTACC_POINT			"Shader_LightAccPoint"
#define SHADER_LIGHTACC_SPOT			"Shader_LightAccSpot"
#define SHADER_LIGHT_BLEND				"Shader_LightBlend"
#define SHADER_LIGHT_BLEND_RENDER		"Shader_LightBlendRender"

#define SHADER_SKY						"Shader_Sky"
#define SHADER_TEST_CYLINDER			"Shader_TestCylinder"

#define SHADER_COLLIDER					"Shader_ColliderDebug"					// �ݶ��̴� ���̴� (������)

#define SHADER_PARTICLE					"Shader_Particle"						// ��ƼŬ ���̴�
#define SHADER_PARTICLE_INSTNACING		"Shader_Particle_Instancing"			// ��ƼŬ �ν��Ͻ̿�
#define SHADER_LANDSCAPE				"Shader_LandScape"						// ����

#define SHADER_POSTEFFECT								"Shader_PostEffect"
#define SHADER_POSTEFFECT_DOWNSCALE_FIRST				"Shader_PostEffect_DownScale_First"
#define SHADER_POSTEFFECT_DOWNSCALE_SECOND				"Shader_PostEffect_DownScale_Second"
#define SHADER_POSTEFFECT_BLOOM_BRITEPASS				"Shader_PostEffect_Blur_BrightPass"
#define SHADER_POSTEFFECT_BLOOM_BLURFILTER_VERTICAL		"Shader_PostEffect_Bloom_BlurFilter_Vertical"
#define SHADER_POSTEFFECT_BLOOM_BLURFILTER_HORIZON		"Shader_PostEffect_Bloom_BlurFilter_Horizon"

#define SHADER_SHDOW									"Shader_Shadow"

#define SHADER_SSAO_DOWNSCALE			"Shader_SSAO_DownScale"				// SSAO - DownScale
#define SHADER_SSAO_COMPUTESSAO			"Shader_SSAO_ComputeSSAO"			// SSAO - ComputeSSAO

#define SHADER_RAIN						"Shader_Rain"
#define SHADER_RAIN_COMPUTE				"Shader_RainCompute"
#define SHADER_RAIN_GEN					"Shader_RainGen"


#define SHADER_UI_BUTTON				"Shader_UIButton"						// UI ��ư��
#define SHADER_UI_IMAGE					"Shader_UIImage"						// UI �̹����� (�ܼ��̹���)
#define SHADER_UI_BAR					"Shader_UIBar"							// UI Bar

// InputLayout Key
#define LAYOUT_POSITION					"Layout_Position"
#define LAYOUT_POSITION_UV				"Layout_PositionUV"
#define LAYOUT_POSITION_COLOR			"Layout_PosiitonColor"
#define LAYOUT_POSITION_NORMAL_COLOR	"Layout_PositionNormalColor"
#define LAYOUT_VERTEX3D					"Layout_Vertex3D"

#define LAYOUT_VERTEX3D_STATIC_INSTANCING	"Layout_Vertex3D_Static_Instancing"		// �ν��Ͻ� ����ƽ�� ���̾ƿ�
#define LAYOUT_VERTEX3D_STATIC_INSTANCING_DEPTH	"Layout_Vertex3D_Static_Instancing_Depth"		// �ν��Ͻ� ����ƽ�� ���̾ƿ�
#define LAYOUT_PARTICLE_INSTANCING			"Layout_Particle_Instnacing"			// �ν��Ͻ� ��ƼŬ(2D�ִϸ��̼�)��
#define LAYOUT_ANIMATION3D_INSTANCING		"Layout_Animation3D_Instnacing"			// �ν��Ͻ� �ִϸ��̼� 3D��

#define LAYOUT_POSTEFFECT				"Layout_PostEffect"					// ����Ʈ ����Ʈ

#define LAYOUT_SHADOW					"Layout_Shadow"						// �׸���

#define LAYOUT_RAIN					"Layout_Rain"
#define LAYOUT_RAINGEN				"Layout_RainGen"


#define LAYOUT_TEST_CYLINDER_POSITION	"Layout_TestCylinder"

// ���÷� Ű ( ��, ����, �� )
#define SAMPLER_LINEAR			"Sampler_Linear"
#define SAMPLER_POINT			"Sampler_Point"
#define SAMPLER_CLAMP			"Sampler_Clamp"
#define SAMPLER_WRAP			"Sampler_Wrap"
#define SAMPLER_SHADOW			"Sampler_Shadow"

// ������ 
#define BLEND_NORMAL			"Blend_Normal"					// ǥ��
#define BLEND_DARKEN			"Blend_Darken"					// ��Ӱ�
#define BLEND_MULTIPLY			"Blend_Multiply"				// ���ϱ�
#define BLEND_LIGHTEN			"Blend_Lighten"					// ���
#define BLEND_SCREEN			"Blend_Screen"					// ��ũ��
#define BLEND_LINEAR_DODGE		"Blend_Linear_Dodge"			// ���� ����
#define BLEND_SUBTRACT			"Blend_Subtract"				// ����

template <typename T>
void Safe_Delete_VectorList(T& _p)
{
	T::iterator StartIter = _p.begin();
	T::iterator EndIter = _p.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		SAFE_DELETE((*StartIter));
	}

	_p.clear();
}

template <typename T>
void Safe_Delete_Array_VectorList(T& _p)
{
	T::iterator StartIter = _p.begin();
	T::iterator EndIter = _p.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		SAFE_DELETE_ARRAY((*StartIter));
	}

	_p.clear();
}

template <typename T>
void Safe_Release_VectorList(T& _p)
{
	T::iterator StartIter = _p.begin();
	T::iterator EndIter = _p.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		SAFE_RELEASE((*StartIter));
	}

	_p.clear();
}
 
template <typename T>
void Safe_Delete_Map(T& _p)
{
	T::iterator StartIter = _p.begin();
	T::iterator EndIter = _p.end();

	for ( ; StartIter != EndIter; ++StartIter)
	{
		SAFE_DELETE(StartIter->second);
	}

	_p.clear();
}

template <typename T>
void Safe_Release_Map(T& _p)
{
	T::iterator	iter = _p.begin();
	T::iterator	iterEnd = _p.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_RELEASE(iter->second);
	}

	_p.clear();
}


template <typename T>
void Safe_Delete_Array_Map(T& _p)
{
	T::iterator StartIter = _p.begin();
	T::iterator EndIter = _p.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		SAFE_DELETE_ARRAY(StartIter->second);
	}

	_p.clear();
}



// String -> wstring

ENGINE_DLL std::wstring StringToWstring(std::string s);

// TCHAR -> String
ENGINE_DLL std::string TCHARToString(const TCHAR* ptsz);

// String -> wchar_t*
ENGINE_DLL std::wstring StringToLPWSTR(const std::string input);


ENGINE_DLL bool StringToWstring(const std::string input, std::wstring& _output);