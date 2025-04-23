#pragma once

#include "Urho3D/Core/Context.h"
#include "Urho3D/Core/Object.h"
#include "PythonScriptEventListener.h"
#include "PythonScriptEventInvoker.h"

namespace Urho3D
{

class Scene;

/// Python script subsystem.
class URHO3D_API PythonScript : public Object, public PythonScriptEventListener
{
    URHO3D_OBJECT(PythonScript, Object);

public:
    /// Construct.
    explicit PythonScript(Context* context);
    /// Destruct.
    ~PythonScript() override;
    bool Initialize();
    void RegisterModule();
    void ImportModule();
    bool AddModulePath(const std::string& path);
    PyObject* CallFunction(const std::string& moduleName, const std::string& funcName, PyObject* args = nullptr);
    PyObject* RunSimpleString(const std::string& code);
    void Finalize();
    /// Execute script file. Return true if successful.
    bool ExecuteFile(const ea::string& fileName);
    /// Execute script string. Return true if successful.
    bool ExecuteString(const ea::string& string);
    /// Load script file on file system (i.e. not from resource cache). Return true if successful.
    bool LoadRawFile(const ea::string& fileName);
    /// Load and execute script file on file system (i.e. not from resource cache). Return true if successful.
    bool ExecuteRawFile(const ea::string& fileName);
    /// Execute script function.
    bool ExecuteFunction(const ea::string& functionName);
    void AddEventHandler(const ea::string& eventName, nanobind::callable function) override;
    /// Add a scripted event handler by function at the given stack index.
    void AddEventHandler(const ea::string& eventName, int index) override;
    /// Add a scripted event handler by function name.
    void AddEventHandler(const ea::string& eventName, const ea::string& functionName) override;
    void AddEventHandler(Object* sender, const ea::string& eventName, nanobind::callable function) override;
    /// Add a scripted event handler by function at the given stack index for a specific sender.
    void AddEventHandler(Object* sender, const ea::string& eventName, int index) override;
    /// Add a scripted event handler by function name for a specific sender.
    void AddEventHandler(Object* sender, const ea::string& eventName, const ea::string& functionName) override;
    /// Remove a scripted event handler.
    void RemoveEventHandler(const ea::string& eventName) override;
    /// Remove a scripted event handler for a specific sender.
    void RemoveEventHandler(Object* sender, const ea::string& eventName) override;
    /// Remove all scripted event handlers for a specific sender.
    void RemoveEventHandlers(Object* sender) override;
    /// Remove all scripted event handlers.
    void RemoveAllEventHandlers() override;
    /// Remove all scripted event handlers, except those listed.
    void RemoveEventHandlersExcept(const ea::vector<ea::string>& exceptionNames) override;
    /// Return whether has subscribed to an event.
    bool HasEventHandler(const ea::string& eventName) const override;
    /// Return whether has subscribed to a specific sender's event.
    bool HasEventHandler(Object* sender, const ea::string& eventName) const override;
    nanobind::callable GetFunction(const ea::string& functionName, bool silentIfNotFound = false);

private:
    bool initialized_{ false };
    PyThreadState* main_thread_state_{ nullptr };
    /// Procedural event invoker.
    SharedPtr<PythonScriptEventInvoker> eventInvoker_;
    /// Coroutine update function.
    nanobind::callable coroutineUpdate_;
    /// Function name to function map.
    ea::unordered_map<ea::string, nanobind::callable> functionNameToFunctionMap_;
};

/// Register Python script library objects.
void URHO3D_API RegisterPythonScriptLibrary(Context* context);

bool URHO3D_API RunPython(Context* context, const ea::string& scriptFileName);

}

// nb::object call_python(nb::object py_func, nb::object arg)
// {
//     PyObject* result = PyObject_CallFunctionObjArgs(py_func.ptr(), arg.ptr(), nullptr);
//     if (!result) {
//         PyObject *ptype, *pvalue, *ptraceback;
//         PyErr_Fetch(&ptype, &pvalue, &ptraceback);
//
//         const char* err_msg = pvalue ? PyUnicode_AsUTF8(pvalue) : "Unknown error";
//         URHO3D_LOGERRORF("%s", err_msg);
//
//         Py_XDECREF(ptype);
//         Py_XDECREF(pvalue);
//         Py_XDECREF(ptraceback);
//         return nb::none();
//     }
//     return nb::steal(result);
// }

// #define CALL_PYTHON(func, ...) \
//     if (PyObject* result = func(__VA_ARGS__); !result) { \
//         PyObject *ptype, *pvalue, *ptraceback; \
//         PyErr_Fetch(&ptype, &pvalue, &ptraceback); \
//         const char* err_msg = pvalue ? PyUnicode_AsUTF8(pvalue) : "Unknown error"; \
//         URHO3D_LOGERRORF("%s", err_msg); \
//         Py_XDECREF(ptype); \
//         Py_XDECREF(pvalue); \
//         Py_XDECREF(ptraceback); \
//     }

#define CALL_PYTHON(func, ...) \
    nb::object result; \
    try { \
        result = func(__VA_ARGS__); \
    } catch (const nb::python_error& e) { \
        URHO3D_LOGERRORF("%s", e.what()); \
    }

template <typename F, typename... Args> auto CallPythonFunction(F function, Args&&... args)
{
    try {
        nb::object result = function(std::forward<Args>(args)...);
    } catch (const nb::python_error& e) {
        URHO3D_LOGERRORF("%s", e.what());
    }
}
