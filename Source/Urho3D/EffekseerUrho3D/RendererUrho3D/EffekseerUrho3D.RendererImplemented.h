#pragma once
#include "../EffekseerRendererCommon/EffekseerRenderer.CommonUtils.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.StandardRenderer.h"
#include "EffekseerUrho3D.Base.h"
#include "EffekseerUrho3D.Renderer.h"
#include "../../RenderPipeline/RenderPipelineDefs.h"

namespace Diligent
{
    struct IPipelineState;
    struct IShaderResourceVariable;
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

struct SRBInfo {
    Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> shaderResourceBinding;
    //std::unordered_map<std::string, Diligent::IShaderResourceVariable*> shaderResourceVariables;
    std::vector<Diligent::IShaderResourceVariable*> shaderResourceVariables;
};

Urho3D::TextureFormat ConvertTextureFormat(Effekseer::Backend::TextureFormatType format);

class RendererImplemented : public Renderer, public ::Effekseer::ReferenceObject
{
protected:
	std::map<PiplineStateKey, Diligent::RefCntAutoPtr<Diligent::IPipelineState>> piplineStates_;
    std::map<Diligent::IPipelineState*, SRBInfo> shaderResourceBindings_;
    Diligent::IBuffer* currentVertexBuffer_{ nullptr };
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

    Backend::IndexBuffer* currentIndexBuffer_{ nullptr };
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

    ::Effekseer::CompiledMaterialPlatformType platformType_{ Effekseer::CompiledMaterialPlatformType::DirectX12 };

	::Effekseer::MaterialCompiler* materialCompiler_ = nullptr;

	RendererImplemented(int32_t squareMaxCount);

	~RendererImplemented();

	void OnLostDevice() override;

	void OnResetDevice() override;

	bool Initialize(Backend::GraphicsDeviceRef graphicsDevice, /*LLGI::RenderPassPipelineStateKey key, */bool isReversedDepth);

	bool Initialize(Urho3D::RenderDevice* renderDevice, /*LLGI::RenderPassPipelineStateKey key, */bool isReversedDepth);

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

	Urho3D::RenderDevice* GetRenderDevice() const override
	{
		return graphicsDevice_->GetRenderDevice();
	}

	Effekseer::Backend::IndexBufferRef GetIndexBuffer();
	int32_t GetSquareMaxCount() const override;
	::EffekseerRenderer::RenderStateBase* GetRenderState();
	::Effekseer::SpriteRendererRef CreateSpriteRenderer() override;
	::Effekseer::RibbonRendererRef CreateRibbonRenderer() override;
	::Effekseer::RingRendererRef CreateRingRenderer() override;
	::Effekseer::ModelRendererRef CreateModelRenderer() override;
	::Effekseer::TrackRendererRef CreateTrackRenderer() override;
	::Effekseer::TextureLoaderRef CreateTextureLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr) override;
	::Effekseer::ModelLoaderRef CreateModelLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr) override { return nullptr; };
    ::Effekseer::MaterialLoaderRef CreateMaterialLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr) override;

	void SetBackgroundInternal(Urho3D::Texture2D* background);

	EffekseerRenderer::DistortingCallback* GetDistortingCallback() override;

	void SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback) override;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>* GetStandardRenderer()
	{
		return m_standardRenderer;
	}

	void SetVertexBuffer(Diligent::IBuffer* vertexBuffer, int32_t stride);

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

    //void ResetStateForDefferedContext() override;

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
    void CommitUniformAndTextures(Diligent::IPipelineState* pipelineState);
};

void AddVertexUniformLayout(Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout);

void AddPixelUniformLayout(Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout);

void AddDistortionPixelUniformLayout(Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout);

Effekseer::CustomVector<Effekseer::CustomString<char>> GetTextureLocations(EffekseerRenderer::RendererShaderType type);

} // namespace EffekseerUrho3D
