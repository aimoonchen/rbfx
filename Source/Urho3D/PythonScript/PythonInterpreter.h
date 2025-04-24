#pragma once
#include "EASTL/string.h"
namespace Urho3D
{
class Context;
class PythonInterpreter
{
public:
    PythonInterpreter();
    ~PythonInterpreter();
    bool Initialize();
    void RegisterModule(const char* name, PyObject* cmodule);
    void Finalize();
    int RunSimpleString(const ea::string& code);
    PyObject* RunString(const ea::string& code);
    PyObject* CallFunction(const ea::string& moduleName, const ea::string& funcName, PyObject* args);
    bool ExecuteFile(const ea::string& fileName);
    bool ExecuteString(const ea::string& string);
    bool LoadRawFile(const ea::string& fileName);
    bool ExecuteRawFile(const ea::string& fileName);
    bool ExecuteFunction(const ea::string& functionName);
private:
    bool initialized_{ false };
    PyObject* module_dict_{ nullptr };
    PyThreadState* main_thread_state_{ nullptr };
};
}
