#pragma once

#include "../Core/Context.h"
#include "../Core/Object.h"
#include "pytypedefs.h"
namespace Urho3D
{

class Scene;

/// Python script subsystem.
class URHO3D_API PythonScript : public Object
{
    URHO3D_OBJECT(PythonScript, Object);

public:
    /// Construct.
    explicit PythonScript(Context* context);
    /// Destruct.
    ~PythonScript() override;
    bool Initialize(const std::string& pythonHome = "");
    bool AddModulePath(const std::string& path);
    PyObject* CallFunction(const std::string& moduleName, const std::string& funcName, PyObject* args = nullptr);
    PyObject* RunSimpleString(const std::string& code);
    void Finalize();
    /// Execute script file. Return true if successful.
    bool ExecuteFile(const ea::string& fileName, bool fromui = false);
    /// Execute script string. Return true if successful.
    bool ExecuteString(const ea::string& string);
    /// Load script file on file system (i.e. not from resource cache). Return true if successful.
    bool LoadRawFile(const ea::string& fileName);
    /// Load and execute script file on file system (i.e. not from resource cache). Return true if successful.
    bool ExecuteRawFile(const ea::string& fileName);
private:
    bool initialized_{ false };
    PyThreadState* main_thread_state_{ nullptr };
};

/// Register Python script library objects.
void URHO3D_API RegisterPythonScriptLibrary(Context* context);

bool URHO3D_API RunPython(Context* context, const ea::string& scriptFileName);

}
