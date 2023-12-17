#include "EffekseerUrho3D.MaterialLoader.h"
#include "EffekseerUrho3D.ModelRenderer.h"
#include "EffekseerUrho3D.Shader.h"
#include <iostream>
#include <string>

#include "Effekseer/Material/Effekseer.CompiledMaterial.h"
#include "../Utils/EffekseerUrho3D.Compiler.h"
#include "../Utils/EffekseerUrho3D.Utils.h"
#include "../../Core/Context.h"
#include "../../IO/FileSystem.h"
#include "../../IO/File.h"
//#include "../../IO/VirtualFileSystem.h"
#include "../../RenderAPI/RenderDevice.h"

#undef min

namespace EffekseerUrho3D
{

static const int s_InstanceCount = 10;

Effekseer::CustomVector<Effekseer::CustomString<char>> StoreTextureLocations(
    const ::Effekseer::MaterialFile& materialFile)
{
    Effekseer::CustomVector<Effekseer::CustomString<char>> texLoc;

    int32_t maxInd = -1;
    for (int32_t ti = 0; ti < materialFile.GetTextureCount(); ti++)
    {
        maxInd = Effekseer::Max(maxInd, materialFile.GetTextureIndex(ti));
    }

    texLoc.resize(maxInd + 1);
    for (int32_t ti = 0; ti < materialFile.GetTextureCount(); ti++)
    {
        texLoc[materialFile.GetTextureIndex(ti)] = materialFile.GetTextureName(ti);
    }

    texLoc.emplace_back("efk_background");
    texLoc.emplace_back("efk_depth");
    return texLoc;
}

void MaterialLoader::Deserialize(uint8_t* data, uint32_t datasize, LLGI::CompilerResult& result)
{
	if (datasize < 4)
		return;

	uint32_t count = 0;
	uint32_t offset = 0;

	memcpy(&count, data + offset, sizeof(int32_t));
	offset += sizeof(uint32_t);

	result.Binary.resize(count);

	for (uint32_t i = 0; i < count; i++)
	{
		uint32_t size = 0;
		memcpy(&size, data + offset, sizeof(int32_t));
		offset += sizeof(uint32_t);

		result.Binary[i].resize(size);

		memcpy(result.Binary[i].data(), data + offset, size);
		offset += size;
	}
}

MaterialLoader::MaterialLoader(Backend::GraphicsDeviceRef graphicsDevice,
							   ::Effekseer::FileInterfaceRef fileInterface,
							   ::Effekseer::CompiledMaterialPlatformType platformType,
							   ::Effekseer::MaterialCompiler* materialCompiler)
	: fileInterface_(fileInterface)
	, platformType_(platformType)
	, materialCompiler_(materialCompiler)
{
	if (fileInterface == nullptr)
	{
		fileInterface_ = Effekseer::MakeRefPtr<Effekseer::DefaultFileInterface>();
	}

	graphicsDevice_ = graphicsDevice;
	ES_SAFE_ADDREF(materialCompiler_);
}

MaterialLoader ::~MaterialLoader()
{
	ES_SAFE_RELEASE(materialCompiler_);
}

::Effekseer::MaterialRef MaterialLoader::Load(const char16_t* path)
{
    currentPath_ = ToGdString(path);
	// code file
	{
		auto binaryPath = std::u16string(path) + u"d";
		auto reader = fileInterface_->OpenRead(binaryPath.c_str());

		if (reader != nullptr)
		{
			size_t size = reader->GetLength();
			std::vector<char> data;
			data.resize(size);
			reader->Read(data.data(), size);

			auto material = Load(data.data(), (int32_t)size, ::Effekseer::MaterialFileType::Compiled);

			if (material != nullptr)
			{
				return material;
			}
		}
	}

	// code file
	if (materialCompiler_ != nullptr)
	{
		auto reader = fileInterface_->OpenRead(path);

		if (reader != nullptr)
		{
			size_t size = reader->GetLength();
			std::vector<char> data;
			data.resize(size);
			reader->Read(data.data(), size);

			auto material = Load(data.data(), (int32_t)size, ::Effekseer::MaterialFileType::Code);

			return material;
		}
	}

	return nullptr;
}

::Effekseer::MaterialRef MaterialLoader::LoadAcutually(::Effekseer::MaterialFile& materialFile, ::Effekseer::CompiledMaterialBinary* binary)
{
	if (binary == nullptr)
	{
		return nullptr;
	}

	auto material = ::Effekseer::MakeRefPtr<::Effekseer::Material>();
	material->IsSimpleVertex = materialFile.GetIsSimpleVertex();
	material->IsRefractionRequired = materialFile.GetHasRefraction();

	std::array<Effekseer::MaterialShaderType, 2> shaderTypes;
	std::array<Effekseer::MaterialShaderType, 2> shaderTypesModel;

	shaderTypes[0] = Effekseer::MaterialShaderType::Standard;
	shaderTypes[1] = Effekseer::MaterialShaderType::Refraction;
	shaderTypesModel[0] = Effekseer::MaterialShaderType::Model;
	shaderTypesModel[1] = Effekseer::MaterialShaderType::RefractionModel;
	int32_t shaderTypeCount = 1;

	if (materialFile.GetHasRefraction())
	{
		shaderTypeCount = 2;
	}
    // TODO: runtime shader generate on mobile platform?
    auto dir = graphicsDevice_->GetRenderDevice()->GetContext()->GetSubsystem<Urho3D::FileSystem>()->GetProgramDir() + "Assets/Engine/Shaders/GLSL/effekseer/custom/";
//     dir.replace("/build/", "/");
//     dir.replace("/Debug/", "/");
//     dir.replace("/Release/", "/");
    //ea::string dir = "Shaders/HLSL/effekseer/custom/";
    auto startPos = currentPath_.rfind('/');
    auto fileName = ea::string(currentPath_.substr(startPos + 1, currentPath_.rfind('.') - startPos - 1).c_str());

    Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElements;
    auto uniformLayout = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(StoreTextureLocations(materialFile), uniformLayoutElements);

    auto create_shader = [this, &dir, &fileName](::Effekseer::CompiledMaterialBinary* binary,
                             Effekseer::MaterialShaderType type, Effekseer::Backend::UniformLayoutRef uniformLayout, bool isModel)
    {
        auto context = graphicsDevice_->GetRenderDevice()->GetContext();
        //auto vfs = context->GetSubsystem<Urho3D::VirtualFileSystem>();
        auto vsFileName = "vs_" + fileName;
        if (isModel) {
            vsFileName += "_model";
        }
        auto shaderFile = std::make_unique<Urho3D::File>(context);
        auto fullName = dir + vsFileName + ".glsl";
        auto exist = context->GetSubsystem<Urho3D::FileSystem>()->FileExists(fullName);
        if (/*!exist && */ shaderFile->Open(dir + vsFileName + ".glsl", Urho3D::FILE_WRITE)) {
            shaderFile->Write(binary->GetVertexShaderData(type), binary->GetVertexShaderSize(type));
            shaderFile->Close();
        }
        auto fsFileName = "fs_" + fileName;
        if (isModel) {
            fsFileName += "_model";
        }
        fullName = dir + fsFileName + ".glsl";
        exist = context->GetSubsystem<Urho3D::FileSystem>()->FileExists(fullName);
        shaderFile = std::make_unique<Urho3D::File>(context);
        if (/*!exist && */ shaderFile->Open(dir + fsFileName + ".glsl", Urho3D::FILE_WRITE)) {
            shaderFile->Write(binary->GetPixelShaderData(type), binary->GetPixelShaderSize(type));
            shaderFile->Close();
        }
        return graphicsDevice_->CreateShaderFromFile(("effekseer/custom/" + vsFileName).c_str(), ("effekseer/custom/" + fsFileName).c_str(), uniformLayout);
    };
	for (int32_t st = 0; st < shaderTypeCount; st++)
	{
		Shader* shader = nullptr;
		auto parameterGenerator = EffekseerRenderer::MaterialShaderParameterGenerator(materialFile, false, st, s_InstanceCount);
		if (material->IsSimpleVertex)
		{
            /*
			LLGI::CompilerResult resultVS;
			LLGI::CompilerResult resultPS;

			Deserialize((uint8_t*)binary->GetVertexShaderData(shaderTypes[st]), binary->GetVertexShaderSize(shaderTypes[st]), resultVS);
			Deserialize((uint8_t*)binary->GetPixelShaderData(shaderTypes[st]), binary->GetPixelShaderSize(shaderTypes[st]), resultPS);

			std::vector<LLGI::DataStructure> dataVS;
			std::vector<LLGI::DataStructure> dataPS;

			for (size_t i = 0; i < resultVS.Binary.size(); i++)
			{
				LLGI::DataStructure ds;
				ds.Data = resultVS.Binary[i].data();
				ds.Size = static_cast<int32_t>(resultVS.Binary[i].size());
				dataVS.emplace_back(ds);
			}

			for (size_t i = 0; i < resultPS.Binary.size(); i++)
			{
				LLGI::DataStructure ds;
				ds.Data = resultPS.Binary[i].data();
				ds.Size = static_cast<int32_t>(resultPS.Binary[i].size());
				dataPS.emplace_back(ds);
			}
            */
			auto vl = EffekseerRenderer::GetMaterialSimpleVertexLayout(graphicsDevice_).DownCast<Backend::VertexLayout>();

// 			auto vs_shader_data = Backend::Serialize(dataVS);
// 			auto ps_shader_data = Backend::Serialize(dataPS);

			shader = Shader::Create(
				graphicsDevice_,
// 				graphicsDevice_->CreateShaderFromBinaryEx(
// 					vs_shader_data.data(),
// 					(int32_t)vs_shader_data.size(),
// 					ps_shader_data.data(),
// 					(int32_t)ps_shader_data.size(),
//                     uniformLayout),
                create_shader(binary, shaderTypes[st], uniformLayout, false),
				vl,
				"MaterialStandardRenderer Simple");
		}
		else
		{
            /*
			LLGI::CompilerResult resultVS;
			LLGI::CompilerResult resultPS;

			Deserialize((uint8_t*)binary->GetVertexShaderData(shaderTypes[st]), binary->GetVertexShaderSize(shaderTypes[st]), resultVS);
			Deserialize((uint8_t*)binary->GetPixelShaderData(shaderTypes[st]), binary->GetPixelShaderSize(shaderTypes[st]), resultPS);

			std::vector<LLGI::DataStructure> dataVS;
			std::vector<LLGI::DataStructure> dataPS;

			for (size_t i = 0; i < resultVS.Binary.size(); i++)
			{
				LLGI::DataStructure ds;
				ds.Data = resultVS.Binary[i].data();
				ds.Size = static_cast<int32_t>(resultVS.Binary[i].size());
				dataVS.emplace_back(ds);
			}

			for (size_t i = 0; i < resultPS.Binary.size(); i++)
			{
				LLGI::DataStructure ds;
				ds.Data = resultPS.Binary[i].data();
				ds.Size = static_cast<int32_t>(resultPS.Binary[i].size());
				dataPS.emplace_back(ds);
			}
            */
			auto vl = EffekseerRenderer::GetMaterialSpriteVertexLayout(graphicsDevice_, static_cast<int32_t>(materialFile.GetCustomData1Count()), static_cast<int32_t>(materialFile.GetCustomData2Count())).DownCast<Backend::VertexLayout>();

// 			auto vs_shader_data = Backend::Serialize(dataVS);
// 			auto ps_shader_data = Backend::Serialize(dataPS);

			shader = Shader::Create(graphicsDevice_,
// 									graphicsDevice_->CreateShaderFromBinaryEx(
// 										vs_shader_data.data(),
// 										(int32_t)vs_shader_data.size(),
// 										ps_shader_data.data(),
// 										(int32_t)ps_shader_data.size(),
//                                         uniformLayout),
                                    create_shader(binary, shaderTypes[st], uniformLayout, false),
									vl,
									"MaterialStandardRenderer Sprite");
		}

		if (shader == nullptr)
			return nullptr;

		auto vertexUniformSize = parameterGenerator.VertexShaderUniformBufferSize;
		auto pixelUniformSize = parameterGenerator.PixelShaderUniformBufferSize;

		shader->SetVertexConstantBufferSize(vertexUniformSize);
		shader->SetPixelConstantBufferSize(pixelUniformSize);

		material->TextureCount = materialFile.GetTextureCount();
		material->UniformCount = materialFile.GetUniformCount();

		if (st == 0)
		{
			material->UserPtr = shader;
		}
		else
		{
			material->RefractionUserPtr = shader;
		}
	}

	for (int32_t st = 0; st < shaderTypeCount; st++)
	{
        /*
		LLGI::CompilerResult resultVS;
		LLGI::CompilerResult resultPS;

		Deserialize(
			(uint8_t*)binary->GetVertexShaderData(shaderTypesModel[st]), binary->GetVertexShaderSize(shaderTypesModel[st]), resultVS);
		Deserialize((uint8_t*)binary->GetPixelShaderData(shaderTypesModel[st]), binary->GetPixelShaderSize(shaderTypesModel[st]), resultPS);

		std::vector<LLGI::DataStructure> dataVS;
		std::vector<LLGI::DataStructure> dataPS;

		for (size_t i = 0; i < resultVS.Binary.size(); i++)
		{
			LLGI::DataStructure ds;
			ds.Data = resultVS.Binary[i].data();
			ds.Size = static_cast<int32_t>(resultVS.Binary[i].size());
			dataVS.emplace_back(ds);
		}

		for (size_t i = 0; i < resultPS.Binary.size(); i++)
		{
			LLGI::DataStructure ds;
			ds.Data = resultPS.Binary[i].data();
			ds.Size = static_cast<int32_t>(resultPS.Binary[i].size());
			dataPS.emplace_back(ds);
		}
        */
		auto parameterGenerator = EffekseerRenderer::MaterialShaderParameterGenerator(materialFile, true, st, s_InstanceCount);

		// compile
		std::string log;
		auto vl = EffekseerRenderer::GetMaterialModelVertexLayout(graphicsDevice_).DownCast<Backend::VertexLayout>();

// 		auto vs_shader_data = Backend::Serialize(dataVS);
// 		auto ps_shader_data = Backend::Serialize(dataPS);

		auto shader = Shader::Create(graphicsDevice_,
// 									 graphicsDevice_->CreateShaderFromBinaryEx(
// 										 vs_shader_data.data(),
// 										 (int32_t)vs_shader_data.size(),
// 										 ps_shader_data.data(),
// 										 (int32_t)ps_shader_data.size(),
//                                          uniformLayout),
                                     create_shader(binary, shaderTypesModel[st], uniformLayout, true),
									 vl,
									 "MaterialStandardModelRenderer");

		if (shader == nullptr)
			return nullptr;

		auto vertexUniformSize = parameterGenerator.VertexShaderUniformBufferSize;
		auto pixelUniformSize = parameterGenerator.PixelShaderUniformBufferSize;

		shader->SetVertexConstantBufferSize(vertexUniformSize);
		shader->SetPixelConstantBufferSize(pixelUniformSize);

		if (st == 0)
		{
			material->ModelUserPtr = shader;
		}
		else
		{
			material->RefractionModelUserPtr = shader;
		}
	}

	material->CustomData1 = materialFile.GetCustomData1Count();
	material->CustomData2 = materialFile.GetCustomData2Count();
	material->TextureCount = std::min(materialFile.GetTextureCount(), Effekseer::UserTextureSlotMax);
	material->UniformCount = materialFile.GetUniformCount();
	material->ShadingModel = materialFile.GetShadingModel();

	for (int32_t i = 0; i < material->TextureCount; i++)
	{
		material->TextureWrapTypes.at(i) = materialFile.GetTextureWrap(i);
	}

	return material;
}

::Effekseer::MaterialRef MaterialLoader::Load(const void* data, int32_t size, Effekseer::MaterialFileType fileType)
{
	if (fileType == Effekseer::MaterialFileType::Compiled)
	{
		auto compiled = Effekseer::CompiledMaterial();
		if (!compiled.Load(static_cast<const uint8_t*>(data), size))
		{
			return nullptr;
		}

		if (!compiled.GetHasValue(platformType_))
		{
			return nullptr;
		}

		// compiled
		Effekseer::MaterialFile materialFile;
		if (!materialFile.Load((const uint8_t*)compiled.GetOriginalData().data(), static_cast<int32_t>(compiled.GetOriginalData().size())))
		{
			std::cout << "Error : Invalid material is loaded." << std::endl;
			return nullptr;
		}

		auto binary = compiled.GetBinary(platformType_);

		return LoadAcutually(materialFile, binary);
	}
	else
	{
		if (materialCompiler_ == nullptr)
		{
			return nullptr;
		}

		Effekseer::MaterialFile materialFile;
		if (!materialFile.Load((const uint8_t*)data, size))
		{
			std::cout << "Error : Invalid material is loaded." << std::endl;
			return nullptr;
		}
        
		auto binary = ::Effekseer::CreateUniqueReference(materialCompiler_->Compile(&materialFile));

		return LoadAcutually(materialFile, binary.get());
	}
}

void MaterialLoader::Unload(::Effekseer::MaterialRef data)
{
	if (data == nullptr)
		return;
	auto shader = reinterpret_cast<Shader*>(data->UserPtr);
	auto modelShader = reinterpret_cast<Shader*>(data->ModelUserPtr);
	auto refractionShader = reinterpret_cast<Shader*>(data->RefractionUserPtr);
	auto refractionModelShader = reinterpret_cast<Shader*>(data->RefractionModelUserPtr);

	ES_SAFE_DELETE(shader);
	ES_SAFE_DELETE(modelShader);
	ES_SAFE_DELETE(refractionShader);
	ES_SAFE_DELETE(refractionModelShader);

	data->UserPtr = nullptr;
	data->ModelUserPtr = nullptr;
	data->RefractionUserPtr = nullptr;
	data->RefractionModelUserPtr = nullptr;
}

} // namespace EffekseerUrho3D
