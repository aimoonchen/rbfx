
#ifndef __EFFEKSEERRENDERER_LLGI_RENDERER_IMPLEMENTED_H__
#define __EFFEKSEERRENDERER_LLGI_RENDERER_IMPLEMENTED_H__

#include "../EffekseerRendererCommon/EffekseerRenderer.CommonUtils.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.StandardRenderer.h"
#include "EffekseerUrho3D.Base.h"
#include "EffekseerUrho3D.Renderer.h"
// #include <LLGI.CommandList.h>
// #include <LLGI.Graphics.h>
// #include <LLGI.PipelineState.h>
// #include <LLGI.Texture.h>
#include "../../RenderPipeline/RenderPipelineDefs.h"

namespace Diligent
{
    struct IPipelineState;
}

namespace EffekseerUrho3D
{

using Vertex = EffekseerRenderer::SimpleVertex;
// using VertexDistortion = EffekseerRenderer::VertexDistortion;

class PiplineStateKey
{
public:
	Shader* shader = nullptr;
	EffekseerRenderer::RenderStateBase::State state;
	Diligent::PRIMITIVE_TOPOLOGY topologyType;
	//LLGI::RenderPassPipelineState* renderPassPipelineState = nullptr;
	bool operator<(const PiplineStateKey& v) const;
};

Urho3D::TextureFormat ConvertTextureFormat(Effekseer::Backend::TextureFormatType format);

class RendererImplemented : public Renderer, public ::Effekseer::ReferenceObject
{
protected:
	std::map<PiplineStateKey, Diligent::RefCntAutoPtr<Diligent::IPipelineState>> piplineStates_;
	Urho3D::VertexBuffer* currentVertexBuffer_{ nullptr };
	int32_t currentVertexBufferStride_ = 0;
	Diligent::PRIMITIVE_TOPOLOGY currentTopologyType_ = Diligent::PRIMITIVE_TOPOLOGY::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	//std::unordered_map<LLGI::RenderPassPipelineStateKey, std::shared_ptr<LLGI::RenderPassPipelineState>, LLGI::RenderPassPipelineStateKey::Hash> renderpassPipelineStates_;

	// TODO
	/**
		Create constants and copy
		Shader
	*/

	Backend::GraphicsDeviceRef graphicsDevice_ = nullptr;
    Diligent::IDeviceContext* deviceContext_{ nullptr };
	//std::shared_ptr<LLGI::RenderPassPipelineState> currentRenderPassPipelineState_ = nullptr;

    Urho3D::IndexBuffer* currentIndexBuffer_{ nullptr };
	Effekseer::Backend::IndexBufferRef indexBuffer_;
	Effekseer::Backend::IndexBufferRef indexBufferForWireframe_;
	int32_t m_squareMaxCount;
	Shader* currentShader = nullptr;

	bool isReversedDepth_ = false;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>* m_standardRenderer;

	::Effekseer::CoordinateSystem m_coordinateSystem;

	::EffekseerRenderer::RenderStateBase* m_renderState;

	EffekseerRenderer::DistortingCallback* m_distortingCallback;

	::Effekseer::Backend::TextureRef m_backgroundLLGI;

    std::vector<::Effekseer::Backend::TextureRef> m_currentTextures_;

	Effekseer::RenderMode m_renderMode = Effekseer::RenderMode::Normal;

	Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList_ = nullptr;

	Urho3D::DrawCommandQueue* GetCurrentCommandList();

    Diligent::IPipelineState* GetOrCreatePiplineState();

public:
	//! shaders (todo implemented)
	FixedShader fixedShader_;

	::Effekseer::CompiledMaterialPlatformType platformType_;

	::Effekseer::MaterialCompiler* materialCompiler_ = nullptr;

	RendererImplemented(int32_t squareMaxCount);

	~RendererImplemented();

	void OnLostDevice() override;

	void OnResetDevice() override;

	bool Initialize(Backend::GraphicsDeviceRef graphicsDevice, /*LLGI::RenderPassPipelineStateKey key, */bool isReversedDepth);

	bool Initialize(Urho3D::Graphics* graphics, /*LLGI::RenderPassPipelineStateKey key, */bool isReversedDepth);

