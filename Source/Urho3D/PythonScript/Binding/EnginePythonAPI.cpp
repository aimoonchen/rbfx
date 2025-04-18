#include <nanobind/nanobind.h>
//#include <nanobind/stl/string.h>
#include "../../Core/Context.h"
#include "../../Engine/Engine.h"
#include "../../Resource/XMLFile.h"
#include "../../UI/BorderImage.h"
#include "../PythonScript.h"

using namespace Urho3D;
namespace nb = nanobind;
using namespace nb::literals;

// static void RegisterEngineConst(sol::state& lua)
// {
//     auto eventType = lua["EventType"].get_or_create<sol::table>();
//     auto paramType = lua["ParamType"].get_or_create<sol::table>();
// }

extern Urho3D::PythonScript* g_python_script;

NB_MODULE(engine, m)
{
    nb::class_<Engine>(m, "Engine")
        .def("Exit", &Engine::Exit);
//    m.attr("engine") = context->GetSubsystem<Engine>();
//    RegisterEngineConst(lua);
    m.attr("SubscribeToEvent") = [](const ea::string& eventName, const ea::string& functionName) {
        g_python_script->AddEventHandler(eventName, functionName);
    };
    m.attr("SubscribeToEvent") = [](const ea::string& eventName, nb::callable function) {
        LuaScriptAddEventHandler(eventName, &function);
    };
    m.attr("SubscribeToEvent") = [](Object* sender, const ea::string& eventName, const ea::string& functionName) {
        LuaScriptAddEventHandler(sender, eventName, functionName);
    };
    m.attr("SubscribeToEvent") = [](Object* sender, const ea::string& eventName, nb::callable function) {
        LuaScriptAddEventHandler(sender, eventName, &function);
    };
    m.attr("UnSubscribeToEvent") = [](const ea::string& eventName) { LuaScriptRemoveEventHandler(eventName); };
    m.attr("UnSubscribeToEvent") = [](Object* sender, const ea::string& eventName) { LuaScriptRemoveEventHandler(sender, eventName); };
    m.attr("UnSubscribeToAllEvent") = []() { LuaScriptRemoveAllEventHandlers(); };
}
