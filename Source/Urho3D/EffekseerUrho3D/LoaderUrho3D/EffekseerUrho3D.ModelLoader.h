#pragma once

#include <Effekseer.h>

namespace Urho3D
{
    class Context;
}

namespace EffekseerUrho3D
{

class ModelLoader : public ::Effekseer::ModelLoader
{
public:
	ModelLoader(Urho3D::Context* context);

	virtual ~ModelLoader() = default;

	Effekseer::ModelRef Load(const char16_t* path) override;

	Effekseer::ModelRef Load(const void* data, int32_t size) override;

	void Unload(Effekseer::ModelRef data) override;

private:
    Urho3D::Context* context_{ nullptr };
};

} // namespace EffekseerUrho3D

