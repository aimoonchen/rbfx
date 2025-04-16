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
/*
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
*/
typedef std::function<std::vector<char>(const std::string&)> GetFileContentFunc;
static GetFileContentFunc g_get_file_content = nullptr;
static PyObject* custom_find_spec(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* kwlist[] = {"fullname", "path", "target", NULL};
    PyObject* fullname;
    PyObject* path = Py_None;
    PyObject* target = Py_None;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OO", const_cast<char**>(kwlist), &fullname, &path, &target))
    {
        return NULL;
    }

    // 获取模块名
    if (!PyUnicode_Check(fullname))
    {
        //PyErr_SetString(PyExc_TypeError, "模块名必须是字符串");
        return NULL;
    }

    const char* module_name = PyUnicode_AsUTF8(fullname);
    if (!module_name)
    {
        return NULL;
    }

    printf("查找模块: %s\n", module_name);

    // 将模块名转换为可能的文件路径
    std::string module_path = module_name;
    std::replace(module_path.begin(), module_path.end(), '.', '/');

    // 尝试查找模块文件
    std::string py_file_path = module_path + ".py";
    std::string init_file_path = module_path + "/__init__.py";

    std::vector<char> content;
    std::string final_path;
    bool is_package = false;

    // 先尝试作为包导入
    if (g_get_file_content)
    {
        content = g_get_file_content(init_file_path);
        if (!content.empty())
        {
            final_path = init_file_path;
            is_package = true;
            printf("找到包: %s\n", init_file_path.c_str());
        }
        else
        {
            // 再尝试作为模块导入
            content = g_get_file_content(py_file_path);
            if (!content.empty())
            {
                final_path = py_file_path;
                printf("找到模块: %s\n", py_file_path.c_str());
            }
        }
    }

    if (content.empty())
    {
        printf("未找到模块: %s\n", module_name);
        Py_RETURN_NONE; // 找不到模块
    }

    // 导入必要的Python模块
    PyObject* importlib_util = PyImport_ImportModule("importlib.util");
    if (!importlib_util)
    {
        return NULL;
    }

    PyObject* importlib_machinery = PyImport_ImportModule("importlib.machinery");
    if (!importlib_machinery)
    {
        Py_DECREF(importlib_util);
        return NULL;
    }

    // 创建源代码加载器
    PyObject* source_loader_class = PyObject_GetAttrString(importlib_machinery, "SourceFileLoader");
    if (!source_loader_class)
    {
        Py_DECREF(importlib_util);
        Py_DECREF(importlib_machinery);
        return NULL;
    }

    // 创建自定义加载器
    PyObject* loader = PyObject_CallFunction(source_loader_class, "Os", fullname, final_path.c_str());
    Py_DECREF(source_loader_class);

    if (!loader)
    {
        Py_DECREF(importlib_util);
        Py_DECREF(importlib_machinery);
        return NULL;
    }

    // 创建ModuleSpec
    PyObject* spec_from_loader = PyObject_GetAttrString(importlib_util, "spec_from_loader");
    if (!spec_from_loader)
    {
        Py_DECREF(loader);
        Py_DECREF(importlib_util);
        Py_DECREF(importlib_machinery);
        return NULL;
    }

    PyObject* spec;
    if (is_package)
    {
        spec = PyObject_CallFunction(spec_from_loader, "OOi", fullname, loader, 1);
    }
    else
    {
        spec = PyObject_CallFunction(spec_from_loader, "OO", fullname, loader);
    }

    Py_DECREF(spec_from_loader);

    if (!spec)
    {
        Py_DECREF(loader);
        Py_DECREF(importlib_util);
        Py_DECREF(importlib_machinery);
        return NULL;
    }

    // 创建PyMethodDef结构体
    PyMethodDef get_data_method = {"get_data", [](PyObject* self, PyObject* args) -> PyObject*
    {
        const char* path;
        if (!PyArg_ParseTuple(args, "s", &path))
        {
            return NULL;
        }

        printf("获取文件内容: %s\n", path);

        // 从自定义文件系统获取内容
        if (g_get_file_content)
        {
            std::vector<char> content = g_get_file_content(path);
            if (!content.empty())
            {
                printf("从虚拟文件系统获取内容成功: %s (%zu 字节)\n", path, content.size());
                return PyBytes_FromStringAndSize(content.data(), content.size());
            }
        }

        // 如果找不到内容，调用原始方法
        PyObject* original_get_data = PyObject_GetAttrString(self, "_original_get_data");
        if (original_get_data)
        {
            PyObject* result = PyObject_CallObject(original_get_data, args);
            Py_DECREF(original_get_data);
            return result;
        }

        PyErr_SetString(PyExc_FileNotFoundError, path);
        return NULL;
    }, METH_VARARGS, "获取文件内容"};

    // 然后使用这个结构体创建函数对象
    PyObject* get_data_func = PyCFunction_New(&get_data_method, NULL);

    // 保存原始get_data方法
    PyObject* original_get_data = PyObject_GetAttrString(loader, "get_data");
    if (original_get_data)
    {
        PyObject_SetAttrString(loader, "_original_get_data", original_get_data);
        Py_DECREF(original_get_data);
    }

    // 设置自定义get_data方法
    PyObject_SetAttrString(loader, "get_data", get_data_func);
    Py_DECREF(get_data_func);

    // 清理引用
    Py_DECREF(loader);
    Py_DECREF(importlib_util);
    Py_DECREF(importlib_machinery);

    return spec;
}

