#include "EffekseerUrho3D.ModelLoader.h"
#include "../RendererUrho3D/EffekseerUrho3D.RenderResources.h"
#include "../Utils/EffekseerUrho3D.Utils.h"
#include "../../Core/Context.h"
#include "../../Resource/ResourceCache.h"

namespace EffekseerUrho3D
{

ModelLoader::ModelLoader(Urho3D::Context* context)
    : context_{ context }
{
}

Effekseer::ModelRef ModelLoader::Load(const char16_t* path)
{
	static auto cache = context_->GetSubsystem<Urho3D::ResourceCache>();
	auto urho3dPath = ToGdString(path);
	auto urhoFile = cache->GetFile(urho3dPath);
	auto dataSize = urhoFile->GetSize();
	auto data = std::make_unique<char[]>(dataSize);
    if (urhoFile->Read(data.get(), dataSize) != dataSize) {
        return nullptr;
	}
 	return Load(data.get(), dataSize);
}

Effekseer::ModelRef ModelLoader::Load(const void* data, int32_t size)
{
    return Effekseer::MakeRefPtr<Model>(data, size);
}

void ModelLoader::Unload(Effekseer::ModelRef data)
{
}

} // namespace EffekseerUrho3D

