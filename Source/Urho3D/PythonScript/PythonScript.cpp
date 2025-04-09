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

extern "C" {
    static PyObject* original_import = nullptr;
    static PyObject* custom_import(PyObject* self, PyObject* args, PyObject* kwargs)
    {
        static char* kwlist[] = {
            (char*)"name", (char*)"globals", (char*)"locals", (char*)"fromlist", (char*)"level", nullptr };

        PyObject* name;
        PyObject* globals = nullptr;
        PyObject* locals = nullptr;
        PyObject* fromlist = nullptr;
        int level = 0;

        if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OOOi", kwlist, &name, &globals, &locals, &fromlist, &level))
        {
            return nullptr;
        }

        const char* moduleName = PyUnicode_AsUTF8(name);
        if (!moduleName)
        {
            return PyObject_Call(original_import, args, kwargs);
        }

        // 构建可能的模块文件路径
        std::string moduleFilePath = std::string(moduleName);
        std::replace(moduleFilePath.begin(), moduleFilePath.end(), '.', '/');

        std::string pyFilePath = moduleFilePath + ".py";
        std::string pycFilePath = moduleFilePath + ".pyc";

        // 检查模块是否在自定义文件系统中
        bool moduleInCustomFS = true;// g_fileSystem.fileExists(pyFilePath) || g_fileSystem.fileExists(pycFilePath);

        if (moduleInCustomFS)
        {
            // 从自定义文件系统加载模块内容
            std::vector<char> moduleContent;
            std::string filePath;

            //         if (g_fileSystem.fileExists(pycFilePath))
            //         {
            //             moduleContent = g_fileSystem.readFile(pycFilePath);
            //             filePath = pycFilePath;
            //         }
            //         else
            //         {
            //             moduleContent = g_fileSystem.readFile(pyFilePath);
            //             filePath = pyFilePath;
            //         }

            if (moduleContent.empty())
            {
                // 如果无法读取内容，回退到原始导入
                return PyObject_Call(original_import, args, kwargs);
            }

            // 创建模块对象
            PyObject* module = PyImport_AddModule(moduleName);
            if (!module)
            {
                return PyObject_Call(original_import, args, kwargs);
            }

            // 获取模块字典
            PyObject* moduleDict = PyModule_GetDict(module);

            // 设置 __file__ 属性
            PyObject* filePathObj = PyUnicode_FromString(filePath.c_str());
            PyDict_SetItemString(moduleDict, "__file__", filePathObj);
            Py_DECREF(filePathObj);

            // 执行模块代码
            PyObject* result = nullptr;

            if (filePath.substr(filePath.length() - 4) == ".pyc"/*filePath.ends_with(".pyc")*/)
            {
                // 处理编译后的 .pyc 文件
                // 跳过 .pyc 文件头部（通常是 16 字节）
                const char* codeData = moduleContent.data();
                size_t codeSize = moduleContent.size();

                if (codeSize > 16)
                {
                    codeData += 16;
                    codeSize -= 16;
                }

                // 从 .pyc 文件加载代码对象
                PyObject* codeObj = PyMarshal_ReadObjectFromString(codeData, codeSize);
                if (codeObj)
                {
                    // 执行代码对象
                    result = PyEval_EvalCode(codeObj, moduleDict, moduleDict);
                    Py_DECREF(codeObj);
                }
            }
            else
            {
                // 处理源代码 .py 文件
                result = PyRun_StringFlags(moduleContent.data(), Py_file_input, moduleDict, moduleDict, nullptr);
            }

            if (result)
            {
                Py_DECREF(result);
            }
            else
            {
                // 如果执行失败，打印错误并回退到原始导入
                PyErr_Print();
                return PyObject_Call(original_import, args, kwargs);
            }

            // 处理 fromlist
            if (fromlist && PyList_Check(fromlist) && PyList_Size(fromlist) > 0)
            {
                // 返回模块本身
                Py_INCREF(module);
                return module;
            }
            else
            {
                // 处理点分模块名称，返回顶级模块
                const char* dot = strchr(moduleName, '.');
                if (dot)
                {
                    std::string topModuleName(moduleName, dot - moduleName);
                    PyObject* topModule = PyImport_AddModule(topModuleName.c_str());
                    Py_INCREF(topModule);
                    return topModule;
                }
                else
                {
                    Py_INCREF(module);
                    return module;
                }
            }
        }

        return PyObject_Call(original_import, args, kwargs);
    }

    bool installCustomImport()
    {
        PyObject* builtins = PyImport_ImportModule("builtins");
        if (!builtins)
        {
            //std::cerr << "无法导入 builtins 模块" << std::endl;
            return false;
        }

        original_import = PyObject_GetAttrString(builtins, "__import__");
        if (!original_import)
        {
            Py_DECREF(builtins);
            //std::cerr << "无法获取原始 __import__ 函数" << std::endl;
            return false;
        }

        PyMethodDef custom_import_def = {
            "__import__", (PyCFunction)custom_import, METH_VARARGS | METH_KEYWORDS, "自定义 __import__ 函数" };

        PyObject* new_import = PyCFunction_New(&custom_import_def, nullptr);
        if (!new_import)
        {
            Py_DECREF(original_import);
            Py_DECREF(builtins);
            //std::cerr << "无法创建自定义 __import__ 函数" << std::endl;
            return false;
        }

        if (PyObject_SetAttrString(builtins, "__import__", new_import) < 0)
        {
            Py_DECREF(new_import);
            Py_DECREF(original_import);
            Py_DECREF(builtins);
            //std::cerr << "无法替换 __import__ 函数" << std::endl;
            return false;
        }

        Py_DECREF(new_import);
        Py_DECREF(builtins);

        return true;
    }
}
class MinimalPythonEmbedder
{
private:
    bool initialized;
    PyThreadState* mainThreadState;
    std::unordered_set<std::string> availableModules;

public:
    MinimalPythonEmbedder()
        : initialized(false)
        , mainThreadState(nullptr)
    {
    }

