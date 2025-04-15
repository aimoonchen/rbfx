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
#include "Python.h"
#include "marshal.h"
#include <nanobind/nanobind.h>
//#include <nanobind/stl/string.h>
#include <vector>
#include <unordered_set>
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
/*
static PyObject* g_original_open = nullptr;
static PyObject* custom_open(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* kwlist[] = {
        "file", "mode", "buffering", "encoding", "errors", "newline", "closefd", "opener", nullptr};
    const char* path = nullptr;
    const char* mode = "r";
    int buffering = -1;
    const char* encoding = nullptr;
    const char* errors = nullptr;
    const char* newline = nullptr;
    int closefd = 1;
    PyObject* opener = Py_None;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|sisspip", const_cast<char**>(kwlist), &path, &mode, &buffering,
            &encoding, &errors, &newline, &closefd, &opener))
    {
        return nullptr;
    }

    bool is_read_mode = strchr(mode, 'r') != nullptr;
    bool is_binary_mode = strchr(mode, 'b') != nullptr;

    if (is_read_mode)
    {
        std::vector<char> content;// = g_file_system.get_file_content(path);
        if (!content.empty())
        {
            PyObject* io_module = PyImport_ImportModule("io");
            if (!io_module)
            {
                return nullptr;
            }

            PyObject* io_class = nullptr;
            PyObject* content_obj = nullptr;

            if (is_binary_mode)
            {
                io_class = PyObject_GetAttrString(io_module, "BytesIO");
                content_obj = PyBytes_FromStringAndSize(content.data(), content.size());
            }
            else
            {
                io_class = PyObject_GetAttrString(io_module, "StringIO");
                content_obj = PyUnicode_FromStringAndSize(content.data(), content.size());
            }

            Py_DECREF(io_module);

            if (!io_class || !content_obj)
            {
                Py_XDECREF(io_class);
                Py_XDECREF(content_obj);
                return nullptr;
            }

            PyObject* io_obj = PyObject_CallFunctionObjArgs(io_class, content_obj, nullptr);
            Py_DECREF(io_class);
            Py_DECREF(content_obj);

            if (!io_obj)
            {
                return nullptr;
            }

            PyObject* name_obj = PyUnicode_FromString(path);
            if (name_obj)
            {
                PyObject_SetAttrString(io_obj, "name", name_obj);
                Py_DECREF(name_obj);
            }

            PyObject* mode_obj = PyUnicode_FromString(mode);
            if (mode_obj)
            {
                PyObject_SetAttrString(io_obj, "mode", mode_obj);
                Py_DECREF(mode_obj);
            }

            return io_obj;
        }
    }

    return PyObject_Call(g_original_open, args, kwargs);
}

static PyObject* install_custom_file_reader(PyObject* self, PyObject* args)
{
    PyObject* builtins_module = PyImport_ImportModule("builtins");
    if (!builtins_module)
    {
        PyErr_SetString(PyExc_RuntimeError, "无法导入builtins模块");
        return nullptr;
    }

    g_original_open = PyObject_GetAttrString(builtins_module, "open");
    if (!g_original_open)
    {
        Py_DECREF(builtins_module);
        PyErr_SetString(PyExc_RuntimeError, "无法获取原始open函数");
        return nullptr;
    }

    PyMethodDef custom_open_def = {
        "open", (PyCFunction)custom_open, METH_VARARGS | METH_KEYWORDS, "自定义文件打开函数"};

    PyObject* custom_open_func = PyCFunction_New(&custom_open_def, nullptr);
    if (!custom_open_func)
    {
        Py_DECREF(g_original_open);
        Py_DECREF(builtins_module);
        PyErr_SetString(PyExc_RuntimeError, "无法创建自定义open函数");
        return nullptr;
    }

    if (PyObject_SetAttrString(builtins_module, "open", custom_open_func) < 0)
    {
        Py_DECREF(custom_open_func);
        Py_DECREF(g_original_open);
        Py_DECREF(builtins_module);
        PyErr_SetString(PyExc_RuntimeError, "无法替换builtins.open函数");
        return nullptr;
    }

    Py_DECREF(custom_open_func);
    Py_DECREF(builtins_module);

    Py_RETURN_NONE;
}
*/
static PyObject* g_original_get_sourcefile = nullptr;

