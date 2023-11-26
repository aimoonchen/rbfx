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
    eventType["E_BEGINFRAME"]       = E_BEGINFRAME;
    eventType["E_UPDATE"]           = E_UPDATE;
    eventType["E_POSTUPDATE"]       = E_POSTUPDATE;
    eventType["E_RENDERUPDATE"]     = E_RENDERUPDATE;
    eventType["E_POSTRENDERUPDATE"] = E_POSTRENDERUPDATE;
    eventType["E_ENDFRAME"]         = E_ENDFRAME;

    auto paramType = lua["ParamType"].get_or_create<sol::table>();
    paramType["P_FRAMENUMBER"]  = BeginFrame::P_FRAMENUMBER;
    paramType["P_TIMESTEP"]     = BeginFrame::P_TIMESTEP;
}

int sol2_CoreLuaAPI_open(sol::state& lua)
{
    lua["GetPlatform"] = []() { return Urho3D::GetPlatform(); };
    lua["GetPlatformName"] = []() { return Urho3D::GetPlatformName(); };
    //
    lua["GetUserID"] = []() {
        auto& engineParameters = Urho3D::GetEngineParameters();
        if (engineParameters.find(EP_USERID) != engineParameters.end()) {
            return engineParameters[EP_USERID].GetInt();
        }
        return 0;
    };
    auto log = lua["log"].get_or_create<sol::table>();
    log["Info"] = [](const ea::string& msg) { URHO3D_LOGINFO(msg); };
    log["Warn"] = [](const ea::string& msg) { URHO3D_LOGWARNING(msg); };
    log["Error"] = [](const ea::string& msg) { URHO3D_LOGERROR(msg); };
    lua.new_usertype<StringHash>("StringHash",
        sol::call_constructor, sol::factories([](const char* str) { return StringHash(str); },
            [](const ea::string& str) { return StringHash(str); },
            [](const ea::string_view& str) { return StringHash(str); },
            [](const StringHash& rhs) { return StringHash(rhs); }));
    lua.new_usertype<Object>("Object",
        "GetType", &Object::GetType,
        "GetTypeName", &Object::GetTypeName
//        "SendEvent", sol::overload(
//            [](Object* self, StringHash name) { self->SendEvent(name); },
//            [](Object* self, StringHash name, VariantMap& param) { self->SendEvent(name, param); })
        );
    lua.new_usertype<Variant>("Variant",
        sol::call_constructor, sol::factories(
            []() { return Variant(); },
            [](int value) { return Variant(value); },
            [](unsigned value) { return Variant(value); },
            [](bool value) { return Variant(value); },
            [](float value) { return Variant(value); },
            [](const ea::string& value) { return Variant(value); },
            [](const Vector2& value) { return Variant(value); },
            [](const Vector3& value) { return Variant(value); },
            [](const Vector4& value) { return Variant(value); },
            [](const Quaternion& value) { return Variant(value); },
            [](const Color& value) { return Variant(value); },
            [](const ResourceRef& value) { return Variant(value); },
            [](const ResourceRefList& value) { return Variant(value); },
            [](const VariantVector& value) { return Variant(value); },
            [](const VariantMap& value) { return Variant(value); },
            [](const StringVector& value) { return Variant(value); },
            [](const Rect& value) { return Variant(value); },
            [](const IntRect& value) { return Variant(value); },
            [](const IntVector2& value) { return Variant(value); },
            [](const IntVector3& value) { return Variant(value); },
            [](const Matrix3& value) { return Variant(value); },
            [](const Matrix3x4& value) { return Variant(value); },
            [](const Matrix4& value) { return Variant(value); },
            [](const VectorBuffer& value) { return Variant(value); },
            [](const StringHash& value) { return Variant(value); }
        ),
        "GetPtr",       [&lua](Variant* self, const ea::string& type) {
            auto obj = self->GetPtr();
            if (type == "Connection") {
                return sol::make_object(lua.lua_state(), static_cast<const Connection*>(obj));
            } else if (type == "Node") {
                return sol::make_object(lua.lua_state(), static_cast<const Node*>(obj));
            } else if (type == "CrowdAgent") {
                return sol::make_object(lua.lua_state(), static_cast<const CrowdAgent*>(obj));
            }
            return sol::make_object(lua.lua_state(), static_cast<const void*>(obj));
        },
        "GetInt",       &Variant::GetInt,
        "GetUInt",      &Variant::GetUInt,
        "GetInt64",     &Variant::GetInt64,
        "GetUInt64",    &Variant::GetUInt64,
        "GetBool",      &Variant::GetBool,
        "GetFloat",     &Variant::GetFloat,
        "Quaternion",   &Variant::GetQuaternion,
        "Color",        &Variant::GetColor,
        "GetIntVector2",    &Variant::GetIntVector2,
        "GetIntVector3",    &Variant::GetIntVector3,
        "GetVector2",   &Variant::GetVector2,
        "GetVector3",   &Variant::GetVector3,
        "GetVector4",   &Variant::GetVector4,
        "GetVectorBuffer",  &Variant::GetVectorBuffer,
        "GetString",    &Variant::GetString//[](Variant* obj) { return std::string_view{(const char*)obj->GetBuffer().Buffer(), obj->GetBuffer().Size()}; }
    );
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
