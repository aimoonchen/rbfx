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
#include <nanobind/stl/string.h>
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

// 处理 .pyc 文件的替代方法
PyObject* loadPycFile(const char* data, size_t size, PyObject* moduleDict)
{
    // 在 Python 3.13.2 中，我们可以使用 _imp 模块来加载已编译的代码
    PyObject* impModule = PyImport_ImportModule("_imp");
    if (!impModule)
    {
        PyErr_Print();
        return nullptr;
    }

    // 跳过 .pyc 文件头部（通常是 16 字节，但在 Python 3.13 中可能有变化）
    // 头部格式: magic number (4 bytes) + 其他元数据
    size_t headerSize = 16; // 默认头部大小
    if (size <= headerSize)
    {
        PyErr_SetString(PyExc_ImportError, "无效的 .pyc 文件 (太小)");
        Py_DECREF(impModule);
        return nullptr;
    }

    // 创建一个包含字节码的字节对象
    PyObject* codeBytes = PyBytes_FromStringAndSize(data + headerSize, size - headerSize);
    if (!codeBytes)
    {
        Py_DECREF(impModule);
        return nullptr;
    }

    // 使用 _imp.create_dynamic 或 _imp.exec_dynamic 加载代码对象
    PyObject* execDynamic = PyObject_GetAttrString(impModule, "exec_dynamic");
    if (!execDynamic)
    {
        // 如果 exec_dynamic 不存在，尝试使用 create_dynamic
        PyErr_Clear();
        execDynamic = PyObject_GetAttrString(impModule, "create_dynamic");
    }

    PyObject* result = nullptr;
    if (execDynamic && PyCallable_Check(execDynamic))
    {
        // 调用 _imp 模块的函数来执行字节码
        result = PyObject_CallFunction(execDynamic, "OO", codeBytes, moduleDict);
    }
    else
    {
        // 如果上述方法都不可用，尝试使用 marshal 模块
        PyErr_Clear();
        PyObject* marshalModule = PyImport_ImportModule("marshal");
        if (marshalModule)
        {
            PyObject* loads = PyObject_GetAttrString(marshalModule, "loads");
            if (loads)
            {
                PyObject* codeObj = PyObject_CallFunction(loads, "O", codeBytes);
                if (codeObj)
                {
                    result = PyEval_EvalCode(codeObj, moduleDict, moduleDict);
                    Py_DECREF(codeObj);
                }
                Py_DECREF(loads);
            }
            Py_DECREF(marshalModule);
        }
    }

    Py_XDECREF(execDynamic);
    Py_DECREF(codeBytes);
    Py_DECREF(impModule);

    return result;
}

typedef struct
{
    PyObject_HEAD
} CustomImporter;

static PyObject* CustomImporter_find_spec(PyObject* self, PyObject* args)
{
    const char* name;
    PyObject* path;
    PyObject* target = Py_None;
    if (!PyArg_ParseTuple(args, "sO|O", &name, &path, &target))
    {
        return nullptr;
    }

    // 示例：从自定义存储加载模块（替换为实际实现）
    if (strcmp(name, "mymodule") == 0)
    {
        const char* code = "def hello(): return 'From custom loader!'";
        PyObject* module = PyModule_New(name);
        PyObject* dict = PyModule_GetDict(module);
        PyRun_String(code, Py_file_input, dict, dict);
        return PyObject_GetAttrString(module, "__spec__");
    }

    Py_RETURN_NONE;
}

static PyMethodDef CustomImporter_methods[] = {
    {"find_spec", CustomImporter_find_spec, METH_VARARGS, "Custom find_spec implementation"}, {nullptr, nullptr, 0, nullptr}};

// static PyTypeObject CustomImporterType = {
//     PyVarObject_HEAD_INIT(NULL, 0).tp_name = "CustomImporter",
//     .tp_basicsize = sizeof(CustomImporter),
//     .tp_flags = Py_TPFLAGS_DEFAULT,
//     .tp_methods = CustomImporter_methods,
// };
static PyTypeObject CustomImporterType = []()
{
    PyTypeObject type = {PyVarObject_HEAD_INIT(nullptr, 0)};
    type.tp_name = "CustomImporter";
    type.tp_basicsize = sizeof(CustomImporter);
    type.tp_flags = Py_TPFLAGS_DEFAULT;
    type.tp_methods = CustomImporter_methods;
    return type;
}();

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

//     if (PyType_Ready(&CustomImporterType) < 0) {
//         PyErr_Print();
//         return false;
//     }
// 
//     PyObject* sys_meta_path = PySys_GetObject("meta_path");
//     PyObject* importer = PyObject_CallFunctionObjArgs((PyObject*)&CustomImporterType, nullptr);
//     PyList_Insert(sys_meta_path, 0, importer);


    PyObject* mymodule = PyInit_my_ext();
    PyObject* sys = PyImport_ImportModule("sys");
    PyObject* modules = PyObject_GetAttrString(sys, "modules");
    PyDict_SetItemString(modules, "my_ext", mymodule);

    //auto ret = PyRun_SimpleString("from time import time,ctime; print('Today is', ctime(time()))\n");
    //ret = PyRun_SimpleString("import sys; print('Python sys.path:', sys.path)");
    //ret = PyRun_SimpleString("import my_ext; print('Hello from embedded Python!')");
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