    // 使用 PyConfig_InitIsolatedConfig 初始化 Python 解释器
    bool initialize()
    {
        if (initialized)
        {
            return true;
        }

        // 初始化 Python 预配置
        PyPreConfig preconfig;
        PyPreConfig_InitIsolatedConfig(&preconfig);

        // 可选：设置 UTF-8 模式
        preconfig.utf8_mode = 1;

        // 应用预配置
        PyStatus status = Py_PreInitialize(&preconfig);
        if (PyStatus_Exception(status))
        {
            //std::cerr << "Python 预初始化失败: " << status.err_msg << std::endl;
            //PyMem_RawFree(status.err_msg);
            return false;
        }

        // 初始化主配置
        PyConfig config;
        PyConfig_InitIsolatedConfig(&config);
        config.module_search_paths_set = 1;
        // 设置空路径，只使用内置和 frozen 模块
        status = PyConfig_SetWideStringList(&config, &config.module_search_paths, 0, NULL);
        if (PyStatus_Exception(status))
        {
            //std::cerr << "设置模块搜索路径标志失败: " << status.err_msg << std::endl;
            //PyMem_RawFree(status.err_msg);
            PyConfig_Clear(&config);
            return false;
        }

        // 禁用站点包导入
        config.site_import = 0;

        // 禁用用户站点目录
        config.user_site_directory = 0;

        // 禁用导入警告
        //config.warnings = 0;

        // 禁用 __pycache__ 目录
        config.write_bytecode = 0;

        // 可选：设置程序名称
        status = PyConfig_SetString(&config, &config.program_name, L"minimal_python");
        if (PyStatus_Exception(status))
        {
            //std::cerr << "设置程序名称失败: " << status.err_msg << std::endl;
            //PyMem_RawFree(status.err_msg);
            PyConfig_Clear(&config);
            return false;
        }

        // 应用配置并初始化 Python
        status = Py_InitializeFromConfig(&config);
        PyConfig_Clear(&config);

        if (PyStatus_Exception(status))
        {
            //std::cerr << "Python 初始化失败: " << status.err_msg << std::endl;
            //PyMem_RawFree(status.err_msg);
            return false;
        }

        // 缓存可用的模块
        cacheAvailableModules();

        // 保存主线程状态
        mainThreadState = PyEval_SaveThread();

        initialized = true;
        return true;
    }

