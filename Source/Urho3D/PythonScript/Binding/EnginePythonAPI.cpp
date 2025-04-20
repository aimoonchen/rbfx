#include <nanobind/nanobind.h>
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

#define PythonScriptAddEventHandler g_python_script->AddEventHandler
#define PythonScriptRemoveEventHandler g_python_script->RemoveEventHandler
#define PythonScriptRemoveEventHandlers g_python_script->RemoveEventHandlers
#define PythonScriptRemoveAllEventHandlers g_python_script->RemoveAllEventHandlers
#define PythonScriptRemoveEventHandlersExcept g_python_script->RemoveEventHandlersExcept
#define PythonScriptHasSubscribedToEvent g_python_script->HasEventHandler

#define PythonScriptSendEvent g_python_script->SendEvent
#define PythonScriptSetExecuteConsoleCommands g_python_script->SetExecuteConsoleCommands
#define PythonScriptGetExecuteConsoleCommands g_python_script->GetExecuteConsoleCommands

#define PythonScriptSetGlobalVar g_python_script->SetGlobalVar
#define PythonScriptGetGlobalVar g_python_script->GetGlobalVar
#define PythonScriptGetGlobalVars g_python_script->GetGlobalVars

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
        PythonScriptAddEventHandler(eventName, function);
    };
    m.attr("SubscribeToEvent") = [](Object* sender, const ea::string& eventName, const ea::string& functionName) {
        PythonScriptAddEventHandler(sender, eventName, functionName);
    };
    m.attr("SubscribeToEvent") = [](Object* sender, const ea::string& eventName, nb::callable function) {
        PythonScriptAddEventHandler(sender, eventName, function);
    };
    m.attr("UnSubscribeToEvent") = [](const ea::string& eventName) { PythonScriptRemoveEventHandler(eventName); };
    m.attr("UnSubscribeToEvent") = [](Object* sender, const ea::string& eventName) { PythonScriptRemoveEventHandler(sender, eventName); };
    m.attr("UnSubscribeToAllEvent") = []() { PythonScriptRemoveAllEventHandlers(); };
}
