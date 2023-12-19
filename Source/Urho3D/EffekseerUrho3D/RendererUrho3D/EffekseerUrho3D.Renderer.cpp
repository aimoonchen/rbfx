

#include "EffekseerUrho3D.Renderer.h"
#include "EffekseerUrho3D.RenderState.h"
#include "EffekseerUrho3D.RendererImplemented.h"

#include "EffekseerUrho3D.MaterialLoader.h"
#include "EffekseerUrho3D.ModelRenderer.h"
#include "EffekseerUrho3D.Shader.h"
#include "EffekseerUrho3D.RenderResources.h"

#include "../EffekseerRendererCommon/EffekseerRenderer.RibbonRendererBase.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.RingRendererBase.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.SpriteRendererBase.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.TrackRendererBase.h"
#include "../EffekseerRendererCommon/ModelLoader.h"
#include "../EffekseerRendererCommon/TextureLoader.h"
#include "../EffekseerMaterialCompiler/OpenGL/EffekseerMaterialCompilerGL.h"
//#include <iostream>
#include <Diligent/Graphics/GraphicsEngine/interface/RenderDevice.h>
#include "../../Graphics/GraphicsUtils.h"
#include "../../RenderAPI/PipelineState.h"
#include "../../RenderAPI/RenderDevice.h"
#include "../../RenderAPI/RenderContext.h"
#include "../../Graphics/Graphics.h"
#include "../../Graphics/Texture2D.h"
#include "../../RenderPipeline/StaticPipelineStateCache.h"

