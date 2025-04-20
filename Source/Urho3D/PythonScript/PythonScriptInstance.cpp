#include "../Precompiled.h"
#include <nanobind/nanobind.h>
#include "../Core/CoreEvents.h"
#include "../Core/Context.h"
#include "../Core/ProcessUtils.h"
#include "../IO/Log.h"
#include "../IO/MemoryBuffer.h"
#include "../PythonScript/PythonFile.h"
// #include "../LuaScript/LuaFunction.h"
#include "../PythonScript/PythonScript.h"
#include "../PythonScript/PythonScriptEventInvoker.h"
#if defined(URHO3D_PHYSICS) || defined(URHO3D_URHO2D)
#include "../Physics/PhysicsEvents.h"
#endif
#include "../Resource/ResourceCache.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneEvents.h"

#include "../PythonScript/PythonScriptInstance.h"
#include "../DebugNew.h"

namespace nb = nanobind;

namespace Urho3D
{

static const char* scriptObjectMethodNames[] = {
    "Start",
    "Stop",
    "DelayedStart",
    "Update",
    "PostUpdate",
    "FixedUpdate",
    "FixedPostUpdate",
    "Load",
    "Save",
    "ReadNetworkUpdate",
    "WriteNetworkUpdate",
    "ApplyAttributes",
    "TransformChanged"
};

PythonScriptInstance::PythonScriptInstance(Context* context) :
    Component(context),
    pythonScript_(GetSubsystem<PythonScript>()),
    eventInvoker_(new PythonScriptEventInvoker(this))
{
    attributeInfos_ = *context_->GetAttributes(GetTypeStatic());
}

PythonScriptInstance::~PythonScriptInstance()
{
    ReleaseObject();
}

void PythonScriptInstance::RegisterObject(Context* context)
{
    context->RegisterFactory<PythonScriptInstance>(Category_Logic);

    URHO3D_ACCESSOR_ATTRIBUTE("Is Enabled", IsEnabled, SetEnabled, bool, true, AM_DEFAULT);
//     URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Script File", GetScriptFileAttr, SetScriptFileAttr, ResourceRef,
//         ResourceRef(LuaFile::GetTypeStatic()), AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Script Object Type", GetScriptObjectType, SetScriptObjectType, ea::string, EMPTY_STRING, AM_DEFAULT);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Script Data", GetScriptDataAttr, SetScriptDataAttr, ea::vector<unsigned char>, Variant::emptyBuffer,
        AM_FILE | AM_NOEDIT);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Script Network Data", GetScriptNetworkDataAttr, SetScriptNetworkDataAttr, ea::vector<unsigned char>,
        Variant::emptyBuffer, AM_NOEDIT);
}

