
#include "EffekseerUrho3D.RenderState.h"
#include "EffekseerUrho3D.RendererImplemented.h"

#include "EffekseerUrho3D.ModelRenderer.h"
#include "EffekseerUrho3D.Shader.h"

#include "GraphicsDevice.h"

namespace EffekseerUrho3D
{
const int LLGI_InstanceCount = 10;

ModelRenderer::ModelRenderer(RendererImplemented* renderer,
							 Shader* shader_ad_lit,
							 Shader* shader_ad_unlit,
							 Shader* shader_ad_distortion,
							 Shader* shader_lighting_texture_normal,
							 Shader* shader_texture,
							 Shader* shader_distortion_texture)
	: m_renderer(renderer)
	, shader_ad_lit_(shader_ad_lit)
	, shader_ad_unlit_(shader_ad_unlit)
	, shader_ad_distortion_(shader_ad_distortion)
	, m_shader_lighting_texture_normal(shader_lighting_texture_normal)
	, m_shader_texture(shader_texture)
	, m_shader_distortion_texture(shader_distortion_texture)
{
	// Ad
	{
		std::array<Shader*, 2> shaders;
		shaders[0] = shader_ad_lit_;
		shaders[1] = shader_ad_unlit_;

		for (size_t i = 0; i < shaders.size(); i++)
		{
			shaders[i]->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererAdvancedVertexConstantBuffer<LLGI_InstanceCount>));
			shaders[i]->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::PixelConstantBuffer));
		}

		shader_ad_distortion_->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererAdvancedVertexConstantBuffer<LLGI_InstanceCount>));
		shader_ad_distortion_->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::PixelConstantBufferDistortion));
	}

	{
		std::array<Shader*, 2> shaders;
		shaders[0] = m_shader_lighting_texture_normal;
		shaders[1] = m_shader_texture;

		for (size_t i = 0; i < shaders.size(); i++)
		{
			shaders[i]->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<LLGI_InstanceCount>));
			shaders[i]->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::PixelConstantBuffer));
		}

		m_shader_distortion_texture->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<LLGI_InstanceCount>));
		m_shader_distortion_texture->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::PixelConstantBufferDistortion));
	}

	VertexType = EffekseerRenderer::ModelRendererVertexType::Instancing;

	graphicsDevice_ = m_renderer->GetGraphicsDeviceInternal().Get();
	//LLGI::SafeAddRef(graphicsDevice_);
}

ModelRenderer::~ModelRenderer()
{
	ES_SAFE_DELETE(m_shader_lighting_texture_normal);
	ES_SAFE_DELETE(m_shader_texture);
	ES_SAFE_DELETE(m_shader_distortion_texture);

	ES_SAFE_DELETE(shader_ad_unlit_);
	ES_SAFE_DELETE(shader_ad_lit_);
	ES_SAFE_DELETE(shader_ad_distortion_);

	//LLGI::SafeRelease(graphicsDevice_);
}

static const char* kShaderName[] = {
    "ModelUnlit",                   // RendererShaderType::Unlit
    "ModelLit",                     // RendererShaderType::Lit
    "ModelBackDistortion",          // RendererShaderType::BackDistortion
    "ModelAdvancedUnlit",           // RendererShaderType::AdvancedUnlit
    "ModelAdvancedLit",             // RendererShaderType::AdvancedLit
    "ModelAdvancedBackDistortion",  // RendererShaderType::AdvancedBackDistortion
};

static const char* kShaderFilepath[(int)EffekseerRenderer::RendererShaderType::Material][2] = {
    {"effekseer/builtin/model_unlit_vs",       "effekseer/builtin/model_unlit_ps"},                   // RendererShaderType::Unlit
    {"effekseer/builtin/model_lit_vs",           "effekseer/builtin/model_lit_ps"},                       // RendererShaderType::Lit
    {"effekseer/builtin/model_distortion_vs",    "effekseer/builtin/model_distortion_ps"},         // RendererShaderType::BackDistortion
    {"effekseer/builtin/ad_model_unlit_vs",      "effekseer/builtin/ad_model_unlit_ps"},             // RendererShaderType::AdvancedUnlit
    {"effekseer/builtin/ad_model_lit_vs",        "effekseer/builtin/ad_model_lit_ps"},                 // RendererShaderType::AdvancedLit
    {"effekseer/builtin/ad_model_distortion_vs", "effekseer/builtin/ad_model_distortion_ps"}    // RendererShaderType::AdvancedBackDistortion
};