namespace EffekseerUrho3D
{

bool PiplineStateKey::operator<(const PiplineStateKey& v) const
{
	if (shader != v.shader)
		return shader < v.shader;
	if (state.AlphaBlend != v.state.AlphaBlend)
		return state.AlphaBlend < v.state.AlphaBlend;
	if (state.CullingType != v.state.CullingType)
		return state.CullingType < v.state.CullingType;
	if (state.DepthTest != v.state.DepthTest)
		return v.state.DepthTest;
	if (state.DepthWrite != v.state.DepthWrite)
		return v.state.DepthWrite;

	for (int i = 0; i < 4; i++)
	{
		if (state.TextureFilterTypes[i] != v.state.TextureFilterTypes[i])
			return state.TextureFilterTypes[i] < v.state.TextureFilterTypes[i];
		if (state.TextureWrapTypes[i] != v.state.TextureWrapTypes[i])
			return state.TextureWrapTypes[i] < v.state.TextureWrapTypes[i];
	}

	if (topologyType != v.topologyType)
		return topologyType < v.topologyType;

// 	if (renderPassPipelineState != v.renderPassPipelineState)
// 		return renderPassPipelineState < v.renderPassPipelineState;

	return false;
}

Urho3D::TextureFormat ConvertTextureFormat(Effekseer::Backend::TextureFormatType format)
{
	switch (format)
	{
	case Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM:
		return Urho3D::TextureFormat::TEX_FORMAT_RGBA8_UNORM;
		break;
	case Effekseer::Backend::TextureFormatType::B8G8R8A8_UNORM:
		return Urho3D::TextureFormat::TEX_FORMAT_BGRA8_UNORM;
		break;
	case Effekseer::Backend::TextureFormatType::R8_UNORM:
		return Urho3D::TextureFormat::TEX_FORMAT_R8_UNORM;
		break;
	case Effekseer::Backend::TextureFormatType::R16G16_FLOAT:
		return Urho3D::TextureFormat::TEX_FORMAT_RG16_FLOAT;
		break;
	case Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT:
		return Urho3D::TextureFormat::TEX_FORMAT_RGBA16_FLOAT;
		break;
	case Effekseer::Backend::TextureFormatType::R32G32B32A32_FLOAT:
		return Urho3D::TextureFormat::TEX_FORMAT_RGBA32_FLOAT;
		break;
	case Effekseer::Backend::TextureFormatType::BC1:
		return Urho3D::TextureFormat::TEX_FORMAT_BC1_UNORM;
		break;
	case Effekseer::Backend::TextureFormatType::BC2:
		return Urho3D::TextureFormat::TEX_FORMAT_BC2_UNORM;
		break;
	case Effekseer::Backend::TextureFormatType::BC3:
		return Urho3D::TextureFormat::TEX_FORMAT_BC3_UNORM;
		break;
	case Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM_SRGB:
		return Urho3D::TextureFormat::TEX_FORMAT_RGBA8_UNORM_SRGB;
		break;
	case Effekseer::Backend::TextureFormatType::B8G8R8A8_UNORM_SRGB:
		return Urho3D::TextureFormat::TEX_FORMAT_BGRA8_UNORM_SRGB;
		break;
	case Effekseer::Backend::TextureFormatType::BC1_SRGB:
		return Urho3D::TextureFormat::TEX_FORMAT_BC1_UNORM_SRGB;
		break;
	case Effekseer::Backend::TextureFormatType::BC2_SRGB:
		return Urho3D::TextureFormat::TEX_FORMAT_BC2_UNORM_SRGB;
		break;
	case Effekseer::Backend::TextureFormatType::BC3_SRGB:
		return Urho3D::TextureFormat::TEX_FORMAT_BC3_UNORM_SRGB;
		break;
	case Effekseer::Backend::TextureFormatType::D32:
		return Urho3D::TextureFormat::TEX_FORMAT_D32_FLOAT;
		break;
	case Effekseer::Backend::TextureFormatType::D24S8:
		return Urho3D::TextureFormat::TEX_FORMAT_D24_UNORM_S8_UINT;
		break;
	case Effekseer::Backend::TextureFormatType::D32S8:
		return Urho3D::TextureFormat::TEX_FORMAT_D32_FLOAT_S8X24_UINT;
		break;
	case Effekseer::Backend::TextureFormatType::Unknown:
		break;
	default:
		break;
	}

	return Urho3D::TextureFormat::TEX_FORMAT_UNKNOWN;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererRef Renderer::Create(Urho3D::RenderDevice* renderDevice, int32_t squareMaxCount, bool isReversedDepth)
{
    auto renderer = Effekseer::MakeRefPtr<RendererImplemented>(squareMaxCount);
    renderer->materialCompiler_ = new Effekseer::MaterialCompilerGL(renderDevice->GetBackend() == Urho3D::RenderBackend::Vulkan);
    if (renderer->Initialize(renderDevice, isReversedDepth))
    {
        return renderer;
    }
    return nullptr;
}

Urho3D::DrawCommandQueue* RendererImplemented::GetCurrentCommandList()
{
	if (commandList_ != nullptr)
	{
		auto cl = commandList_.DownCast<CommandList>();
		return cl->GetInternal();
	}

	assert(0);
	return nullptr;
}
// TODO: remove this fuction, implemented in EffekseerRenderer::StandardRenderer
static int32_t GetCurrentStride(const EffekseerRenderer::StandardRendererState& state)
{
    const auto renderingMode = state.Collector.ShaderType;
    size_t stride = 0;
    if (renderingMode == EffekseerRenderer::RendererShaderType::Material)
    {
        stride = sizeof(EffekseerRenderer::DynamicVertex);
        stride += (state.CustomData1Count + state.CustomData2Count) * sizeof(float);
    }
    else if (renderingMode == EffekseerRenderer::RendererShaderType::Lit)
    {
        stride = sizeof(EffekseerRenderer::LightingVertex);
    }
    else if (renderingMode == EffekseerRenderer::RendererShaderType::BackDistortion)
    {
        stride = sizeof(EffekseerRenderer::LightingVertex);
    }
    else if (renderingMode == EffekseerRenderer::RendererShaderType::Unlit)
    {
        stride = sizeof(EffekseerRenderer::SimpleVertex);
    }
    else if (renderingMode == EffekseerRenderer::RendererShaderType::AdvancedLit)
    {
        stride = sizeof(EffekseerRenderer::AdvancedLightingVertex);
    }
    else if (renderingMode == EffekseerRenderer::RendererShaderType::AdvancedBackDistortion)
    {
        stride = sizeof(EffekseerRenderer::AdvancedLightingVertex);
    }
    else if (renderingMode == EffekseerRenderer::RendererShaderType::AdvancedUnlit)
    {
        stride = sizeof(EffekseerRenderer::AdvancedSimpleVertex);
    }

    return static_cast<int32_t>(stride);
}
Diligent::IPipelineState* RendererImplemented::GetOrCreatePiplineState()
{
    PiplineStateKey key;
    key.state = m_renderState->GetActiveState();
    key.shader = currentShader;
    key.topologyType = currentTopologyType_;
    // key.renderPassPipelineState = currentRenderPassPipelineState_.get();

    auto it = piplineStates_.find(key);
    if (it != piplineStates_.end())
    {
        return it->second;
    }

    Diligent::GraphicsPipelineStateCreateInfo PSOCreateInfo;

    // Pipeline state name is used by the engine to report issues.
    // It is always a good idea to give objects descriptive names.
    PSOCreateInfo.PSODesc.Name = "Effekseer PSO";

    // This is a graphics pipeline
    PSOCreateInfo.PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;

    auto renderDevice = graphicsDevice_->GetRenderDevice();
    auto swapChain = renderDevice->GetSwapChain();
    // This tutorial will render to a single render target
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
    // Set render target format which is the format of the swap chain's color buffer
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = swapChain->GetDesc().ColorBufferFormat;
    // Use the depth buffer format from the swap chain
    PSOCreateInfo.GraphicsPipeline.DSVFormat = swapChain->GetDesc().DepthBufferFormat;
    
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = currentTopologyType_;

    auto& rasterizerDesc = PSOCreateInfo.GraphicsPipeline.RasterizerDesc;
    rasterizerDesc.ScissorEnable = Diligent::False;

    if (key.state.CullingType == ::Effekseer::CullingType::Front) {
        rasterizerDesc.CullMode = isReversedDepth_ ? Diligent::CULL_MODE_FRONT : Diligent::CULL_MODE_BACK;
    }
    else if (key.state.CullingType == ::Effekseer::CullingType::Back) {
        rasterizerDesc.CullMode = isReversedDepth_ ? Diligent::CULL_MODE_BACK : Diligent::CULL_MODE_FRONT;
    }
    else if (key.state.CullingType == ::Effekseer::CullingType::Double) {
        rasterizerDesc.CullMode = Diligent::CULL_MODE_NONE;
    }

    // Depth testing
    auto& depthStencilDesc = PSOCreateInfo.GraphicsPipeline.DepthStencilDesc;
    depthStencilDesc.DepthEnable = key.state.DepthTest;
    depthStencilDesc.DepthWriteEnable = key.state.DepthWrite;
    depthStencilDesc.DepthFunc = isReversedDepth_ ? Diligent::COMPARISON_FUNC_GREATER_EQUAL : Diligent::COMPARISON_FUNC_LESS_EQUAL;

    PSOCreateInfo.pVS = currentShader->GetVertexShader();
    PSOCreateInfo.pPS = currentShader->GetPixelShader();

    const auto& elements = currentShader->GetVertexLayouts()->GetElements();
    auto& inputLayout = PSOCreateInfo.GraphicsPipeline.InputLayout;
    inputLayout.NumElements = elements.size();
    inputLayout.LayoutElements = elements.data();

    ea::vector<Diligent::ShaderResourceVariableDesc> Vars;
    ea::vector<Diligent::ImmutableSamplerDesc> ImtblSamplers;
    const auto& uniformLayout = currentShader->GetUniformLayout();
    if (uniformLayout != nullptr) {
        Diligent::TEXTURE_ADDRESS_MODE ws[2]{};
        ws[(int)Effekseer::TextureWrapType::Clamp] = Diligent::TEXTURE_ADDRESS_MODE::TEXTURE_ADDRESS_CLAMP;
        ws[(int)Effekseer::TextureWrapType::Repeat] = Diligent::TEXTURE_ADDRESS_MODE::TEXTURE_ADDRESS_WRAP;

        Diligent::FILTER_TYPE fs[2]{};
        fs[(int)Effekseer::TextureFilterType::Linear] = Diligent::FILTER_TYPE::FILTER_TYPE_LINEAR;
        fs[(int)Effekseer::TextureFilterType::Nearest] = Diligent::FILTER_TYPE::FILTER_TYPE_POINT;

        const auto& samplerNames = uniformLayout->GetTextures();
        auto samplerCount = samplerNames.size();
        const auto& state = key.state;
        Vars.reserve(samplerCount);
        ImtblSamplers.reserve(samplerCount);
        for (int i = 0; i < samplerCount; i++) {
            auto filterMode = fs[(int)state.TextureFilterTypes[i]];
            auto addressMode = ws[(int)state.TextureWrapTypes[i]];
            Vars.emplace_back(Diligent::SHADER_TYPE_PIXEL, samplerNames[i].c_str(), Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC/*Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE*/);
            ImtblSamplers.emplace_back(Diligent::SHADER_TYPE_PIXEL, samplerNames[i].c_str(),
                Diligent::SamplerDesc{ filterMode, filterMode, filterMode, addressMode, addressMode, addressMode }
            );
        }
    }
    auto& resourceLayout = PSOCreateInfo.PSODesc.ResourceLayout;
    resourceLayout.DefaultVariableType = Diligent::SHADER_RESOURCE_VARIABLE_TYPE_STATIC; 
    resourceLayout.Variables = Vars.data();
    resourceLayout.NumVariables = Vars.size();
    resourceLayout.ImmutableSamplers = ImtblSamplers.data();
    resourceLayout.NumImmutableSamplers = ImtblSamplers.size();

    auto& blendDesc = PSOCreateInfo.GraphicsPipeline.BlendDesc.RenderTargets[0];
    blendDesc.BlendEnable = true;
    blendDesc.SrcBlendAlpha = Diligent::BLEND_FACTOR_ONE;
    blendDesc.DestBlendAlpha = Diligent::BLEND_FACTOR_ONE;
    blendDesc.BlendOpAlpha = Diligent::BLEND_OPERATION_MAX;

    if (key.state.AlphaBlend == Effekseer::AlphaBlendType::Opacity) {
        blendDesc.BlendEnable = false;
        blendDesc.DestBlend = Diligent::BLEND_FACTOR_ZERO;
        blendDesc.SrcBlend = Diligent::BLEND_FACTOR_ONE;
        blendDesc.BlendOp = Diligent::BLEND_OPERATION_ADD;
    }

    if (key.state.AlphaBlend == Effekseer::AlphaBlendType::Blend) {
        if (GetImpl()->IsPremultipliedAlphaEnabled) {
            blendDesc.BlendOp = Diligent::BLEND_OPERATION_ADD;
            blendDesc.BlendOpAlpha = Diligent::BLEND_OPERATION_ADD;
            blendDesc.SrcBlend = Diligent::BLEND_FACTOR_SRC_ALPHA;
            blendDesc.DestBlend = Diligent::BLEND_FACTOR_INV_SRC_ALPHA;
            blendDesc.SrcBlendAlpha = Diligent::BLEND_FACTOR_ONE;
            blendDesc.DestBlendAlpha = Diligent::BLEND_FACTOR_INV_SRC_ALPHA;
        } else {
            blendDesc.BlendOp = Diligent::BLEND_OPERATION_ADD;
            blendDesc.SrcBlend = Diligent::BLEND_FACTOR_SRC_ALPHA;
            blendDesc.DestBlend = Diligent::BLEND_FACTOR_INV_SRC_ALPHA;
        }
    }

    if (key.state.AlphaBlend == Effekseer::AlphaBlendType::Add) {
        if (GetImpl()->IsPremultipliedAlphaEnabled) {
            blendDesc.BlendOp = Diligent::BLEND_OPERATION_ADD;
            blendDesc.BlendOpAlpha = Diligent::BLEND_OPERATION_ADD;
            blendDesc.SrcBlend = Diligent::BLEND_FACTOR_SRC_ALPHA;
            blendDesc.DestBlend = Diligent::BLEND_FACTOR_ONE;
            blendDesc.SrcBlendAlpha = Diligent::BLEND_FACTOR_ZERO;
            blendDesc.DestBlendAlpha = Diligent::BLEND_FACTOR_ONE;
        } else {
            blendDesc.BlendOp = Diligent::BLEND_OPERATION_ADD;
            blendDesc.SrcBlend = Diligent::BLEND_FACTOR_SRC_ALPHA;
            blendDesc.DestBlend = Diligent::BLEND_FACTOR_ONE;
        }
    }

    if (key.state.AlphaBlend == Effekseer::AlphaBlendType::Sub) {
        blendDesc.DestBlend = Diligent::BLEND_FACTOR_ONE;
        blendDesc.SrcBlend = Diligent::BLEND_FACTOR_SRC_ALPHA;
        blendDesc.BlendOp = Diligent::BLEND_OPERATION_REV_SUBTRACT;
        blendDesc.SrcBlendAlpha = Diligent::BLEND_FACTOR_ZERO;
        blendDesc.DestBlendAlpha = Diligent::BLEND_FACTOR_ONE;
        blendDesc.BlendOpAlpha = Diligent::BLEND_OPERATION_ADD;
    }

    if (key.state.AlphaBlend == Effekseer::AlphaBlendType::Mul) {
        blendDesc.DestBlend = Diligent::BLEND_FACTOR_SRC_COLOR;
        blendDesc.SrcBlend = Diligent::BLEND_FACTOR_ZERO;
        blendDesc.BlendOp = Diligent::BLEND_OPERATION_ADD;
        blendDesc.SrcBlendAlpha = Diligent::BLEND_FACTOR_ZERO;
        blendDesc.DestBlendAlpha = Diligent::BLEND_FACTOR_ONE;
        blendDesc.BlendOpAlpha = Diligent::BLEND_OPERATION_ADD;
    }

    Diligent::RefCntAutoPtr<Diligent::IPipelineState> piplineState;
    auto device = renderDevice->GetRenderDevice();
    device->CreateGraphicsPipelineState(PSOCreateInfo, &piplineState);

    Diligent::IShaderResourceVariable* srv = piplineState->GetStaticVariableByName(Diligent::SHADER_TYPE_VERTEX, "VS_ConstantBuffer");
    if (!srv) {
        srv = piplineState->GetStaticVariableByName(Diligent::SHADER_TYPE_VERTEX, "VSConstantBuffer");
    }
    if (srv) {
        srv->Set(currentShader->GetVertexUniformBuffer());
    }
    srv = piplineState->GetStaticVariableByName(Diligent::SHADER_TYPE_PIXEL, "PS_ConstantBuffer");
    if (!srv) {
        srv = piplineState->GetStaticVariableByName(Diligent::SHADER_TYPE_PIXEL, "PSConstantBuffer");
    }
    if (srv) {
        srv->Set(currentShader->GetPixelUniformBuffer());
    }
    
    auto& srbinfo = shaderResourceBindings_[piplineState];
    piplineState->CreateShaderResourceBinding(&srbinfo.shaderResourceBinding, true);

    const auto& samplerNames = uniformLayout->GetTextures();
    srbinfo.shaderResourceVariables.resize(samplerNames.size());
    auto get_index_by_name = [&samplerNames](const char* name) {
        for (int i = 0; i < samplerNames.size(); i++) {
            if (name == samplerNames[i]) {
                return i;
            }
        }
        return -1;
    };

    const unsigned numResources = srbinfo.shaderResourceBinding->GetVariableCount(Diligent::SHADER_TYPE_PIXEL);
    for (unsigned resourceIndex = 0; resourceIndex < numResources; ++resourceIndex) {
        auto variable = srbinfo.shaderResourceBinding->GetVariableByIndex(Diligent::SHADER_TYPE_PIXEL, resourceIndex);
        Diligent::ShaderResourceDesc desc;
        variable->GetResourceDesc(desc);
        int index = -1;
        switch (desc.Type) {
        case Diligent::SHADER_RESOURCE_TYPE_TEXTURE_SRV:
            // TODO: remove this compute
            index = get_index_by_name(desc.Name);
            assert(index >= 0);
            srbinfo.shaderResourceVariables[index/*variable->GetIndex()*/] = variable;
            break;
        default: break;
        }
    }
    piplineStates_[key] = piplineState;

    return piplineState;
}

RendererImplemented::RendererImplemented(int32_t squareMaxCount)
	: graphicsDevice_(nullptr)
	, m_squareMaxCount(squareMaxCount)
	, m_coordinateSystem(::Effekseer::CoordinateSystem::RH)
	, m_renderState(nullptr)
	, m_standardRenderer(nullptr)
	, m_distortingCallback(nullptr)
{
}

RendererImplemented::~RendererImplemented()
{
	// to prevent objects to be disposed before finish renderings.
// 	GetGraphics()->WaitFinish();
// 
// 	for (auto p : piplineStates_)
// 	{
// 		p.second->Release();
// 	}
	piplineStates_.clear();

	commandList_.Reset();
	GetImpl()->DeleteProxyTextures(this);

	ES_SAFE_DELETE(m_distortingCallback);
	ES_SAFE_DELETE(m_standardRenderer);
	ES_SAFE_DELETE(m_renderState);

	if (materialCompiler_ != nullptr)
	{
		materialCompiler_->Release();
		materialCompiler_ = nullptr;
	}
}

void RendererImplemented::OnLostDevice()
{
}

void RendererImplemented::OnResetDevice()
{
}

bool RendererImplemented::Initialize(Urho3D::RenderDevice* renderDevice, /*LLGI::RenderPassPipelineStateKey key, */bool isReversedDepth)
{

	auto gd = Effekseer::MakeRefPtr<Backend::GraphicsDevice>(renderDevice);

	auto ret = Initialize(gd, /*key, */isReversedDepth);

	return ret;
}

static const char* kShaderName[] = {
    "Unlit",                  // RendererShaderType::Unlit
    "Lit",                    // RendererShaderType::Lit
    "BackDistortion",         // RendererShaderType::BackDistortion
    "AdvancedUnlit",          // RendererShaderType::AdvancedUnlit
    "AdvancedLit",            // RendererShaderType::AdvancedLit
    "AdvancedBackDistortion", // RendererShaderType::AdvancedBackDistortion
};

static const char* kShaderFilepath[(int)EffekseerRenderer::RendererShaderType::Material][2] = {
    {"effekseer/builtin/sprite_unlit_vs",            "effekseer/builtin/model_unlit_ps"}, // RendererShaderType::Unlit
    {"effekseer/builtin/sprite_lit_vs",              "effekseer/builtin/model_lit_ps"}, // RendererShaderType::Lit
    {"effekseer/builtin/sprite_distortion_vs",       "effekseer/builtin/model_distortion_ps"}, // RendererShaderType::BackDistortion
    {"effekseer/builtin/ad_sprite_unlit_vs",         "effekseer/builtin/ad_model_unlit_ps"}, // RendererShaderType::AdvancedUnlit
    {"effekseer/builtin/ad_sprite_lit_vs",           "effekseer/builtin/ad_model_lit_ps"}, // RendererShaderType::AdvancedLit
    {"effekseer/builtin/ad_sprite_distortion_vs",    "effekseer/builtin/ad_model_distortion_ps"} // RendererShaderType::AdvancedBackDistortion
};

// static const char* kShaderFilepath[(int)EffekseerRenderer::RendererShaderType::Material][2] = {
//     {"Shaders/HLSL/effekseer/builtin/sprite_unlit_vs.fx",            "Shaders/HLSL/effekseer/builtin/model_unlit_ps.fx"}, // RendererShaderType::Unlit
//     {"Shaders/HLSL/effekseer/builtin/sprite_lit_vs.fx",              "Shaders/HLSL/effekseer/builtin/model_lit_ps.fx"}, // RendererShaderType::Lit
//     {"Shaders/HLSL/effekseer/builtin/sprite_distortion_vs.fx",       "Shaders/HLSL/effekseer/builtin/model_distortion_ps.fx"}, // RendererShaderType::BackDistortion
//     {"Shaders/HLSL/effekseer/builtin/ad_sprite_unlit_vs.fx",         "Shaders/HLSL/effekseer/builtin/ad_model_unlit_ps.fx"}, // RendererShaderType::AdvancedUnlit
//     {"Shaders/HLSL/effekseer/builtin/ad_sprite_lit_vs.fx",           "Shaders/HLSL/effekseer/builtin/ad_model_lit_ps.fx"}, // RendererShaderType::AdvancedLit
//     {"Shaders/HLSL/effekseer/builtin/ad_sprite_distortion_vs.fx",    "Shaders/HLSL/effekseer/builtin/ad_model_distortion_ps.fx"} // RendererShaderType::AdvancedBackDistortion
// };

bool RendererImplemented::Initialize(Backend::GraphicsDeviceRef graphicsDevice,
									 //LLGI::RenderPassPipelineStateKey key,
									 bool isReversedDepth)
{
	graphicsDevice_ = graphicsDevice;
    auto renderDevice = graphicsDevice_->GetRenderDevice();
    deviceContext_ = renderDevice->GetImmediateContext();
//	ChangeRenderPassPipelineState(key);
	isReversedDepth_ = isReversedDepth;

// 	LLGI::SetLogger([](LLGI::LogType type, const std::string& message)
// 					{ std::cout << message << std::endl; });

	// Generate vertex buffer
	{
		GetImpl()->InternalVertexBuffer = std::make_shared<EffekseerRenderer::VertexBufferRing>(graphicsDevice_, EffekseerRenderer::GetMaximumVertexSizeInAllTypes() * m_squareMaxCount * 4, 1);
		if (!GetImpl()->InternalVertexBuffer->GetIsValid())
		{
			GetImpl()->InternalVertexBuffer = nullptr;
			return false;
		}
	}

	if (!EffekseerRenderer::GenerateIndexDataStride<int16_t>(graphicsDevice_, m_squareMaxCount, indexBuffer_, indexBufferForWireframe_))
	{
		return false;
	}

	m_renderState = new RenderState(this);

	auto vlUnlit = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::Unlit).DownCast<Backend::VertexLayout>();
	auto vlLit = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::Lit).DownCast<Backend::VertexLayout>();
	auto vlDist = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::BackDistortion).DownCast<Backend::VertexLayout>();
	auto vlUnlitAd = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::AdvancedUnlit).DownCast<Backend::VertexLayout>();
	auto vlLitAd = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::AdvancedLit).DownCast<Backend::VertexLayout>();
	auto vlDistAd = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::AdvancedBackDistortion).DownCast<Backend::VertexLayout>();

	vlUnlit->MakeGenerated();
	vlLit->MakeGenerated();
	vlDist->MakeGenerated();
	vlUnlitAd->MakeGenerated();
	vlLitAd->MakeGenerated();
	vlDistAd->MakeGenerated();
    // TODO: uniform layout for rbfx
    const auto texLocUnlit = GetTextureLocations(EffekseerRenderer::RendererShaderType::Unlit);
    const auto texLocLit = GetTextureLocations(EffekseerRenderer::RendererShaderType::Lit);
    const auto texLocDist = GetTextureLocations(EffekseerRenderer::RendererShaderType::BackDistortion);
    const auto texLocAdUnlit = GetTextureLocations(EffekseerRenderer::RendererShaderType::AdvancedUnlit);
    const auto texLocAdLit = GetTextureLocations(EffekseerRenderer::RendererShaderType::AdvancedLit);
    const auto texLocAdDist = GetTextureLocations(EffekseerRenderer::RendererShaderType::AdvancedBackDistortion);

    Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElementsLitUnlit;
    Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElementsDist;

	auto unlit_vs_shader_data = Backend::Serialize(fixedShader_.SpriteUnlit_VS);
	auto unlit_ps_shader_data = Backend::Serialize(fixedShader_.ModelUnlit_PS);
    auto pathIndex = (int)EffekseerRenderer::RendererShaderType::Unlit;
	auto shader_unlit = Shader::Create(graphicsDevice_,
// 									   graphicsDevice_->CreateShaderFromBinary(
// 										   unlit_vs_shader_data.data(),
// 										   static_cast<int32_t>(unlit_vs_shader_data.size()),
// 										   unlit_ps_shader_data.data(),
// 										   static_cast<int32_t>(unlit_ps_shader_data.size())),
                                       graphicsDevice_->CreateShaderFromFile(
                                           kShaderFilepath[pathIndex][0],
                                           kShaderFilepath[pathIndex][1],
                                           Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocUnlit, uniformLayoutElementsLitUnlit)),
									   vlUnlit,
									   "StandardRenderer Unlit");
	if (shader_unlit == nullptr)
		return false;
	GetImpl()->ShaderUnlit = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_unlit);

	auto distortion_vs_shader_data = Backend::Serialize(fixedShader_.SpriteDistortion_VS);
	auto distortion_ps_shader_data = Backend::Serialize(fixedShader_.ModelDistortion_PS);
    pathIndex = (int)EffekseerRenderer::RendererShaderType::BackDistortion;
	auto shader_distortion = Shader::Create(graphicsDevice_,
// 											graphicsDevice_->CreateShaderFromBinary(
// 												distortion_vs_shader_data.data(),
// 												static_cast<int32_t>(distortion_vs_shader_data.size()),
// 												distortion_ps_shader_data.data(),
// 												static_cast<int32_t>(distortion_ps_shader_data.size())),
                                            graphicsDevice_->CreateShaderFromFile(
                                                kShaderFilepath[pathIndex][0],
                                                kShaderFilepath[pathIndex][1],
                                                Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocDist, uniformLayoutElementsDist)),
											vlDist,
											"StandardRenderer BackDistortion");
	if (shader_distortion == nullptr)
		return false;
	GetImpl()->ShaderDistortion = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_distortion);

	auto ad_unlit_vs_shader_data = Backend::Serialize(fixedShader_.AdvancedSpriteUnlit_VS);
	auto ad_unlit_ps_shader_data = Backend::Serialize(fixedShader_.AdvancedModelUnlit_PS);
    pathIndex = (int)EffekseerRenderer::RendererShaderType::AdvancedUnlit;
	auto shader_ad_unlit = Shader::Create(graphicsDevice_,
// 										  graphicsDevice_->CreateShaderFromBinary(
// 											  ad_unlit_vs_shader_data.data(),
// 											  static_cast<int32_t>(ad_unlit_vs_shader_data.size()),
// 											  ad_unlit_ps_shader_data.data(),
// 											  static_cast<int32_t>(ad_unlit_ps_shader_data.size())),
                                          graphicsDevice_->CreateShaderFromFile(
                                              kShaderFilepath[pathIndex][0],
                                              kShaderFilepath[pathIndex][1],
                                              Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocAdUnlit, uniformLayoutElementsLitUnlit)),
										  vlUnlitAd,
										  "StandardRenderer AdvancedUnlit");
	if (shader_ad_unlit == nullptr)
		return false;
	GetImpl()->ShaderAdUnlit = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_ad_unlit);

	auto ad_dist_vs_shader_data = Backend::Serialize(fixedShader_.AdvancedSpriteDistortion_VS);
	auto ad_dist_ps_shader_data = Backend::Serialize(fixedShader_.AdvancedModelDistortion_PS);
    pathIndex = (int)EffekseerRenderer::RendererShaderType::AdvancedBackDistortion;
	auto shader_ad_distortion = Shader::Create(graphicsDevice_,
// 											   graphicsDevice_->CreateShaderFromBinary(
// 												   ad_dist_vs_shader_data.data(),
// 												   static_cast<int32_t>(ad_dist_vs_shader_data.size()),
// 												   ad_dist_ps_shader_data.data(),
// 												   static_cast<int32_t>(ad_dist_ps_shader_data.size())),
                                               graphicsDevice_->CreateShaderFromFile(
                                                   kShaderFilepath[pathIndex][0],
                                                   kShaderFilepath[pathIndex][1],
                                                   Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocAdDist, uniformLayoutElementsDist)),
											   vlDistAd,
											   "StandardRenderer AdvancedBackDistortion");
	if (shader_ad_distortion == nullptr)
		return false;
	GetImpl()->ShaderAdDistortion = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_ad_distortion);

	auto lit_vs_shader_data = Backend::Serialize(fixedShader_.SpriteLit_VS);
	auto lit_ps_shader_data = Backend::Serialize(fixedShader_.ModelLit_PS);
    pathIndex = (int)EffekseerRenderer::RendererShaderType::Lit;
	auto shader_lit = Shader::Create(graphicsDevice_,
// 									 graphicsDevice_->CreateShaderFromBinary(
// 										 lit_vs_shader_data.data(),
// 										 static_cast<int32_t>(lit_vs_shader_data.size()),
// 										 lit_ps_shader_data.data(),
// 										 static_cast<int32_t>(lit_ps_shader_data.size())),
                                     graphicsDevice_->CreateShaderFromFile(
                                         kShaderFilepath[pathIndex][0],
                                         kShaderFilepath[pathIndex][1],
                                         Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocLit, uniformLayoutElementsLitUnlit)),
									 vlLit,
									 "StandardRenderer Lit");
	GetImpl()->ShaderLit = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_lit);

	auto ad_lit_vs_shader_data = Backend::Serialize(fixedShader_.AdvancedSpriteLit_VS);
	auto ad_lit_ps_shader_data = Backend::Serialize(fixedShader_.AdvancedModelLit_PS);
    pathIndex = (int)EffekseerRenderer::RendererShaderType::AdvancedLit;
	auto shader_ad_lit = Shader::Create(graphicsDevice_,
// 										graphicsDevice_->CreateShaderFromBinary(
// 											ad_lit_vs_shader_data.data(),
// 											static_cast<int32_t>(ad_lit_vs_shader_data.size()),
// 											ad_lit_ps_shader_data.data(),
// 											static_cast<int32_t>(ad_lit_ps_shader_data.size())),
                                        graphicsDevice_->CreateShaderFromFile(
                                            kShaderFilepath[pathIndex][0],
                                            kShaderFilepath[pathIndex][1],
                                            Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocAdLit, uniformLayoutElementsLitUnlit)),
										vlLitAd,
										"StandardRenderer AdvancedLit");
	GetImpl()->ShaderAdLit = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_ad_lit);

	shader_unlit->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_unlit->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBuffer));

	shader_distortion->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_distortion->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBufferDistortion));

	shader_ad_unlit->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_ad_unlit->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBuffer));

	shader_ad_distortion->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_ad_distortion->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBufferDistortion));

	shader_lit->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_lit->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBuffer));

	shader_ad_lit->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_ad_lit->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBuffer));

	m_standardRenderer = new EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>(this);

	GetImpl()->CreateProxyTextures(this);
	GetImpl()->isSoftParticleEnabled = true;

	GetImpl()->isDepthReversed = isReversedDepth;
	return true;
}