void PythonScriptInstance::OnSetAttribute(const AttributeInfo& attr, const Variant& src)
{
//     if (attr.ptr_ != (void*)0xffffffffffffffff)
//     {
//         Serializable::OnSetAttribute(attr, src);
//         return;
//     }
// 
//     if (scriptObjectRef_ == sol::lua_nil)
//         return;
// 
//     ea::string name = attr.name_;
//     unsigned length = name.length();
//     if (name.back() == '_')
//         length -= 1;
// 
//     int top = lua_gettop(luaState_);
// 
//     ea::string functionName = ea::string("Set") + name.substr(0, 1).to_upper() + name.substr(1, length - 1);
//     auto function = GetScriptObjectFunction(functionName);
//     // If set function exist
//     if (function)
//     {
// //         if (function->BeginCall(this))
// //         {
// //             function->PushVariant(src);
// //             function->EndCall();
// //         }
//         CallPythonFunction(function, scriptObjectRef_, src);
//     }
//     else
//     {
// //         lua_rawgeti(luaState_, LUA_REGISTRYINDEX, scriptObjectRef_);
// //         lua_pushstring(luaState_, name.CString());
// 
//         switch (attr.type_)
//         {
//         case VAR_BOOL:
//             lua_pushboolean(luaState_, src.GetBool());
//             break;
//         case VAR_DOUBLE:
//             lua_pushnumber(luaState_, src.GetDouble());
//             break;
//         case VAR_STRING:
//             tolua_pushurho3dstring(luaState_, src.GetString());
//             break;
// //         case VAR_VECTOR2:
// //             {
// //                 auto* value = new Vector2(src.GetVector2());
// //                 tolua_pushusertype(luaState_, value, "Vector2");
// //                 tolua_register_gc(luaState_, lua_gettop(luaState_));
// //             }
// //             break;
// //         case VAR_VECTOR3:
// //             {
// //                 auto* value = new Vector3(src.GetVector3());
// //                 tolua_pushusertype(luaState_, value, "Vector3");
// //                 tolua_register_gc(luaState_, lua_gettop(luaState_));
// //             }
// //             break;
// //         case VAR_VECTOR4:
// //             {
// //                 auto* value = new Vector4(src.GetVector4());
// //                 tolua_pushusertype(luaState_, value, "Vector4");
// //                 tolua_register_gc(luaState_, lua_gettop(luaState_));
// //             }
// //             break;
// //         case VAR_QUATERNION:
// //             {
// //                 auto* value = new Quaternion(src.GetQuaternion());
// //                 tolua_pushusertype(luaState_, value, "Quaternion");
// //                 tolua_register_gc(luaState_, lua_gettop(luaState_));
// //             }
// //             break;
// //         case VAR_COLOR:
// //             {
// //                 auto* value = new Color(src.GetColor());
// //                 tolua_pushusertype(luaState_, value, "Color");
// //                 tolua_register_gc(luaState_, lua_gettop(luaState_));
// //             }
// //             break;
// //         case VAR_INTRECT:
// //             {
// //                 auto* value = new IntRect(src.GetIntRect());
// //                 tolua_pushusertype(luaState_, value, "IntRect");
// //                 tolua_register_gc(luaState_, lua_gettop(luaState_));
// //             }
// //             break;
// //         case VAR_INTVECTOR2:
// //             {
// //                 auto* value = new IntVector2(src.GetIntVector2());
// //                 tolua_pushusertype(luaState_, value, "IntVector2");
// //                 tolua_register_gc(luaState_, lua_gettop(luaState_));
// //             }
// //             break;
// //         case VAR_INTVECTOR3:
// //             {
// //                 auto* value = new IntVector3(src.GetIntVector3());
// //                 tolua_pushusertype(luaState_, value, "IntVector3");
// //                 tolua_register_gc(luaState_, lua_gettop(luaState_));
// //             }
// //             break;
//         default:
//             URHO3D_LOGERROR("Unsupported data type");
//             lua_settop(luaState_, top);
//             return;
//         }
// 
//         lua_settable(luaState_, -3);
//     }
// 
//     lua_settop(luaState_, top);
}

