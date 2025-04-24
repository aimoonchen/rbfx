#include "../Precompiled.h"

#include <nanobind/nanobind.h>
#include "PythonScript.h"
#include "Urho3D/Core/CoreEvents.h"
#include "Urho3D/Core/ProcessUtils.h"
#include "Urho3D/Core/Profiler.h"
#include "Urho3D/Engine/Engine.h"
#include "Urho3D/Engine/EngineEvents.h"
#include "Urho3D/Engine/EngineDefs.h"
#include "Urho3D/IO/File.h"
#include "Urho3D/IO/FileSystem.h"
#include "Urho3D/IO/Log.h"
#include "Urho3D/Resource/ResourceCache.h"
#include "Urho3D/Scene/Scene.h"
#include "PythonInterpreter.h"
#include "../DebugNew.h"

extern "C" PyObject* PyInit_engine();
extern "C" PyObject* PyInit_core();
extern "C" PyObject* PyInit_graphics();
extern "C" PyObject* PyInit_math3d();

Urho3D::PythonScript* g_python_script = nullptr;
namespace nb = nanobind;
namespace Urho3D
{
StringVariantMap& GetEngineParameters();

PythonScript::PythonScript(Context* context) :
    Object(context)
{
    interpreter_ = std::make_unique<PythonInterpreter>();
    eventInvoker_ = std::make_unique<PythonScriptEventInvoker>(context);
    g_python_script = this;
    interpreter_->RegisterModule("engine", PyInit_engine());
    interpreter_->RegisterModule("core", PyInit_core());
    interpreter_->RegisterModule("graphics", PyInit_graphics());
    interpreter_->RegisterModule("math3d", PyInit_math3d());
    // auto ret = RunSimpleString("import my_ext; print(my_ext)");
}

PythonScript::~PythonScript()
{
}

int PythonScript::RunSimpleString(const ea::string& code)
{
    return interpreter_->RunSimpleString(code);
}

PyObject* PythonScript::RunString(const ea::string& code)
{
    return interpreter_->RunString(code);
}

PyObject* PythonScript::CallFunction(const ea::string& moduleName, const ea::string& funcName, PyObject* args)
{
    return interpreter_->CallFunction(moduleName, funcName, args);
}

void PythonScript::AddEventHandler(const ea::string& eventName, nanobind::callable function)
{
    eventInvoker_->AddEventHandler(nullptr, eventName, function);
}

void PythonScript::AddEventHandler(const ea::string& eventName, int index)
{
    assert(false);
}

void PythonScript::AddEventHandler(const ea::string& eventName, const ea::string& functionName)
{
    auto function = GetFunction(functionName);
    if (function)
        eventInvoker_->AddEventHandler(nullptr, eventName, function);
}

void PythonScript::AddEventHandler(Object* sender, const ea::string& eventName, nanobind::callable function)
{
    if (!sender) {
        return;
    }
    eventInvoker_->AddEventHandler(sender, eventName, function);
}

void PythonScript::AddEventHandler(Object* sender, const ea::string& eventName, int index)
{
    assert(false);
}

void PythonScript::AddEventHandler(Object* sender, const ea::string& eventName, const ea::string& functionName)
{
    if (!sender)
        return;

    auto function = GetFunction(functionName);
    if (function)
        eventInvoker_->AddEventHandler(sender, eventName, function);
}

void PythonScript::RemoveEventHandler(const ea::string& eventName)
{
    eventInvoker_->UnsubscribeFromEvent(eventName);
}

void PythonScript::RemoveEventHandler(Object* sender, const ea::string& eventName)
{
    if (!sender)
        return;

    eventInvoker_->UnsubscribeFromEvent(sender, eventName);
}

void PythonScript::RemoveEventHandlers(Object* sender)
{
    if (!sender)
        return;

    eventInvoker_->UnsubscribeFromEvents(sender);
}

void PythonScript::RemoveAllEventHandlers()
{
    eventInvoker_->UnsubscribeFromAllEvents();
}

void PythonScript::RemoveEventHandlersExcept(const ea::vector<ea::string>& exceptionNames)
{
    ea::vector<StringHash> exceptionTypes(exceptionNames.size());
    for (unsigned i = 0; i < exceptionTypes.size(); ++i)
        exceptionTypes[i] = StringHash(exceptionNames[i]);

    eventInvoker_->UnsubscribeFromAllEventsExcept(exceptionTypes);
}

bool PythonScript::HasEventHandler(const ea::string& eventName) const
{
    return eventInvoker_->HasSubscribedToEvent(eventName);
}

bool PythonScript::HasEventHandler(Object* sender, const ea::string& eventName) const
{
    return eventInvoker_->HasSubscribedToEvent(sender, eventName);
}

bool PythonScript::ExecuteFile(const ea::string& filename)
{
    return interpreter_->ExecuteFile(filename);
}

bool PythonScript::ExecuteString(const ea::string& string)
{
    return false;
}

bool PythonScript::LoadRawFile(const ea::string& fileName)
{
    return false;
}

bool PythonScript::ExecuteRawFile(const ea::string& fileName)
{
    return false;
}
bool PythonScript::ExecuteFunction(const ea::string& functionName)
{
    return false;
}

nb::callable PythonScript::GetFunction(const ea::string& functionName, bool silentIfNotFound)
{
    auto i = functionNameToFunctionMap_.find(functionName);
    if (i != functionNameToFunctionMap_.end())
        return i->second;
    const auto& path = functionName.split('.');
//     nb::object lobj = (*luaState_)[path[0].c_str()];
//     if (path.size() > 1)
//     {
//         for (size_t idx = 1; idx < path.size() - 1; ++idx)
//         {
//             lobj = lobj.as<sol::table>()[path[idx].c_str()];
//         }
//         lobj = lobj.as<sol::table>()[path[path.size() - 1].c_str()];
//     }
    nb::callable func;
//     if (lobj.get_type() == sol::type::function) {
//         func = std::make_shared<sol::function>(lobj.as<sol::function>());
//     }

    if (func) {
        functionNameToFunctionMap_[functionName] = func;
    } else if (!silentIfNotFound) {
        URHO3D_LOGERRORF("Can not find lua function : %s", functionName.c_str());
    }
    return func;
}

void RegisterPythonScriptLibrary(Context* context)
{
    static bool test = false;
    if (!test) {
//         LuaFile::RegisterObject(context);
//         LuaScriptInstance::RegisterObject(context);
        test = true;
    }
}

bool RunPython(Context* context, const ea::string& scriptFileName)
{
    // Instantiate and register the Python script subsystem
    PythonScript* pythonScript = context->GetSubsystem<PythonScript>();
    if (!pythonScript) {
        pythonScript = new PythonScript(context);
        context->RegisterSubsystem(pythonScript);
    }
    // If script loading is successful, proceed to main loop
    if (pythonScript->ExecuteFile(scriptFileName)) {
        return pythonScript->ExecuteFunction("Start");
    }
    return false;
}

}