void RendererImplemented::SetRestorationOfStatesFlag(bool flag)
{
}

// void RendererImplemented::ChangeRenderPassPipelineState(LLGI::RenderPassPipelineStateKey key)
// {
// 	auto it = renderpassPipelineStates_.find(key);
// 	if (it != renderpassPipelineStates_.end())
// 	{
// 		currentRenderPassPipelineState_ = it->second;
// 	}
// 	else
// 	{
// 		auto gd = graphicsDevice_.DownCast<EffekseerUrho3D::Backend::GraphicsDevice>();
// 		auto pipelineState = LLGI::CreateSharedPtr(gd->GetGraphics()->CreateRenderPassPipelineState(key));
// 		if (pipelineState != nullptr)
// 		{
// 			renderpassPipelineStates_[key] = pipelineState;
// 		}
// 		currentRenderPassPipelineState_ = pipelineState;
// 	}
// }

bool RendererImplemented::BeginRendering()
{
	assert(graphicsDevice_ != nullptr);

//    auto cl = commandList_.DownCast<::EffekseerUrho3D::CommandList>();
//    cl->GetInternal()->Reset();
// 	if (commandList_ == nullptr)
// 	{
// 		return false;
// 	}

	impl->CalculateCameraProjectionMatrix();

	// initialize states
	m_renderState->GetActiveState().Reset();
	m_renderState->Update(true);

	// reset renderer
	m_standardRenderer->ResetAndRenderingIfRequired();

	return true;
}