void PythonScriptInstance::OnGetAttribute(const AttributeInfo& attr, Variant& dest) const
{
//     if (attr.ptr_ != (void*)0xffffffffffffffff)
//     {
//         Serializable::OnGetAttribute(attr, dest);
//         return;
//     }
// 
//     if (scriptObjectRef_ == sol::lua_nil)
//         return;
// 
//     ea::string name = attr.name_;
//     unsigned length = name.length();
//     if (name.back() == '_')
//         length -= 1;
// 
//     int top = lua_gettop(luaState_);
// 
//     ea::string functionName = ea::string("Get") + name.substr(0, 1).to_upper() + name.substr(1, length - 1);
//     auto function = GetScriptObjectFunction(functionName);
//     // If get function exist
//     if (function)
//     {
// //         if (function->BeginCall(this))
// //             function->EndCall(1);
// //        (*function)();
//         CallPythonFunction(function, scriptObjectRef_);
//     }
// //     else
// //     {
// //         lua_rawgeti(luaState_, LUA_REGISTRYINDEX, scriptObjectRef_);
// //         lua_pushstring(luaState_, name.CString());
// //         lua_gettable(luaState_, -2);
// //     }
// 
//     switch (attr.type_)
//     {
//     case VAR_BOOL:
//         dest = lua_toboolean(luaState_, -1) != 0;
//         break;
//     case VAR_DOUBLE:
//         dest = lua_tonumber(luaState_, -1);
//         break;
// //     case VAR_STRING:
// //         dest = tolua_tourho3dstring(luaState_, -1, "");
// //         break;
// //     case VAR_VECTOR2:
// //         dest = *((Vector2*)tolua_tousertype(luaState_, -1, nullptr));
// //         break;
// //     case VAR_VECTOR3:
// //         dest = *((Vector3*)tolua_tousertype(luaState_, -1, nullptr));
// //         break;
// //     case VAR_VECTOR4:
// //         dest = *((Vector4*)tolua_tousertype(luaState_, -1, nullptr));
// //         break;
// //     case VAR_QUATERNION:
// //         dest = *((Quaternion*)tolua_tousertype(luaState_, -1, nullptr));
// //         break;
// //     case VAR_COLOR:
// //         dest = *((Color*)tolua_tousertype(luaState_, -1, nullptr));
// //         break;
// //     case VAR_INTRECT:
// //         dest = *((IntRect*)tolua_tousertype(luaState_, -1, nullptr));
// //         break;
// //     case VAR_INTVECTOR2:
// //         dest = *((IntVector2*)tolua_tousertype(luaState_, -1, nullptr));
// //         break;
// //     case VAR_INTVECTOR3:
// //         dest = *((IntVector3*)tolua_tousertype(luaState_, -1, nullptr));
// //         break;
//     default:
//         URHO3D_LOGERROR("Unsupported data type");
//         return;
//     }
// 
// //    lua_settop(luaState_, top);
}

void PythonScriptInstance::ApplyAttributes()
{
    auto& function = scriptObjectMethods_[LSOM_APPLYATTRIBUTES];
//     if (function && function->BeginCall(this))
//         function->EndCall();
    if (function) {
        //(*function)();
        CallPythonFunction(function, scriptObjectRef_);
    }
}

void PythonScriptInstance::OnSetEnabled()
{
    if (IsEnabledEffective())
        SubscribeToScriptMethodEvents();
    else
        UnsubscribeFromScriptMethodEvents();
}

void PythonScriptInstance::AddEventHandler(const ea::string& eventName, nb::callable function)
{
    eventInvoker_->AddEventHandler(nullptr, eventName, function);
    python_functions_.emplace_back(function);
}

void PythonScriptInstance::AddEventHandler(Object* sender, const ea::string& eventName, nb::callable function)
{
    if (!sender)
        return;
    eventInvoker_->AddEventHandler(sender, eventName, function);
    python_functions_.emplace_back(function);
}

void PythonScriptInstance::AddEventHandler(const ea::string& eventName, int functionIndex)
{
    assert(false);
//     LuaFunction* function = luaScript_->GetFunction(functionIndex);
//     if (function)
//        eventInvoker_->AddEventHandler(nullptr, eventName, function);
}

void PythonScriptInstance::AddEventHandler(const ea::string& eventName, const ea::string& functionName)
{
    ea::string realFunctionName = functionName.replaced(":", ".");
    auto function = pythonScript_->GetFunction(realFunctionName);
    if (function)
        eventInvoker_->AddEventHandler(nullptr, eventName, function);
}

void PythonScriptInstance::AddEventHandler(Object* sender, const ea::string& eventName, int functionIndex)
{
    assert(false);
//     if (!sender)
//         return;
// 
//     LuaFunction* function = luaScript_->GetFunction(functionIndex);
//     if (function)
//         eventInvoker_->AddEventHandler(sender, eventName, function);
}

void PythonScriptInstance::AddEventHandler(Object* sender, const ea::string& eventName, const ea::string& functionName)
{
    if (!sender)
        return;

    ea::string realFunctionName = functionName.replaced(":", ".");
    auto function = pythonScript_->GetFunction(realFunctionName);
    if (function)
        eventInvoker_->AddEventHandler(sender, eventName, function);
}

void PythonScriptInstance::RemoveEventHandler(const ea::string& eventName)
{
    eventInvoker_->UnsubscribeFromEvent(eventName);
}

