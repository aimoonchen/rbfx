#include <nanobind/nanobind.h>
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
namespace nb = nanobind;
using namespace nb::literals;

namespace Urho3D
{
class Connection;
StringVariantMap& GetEngineParameters();
}
static void RegisterCoreConst(nb::module_ m)
{
    auto eventType = m.def_submodule("EventType");
    eventType["BeginFrame"]       = E_BEGINFRAME;
    eventType["InputReady"]       = E_INPUTREADY;
    eventType["Update"]           = E_UPDATE;
    eventType["PostUpdate"]       = E_POSTUPDATE;
    eventType["RenderUpdate"]     = E_RENDERUPDATE;
    eventType["PostRenderUpdate"] = E_POSTRENDERUPDATE;
    eventType["EndFrame"]         = E_ENDFRAME;

    auto paramType = m.def_submodule("ParamType");
    auto beginFrame = paramType.def_submodule("BeginFrame");
    beginFrame["FrameNumber"]       = BeginFrame::P_FRAMENUMBER;
    beginFrame["TimeStep"]          = BeginFrame::P_TIMESTEP;
    auto inputReady                 = paramType.def_submodule("InputReady");
    inputReady["TimeStep"]          = InputReady::P_TIMESTEP;
    auto update                     = paramType.def_submodule("Update");
    update["TimeStep"]              = Update::P_TIMESTEP;
    auto postUpdate                 = paramType.def_submodule("PostUpdate");
    postUpdate["TimeStep"]          = PostUpdate::P_TIMESTEP;
    auto upRenderUpdatedate         = paramType.def_submodule("UpRenderUpdatedate");
    upRenderUpdatedate["TimeStep"]  = RenderUpdate::P_TIMESTEP;
    auto postRenderUpdate           = paramType.def_submodule("PostRenderUpdate");
    postRenderUpdate["TimeStep"]    = PostRenderUpdate::P_TIMESTEP;
}
#undef NB_EXPORT
#define NB_EXPORT
NB_MODULE(core, m)
{
    m.def("GetPlatform", []() { return Urho3D::GetPlatform(); });
    m.def("GetPlatformName", []() { return Urho3D::GetPlatformName(); });
    //
    m.def("GetUserID", []() {
        auto& engineParameters = Urho3D::GetEngineParameters();
        if (engineParameters.find(EP_USERID) != engineParameters.end()) {
            return engineParameters[EP_USERID].GetInt();
        }
        return 0;
    });
    m.def_submodule("log")
        .def("Info", [](const ea::string& msg) { URHO3D_LOGINFO(msg); })
        .def("Warn", [](const ea::string& msg) { URHO3D_LOGWARNING(msg); })
        .def("Error", [](const ea::string& msg) { URHO3D_LOGERROR(msg); });

    nb::class_<StringHash>(m, "StringHash")
        .def(nb::init<const char*>());

    nb::class_<Object>(m, "Object")
        .def("GetType", &Object::GetType)
        .def("GetTypeName", &Object::GetTypeName)
        .def("SendEvent", [](Object* self, StringHash name) { self->SendEvent(name); })
        .def("SendEvent", [](Object* self, StringHash name, VariantMap& param) { self->SendEvent(name, param); });

    nb::class_<Variant>(m, "Variant")
        .def(nb::init<>())
        .def(nb::init<int>())
        .def(nb::init<unsigned>())
        .def(nb::init<bool>())
        .def(nb::init<float>())
        .def(nb::init<const ea::string&>())
        .def(nb::init<const Vector2&>())
        .def(nb::init<const Vector3&>())
        .def(nb::init<const Vector4&>())
        .def(nb::init<const Quaternion&>())
        .def(nb::init<const Color&>())
        .def(nb::init<const ResourceRef&>())
        .def(nb::init<const ResourceRefList&>())
        .def(nb::init<const VariantVector&>())
        .def(nb::init<const VariantMap&>())
        .def(nb::init<const StringVector&>())
        .def(nb::init<const Rect&>())
        .def(nb::init<const IntRect&>())
        .def(nb::init<const IntVector2&>())
        .def(nb::init<const IntVector3&>())
        .def(nb::init<const Matrix3&>())
        .def(nb::init<const Matrix3x4&>())
        .def(nb::init<const Matrix4&>())
        .def(nb::init<const VectorBuffer&>())
        .def(nb::init<const StringHash&>())
        //     bindVariant["GetPtr"]           = [&lua](Variant* self, const ea::string& type) {
        //         auto obj = self->GetPtr();
        //         if (type == "Connection") {
        //             return sol::make_object(lua.lua_state(), static_cast<const Connection*>(obj));
        //         }
        //         else if (type == "Node") {
        //             return sol::make_object(lua.lua_state(), static_cast<const Node*>(obj));
        //         }
        //         else if (type == "CrowdAgent") {
        //             return sol::make_object(lua.lua_state(), static_cast<const CrowdAgent*>(obj));
        //         }
        //         else if (type == "RigidBody2D")
        //         {
        //             return sol::make_object(lua.lua_state(), static_cast<const RigidBody2D*>(obj));
        //         }
        //         else if (type == "CollisionShape2D")
        //         {
        //             return sol::make_object(lua.lua_state(), static_cast<const CollisionShape2D*>(obj));
        //         }
        //         return sol::make_object(lua.lua_state(), static_cast<const void*>(obj));
        //         };
        .def("GetInt", &Variant::GetInt)
        .def("GetUInt", &Variant::GetUInt)
        .def("GetInt64", &Variant::GetInt64)
        .def("GetUInt64", &Variant::GetUInt64)
        .def("GetBool", &Variant::GetBool)
        .def("GetFloat", &Variant::GetFloat)
        .def("Quaternion", &Variant::GetQuaternion)
        .def("Color", &Variant::GetColor)
        .def("GetIntVector2", &Variant::GetIntVector2)
        .def("GetIntVector3", &Variant::GetIntVector3)
        .def("GetVector2", &Variant::GetVector2)
        .def("GetVector3", &Variant::GetVector3)
        .def("GetVector4", &Variant::GetVector4)
        .def("GetVectorBuffer", &Variant::GetVectorBuffer)
        .def("GetString", &Variant::GetString);//[](Variant* obj) { return std::string_view{(const char*)obj->GetBuffer().Buffer(), obj->GetBuffer().Size()}; }
    
//     sol::automagic_enrollments enrollments;
//     enrollments.less_than_operator = false;
//     enrollments.less_than_operator = false;
    nb::class_<VariantMap>(m, "VariantMap")
        .def(nb::init<>())
        .def("__getitem__", [](VariantMap& map, StringHash key) { return &map[key]; }, nb::is_operator())
        .def("__setitem__", [](VariantMap& map, StringHash key, const Variant& variant) { map[key] = variant; }, nb::is_operator())
        .def("HasKey", [](VariantMap& map, StringHash key) { return map.find(key) != map.end(); })
        .def("Size", [](VariantMap& map) { return map.size(); });

    nb::class_<Time>(m, "Time")
        .def("GetTimeStamp", [](Time* self) { return self->GetTimeStamp(); });

    m.attr("time") = Context::GetInstance()->GetSubsystem<Time>();
    //
    m.def("GetEventSender", []() {
        auto obj = Context::GetInstance()->GetEventSender();
        if (obj->GetType() == CrowdManager::GetTypeStatic()) {
            return static_cast<const CrowdManager*>(obj);
        } else {
            return (const CrowdManager*)nullptr;
        }
    });
    RegisterCoreConst(m);
}
