//#include "../Precompiled.h"
#include "PythonScript.h"
#include "../Core/CoreEvents.h"
#include "../Core/ProcessUtils.h"
#include "../Core/Profiler.h"
#include "../Engine/Engine.h"
#include "../Engine/EngineEvents.h"
#include "../Engine/EngineDefs.h"
#include "../IO/File.h"
#include "../IO/FileSystem.h"
#include "../IO/Log.h"
#include "../Resource/ResourceCache.h"
#include "../Scene/Scene.h"
#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <vector>
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

Urho3D::PythonScript* g_python_script = nullptr;

namespace Urho3D
{
StringVariantMap& GetEngineParameters();

PythonScript::PythonScript(Context* context) :
    Object(context)
{
    Initialize();
}

PythonScript::~PythonScript()
{
    Finalize();
}

void initialize_python_without_frozen()
{
    PyConfig config;
    PyConfig_InitPythonConfig(&config);
    config._install_importlib = 0;
    config.module_search_paths_set = 1;
    PyWideStringList_Append(&config.module_search_paths, L"D:/Github/rbfx-v3/Source/ThirdParty/Python/Python-3.13.2/Lib");
    PyStatus status = Py_InitializeFromConfig(&config);
    if (PyStatus_Exception(status)) {
        //fprintf(stderr, "Python初始化失败: %s\n", status.err_msg);
        Py_ExitStatusException(status);
    }
    PyConfig_Clear(&config);
}

void bootstrap_core_modules()
{
    PyObject* sys_module = PyModule_New("sys");
    PyDict_SetItemString(PyImport_GetModuleDict(), "sys", sys_module);

    PyModule_AddStringConstant(sys_module, "version", Py_GetVersion());
    PyModule_AddObject(sys_module, "path", PyList_New(0));
    PyModule_AddObject(sys_module, "modules", PyImport_GetModuleDict());

    PyObject* builtins = PyImport_ImportModule("builtins");
    if (!builtins)
    {
        PyErr_Print();
        exit(1);
    }
    PyDict_SetItemString(PyImport_GetModuleDict(), "builtins", builtins);

    PyObject* imp_module = PyImport_ImportModule("_imp");
    if (!imp_module)
    {
        PyErr_Print();
        exit(1);
    }

    PyRun_SimpleString(
        "import sys\n"
        "import _imp\n"
        "sys.meta_path = []\n"
        "sys.path_importer_cache = {}\n");
}

// PyObject* load_stdlib_module(const char* name)
// {
//     PyObject* modules = PyImport_GetModuleDict();
//     PyObject* module = PyDict_GetItemString(modules, name);
//     if (module)
//     {
//         Py_INCREF(module);
//         return module;
//     }
// 
//     if (strcmp(name, "sys") == 0)
//     {
//         return PyImport_GetModule(name);
//     }
// 
//     PyObject* py_name = PyUnicode_FromString(name);
//     module = PyImport_Import(py_name);
//     Py_DECREF(py_name);
// 
//     if (!module)
//     {
//         PyErr_Print();
//         return NULL;
//     }
// 
//     PyDict_SetItemString(modules, name, module);
//     return module;
// }
/*
PyObject* custom_import(PyObject* self, PyObject* args)
{
    const char* name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return nullptr;

    std::string code;// = your_load_module_code(name);
    PyObject* dict = PyDict_New();
    PyObject* result = PyRun_String(code.c_str(), Py_file_input, dict, dict);

    if (!result)
        return nullptr;

    PyObject* module = PyModule_New(name);
    PyModule_ExecDict(module, dict);

    Py_DECREF(dict);
    return module;
}

void override_import()
{
    PyObject* builtins = PyEval_GetBuiltins();
    PyObject* import_func = PyCFunction_New(&(PyMethodDef){"custom_import", custom_import, METH_VARARGS, nullptr}, nullptr);

    PyDict_SetItemString(builtins, "__import__", import_func);
    Py_DECREF(import_func);
}

extern "C" {
    static PyObject* original_import = NULL;

    static PyObject* custom_import(PyObject* self, PyObject* args, PyObject* kwargs)
    {
        static const char* kwlist[] = {"name", "globals", "locals", "fromlist", "level", NULL};

        PyObject *name, *globals = NULL, *locals = NULL, *fromlist = NULL;
        int level = 0;

        if (!PyArg_ParseTupleAndKeywords(args, kwargs, "U|OOOi:__import__", (char​ * *​) kwlist, &name, &globals,
                &locals, &fromlist, &level))
        {
            return NULL;
        }

        // 获取模块名称
        const char* mod_name = PyUnicode_AsUTF8(name);
        if (!mod_name)
            return NULL;

        // 这里实现自定义的模块加载逻辑
        PyObject* module = your_custom_import_logic(mod_name);
        if (!module)
        {
            // 回退到原版 __import__ 如果自定义加载失败
            static PyObject* builtins = NULL;
            static PyObject* orig_import = NULL;

            PyThreadState* tstate = PyThreadState_Get();
            if (!builtins)
            {
                builtins = PyEval_GetBuiltins();
                orig_import = PyDict_GetItemString(builtins, "__import__");
                Py_XINCREF(orig_import);
            }

            if (orig_import)
            {
                module = PyObject_Call(orig_import, args, kwargs);
            }
            else
            {
                PyErr_SetString(PyExc_ImportError, "Import fallback failed");
            }
        }

        return module;
    }

    void install_custom_importer()
    {
        PyObject* builtins = PyEval_GetBuiltins();
        original_import = PyDict_GetItemString(builtins, "__import__");
        Py_XINCREF(original_import);

        PyMethodDef def = { "__import__", (PyCFunction)custom_import, METH_VARARGS | METH_KEYWORDS, NULL };
        PyObject* func = PyCFunction_New(&def, NULL);

        if (PyDict_SetItemString(builtins, "__import__", func) < 0)
        {
            Py_DECREF(func);
            return;
        }
        Py_DECREF(func);
    }

    void uninstall_custom_importer()
    {
        if (original_import)
        {
            PyObject* builtins = PyEval_GetBuiltins();
            PyDict_SetItemString(builtins, "__import__", original_import);
            Py_DECREF(original_import);
            original_import = NULL;
        }
    }
}
*/
bool PythonScript::Initialize(const std::string& pythonHome)
{
    if (initialized_) {
        return true;
    }
    PyConfig config;
    PyConfig_InitIsolatedConfig(&config);
    config.module_search_paths_set = 1;
    config._init_main = 0;
    config.site_import = 0;
//     config.use_frozen_modules = 0;
//     config._install_importlib = 0;
    std::vector<std::wstring> moduleSearchPaths = {
        L"D:/Github/rbfx-v3/Source/ThirdParty/Python/Python-3.13.2/Lib"
//         L"Script",
//         L"Script/Lib",
//         L"Script/Engine",
    };
    for (auto& path : moduleSearchPaths) {
        PyWideStringList_Append(&config.module_search_paths, path.c_str());
    }
    PyStatus status = Py_InitializeFromConfig(&config);
    PyConfig_Clear(&config);
    if (PyStatus_Exception(status)) {
        return false;
    }

//    initialize_python_without_frozen();

    status = _Py_InitializeMain();
    if (PyStatus_Exception(status))
        return false;

    if (!Py_IsInitialized()) {
        return false;
    }

//     bootstrap_core_modules();
//     PyObject* os_module = load_stdlib_module("os");
//     PyObject* json_module = load_stdlib_module("json");
// 
//     if (!os_module || !json_module) {
//         // fprintf(stderr, "无法加载必需的标准库模块\n");
//         Py_Finalize();
//         return false;
//     }
    auto ret = PyRun_SimpleString(
        "from time import time,ctime\n"
        "print('Today is', ctime(time()))\n");
    ret = PyRun_SimpleString("import sys; print('Python sys.path:', sys.path)");
    ret = PyRun_SimpleString("print('Hello from embedded Python!')");

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

bool PythonScript::ExecuteFile(const ea::string& fileName, bool fromui)
{
    return false;
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
//     if (pythonScript->ExecuteFile(scriptFileName)) {
//         //URHO3D_LOGERRORF("%s error\n\t%s", sol::to_string(status).c_str(), err.what());
//     }
    return false;
}

}