void PythonScriptInstance::RemoveEventHandler(Object* sender, const ea::string& eventName)
{
    if (!sender)
        return;

    eventInvoker_->UnsubscribeFromEvent(sender, eventName);
}

void PythonScriptInstance::RemoveEventHandlers(Object* sender)
{
    if (!sender)
        return;

    eventInvoker_->UnsubscribeFromEvents(sender);
}

void PythonScriptInstance::RemoveAllEventHandlers()
{
    eventInvoker_->UnsubscribeFromAllEvents();
}

void PythonScriptInstance::RemoveEventHandlersExcept(const ea::vector<ea::string>& exceptionNames)
{
    ea::vector<StringHash> exceptionTypes(exceptionNames.size());
    for (unsigned i = 0; i < exceptionTypes.size(); ++i)
        exceptionTypes[i] = StringHash(exceptionNames[i]);

    eventInvoker_->UnsubscribeFromAllEventsExcept(exceptionTypes);
}

bool PythonScriptInstance::HasEventHandler(const ea::string& eventName) const
{
    return eventInvoker_->HasSubscribedToEvent(eventName);
}

bool PythonScriptInstance::HasEventHandler(Object* sender, const ea::string& eventName) const
{
    return eventInvoker_->HasSubscribedToEvent(sender, eventName);
}

bool PythonScriptInstance::CreateObject(const ea::string& scriptObjectType)
{
//     SetScriptFile(nullptr);
//     SetScriptObjectType(scriptObjectType);
//     return scriptObjectRef_ != sol::lua_nil;
    return false;
}

bool PythonScriptInstance::CreateObject(PythonFile* scriptFile, const ea::string& scriptObjectType)
{
//     SetScriptFile(scriptFile);
//     SetScriptObjectType(scriptObjectType);
//     return scriptObjectRef_ != sol::lua_nil;
    return false;
}

void PythonScriptInstance::SetScriptFile(PythonFile* scriptFile)
{
//     if (scriptFile == scriptFile_)
//         return;
// 
//     scriptFile_ = scriptFile;
// 
//     if (!scriptFile_)
//         return;
// 
//     if (!scriptFile_->LoadAndExecute(luaState_))
//         URHO3D_LOGERROR("Execute Lua file failed: " + scriptFile_->GetName());
}

void PythonScriptInstance::SetScriptObjectType(const ea::string& scriptObjectType)
{
    if (scriptObjectType == scriptObjectType_)
        return;

    ReleaseObject();
    /*
    auto function = pythonScript_->GetFunction("CreateScriptObjectInstance");
    if (!function)
        return;
    auto sol_lua = sol::state_view(luaState_);
    sol::table scriptClass = sol_lua[scriptObjectType.c_str()];
    sol::protected_function_result result = CallPythonFunction(function, scriptClass, this);
    if (!result.valid()) {
        return;
    }
    //     function->PushLuaTable(scriptObjectType);
//     function->PushUserType((void*)this, "PythonScriptInstance");
// 
//     // Return script object and attribute names
//     if (!function->EndCall(2))
//         return;
    sol::table scriptObject = result.get<sol::table>();
    if (result.return_count() > 1) {
        sol::table attrNames = result.get<sol::table>(1);
    }
    //GetScriptAttributes();
    scriptObjectType_ = scriptObjectType;
    scriptObjectRef_ = scriptObject;
    //scriptObjectRef_ = luaL_ref(luaState_, LUA_REGISTRYINDEX);
    */
    // Find script object method refs
    FindScriptObjectMethodRefs();
}

void PythonScriptInstance::SetScriptDataAttr(const ea::vector<unsigned char>& data)
{
    if (!scriptObjectRef_)
        return;

    auto function = scriptObjectMethods_[LSOM_LOAD];
    if (function/* && function->BeginCall(this)*/)
    {
        MemoryBuffer buf(data);
//         function->PushUserType((Deserializer&)buf, "Deserializer");
//         function->EndCall();
        //(*function)((Deserializer&)buf);
        CallPythonFunction(function, scriptObjectRef_, (Deserializer&)buf);
    }
}

