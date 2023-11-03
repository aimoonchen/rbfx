#include "Common.h"
#include "EffekseerUrho3D.RendererImplemented.h"
#include "GraphicsDevice.h"

#include "../../Graphics/Graphics.h"
#include "../../RenderAPI/RenderDevice.h"
#include "../../RenderAPI/DrawCommandQueue.h"

namespace EffekseerRenderer
{

// void ChangeRenderPassPipelineState(EffekseerRenderer::RendererRef renderer, RenderPassPipelineStateKey key)
// {
// 	auto r = renderer.DownCast<EffekseerUrho3D::RendererImplemented>();
// 
// 	LLGI::RenderPassPipelineStateKey llgiKey;
// 
// 	llgiKey.RenderTargetFormats.resize(key.RenderTargetCount);
// 
// 	for (size_t i = 0; i < llgiKey.RenderTargetFormats.size(); i++)
// 	{
// 		llgiKey.RenderTargetFormats.at(i) = EffekseerUrho3D::ConvertTextureFormat(key.RenderTargetFormats[i]);
// 	}
// 	llgiKey.DepthFormat = EffekseerUrho3D::ConvertTextureFormat(key.DepthFormat);
// 
// 	r->ChangeRenderPassPipelineState(llgiKey);
// }
// 
// void FlushAndWait(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice)
// {
// 	auto gd = static_cast<::EffekseerUrho3D::Backend::GraphicsDevice*>(graphicsDevice.Get());
// 	auto g = static_cast<Urho3D::Graphics*>(gd->GetGraphics());
// 	g->WaitFinish();
// }

Effekseer::RefPtr<EffekseerRenderer::CommandList> CreateCommandList(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
																	Effekseer::RefPtr<::EffekseerRenderer::SingleFrameMemoryPool> memoryPool)
{
	auto gd = static_cast<::EffekseerUrho3D::Backend::GraphicsDevice*>(graphicsDevice.Get());
	auto g = static_cast<Urho3D::Graphics*>(gd->GetGraphics());
// 	auto mp = static_cast<::EffekseerUrho3D::SingleFrameMemoryPool*>(memoryPool.Get());
// 	auto commandList = g->CreateCommandList(mp->GetInternal());
    auto drawQueue = g->GetSubsystem<Urho3D::RenderDevice>()->GetDefaultQueue();
    drawQueue->Reset();
	auto ret = Effekseer::MakeRefPtr<EffekseerUrho3D::CommandList>(g, drawQueue/*, mp->GetInternal()*/);
//	ES_SAFE_RELEASE(commandList);
	return ret;
}

// Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> CreateSingleFrameMemoryPool(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice)
// {
// 	auto gd = static_cast<::EffekseerUrho3D::Backend::GraphicsDevice*>(graphicsDevice.Get());
// 	auto g = static_cast<Urho3D::Graphics*>(gd->GetGraphics());
// 	auto mp = g->CreateSingleFrameMemoryPool(1024 * 1024 * 8, 128);
// 	auto ret = Effekseer::MakeRefPtr<EffekseerUrho3D::SingleFrameMemoryPool>(mp);
// 	ES_SAFE_RELEASE(mp);
// 	return ret;
// }

} // namespace EffekseerRenderer
