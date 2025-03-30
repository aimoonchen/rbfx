#if defined(__linux__) && !defined(__ANDROID__)
#else
#include "GetPush.h"
#include "fmod_studio.hpp"
#include "fmod_errors.h"
#include "../../Core/Context.h"
#include "../../Resource/ResourceCache.h"
#include "../../Audio/Audio.h"
#include "../../IO/Log.h"

using namespace Urho3D;
Urho3D::Context* GetContext(lua_State* L);

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

int sol2_AudioLuaAPI_open(sol::state& solLua)
{
    auto context = GetContext(solLua.lua_state());
    auto audio = solLua["Audio"].get_or_create<sol::table>();
    auto bindEventInstance = audio.new_usertype<FMOD::Studio::EventInstance>("EventInstance");
    bindEventInstance["Start"]      = &FMOD::Studio::EventInstance::start;
    bindEventInstance["Release"]    = &FMOD::Studio::EventInstance::release;
    bindEventInstance["Stop"]       = [](FMOD::Studio::EventInstance* self, bool fadeout) { self->stop(fadeout ? FMOD_STUDIO_STOP_ALLOWFADEOUT : FMOD_STUDIO_STOP_IMMEDIATE); };
    bindEventInstance["GetVolume"]  = [](FMOD::Studio::EventInstance* self) { float volume = 0.0f; self->getVolume(&volume); return volume; };
    bindEventInstance["SetVolume"]  = &FMOD::Studio::EventInstance::setVolume;
    bindEventInstance["GetPitch"]   = [](FMOD::Studio::EventInstance* self) { float pitch = 0.0f; self->getPitch(&pitch); return pitch; };
    bindEventInstance["SetPitch"]   = &FMOD::Studio::EventInstance::setPitch;

    audio["LoadBank"] = [context](std::string_view filename) {
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
    };
    audio["Play"] = sol::overload(
        [context](std::string_view eventName) {
            auto inst = CreateEventInstance(context, eventName);
            inst->start();
            inst->release(); },
        [context](std::string_view eventName, float volume) {
            auto inst = CreateEventInstance(context, eventName);
            inst->setVolume(volume);
            inst->start();
            inst->release(); });
    audio["UnloadBank"] = [context](std::string_view filename) { context->GetSubsystem<Audio>()->UnloadBank(filename); };
    audio["CreateEvent"] = [context](std::string_view eventName) { return CreateEventInstance(context, eventName); };
    return 0;
}
#endif
