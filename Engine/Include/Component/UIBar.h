#pragma once
#include "UI.h"

ENGINE_BEGIN

// 바는 단순히 뒷 텍스처와 앞 텍스처가 있으면 된다.
// 앞텍스처로 조절한다. (크기)

class ENGINE_DLL UIBar : public UI
{
private:
	friend class GameObject;

protected:
	UIBar();
	UIBar(const UIBar& _Com);
	virtual ~UIBar();

protected:
	float m_fRatio;					// 비율
	Vector4 m_vColor;				// 색상
	BarCBuffer m_tBarCBuffer;

	class Texture*		m_pTexture;
	class Material*		m_pMaterial;
	class Renderer*		m_pRenderer;

public:
	void SetColor(const Vector4& _vColor);
	void SetRatio(const float& _fRatio);
	void SetTexture(const std::string& _strName, const TCHAR* _pFileName, const std::string& _strPathName = PATH_TEXTURE);
	void SetShader(const std::string& _strKey);
	void SetRenderState(const std::string& _strKey);

public:
	virtual void Start();				// 배치될려는 레이어에 포함될때 (배치될때) 호출, 즉 레이어에서 호출해준다.
	virtual bool Init();
	virtual int Input(float _fTime);
	virtual int Update(float _fTime);
	virtual int LateUpdate(float _fTime);
	virtual int Collision(float _fTime);
	virtual int PrevRender(float _fTime);	// 렌더를 하기 직전에 호출해주는 함수ㅔㄱㅁ
	virtual int Render(float _fTime);
	virtual UIBar* Clone()	const;

private:
	void DefaultBarInit();

};

ENGINE_END