static PyObject* custom_get_sourcefile(PyObject* self, PyObject* args)
{
    const char* path;

    if (!PyArg_ParseTuple(args, "s", &path))
    {
        return nullptr;
    }
    std::vector<char> content;// = g_get_file_content(path);
    if (!content.empty())
    {
        PyObject* io_module = PyImport_ImportModule("io");
        if (!io_module)
        {
            return nullptr;
        }

        PyObject* bytes_io_class = PyObject_GetAttrString(io_module, "BytesIO");
        Py_DECREF(io_module);

        if (!bytes_io_class)
        {
            return nullptr;
        }

        PyObject* content_bytes = PyBytes_FromStringAndSize(content.data(), content.size());
        if (!content_bytes)
        {
            Py_DECREF(bytes_io_class);
            return nullptr;
        }

        PyObject* bytes_io = PyObject_CallFunctionObjArgs(bytes_io_class, content_bytes, nullptr);
        Py_DECREF(bytes_io_class);
        Py_DECREF(content_bytes);

        if (!bytes_io)
        {
            return nullptr;
        }

        return bytes_io;
    }

    return PyObject_CallObject(g_original_get_sourcefile, args);
}

static bool install_custom_loader()
{
    PyGILState_STATE gstate = PyGILState_Ensure();
    bool success = false;

    PyObject* bootstrap_external = PyImport_ImportModule("importlib._bootstrap_external");
    if (!bootstrap_external)
    {
        //std::cerr << "无法导入importlib._bootstrap_external模块" << std::endl;
        PyGILState_Release(gstate);
        return false;
    }

    g_original_get_sourcefile = PyObject_GetAttrString(bootstrap_external, "_get_sourcefile");
    if (!g_original_get_sourcefile)
    {
        //std::cerr << "无法获取_get_sourcefile函数" << std::endl;
        Py_DECREF(bootstrap_external);
        PyGILState_Release(gstate);
        return false;
    }

    PyMethodDef custom_get_sourcefile_def = {
        "_get_sourcefile", custom_get_sourcefile, METH_VARARGS, "自定义获取源文件函数"};

    PyObject* custom_get_sourcefile_func = PyCFunction_New(&custom_get_sourcefile_def, nullptr);
    if (!custom_get_sourcefile_func)
    {
        //std::cerr << "无法创建自定义_get_sourcefile函数" << std::endl;
        Py_DECREF(g_original_get_sourcefile);
        Py_DECREF(bootstrap_external);
        PyGILState_Release(gstate);
        return false;
    }

    if (PyObject_SetAttrString(bootstrap_external, "_get_sourcefile", custom_get_sourcefile_func) < 0)
    {
        //std::cerr << "无法替换_get_sourcefile函数" << std::endl;
        Py_DECREF(custom_get_sourcefile_func);
        Py_DECREF(g_original_get_sourcefile);
        Py_DECREF(bootstrap_external);
        PyGILState_Release(gstate);
        return false;
    }

    Py_DECREF(custom_get_sourcefile_func);
    Py_DECREF(bootstrap_external);

    success = true;
    PyGILState_Release(gstate);
    return success;
}

bool PythonScript::Initialize(const std::string& pythonHome)
{
    if (initialized_) {
        return true;
    }
//     PyImport_AppendInittab("_frozen_importlib", PyInit__frozen_importlib);
//     PyImport_AppendInittab("_frozen_importlib_external", PyInit__frozen_importlib_external);

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
    //
    PyConfig config;
    PyConfig_InitIsolatedConfig(&config);
    config.use_environment = 0;
    config.module_search_paths_set = 1;
    config.pathconfig_warnings = 0;
    config.site_import = 0;
    config.user_site_directory = 0;
//    config.write_bytecode = 0;
//     config.use_frozen_modules = 1;
//     config._install_importlib = 1;
//     config.module_search_paths.length = 0;
//     config.module_search_paths.items = nullptr;
    std::vector<std::wstring> moduleSearchPaths = {
        L"D:/Github/rbfx-v3/Source/ThirdParty/Python/Python-3.13.2/Lib"
//         L"Script",
//         L"Script/Lib",
//         L"Script/Engine",
    };
    for (auto& path : moduleSearchPaths) {
        PyWideStringList_Append(&config.module_search_paths, path.c_str());
    }
    status = Py_InitializeFromConfig(&config);
    PyConfig_Clear(&config);
    if (PyStatus_Exception(status)) {
        printf("Python initialize failed, %s : %s\n", status.func, status.err_msg);
        return false;
    }

    install_custom_loader();

//     PyObject* mymodule = PyInit_my_ext();
//     PyObject* sys = PyImport_ImportModule("sys");
//     PyObject* modules = PyObject_GetAttrString(sys, "modules");
//     PyDict_SetItemString(modules, "my_ext", mymodule);

    //auto ret = PyRun_SimpleString("from time import time,ctime; print('Today is', ctime(time()))\n");
    //ret = PyRun_SimpleString("import sys; print('Python sys.path:', sys.path)");
    //ret = PyRun_SimpleString("import my_ext; print('Hello from embedded Python!')");
    //auto ret = PyRun_SimpleString("import my_ext; print(my_ext)");
    auto ret = PyRun_SimpleString("import datetime; print(datetime)");
    
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
