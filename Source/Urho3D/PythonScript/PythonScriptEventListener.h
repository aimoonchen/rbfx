#pragma once

#include "../Core/Object.h"

namespace Urho3D
{

/// Python script event listener.
class URHO3D_API PythonScriptEventListener
{
public:
    /// Destruct.
    virtual ~PythonScriptEventListener() = default;
    virtual void AddEventHandler(const ea::string& eventName, nb::callable function) = 0;
    /// Add a scripted event handler by function at the given stack index.
    virtual void AddEventHandler(const ea::string& eventName, int index) = 0;
    /// Add a scripted event handler by function name.
    virtual void AddEventHandler(const ea::string& eventName, const ea::string& functionName) = 0;
    virtual void AddEventHandler(Object* sender, const ea::string& eventName, nb::callable function) = 0;
    /// Add a scripted event handler by function at the given stack index for a specific sender.
    virtual void AddEventHandler(Object* sender, const ea::string& eventName, int index) = 0;
    /// Add a scripted event handler by function name for a specific sender.
    virtual void AddEventHandler(Object* sender, const ea::string& eventName, const ea::string& functionName) = 0;
    /// Remove a scripted event handler.
    virtual void RemoveEventHandler(const ea::string& eventName) = 0;
    /// Remove a scripted event handler for a specific sender.
    virtual void RemoveEventHandler(Object* sender, const ea::string& eventName) = 0;
    /// Remove all scripted event handlers for a specific sender.
    virtual void RemoveEventHandlers(Object* sender) = 0;
    /// Remove all scripted event handlers.
    virtual void RemoveAllEventHandlers() = 0;
    /// Remove all scripted event handlers, except those listed.
    virtual void RemoveEventHandlersExcept(const ea::vector<ea::string>& exceptionNames) = 0;
    /// Return whether has subscribed to an event.
    virtual bool HasEventHandler(const ea::string& eventName) const = 0;
    /// Return whether has subscribed to a specific sender's event.
    virtual bool HasEventHandler(Object* sender, const ea::string& eventName) const = 0;
};

}
