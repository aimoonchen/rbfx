#include "GetPush.h"
#include <string_view>
#include "../../Core/Context.h"
#include "../../Core/ProcessUtils.h"
#include "../../Core/Object.h"
#include "../../Core/Variant.h"
#include "../../Core/CoreEvents.h"
#include "../../Core/Timer.h"
#include "../../IO/Log.h"
#include "../../Network/Connection.h"
#include "../../Scene/Node.h"
#include "../../Navigation/CrowdAgent.h"
#include "../../Engine/EngineDefs.h"
#include "../../Physics2D/CollisionShape2D.h"
#include "../../Physics2D/RigidBody2D.h"

using namespace Urho3D;
Urho3D::Context* GetContext(lua_State* L);
namespace Urho3D
{
class Connection;
StringVariantMap& GetEngineParameters();
}

namespace sol {
template <> struct is_automagical<VariantMap> : std::integral_constant<bool, false>
{
};
}
static void RegisterCoreConst(sol::state& lua)
{
    auto eventType = lua["EventType"].get_or_create<sol::table>();
    eventType["BeginFrame"]       = E_BEGINFRAME;
    eventType["InputReady"]       = E_INPUTREADY;
    eventType["Update"]           = E_UPDATE;
    eventType["PostUpdate"]       = E_POSTUPDATE;
    eventType["RenderUpdate"]     = E_RENDERUPDATE;
    eventType["PostRenderUpdate"] = E_POSTRENDERUPDATE;
    eventType["EndFrame"]         = E_ENDFRAME;

    auto paramType = lua["ParamType"].get_or_create<sol::table>();
    auto beginFrame = paramType["BeginFrame"].get_or_create<sol::table>();
    beginFrame["FrameNumber"]       = BeginFrame::P_FRAMENUMBER;
    beginFrame["TimeStep"]          = BeginFrame::P_TIMESTEP;
    auto inputReady                 = paramType["InputReady"].get_or_create<sol::table>();
    inputReady["TimeStep"]          = InputReady::P_TIMESTEP;
    auto update                     = paramType["Update"].get_or_create<sol::table>();
    update["TimeStep"]              = Update::P_TIMESTEP;
    auto postUpdate                 = paramType["PostUpdate"].get_or_create<sol::table>();
    postUpdate["TimeStep"]          = PostUpdate::P_TIMESTEP;
    auto upRenderUpdatedate         = paramType["UpRenderUpdatedate"].get_or_create<sol::table>();
    upRenderUpdatedate["TimeStep"]  = RenderUpdate::P_TIMESTEP;
    auto postRenderUpdate           = paramType["PostRenderUpdate"].get_or_create<sol::table>();
    postRenderUpdate["TimeStep"]    = PostRenderUpdate::P_TIMESTEP;
}