bool RendererImplemented::EndRendering()
{
	assert(graphicsDevice_ != nullptr);

// 	if (commandList_ == nullptr)
// 	{
// 		return false;
// 	}

	// reset renderer
	m_standardRenderer->ResetAndRenderingIfRequired();

	currentIndexBuffer_ = nullptr;

	return true;
}

void RendererImplemented::SetCommandList(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList)
{
	commandList_ = commandList;
}

Effekseer::Backend::IndexBufferRef RendererImplemented::GetIndexBuffer()
{
	if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
	{
		return indexBufferForWireframe_;
	}
	return indexBuffer_;
}

int32_t RendererImplemented::GetSquareMaxCount() const
{
	return m_squareMaxCount;
}

::EffekseerRenderer::RenderStateBase* RendererImplemented::GetRenderState()
{
	return m_renderState;
}

::Effekseer::SpriteRendererRef RendererImplemented::CreateSpriteRenderer()
{
	return ::Effekseer::SpriteRendererRef(new ::EffekseerRenderer::SpriteRendererBase<RendererImplemented, false>(this));
}

::Effekseer::RibbonRendererRef RendererImplemented::CreateRibbonRenderer()
{
	return ::Effekseer::RibbonRendererRef(new ::EffekseerRenderer::RibbonRendererBase<RendererImplemented, false>(this));
}