// static const char* kShaderFilepath[(int)EffekseerRenderer::RendererShaderType::Material][2] = {
//     {"Shaders/HLSL/effekseer/builtin/model_unlit_vs.fx",         "Shaders/HLSL/effekseer/builtin/model_unlit_ps.fx"},                   // RendererShaderType::Unlit
//     {"Shaders/HLSL/effekseer/builtin/model_lit_vs.fx",           "Shaders/HLSL/effekseer/builtin/model_lit_ps.fx"},                       // RendererShaderType::Lit
//     {"Shaders/HLSL/effekseer/builtin/model_distortion_vs.fx",    "Shaders/HLSL/effekseer/builtin/model_distortion_ps.fx"},         // RendererShaderType::BackDistortion
//     {"Shaders/HLSL/effekseer/builtin/ad_model_unlit_vs.fx",      "Shaders/HLSL/effekseer/builtin/ad_model_unlit_ps.fx"},             // RendererShaderType::AdvancedUnlit
//     {"Shaders/HLSL/effekseer/builtin/ad_model_lit_vs.fx",        "Shaders/HLSL/effekseer/builtin/ad_model_lit_ps.fx"},                 // RendererShaderType::AdvancedLit
//     {"Shaders/HLSL/effekseer/builtin/ad_model_distortion_vs.fx", "Shaders/HLSL/effekseer/builtin/ad_model_distortion_ps.fx"}    // RendererShaderType::AdvancedBackDistortion
// };

ModelRendererRef ModelRenderer::Create(RendererImplemented* renderer, FixedShader* fixedShader)
{

	assert(renderer != nullptr);
	assert(renderer->GetRenderDevice() != nullptr);

	auto graphicsDevice = renderer->GetGraphicsDevice();
	auto vl = EffekseerRenderer::GetModelRendererVertexLayout(renderer->GetGraphicsDevice()).DownCast<Backend::VertexLayout>();
	vl->MakeGenerated();

    const auto texLocUnlit = GetTextureLocations(EffekseerRenderer::RendererShaderType::Unlit);
    const auto texLocLit = GetTextureLocations(EffekseerRenderer::RendererShaderType::Lit);
    const auto texLocDist = GetTextureLocations(EffekseerRenderer::RendererShaderType::BackDistortion);
    const auto texLocAdUnlit = GetTextureLocations(EffekseerRenderer::RendererShaderType::AdvancedUnlit);
    const auto texLocAdLit = GetTextureLocations(EffekseerRenderer::RendererShaderType::AdvancedLit);
    const auto texLocAdDist = GetTextureLocations(EffekseerRenderer::RendererShaderType::AdvancedBackDistortion);

    Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElementsPlaceHold;

	auto lit_vs_shader_data = Backend::Serialize(fixedShader->ModelLit_VS);
	auto lit_ps_shader_data = Backend::Serialize(fixedShader->ModelLit_PS);
    auto pathIndex = (int)EffekseerRenderer::RendererShaderType::Lit;
	Shader* shader_lighting_texture_normal = Shader::Create(graphicsDevice,
                                                            graphicsDevice.DownCast<Backend::GraphicsDevice>()->CreateShaderFromFile(
                                                                kShaderFilepath[pathIndex][0],
                                                                kShaderFilepath[pathIndex][1],
                                                                Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocLit, uniformLayoutElementsPlaceHold)),
															vl,
															"ModelRenderer Lit");

	auto unlit_vs_shader_data = Backend::Serialize(fixedShader->ModelUnlit_VS);
	auto unlit_ps_shader_data = Backend::Serialize(fixedShader->ModelUnlit_PS);
    pathIndex = (int)EffekseerRenderer::RendererShaderType::Unlit;
	Shader* shader_texture = Shader::Create(graphicsDevice,
                                            graphicsDevice.DownCast<Backend::GraphicsDevice>()->CreateShaderFromFile(
                                                kShaderFilepath[pathIndex][0],
                                                kShaderFilepath[pathIndex][1],
                                                Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocUnlit, uniformLayoutElementsPlaceHold)),
											vl,
											"ModelRenderer Unlit");

	auto dist_vs_shader_data = Backend::Serialize(fixedShader->ModelDistortion_VS);
	auto dist_ps_shader_data = Backend::Serialize(fixedShader->ModelDistortion_PS);
    pathIndex = (int)EffekseerRenderer::RendererShaderType::BackDistortion;
	auto shader_distortion_texture = Shader::Create(graphicsDevice,
                                                    graphicsDevice.DownCast<Backend::GraphicsDevice>()->CreateShaderFromFile(
                                                        kShaderFilepath[pathIndex][0],
                                                        kShaderFilepath[pathIndex][1],
                                                        Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocDist, uniformLayoutElementsPlaceHold)),
													vl,
													"ModelRenderer BackDistortion");

	auto ad_lit_vs_shader_data = Backend::Serialize(fixedShader->AdvancedModelLit_VS);
	auto ad_lit_ps_shader_data = Backend::Serialize(fixedShader->AdvancedModelLit_PS);
    pathIndex = (int)EffekseerRenderer::RendererShaderType::AdvancedLit;
	Shader* shader_ad_lit = Shader::Create(graphicsDevice,
                                           graphicsDevice.DownCast<Backend::GraphicsDevice>()->CreateShaderFromFile(
                                                kShaderFilepath[pathIndex][0],
                                                kShaderFilepath[pathIndex][1],
                                                Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocAdLit, uniformLayoutElementsPlaceHold)),
										   vl,
										   "ModelRenderer AdvancedLit");

	auto ad_unlit_vs_shader_data = Backend::Serialize(fixedShader->AdvancedModelUnlit_VS);
	auto ad_unlit_ps_shader_data = Backend::Serialize(fixedShader->AdvancedModelUnlit_PS);
    pathIndex = (int)EffekseerRenderer::RendererShaderType::AdvancedUnlit;
	Shader* shader_ad_unlit = Shader::Create(graphicsDevice,
                                            graphicsDevice.DownCast<Backend::GraphicsDevice>()->CreateShaderFromFile(
                                                kShaderFilepath[pathIndex][0],
                                                kShaderFilepath[pathIndex][1],
                                                Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocAdUnlit, uniformLayoutElementsPlaceHold)),
											 vl,
											 "ModelRenderer AdvancedUnlit");

	auto ad_dist_vs_shader_data = Backend::Serialize(fixedShader->AdvancedModelDistortion_VS);
	auto ad_dist_ps_shader_data = Backend::Serialize(fixedShader->AdvancedModelDistortion_PS);
    pathIndex = (int)EffekseerRenderer::RendererShaderType::AdvancedBackDistortion;
	auto shader_ad_distortion = Shader::Create(graphicsDevice,
                                               graphicsDevice.DownCast<Backend::GraphicsDevice>()->CreateShaderFromFile(
                                                   kShaderFilepath[pathIndex][0],
                                                   kShaderFilepath[pathIndex][1],
                                                   Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocAdDist, uniformLayoutElementsPlaceHold)),
											   vl,
											   "ModelRenderer AdvancedBackDistortion");

	if (shader_lighting_texture_normal == nullptr || shader_texture == nullptr || shader_distortion_texture == nullptr ||
		shader_ad_lit == nullptr || shader_ad_unlit == nullptr || shader_ad_distortion == nullptr)
	{
		ES_SAFE_DELETE(shader_lighting_texture_normal);
		ES_SAFE_DELETE(shader_texture);
		ES_SAFE_DELETE(shader_distortion_texture);
		ES_SAFE_DELETE(shader_ad_unlit);
		ES_SAFE_DELETE(shader_ad_lit);
		ES_SAFE_DELETE(shader_ad_distortion);
	}

	return ModelRendererRef(new ModelRenderer(renderer, shader_ad_lit, shader_ad_unlit, shader_ad_distortion, shader_lighting_texture_normal, shader_texture, shader_distortion_texture));
}