void PythonScriptInstance::SetScriptNetworkDataAttr(const ea::vector<unsigned char>& data)
{
    if (!scriptObjectRef_)
        return;

    auto function = scriptObjectMethods_[LSOM_READNETWORKUPDATE];
    if (function/* && function->BeginCall(this)*/)
    {
        MemoryBuffer buf(data);
//         function->PushUserType((Deserializer&)buf, "Deserializer");
//         function->EndCall();
        CallPythonFunction(function, scriptObjectRef_, (Deserializer&)buf);
    }
}

PythonFile* PythonScriptInstance::GetScriptFile() const
{
    return scriptFile_;
}

ea::vector<unsigned char> PythonScriptInstance::GetScriptDataAttr() const
{
    if (!scriptObjectRef_)
        return ea::vector<unsigned char>();

    VectorBuffer buf;

    auto function = scriptObjectMethods_[LSOM_SAVE];
    if (function/* && function->BeginCall(this)*/)
    {
//         function->PushUserType((Serializer&)buf, "Serializer");
//         function->EndCall();
        CallPythonFunction(function, scriptObjectRef_, (Serializer&)buf);
    }

    return buf.GetBuffer();
}

ea::vector<unsigned char> PythonScriptInstance::GetScriptNetworkDataAttr() const
{
    if (!scriptObjectRef_)
        return ea::vector<unsigned char>();

    VectorBuffer buf;

    auto function = scriptObjectMethods_[LSOM_WRITENETWORKUPDATE];
    if (function/* && function->BeginCall(this)*/)
    {
//         function->PushUserType((Serializer&)buf, "Serializer");
//         function->EndCall();
        CallPythonFunction(function, scriptObjectRef_, (Serializer&)buf);
    }

    return buf.GetBuffer();
}

void PythonScriptInstance::OnSceneSet(Scene* previousScene, Scene* scene)
{
    if (scene)
        SubscribeToScriptMethodEvents();
    else
        UnsubscribeFromScriptMethodEvents();
}

void PythonScriptInstance::OnMarkedDirty(Node* node)
{
    // Script functions are not safe from worker threads
    Scene* scene = GetScene();
    if (scene && scene->IsThreadedUpdate())
    {
        scene->DelayedMarkedDirty(this);
        return;
    }

    auto function = scriptObjectMethods_[LSOM_TRANSFORMCHANGED];
    if (function/* && function->BeginCall(this)*/)
        //function->EndCall();
        CallPythonFunction(function, scriptObjectRef_);
}

void PythonScriptInstance::GetScriptAttributes()
{
//     // Get all attribute names
//     ea::vector<ea::string> names;
//     if (lua_istable(luaState_, -1))
//     {
//         size_t length = lua_rawlen(luaState_, -1);
//         for (size_t i = 1; i <= length; ++i)
//         {
//             lua_pushinteger(luaState_, (int)i);
//             lua_gettable(luaState_, -2);
// 
//             if (!lua_isstring(luaState_, -1))
//             {
//                 lua_pop(luaState_, 1);
//                 continue;
//             }
// 
//             ea::string name = lua_tostring(luaState_, -1);
//             names.push_back(name);
// 
//             lua_pop(luaState_, 1);
//         }
//     }
//     lua_pop(luaState_, 1);
// 
//     attributeInfos_ = *context_->GetAttributes(GetTypeStatic());
// 
//     for (unsigned i = 0; i < names.size(); ++i)
//     {
//         lua_pushstring(luaState_, names[i].c_str());
//         lua_gettable(luaState_, -2);
// 
//         // Get attribute type
//         int type = lua_type(luaState_, -1);
// 
//         AttributeInfo info;
//         info.mode_ = AM_FILE;
//         info.name_ = names[i];
//         info.ptr_ = (void*)0xffffffffffffffff;
// 
//         switch (type)
//         {
//         case LUA_TBOOLEAN:
//             info.type_ = VAR_BOOL;
//             break;
//         case LUA_TNUMBER:
//             info.type_ = VAR_DOUBLE;
//             break;
//         case LUA_TSTRING:
//             info.type_ = VAR_STRING;
//             break;
//         case LUA_TUSERDATA:
//             {
//                 ea::string typeName = "";// tolua_typename(luaState_, -1);
//                 lua_pop(luaState_, 1);
// 
//                 if (typeName == "Vector2")
//                     info.type_ = VAR_VECTOR2;
//                 else if (typeName == "Vector3")
//                     info.type_ = VAR_VECTOR3;
//                 else if (typeName == "Vector4")
//                     info.type_ = VAR_VECTOR4;
//                 else if (typeName == "Quaternion")
//                     info.type_ = VAR_QUATERNION;
//                 else if (typeName == "Color")
//                     info.type_ = VAR_COLOR;
//                 else if (typeName == "IntRect")
//                     info.type_ = VAR_INTRECT;
//                 else if (typeName == "IntVector2")
//                     info.type_ = VAR_INTVECTOR2;
//                 else if (typeName == "IntVector3")
//                     info.type_ = VAR_INTVECTOR3;
//             }
//             break;
//         default:
//             break;
//         }
//         lua_pop(luaState_, 1);
// 
//         if (info.type_ != VAR_NONE)
//             attributeInfos_.push_back(info);
//     }
}