::Effekseer::RingRendererRef RendererImplemented::CreateRingRenderer()
{
	return ::Effekseer::RingRendererRef(new ::EffekseerRenderer::RingRendererBase<RendererImplemented, false>(this));
}

::Effekseer::ModelRendererRef RendererImplemented::CreateModelRenderer()
{
	return ModelRenderer::Create(this, &fixedShader_);
}

::Effekseer::TrackRendererRef RendererImplemented::CreateTrackRenderer()
{
	return ::Effekseer::TrackRendererRef(new ::EffekseerRenderer::TrackRendererBase<RendererImplemented, false>(this));
}

::Effekseer::TextureLoaderRef RendererImplemented::CreateTextureLoader(::Effekseer::FileInterfaceRef fileInterface)
{
	//return ::EffekseerRenderer::CreateTextureLoader(graphicsDevice_, fileInterface, ::Effekseer::ColorSpaceType::Gamma);
    return EffekseerUrho3D::CreateTextureLoader(graphicsDevice_->GetRenderDevice()->GetContext(), graphicsDevice_, ::Effekseer::ColorSpaceType::Gamma);
}

// ::Effekseer::ModelLoaderRef RendererImplemented::CreateModelLoader(::Effekseer::FileInterfaceRef fileInterface)
// {
// 	return ::Effekseer::MakeRefPtr<EffekseerRenderer::ModelLoader>(graphicsDevice_, fileInterface);
// }