	void SetRestorationOfStatesFlag(bool flag) override;

	//void ChangeRenderPassPipelineState(LLGI::RenderPassPipelineStateKey key);

	bool BeginRendering() override;

	bool EndRendering() override;

	void SetCommandList(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList) override;

	Effekseer::Backend::GraphicsDeviceRef GetGraphicsDevice() const override
	{
		return graphicsDevice_;
	}

	Backend::GraphicsDeviceRef& GetGraphicsDeviceInternal()
	{
		return graphicsDevice_;
	}

	Urho3D::Graphics* GetGraphics() const override
	{
		return graphicsDevice_->GetGraphics();
	}

	Effekseer::Backend::IndexBufferRef GetIndexBuffer();

	/**
		@brief	最大描画スプライト数
	*/
	int32_t GetSquareMaxCount() const override;

	::EffekseerRenderer::RenderStateBase* GetRenderState();

	/**
		@brief	スプライトレンダラーを生成する。
	*/
	::Effekseer::SpriteRendererRef CreateSpriteRenderer() override;

	/**
		@brief	リボンレンダラーを生成する。
	*/
	::Effekseer::RibbonRendererRef CreateRibbonRenderer() override;

	/**
		@brief	リングレンダラーを生成する。
	*/
	::Effekseer::RingRendererRef CreateRingRenderer() override;

	/**
		@brief	モデルレンダラーを生成する。
	*/
	::Effekseer::ModelRendererRef CreateModelRenderer() override;

	/**
		@brief	軌跡レンダラーを生成する。
	*/
	::Effekseer::TrackRendererRef CreateTrackRenderer() override;

	/**
		@brief	テクスチャ読込クラスを生成する。
	*/
	::Effekseer::TextureLoaderRef CreateTextureLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr) override { return nullptr; };

	/**
		@brief	モデル読込クラスを生成する。
	*/
	::Effekseer::ModelLoaderRef CreateModelLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr) override { return nullptr; };

    ::Effekseer::MaterialLoaderRef CreateMaterialLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr) override { return nullptr; };

	void SetBackgroundInternal(Urho3D::Texture2D* background);

	EffekseerRenderer::DistortingCallback* GetDistortingCallback() override;

	void SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback) override;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>* GetStandardRenderer()
	{
		return m_standardRenderer;
	}

	void SetVertexBuffer(Urho3D::VertexBuffer* vertexBuffer, int32_t stride);

	void SetVertexBuffer(const Effekseer::Backend::VertexBufferRef& vertexBuffer, int32_t stride);
	void SetIndexBuffer(const Effekseer::Backend::IndexBufferRef& indexBuffer);

	void SetLayout(Shader* shader);
	void DrawSprites(int32_t spriteCount, int32_t vertexOffset);
	void DrawPolygon(int32_t vertexCount, int32_t indexCount);
	void DrawPolygonInstanced(int32_t vertexCount, int32_t indexCount, int32_t instanceCount);

	void BeginShader(Shader* shader);
	void EndShader(Shader* shader);

	void SetVertexBufferToShader(const void* data, int32_t size, int32_t dstOffset);

	void SetPixelBufferToShader(const void* data, int32_t size, int32_t dstOffset);

	void SetTextures(Shader* shader, Effekseer::Backend::TextureRef* textures, int32_t count);

	void ResetRenderState() override;

	virtual int GetRef() override
	{
		return ::Effekseer::ReferenceObject::GetRef();
	}
	virtual int AddRef() override
	{
		return ::Effekseer::ReferenceObject::AddRef();
	}
	virtual int Release() override
	{
		return ::Effekseer::ReferenceObject::Release();
	}
    // TODO: for rbfx
    void CommitUniformAndTextures();
};

void AddVertexUniformLayout(Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout);

void AddPixelUniformLayout(Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout);

void AddDistortionPixelUniformLayout(Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout);

Effekseer::CustomVector<Effekseer::CustomString<char>> GetTextureLocations(EffekseerRenderer::RendererShaderType type);

} // namespace EffekseerUrho3D

#endif // __EFFEKSEERRENDERER_LLGI_RENDERER_IMPLEMENTED_H__
