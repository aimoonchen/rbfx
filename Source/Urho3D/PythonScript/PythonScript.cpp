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
#include "../DebugNew.h"

struct Dog
{
    std::string name;

    std::string bark() const { return name + ": woof!"; }
};

namespace nb = nanobind;

NB_MODULE(my_ext, m)
{
    nb::class_<Dog>(m, "Dog")
        .def(nb::init<>())
        .def(nb::init<const std::string&>())
        .def("bark", &Dog::bark)
        .def_rw("name", &Dog::name);
}


extern "C" [[maybe_unused]] NB_EXPORT PyObject* PyInit_engine();

Urho3D::PythonScript* g_python_script = nullptr;

namespace {
Urho3D::Context* g_context = nullptr;
ea::vector<char> get_file_content(const ea::string& path)
{
    auto cache = g_context->GetSubsystem<Urho3D::ResourceCache>();
    auto file = cache->GetFile(path.c_str(), false);
    if (!file) {
        URHO3D_LOGERRORF("LoadFile failed: %s", path.c_str());
        return {};
    }
    auto size = file->GetSize();
    ea::vector<char> scriptContent;
    scriptContent.resize(file->GetSize());
    if (file->Read(scriptContent.data(), scriptContent.size()) != scriptContent.size()) {
        URHO3D_LOGERRORF("ReadFile failed: %s", path.c_str());
        return {};
    }
    return ea::move(scriptContent);
}

PyObject* g_spec_from_loader = nullptr;
PyObject* g_source_loader_class = nullptr;

bool initialize_import_cache()
{
    PyGILState_STATE gstate = PyGILState_Ensure();

    auto importlib_util = PyImport_ImportModule("importlib.util");
    auto importlib_machinery = PyImport_ImportModule("importlib.machinery");

    if (!importlib_util || !importlib_machinery)
    {
        Py_XDECREF(importlib_util);
        Py_XDECREF(importlib_machinery);
        PyGILState_Release(gstate);
        return false;
    }

    g_spec_from_loader = PyObject_GetAttrString(importlib_util, "spec_from_loader");
    g_source_loader_class = PyObject_GetAttrString(importlib_machinery, "SourceFileLoader");
    Py_XDECREF(importlib_util);
    Py_XDECREF(importlib_machinery);
    if (!g_spec_from_loader || !g_source_loader_class)
    {
        PyGILState_Release(gstate);
        return false;
    }

    PyGILState_Release(gstate);
    return true;
}

void cleanup_import_cache()
{
    PyGILState_STATE gstate = PyGILState_Ensure();

    Py_XDECREF(g_spec_from_loader);
    Py_XDECREF(g_source_loader_class);
    g_spec_from_loader = nullptr;
    g_source_loader_class = nullptr;

    PyGILState_Release(gstate);
}
}
namespace Urho3D
{
StringVariantMap& GetEngineParameters();

PythonScript::PythonScript(Context* context) :
    Object(context)
{
    g_context = context;
    Initialize();
    g_python_script = this;
}

PythonScript::~PythonScript()
{
    Finalize();
}

PyObject* custom_get_data(PyObject* self, PyObject* args)
{
    const char* path;
    if (!PyArg_ParseTuple(args, "s", &path))
    {
        return nullptr;
    }

    ea::vector<char> content = get_file_content(path);
    if (!content.empty())
    {
        return PyBytes_FromStringAndSize(content.data(), content.size());
    }

    PyObject* original_get_data = PyObject_GetAttrString(self, "_original_get_data");
    if (original_get_data)
    {
        PyObject* result = PyObject_CallObject(original_get_data, args);
        Py_DECREF(original_get_data);
        return result;
    }

    PyErr_SetString(PyExc_FileNotFoundError, path);
    return nullptr;
}

static PyObject* custom_find_spec(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* kwlist[] = {"fullname", "path", "target", nullptr};
    PyObject* fullname;
    PyObject* path = Py_None;
    PyObject* target = Py_None;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OO", const_cast<char**>(kwlist), &fullname, &path, &target)) {
        return nullptr;
    }

    if (!PyUnicode_Check(fullname)) {
        //PyErr_SetString(PyExc_TypeError, "模块名必须是字符串");
        return nullptr;
    }

    const char* module_name = PyUnicode_AsUTF8(fullname);
    if (!module_name) {
        return nullptr;
    }

