#if defined(__linux__) && !defined(__ANDROID__)
#else
#include <nanobind/nanobind.h>
#include "fmod_studio.hpp"
#include "fmod_errors.h"
#include "../../Core/Context.h"
#include "../../Resource/ResourceCache.h"
#include "../../Audio/Audio.h"
#include "../../IO/Log.h"

using namespace Urho3D;
namespace nb = nanobind;
using namespace nb::literals;

static void ERRCHECK_fn(FMOD_RESULT result, const char* file, int line)
{
    if (result != FMOD_OK)
    {
        URHO3D_LOGERRORF("%s(%d): FMOD error %d - %s", file, line, result, FMOD_ErrorString(result));
    }
}
#define ERRCHECK(_result) ERRCHECK_fn(_result, __FILE__, __LINE__)

static FMOD::Studio::EventInstance* CreateEventInstance(Urho3D::Context* context, std::string_view eventName)
{
    static std::unordered_map<std::string_view, FMOD::Studio::EventDescription*> event_desc_cache;
    FMOD::Studio::EventDescription* eventDescription = nullptr;
    auto it = event_desc_cache.find(eventName);
    if (it == event_desc_cache.end()) {
        auto system = context->GetSubsystem<Audio>()->GetSystem();
        ERRCHECK(system->getEvent(eventName.data(), &eventDescription));
        if (eventDescription) {
            event_desc_cache.insert({eventName, eventDescription});
        }
    } else {
        eventDescription = it->second;
    }
    FMOD::Studio::EventInstance* sound = nullptr;
    if (eventDescription) {
        ERRCHECK(eventDescription->createInstance(&sound));
    }
    return sound;
}
NB_MODULE(audio, m)
{
    nb::class_<FMOD::Studio::EventInstance>(m, "EventInstance")
        .def("Start", &FMOD::Studio::EventInstance::start)
        .def("Release", &FMOD::Studio::EventInstance::release)
        .def("Stop", [](FMOD::Studio::EventInstance* self, bool fadeout) { self->stop(fadeout ? FMOD_STUDIO_STOP_ALLOWFADEOUT : FMOD_STUDIO_STOP_IMMEDIATE); })
        .def("GetVolume", [](FMOD::Studio::EventInstance* self) { float volume = 0.0f; self->getVolume(&volume); return volume; })
        .def("SetVolume", &FMOD::Studio::EventInstance::setVolume)
        .def("GetPitch", [](FMOD::Studio::EventInstance* self) { float pitch = 0.0f; self->getPitch(&pitch); return pitch; })
        .def("SetPitch", &FMOD::Studio::EventInstance::setPitch);
    auto context = Context::GetInstance();
    m.def("LoadBank", [context](std::string_view filename) {
        return context->GetSubsystem<Audio>()->LoadBank(filename) != nullptr;
//         auto bank = context->GetSubsystem<Audio>()->LoadBank(filename);
//         std::vector<FMOD::Studio::EventDescription*> event_desc;
//         std::vector<std::string> event_list;
//         int count = 0;
//         bank->getEventCount(&count);
//         if (count > 0) {
//             event_list.reserve(count);
//             event_desc.resize(count);
//             bank->getEventList(&event_desc[0], count, &count);
//         }
//         char temp[256];
//         for (auto desc : event_desc) {
//             desc->getPath(temp, 256, nullptr);
//             event_list.push_back(temp);
//         }
//         //bank->getPath(temp, 256, nullptr);
//         return sol::as_table(event_list);
    });
    m.def("Play", [context](std::string_view eventName) {
        auto inst = CreateEventInstance(context, eventName);
        inst->start();
        inst->release();
    });
    m.def("Play", [context](std::string_view eventName, float volume) {
        auto inst = CreateEventInstance(context, eventName);
        inst->setVolume(volume);
        inst->start();
        inst->release();
    });
    m.def("UnloadBank", [context](std::string_view filename) { context->GetSubsystem<Audio>()->UnloadBank(filename); });
    m.def("CreateEvent", [context](std::string_view eventName) { return CreateEventInstance(context, eventName); });
}
#endif
