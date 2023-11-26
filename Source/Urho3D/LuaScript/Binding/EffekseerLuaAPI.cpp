#if defined(__linux__) && !defined(__ANDROID__)
#else
#include "GetPush.h"
#include "../../Core/Context.h"
#include "../../Resource/ResourceCache.h"
#include "../../EffekseerUrho3D/EffekseerEffect.h"
#include "../../EffekseerUrho3D/EffekseerEmitter.h"
#include "../../EffekseerUrho3D/EffekseerSystem.h"

using namespace Urho3D;

namespace sol {
}

Urho3D::Context* GetContext(lua_State* L);

int sol2_EffekseerLuaAPI_open(sol::state& lua)
{
	auto context = GetContext(lua.lua_state());
	//auto cache = context->GetSubsystem<ResourceCache>();
	auto effekseer = lua["Effekseer"].get_or_create<sol::table>();
	lua.new_usertype<EffekseerEmitter>("EffekseerEmitter",
        "id", sol::var(StringHash("EffekseerEmitter")),
		"SetEffect", [](EffekseerEmitter* emitter, std::string_view filename) {
			auto cache = emitter->GetSubsystem<ResourceCache>();
			emitter->SetEffect(cache->GetResource<EffekseerEffect>(filename.data())); },
		"Play", sol::overload([](EffekseerEmitter* emitter) { emitter->play(); }, [](EffekseerEmitter* emitter, int32_t startFrame) { emitter->play(startFrame); }, [](EffekseerEmitter* emitter, int32_t startFrame, bool mgr) { emitter->play(startFrame, mgr); }),
        "SetPause", &EffekseerEmitter::set_paused,
        "Stop", &EffekseerEmitter::stop,
		"SetSpeed", &EffekseerEmitter::set_speed,
		"SetLooping", &EffekseerEmitter::set_looping,
		"IsPlaying", &EffekseerEmitter::is_playing
		);
	effekseer["SetCamera"] = [](Camera* camera) { EffekseerSystem::get_instance()->SetCamera(camera); };
	return 0;
}
#endif
