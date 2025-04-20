#if defined(__linux__) && !defined(__ANDROID__)
#else
#include <nanobind/nanobind.h>
#include "../../Core/Context.h"
#include "../../Resource/ResourceCache.h"
#include "../../EffekseerUrho3D/EffekseerEffect.h"
#include "../../EffekseerUrho3D/EffekseerEmitter.h"
#include "../../EffekseerUrho3D/EffekseerSystem.h"

using namespace Urho3D;
namespace nb = nanobind;
using namespace nb::literals;

NB_MODULE(effekseer, m)
{
	//auto effekseer = lua["Effekseer"].get_or_create<sol::table>();
    nb::class_<EffekseerEmitter, Drawable>(m, "EffekseerEmitter")
        //.def("id"]          = sol::var(StringHash("EffekseerEmitter"));
        .def("SetEffect", [](EffekseerEmitter* emitter, std::string_view filename) {
            auto cache = emitter->GetSubsystem<ResourceCache>();
            emitter->SetEffect(cache->GetResource<EffekseerEffect>(filename.data())); })
        .def("Play", &EffekseerEmitter::play)
//         .def("Play", [](EffekseerEmitter& emitter) { emitter.play(); })
//         .def("Play", [](EffekseerEmitter& emitter, int32_t startFrame) { emitter.play(startFrame); })
//         .def("Play", [](EffekseerEmitter& emitter, int32_t startFrame, bool mgr) { emitter.play(startFrame, mgr); })
        .def("SetPause", &EffekseerEmitter::set_paused)
        .def("Stop", &EffekseerEmitter::stop)
        .def("SetSpeed", &EffekseerEmitter::set_speed)
        .def("SetLooping", &EffekseerEmitter::set_looping)
        .def("IsPlaying", &EffekseerEmitter::is_playing)
        .def("SetVisible", &EffekseerEmitter::set_visible)
        .def("SetCullBoundingBox", &EffekseerEmitter::SetCullBoundingBox);

	//m.def("SetCamera", [](Camera* camera) { EffekseerSystem::get_instance()->SetCamera(camera); });
}
#endif
