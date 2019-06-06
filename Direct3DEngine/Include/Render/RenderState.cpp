#include "RenderState.h"

ENGINE_USING

RenderState::RenderState()
	: m_pCurState(nullptr), m_pOldState(nullptr)
	, m_eType(RS_END)
{
}


RenderState::~RenderState()
{
	SAFE_RELEASE(m_pOldState);
	SAFE_RELEASE(m_pCurState);
}

RENDER_STATE RenderState::GetRenderStateType() const
{
	return m_eType;
}
