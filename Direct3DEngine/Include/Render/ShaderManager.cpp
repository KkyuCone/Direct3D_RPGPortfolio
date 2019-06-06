#include "ShaderManager.h"
#include "Shader.h"
#include "../Device.h"

ENGINE_USING

DEFINITION_SINGLETON(ShaderManager)

ShaderManager::ShaderManager() : m_iInputSize(0)
{
}


ShaderManager::~ShaderManager()
{
	std::unordered_map<std::string, PConstBuffer>::iterator StartIter = m_mapConstBuffer.begin();
	std::unordered_map<std::string, PConstBuffer>::iterator EndIter = m_mapConstBuffer.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		SAFE_DELETE_ARRAY(StartIter->second->pData);
		SAFE_RELEASE(StartIter->second->pBuffer);
		SAFE_DELETE(StartIter->second);
	}

	m_mapConstBuffer.clear();

	Safe_Release_Map(m_mapShader);
	Safe_Release_Map(m_mapInputLayout);
}

bool ShaderManager::Init()
{
	std::string pEntry[ST_END] = {};

#pragma region Color Shader

	pEntry[ST_VERTEX] = "VS_StandardColor";
	pEntry[ST_PIXEL] = "PS_StandardColor";

	// ���̴� �ε�
	if (false == LoadShader(SHADER_STANDARD_COLOR, TEXT("Standard.fx"), pEntry))
	{
		return false;
	}

	// �Է·��̾ƿ�
	AddInputDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 16, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);

	if (false == CreateInputLayout(LAYOUT_POSITION_COLOR, SHADER_STANDARD_COLOR))
	{
		return false;
	}

#pragma endregion

#pragma region Light Shader - Normal

	pEntry[ST_VERTEX] = "VS_StandardNormalColor";
	pEntry[ST_PIXEL] = "PS_StandardNormalColor";

	if (false == LoadShader(SHADER_STANDARD_NORMAL_COLOR, TEXT("Standard.fx"), pEntry))
	{
		return false;
	}

	AddInputDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);

	if (false == CreateInputLayout(LAYOUT_POSITION_NORMAL_COLOR, SHADER_STANDARD_NORMAL_COLOR))
	{
		return false;
	}

#pragma endregion


#pragma region Sky Shader (Sphere)

	// ��ī�� ���̴�
	pEntry[ST_VERTEX] = "VS_Sky";
	pEntry[ST_PIXEL] = "PS_Sky";

	if (false == LoadShader(SHADER_SKY, TEXT("Standard.fx"), pEntry))
	{
		return false;
	}

	AddInputDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);

	if (false == CreateInputLayout(LAYOUT_POSITION, SHADER_SKY))
	{
		return false;
	}

#pragma endregion

#pragma region Cylinder Shader
	// ��ī�� ���̴�
	pEntry[ST_VERTEX] = "VS_Sky";
	pEntry[ST_PIXEL] = "PS_Cone";

	if (false == LoadShader(SHADER_TEST_CYLINDER, TEXT("Standard.fx"), pEntry))
	{
		return false;
	}

	AddInputDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	//AddInputDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12,
	//	0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	//AddInputDesc("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
	//	0, D3D11_INPUT_PER_VERTEX_DATA, 0);

	if (false == CreateInputLayout(LAYOUT_TEST_CYLINDER_POSITION, SHADER_TEST_CYLINDER))
	{
		return false;
	}
#pragma endregion



#pragma region Vertex3D Shader

	// �⺻ ���ؽ��� ���̴�(Material) -  Vertex3D
	pEntry[ST_VERTEX] = "VS_StandardVertex3D";
	pEntry[ST_PIXEL] = "PS_StandardVertex3D";

	if (false == LoadShader("Vertex3D", TEXT("Share.fx"), pEntry))
	{
		return false;
	}

	AddInputDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 8,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);

	if (false == CreateInputLayout(LAYOUT_VERTEX3D, "Vertex3D"))
	{
		return false;
	}

#pragma endregion

