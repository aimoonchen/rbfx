#include "../../Core/Context.h"
#include "../../Graphics/Texture2D.h"
#include "../../Resource/ResourceCache.h"
#include "../../IO/VirtualFileSystem.h"
#include "EffekseerUrho3D.TextureLoader.h"
#include "../RendererUrho3D/EffekseerUrho3D.RenderResources.h"
#include "../Utils/EffekseerUrho3D.Utils.h"

namespace EffekseerUrho3D
{

TextureLoader::TextureLoader(Urho3D::Context* context,
    ::Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
    ::Effekseer::ColorSpaceType colorSpaceType)
    : context_{ context }
    , graphicsDevice_{ graphicsDevice }
    , colorSpaceType_{ colorSpaceType }
{
}

Effekseer::TextureRef TextureLoader::Load(const char16_t* path, Effekseer::TextureType textureType)
{
	static auto* cache = context_->GetSubsystem<Urho3D::ResourceCache>();
	auto urho3dPath = ToGdString(path);

    // TODO: convert to rgba image
    auto texture = new Urho3D::Texture2D(context_);
    auto image = ea::make_unique<Urho3D::Image>(context_);
    image->SetForceRGBA(true);
    auto vfs = context_->GetSubsystem<Urho3D::VirtualFileSystem>();
    auto file = vfs->OpenFile(urho3dPath, Urho3D::FILE_READ);
    if (file) {
        if (image->Load(*file)) {
            texture->SetData(image.get());
        }
    }

	//auto texture = cache->GetResource<Urho3D::Texture2D>(urho3dPath);

	auto backend = ::Effekseer::MakeRefPtr<Texture>();
	backend->param_.Size[0] = (int32_t)texture->GetWidth();
	backend->param_.Size[1] = (int32_t)texture->GetHeight();
	backend->texture_.reset(texture);
	//backend->godotTexture_ = resource;
	//backend->textureRid_ = resource->get_rid();

	//auto format = texture->get_format();
	//switch (format)
	//{
	//case godot::Image::FORMAT_RGBA8: backend->format_ = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM; break;
	//case godot::Image::FORMAT_DXT1:  backend->format_ = Effekseer::Backend::TextureFormatType::BC1; break;
	//case godot::Image::FORMAT_DXT3:  backend->format_ = Effekseer::Backend::TextureFormatType::BC2; break;
	//case godot::Image::FORMAT_DXT5:  backend->format_ = Effekseer::Backend::TextureFormatType::BC3; break;
	//default: printf("Unknown format.\n");
	//}

	//printf("TextureLoader::Load path=%s (%dx%d)\n", path8, backend->GetSize()[0], backend->GetSize()[1]);

	auto result = Effekseer::MakeRefPtr<Effekseer::Texture>();
	result->SetBackend(backend);
	return result;
}

void TextureLoader::Unload(Effekseer::TextureRef textureData)
{
}

::Effekseer::TextureLoaderRef CreateTextureLoader(
    Urho3D::Context* context,
    ::Effekseer::Backend::GraphicsDeviceRef gprahicsDevice,
    ::Effekseer::ColorSpaceType colorSpaceType)
{
    return ::Effekseer::MakeRefPtr<TextureLoader>(context, gprahicsDevice, colorSpaceType);
}
} // namespace EffekseerUrho3D