void PythonScriptInstance::FindScriptObjectMethodRefs()
{
    for (unsigned i = 0; i < MAX_LUA_SCRIPT_OBJECT_METHODS; ++i)
        scriptObjectMethods_[i] = GetScriptObjectFunction(scriptObjectMethodNames[i]);

    if (IsEnabledEffective())
        SubscribeToScriptMethodEvents();
}

void PythonScriptInstance::SubscribeToScriptMethodEvents()
{
    Scene* scene = GetScene();

    if (scene && (scriptObjectMethods_[LSOM_UPDATE] || scriptObjectMethods_[LSOM_DELAYEDSTART]))
        SubscribeToEvent(scene, E_SCENEUPDATE, URHO3D_HANDLER(PythonScriptInstance, HandleUpdate));

    if (scene && scriptObjectMethods_[LSOM_POSTUPDATE])
        SubscribeToEvent(scene, E_SCENEPOSTUPDATE, URHO3D_HANDLER(PythonScriptInstance, HandlePostUpdate));

#if defined(URHO3D_PHYSICS) || defined(URHO3D_URHO2D)
    Component* world = GetFixedUpdateSource();

    if (world && scriptObjectMethods_[LSOM_FIXEDUPDATE])
        SubscribeToEvent(world, E_PHYSICSPRESTEP, URHO3D_HANDLER(PythonScriptInstance, HandleFixedUpdate));

    if (world && scriptObjectMethods_[LSOM_FIXEDPOSTUPDATE])
        SubscribeToEvent(world, E_PHYSICSPOSTSTEP, URHO3D_HANDLER(PythonScriptInstance, HandlePostFixedUpdate));
#endif

    if (node_ && scriptObjectMethods_[LSOM_TRANSFORMCHANGED])
        node_->AddListener(this);
}

void PythonScriptInstance::UnsubscribeFromScriptMethodEvents()
{
    UnsubscribeFromEvent(E_SCENEUPDATE);
    UnsubscribeFromEvent(E_SCENEPOSTUPDATE);

#if defined(URHO3D_PHYSICS) || defined(URHO3D_URHO2D)
    UnsubscribeFromEvent(E_PHYSICSPRESTEP);
    UnsubscribeFromEvent(E_PHYSICSPOSTSTEP);
#endif

    if (node_ && scriptObjectMethods_[LSOM_TRANSFORMCHANGED])
        node_->RemoveListener(this);
}

void PythonScriptInstance::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    // Execute delayed start before first update
    if (scriptObjectMethods_[LSOM_DELAYEDSTART])
    {
//         if (scriptObjectMethods_[LSOM_DELAYEDSTART]->BeginCall(this))
//             scriptObjectMethods_[LSOM_DELAYEDSTART]->EndCall();
        CallPythonFunction(scriptObjectMethods_[LSOM_DELAYEDSTART], scriptObjectRef_);
//        scriptObjectMethods_[LSOM_DELAYEDSTART] = nb::none;  // Only execute once
    }

    auto& function = scriptObjectMethods_[LSOM_UPDATE];
    if (function) {
        //(*function)(timeStep);
        CallPythonFunction(function, scriptObjectRef_, timeStep);
    }