    // 缓存所有可用的模块（内置和 frozen）
    void cacheAvailableModules()
    {
        PyGILState_STATE gstate = PyGILState_Ensure();

        // 获取 sys.builtin_module_names
        PyObject* sysModule = PyImport_ImportModule("sys");
        if (sysModule)
        {
            PyObject* builtinModuleNames = PyObject_GetAttrString(sysModule, "builtin_module_names");
            if (builtinModuleNames && PyTuple_Check(builtinModuleNames))
            {
                Py_ssize_t size = PyTuple_Size(builtinModuleNames);
                for (Py_ssize_t i = 0; i < size; i++)
                {
                    PyObject* name = PyTuple_GetItem(builtinModuleNames, i);
                    if (PyUnicode_Check(name))
                    {
                        availableModules.insert(PyUnicode_AsUTF8(name));
                    }
                }
            }
            Py_XDECREF(builtinModuleNames);

            // 检查 sys.path 是否为空
            PyObject* sysPath = PyObject_GetAttrString(sysModule, "path");
            if (sysPath && PyList_Check(sysPath))
            {
                //std::cout << "sys.path 长度: " << PyList_Size(sysPath) << std::endl;
                if (PyList_Size(sysPath) > 0)
                {
                    //std::cout << "警告: sys.path 不为空，可能会加载外部模块" << std::endl;
                    for (Py_ssize_t i = 0; i < PyList_Size(sysPath); i++)
                    {
                        PyObject* path = PyList_GetItem(sysPath, i);
                        if (PyUnicode_Check(path))
                        {
                            ;// std::cout << "  - " << PyUnicode_AsUTF8(path) << std::endl;
                        }
                    }
                }
            }
            Py_XDECREF(sysPath);

            Py_DECREF(sysModule);
        }

        // 获取 frozen 模块 - 使用 Python 3.13.2 中的新方法
        PyObject* importlibModule = PyImport_ImportModule("importlib._bootstrap");
        if (importlibModule)
        {
            // 在 Python 3.13.2 中，FrozenImporter 可能有所变化
            PyObject* frozenImporter = PyObject_GetAttrString(importlibModule, "FrozenImporter");
            if (frozenImporter)
            {
                PyObject* instance = PyObject_CallObject(frozenImporter, NULL);
                if (instance)
                {
                    // 尝试获取 frozen 模块名称列表
                    PyObject* moduleNames = PyObject_CallMethod(instance, "get_frozen_module_names", NULL);
                    if (!moduleNames || PyErr_Occurred())
                    {
                        PyErr_Clear();
                        // 备选方法：尝试使用 _FROZEN_IMPORT_NAMES
                        moduleNames = PyObject_GetAttrString(frozenImporter, "_FROZEN_IMPORT_NAMES");
                    }

                    if (moduleNames && PyList_Check(moduleNames))
                    {
                        Py_ssize_t size = PyList_Size(moduleNames);
                        for (Py_ssize_t i = 0; i < size; i++)
                        {
                            PyObject* name = PyList_GetItem(moduleNames, i);
                            if (PyUnicode_Check(name))
                            {
                                availableModules.insert(PyUnicode_AsUTF8(name));
                            }
                        }
                    }
                    Py_XDECREF(moduleNames);
                    Py_DECREF(instance);
                }
                Py_DECREF(frozenImporter);
            }
            else
            {
                PyErr_Clear();
                // 备选方法：尝试使用 _frozen_importlib_external
                PyObject* externalModule = PyImport_ImportModule("_frozen_importlib_external");
                if (externalModule)
                {
                    availableModules.insert("_frozen_importlib_external");
                    Py_DECREF(externalModule);
                }
                else
                {
                    PyErr_Clear();
                }
            }
            Py_DECREF(importlibModule);
        }
        else
        {
            PyErr_Clear();
        }

        // 添加一些核心模块，这些模块可能不在上述列表中但通常可用
        availableModules.insert("sys");
        availableModules.insert("builtins");
        availableModules.insert("_frozen_importlib");
        availableModules.insert("_frozen_importlib_external");
        availableModules.insert("_io");
        availableModules.insert("marshal");
        availableModules.insert("_imp");

        PyGILState_Release(gstate);
    }