::Effekseer::MaterialLoaderRef RendererImplemented::CreateMaterialLoader(::Effekseer::FileInterfaceRef fileInterface)
{
	return ::Effekseer::MakeRefPtr<MaterialLoader>(graphicsDevice_, fileInterface, platformType_, materialCompiler_);
}

void RendererImplemented::SetBackgroundInternal(Urho3D::Texture2D* background)
{
	if (m_backgroundLLGI == nullptr)
	{
		m_backgroundLLGI = graphicsDevice_->CreateTexture(background);
	}
	else
	{
		auto texture = static_cast<EffekseerUrho3D::Texture*>(m_backgroundLLGI.Get());
		texture->Init(background);
	}

	EffekseerRenderer::Renderer::SetBackground((background) ? m_backgroundLLGI : nullptr);
}

EffekseerRenderer::DistortingCallback* RendererImplemented::GetDistortingCallback()
{
	return m_distortingCallback;
}

void RendererImplemented::SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback)
{
	ES_SAFE_DELETE(m_distortingCallback);
	m_distortingCallback = callback;
}

void RendererImplemented::SetVertexBuffer(Diligent::IBuffer* vertexBuffer, int32_t stride)
{
	currentVertexBuffer_ = vertexBuffer;
	currentVertexBufferStride_ = stride;
}

