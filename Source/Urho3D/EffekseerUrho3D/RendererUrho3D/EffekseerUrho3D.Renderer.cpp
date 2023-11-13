

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
RendererRef Renderer::Create(Urho3D::Graphics* graphics, int32_t squareMaxCount, bool isReversedDepth)
{
    auto renderer = Effekseer::MakeRefPtr<RendererImplemented>(squareMaxCount);
    if (renderer->Initialize(graphics, isReversedDepth))
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

    auto renderDevice = graphicsDevice_->GetGraphics()->GetSubsystem<Urho3D::RenderDevice>();
    auto swapChain = renderDevice->GetSwapChain();
    // This tutorial will render to a single render target
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
    // Set render target format which is the format of the swap chain's color buffer
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = swapChain->GetDesc().ColorBufferFormat;
    // Use the depth buffer format from the swap chain
    PSOCreateInfo.GraphicsPipeline.DSVFormat = swapChain->GetDesc().DepthBufferFormat;
    // Primitive topology defines what kind of primitives will be rendered by this pipeline state
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = Diligent::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // No back face culling for this tutorial
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = Diligent::CULL_MODE_NONE;
    // Depth testing
    auto& depthStencilDesc = PSOCreateInfo.GraphicsPipeline.DepthStencilDesc;
    depthStencilDesc.DepthEnable = key.state.DepthTest;
    depthStencilDesc.DepthWriteEnable = key.state.DepthWrite;
    depthStencilDesc.DepthFunc = isReversedDepth_ ? Diligent::COMPARISON_FUNC_GREATER_EQUAL : Diligent::COMPARISON_FUNC_LESS_EQUAL;

    auto& rasterizerDesc = PSOCreateInfo.GraphicsPipeline.RasterizerDesc;
    if (key.state.CullingType == ::Effekseer::CullingType::Back)
    {
        rasterizerDesc.CullMode = isReversedDepth_ ? Diligent::CULL_MODE_FRONT : Diligent::CULL_MODE_BACK;
    }
    else if (key.state.CullingType == ::Effekseer::CullingType::Front)
    {
        rasterizerDesc.CullMode = isReversedDepth_ ? Diligent::CULL_MODE_BACK : Diligent::CULL_MODE_FRONT;
    }
    else if (key.state.CullingType == ::Effekseer::CullingType::Double)
    {
        rasterizerDesc.CullMode = Diligent::CULL_MODE_NONE;
    }

    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = currentTopologyType_;

    PSOCreateInfo.pVS = currentShader->GetVertexShader();
    PSOCreateInfo.pPS = currentShader->GetPixelShader();

    const auto& elements = currentShader->GetVertexLayouts()->GetElements();
    auto& inputLayout = PSOCreateInfo.GraphicsPipeline.InputLayout;
    inputLayout.NumElements = elements.size();
    inputLayout.LayoutElements = elements.data();
//     ea::vector<Diligent::LayoutElement> layoutElement;
//     layoutElement.resize(elements.size());
//     for (size_t i = 0; i < elements.size(); i++) {
//         layoutElement[i] = elements[i].Format;
//     }

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
            Vars.emplace_back(Diligent::SHADER_TYPE_PIXEL, samplerNames[i].c_str(), Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE);
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

    if (key.state.AlphaBlend == Effekseer::AlphaBlendType::Opacity)
    {
        blendDesc.BlendEnable = false;
        blendDesc.DestBlend = Diligent::BLEND_FACTOR_ZERO;
        blendDesc.SrcBlend = Diligent::BLEND_FACTOR_ONE;
        blendDesc.BlendOp = Diligent::BLEND_OPERATION_ADD;
    }

    if (key.state.AlphaBlend == Effekseer::AlphaBlendType::Blend)
    {
        if (GetImpl()->IsPremultipliedAlphaEnabled)
        {
            blendDesc.BlendOp = Diligent::BLEND_OPERATION_ADD;
            blendDesc.BlendOpAlpha = Diligent::BLEND_OPERATION_ADD;
            blendDesc.SrcBlend = Diligent::BLEND_FACTOR_SRC_ALPHA;
            blendDesc.DestBlend = Diligent::BLEND_FACTOR_INV_SRC_ALPHA;
            blendDesc.SrcBlendAlpha = Diligent::BLEND_FACTOR_ONE;
            blendDesc.DestBlendAlpha = Diligent::BLEND_FACTOR_INV_SRC_ALPHA;
        }
        else
        {
            blendDesc.BlendOp = Diligent::BLEND_OPERATION_ADD;
            blendDesc.SrcBlend = Diligent::BLEND_FACTOR_SRC_ALPHA;
            blendDesc.DestBlend = Diligent::BLEND_FACTOR_INV_SRC_ALPHA;
        }
    }

    if (key.state.AlphaBlend == Effekseer::AlphaBlendType::Add)
    {
        if (GetImpl()->IsPremultipliedAlphaEnabled)
        {
            blendDesc.BlendOp = Diligent::BLEND_OPERATION_ADD;
            blendDesc.BlendOpAlpha = Diligent::BLEND_OPERATION_ADD;
            blendDesc.SrcBlend = Diligent::BLEND_FACTOR_SRC_ALPHA;
            blendDesc.DestBlend = Diligent::BLEND_FACTOR_ONE;
            blendDesc.SrcBlendAlpha = Diligent::BLEND_FACTOR_ZERO;
            blendDesc.DestBlendAlpha = Diligent::BLEND_FACTOR_ONE;
        }
        else
        {
            blendDesc.BlendOp = Diligent::BLEND_OPERATION_ADD;
            blendDesc.SrcBlend = Diligent::BLEND_FACTOR_SRC_ALPHA;
            blendDesc.DestBlend = Diligent::BLEND_FACTOR_ONE;
        }
    }

    if (key.state.AlphaBlend == Effekseer::AlphaBlendType::Sub)
    {
        blendDesc.DestBlend = Diligent::BLEND_FACTOR_ONE;
        blendDesc.SrcBlend = Diligent::BLEND_FACTOR_SRC_ALPHA;
        blendDesc.BlendOp = Diligent::BLEND_OPERATION_REV_SUBTRACT;
        blendDesc.SrcBlendAlpha = Diligent::BLEND_FACTOR_ZERO;
        blendDesc.DestBlendAlpha = Diligent::BLEND_FACTOR_ONE;
        blendDesc.BlendOpAlpha = Diligent::BLEND_OPERATION_ADD;
    }

    if (key.state.AlphaBlend == Effekseer::AlphaBlendType::Mul)
    {
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
    piplineState->GetStaticVariableByName(Diligent::SHADER_TYPE_VERTEX, "CBVS0")->Set(currentShader->GetVertexUniformBuffer());
    piplineState->GetStaticVariableByName(Diligent::SHADER_TYPE_PIXEL, "CBPS0")->Set(currentShader->GetPixelUniformBuffer());
    Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> SRB;
    piplineState->CreateShaderResourceBinding(&SRB, true);
    currentShader->SetShaderResourceBinding(SRB);
    piplineStates_[key] = piplineState;

    return piplineState;
    /*
    Urho3D::GraphicsPipelineStateDesc desc;
    Urho3D::InitializeInputLayout(desc.inputLayout_, { currentVertexBuffer_ });
    desc.colorWriteEnabled_ = true;

//	auto piplineState = GetGraphics()->CreatePiplineState();

//     desc.vertexShader_ = currentShader->GetVertexShader();
//     desc.pixelShader_ = currentShader->GetPixelShader();

    const auto& elements = currentShader->GetVertexLayouts()->GetElements();
    desc.inputLayout_.size_ = static_cast<int32_t>(elements.size());
    int32_t stride = currentShader->GetVertexSize();//GetCurrentStride();// 
	for (size_t i = 0; i < elements.size(); i++) {
        const auto& element = elements[i].Format;
        auto& layout = desc.inputLayout_.elements_[i];
        layout.bufferIndex_             = 0;
        layout.bufferStride_            = stride;// currentVertexBuffer_->GetVertexSize();
        layout.elementOffset_           = element.offset_;
        layout.instanceStepRate_        = element.stepRate_;
        
        layout.elementType_             = element.type_;
        layout.elementSemantic_         = element.semantic_;
        layout.elementSemanticIndex_    = element.index_;
	}

    const auto& uniformLayout = currentShader->GetUniformLayout();
    if (uniformLayout != nullptr) {
        Urho3D::TextureAddressMode ws[2]{};
        ws[(int)Effekseer::TextureWrapType::Clamp] = Urho3D::TextureAddressMode::ADDRESS_CLAMP;
        ws[(int)Effekseer::TextureWrapType::Repeat] = Urho3D::TextureAddressMode::ADDRESS_WRAP;

        Urho3D::TextureFilterMode fs[2]{};
        fs[(int)Effekseer::TextureFilterType::Linear] = Urho3D::TextureFilterMode::FILTER_BILINEAR;
        fs[(int)Effekseer::TextureFilterType::Nearest] = Urho3D::TextureFilterMode::FILTER_NEAREST;
        
        const auto& samplerNames = uniformLayout->GetTextures();
        auto samplerCount = samplerNames.size();
        const auto& state = key.state;
        for (int i = 0; i < samplerCount; i++) {
            auto ssd = Urho3D::SamplerStateDesc::Default();
            ssd.filterMode_ = fs[(int)state.TextureFilterTypes[i]];
            ssd.addressMode_.fill(ws[(int)state.TextureWrapTypes[i]]);
            desc.samplers_.Add(samplerNames[i].c_str(), ssd);
        }
    }

//     piplineState->SetRenderPassPipelineState(currentRenderPassPipelineState_.get());
// 
// 	if (!piplineState->Compile())
// 	{
// 		assert(0);
// 	}

    auto piplineState = GetGraphics()->GetSubsystem<Urho3D::PipelineStateCache>()->GetGraphicsPipelineState(desc);
	piplineStates_[key] = piplineState;

	return piplineState;
    */
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

bool RendererImplemented::Initialize(Urho3D::Graphics* graphics, /*LLGI::RenderPassPipelineStateKey key, */bool isReversedDepth)
{

	auto gd = Effekseer::MakeRefPtr<Backend::GraphicsDevice>(graphics);

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

// static const char* kShaderFilepath[(int)EffekseerRenderer::RendererShaderType::Material][2] = {
//     {"Shaders/HLSL/effekseer/builtin/sprite_unlit_vs.hlsl",            "Shaders/HLSL/effekseer/builtin/model_unlit_ps.hlsl"}, // RendererShaderType::Unlit
//     {"Shaders/HLSL/effekseer/builtin/sprite_lit_vs.hlsl",              "Shaders/HLSL/effekseer/builtin/model_lit_ps.hlsl"}, // RendererShaderType::Lit
//     {"Shaders/HLSL/effekseer/builtin/sprite_distortion_vs.hlsl",       "Shaders/HLSL/effekseer/builtin/model_distortion_ps.hlsl"}, // RendererShaderType::BackDistortion
//     {"Shaders/HLSL/effekseer/builtin/ad_sprite_unlit_vs.hlsl",         "Shaders/HLSL/effekseer/builtin/ad_model_unlit_ps.hlsl"}, // RendererShaderType::AdvancedUnlit
//     {"Shaders/HLSL/effekseer/builtin/ad_sprite_lit_vs.hlsl",           "Shaders/HLSL/effekseer/builtin/ad_model_lit_ps.hlsl"}, // RendererShaderType::AdvancedLit
//     {"Shaders/HLSL/effekseer/builtin/ad_sprite_distortion_vs.hlsl",    "Shaders/HLSL/effekseer/builtin/ad_model_distortion_ps.hlsl"} // RendererShaderType::AdvancedBackDistortion
// };

static const char* kShaderFilepath[(int)EffekseerRenderer::RendererShaderType::Material][2] = {
    {"Shaders/HLSL/effekseer/builtin/sprite_unlit_vs.fx",            "Shaders/HLSL/effekseer/builtin/model_unlit_ps.fx"}, // RendererShaderType::Unlit
    {"Shaders/HLSL/effekseer/builtin/sprite_lit_vs.fx",              "Shaders/HLSL/effekseer/builtin/model_lit_ps.fx"}, // RendererShaderType::Lit
    {"Shaders/HLSL/effekseer/builtin/sprite_distortion_vs.fx",       "Shaders/HLSL/effekseer/builtin/model_distortion_ps.fx"}, // RendererShaderType::BackDistortion
    {"Shaders/HLSL/effekseer/builtin/ad_sprite_unlit_vs.fx",         "Shaders/HLSL/effekseer/builtin/ad_model_unlit_ps.fx"}, // RendererShaderType::AdvancedUnlit
    {"Shaders/HLSL/effekseer/builtin/ad_sprite_lit_vs.fx",           "Shaders/HLSL/effekseer/builtin/ad_model_lit_ps.fx"}, // RendererShaderType::AdvancedLit
    {"Shaders/HLSL/effekseer/builtin/ad_sprite_distortion_vs.fx",    "Shaders/HLSL/effekseer/builtin/ad_model_distortion_ps.fx"} // RendererShaderType::AdvancedBackDistortion
};

bool RendererImplemented::Initialize(Backend::GraphicsDeviceRef graphicsDevice,
									 //LLGI::RenderPassPipelineStateKey key,
									 bool isReversedDepth)
{
	graphicsDevice_ = graphicsDevice;
    auto renderDevice = graphicsDevice_->GetGraphics()->GetSubsystem<Urho3D::RenderDevice>();
    deviceContext_ = renderDevice->GetImmediateContext();
//	ChangeRenderPassPipelineState(key);
	isReversedDepth_ = isReversedDepth;

// 	LLGI::SetLogger([](LLGI::LogType type, const std::string& message)
// 					{ std::cout << message << std::endl; });

	// Generate vertex buffer
	{
		GetImpl()->InternalVertexBuffer = std::make_shared<EffekseerRenderer::VertexBufferRing>(graphicsDevice_, EffekseerRenderer::GetMaximumVertexSizeInAllTypes() * m_squareMaxCount * 4, 3);
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
    const auto texLocLit = GetTextureLocations(EffekseerRenderer::RendererShaderType::AdvancedLit);
    const auto texLocDist = GetTextureLocations(EffekseerRenderer::RendererShaderType::BackDistortion);
    const auto texLocAdUnlit = GetTextureLocations(EffekseerRenderer::RendererShaderType::AdvancedUnlit);
    const auto texLocAdLit = GetTextureLocations(EffekseerRenderer::RendererShaderType::AdvancedLit);
    const auto texLocAdDist = GetTextureLocations(EffekseerRenderer::RendererShaderType::AdvancedBackDistortion);

    Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElementsLitUnlit;
    AddVertexUniformLayout(uniformLayoutElementsLitUnlit);
    AddPixelUniformLayout(uniformLayoutElementsLitUnlit);

    Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElementsDist;
    AddVertexUniformLayout(uniformLayoutElementsDist);
    AddDistortionPixelUniformLayout(uniformLayoutElementsDist);
    //
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
									   "StandardRenderer");
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
											"StandardRenderer Distortion");
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
										  "StandardRenderer");
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
											   "StandardRenderer Distortion");
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
									 "StandardRenderer Lighting");
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
										"StandardRenderer Lighting");
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

    auto cl = commandList_.DownCast<::EffekseerUrho3D::CommandList>();
    cl->GetInternal()->Reset();
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

