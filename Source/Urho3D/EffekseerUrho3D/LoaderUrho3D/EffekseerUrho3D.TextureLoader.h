#pragma once

#include <Effekseer.h>

namespace Urho3D
{
    class Context;
}

namespace EffekseerUrho3D
{

class TextureLoader : public Effekseer::TextureLoader
{
public:
	TextureLoader(Urho3D::Context* context,
        ::Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
        ::Effekseer::ColorSpaceType colorSpaceType = ::Effekseer::ColorSpaceType::Gamma);
	virtual ~TextureLoader() = default;
	Effekseer::TextureRef Load(const char16_t* path, Effekseer::TextureType textureType) override;
	void Unload(Effekseer::TextureRef texture) override;

private:
    Urho3D::Context* context_{ nullptr };
    ::Effekseer::Backend::GraphicsDeviceRef graphicsDevice_;
    ::Effekseer::ColorSpaceType colorSpaceType_;
};

} // namespace EffekseerUrho3D