//     if (function && function->BeginCall(this))
//     {
//         function->PushFloat(timeStep);
//         function->EndCall();
//     }
}

void PythonScriptInstance::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace PostUpdate;
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    auto function = scriptObjectMethods_[LSOM_POSTUPDATE];
    if (function/* && function->BeginCall(this)*/)
    {
        CallPythonFunction(function, scriptObjectRef_, timeStep);
        //         function->PushFloat(timeStep);
//         function->EndCall();
    }
}

#if defined(URHO3D_PHYSICS) || defined(URHO3D_URHO2D)

void PythonScriptInstance::HandleFixedUpdate(StringHash eventType, VariantMap& eventData)
{
    // Execute delayed start before first fixed update if not called yet
    if (scriptObjectMethods_[LSOM_DELAYEDSTART])
    {
//         if (scriptObjectMethods_[LSOM_DELAYEDSTART]->BeginCall(this))
//             scriptObjectMethods_[LSOM_DELAYEDSTART]->EndCall();
        CallPythonFunction(scriptObjectMethods_[LSOM_DELAYEDSTART], scriptObjectRef_);
//        scriptObjectMethods_[LSOM_DELAYEDSTART] = nullptr;  // Only execute once
    }

    using namespace PhysicsPreStep;
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    auto& function = scriptObjectMethods_[LSOM_FIXEDUPDATE];
    if (function/* && function->BeginCall(this)*/)
    {
        CallPythonFunction(function, scriptObjectRef_, timeStep);
        //         function->PushFloat(timeStep);
//         function->EndCall();
    }
}

void PythonScriptInstance::HandlePostFixedUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace PhysicsPostStep;
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    auto function = scriptObjectMethods_[LSOM_FIXEDPOSTUPDATE];
    if (function/* && function->BeginCall(this)*/)
    {
        CallPythonFunction(function, scriptObjectRef_, timeStep);
        //         function->PushFloat(timeStep);
//         function->EndCall();
    }
}

#endif

void PythonScriptInstance::ReleaseObject()
{
//     if (scriptObjectRef_ == sol::lua_nil)
//         return;
// 
//     attributeInfos_ = *context_->GetAttributes(GetTypeStatic());
// 
//     if (IsEnabledEffective())
//         UnsubscribeFromScriptMethodEvents();
// 
//     // Unref script object
//     //luaL_unref(luaState_, LUA_REGISTRYINDEX, scriptObjectRef_);
//     scriptObjectRef_ = sol::lua_nil;
// 
//     auto function = luaScript_->GetFunction("DestroyScriptObjectInstance");
//     if (function/* && function->BeginCall()*/)
//     {
//         CallPythonFunction(function, this);
//         //         function->PushUserType((void*)this, "PythonScriptInstance");
// //         function->EndCall();
//     }
// 
//     for (auto& scriptObjectMethod : scriptObjectMethods_)
//         scriptObjectMethod = nullptr;
}

nb::callable PythonScriptInstance::GetScriptObjectFunction(const ea::string& functionName) const
{
    return pythonScript_->GetFunction(scriptObjectType_ + "." + functionName, true);
}

void PythonScriptInstance::SetScriptFileAttr(const ResourceRef& value)
{
    auto* cache = GetSubsystem<ResourceCache>();
    SetScriptFile(cache->GetResource<PythonFile>("Scripts/" + value.name_));
}

ResourceRef PythonScriptInstance::GetScriptFileAttr() const
{
    //return GetResourceRef(scriptFile_, PythonFile::GetTypeStatic());
    return {};
}

nb::object PythonScriptInstance::GetScriptObject()
{
    return scriptObjectRef_;
}

void PythonScriptInstance::SetScriptObject(nb::object obj)
{
    scriptObjectRef_ = obj;
}

}
