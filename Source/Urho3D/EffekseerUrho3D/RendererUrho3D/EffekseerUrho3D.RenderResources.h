#pragma once

#include <stdint.h>
#include <Effekseer.h>

namespace Urho3D
{
class Context;
class Texture2D;
} // namespace Urho3D

namespace EffekseerUrho3D
{
class Texture : public Effekseer::Backend::Texture
{
    ea::shared_ptr<Urho3D::Texture2D> texture_;
    std::function<void()> onDisposed_;
    friend class TextureLoader;

public:
    Texture() = default;
    ~Texture() override;

    bool Init(Urho3D::Context* context, const Effekseer::Backend::TextureParameter& param, const Effekseer::CustomVector<uint8_t>& initialData);

    bool Init(uint64_t id, std::function<void()> onDisposed);

    bool Init(Urho3D::Texture2D* texture);

    ea::shared_ptr<Urho3D::Texture2D>& GetTexture() { return texture_; }
};

class Model : public Effekseer::Model
{
public:
	Model(const Effekseer::CustomVector<Vertex>& vertecies, const Effekseer::CustomVector<Face>& faces);
	Model(const void* data, int32_t size);
	~Model();

private:
	void UploadToEngine();
    friend class ModelLoader;
};

}