// 安装自定义导入钩子
bool install_virtual_importer()
{
    // 获取GIL
    PyGILState_STATE gstate = PyGILState_Ensure();
    bool success = false;

    // 首先定义PyMethodDef数组
    PyMethodDef finder_methods[] = {
        {"find_spec", (PyCFunction)custom_find_spec, METH_VARARGS | METH_KEYWORDS, "查找模块规格"},
        {NULL, NULL, 0, NULL}};

    // 然后定义PyType_Slot数组
    PyType_Slot finder_slots[] = {{Py_tp_new, (void*)PyType_GenericNew}, {Py_tp_methods, finder_methods}, {0, NULL}};

    // 最后定义PyType_Spec结构体
    PyType_Spec finder_spec = {"virtual_importer.Finder", sizeof(PyObject), 0, Py_TPFLAGS_DEFAULT, finder_slots};

    // 创建自定义Finder类型
    PyTypeObject* finder_type = (PyTypeObject*)PyType_FromSpec(&finder_spec);

    if (!finder_type)
    {
        printf("无法创建自定义Finder类型\n");
        PyGILState_Release(gstate);
        return false;
    }

    // 创建Finder实例
    PyObject* finder = PyObject_CallObject((PyObject*)finder_type, NULL);
    Py_DECREF(finder_type);

    if (!finder)
    {
        printf("无法创建自定义Finder实例\n");
        PyGILState_Release(gstate);
        return false;
    }

    // 获取sys.meta_path
    PyObject* sys_module = PyImport_ImportModule("sys");
    if (!sys_module)
    {
        printf("无法导入sys模块\n");
        Py_DECREF(finder);
        PyGILState_Release(gstate);
        return false;
    }

    PyObject* meta_path = PyObject_GetAttrString(sys_module, "meta_path");
    Py_DECREF(sys_module);

    if (!meta_path)
    {
        printf("无法获取sys.meta_path\n");
        Py_DECREF(finder);
        PyGILState_Release(gstate);
        return false;
    }

    // 将Finder添加到meta_path的最前面
    if (PyList_Insert(meta_path, 0, finder) < 0)
    {
        printf("无法将Finder添加到sys.meta_path\n");
        Py_DECREF(meta_path);
        Py_DECREF(finder);
        PyGILState_Release(gstate);
        return false;
    }

    Py_DECREF(meta_path);
    Py_DECREF(finder);

    success = true;
    printf("虚拟导入器安装成功\n");
    PyGILState_Release(gstate);
    return success;
}

std::vector<char> get_file_from_binary_package(const std::string& path) {
    return {};
}

bool RunScriptFromMemory(const std::vector<char>& scriptContent, const std::string& filename)
{
    // 获取GIL
    PyGILState_STATE gstate = PyGILState_Ensure();

    // 获取主模块和全局字典
    PyObject* mainModule = PyImport_AddModule("__main__");
    if (!mainModule)
    {
        PyGILState_Release(gstate);
        return false;
    }

    PyObject* globalsDict = PyModule_GetDict(mainModule);

    // 设置__file__变量
    PyObject* filenameObj = PyUnicode_FromString(filename.c_str());
    if (PyDict_SetItemString(globalsDict, "__file__", filenameObj) < 0)
    {
        Py_DECREF(filenameObj);
        PyGILState_Release(gstate);
        return false;
    }
    Py_DECREF(filenameObj);

    // 从二进制内容创建Python字符串
    PyObject* codeString = PyUnicode_FromStringAndSize(scriptContent.data(), scriptContent.size());
    if (!codeString)
    {
        // 如果直接转换失败，尝试通过bytes对象转换
        PyObject* codeBytes = PyBytes_FromStringAndSize(scriptContent.data(), scriptContent.size());
        if (!codeBytes)
        {
            PyGILState_Release(gstate);
            return false;
        }

        codeString = PyUnicode_FromEncodedObject(codeBytes, "utf-8", "strict");
        Py_DECREF(codeBytes);

        if (!codeString)
        {
            PyGILState_Release(gstate);
            return false;
        }
    }

    // 执行脚本
    PyObject* result = PyRun_String(PyUnicode_AsUTF8(codeString), Py_file_input, globalsDict, globalsDict);

    Py_DECREF(codeString);

    bool success = (result != NULL);
    if (result)
    {
        Py_DECREF(result);
    }
    else
    {
        PyErr_Print();
    }

    // 释放GIL
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

    install_virtual_importer();

//     PyObject* mymodule = PyInit_my_ext();
//     PyObject* sys = PyImport_ImportModule("sys");
//     PyObject* modules = PyObject_GetAttrString(sys, "modules");
//     PyDict_SetItemString(modules, "my_ext", mymodule);

    //auto ret = PyRun_SimpleString("from time import time,ctime; print('Today is', ctime(time()))\n");
    //ret = PyRun_SimpleString("import sys; print('Python sys.path:', sys.path)");
    //ret = PyRun_SimpleString("import my_ext; print('Hello from embedded Python!')");
    //auto ret = PyRun_SimpleString("import my_ext; print(my_ext)");
    auto ret = PyRun_SimpleString("import mymodule; print(datetime)");
    
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