// ::Effekseer::TextureLoaderRef RendererImplemented::CreateTextureLoader(::Effekseer::FileInterfaceRef fileInterface)
// {
// 	return ::EffekseerRenderer::CreateTextureLoader(graphicsDevice_, fileInterface, ::Effekseer::ColorSpaceType::Gamma);
// }
// 
// ::Effekseer::ModelLoaderRef RendererImplemented::CreateModelLoader(::Effekseer::FileInterfaceRef fileInterface)
// {
// 	return ::Effekseer::MakeRefPtr<EffekseerRenderer::ModelLoader>(graphicsDevice_, fileInterface);
// }
// 
// ::Effekseer::MaterialLoaderRef RendererImplemented::CreateMaterialLoader(::Effekseer::FileInterfaceRef fileInterface)
// {
// 	return ::Effekseer::MakeRefPtr<MaterialLoader>(graphicsDevice_, fileInterface, platformType_, materialCompiler_);
// }

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

void RendererImplemented::SetVertexBuffer(Urho3D::VertexBuffer* vertexBuffer, int32_t stride)
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
	auto ib = static_cast<Backend::IndexBuffer*>(indexBuffer.Get());
    currentIndexBuffer_ = ib->GetBuffer();
	//GetCurrentCommandList()->SetIndexBuffer(ib->GetBuffer()/*, ib->GetStrideType() == Effekseer::Backend::IndexBufferStrideType::Stride2 ? 2 : 4*/);
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

