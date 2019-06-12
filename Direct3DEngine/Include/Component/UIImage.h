#pragma once
#include "UI.h"

ENGINE_BEGIN

class UIImage : public UI
{
private:
	friend class GameObject;

protected:
	UIImage();
	UIImage(const UIImage& _Com);
	virtual ~UIImage();

protected:
	class Texture*		m_pTexture;			// 해당 이미지 텍스처  ( 이미지니까 텍스처 1개 )
	Vector4				m_vColor;			// 색상
	ImageCBuffer		m_tImageCBuffer;	// 색상값 상수 버퍼
	class Material*		m_pMaterial;
	class Renderer*		m_pRenderer;

	//std::function<void(float)> m_CallBackFunc;		// 함수

public:
	// 이미지 색상 셋팅
	void SetColor(const Vector4& _vColor);

	// 이미지 텍스처 셋팅
	void SetTexture(const std::string& _strName, const TCHAR* _pFileName, const std::string& _strPathName = PATH_TEXTURE);

	// 셰이더 셋팅, 블렌드 스테이트 셋팅
	void SetShader(const std::string& _strKey = SHADER_UI_IMAGE);
	void SetBlendState(const std::string& _strKey = "AlphaBlend");

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출, 즉 레이어에서 호출해준다.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// 렌더를 하기 직전에 호출해주는 함수ㅔㄱ
	virtual int Render(float _fTime);
	virtual UIImage* Clone()	const;

private:
	void DefaultInit();			// 디폴트값

//public:
//	void SetCallBackFunc(void(*_pFunc)(float));
//
//	template <typename FuncType>
//	void SetCallBackFunc(FuncType* _pObejct, void(FuncType::*_pFunc)(float))
//	{
//		m_CallBackFunc = std::binary_function(_pFunc, _pObejct, std::placeholders::_1);
//	}
};

ENGINE_END