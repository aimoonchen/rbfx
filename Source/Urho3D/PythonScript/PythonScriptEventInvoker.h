#pragma once

#include "Urho3D/Core/Object.h"
#include <vector>
namespace Urho3D
{

class PythonScriptInstance;

/// Python script event invoker.
class PythonScriptEventInvoker : public Object
{
    URHO3D_OBJECT(PythonScriptEventInvoker, Object);

public:
    /// Construct.
    explicit PythonScriptEventInvoker(Context* context);
    /// Construct from PythoScriptInstance.
    explicit PythonScriptEventInvoker(PythonScriptInstance* instance);
    /// Destruct.
    ~PythonScriptEventInvoker() override;

    /// Add a scripted event handler.
    void AddEventHandler(Object* sender, const StringHash& eventType, nanobind::callable function);
private:
    /// Handle script event in Python script.
    void HandlePythonScriptEvent(StringHash eventType, VariantMap& eventData);

    /// Python script instance.
    WeakPtr<PythonScriptInstance> instance_;

    /// TODO: remvoe this : just keep alive
    std::vector<nanobind::object> python_functions_;
};

}
