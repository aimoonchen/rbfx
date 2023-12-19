#include "../../Core/Context.h"
#include "../../Graphics/Texture2D.h"
#include "../../Resource/ResourceCache.h"
#include "../../IO/VirtualFileSystem.h"
#include "EffekseerUrho3D.TextureLoader.h"
#include "../RendererUrho3D/EffekseerUrho3D.RenderResources.h"
#include "../Utils/EffekseerUrho3D.Utils.h"
#include <STB/stb_image.h>

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
    Urho3D::Texture2D* texture{ nullptr };
    // TODO: convert to rgba image
    auto vfs = context_->GetSubsystem<Urho3D::VirtualFileSystem>();
    auto file = vfs->OpenFile(urho3dPath, Urho3D::FILE_READ);
    if (file) {
        auto dataSize = file->GetSize();
        ea::shared_array<unsigned char> buffer(new unsigned char[dataSize]);
        file->Read(buffer.get(), dataSize);
        int width{ 0 };
        int height{ 0 };
        unsigned int components{ 0 };
        auto pixels = (unsigned char*)stbi_load_from_memory(buffer.get(), dataSize, &width, &height, (int*)&components, 0);
        if (components < 3) {
            auto newPixels = ea::make_unique<uint8_t[]>(width * height * 4);
            auto buf = newPixels.get();
            if (components == 2) {
                // Gray+Alpha
                for (int h = 0; h < height; h++) {
                    for (int w = 0; w < width; w++) {
                        ((uint8_t*)buf)[(w + h * width) * 4 + 0] = pixels[(w + h * width) * 2 + 0];
                        ((uint8_t*)buf)[(w + h * width) * 4 + 1] = pixels[(w + h * width) * 2 + 0];
                        ((uint8_t*)buf)[(w + h * width) * 4 + 2] = pixels[(w + h * width) * 2 + 0];
                        ((uint8_t*)buf)[(w + h * width) * 4 + 3] = pixels[(w + h * width) * 2 + 1];
                    }
                }
            } else if (components == 1) {
                // Gray
                for (int h = 0; h < height; h++) {
                    for (int w = 0; w < width; w++) {
                        ((uint8_t*)buf)[(w + h * width) * 4 + 0] = pixels[(w + h * width) * 1 + 0];
                        ((uint8_t*)buf)[(w + h * width) * 4 + 1] = pixels[(w + h * width) * 1 + 0];
                        ((uint8_t*)buf)[(w + h * width) * 4 + 2] = pixels[(w + h * width) * 1 + 0];
                        ((uint8_t*)buf)[(w + h * width) * 4 + 3] = 255;
                    }
                }
            }
            auto image = ea::make_unique<Urho3D::Image>(context_);
            image->SetSize(width, height, 4);
            image->SetData(newPixels.get());
            texture = new Urho3D::Texture2D(context_);
            texture->SetNumLevels(1);
            texture->SetData(image.get());
        } else {
            texture = cache->GetResource<Urho3D::Texture2D>(urho3dPath);
            // TODO: no mipmap, not work
            texture->SetNumLevels(1);
        }
        stbi_image_free(pixels);
    }

    if (!texture) {
        return nullptr;
    }

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