void RendererImplemented::SetVertexBuffer(const Effekseer::Backend::VertexBufferRef& vertexBuffer, int32_t stride)
{
	auto vb = static_cast<Backend::VertexBuffer*>(vertexBuffer.Get());

	SetVertexBuffer(vb->GetBuffer(), stride);
}

void RendererImplemented::SetIndexBuffer(const Effekseer::Backend::IndexBufferRef& indexBuffer)
{
    currentIndexBuffer_ = static_cast<Backend::IndexBuffer*>(indexBuffer.Get());
}

void RendererImplemented::SetLayout(Shader* shader)
{
	if (m_renderMode == Effekseer::RenderMode::Normal)
	{
		currentTopologyType_ = Diligent::PRIMITIVE_TOPOLOGY::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	}
	else
	{
		currentTopologyType_ = Diligent::PRIMITIVE_TOPOLOGY::PRIMITIVE_TOPOLOGY_LINE_LIST;
	}
}

void RendererImplemented::CommitUniformAndTextures(Diligent::IPipelineState* pipelineState)
{
    if (currentShader->GetVertexConstantBufferSize() > 0)
    {
        Diligent::PVoid pMappedData{nullptr};
        auto buffer = currentShader->GetVertexUniformBuffer();
        deviceContext_->MapBuffer(buffer, Diligent::MAP_WRITE, Diligent::MAP_FLAG_DISCARD, pMappedData);
        if (pMappedData)
        {
            memcpy(pMappedData, currentShader->GetVertexConstantBuffer(), currentShader->GetVertexConstantBufferSize());
            deviceContext_->UnmapBuffer(buffer, Diligent::MAP_WRITE);
        }
    }

    if (currentShader->GetPixelConstantBufferSize() > 0)
    {
        Diligent::PVoid pMappedData{nullptr};
        auto buffer = currentShader->GetPixelUniformBuffer();
        deviceContext_->MapBuffer(buffer, Diligent::MAP_WRITE, Diligent::MAP_FLAG_DISCARD, pMappedData);
        if (pMappedData)
        {
            memcpy(pMappedData, currentShader->GetPixelConstantBuffer(), currentShader->GetPixelConstantBufferSize());
            deviceContext_->UnmapBuffer(buffer, Diligent::MAP_WRITE);
        }
    }

    const auto& uniformLayout = currentShader->GetUniformLayout();
    if (uniformLayout == nullptr) {
        return;
    }
    //const auto& samplerNames = uniformLayout->GetTextures();
    auto count = m_currentTextures_.size();
    auto& shaderResourceVariables = shaderResourceBindings_[pipelineState].shaderResourceVariables;
    int32_t j = 0;
    for (int32_t i = 0; i < count; i++) {
        if (m_currentTextures_[i] != nullptr) {
            auto texture = static_cast<EffekseerUrho3D::Texture*>(m_currentTextures_[i].Get());
            const auto& srv = texture->GetTexture()->GetHandles().srv_;
            //shaderResourceVariables[samplerNames[j++]]->Set(srv);
            if (shaderResourceVariables[j]) {
                shaderResourceVariables[j++]->Set(srv/*, Diligent::SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE*/);
            }
        }
    }
}