    // 检查模块是否可用（内置或 frozen）
    bool isModuleAvailable(const std::string& moduleName)
    {
        // 首先检查缓存
        if (availableModules.find(moduleName) != availableModules.end())
        {
            return true;
        }

        // 尝试导入模块来检查可用性
        PyGILState_STATE gstate = PyGILState_Ensure();

        PyObject* module = PyImport_ImportModule(moduleName.c_str());
        bool available = (module != nullptr);

        if (module)
        {
            // 如果模块可导入，添加到缓存
            availableModules.insert(moduleName);
            Py_DECREF(module);
        }
        else
        {
            PyErr_Clear(); // 清除导入错误
        }

        PyGILState_Release(gstate);
        return available;
    }

    // 获取所有可用模块的列表
    std::vector<std::string> getAvailableModules()
    {
        std::vector<std::string> result;
        for (const auto& module : availableModules)
        {
            result.push_back(module);
        }
        return result;
    }

    // 添加自定义模块加载器
    bool addCustomModuleLoader(const std::unordered_map<std::string, std::string>& moduleContents)
    {
        PyGILState_STATE gstate = PyGILState_Ensure();

        bool success = false;

        // 创建一个字典来存储模块内容
        PyObject* moduleDict = PyDict_New();
        if (!moduleDict)
        {
            PyGILState_Release(gstate);
            return false;
        }

        // 将模块内容添加到字典中
        for (const auto& entry : moduleContents)
        {
            PyObject* key = PyUnicode_FromString(entry.first.c_str());
            PyObject* value = PyUnicode_FromString(entry.second.c_str());
            if (key && value)
            {
                PyDict_SetItem(moduleDict, key, value);
            }
            Py_XDECREF(key);
            Py_XDECREF(value);
        }

        // 运行自定义导入器代码 - 针对 Python 3.13.2 优化
        const char* importerCode = R"(
import sys
import types
import importlib.abc
import importlib.machinery

class MemoryLoader(importlib.abc.MetaPathFinder, importlib.abc.Loader):
    def __init__(self, module_contents):
        self.module_contents = module_contents
    
    def find_spec(self, fullname, path, target=None):
        if fullname in self.module_contents:
            return importlib.machinery.ModuleSpec(fullname, self, is_package=False)
        return None
    
    def create_module(self, spec):
        return None  # 使用默认创建的模块
    
    def exec_module(self, module):
        fullname = module.__name__
        if fullname in self.module_contents:
            code = compile(self.module_contents[fullname], fullname, 'exec')
            exec(code, module.__dict__)

# 获取传入的模块内容字典
module_contents = {}
for key, value in _memory_modules.items():
    module_contents[key] = value

# 创建并注册导入器
memory_loader = MemoryLoader(module_contents)
sys.meta_path.insert(0, memory_loader)
    )";

        // 设置全局变量 _memory_modules
        PyObject* mainModule = PyImport_AddModule("__main__");
        if (mainModule)
        {
            PyObject* mainDict = PyModule_GetDict(mainModule);
            PyDict_SetItemString(mainDict, "_memory_modules", moduleDict);

            // 执行导入器代码
            PyObject* result = PyRun_String(importerCode, Py_file_input, mainDict, mainDict);
            if (result)
            {
                success = true;
                Py_DECREF(result);
            }
            else
            {
                PyErr_Print();
            }
        }

