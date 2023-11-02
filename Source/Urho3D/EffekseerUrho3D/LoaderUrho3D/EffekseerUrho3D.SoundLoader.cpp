//#include <AudioStream.hpp>
#include "EffekseerUrho3D.SoundLoader.h"
#include "../Utils/EffekseerUrho3D.Utils.h"
#include "../SoundUrho3D/EffekseerUrho3D.SoundResources.h"

namespace EffekseerUrho3D
{

SoundLoader::SoundLoader(/*godot::Ref<godot::Reference> soundContext*/)
//	: soundContext_(soundContext)
{
}

Effekseer::SoundDataRef SoundLoader::Load(const char16_t* path)
{
	// Load by Godot
// 	godot::Ref<godot::AudioStream> resource = soundContext_->call("load_sound", ToGdString(path));
// 	if (!resource.is_valid())
// 	{
// 		return nullptr;
// 	}
// 
// 	auto result = Effekseer::MakeRefPtr<SoundData>(resource);
// 
// 	return result;
	return nullptr;
}

void SoundLoader::Unload(Effekseer::SoundDataRef textureData)
{
}

} // namespace EffekseerUrho3D