    ea::vector<ea::string> possible_paths;
    ea::string module_path(module_name);
    ea::replace(module_path.begin(), module_path.end(), '.', '/');
//     possible_paths.push_back("/" + module_path + ".py");
//     possible_paths.push_back("/" + module_path + "/__init__.py");
    possible_paths.push_back("Scripts/" + module_path + ".py");
    possible_paths.push_back("Scripts/" + module_path + "/__init__.py");

    ea::vector<char> content;
    ea::string found_path;
    bool is_package = false;

    for (const auto& possible_path : possible_paths) {
        content = get_file_content(possible_path);
        if (!content.empty()) {
            found_path = possible_path;
            is_package = (possible_path.find("__init__.py") != ea::string::npos);
            break;
        }
    }

    if (content.empty()) {
        Py_RETURN_NONE;
    }

    PyObject* loader = PyObject_CallFunction(g_source_loader_class, "Os", fullname, found_path.c_str());
    if (!loader) {
        return nullptr;
    }

    static PyMethodDef get_data_method = {"get_data", (PyCFunction)custom_get_data, METH_VARARGS, "获取文件内容"};

    PyObject* get_data_func = PyCFunction_New(&get_data_method, loader);
    if (!get_data_func) {
        Py_DECREF(loader);
        return nullptr;
    }

    PyObject* original_get_data = PyObject_GetAttrString(loader, "get_data");
    if (original_get_data) {
        PyObject_SetAttrString(loader, "_original_get_data", original_get_data);
        Py_DECREF(original_get_data);
    }

    PyObject_SetAttrString(loader, "get_data", get_data_func);
    Py_DECREF(get_data_func);

    PyObject* spec_args = is_package ? Py_BuildValue("OOi", fullname, loader, 1) : Py_BuildValue("OO", fullname, loader);

    if (!spec_args) {
        Py_DECREF(loader);
        return nullptr;
    }

    PyObject* spec = PyObject_CallObject(g_spec_from_loader, spec_args);
    Py_DECREF(spec_args);
    Py_DECREF(loader);

    return spec;
}

static bool install_virtual_importer()
{
    if (!initialize_import_cache()) {
        return false;
    }

    PyGILState_STATE gstate = PyGILState_Ensure();
    bool success = false;

    static PyMethodDef finder_methods[] = {
        {"find_spec", (PyCFunction)custom_find_spec, METH_VARARGS | METH_KEYWORDS, "查找模块规格"},
        {nullptr, nullptr, 0, nullptr}
    };

    static PyType_Slot finder_slots[] = {
        {Py_tp_new, (void*)PyType_GenericNew}, {Py_tp_methods, finder_methods}, {0, nullptr}
    };

    static PyType_Spec finder_spec = {"virtual_importer.Finder", sizeof(PyObject), 0, Py_TPFLAGS_DEFAULT, finder_slots};

    PyTypeObject* finder_type = (PyTypeObject*)PyType_FromSpec(&finder_spec);
    if (!finder_type) {
        PyGILState_Release(gstate);
        return false;
    }

    PyObject* finder = PyObject_CallObject((PyObject*)finder_type, nullptr);
    Py_DECREF(finder_type);

    if (finder) {
        PyObject* sys_module = PyImport_ImportModule("sys");
        if (sys_module) {
            PyObject* meta_path = PyObject_GetAttrString(sys_module, "meta_path");
            if (meta_path && PyList_Check(meta_path)) {
                if (PyList_Insert(meta_path, 0, finder) == 0) {
                    success = true;
                }
                Py_DECREF(meta_path);
            }
            Py_DECREF(sys_module);
        }
        Py_DECREF(finder);
    }

    PyGILState_Release(gstate);
    return success;
}

void PythonScript::RegisterModule()
{
    auto ret = PyImport_AppendInittab("my_ext", PyInit_my_ext);
    if (ret != 0) {
        ;
    }
}

void PythonScript::ImportModule()
{
    PyObject* my_ext_module = PyImport_ImportModule("my_ext");
    if (my_ext_module) {
        Py_DECREF(my_ext_module);
    }
    else {
        ;
    }
}