void RendererImplemented::CommitUniformAndTextures()
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
    const auto& samplerNames = uniformLayout->GetTextures();
    auto count = m_currentTextures_.size();
    auto srb = currentShader->GetShaderResourceBinding();
    for (int32_t i = 0; i < count; i++) {
        if (m_currentTextures_[i] == nullptr) {
            srb->GetVariableByName(Diligent::SHADER_TYPE_PIXEL, samplerNames[i].c_str())->Set(nullptr);
        } else {
            auto texture = static_cast<EffekseerUrho3D::Texture*>(m_currentTextures_[i].Get());
            const auto& handles = texture->GetTexture()->GetHandles();
            srb->GetVariableByName(Diligent::SHADER_TYPE_PIXEL, samplerNames[i].c_str())->Set(handles.texture_);
        }
    }
}

void RendererImplemented::DrawSprites(int32_t spriteCount, int32_t vertexOffset)
{
    auto piplineState = GetOrCreatePiplineState();
    CommitUniformAndTextures();

    const Diligent::Uint64 offset = 0;
    Diligent::IBuffer* pBuffs[] = { currentVertexBuffer_->GetHandle() };
    deviceContext_->SetVertexBuffers(0, 1, pBuffs, &offset, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, Diligent::SET_VERTEX_BUFFERS_FLAG_RESET);
    deviceContext_->SetIndexBuffer(currentIndexBuffer_->GetHandle(), 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    deviceContext_->SetPipelineState(piplineState);
    deviceContext_->CommitShaderResources(currentShader->GetShaderResourceBinding(), Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    impl->drawcallCount++;
	impl->drawvertexCount += spriteCount * 4;

    Diligent::DrawIndexedAttribs DrawAttrs; // This is an indexed draw call
    DrawAttrs.IndexType = currentIndexBuffer_->GetStride() > 2 ? Diligent::VT_UINT32 : Diligent::VT_UINT16; // Index type

    if (m_renderMode == Effekseer::RenderMode::Normal)
	{
        DrawAttrs.FirstIndexLocation = vertexOffset / 4 * 6;
        DrawAttrs.NumIndices = spriteCount * 6;
	}
	else
	{
        DrawAttrs.FirstIndexLocation = vertexOffset / 4 * 8;
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
    CommitUniformAndTextures();
    
    const Diligent::Uint64 offset = 0;
    Diligent::IBuffer* pBuffs[] = { currentVertexBuffer_->GetHandle() };
    deviceContext_->SetVertexBuffers(0, 1, pBuffs, &offset, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, Diligent::SET_VERTEX_BUFFERS_FLAG_RESET);
    deviceContext_->SetIndexBuffer(currentIndexBuffer_->GetHandle(), 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    deviceContext_->SetPipelineState(piplineState);
    deviceContext_->CommitShaderResources(currentShader->GetShaderResourceBinding(), Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

	impl->drawcallCount++;
	impl->drawvertexCount += vertexCount * instanceCount;

    Diligent::DrawIndexedAttribs DrawAttrs; // This is an indexed draw call
    DrawAttrs.IndexType = currentIndexBuffer_->GetStride() > 2 ? Diligent::VT_UINT32 : Diligent::VT_UINT16; // Index type
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
// 	auto state = GetRenderState()->GetActiveState();
//     Urho3D::TextureAddressMode ws[2];
// 	ws[(int)Effekseer::TextureWrapType::Clamp] = Urho3D::TextureAddressMode::ADDRESS_CLAMP;
// 	ws[(int)Effekseer::TextureWrapType::Repeat] = Urho3D::TextureAddressMode::ADDRESS_WRAP;
// 
// 	Urho3D::TextureFilterMode fs[2];
// 	fs[(int)Effekseer::TextureFilterType::Linear] = Urho3D::TextureFilterMode::FILTER_BILINEAR;
// 	fs[(int)Effekseer::TextureFilterType::Nearest] = Urho3D::TextureFilterMode::FILTER_NEAREST;
// 
// 	for (int32_t i = 0; i < count; i++)
// 	{
// 		if (textures[i] == nullptr)
// 		{
// 			GetCurrentCommandList()->SetTexture(nullptr, ws[(int)state.TextureWrapTypes[i]], fs[(int)state.TextureFilterTypes[i]], i);
// 			GetCurrentCommandList()->SetTexture(nullptr, ws[(int)state.TextureWrapTypes[i]], fs[(int)state.TextureFilterTypes[i]], i);
// 		}
// 		else
// 		{
// 			auto texture = static_cast<Backend::Texture*>(textures[i].Get());
// 			auto t = texture->GetTexture().get();
// 			GetCurrentCommandList()->SetTexture(t, ws[(int)state.TextureWrapTypes[i]], fs[(int)state.TextureFilterTypes[i]], i);
// 			GetCurrentCommandList()->SetTexture(t, ws[(int)state.TextureWrapTypes[i]], fs[(int)state.TextureFilterTypes[i]], i);
// 		}
// 	}

    m_currentTextures_.resize(count);
    for (int32_t i = 0; i < count; i++) {
        if (textures[i] != nullptr) {
            m_currentTextures_[i] = textures[i];
        }
        else {
            m_currentTextures_[i].Reset();
        }
    }

//     const auto& uniformLayout = shader->GetUniformLayout();
//     if (uniformLayout == nullptr)
//     {
//         return;
//     }
//     const auto& samplerNames = uniformLayout->GetTextures();
//     for (int32_t i = 0; i < count; i++) {
//         if (textures[i] == nullptr) {
//             GetCurrentCommandList()->AddShaderResource(samplerNames[i].c_str(), nullptr);
//         } else {
//             auto texture = static_cast<EffekseerUrho3D::Texture*>(textures[i].Get());
//             GetCurrentCommandList()->AddShaderResource(samplerNames[i].c_str(), texture->GetTexture().get());
//         }
//     }
}

void RendererImplemented::ResetRenderState()
{
	m_renderState->GetActiveState().Reset();
	m_renderState->Update(true);
}

void AddVertexUniformLayout(Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout)
{
    using namespace Effekseer::Backend;

    int vsOffset = 0;

    auto storeVector = [&](const char* name)
    {
        uniformLayout.emplace_back(
            UniformLayoutElement{ShaderStageType::Vertex, name, UniformBufferLayoutElementType::Vector4, 1, vsOffset});
        vsOffset += sizeof(float[4]);
    };

    auto storeMatrix = [&](const char* name)
    {
        uniformLayout.emplace_back(
            UniformLayoutElement{ShaderStageType::Vertex, name, UniformBufferLayoutElementType::Matrix44, 1, vsOffset});
        vsOffset += sizeof(Effekseer::Matrix44);
    };

    storeMatrix("CBVS0.mCamera");
    storeMatrix("CBVS0.mCameraProj");
    storeVector("CBVS0.mUVInversed");
    storeVector("CBVS0.flipbookParameter1");
    storeVector("CBVS0.flipbookParameter2");
}

void AddPixelUniformLayout(Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout)
{
    using namespace Effekseer::Backend;

    int psOffset = 0;

    auto storeVector = [&](const char* name)
    {
        uniformLayout.emplace_back(
            UniformLayoutElement{ShaderStageType::Pixel, name, UniformBufferLayoutElementType::Vector4, 1, psOffset});
        psOffset += sizeof(float[4]);
    };

    storeVector("CBPS0.fLightDirection");
    storeVector("CBPS0.fLightColor");
    storeVector("CBPS0.fLightAmbient");
    storeVector("CBPS0.fFlipbookParameter");
    storeVector("CBPS0.fUVDistortionParameter");
    storeVector("CBPS0.fBlendTextureParameter");
    storeVector("CBPS0.fCameraFrontDirection");
    storeVector("CBPS0.fFalloffParameter");
    storeVector("CBPS0.fFalloffBeginColor");
    storeVector("CBPS0.fFalloffEndColor");
    storeVector("CBPS0.fEmissiveScaling");
    storeVector("CBPS0.fEdgeColor");
    storeVector("CBPS0.fEdgeParameter");
    storeVector("CBPS0.softParticleParam");
    storeVector("CBPS0.reconstructionParam1");
    storeVector("CBPS0.reconstructionParam2");
    storeVector("CBPS0.mUVInversedBack");
    storeVector("CBPS0.miscFlags");
}

void AddDistortionPixelUniformLayout(Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout)
{
    using namespace Effekseer::Backend;

    int psOffset = 0;

    auto storeVector = [&](const char* name)
    {
        uniformLayout.emplace_back(
            UniformLayoutElement{ShaderStageType::Pixel, name, UniformBufferLayoutElementType::Vector4, 1, psOffset});
        psOffset += sizeof(float[4]);
    };

    storeVector("CBPS0.g_scale");
    storeVector("CBPS0.mUVInversedBack");
    storeVector("CBPS0.fFlipbookParameter");
    storeVector("CBPS0.fUVDistortionParameter");
    storeVector("CBPS0.fBlendTextureParameter");
    storeVector("CBPS0.softParticleParam");
    storeVector("CBPS0.reconstructionParam1");
    storeVector("CBPS0.reconstructionParam2");
    storeVector("CBPS0.miscFlags");
}

Effekseer::CustomVector<Effekseer::CustomString<char>> GetTextureLocations(EffekseerRenderer::RendererShaderType type)
{
    Effekseer::CustomVector<Effekseer::CustomString<char>> texLoc;

    auto pushColor = [](Effekseer::CustomVector<Effekseer::CustomString<char>>& texLoc)
    { texLoc.emplace_back("colorTex"); };

    auto pushDepth = [](Effekseer::CustomVector<Effekseer::CustomString<char>>& texLoc)
    { texLoc.emplace_back("depthTex"); };

    auto pushBack = [](Effekseer::CustomVector<Effekseer::CustomString<char>>& texLoc)
    { texLoc.emplace_back("backTex"); };

    auto pushNormal = [](Effekseer::CustomVector<Effekseer::CustomString<char>>& texLoc)
    { texLoc.emplace_back("normalTex"); };

    auto pushAdvancedRendererParameterLoc = [](Effekseer::CustomVector<Effekseer::CustomString<char>>& texLoc) -> void
    {
        texLoc.emplace_back("alphaTex");
        texLoc.emplace_back("uvDistortionTex");
        texLoc.emplace_back("blendTex");
        texLoc.emplace_back("blendAlphaTex");
        texLoc.emplace_back("blendUVDistortionTex");
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