void RendererImplemented::DrawSprites(int32_t spriteCount, int32_t vertexOffset)
{
    auto piplineState = GetOrCreatePiplineState();
    const auto& srbinfo = shaderResourceBindings_[piplineState];
    CommitUniformAndTextures(piplineState);

    const Diligent::Uint64 offset = 0;
    Diligent::IBuffer* pBuffs[] = { currentVertexBuffer_ };
    deviceContext_->SetVertexBuffers(0, 1, pBuffs, &offset, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, Diligent::SET_VERTEX_BUFFERS_FLAG_RESET);
    deviceContext_->SetIndexBuffer(currentIndexBuffer_->GetBuffer(), 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    deviceContext_->SetPipelineState(piplineState);
    deviceContext_->CommitShaderResources(srbinfo.shaderResourceBinding, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    impl->drawcallCount++;
	impl->drawvertexCount += spriteCount * 4;

    Diligent::DrawIndexedAttribs DrawAttrs; // This is an indexed draw call
    DrawAttrs.IndexType = (currentIndexBuffer_->GetStrideType() == Effekseer::Backend::IndexBufferStrideType::Stride4) ? Diligent::VT_UINT32 : Diligent::VT_UINT16; // Index type
    DrawAttrs.BaseVertex = vertexOffset;
    if (m_renderMode == Effekseer::RenderMode::Normal) {
        DrawAttrs.NumIndices = spriteCount * 6;
	} else {
        DrawAttrs.NumIndices = spriteCount * 8;
	}
    // Verify the state of vertex and index buffers
    DrawAttrs.Flags = Diligent::DRAW_FLAG_VERIFY_ALL;
    deviceContext_->DrawIndexed(DrawAttrs);
}

void RendererImplemented::DrawPolygon(int32_t vertexCount, int32_t indexCount)
{
	DrawPolygonInstanced(vertexCount, indexCount, 1);
}

void RendererImplemented::DrawPolygonInstanced(int32_t vertexCount, int32_t indexCount, int32_t instanceCount)
{
    auto piplineState = GetOrCreatePiplineState();
    const auto& srbinfo = shaderResourceBindings_[piplineState];
    CommitUniformAndTextures(piplineState);
    
    const Diligent::Uint64 offset = 0;
    Diligent::IBuffer* pBuffs[] = { currentVertexBuffer_ };
    deviceContext_->SetVertexBuffers(0, 1, pBuffs, &offset, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, Diligent::SET_VERTEX_BUFFERS_FLAG_RESET);
    deviceContext_->SetIndexBuffer(currentIndexBuffer_->GetBuffer(), 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    deviceContext_->SetPipelineState(piplineState);
    deviceContext_->CommitShaderResources(srbinfo.shaderResourceBinding, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

	impl->drawcallCount++;
	impl->drawvertexCount += vertexCount * instanceCount;

    Diligent::DrawIndexedAttribs DrawAttrs; // This is an indexed draw call
    DrawAttrs.IndexType = (currentIndexBuffer_->GetStrideType() == Effekseer::Backend::IndexBufferStrideType::Stride4) ? Diligent::VT_UINT32 : Diligent::VT_UINT16; // Index type
    DrawAttrs.NumIndices = indexCount;
    DrawAttrs.NumInstances = instanceCount;
    DrawAttrs.Flags = Diligent::DRAW_FLAG_VERIFY_ALL;
    deviceContext_->DrawIndexed(DrawAttrs);
}

void RendererImplemented::BeginShader(Shader* shader)
{
	currentShader = shader;
}

void RendererImplemented::EndShader(Shader* shader)
{
	currentShader = nullptr;
}

void RendererImplemented::SetVertexBufferToShader(const void* data, int32_t size, int32_t dstOffset)
{
	assert(currentShader != nullptr);
	auto p = static_cast<uint8_t*>(currentShader->GetVertexConstantBuffer()) + dstOffset;
	memcpy(p, data, size);
}

void RendererImplemented::SetPixelBufferToShader(const void* data, int32_t size, int32_t dstOffset)
{
	assert(currentShader != nullptr);
	auto p = static_cast<uint8_t*>(currentShader->GetPixelConstantBuffer()) + dstOffset;
	memcpy(p, data, size);
}

void RendererImplemented::SetTextures(Shader* shader, Effekseer::Backend::TextureRef* textures, int32_t count)
{
    m_currentTextures_.resize(count);
    for (int32_t i = 0; i < count; i++) {
        if (textures[i] != nullptr) {
            m_currentTextures_[i] = textures[i];
        } else {
            m_currentTextures_[i].Reset();
        }
    }
}

void RendererImplemented::ResetRenderState()
{
	m_renderState->GetActiveState().Reset();
	m_renderState->Update(true);
}

// void RendererImplemented::ResetStateForDefferedContext()
// {
//     GetImpl()->InternalVertexBuffer->GetCurrentBuffer().DownCast<Backend::VertexBuffer>()->MakeAllDirtied();
// }

Effekseer::CustomVector<Effekseer::CustomString<char>> GetTextureLocations(EffekseerRenderer::RendererShaderType type)
{
    Effekseer::CustomVector<Effekseer::CustomString<char>> texLoc;

    auto pushColor = [](Effekseer::CustomVector<Effekseer::CustomString<char>>& texLoc)
    { texLoc.emplace_back("Sampler_sampler_colorTex"); };

    auto pushDepth = [](Effekseer::CustomVector<Effekseer::CustomString<char>>& texLoc)
    { texLoc.emplace_back("Sampler_sampler_depthTex"); };

    auto pushBack = [](Effekseer::CustomVector<Effekseer::CustomString<char>>& texLoc)
    { texLoc.emplace_back("Sampler_sampler_backTex"); };

    auto pushNormal = [](Effekseer::CustomVector<Effekseer::CustomString<char>>& texLoc)
    { texLoc.emplace_back("Sampler_sampler_normalTex"); };

    auto pushAdvancedRendererParameterLoc = [](Effekseer::CustomVector<Effekseer::CustomString<char>>& texLoc) -> void
    {
        texLoc.emplace_back("Sampler_sampler_alphaTex");
        texLoc.emplace_back("Sampler_sampler_uvDistortionTex");
        texLoc.emplace_back("Sampler_sampler_blendTex");
        texLoc.emplace_back("Sampler_sampler_blendAlphaTex");
        texLoc.emplace_back("Sampler_sampler_blendUVDistortionTex");
    };

    pushColor(texLoc);

    if (type == EffekseerRenderer::RendererShaderType::Lit)
    {
        pushNormal(texLoc);
    }
    else if (type == EffekseerRenderer::RendererShaderType::BackDistortion)
    {
        pushBack(texLoc);
    }
    else if (type == EffekseerRenderer::RendererShaderType::AdvancedUnlit)
    {
        pushAdvancedRendererParameterLoc(texLoc);
    }
    else if (type == EffekseerRenderer::RendererShaderType::AdvancedLit)
    {
        pushNormal(texLoc);
        pushAdvancedRendererParameterLoc(texLoc);
    }
    else if (type == EffekseerRenderer::RendererShaderType::AdvancedBackDistortion)
    {
        pushBack(texLoc);
        pushAdvancedRendererParameterLoc(texLoc);
    }
    pushDepth(texLoc);

    return texLoc;
}

} // namespace EffekseerUrho3D
