//#include <ResourceLoader.hpp>
#include "EffekseerUrho3D.CurveLoader.h"
#include "../Utils/EffekseerUrho3D.Utils.h"
#include "../EffekseerResource.h"

namespace EffekseerUrho3D
{

Effekseer::CurveRef CurveLoader::Load(const char16_t* path)
{
	// Load by Godot
// 	auto loader = godot::ResourceLoader::get_singleton();
// 	auto resource = loader->load(ToGdString(path), "");
// 	if (!resource.is_valid())
// 	{
// 		return nullptr;
// 	}
// 
// 	auto efkres = godot::as<godot::EffekseerResource>(resource.ptr());
// 	auto& data = efkres->get_data_ref();
// 
// 	return Load(data.read().ptr(), data.size());
	return nullptr;
}

Effekseer::CurveRef CurveLoader::Load(const void* data, int32_t size)
{
	return Effekseer::MakeRefPtr<Effekseer::Curve>(data, size);
}

void CurveLoader::Unload(Effekseer::CurveRef data)
{
}

} // namespace EffekseerUrho3D

