#pragma once

#include "../Core/Context.h"
#include "../Core/Object.h"

namespace Urho3D
{

/// Python script subsystem.
class URHO3D_API PythonScript : public Object
{
    URHO3D_OBJECT(PythonScript, Object);

public:
    /// Construct.
    explicit PythonScript(Context* context);
    /// Destruct.
    ~PythonScript() override;
//     void AddEventHandler(const ea::string& eventName, void* function) override;
//     /// Add a scripted event handler by function at the given stack index.
//     void AddEventHandler(const ea::string& eventName, int index) override;
//     /// Add a scripted event handler by function name.
//     void AddEventHandler(const ea::string& eventName, const ea::string& functionName) override;
//     void AddEventHandler(Object* sender, const ea::string& eventName, void* function) override;
//     /// Add a scripted event handler by function at the given stack index for a specific sender.
//     void AddEventHandler(Object* sender, const ea::string& eventName, int index) override;
//     /// Add a scripted event handler by function name for a specific sender.
//     void AddEventHandler(Object* sender, const ea::string& eventName, const ea::string& functionName) override;
//     /// Remove a scripted event handler.
//     void RemoveEventHandler(const ea::string& eventName) override;
//     /// Remove a scripted event handler for a specific sender.
//     void RemoveEventHandler(Object* sender, const ea::string& eventName) override;
//     /// Remove all scripted event handlers for a specific sender.
//     void RemoveEventHandlers(Object* sender) override;
//     /// Remove all scripted event handlers.
//     void RemoveAllEventHandlers() override;
//     /// Remove all scripted event handlers, except those listed.
//     void RemoveEventHandlersExcept(const ea::vector<ea::string>& exceptionNames) override;
//     /// Return whether has subscribed to an event.
//     bool HasEventHandler(const ea::string& eventName) const override;
//     /// Return whether has subscribed to a specific sender's event.
//     bool HasEventHandler(Object* sender, const ea::string& eventName) const override;

    /// Execute script file. Return true if successful.
    bool ExecuteFile(const ea::string& fileName, bool fromui = false);
    /// Execute script string. Return true if successful.
    bool ExecuteString(const ea::string& string);
    /// Load script file on file system (i.e. not from resource cache). Return true if successful.
    bool LoadRawFile(const ea::string& fileName);
    /// Load and execute script file on file system (i.e. not from resource cache). Return true if successful.
    bool ExecuteRawFile(const ea::string& fileName);
    /// Execute script function.
//     sol::protected_function_result ExecuteFunction(const ea::string& functionName);
//     /// Set whether to execute engine console commands as script code.
//     void SetExecuteConsoleCommands(bool enable);
// 
//     /// Return Lua state.
//     sol::state* GetState() const { return luaState_.get(); }
// 
//     sol::function* GetFunction(const ea::string& functionName, bool silentIfNotFound = false);
//     /// Return whether is executing engine console commands as script code.
//     bool GetExecuteConsoleCommands() const { return executeConsoleCommands_; }
// 
//     /// Push Lua function to stack. Return true if is successful. Return false on any error and an error string is pushed instead.
//     static bool PushLuaFunction(lua_State* L, const ea::string& functionName);

private:
};

/// Register Lua script library objects.
void URHO3D_API RegisterPythonScriptLibrary(Context* context);

bool URHO3D_API RunPython(Context* context, const ea::string& scriptFileName);

}
