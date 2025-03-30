// #include "EffekseerBGFX.MaterialLoader.h"
// #include "../RendererUrho3D/EffekseerUrho3D.ModelRenderer.h"
// #include "../RendererUrho3D/EffekseerUrho3D.Shader.h"
// 
// #include <iostream>
// #include <fstream>
// #include <EASTL/string.h>
// #include "../EffekseerMaterialCompiler/EffekseerMaterialCompilerBGFX.h"
// #include "Effekseer/Material/Effekseer.CompiledMaterial.h"
// #include "../Utils/EffekseerUrho3D.Utils.h"
// #include "../../Core/Context.h"
// #include "../../IO/FileSystem.h"
// #include "../../Resource/ResourceCache.h"
// #include "../../Graphics/Graphics.h"
// #include "../../Cocos2D/Urho3DContext.h"
// 
// #undef min
// 
// namespace EffekseerUrho3D
// {
// 
// static const int GL_InstanceCount = 10;
// 
// ::Effekseer::MaterialRef MaterialLoader::LoadAcutually(::Effekseer::MaterialFile& materialFile, ::Effekseer::CompiledMaterialBinary* binary)
// {
// //	auto deviceType = graphicsDevice_->GetDeviceType();
// 
// 	auto instancing = false; // deviceType == OpenGLDeviceType::OpenGL3 || deviceType == OpenGLDeviceType::OpenGLES3;
// 
// 	auto material = ::Effekseer::MakeRefPtr<::Effekseer::Material>();
// 	material->IsSimpleVertex = materialFile.GetIsSimpleVertex();
// 	material->IsRefractionRequired = materialFile.GetHasRefraction();
// 
// 	std::array<Effekseer::MaterialShaderType, 2> shaderTypes;
// 	std::array<Effekseer::MaterialShaderType, 2> shaderTypesModel;
// 
// 	shaderTypes[0] = Effekseer::MaterialShaderType::Standard;
// 	shaderTypes[1] = Effekseer::MaterialShaderType::Refraction;
// 	shaderTypesModel[0] = Effekseer::MaterialShaderType::Model;
// 	shaderTypesModel[1] = Effekseer::MaterialShaderType::RefractionModel;
// 	int32_t shaderTypeCount = 1;
// 
// 	if (materialFile.GetHasRefraction())
// 	{
// 		shaderTypeCount = 2;
// 	}
//     auto context = GetUrho3DContext();
//     // TODO: runtime shader generate on mobile platform?
// //     auto dir = context->GetSubsystem<Urho3D::FileSystem>()->GetProgramDir() + "Assets/Engine/Shaders/BGFX/Effekseer/custom/";
// //     dir.replace("/build_2019/", "/");
// //     dir.replace("/build/", "/");
// //     dir.replace("/Debug/", "/");
// //     dir.replace("/Release/", "/");
//     ea::string dir = "Shaders/BGFX/Effekseer/custom/";
//     auto startPos = currentPath_.rfind('/');
//     auto fileName = ea::string(currentPath_.substr(startPos + 1, currentPath_.rfind('.') - startPos - 1).c_str());
//     
// 	auto create_shader = [context, &dir, &fileName, &material](::Effekseer::CompiledMaterialBinary* binary,
//                                                     Effekseer::MaterialShaderType type, bool isModel) {
//         auto vsFileName = "vs_" + fileName;
// 		if (isModel) {
//             vsFileName += "_model";
// 		}
//         auto shaderFile = std::make_unique<Urho3D::File>(GetUrho3DContext());
//         auto fullName = dir + vsFileName + ".sc";
//         auto exist = context->GetSubsystem<Urho3D::FileSystem>()->FileExists(fullName);
//         if (/*!exist && */shaderFile->Open(dir + vsFileName + ".sc", Urho3D::FILE_WRITE)) {
//             shaderFile->Write(binary->GetVertexShaderData(type), binary->GetVertexShaderSize(type));
//             shaderFile->Close();
//         }
//         auto fsFileName = "fs_" + fileName;
//         if (isModel) {
//             fsFileName += "_model";
//         }
//         fullName = dir + fsFileName + ".sc";
//         exist = context->GetSubsystem<Urho3D::FileSystem>()->FileExists(fullName);
//         if (true/*!exist*/) {
//             shaderFile = nullptr;
//             shaderFile = std::make_unique<Urho3D::File>(GetUrho3DContext());
//             if (shaderFile->Open(dir + fsFileName + ".sc", Urho3D::FILE_WRITE)) {
//                 shaderFile->Write(binary->GetPixelShaderData(type), binary->GetPixelShaderSize(type));
//                 shaderFile->Close();
//             }
//         }
//         return Shader::Create(context->GetSubsystem<Urho3D::Graphics>(), ("Effekseer/custom/" + vsFileName).c_str(),
//             ("Effekseer/custom/" + fsFileName).c_str(),
//             (!isModel && !material->IsSimpleVertex) ? "Effekseer/custom/pack_varying.def.sc" : "Effekseer/custom/varying.def.sc",
//             EffekseerRenderer::RendererShaderType::Material,
//             "CustomMaterial");
// 	};
// 
// 	for (int32_t st = 0; st < shaderTypeCount; st++)
// 	{
// 		auto parameterGenerator = EffekseerRenderer::MaterialShaderParameterGenerator(materialFile, false, st, 1);
// 
//         auto shader = create_shader(binary, shaderTypes[st], false);
//         shader->SetVertexConstantBufferSize(parameterGenerator.VertexShaderUniformBufferSize);
//         shader->SetPixelConstantBufferSize(parameterGenerator.PixelShaderUniformBufferSize);
// 
// 		shader->AddVertexUniformLayout("uMatCamera", parameterGenerator.VertexCameraMatrixOffset);
// 		shader->AddVertexUniformLayout("uMatProjection", parameterGenerator.VertexProjectionMatrixOffset);
// 		shader->AddVertexUniformLayout("mUVInversed", parameterGenerator.VertexInversedFlagOffset);
// 		shader->AddVertexUniformLayout("vs_predefined_uniform", parameterGenerator.VertexPredefinedOffset);
// 		shader->AddVertexUniformLayout("vs_cameraPosition", parameterGenerator.VertexCameraPositionOffset);
// 
// 		for (int32_t ui = 0; ui < materialFile.GetUniformCount(); ui++)
// 		{
//             auto name = std::string("vs_") + materialFile.GetUniformName(ui);
//             shader->AddVertexUniformLayout(name.c_str(), parameterGenerator.VertexUserUniformOffset + sizeof(float) * 4 * ui);
// 		}
// 		shader->AddPixelUniformLayout("mUVInversedBack", parameterGenerator.PixelInversedFlagOffset);
// 		shader->AddPixelUniformLayout("fs_predefined_uniform", parameterGenerator.PixelPredefinedOffset);
// 		shader->AddPixelUniformLayout("fs_cameraPosition", parameterGenerator.PixelCameraPositionOffset);
// 		shader->AddPixelUniformLayout("reconstructionParam1", parameterGenerator.PixelReconstructionParam1Offset);
// 		shader->AddPixelUniformLayout("reconstructionParam2", parameterGenerator.PixelReconstructionParam2Offset);
// 
// 		// shiding model
// 		if (materialFile.GetShadingModel() == ::Effekseer::ShadingModelType::Lit)
// 		{
//             shader->AddPixelUniformLayout("lightDirection", parameterGenerator.PixelLightDirectionOffset);
//             shader->AddPixelUniformLayout("lightColor", parameterGenerator.PixelLightColorOffset);
//             shader->AddPixelUniformLayout("lightAmbientColor", parameterGenerator.PixelLightAmbientColorOffset);
// 		}
// 		else if (materialFile.GetShadingModel() == ::Effekseer::ShadingModelType::Unlit)
// 		{
// 		}
// 
// 		if (materialFile.GetHasRefraction() && st == 1)
// 		{
//             shader->AddPixelUniformLayout("cameraMat", parameterGenerator.PixelCameraMatrixOffset);
// 		}
// 
// 		for (int32_t ui = 0; ui < materialFile.GetUniformCount(); ui++)
// 		{
//             auto name = std::string("fs_") + materialFile.GetUniformName(ui);
//             shader->AddPixelUniformLayout(name.c_str(), parameterGenerator.PixelUserUniformOffset + sizeof(float) * 4 * ui);
// 		}
// 
//         if (material->IsSimpleVertex)
//         {
//             ea::vector<Effekseer::Backend::VertexLayoutElement> layout = {
//                 {Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "atPosition", "POSITION", 0},
//                 {Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "atColor", "NORMAL", 0},
//                 {Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "atTexCoord", "TEXCOORD", 0},
//             };
// 
//             //auto vl = graphicsDevice_->CreateVertexLayout(vlElem, 3).DownCast<Backend::VertexLayout>();
//             shader->SetVertexLayout(layout);
//         }
//         else
//         {
//             ea::vector<Effekseer::Backend::VertexLayoutElement> layout = {
//                 {Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "atPosition", "POSITION", 0},
//                 {Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "atColor", "NORMAL", 0},
//                 {Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "atNormal", "NORMAL", 1},
//                 {Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "atTangent", "NORMAL", 2},
//                 {Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "atTexCoord", "TEXCOORD", 0},
//                 {Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "atTexCoord2", "TEXCOORD", 1}
// //                 {Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "", "TEXCOORD", 2},
// //                 {Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "", "TEXCOORD", 3},
//             };
// 
//             auto getFormat = [](int32_t i) -> Effekseer::Backend::VertexLayoutFormat
//             {
//                 if (i == 1)
//                     return Effekseer::Backend::VertexLayoutFormat::R32_FLOAT;
//                 if (i == 2)
//                     return Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT;
//                 if (i == 3)
//                     return Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT;
//                 if (i == 4)
//                     return Effekseer::Backend::VertexLayoutFormat::R32G32B32A32_FLOAT;
// 
//                 assert(0);
//                 return Effekseer::Backend::VertexLayoutFormat::R32_FLOAT;
//             };
// 
//             int32_t offset = 40;
//             int count = 6;
//             int semanticIndex = 2;
//             const char* customData1Name = "atCustomData1";
//             const char* customData2Name = "atCustomData2";
// 
//             if (materialFile.GetCustomData1Count() > 0)
//             {
//                 auto& nl = layout.emplace_back();
//                 nl.Name = customData1Name;
//                 nl.Format = getFormat(materialFile.GetCustomData1Count());
//                 nl.SemanticName = "TEXCOORD";
//                 nl.SemanticIndex = semanticIndex;
//                 semanticIndex++;
// 
//                 count++;
//                 offset += sizeof(float) * materialFile.GetCustomData1Count();
//             }
// 
//             if (materialFile.GetCustomData2Count() > 0)
//             {
//                 auto& nl = layout.emplace_back();
//                 nl.Name = customData2Name;
//                 nl.Format = getFormat(materialFile.GetCustomData2Count());
//                 nl.SemanticName = "TEXCOORD";
//                 nl.SemanticIndex = semanticIndex;
//                 semanticIndex++;
// 
//                 count++;
//                 offset += sizeof(float) * materialFile.GetCustomData2Count();
//             }
// 
//             //auto vl = graphicsDevice_->CreateVertexLayout(vlElem, count).DownCast<Backend::VertexLayout>();
//             shader->SetVertexLayout(layout);
//         }
// 
// 		int32_t lastIndex = -1;
// 		for (int32_t ti = 0; ti < materialFile.GetTextureCount(); ti++)
// 		{
// 			shader->SetTextureSlot(materialFile.GetTextureIndex(ti), shader->GetUniformId(materialFile.GetTextureName(ti)));
// 			lastIndex = Effekseer::Max(lastIndex, materialFile.GetTextureIndex(ti));
// 		}
// 
// 		lastIndex += 1;
// 		shader->SetTextureSlot(lastIndex, shader->GetUniformId("efk_background"));
// 
// 		lastIndex += 1;
// 		shader->SetTextureSlot(lastIndex, shader->GetUniformId("efk_depth"));
// 
// 		material->TextureCount = materialFile.GetTextureCount();
// 		material->UniformCount = materialFile.GetUniformCount();
// 
// 		if (st == 0)
// 		{
// 			material->UserPtr = shader.release();
// 		}
// 		else
// 		{
// 			material->RefractionUserPtr = shader.release();
// 		}
// 	}
// 
// 	for (int32_t st = 0; st < shaderTypeCount; st++)
// 	{
// 		auto parameterGenerator = EffekseerRenderer::MaterialShaderParameterGenerator(materialFile, true, st, instancing ? GL_InstanceCount : 1);
// 	
// 		auto shader = create_shader(binary, shaderTypesModel[st], true);
//         shader->SetVertexConstantBufferSize(parameterGenerator.VertexShaderUniformBufferSize);
//         shader->SetPixelConstantBufferSize(parameterGenerator.PixelShaderUniformBufferSize);
// 		shader->AddVertexUniformLayout("ProjectionMatrix", parameterGenerator.VertexProjectionMatrixOffset);
// 
// 		if (instancing)
// 		{
//             shader->AddVertexUniformLayout("ModelMatrix", parameterGenerator.VertexModelMatrixOffset);
// 			shader->AddVertexUniformLayout("UVOffset", parameterGenerator.VertexModelUVOffset);
// 			shader->AddVertexUniformLayout("ModelColor", parameterGenerator.VertexModelColorOffset);
// 		}
// 		else
// 		{
//             shader->AddVertexUniformLayout("ModelMatrix", parameterGenerator.VertexModelMatrixOffset);
// 			shader->AddVertexUniformLayout("UVOffset", parameterGenerator.VertexModelUVOffset);
// 			shader->AddVertexUniformLayout("ModelColor", parameterGenerator.VertexModelColorOffset);
// 		}
// 
// 		shader->AddVertexUniformLayout("mUVInversed", parameterGenerator.VertexInversedFlagOffset);
// 		shader->AddVertexUniformLayout("vs_predefined_uniform", parameterGenerator.VertexPredefinedOffset);
// 		shader->AddVertexUniformLayout("vs_cameraPosition", parameterGenerator.VertexCameraPositionOffset);
// 		shader->AddPixelUniformLayout("reconstructionParam1", parameterGenerator.PixelReconstructionParam1Offset);
// 		shader->AddPixelUniformLayout("reconstructionParam2", parameterGenerator.PixelReconstructionParam2Offset);
// 
// 		if (instancing)
// 		{
// 			if (materialFile.GetCustomData1Count() > 0)
// 			{
//                 shader->AddVertexUniformLayout("customData1s", parameterGenerator.VertexModelCustomData1Offset);
// 			}
// 
// 			if (materialFile.GetCustomData2Count() > 0)
// 			{
//                 shader->AddVertexUniformLayout("customData2s", parameterGenerator.VertexModelCustomData2Offset);
// 			}
// 		}
// 		else
// 		{
// 			if (materialFile.GetCustomData1Count() > 0)
// 			{
//                 shader->AddVertexUniformLayout("customData1", parameterGenerator.VertexModelCustomData1Offset);
// 			}
// 
// 			if (materialFile.GetCustomData2Count() > 0)
// 			{
//                 shader->AddVertexUniformLayout("customData2", parameterGenerator.VertexModelCustomData2Offset);
// 			}
// 		}
// 
// 		for (int32_t ui = 0; ui < materialFile.GetUniformCount(); ui++)
// 		{
// 			auto name = std::string("vs_") + materialFile.GetUniformName(ui);
//             shader->AddVertexUniformLayout(name.c_str(), parameterGenerator.VertexUserUniformOffset + sizeof(float) * 4 * ui);
// 		}
// 		shader->AddPixelUniformLayout("mUVInversedBack", parameterGenerator.PixelInversedFlagOffset);
// 		shader->AddPixelUniformLayout("fs_predefined_uniform", parameterGenerator.PixelPredefinedOffset);
// 		shader->AddPixelUniformLayout("fs_cameraPosition", parameterGenerator.PixelCameraPositionOffset);
// 
// 		// shiding model
// 		if (materialFile.GetShadingModel() == ::Effekseer::ShadingModelType::Lit)
// 		{
// 			shader->AddPixelUniformLayout("lightDirection", parameterGenerator.PixelLightDirectionOffset);
// 			shader->AddPixelUniformLayout("lightColor", parameterGenerator.PixelLightColorOffset);
// 			shader->AddPixelUniformLayout("lightAmbientColor", parameterGenerator.PixelLightAmbientColorOffset);
// 		}
// 		else if (materialFile.GetShadingModel() == ::Effekseer::ShadingModelType::Unlit)
// 		{
// 		}
// 
// 		if (materialFile.GetHasRefraction() && st == 1)
// 		{
// 			shader->AddPixelUniformLayout("cameraMat", parameterGenerator.PixelCameraMatrixOffset);
// 		}
// 
// 		for (int32_t ui = 0; ui < materialFile.GetUniformCount(); ui++)
// 		{
// 			auto name = std::string("fs_") + materialFile.GetUniformName(ui);
// 			shader->AddPixelUniformLayout(name.c_str(), parameterGenerator.PixelUserUniformOffset + sizeof(float) * 4 * ui);
// 		}
// 
//         ea::vector<Effekseer::Backend::VertexLayoutElement> vlElem = {
//             {Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "a_Position", "POSITION", 0},
//             {Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "a_Color", "NORMAL", 0},
//             {Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "a_Normal", "NORMAL", 1},
//             {Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "a_Tangent", "NORMAL", 2},
//             {Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "a_Binormal", "NORMAL", 3},
//             {Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "a_TexCoord", "TEXCOORD", 0}
//         };
// 
//         //auto vl = graphicsDevice_->CreateVertexLayout(vlElem, 6).DownCast<Backend::VertexLayout>();
//         shader->SetVertexLayout(vlElem);
// 
// 		int32_t lastIndex = -1;
// 		for (int32_t ti = 0; ti < materialFile.GetTextureCount(); ti++)
// 		{
// 			shader->SetTextureSlot(materialFile.GetTextureIndex(ti), shader->GetUniformId(materialFile.GetTextureName(ti)));
// 			lastIndex = Effekseer::Max(lastIndex, materialFile.GetTextureIndex(ti));
// 		}
// 
// 		lastIndex += 1;
// 		shader->SetTextureSlot(lastIndex, shader->GetUniformId("efk_background"));
// 
// 		lastIndex += 1;
// 		shader->SetTextureSlot(lastIndex, shader->GetUniformId("efk_depth"));
// 
// 		if (st == 0)
// 		{
// 			material->ModelUserPtr = shader.release();
// 		}
// 		else
// 		{
// 			material->RefractionModelUserPtr = shader.release();
// 		}
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
// MaterialLoader::MaterialLoader(bool canLoadFromCache)
// 	: canLoadFromCache_(canLoadFromCache)
// {
// 
// }
// 
// MaterialLoader ::~MaterialLoader()
// {
// }
// 
// ::Effekseer::MaterialRef MaterialLoader::Load(const char16_t* path)
// {
// 	// code file
// 	if (canLoadFromCache_)
// 	{
// 		auto binaryPath = std::u16string(path) + u"d";
// 		std::unique_ptr<Effekseer::FileReader> reader(fileInterface_->TryOpenRead(binaryPath.c_str()));
// 
// 		if (reader.get() != nullptr)
// 		{
// 			size_t size = reader->GetLength();
// 			std::vector<char> data;
// 			data.resize(size);
// 			reader->Read(data.data(), size);
// 
// 			auto material = Load(data.data(), (int32_t)size, ::Effekseer::MaterialFileType::Compiled);
// 
// 			if (material != nullptr)
// 			{
// 				return material;
// 			}
// 		}
// 	}
// 
// 	// code file
// 	{
// //		std::unique_ptr<Effekseer::FileReader> reader(fileInterface_->OpenRead(path));
//         static auto cache = GetUrho3DContext()->GetSubsystem<Urho3D::ResourceCache>();
// 		auto urhoPath = ToGdString(path);
// 		currentPath_ = urhoPath.c_str();
//         auto urhoFile = cache->GetFile(urhoPath);
//         auto size = urhoFile->GetSize();
//         auto data = std::make_unique<char[]>(size);
//         if (urhoFile->Read(data.get(), size) != size)
//         {
//             return nullptr;
//         }
// 
// // 		if (reader.get() != nullptr)
// // 		{
// // 			size_t size = reader->GetLength();
// // 			std::vector<char> data;
// // 			data.resize(size);
// // 			reader->Read(data.data(), size);
// 
// 			auto material = Load(data.get(), (int32_t)size, ::Effekseer::MaterialFileType::Code);
// 
// 			return material;
// //		}
// 	}
// 
// 	return nullptr;
// }
// 
// ::Effekseer::MaterialRef MaterialLoader::Load(const void* data, int32_t size, Effekseer::MaterialFileType fileType)
// {
// 	if (fileType == Effekseer::MaterialFileType::Compiled)
// 	{
// 		auto compiled = Effekseer::CompiledMaterial();
// 		if (!compiled.Load(static_cast<const uint8_t*>(data), size))
// 		{
// 			return nullptr;
// 		}
// 
// 		if (!compiled.GetHasValue(::Effekseer::CompiledMaterialPlatformType::OpenGL))
// 		{
// 			return nullptr;
// 		}
// 
// 		// compiled
// 		Effekseer::MaterialFile materialFile;
// 		if (!materialFile.Load((const uint8_t*)compiled.GetOriginalData().data(), static_cast<int32_t>(compiled.GetOriginalData().size())))
// 		{
// 			std::cout << "Error : Invalid material is loaded." << std::endl;
// 			return nullptr;
// 		}
// 
// 		auto binary = compiled.GetBinary(::Effekseer::CompiledMaterialPlatformType::OpenGL);
// 
// 		return LoadAcutually(materialFile, binary);
// 	}
// 	else
// 	{
// 		Effekseer::MaterialFile materialFile;
// 		if (!materialFile.Load((const uint8_t*)data, size))
// 		{
// 			std::cout << "Error : Invalid material is loaded." << std::endl;
// 			return nullptr;
// 		}
// 
// 		auto compiler = ::Effekseer::CreateUniqueReference(new Effekseer::MaterialCompilerBGFX());
// 		auto binary = ::Effekseer::CreateUniqueReference(compiler->Compile(&materialFile));
// 
// 		return LoadAcutually(materialFile, binary.get());
// 	}
// }
// 
// void MaterialLoader::Unload(::Effekseer::MaterialRef data)
// {
// 	if (data == nullptr)
// 		return;
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
// } // namespace EffekseerRendererGL
