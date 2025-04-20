#pragma once

#include "../PythonScript/PythonScriptEventListener.h"
#include "../Scene/Component.h"

namespace Urho3D
{
class PythonFile;
class PythonScript;
class PythonScriptEventInvoker;

/// Python Script object methods.
enum PythonScriptObjectMethod
{
    LSOM_START = 0,
    LSOM_STOP,
    LSOM_DELAYEDSTART,
    LSOM_UPDATE,
    LSOM_POSTUPDATE,
    LSOM_FIXEDUPDATE,
    LSOM_FIXEDPOSTUPDATE,
    LSOM_LOAD,
    LSOM_SAVE,
    LSOM_READNETWORKUPDATE,
    LSOM_WRITENETWORKUPDATE,
    LSOM_APPLYATTRIBUTES,
    LSOM_TRANSFORMCHANGED,
    MAX_LUA_SCRIPT_OBJECT_METHODS
};

/// Python script object component.
class URHO3D_API PythonScriptInstance : public Component, public PythonScriptEventListener
{
    URHO3D_OBJECT(PythonScriptInstance, Component);

public:
    /// Construct.
    explicit PythonScriptInstance(Context* context);
    /// Destruct.
    ~PythonScriptInstance() override;
    /// Register object factory.
    /// @nobind
    static void RegisterObject(Context* context);

    /// Handle attribute write access.
    void OnSetAttribute(const AttributeInfo& attr, const Variant& src) override;
    /// Handle attribute read access.
    void OnGetAttribute(const AttributeInfo& attr, Variant& dest) const override;

    /// Return attribute descriptions, or null if none defined.
    const ea::vector<AttributeInfo>* GetAttributes() const override { return &attributeInfos_; }

    /// Apply attribute changes that can not be applied immediately. Called after scene load or a network update.
    void ApplyAttributes() override;
    /// Handle enabled/disabled state change.
    void OnSetEnabled() override;

    void AddEventHandler(const ea::string& eventName, nanobind::callable function) override;
    /// Add a scripted event handler by function.
    void AddEventHandler(const ea::string& eventName, int functionIndex) override;
    /// Add a scripted event handler by function name.
    void AddEventHandler(const ea::string& eventName, const ea::string& functionName) override;
    void AddEventHandler(Object* sender, const ea::string& eventName, nanobind::callable function) override;
    /// Add a scripted event handler by function for a specific sender.
    void AddEventHandler(Object* sender, const ea::string& eventName, int functionIndex) override;
    /// Add a scripted event handler by function name for a specific sender.
    void AddEventHandler(Object* sender, const ea::string& eventName, const ea::string& functionName) override;
    /// Remove a scripted event handler.
    void RemoveEventHandler(const ea::string& eventName) override;
    /// Remove a scripted event handler for a specific sender.
    void RemoveEventHandler(Object* sender, const ea::string& eventName) override;
    /// Remove all scripted event handlers for a specific sender.
    void RemoveEventHandlers(Object* sender) override;
    /// Remove all scripted event handlers.
    void RemoveAllEventHandlers() override;
    /// Remove all scripted event handlers, except those listed.
    void RemoveEventHandlersExcept(const ea::vector<ea::string>& exceptionNames) override;
    /// Return whether has subscribed to an event.
    bool HasEventHandler(const ea::string& eventName) const override;
    /// Return whether has subscribed to a specific sender's event.
    bool HasEventHandler(Object* sender, const ea::string& eventName) const override;

    /// Create script object. Return true if successful.
    bool CreateObject(const ea::string& scriptObjectType);
    /// Create script object. Return true if successful.
    bool CreateObject(PythonFile* scriptFile, const ea::string& scriptObjectType);
    /// Set script file.
    void SetScriptFile(PythonFile* scriptFile);
    /// Set script object type.
    void SetScriptObjectType(const ea::string& scriptObjectType);
    /// Set script file serialization attribute by calling a script function.
    void SetScriptDataAttr(const ea::vector<unsigned char>& data);
    /// Set script network serialization attribute by calling a script function.
    void SetScriptNetworkDataAttr(const ea::vector<unsigned char>& data);

    /// Return script file.
    PythonFile* GetScriptFile() const;

    /// Return script object type.
    const ea::string& GetScriptObjectType() const { return scriptObjectType_; }

    /// Return Python reference to script object.
    //int GetScriptObjectRef() const { return scriptObjectRef_; }

    /// Get script file serialization attribute by calling a script function.
    ea::vector<unsigned char> GetScriptDataAttr() const;
    /// Get script network serialization attribute by calling a script function.
    ea::vector<unsigned char> GetScriptNetworkDataAttr() const;
    /// Return script object's funcition.
    nanobind::callable GetScriptObjectFunction(const ea::string& functionName) const;

    /// Set script file attribute.
    void SetScriptFileAttr(const ResourceRef& value);
    /// Return script file attribute.
    ResourceRef GetScriptFileAttr() const;

    nanobind::object GetScriptObject();
    void SetScriptObject(nanobind::object obj);

protected:
    /// Handle scene being assigned.
    void OnSceneSet(Scene* previousScene, Scene* scene) override;
    /// Handle node transform being dirtied.
    void OnMarkedDirty(Node* node) override;

private:
    /// Find script object attributes.
    void GetScriptAttributes();
    /// Find script object method refs.
    void FindScriptObjectMethodRefs();
    /// Subscribe to script method events.
    void SubscribeToScriptMethodEvents();
    /// Unsubscribe from script method events.
    void UnsubscribeFromScriptMethodEvents();
    /// Handle the logic update event.
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle the logic post update event.
    void HandlePostUpdate(StringHash eventType, VariantMap& eventData);
#if defined(URHO3D_PHYSICS) || defined(URHO3D_URHO2D)
    /// Handle the physics update event.
    void HandleFixedUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle the physics post update event.
    void HandlePostFixedUpdate(StringHash eventType, VariantMap& eventData);
#endif
    /// Release the script object.
    void ReleaseObject();

    /// Python Script subsystem.
    PythonScript* pythonScript_{};
    /// Event invoker.
    SharedPtr<PythonScriptEventInvoker> eventInvoker_;
    /// Script file.
    SharedPtr<PythonFile> scriptFile_;
    /// Script object type.
    ea::string scriptObjectType_;
    /// Attributes, including script object variables.
    ea::vector<AttributeInfo> attributeInfos_;
    /// Python reference to script object.
    //int scriptObjectRef_{};
    //sol::table scriptObjectRef_{ sol::lua_nil };
    nanobind::object scriptObjectRef_;
    /// Script object method.
    nanobind::callable scriptObjectMethods_[MAX_LUA_SCRIPT_OBJECT_METHODS];

    /// TODO: remvoe this : just keep alive
    std::vector<nanobind::callable> python_functions_;
};

}