        Py_DECREF(moduleDict);
        PyGILState_Release(gstate);
        return success;
    }

    // 执行 Python 代码
    bool runSimpleString(const std::string& code)
    {
        PyGILState_STATE gstate = PyGILState_Ensure();

        int result = PyRun_SimpleString(code.c_str());

        PyGILState_Release(gstate);
        return (result == 0);
    }

    void finalize()
    {
        if (!initialized)
        {
            return;
        }

        // 恢复主线程状态
        PyEval_RestoreThread(mainThreadState);
        mainThreadState = nullptr;

        // 清理 Python 解释器
        Py_Finalize();

        initialized = false;
        availableModules.clear();
    }

    ~MinimalPythonEmbedder() { finalize(); }
};
/*
// 示例使用
int main()
{
    MinimalPythonEmbedder pyEmbed;

    // 初始化 Python 环境，只使用内置模块和 frozen 模块
    if (!pyEmbed.initialize())
    {
        std::cerr << "Python 环境初始化失败" << std::endl;
        return 1;
    }

    // 列出所有可用模块
    std::cout << "可用的模块:" << std::endl;
    std::vector<std::string> availableModules = pyEmbed.getAvailableModules();
    for (const auto& moduleName : availableModules)
    {
        std::cout << "- " << moduleName << std::endl;
    }

    // 测试一些常见模块是否可用
    std::vector<std::string> modulesToTest = {"sys", "builtins", "_frozen_importlib", "_frozen_importlib_external",
        "os", "json", "re", "math", "datetime", "_io", "marshal", "_imp"};

    std::cout << "\n模块可用性测试:" << std::endl;
    for (const auto& moduleName : modulesToTest)
    {
        bool isAvailable = pyEmbed.isModuleAvailable(moduleName);
        std::cout << "- " << moduleName << ": " << (isAvailable ? "可用" : "不可用") << std::endl;
    }

    // 添加一些自定义模块
    std::unordered_map<std::string, std::string> customModules = {{"custom_module", R"(
def hello():
    return "Hello from custom module!"

def test():
    import sys
    return f"Python version: {sys.version}"
)"}};

    if (pyEmbed.addCustomModuleLoader(customModules))
    {
        std::cout << "\n添加自定义模块加载器成功" << std::endl;

        // 测试自定义模块
        pyEmbed.runSimpleString(
            "try:\n"
            "    import custom_module\n"
            "    print('自定义模块测试:')\n"
            "    print(custom_module.hello())\n"
            "    print(custom_module.test())\n"
            "except Exception as e:\n"
            "    print('导入自定义模块出错:', e)\n");
    }

    // 尝试执行一些只使用内置功能的 Python 代码
    std::cout << "\n执行 Python 代码:" << std::endl;
    pyEmbed.runSimpleString(
        "import sys\n"
        "print('Python 版本:', sys.version)\n"
        "print('内置模块:', sys.builtin_module_names)\n"
        "print('sys.path:', sys.path)\n");

    return 0;
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
    //config._init_main = 0;
    config.site_import = 0;
    config.user_site_directory = 0;
    // 禁用导入警告
    //config.warnings = 0;

    // 禁用 __pycache__ 目录
    //config.write_bytecode = 0;
//     config.use_frozen_modules = 1;
//     config._install_importlib = 1;
    std::vector<std::wstring> moduleSearchPaths = {
        //L"",
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
//    cacheAvailableModules();
//    initialize_python_without_frozen();
//     if (!installCustomImport()) {
//         return false;
//     }
//     status = _Py_InitializeMain();
//     if (PyStatus_Exception(status))
//         return false;
// 
//     if (!Py_IsInitialized()) {
//         return false;
//     }

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