#pragma region Vertex3D Shader Instancing
	// �⺻ ���ؽ��� ���̴�(Material) -  Vertex3D
	pEntry[ST_VERTEX] = "VS_Standard3D_Instancing";
	pEntry[ST_PIXEL] = "PS_Standard3D_Instancing";
	pEntry[ST_GEOMETRY] = "";

	if (false == LoadShader(SHADER_STANDARD_3D_INSTANCING, TEXT("Standard.fx"), pEntry))
	{
		return false;
	}

	AddInputDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 8,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);

	// �ν��Ͻ� ������ ���̾ƿ��� ����� ���� �ʱ�ȭ�ϱ�
	m_iInputSize = 0;

	// 
	AddInputDesc("WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);

	AddInputDesc("WORLDVIEW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("WORLDVIEW", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("WORLDVIEW", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("WORLDVIEW", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);

	AddInputDesc("WORLDVIEWROT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("WORLDVIEWROT", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("WORLDVIEWROT", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("WORLDVIEWROT", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);

	if (false == CreateInputLayout(LAYOUT_VERTEX3D_STATIC_INSTANCING, SHADER_STANDARD_3D_INSTANCING))
	{
		return false;
	}


	//

	pEntry[ST_VERTEX] = "VS_Standard3D_Instancing";
	pEntry[ST_PIXEL] = "PS_Standard3D_Instancing_Depth";
	pEntry[ST_GEOMETRY] = "";

	if (false == LoadShader(SHADER_STANDARD_3D_INSTANCING_DEPTH, TEXT("Standard.fx"), pEntry))
	{
		return false;
	}

	AddInputDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 8,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);

	// �ν��Ͻ� ������ ���̾ƿ��� ����� ���� �ʱ�ȭ�ϱ�
	m_iInputSize = 0;

	// 
	AddInputDesc("WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);

	AddInputDesc("WORLDVIEW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("WORLDVIEW", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("WORLDVIEW", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("WORLDVIEW", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);

	AddInputDesc("WORLDVIEWROT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("WORLDVIEWROT", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("WORLDVIEWROT", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("WORLDVIEWROT", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);

	if (false == CreateInputLayout(LAYOUT_VERTEX3D_STATIC_INSTANCING_DEPTH, SHADER_STANDARD_3D_INSTANCING_DEPTH))
	{
		return false;
	}

#pragma endregion



#pragma region Light Shader

	// ����Ʈ ���� - Directional
	pEntry[ST_VERTEX] = "VS_LightAccDir";
	pEntry[ST_PIXEL] = "PS_LightAcc";

	if (false == LoadShader(SHADER_LIGHTACC_DIR, TEXT("Light.fx"), pEntry))
	{
		return false;
	}

	// ����Ʈ ���� - Point Light
	pEntry[ST_VERTEX] = "VS_LightAccPoint";
	pEntry[ST_PIXEL] = "PS_LightAcc";

	if (false == LoadShader(SHADER_LIGHTACC_POINT, TEXT("Light.fx"), pEntry))
	{
		return false;
	}

	// ����Ʈ ���� - Spot Light
	pEntry[ST_VERTEX] = "VS_LightAccSpot";
	pEntry[ST_PIXEL] = "PS_LightAcc";

	if (false == LoadShader(SHADER_LIGHTACC_SPOT, TEXT("Light.fx"), pEntry))
	{
		return false;
	}

	// ����Ʈ ����
	pEntry[ST_VERTEX] = "VS_LightAccDir";
	pEntry[ST_PIXEL] = "PS_LightBlend";

	if (false == LoadShader(SHADER_LIGHT_BLEND, TEXT("Light.fx"), pEntry))
	{
		return false;
	}

	// ����Ʈ ���� ����
	pEntry[ST_VERTEX] = "VS_LightAccDir";
	pEntry[ST_PIXEL] = "PS_LightBlendRender";

	if (false == LoadShader(SHADER_LIGHT_BLEND_RENDER, TEXT("Light.fx"), pEntry))
	{
		return false;
	}

#pragma endregion



#pragma region Debug Shader

	pEntry[ST_VERTEX] = "VS_Debug";
	pEntry[ST_PIXEL] = "PS_Debug";

	if (false == LoadShader(SHADER_DEBUG, TEXT("Debug.fx"), pEntry))
	{
		return false;
	}

	// �Է� ���̾ƿ�
	AddInputDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 8,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);

	if (false == CreateInputLayout(LAYOUT_POSITION_UV, SHADER_DEBUG))
	{
		return false;
	}
#pragma endregion


#pragma region �⺻ �ؽ�ó ���̴���

	// ���� �ؽ�ó ���̴�
	pEntry[ST_VERTEX] = "VS_StandardTextureNormal";
	pEntry[ST_PIXEL] = "PS_Standard3D";
	pEntry[ST_GEOMETRY] = "";
	pEntry[ST_COMPUTE] = "";

	if (false == LoadShader(SHADER_STANDARD_BUMP, TEXT("Standard.fx"), pEntry))
	{
		return false;
	}

	// �⺻ �ؽ��ĵ� ���̴�
	pEntry[ST_VERTEX] = "VS_Standard3D";
	pEntry[ST_PIXEL] = "PS_Standard3D";
	pEntry[ST_GEOMETRY] = "";
	pEntry[ST_COMPUTE] = "";

	if (false == LoadShader(SHADER_STANDARD_3D, TEXT("Standard.fx"), pEntry))
	{
		return false;
	}

	pEntry[ST_VERTEX] = "VS_Standard3D";
	pEntry[ST_PIXEL] = "PS_Dynamic_Depth";
	pEntry[ST_GEOMETRY] = "";
	pEntry[ST_COMPUTE] = "";

	if (false == LoadShader(SHADER_STANDARD_3D_DYANMIC_DEPTH, TEXT("Standard.fx"), pEntry))
	{
		return false;
	}

	pEntry[ST_VERTEX] = "VS_Standard3D";
	pEntry[ST_PIXEL] = "PS_Standard3D_Depth";
	pEntry[ST_GEOMETRY] = "";
	pEntry[ST_COMPUTE] = "";

	if (false == LoadShader(SHADER_STANDARD_3D_STATIC_DEPTH, TEXT("Standard.fx"), pEntry))
	{
		return false;
	}

#pragma endregion

#pragma region �ݶ��̴� ������ ���̴�

	pEntry[ST_VERTEX] = "VS_Collider";
	pEntry[ST_PIXEL] = "PS_Collider";
	pEntry[ST_GEOMETRY] = "";
	pEntry[ST_COMPUTE] = "";

	if (false == LoadShader(SHADER_COLLIDER, TEXT("Collider.fx"), pEntry))
	{
		return false;
	}
#pragma endregion

#pragma region ��ƼŬ�� ���̴�

	pEntry[ST_VERTEX] = "VS_Particle";
	pEntry[ST_PIXEL] = "PS_Particle";
	pEntry[ST_GEOMETRY] = "GS_Particle";
	pEntry[ST_COMPUTE] = "";

	if (false == LoadShader(SHADER_PARTICLE, TEXT("Particle.fx"), pEntry))
	{
		return false;
	}

	// �ν��Ͻ� ��ƼŬ - �ִϸ��̼� ������
	pEntry[ST_VERTEX] = "VS_Particle_INSTANCING";
	pEntry[ST_PIXEL] = "PS_Particle_INSTANCING";
	pEntry[ST_GEOMETRY] = "GS_Particle_INSTANCING";

	if (false == LoadShader(SHADER_PARTICLE_INSTNACING, TEXT("Particle.fx"), pEntry))
	{
		return false;
	}

	// �ν��Ͻ� ��ƼŬ - ���̾ƿ�
	// �Է� ���̾ƿ�
	m_iInputSize = 0;
	AddInputDesc("WORLDVIEWPROJ", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("WORLDVIEWPROJ", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("WORLDVIEWPROJ", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("WORLDVIEWPROJ", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);

	AddInputDesc("VIEWPROJ", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("VIEWPROJ", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("VIEWPROJ", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("VIEWPROJ", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);

	AddInputDesc("WORLDPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("PARTICLESCALE", 0, DXGI_FORMAT_R32G32_FLOAT, 8,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);

	AddInputDesc("COORDSTART", 0, DXGI_FORMAT_R32G32_FLOAT, 8,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("COORDEND", 0, DXGI_FORMAT_R32G32_FLOAT, 8,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	AddInputDesc("FRAME", 0, DXGI_FORMAT_R32_SINT, 4,
		1, D3D11_INPUT_PER_INSTANCE_DATA, 1);


	if (false == CreateInputLayout(LAYOUT_PARTICLE_INSTANCING, SHADER_PARTICLE_INSTNACING))
	{
		return false;
	}


#pragma endregion

#pragma region ���� ���� ���̴� ( LandScape )

	pEntry[ST_VERTEX] = "VS_LandScape";
	pEntry[ST_PIXEL] = "PS_LandScape";
	pEntry[ST_GEOMETRY] = "";
	pEntry[ST_COMPUTE] = "";

	if (false == LoadShader(SHADER_LANDSCAPE, TEXT("LandScape.fx"), pEntry))
	{
		return false;
	}

#pragma endregion

#pragma region UI ����

	// ��ư
	pEntry[ST_VERTEX] = "VS_Button";
	pEntry[ST_PIXEL] = "PS_Button";
	pEntry[ST_GEOMETRY] = "";
	pEntry[ST_COMPUTE] = "";

	if (false == LoadShader(SHADER_UI_BUTTON, TEXT("UI.fx"), pEntry))
	{
		return false;
	}

	// �̹���
	pEntry[ST_VERTEX] = "VS_Image";
	pEntry[ST_PIXEL] = "PS_Image";
	pEntry[ST_GEOMETRY] = "";
	pEntry[ST_COMPUTE] = "";

	if (false == LoadShader(SHADER_UI_IMAGE, TEXT("UI.fx"), pEntry))
	{
		return false;
	}

	// ��
	pEntry[ST_VERTEX] = "VS_Bar";
	pEntry[ST_PIXEL] = "PS_Bar";
	pEntry[ST_GEOMETRY] = "";
	pEntry[ST_COMPUTE] = "";

	if (false == LoadShader(SHADER_UI_BAR, TEXT("UI.fx"), pEntry))
	{
		return false;
	}
#pragma endregion

#pragma region ����Ʈ ���μ��� ����

	// PostEffect
	pEntry[ST_VERTEX] = "VS_FullScreenQuad";
	pEntry[ST_PIXEL] = "PS_FinalPass";
	pEntry[ST_GEOMETRY] = "";
	pEntry[ST_COMPUTE] = "";

	if (false == LoadShader(SHADER_POSTEFFECT, TEXT("PostEffect.fx"), pEntry))
	{
		return false;
	}

	AddInputDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 8,
		0, D3D11_INPUT_PER_VERTEX_DATA, 0);

	if (false == CreateInputLayout(LAYOUT_POSTEFFECT, SHADER_POSTEFFECT))
	{
		return false;
	}

	// PostEffect DownScale - DownScaleFirstPass
	pEntry[ST_VERTEX] = "";
	pEntry[ST_PIXEL] = "";
	pEntry[ST_GEOMETRY] = "";
	pEntry[ST_COMPUTE] = "DownScaleFirstPass";

	if (false == LoadShader(SHADER_POSTEFFECT_DOWNSCALE_FIRST, TEXT("PostEffectDownScale.fx"), pEntry))
	{
		return false;
	}

	// PostEffect DownScale - DownScaleSecondPass
	pEntry[ST_VERTEX] = "";
	pEntry[ST_PIXEL] = "";
	pEntry[ST_GEOMETRY] = "";
	pEntry[ST_COMPUTE] = "DownScaleSecondPass";

	if (false == LoadShader(SHADER_POSTEFFECT_DOWNSCALE_SECOND, TEXT("PostEffectDownScale.fx"), pEntry))
	{
		return false;
	}

	// ��� - BritePass, ��
	pEntry[ST_VERTEX] = "";
	pEntry[ST_PIXEL] = "";
	pEntry[ST_GEOMETRY] = "";
	pEntry[ST_COMPUTE] = "BrightPass";

	if (false == LoadShader(SHADER_POSTEFFECT_BLOOM_BRITEPASS, TEXT("PostEffectDownScale.fx"), pEntry))
	{
		return false;
	}

	pEntry[ST_VERTEX] = "";
	pEntry[ST_PIXEL] = "";
	pEntry[ST_GEOMETRY] = "";
	pEntry[ST_COMPUTE] = "BlurVerticalFilter";

	if (false == LoadShader(SHADER_POSTEFFECT_BLOOM_BLURFILTER_VERTICAL, TEXT("Blur.fx"), pEntry))
	{
		return false;
	}

	pEntry[ST_VERTEX] = "";
	pEntry[ST_PIXEL] = "";
	pEntry[ST_GEOMETRY] = "";
	pEntry[ST_COMPUTE] = "BlurHorizonFilter";

	if (false == LoadShader(SHADER_POSTEFFECT_BLOOM_BLURFILTER_HORIZON, TEXT("Blur.fx"), pEntry))
	{
		return false;
	}

#pragma endregion


#pragma region SSAO

	pEntry[ST_VERTEX] = "";
	pEntry[ST_PIXEL] = "";
	pEntry[ST_GEOMETRY] = "";
	pEntry[ST_COMPUTE] = "DownScale";

	if (false == LoadShader(SHADER_SSAO_DOWNSCALE, TEXT("SSAO.fx"), pEntry))
	{
		return false;
	}

	pEntry[ST_VERTEX] = "";
	pEntry[ST_PIXEL] = "";
	pEntry[ST_GEOMETRY] = "";
	pEntry[ST_COMPUTE] = "ComputeSSAO";

	if (false == LoadShader(SHADER_SSAO_COMPUTESSAO, TEXT("SSAO.fx"), pEntry))
	{
		return false;
	}

#pragma endregion

	
#pragma region Constance Buffer  �������

	// ��� ���� ����
	CreateConstBuffer("Transform", sizeof(TransformConstBuffer), 0, CS_VERTEX | CS_PIXEL | CS_GEOMETRY);
	CreateConstBuffer("Material", sizeof(MaterialCBuffer), 1, CS_VERTEX | CS_PIXEL);
	CreateConstBuffer("Light", sizeof(LightInfo), 2, CS_VERTEX | CS_PIXEL);
	CreateConstBuffer("Rendering", sizeof(RenderCbuffer), 3, CS_VERTEX | CS_PIXEL);
	CreateConstBuffer("Debug", sizeof(DebugCBuffer), 9, CS_VERTEX | CS_PIXEL);
	CreateConstBuffer("Collider", sizeof(Vector4), 10, CS_PIXEL);
	CreateConstBuffer("Particle", sizeof(ParticleCBuffer), 11, CS_GEOMETRY);
	CreateConstBuffer("AnimationFrame", sizeof(AnimationFrameCBuffer), 8, CS_VERTEX | CS_PIXEL | CS_GEOMETRY);
	CreateConstBuffer("LandScape", sizeof(LandScaleCBuffer), 10, CS_VERTEX | CS_PIXEL);
	
	CreateConstBuffer("UI_Button", sizeof(ButtonCBuffer), 12, CS_VERTEX | CS_PIXEL);
	CreateConstBuffer("UI_Image", sizeof(ImageCBuffer), 13, CS_VERTEX | CS_PIXEL);
	CreateConstBuffer("UI_Bar", sizeof(BarCBuffer), 11, CS_VERTEX | CS_PIXEL);

	// ����Ʈ ���μ��� ����
	CreateConstBuffer("TDownScaleCBuffer", sizeof(TDownScaleCBuffer), 11, CS_COMPUTE);
	CreateConstBuffer("TFinalPassCBuffer", sizeof(TFinalPassCBuffer), 12, CS_VERTEX | CS_PIXEL);

	// �׸���
	CreateConstBuffer("ShadowMatCBuffer", sizeof(ShadowMatCBuffer), 4, CS_VERTEX | CS_PIXEL);
	CreateConstBuffer("ShadowLightVSCBuffer", sizeof(ShadowLightVSCBuffer), 5, CS_VERTEX | CS_PIXEL);
	CreateConstBuffer("ShadowLightPSBuffer", sizeof(ShadowLightPSBuffer), 6, CS_VERTEX | CS_PIXEL);
	CreateConstBuffer("ShadowBias", sizeof(ShadowBiasCBuffer), 4, CS_VERTEX | CS_PIXEL);

	// SSAO
	CreateConstBuffer("SSAOCBuffer", sizeof(SSAODownScaleCBuffer), 0, CS_COMPUTE);

	// Depth Fog
	CreateConstBuffer("DepthFogCBuffer", sizeof(DepthFogCBuffer), 5, CS_VERTEX | CS_PIXEL);

	// ������Ʈ ���� ����
	CreateConstBuffer("BLimLightCBuffer", sizeof(BLimLightCBuffer), 4, CS_VERTEX | CS_PIXEL);
#pragma endregion

	return true;
}

// ���̴� �ε�
bool ShaderManager::LoadShader(const std::string& _strName, const TCHAR* _pFileName,
	std::string _pEntry[ST_END], const std::string& _strPathKey /*= PATH_SHADER*/)
{
	Shader* pShader = FindShader(_strName);

	// ���̴��� �ߺ��Ǹ� �ȵ�
	if (nullptr != pShader)
	{
		SAFE_RELEASE(pShader);				// ����ī��Ʈ�� ���������� Release�� ���ش�.
		return false;
	}

	pShader = new Shader;

	if (false == pShader->LoadShader(_strName, _pFileName, _pEntry, _strPathKey))
	{
		SAFE_RELEASE(pShader);				// ����ī��Ʈ�� ���������� Release�� ���ش�.
		return false;
	}

	m_mapShader.insert(std::make_pair(_strName, pShader));

	return true;
}

class Shader* ShaderManager::FindShader(const std::string& _strName)
{
	std::unordered_map<std::string, Shader*>::iterator FindShaderIter = m_mapShader.find(_strName);

	// ���̴��� ������..
	if (FindShaderIter == m_mapShader.end())
	{
		return nullptr;
	}

	FindShaderIter->second->AddReference();

	return FindShaderIter->second;
}

void ShaderManager::AddInputDesc(const char* _pSemanticName, UINT _iSementicIdx,
	DXGI_FORMAT _eFormat, UINT _iSize, UINT _iInputSlot,
	D3D11_INPUT_CLASSIFICATION _eClass, UINT _iStepRate)
{
	// ���̾ƿ��� �߰��Ϸ��� D3D11_INPUT_ELEMENT_DESC ����ü�� ä������Ѵ�.
	D3D11_INPUT_ELEMENT_DESC	tDesc = {};
	tDesc.SemanticName = _pSemanticName;
	tDesc.SemanticIndex = _iSementicIdx;
	tDesc.Format = _eFormat;
	tDesc.InputSlot = _iInputSlot;
	tDesc.InputSlotClass = _eClass;
	tDesc.InstanceDataStepRate = _iStepRate;
	tDesc.AlignedByteOffset = m_iInputSize;

	m_iInputSize += _iSize;			// �������� ������ �ִ� ����� �°� �ڷ� �о������

	m_vecInputDesc.push_back(tDesc);
}

bool ShaderManager::CreateInputLayout(const std::string& _strName, const std::string& _strShaderKey)
{
	ID3D11InputLayout* pLayout = FindInputLayout(_strName);

	// �̹� �ش� ���̾ƿ��� �����Ѵٸ� ( ���̾ƿ� ����ǵ� ������ �̹� ������ �ʿ����)
	if (nullptr != pLayout)
	{
		return false;
	}

	Shader* pShader = FindShader(_strShaderKey);

	// �ش� ���̴��� �������� �ʴٸ�
	if (nullptr == pShader)
	{
		return false;
	}

	if (FAILED(_DEVICE->CreateInputLayout(&m_vecInputDesc[0],
		(UINT)(m_vecInputDesc.size()), pShader->GetVSCode(), (UINT)(pShader->GetVSCodeSize()),
		&pLayout)))
	{
		SAFE_RELEASE(pShader);
		return false;
	}

	m_vecInputDesc.clear();
	m_iInputSize = 0;

	m_mapInputLayout.insert(std::make_pair(_strName, pLayout));
	SAFE_RELEASE(pShader);

	return true;
}

ID3D11InputLayout* ShaderManager::FindInputLayout(const std::string& _strName)
{
	std::unordered_map<std::string, ID3D11InputLayout*>::iterator FindInputLayoutIter = m_mapInputLayout.find(_strName);

	if (FindInputLayoutIter == m_mapInputLayout.end())
	{
		return nullptr;
	}

	return FindInputLayoutIter->second;
}

bool ShaderManager::CreateConstBuffer(const std::string & _strName, 
	int _iSize, 
	int _iRegister, 
	int _iConstantShader)
{
	PConstBuffer	pBuffer = FindConstBuffer(_strName);

	if (nullptr != pBuffer)
	{
		return false;
	}

	pBuffer = new ConstBuffer;

	pBuffer->iSize = _iSize;
	pBuffer->iRegisterSlot = _iRegister;
	pBuffer->iConstant = _iConstantShader;
	pBuffer->strName = _strName;

	pBuffer->pData = new char[_iSize];

	D3D11_BUFFER_DESC	tDesc = {};
	tDesc.ByteWidth = _iSize;
	tDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tDesc.Usage = D3D11_USAGE_DYNAMIC;
	tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(_DEVICE->CreateBuffer(&tDesc, nullptr, &pBuffer->pBuffer)))
	{
		SAFE_DELETE_ARRAY(pBuffer->pData);
		SAFE_DELETE(pBuffer);
		return false;
	}

	m_mapConstBuffer.insert(std::make_pair(_strName, pBuffer));

	return true;
}

bool ShaderManager::UpdateConstBuffer(const std::string & _strName, void * _pData)
{
	PConstBuffer	pBuffer = FindConstBuffer(_strName);

	// �ش��ϴ� ������۰� ���ٸ� ������Ʈ�� �ʿ䰡�����Ƿ� return false
	if (nullptr == pBuffer)
	{
		assert(false);
		return false;
	}

	 // ������۸� ������Ʈ�ϱ� ���ؼ� �����͸� ä������Ѵ�. 
	// map, unmap���, -> lock, unlcokó�� �����ϸ�ȴ�. + map�� ������ �ݵ�� unmap�� �ؾ��Ѵ�.
	// + ���� map unmap�� ����ϴ� ������ �ش� ������۸� ���鶧 dynamic���� �����߱� �����̴�. ������
	// ������۷δ� ��뷮�� �����͵��� ��������.
	// ������ ���뷮�̱⿡ ������۷� �������� ���� ��������� �Ѱ�뷮�� ����� �����͵��� �������� ��쿡��
	// �ؽ�ó�� �̿��ؾ��Ѵ�. ( VTF )
	D3D11_MAPPED_SUBRESOURCE	tMap = {};

	// Map(����۸� ����Ұ��� (Resource��� ���۳� �ؽ�ó),  ���ҽ�����, CPU�� �б� �� ���� ��������, �Ӹʼ���, tMap)
	_CONTEXT->Map(pBuffer->pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tMap);

	memcpy(tMap.pData, _pData, pBuffer->iSize);

	_CONTEXT->Unmap(pBuffer->pBuffer, 0);

	// ���ŵ� ������۸� �Ѱ��ֱ�
	if (pBuffer->iConstant & CS_VERTEX)
	{
		// 1�� ���� �����ε� ������
		_CONTEXT->VSSetConstantBuffers(pBuffer->iRegisterSlot, 1, &pBuffer->pBuffer);
	}

	if (pBuffer->iConstant & CS_PIXEL)
	{
		_CONTEXT->PSSetConstantBuffers(pBuffer->iRegisterSlot, 1, &pBuffer->pBuffer);
	}

	if (pBuffer->iConstant & CS_GEOMETRY)
	{
		_CONTEXT->GSSetConstantBuffers(pBuffer->iRegisterSlot, 1, &pBuffer->pBuffer);
	}

	if (pBuffer->iConstant & CS_COMPUTE)
	{
		_CONTEXT->CSSetConstantBuffers(pBuffer->iRegisterSlot, 1, &pBuffer->pBuffer);
	}

	return true;
}

PConstBuffer ShaderManager::FindConstBuffer(const std::string & _strName)
{
	std::unordered_map<std::string, PConstBuffer>::iterator FindBuffer = m_mapConstBuffer.find(_strName);

	if (FindBuffer == m_mapConstBuffer.end())
	{
		return nullptr;
	}

	return FindBuffer->second;
}


