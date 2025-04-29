#if defined(__linux__) && !defined(__ANDROID__)
#else
#include <nanobind/nanobind.h>
#include <nanobind/stl/string_view.h>
#include "../../Core/Context.h"
#include "../../Resource/ResourceCache.h"
#include "../../EffekseerUrho3D/EffekseerEffect.h"
#include "../../EffekseerUrho3D/EffekseerEmitter.h"
#include "../../EffekseerUrho3D/EffekseerSystem.h"
#include "../../Graphics/Camera.h"

using namespace Urho3D;
namespace nb = nanobind;
using namespace nb::literals;

void init_cmodule_effekseer(nb::module_& pm)
{
    auto m = pm.def_submodule("effekseer");
    nb::class_<EffekseerEmitter, Drawable>(m, "EffekseerEmitter")
        .def_ro_static("TypeId", &EffekseerEmitter::TypeId)
        .def("SetEffect", [](EffekseerEmitter* emitter, std::string_view filename) {
            auto cache = emitter->GetSubsystem<ResourceCache>();
            emitter->SetEffect(cache->GetResource<EffekseerEffect>(filename.data())); })
        .def("Play", &EffekseerEmitter::play, nb::arg("startFrame") = 0, nb::arg("disown") = true)
        .def("Play", [](EffekseerEmitter& emitter) { emitter.play(); })
        .def("SetPause", &EffekseerEmitter::set_paused)
        .def("Stop", &EffekseerEmitter::stop)
        .def("SetSpeed", &EffekseerEmitter::set_speed)
        .def("SetLooping", &EffekseerEmitter::set_looping)
        .def("IsPlaying", &EffekseerEmitter::is_playing)
        .def("SetVisible", &EffekseerEmitter::set_visible)
        .def("SetCullBoundingBox", &EffekseerEmitter::SetCullBoundingBox);

	m.def("SetCamera", [](Camera* camera) { EffekseerSystem::get_instance()->SetCamera(camera); });
}
#endif
