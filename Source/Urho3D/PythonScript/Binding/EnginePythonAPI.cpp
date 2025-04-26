#include <nanobind/nanobind.h>
#include "../../Core/Context.h"
#include "../../Engine/Engine.h"
#include "../../UI/UI.h"
#include "../../Graphics/Graphics.h"
#include "../../Graphics/Renderer.h"
#include "../../Actions/ActionManager.h"
#include "../../IO/VirtualFileSystem.h"
#include "../../Network/Network.h"
#include "../../Resource/ResourceCache.h"
#include "../PythonScript.h"

using namespace Urho3D;
namespace nb = nanobind;
using namespace nb::literals;
void init_cmodule_action(nb::module_& pm);
void init_cmodule_audio(nb::module_& pm);
void init_cmodule_core(nb::module_& pm);
void init_cmodule_math3d(nb::module_& pm);
void init_cmodule_input(nb::module_& pm);
void init_cmodule_io(nb::module_& pm);
void init_cmodule_audio(nb::module_& pm);
void init_cmodule_action(nb::module_& pm);
void init_cmodule_resource(nb::module_& pm);
void init_cmodule_scene(nb::module_& pm);
void init_cmodule_graphics(nb::module_& pm);
void init_cmodule_network(nb::module_& pm);
void init_cmodule_physics(nb::module_& pm);
void init_cmodule_physics2d(nb::module_& pm);
void init_cmodule_ui(nb::module_& pm);
void init_cmodule_fairygui(nb::module_& pm);
void init_cmodule_effekseer(nb::module_& pm);
void init_cmodule_urho2d(nb::module_& pm);

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
#undef NB_EXPORT
#define NB_EXPORT
NB_MODULE(kfengine, m)
{
    nb::class_<Engine>(m, "Engine")
        .def("Exit", &Engine::Exit);
//    m.attr("engine") = context->GetSubsystem<Engine>();
//    RegisterEngineConst(lua);
    m.def("SubscribeToEvent", [](const char* eventName, const char* functionName) {
        g_python_script->AddEventHandler(eventName, functionName);
    });
    m.def("SubscribeToEvent", [](const char* eventName, nb::callable function) {
        PythonScriptAddEventHandler(eventName, function);
    });
    m.def("SubscribeToEvent", [](Object* sender, const char* eventName, const char* functionName) {
        PythonScriptAddEventHandler(sender, eventName, functionName);
    });
    m.def("SubscribeToEvent", [](Object* sender, const char* eventName, nb::callable function) {
        PythonScriptAddEventHandler(sender, eventName, function);
    });
    m.def("UnSubscribeToEvent", [](const char* eventName) { PythonScriptRemoveEventHandler(eventName); });
    m.def("UnSubscribeToEvent", [](Object* sender, const char* eventName) { PythonScriptRemoveEventHandler(sender, eventName); });
    m.def("UnSubscribeToAllEvent", []() { PythonScriptRemoveAllEventHandlers(); });

    init_cmodule_core(m);
    init_cmodule_math3d(m);
    init_cmodule_input(m);
    init_cmodule_io(m);
    init_cmodule_audio(m);
    init_cmodule_action(m);
    init_cmodule_resource(m);
    init_cmodule_scene(m);
    init_cmodule_graphics(m);
    init_cmodule_network(m);
    init_cmodule_physics(m);
    init_cmodule_physics2d(m);
    init_cmodule_ui(m);
    init_cmodule_fairygui(m);
    init_cmodule_effekseer(m);
    init_cmodule_urho2d(m);

    auto context = Context::GetInstance();
    m.attr("ui")                    = context->GetSubsystem<UI>();
    m.attr("action_manager")        = context->GetSubsystem<ActionManager>();
    m.attr("time")                  = context->GetSubsystem<Time>();
    m.attr("graphics_system")       = context->GetSubsystem<Graphics>();
    m.attr("renderer_system")       = context->GetSubsystem<Renderer>();
    m.attr("filesystem")            = context->GetSubsystem<FileSystem>();
    m.attr("virtual_filesystem")    = context->GetSubsystem<VirtualFileSystem>();
    m.attr("network")               = context->GetSubsystem<Network>();
    m.attr("cache")                 = context->GetSubsystem<ResourceCache>();
}
