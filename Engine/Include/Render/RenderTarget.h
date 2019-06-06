#pragma once
#include "..\Engine.h"

ENGINE_BEGIN

class ENGINE_DLL RenderTarget
{
private:
	friend class RenderManager;

public:
	RenderTarget();
	~RenderTarget();

private:
	std::string					m_strName;				// 렌더타겟 이름
	float						m_fCelarColor[4];		// 지워줄 컬러 (텍스처에 그려진걸 지울거임)

	ID3D11RenderTargetView*		m_pRenderTargetView;	// 렌더타겟 (바꿀녀석)
	ID3D11Texture2D*			m_pRenderTargetTexture;	// 텍스처
	ID3D11ShaderResourceView*	m_pRenderTargetSRV;		// 렌더타겟용 셰이더리소스뷰 (셰이더로 넘기기 위해 필요)

	ID3D11DepthStencilView*		m_pDepthView;			// 깊이뷰
	ID3D11Texture2D*			m_pDepthTexture;		// 깊이 텍스처

private:
	ID3D11RenderTargetView*		m_pOldRenderTargetView;	// 이전 렌더타겟뷰,  렌더타겟을 바꾼후에 기존의 렌더타겟으로 바꿔줘야하기때문
	ID3D11DepthStencilView*		m_pOldDepthView;

public:
	ID3D11RenderTargetView*	GetRenderTargetView()	const;
	ID3D11DepthStencilView*	GetDepthStencilView()	const;
	ID3D11ShaderResourceView* GetShaderResourceView() const;

private:
	Vector3			m_vPosition;
	Vector3			m_vScale;
	bool			m_bDebug;			// 디버그 여부
	Matrix*			m_pProj;			// 프로젝션 행렬 (투영행렬)
	class Mesh*		m_pMesh;			// 디버그용 렌더타겟을 띄우기 위한 메쉬
	class Shader*	m_pShader;			// 디버그용 사용할 셰이더 
	ID3D11InputLayout*	m_pLayout;		// 디버그용 레이아웃

public:
	// 렌더타겟을 만들 함수 ( 디퍼드 렌더링시 꼭 필요함 )
	// MRT시 필요  ( Multi Render Target )
	bool CreateRenderTarget(const std::string& _strName,
		UINT _iWidth, UINT _iHeight, DXGI_FORMAT _eForamt,
		float _fClearColor[4], int _iSampleCount = 1,
		DXGI_FORMAT _eDepthFormat = DXGI_FORMAT_UNKNOWN);		// 뎁스의 디폴트값은 사용하지 않겠다는말임 ㅇㅅㅇ

	void OnDebug(const Vector3& _vPos, const Vector3& _vScale, bool _bDebug = true);
	void ClearRenderTarget();				// 지우기
	void SetRenderTarget();					// 렌더 타겟 셋팅하기 (바꿔주기)
	void ResetRenderTarget();				// 렌더타겟 원래대로 되돌리기

	void SetShader(int _iRegister);			// 셰이더 셋팅해주기
	void SetCShader(int _iRegister);
	void ResetShader(int _iRegister);		// 셰이더를 사용한 후에 다시 되돌리기
	void ResetCShader(int _iRegister);
	void Render();

};

ENGINE_END

