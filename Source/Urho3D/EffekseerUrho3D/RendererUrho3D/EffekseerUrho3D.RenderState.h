
#ifndef __EFFEKSEERRENDERER_LLGI_RENDERSTATE_H__
#define __EFFEKSEERRENDERER_LLGI_RENDERSTATE_H__

#include "../EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h"
#include "EffekseerUrho3D.Base.h"
#include "EffekseerUrho3D.RendererImplemented.h"

namespace EffekseerUrho3D
{

class RenderState : public ::EffekseerRenderer::RenderStateBase
{
private:
	static const int32_t DepthTestCount = 2;
	static const int32_t DepthWriteCount = 2;
	static const int32_t CulTypeCount = 3;
	static const int32_t AlphaTypeCount = 5;
	static const int32_t TextureFilterCount = 2;
	static const int32_t TextureWrapCount = 2;

	RendererImplemented* m_renderer;

public:
	RenderState(RendererImplemented* renderer);
	virtual ~RenderState();

	void Update(bool forced);
};

} // namespace EffekseerUrho3D

#endif // __EFFEKSEERRENDERER_LLGI_RENDERSTATE_H__