bool PythonScript::Initialize()
{
    if (initialized_) {
        return true;
    }
    URHO3D_LOGINFOF("Python version: %s\n", PY_VERSION);
    URHO3D_LOGINFOF("Python runtime version: %s\n", Py_GetVersion());
    PyStatus status;
//     PyPreConfig preconfig;
//     PyPreConfig_InitIsolatedConfig(&preconfig);
//     preconfig.configure_locale = 0;
//     preconfig.utf8_mode = 1;
//     preconfig.coerce_c_locale = 0;
//     status = Py_PreInitialize(&preconfig);
//     if (PyStatus_Exception(status)) {
//         printf("Python initialize failed, %s : %s\n", status.func, status.err_msg);
//         return false;
//     }
    PyConfig config = {};
    PyConfig_InitIsolatedConfig(&config);
    config.use_environment = 0;
    config.module_search_paths_set = 1;
    config.pathconfig_warnings = 0;
    config.site_import = 0;
    config.user_site_directory = 0;
//     config.write_bytecode = 0;
//     config.use_frozen_modules = 1;
//     config._install_importlib = 1;
//     config.module_search_paths.length = 0;
//     config.module_search_paths.items = nullptr;

    status = PyWideStringList_Append(&config.module_search_paths, L"D:/Github/rbfx-v3/Source/ThirdParty/Python/Python-3.13.2/Lib");
    if (PyStatus_Exception(status)) {
        PyConfig_Clear(&config);
        URHO3D_LOGERRORF("Failed to append module search path: %s\n", status.err_msg);
        return false;
    }

    RegisterModule();

    status = Py_InitializeFromConfig(&config);
    PyConfig_Clear(&config);
    if (PyStatus_Exception(status)) {
        URHO3D_LOGERRORF("Python initialize failed, %s : %s\n", status.func, status.err_msg);
        return false;
    }

    if (!install_virtual_importer()) {
        return false;
    }

    ImportModule();

    auto ret = PyRun_SimpleString("import my_ext; print(my_ext)");
    
    main_thread_state_ = PyEval_SaveThread();
    initialized_ = true;
    g_python_script = this;
    return true;
}

bool PythonScript::AddModulePath(const std::string& path)
{
    if (!initialized_) {
        return false;
    }

    PyGILState_STATE gstate = PyGILState_Ensure();

    PyObject* sysModule = PyImport_ImportModule("sys");
    if (!sysModule)
    {
        PyGILState_Release(gstate);
        return false;
    }

    PyObject* sysPath = PyObject_GetAttrString(sysModule, "path");
    if (!sysPath)
    {
        Py_DECREF(sysModule);
        PyGILState_Release(gstate);
        return false;
    }

    PyObject* pathObj = PyUnicode_FromString(path.c_str());

    if (!pathObj)
    {
        Py_DECREF(sysPath);
        Py_DECREF(sysModule);
        PyGILState_Release(gstate);
        return false;
    }

    int result = PyList_Append(sysPath, pathObj);
    Py_DECREF(pathObj);
    Py_DECREF(sysPath);
    Py_DECREF(sysModule);

    PyGILState_Release(gstate);
    return (result == 0);
}

PyObject* PythonScript::RunSimpleString(const std::string& code)
{
    if (!initialized_)
    {
        return nullptr;
    }

    PyGILState_STATE gstate = PyGILState_Ensure();

    PyObject* mainModule = PyImport_AddModule("__main__");
    PyObject* globalDict = PyModule_GetDict(mainModule);

    PyObject* result = PyRun_String(code.c_str(), Py_eval_input, globalDict, globalDict);

    if (PyErr_Occurred())
    {
        PyErr_Print();
        PyGILState_Release(gstate);
        return nullptr;
    }

    PyGILState_Release(gstate);
    return result;
}

PyObject* PythonScript::CallFunction(const std::string& moduleName, const std::string& funcName, PyObject* args)
{
    if (!initialized_)
    {
        return nullptr;
    }

    PyGILState_STATE gstate = PyGILState_Ensure();

    PyObject* pModule = PyImport_ImportModule(moduleName.c_str());
    if (!pModule)
    {
        PyErr_Print();
        PyGILState_Release(gstate);
        return nullptr;
    }

    PyObject* pFunc = PyObject_GetAttrString(pModule, funcName.c_str());
    if (!pFunc || !PyCallable_Check(pFunc))
    {
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
        PyGILState_Release(gstate);
        return nullptr;
    }

    PyObject* pResult = nullptr;
    if (args)
    {
        pResult = PyObject_CallObject(pFunc, args);
    }
    else
    {
        pResult = PyObject_CallObject(pFunc, nullptr);
    }

    if (PyErr_Occurred())
    {
        PyErr_Print();
        Py_XDECREF(pResult);
        pResult = nullptr;
    }

    Py_DECREF(pFunc);
    Py_DECREF(pModule);

    PyGILState_Release(gstate);
    return pResult;
}

