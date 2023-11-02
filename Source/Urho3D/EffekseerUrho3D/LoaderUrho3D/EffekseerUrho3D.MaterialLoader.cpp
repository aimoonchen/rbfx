// #include "EffekseerUrho3D.MaterialLoader.h"
// #include "../RendererUrho3D/EffekseerUrho3D.ShaderGenerator.h"
// #include "../RendererUrho3D/EffekseerUrho3D.Shader.h"
// #include "../RendererUrho3D/EffekseerUrho3D.RenderResources.h"
// #include "../Utils/EffekseerUrho3D.Utils.h"
// #include "../EffekseerResource.h"
// #include "../../Core/Context.h"
// #include "../../Resource/ResourceCache.h"
// #include "../../Cocos2D/Urho3DContext.h"
// 
// namespace EffekseerUrho3D
// {
// 
// ::Effekseer::MaterialRef MaterialLoader::Load(const char16_t* path)
// {
//     static auto cache = GetUrho3DContext()->GetSubsystem<Urho3D::ResourceCache>();
//     Urho3D::String urho3dPath = ToGdString(path);
//     auto urhoFile = cache->GetFile(urho3dPath);
//     auto dataSize = urhoFile->GetSize();
//     auto data = std::make_unique<char[]>(dataSize);
//     if (urhoFile->Read(data.get(), dataSize) != dataSize) {
//         return nullptr;
// 	}
// 	return Load(data.get(), dataSize, Effekseer::MaterialFileType::Code);
// }
// 
// ::Effekseer::MaterialRef MaterialLoader::LoadAcutually(const ::Effekseer::MaterialFile& materialFile)
// {
// 	using namespace EffekseerRenderer;
// 
// 	auto material = ::Effekseer::MakeRefPtr<::Effekseer::Material>();
// 	material->IsSimpleVertex = materialFile.GetIsSimpleVertex();
// 	material->IsRefractionRequired = materialFile.GetHasRefraction();
// 
// 	ShaderGenerator shaderGenerator;
// 	auto shaderDataList = shaderGenerator.Generate(materialFile);
// 
// 	{
// 		auto shader = Shader::Create(nullptr, "Custom_Sprite", RendererShaderType::Material);
// 		shader->SetVertexConstantBufferSize(shaderDataList[0].VertexConstantBufferSize);
// 		shader->SetPixelConstantBufferSize(shaderDataList[0].PixelConstantBufferSize);
// 		shader->Compile(Shader::RenderType::SpatialLightweight, shaderDataList[0].CodeSpatial.c_str(), std::vector<Shader::ParamDecl>(shaderDataList[0].ParamDecls));
// 		shader->Compile(Shader::RenderType::CanvasItem, shaderDataList[0].CodeCanvasItem.c_str(), std::vector<Shader::ParamDecl>(shaderDataList[0].ParamDecls));
// 		material->UserPtr = shader.release();
// 	}
// 	{
//         auto shader = Shader::Create(nullptr, "Model_Custom", RendererShaderType::Material);
// 		shader->SetVertexConstantBufferSize(shaderDataList[1].VertexConstantBufferSize);
// 		shader->SetPixelConstantBufferSize(shaderDataList[1].PixelConstantBufferSize);
// 		shader->Compile(Shader::RenderType::SpatialLightweight, shaderDataList[1].CodeSpatial.c_str(), std::vector<Shader::ParamDecl>(shaderDataList[1].ParamDecls));
// 		shader->Compile(Shader::RenderType::CanvasItem, shaderDataList[1].CodeCanvasItem.c_str(), std::vector<Shader::ParamDecl>(shaderDataList[1].ParamDecls));
// 		material->ModelUserPtr = shader.release();
// 	}
// 
// 	if (materialFile.GetHasRefraction())
// 	{
// 	}
// 
// 	material->CustomData1 = materialFile.GetCustomData1Count();
// 	material->CustomData2 = materialFile.GetCustomData2Count();
// 	material->TextureCount = std::min(materialFile.GetTextureCount(), Effekseer::UserTextureSlotMax);
// 	material->UniformCount = materialFile.GetUniformCount();
// 	material->ShadingModel = materialFile.GetShadingModel();
// 
// 	for (int32_t i = 0; i < material->TextureCount; i++)
// 	{
// 		material->TextureWrapTypes.at(i) = materialFile.GetTextureWrap(i);
// 	}
// 
// 	return material;
// }
// 
// ::Effekseer::MaterialRef MaterialLoader::Load(const void* data, int32_t size, Effekseer::MaterialFileType fileType)
// {
// 	Effekseer::MaterialFile materialFile;
// 
// 	if (materialFile.Load((const uint8_t*)data, size))
// 	{
// 		return LoadAcutually(materialFile);
// 	}
// 
// 	return nullptr;
// }
// 
// void MaterialLoader::Unload(::Effekseer::MaterialRef data)
// {
// 	if (data == nullptr)
// 		return;
// 
// 	auto shader = reinterpret_cast<Shader*>(data->UserPtr);
// 	auto modelShader = reinterpret_cast<Shader*>(data->ModelUserPtr);
// 	auto refractionShader = reinterpret_cast<Shader*>(data->RefractionUserPtr);
// 	auto refractionModelShader = reinterpret_cast<Shader*>(data->RefractionModelUserPtr);
// 
// 	ES_SAFE_DELETE(shader);
// 	ES_SAFE_DELETE(modelShader);
// 	ES_SAFE_DELETE(refractionShader);
// 	ES_SAFE_DELETE(refractionModelShader);
// 
// 	data->UserPtr = nullptr;
// 	data->ModelUserPtr = nullptr;
// 	data->RefractionUserPtr = nullptr;
// 	data->RefractionModelUserPtr = nullptr;
// }
// 
// } // namespace EffekseerUrho3D
