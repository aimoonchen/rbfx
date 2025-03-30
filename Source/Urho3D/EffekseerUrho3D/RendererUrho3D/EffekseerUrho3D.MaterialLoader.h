
#ifndef __EFFEKSEERRENDERER_LLGI_MATERIALLOADER_H__
#define __EFFEKSEERRENDERER_LLGI_MATERIALLOADER_H__

//#include "../3rdParty/LLGI/src/LLGI.Compiler.h"
#include "../Effekseer/Effekseer/Material/Effekseer.MaterialCompiler.h"
#include "EffekseerUrho3D.RendererImplemented.h"

#include <Effekseer.h>
#include <memory>

namespace LLGI
{
    struct CompilerResult;
} // namespace LLGI

namespace Effekseer
{
class Material;
class CompiledMaterialBinary;
} // namespace Effekseer

namespace EffekseerUrho3D
{

class MaterialLoader : public ::Effekseer::MaterialLoader
{
private:
	Backend::GraphicsDeviceRef graphicsDevice_ = nullptr;
	::Effekseer::FileInterfaceRef fileInterface_ = nullptr;
	::Effekseer::CompiledMaterialPlatformType platformType_;
	::Effekseer::MaterialCompiler* materialCompiler_ = nullptr;
	::Effekseer::DefaultFileInterface defaultFileInterface_;

	::Effekseer::MaterialRef LoadAcutually(::Effekseer::MaterialFile& materialFile, ::Effekseer::CompiledMaterialBinary* binary);

    ea::string currentPath_;
protected:
	virtual void Deserialize(uint8_t* data, uint32_t datasize, LLGI::CompilerResult& result);

public:
	MaterialLoader(Backend::GraphicsDeviceRef graphicsDevice,
				   ::Effekseer::FileInterfaceRef fileInterface,
				   ::Effekseer::CompiledMaterialPlatformType platformType,
				   ::Effekseer::MaterialCompiler* materialCompiler);
	virtual ~MaterialLoader();

	::Effekseer::MaterialRef Load(const char16_t* path) override;

	::Effekseer::MaterialRef Load(const void* data, int32_t size, Effekseer::MaterialFileType fileType) override;

	void Unload(::Effekseer::MaterialRef data) override;
};

} // namespace EffekseerUrho3D

#endif // __EFFEKSEERRENDERER_LLGI_MATERIALLOADER_H__