static PyObject* toPyString(const std::string& str)
{
    return PyUnicode_FromString(str.c_str());
}

static std::string toCppString(PyObject* pyStr)
{
    if (!pyStr || !PyUnicode_Check(pyStr))
    {
        return "";
    }
    return PyUnicode_AsUTF8(pyStr);
}

void PythonScript::Finalize()
{
    if (!initialized_)
    {
        return;
    }

    PyEval_RestoreThread(main_thread_state_);
    main_thread_state_ = nullptr;

    Py_Finalize();

    initialized_ = false;
}

void PythonScript::AddEventHandler(const ea::string& eventName, nanobind::callable function)
{
    eventInvoker_->AddEventHandler(nullptr, eventName, function);
    python_functions_.emplace_back(function);
}

void PythonScript::AddEventHandler(const ea::string& eventName, int index)
{
    assert(false);
    //     LuaFunction* function = GetFunction(index);
    //     if (function)
    //         eventInvoker_->AddEventHandler(nullptr, eventName, function);
}

void PythonScript::AddEventHandler(const ea::string& eventName, const ea::string& functionName)
{
    auto function = GetFunction(functionName);
    if (function)
        eventInvoker_->AddEventHandler(nullptr, eventName, function);
}

void PythonScript::AddEventHandler(Object* sender, const ea::string& eventName, nanobind::callable function)
{
    if (!sender)
    {
        return;
    }
    eventInvoker_->AddEventHandler(sender, eventName, function);
    python_functions_.emplace_back(function);
}

void PythonScript::AddEventHandler(Object* sender, const ea::string& eventName, int index)
{
    assert(false);
    //     if (!sender)
    //         return;
    //
    //     LuaFunction* function = GetFunction(index);
    //     if (function)
    //         eventInvoker_->AddEventHandler(sender, eventName, function);
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
    ea::vector<char> scriptContent = get_file_content(filename);
    if (scriptContent.empty()) {
        return false;
    }
    PyGILState_STATE gstate = PyGILState_Ensure();

    PyObject* mainModule = PyImport_AddModule("__main__");
    if (!mainModule) {
        PyGILState_Release(gstate);
        return false;
    }

    PyObject* globalsDict = PyModule_GetDict(mainModule);

    PyObject* filenameObj = PyUnicode_FromString(filename.c_str());
    if (PyDict_SetItemString(globalsDict, "__file__", filenameObj) < 0) {
        Py_DECREF(filenameObj);
        PyGILState_Release(gstate);
        return false;
    }
    Py_DECREF(filenameObj);

    PyObject* codeString = PyUnicode_FromStringAndSize(scriptContent.data(), scriptContent.size());
    if (!codeString) {
        PyObject* codeBytes = PyBytes_FromStringAndSize(scriptContent.data(), scriptContent.size());
        if (!codeBytes) {
            PyGILState_Release(gstate);
            return false;
        }

        codeString = PyUnicode_FromEncodedObject(codeBytes, "utf-8", "strict");
        Py_DECREF(codeBytes);

        if (!codeString) {
            PyGILState_Release(gstate);
            return false;
        }
    }

    //     PyObject* result = PyRun_String(PyUnicode_AsUTF8(codeString), Py_file_input, globalsDict, globalsDict);
    //     Py_DECREF(codeString);

    PyObject* compiled = Py_CompileString(PyUnicode_AsUTF8(codeString), filename.c_str(), Py_file_input);
    Py_DECREF(codeString);
    if (!compiled) {
        PyErr_Print();
        PyGILState_Release(gstate);
        return false;
    }

    PyObject* result = PyEval_EvalCode(compiled, globalsDict, globalsDict);
    Py_DECREF(compiled);

    bool success = (result != nullptr);
    if (result) {
        Py_DECREF(result);
    } else {
        PyErr_Print();
    }

    // test
    const char* function_name = "Start";
    PyObject* func = PyDict_GetItemString(globalsDict, function_name);
    if (func && PyCallable_Check(func)) {
        PyObject* result = PyObject_CallObject(func, nullptr);
        if (result) {
            printf("Function returned: ");
            PyObject_Print(result, stdout, 0);
            Py_DECREF(result);
        } else {
            PyErr_Print();
        }
    } else {
        printf("Function '%s' not found or not callable\n", function_name);
    }

    PyGILState_Release(gstate);

    return success;
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