void ModelRenderer::BeginRendering(const efkModelNodeParam& parameter, int32_t count, void* userData)
{
	BeginRendering_(m_renderer, parameter, count, userData);
}

void ModelRenderer::Rendering(const efkModelNodeParam& parameter, const InstanceParameter& instanceParameter, void* userData)
{
	Rendering_<RendererImplemented>(m_renderer, parameter, instanceParameter, userData);
}

void ModelRenderer::EndRendering(const efkModelNodeParam& parameter, void* userData)
{
	if (parameter.ModelIndex < 0)
	{
		return;
	}

	Effekseer::ModelRef model = nullptr;

	if (parameter.IsProceduralMode)
	{
		model = parameter.EffectPointer->GetProceduralModel(parameter.ModelIndex);
	}
	else
	{
		model = parameter.EffectPointer->GetModel(parameter.ModelIndex);
	}

	if (model == nullptr)
	{
		return;
	}

	model->StoreBufferToGPU(graphicsDevice_);
	if (!model->GetIsBufferStoredOnGPU())
	{
		return;
	}

	if (m_renderer->GetRenderMode() == Effekseer::RenderMode::Wireframe)
	{
		model->GenerateWireIndexBuffer(graphicsDevice_);
		if (!model->GetIsWireIndexBufferGenerated())
		{
			return;
		}
	}

	EndRendering_<RendererImplemented, Shader, Effekseer::Model, true, LLGI_InstanceCount>(
		m_renderer, shader_ad_lit_, shader_ad_unlit_, shader_ad_distortion_, m_shader_lighting_texture_normal, m_shader_texture, m_shader_distortion_texture, parameter, userData);
}

} // namespace EffekseerUrho3D