int sol2_CoreLuaAPI_open(sol::state& lua)
{
    lua["GetPlatform"]      = []() { return Urho3D::GetPlatform(); };
    lua["GetPlatformName"]  = []() { return Urho3D::GetPlatformName(); };
    //
    lua["GetUserID"]        = []() {
        auto& engineParameters = Urho3D::GetEngineParameters();
        if (engineParameters.find(EP_USERID) != engineParameters.end()) {
            return engineParameters[EP_USERID].GetInt();
        }
        return 0;
    };
    auto log = lua["log"].get_or_create<sol::table>();
    log["Info"]     = [](const ea::string& msg) { URHO3D_LOGINFO(msg); };
    log["Warn"]     = [](const ea::string& msg) { URHO3D_LOGWARNING(msg); };
    log["Error"]    = [](const ea::string& msg) { URHO3D_LOGERROR(msg); };

    lua.new_usertype<StringHash>("StringHash", sol::call_constructor,
        sol::constructors<StringHash(const char* str),
            StringHash(const ea::string&),
            StringHash(const ea::string_view&),
            StringHash(const StringHash&)>());

    auto bindObject = lua.new_usertype<Object>("Object");
    bindObject["GetType"]       = &Object::GetType;
    bindObject["GetTypeName"]   = &Object::GetTypeName;
//        "SendEvent", sol::overload(
//            [](Object* self, StringHash name) { self->SendEvent(name); },
//            [](Object* self, StringHash name, VariantMap& param) { self->SendEvent(name, param); })

    auto bindVariant = lua.new_usertype<Variant>("Variant", sol::call_constructor,
        sol::constructors<Variant(),
            Variant(int),
            Variant(unsigned),
            Variant(bool),
            Variant(float),
            Variant(const ea::string&),
            Variant(const Vector2&),
            Variant(const Vector3&),
            Variant(const Vector4&),
            Variant(const Quaternion&),
            Variant(const Color&),
            Variant(const ResourceRef&),
            Variant(const ResourceRefList&),
            Variant(const VariantVector&),
            Variant(const VariantMap&),
            Variant(const StringVector&),
            Variant(const Rect&),
            Variant(const IntRect&),
            Variant(const IntVector2&),
            Variant(const IntVector3&),
            Variant(const Matrix3&),
            Variant(const Matrix3x4&),
            Variant(const Matrix4&),
            Variant(const VectorBuffer&),
            Variant(const StringHash&)>());
    bindVariant["GetPtr"]           = [&lua](Variant* self, const ea::string& type) {
        auto obj = self->GetPtr();
        if (type == "Connection") {
            return sol::make_object(lua.lua_state(), static_cast<const Connection*>(obj));
        }
        else if (type == "Node") {
            return sol::make_object(lua.lua_state(), static_cast<const Node*>(obj));
        }
        else if (type == "CrowdAgent") {
            return sol::make_object(lua.lua_state(), static_cast<const CrowdAgent*>(obj));
        }
        else if (type == "RigidBody2D")
        {
            return sol::make_object(lua.lua_state(), static_cast<const RigidBody2D*>(obj));
        }
        else if (type == "CollisionShape2D")
        {
            return sol::make_object(lua.lua_state(), static_cast<const CollisionShape2D*>(obj));
        }
        return sol::make_object(lua.lua_state(), static_cast<const void*>(obj));
        };
    bindVariant["GetInt"]           = &Variant::GetInt;
    bindVariant["GetUInt"]          = &Variant::GetUInt;
    bindVariant["GetInt64"]         = &Variant::GetInt64;
    bindVariant["GetUInt64"]        = &Variant::GetUInt64;
    bindVariant["GetBool"]          = &Variant::GetBool;
    bindVariant["GetFloat"]         = &Variant::GetFloat;
    bindVariant["Quaternion"]       = &Variant::GetQuaternion;
    bindVariant["Color"]            = &Variant::GetColor;
    bindVariant["GetIntVector2"]    = &Variant::GetIntVector2;
    bindVariant["GetIntVector3"]    = &Variant::GetIntVector3;
    bindVariant["GetVector2"]       = &Variant::GetVector2;
    bindVariant["GetVector3"]       = &Variant::GetVector3;
    bindVariant["GetVector4"]       = &Variant::GetVector4;
    bindVariant["GetVectorBuffer"]  = &Variant::GetVectorBuffer;
    bindVariant["GetString"]        = &Variant::GetString;//[](Variant* obj) { return std::string_view{(const char*)obj->GetBuffer().Buffer(), obj->GetBuffer().Size()}; }
    
//     sol::automagic_enrollments enrollments;
//     enrollments.less_than_operator = false;
//     enrollments.less_than_operator = false;
    lua.new_usertype<VariantMap>("VariantMap",
        sol::call_constructor, sol::factories([]() { return VariantMap(); }),
        sol::meta_function::index, [](VariantMap* map, StringHash key) { return &(*map)[key]; },
        sol::meta_function::new_index, [](VariantMap* map, StringHash key, const Variant& variant) { (*map)[key] = variant; },
        "HasKey", [](VariantMap* map, StringHash key) { return map->find(key) != map->end(); },
        "Size", [](VariantMap* map){ return map->size(); }
        );

    lua.new_usertype<Time>("Time", "GetTimeStamp", [](Time* self) { return self->GetTimeStamp(); });
    auto context = GetContext(lua.lua_state());
    lua["time"] = context->GetSubsystem<Time>();
    //
    lua["GetEventSender"] = [context]() {
        auto obj = context->GetEventSender();
        if (obj->GetType() == CrowdManager::GetTypeStatic()) {
            return static_cast<const CrowdManager*>(obj);
        } else {
            return (const CrowdManager*)nullptr;
        }
    };
    RegisterCoreConst(lua);
    return 0;
}
