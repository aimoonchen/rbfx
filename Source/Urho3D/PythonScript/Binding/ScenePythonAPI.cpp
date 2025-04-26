#include <nanobind/nanobind.h>
#include "EAStringAPI.h"
#include <nanobind/stl/unique_ptr.h>
#include <RmlUi/Core/ElementDocument.h>
#include "Urho3D/Core/Context.h"
#include "Urho3D/Scene/Node.h"
#include "Urho3D/Scene/Scene.h"
#include "Urho3D/Scene/Component.h"
#include "Urho3D/Scene/LogicComponent.h"
#include "Urho3D/RmlUI/RmlUIComponent.h"
#include "Urho3D/Scene/ValueAnimation.h"
#include "Urho3D/Scene/ObjectAnimation.h"
#include "Urho3D/Scene/PrefabReference.h"
#include "Urho3D/Scene/SplinePath.h"
#include "Urho3D/Scene/SceneEvents.h"
#include "Urho3D/IO/File.h"
#include "Urho3D/Graphics/Light.h"
#include "Urho3D/Graphics/StaticModel.h"
#include "Urho3D/Graphics/AnimatedModel.h"
#include "Urho3D/Graphics/AnimationController.h"
#include "Urho3D/Graphics/Skybox.h"
#include "Urho3D/Graphics/Octree.h"
#include "Urho3D/Graphics/Camera.h"
#include "Urho3D/Graphics/Zone.h"
#include "Urho3D/Graphics/DebugRenderer.h"
#include "Urho3D/Graphics/MeshLine.h"
#include "Urho3D/Graphics/DecalSet.h"
#include "Urho3D/Graphics/BillboardSet.h"
#include "Urho3D/Graphics/Terrain.h"
#include "Urho3D/Graphics/ProceduralSky.h"
#include "Urho3D/Graphics/OutlineGroup.h"
#include "Urho3D/Graphics/Texture2D.h"
#include "Urho3D/UI/Text3D.h"
#include "Urho3D/RenderPipeline/RenderPipeline.h"

#if defined(__linux__) && !defined(__ANDROID__)
#else
#include "../../EffekseerUrho3D/EffekseerEmitter.h"
#endif
#include "../../Navigation/NavigationMesh.h"
#include "../../Navigation/DynamicNavigationMesh.h"
#include "../../Navigation/Navigable.h"
#include "../../Navigation/Obstacle.h"
#include "../../Navigation/CrowdAgent.h"
#include "../../Navigation/CrowdManager.h"
#include "../../Navigation/OffMeshConnection.h"
#include "../../Physics/PhysicsWorld.h"
#include "../../Physics/CollisionShape.h"
#include "../../Physics/Constraint.h"
#include "../../Physics/RigidBody.h"
#include "../../Physics2D/PhysicsWorld2D.h"
#include "../../Physics2D/RigidBody2D.h"
#include "../../Physics2D/CollisionBox2D.h"
#include "../../Physics2D/CollisionCircle2D.h"
#include "../../Physics2D/CollisionPolygon2D.h"
#include "../../Physics2D/CollisionChain2D.h"
#include "../../Urho2D/StaticSprite2D.h"
#include "../../Urho2D/AnimatedSprite2D.h"
#include "../../Urho2D/TileMapLayer2D.h"
#include "../../Urho2D/TileMap2D.h"
#include "../../Urho2D/StretchableSprite2D.h"

using namespace Urho3D;
namespace nb = nanobind;
using namespace nb::literals;

// namespace {
// ea::unordered_map<Urho3D::StringHash, ea::function<int(lua_State* L, const Component* obj)>> component_convert_map = {
//     {EffekseerEmitter::GetTypeStatic(),         [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const EffekseerEmitter*>(obj)).push(L); }},
//     {Octree::GetTypeStatic(),                   [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const Octree*>(obj)).push(L); }},
//     {DebugRenderer::GetTypeStatic(),            [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const DebugRenderer*>(obj)).push(L); }},
//     {MeshLine::GetTypeStatic(),                 [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const MeshLine*>(obj)).push(L); }},
//     {Camera::GetTypeStatic(),                   [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const Camera*>(obj)).push(L); }},
//     {Zone::GetTypeStatic(),                     [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const Zone*>(obj)).push(L); }},
//     {Light::GetTypeStatic(),                    [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const Light*>(obj)).push(L); }},
//     {StaticModel::GetTypeStatic(),              [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const StaticModel*>(obj)).push(L); }},
//     {AnimatedModel::GetTypeStatic(),            [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const AnimatedModel*>(obj)).push(L); }},
//     {Skybox::GetTypeStatic(),                   [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const Skybox*>(obj)).push(L); }},
//     {DecalSet::GetTypeStatic(),                 [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const DecalSet*>(obj)).push(L); }},
//     {BillboardSet::GetTypeStatic(),             [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const BillboardSet*>(obj)).push(L); }},
//     {Terrain::GetTypeStatic(),                  [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const Terrain*>(obj)).push(L); }},
//     {RenderPipeline::GetTypeStatic(),           [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const RenderPipeline*>(obj)).push(L); }},
//     {ProceduralSky::GetTypeStatic(),            [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const ProceduralSky*>(obj)).push(L); }},
//     {OutlineGroup::GetTypeStatic(),             [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const OutlineGroup*>(obj)).push(L); }},
//     {Text3D::GetTypeStatic(),                   [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const Text3D*>(obj)).push(L); }},
//     {NavigationMesh::GetTypeStatic(),           [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const NavigationMesh*>(obj)).push(L); }},
//     {DynamicNavigationMesh::GetTypeStatic(),    [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const DynamicNavigationMesh*>(obj)).push(L); }},
//     {CrowdAgent::GetTypeStatic(),               [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const CrowdAgent*>(obj)).push(L); }},
//     {CrowdManager::GetTypeStatic(),             [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const CrowdManager*>(obj)).push(L); }},
//     {Navigable::GetTypeStatic(),                [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const Navigable*>(obj)).push(L); }},
//     {Obstacle::GetTypeStatic(),                 [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const Obstacle*>(obj)).push(L); }},
//     {OffMeshConnection::GetTypeStatic(),        [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const OffMeshConnection*>(obj)).push(L); }},
//     {RigidBody::GetTypeStatic(),                [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const RigidBody*>(obj)).push(L); }},
//     {CollisionShape::GetTypeStatic(),           [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const CollisionShape*>(obj)).push(L); }},
//     {Constraint::GetTypeStatic(),               [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const Constraint*>(obj)).push(L); }},
//     {PhysicsWorld::GetTypeStatic(),             [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const PhysicsWorld*>(obj)).push(L); }},
//     {PhysicsWorld2D::GetTypeStatic(),           [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const PhysicsWorld2D*>(obj)).push(L); }},
//     {RigidBody2D::GetTypeStatic(),              [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const RigidBody2D*>(obj)).push(L); }},
//     {CollisionBox2D::GetTypeStatic(),           [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const CollisionBox2D*>(obj)).push(L); }},
//     {CollisionCircle2D::GetTypeStatic(),        [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const CollisionCircle2D*>(obj)).push(L); }},
//     {CollisionPolygon2D::GetTypeStatic(),       [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const CollisionPolygon2D*>(obj)).push(L); }},
//     {CollisionChain2D::GetTypeStatic(),         [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const CollisionChain2D*>(obj)).push(L); }},
//     {StaticSprite2D::GetTypeStatic(),           [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const StaticSprite2D*>(obj)).push(L); }},
//     {AnimatedSprite2D::GetTypeStatic(),         [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const AnimatedSprite2D*>(obj)).push(L); }},
//     {StretchableSprite2D::GetTypeStatic(),      [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const StretchableSprite2D*>(obj)).push(L); }},
//     {PrefabReference::GetTypeStatic(),          [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const PrefabReference*>(obj)).push(L); }},
//     {RmlUIComponent::GetTypeStatic(),           [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const RmlUIComponent*>(obj)).push(L); }},
//     {TileMapLayer2D::GetTypeStatic(),           [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const TileMapLayer2D*>(obj)).push(L); }},
//     {TileMap2D::GetTypeStatic(),                [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const TileMap2D*>(obj)).push(L); }}
// };
// } // namespace
// 
// template <typename T> void RegisterComponentConverter()
// {
//     component_convert_map.insert({T::GetTypeStatic(), [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const T*>(obj)).push(L); }});
// }
// 
// namespace sol {
//     int sol_lua_push(sol::types<Component*>, lua_State* L, const Component* obj)
//     {
//         if (obj) {
//             if (auto it = component_convert_map.find(obj->GetType()); it != component_convert_map.end()) {
//                 auto& [typeKey, converter] = *it;
//                 return converter(L, obj);
//             } else {
//                 URHO3D_LOGERRORF("sol_lua_push error, can't find convert function for %s.", obj->GetTypeName());
//             }
//         }
//         return sol::make_object(L, obj).push(L);
//     }
// }

static void RegisterSceneConst(nb::module_ m)
{
    auto eventType              = m.def_submodule("EventType");
    eventType.attr("SceneUpdate") = E_SCENEUPDATE;
    eventType.attr("ScenePostUpdate") = E_SCENEPOSTUPDATE;

    auto paramType              = m.def_submodule("ParamType");
    auto sceneUpdate            = paramType.def_submodule("SceneUpdate");
    sceneUpdate.attr("Scene") = SceneUpdate::P_SCENE;
    sceneUpdate.attr("TimeStep") = SceneUpdate::P_TIMESTEP;
    auto scenePostUpdate        = paramType.def_submodule("ScenePostUpdate");
    scenePostUpdate.attr("Scene") = ScenePostUpdate::P_SCENE;
    scenePostUpdate.attr("TimeStep") = ScenePostUpdate::P_TIMESTEP;
}
void init_cmodule_scene(nb::module_& pm)
{
    auto m = pm.def_submodule("scene");
    nb::enum_<WrapMode>(m, "WrapMode")
        .value("WM_LOOP", WM_LOOP)
        .value("WM_ONCE", WM_ONCE)
        .value("WM_CLAMP", WM_CLAMP);

    nb::enum_<InterpMethod>(m, "InterpMethod")
        .value("IM_NONE", IM_NONE)
        .value("IM_LINEAR", IM_LINEAR)
        .value("IM_SPLINE", IM_SPLINE);

    nb::enum_<TransformSpace>(m, "TransformSpace")
        .value("TS_LOCAL", TS_LOCAL)
        .value("TS_PARENT", TS_PARENT)
        .value("TS_WORLD", TS_WORLD);

    nb::class_<Component>(m, "Component")
        .def("GetID", &Component::GetID)
        .def("SetEnabled", &Component::SetEnabled)
        .def("IsEnabled", &Component::IsEnabled)
        .def("GetNode", &Component::GetNode)
        .def("GetScene", &Component::GetScene)
        .def("Remove", &Component::Remove)
        .def("DrawDebugGeometry", &Component::DrawDebugGeometry)
        .def("SetAttribute", [](Component* self, const ea::string& name, const Variant& value) { self->SetAttribute(name, value); })
        .def("SendEvent", [](Component* self, StringHash name) { self->SendEvent(name); })
        .def("SendEvent", [](Component* self, StringHash name, VariantMap& param) { self->SendEvent(name, param); });
        //         "SetAnimationEnabled", &Animatable::SetAnimationEnabled,
        //         "SetAnimationTime", &Animatable::SetAnimationTime,
        //         "SetObjectAnimation", &Animatable::SetObjectAnimation,
        //         "SetAttributeAnimationWrapMode", &Animatable::SetAttributeAnimationWrapMode,
        //         "SetAttributeAnimationSpeed", &Animatable::SetAttributeAnimationSpeed,
        //         "SetAttributeAnimationTime", &Animatable::SetAttributeAnimationTime,
        //         "RemoveObjectAnimation", &Animatable::RemoveObjectAnimation,
        //         "RemoveAttributeAnimation", &Animatable::RemoveAttributeAnimation

    nb::class_<LogicComponent, Component>(m, "LogicComponent")
        .def("GetID", &LogicComponent::GetID)
        .def("SetUpdateEventMask", &LogicComponent::SetUpdateEventMask)
        .def("GetUpdateEventMask", &LogicComponent::GetUpdateEventMask);

    nb::class_<Node>(m, "Node")
        //.def(nb::init<Context*>())
        .def(nb::new_([]() { return std::make_unique<Node>(Context::GetInstance()); }))
        //.def(nb::init<std::unique_ptr<Node>>([]() { return std::make_unique<Node>(Context::GetInstance()); }))
        .def_prop_rw("id", &Node::GetID, &Node::SetID)
        .def_prop_rw("name", &Node::GetName, &Node::SetName)
        .def_prop_rw("parent", &Node::GetParent, &Node::SetParent)
        .def_prop_rw("scale", &Node::GetScale, nb::overload_cast<const Vector3&>(&Node::SetScale))
        .def_prop_rw("rotation", &Node::GetRotation, &Node::SetRotation)
        .def_prop_rw("position", &Node::GetPosition, &Node::SetPosition)
        .def_prop_rw("direction", &Node::GetDirection, &Node::SetDirection)
        .def_prop_rw("local_matrix", &Node::GetTransformMatrix, &Node::SetTransformMatrix)
        .def_prop_rw("world_scale", &Node::GetWorldScale, nb::overload_cast<const Vector3&>(&Node::SetWorldScale))
        .def_prop_rw("world_rotation", &Node::GetWorldRotation, &Node::SetWorldRotation)
        .def_prop_rw("world_position", &Node::GetWorldPosition, &Node::SetWorldPosition)
        .def_prop_rw("world_direction", &Node::GetWorldDirection, &Node::SetWorldDirection)
        .def_prop_ro("world_up", &Node::GetWorldUp)
        .def_prop_ro("world_right", &Node::GetWorldRight)
        .def_prop_rw("world_matrix", &Node::GetWorldTransform, nb::overload_cast<const Matrix3x4&>(&Node::SetWorldTransform))
    
        .def("SetTransform", [](Node* self, const Vector3& position, const Quaternion& rotation) { self->SetTransform(position, rotation); })
        .def("SetTransform", [](Node* self, const Vector3& position, const Quaternion& rotation, float scale) { self->SetTransform(position, rotation, scale); })
        .def("SetTransform", [](Node* self, const Vector3& position, const Quaternion& rotation, const Vector3& scale) { self->SetTransform(position, rotation, scale); })
        .def("SetWorldTransform", [](Node* self, const Vector3& position, const Quaternion& rotation) { self->SetWorldTransform(position, rotation); })
        .def("SetWorldTransform", [](Node* self, const Vector3& position, const Quaternion& rotation, float scale) { self->SetWorldTransform(position, rotation, scale); })
        .def("SetWorldTransform", [](Node* self, const Vector3& position, const Quaternion& rotation, const Vector3& scale) { self->SetWorldTransform(position, rotation, scale); })
    
        .def("AddTag", &Node::AddTag)
        .def("SetTags", [](Node* self, const ea::string& tags) {
            self->RemoveAllTags();
            self->AddTags(tags); })
        .def("AddTags", [](Node* self, const ea::string& tags) { self->AddTags(tags); })
        .def("AddTags", [](Node* self, const ea::string& tags, char separator) { self->AddTags(tags, separator); })
        .def("GetTags", [](Node* self) {
            const auto& rawtags = self->GetTags();
            // TODO:
            std::vector<std::string> tags;
            tags.reserve(rawtags.size());
            for (const auto& tag : rawtags) {
                tags.emplace_back(tag.c_str());
            }
            return tags;
        })
        .def("HasTag", &Node::HasTag)
        .def("RemoveTag", &Node::RemoveTag)
        .def("RemoveAllTags", &Node::RemoveAllTags)
        .def("SetScale", nb::overload_cast<float>(&Node::SetScale))
        .def("SetScale", [](Node* self, float sx, float sy, float sz) { self->SetScale({ sx, sy, sz }); })
        .def("SetScale", nb::overload_cast<const Vector3&>(&Node::SetScale))
        .def("SetWorldScale", nb::overload_cast<float>(&Node::SetWorldScale))
        .def("SetWorldScale", [](Node* self, float sx, float sy, float sz) { self->SetWorldScale({ sx, sy, sz }); })
        .def("SetWorldScale", nb::overload_cast<const Vector3&>(&Node::SetWorldScale))
        .def("SetRotation", [](Node* self, const Quaternion& rot) { self->SetRotation(rot); })
        .def("SetRotation", [](Node* self, float rx, float ry, float rz) { self->SetRotation({ rx, ry, rz }); })
        .def("SetPosition", [](Node* self, const Vector3& pos) { self->SetPosition(pos); })
        .def("SetPosition", [](Node* self, float x, float y, float z) { self->SetPosition({ x, y, z }); })
        .def("SetDirection", [](Node* self, const Vector3& dir) { self->SetDirection(dir); })
        .def("SetDirection", [](Node* self, float dx, float dy, float dz) { self->SetDirection({ dx, dy, dz }); })
        .def("Rotate", [](Node* self, float rx, float ry, float rz) { self->Rotate({ rx, ry, rz }); })
        .def("Rotate", [](Node* self, float rx, float ry, float rz, TransformSpace ts) { self->Rotate({ rx, ry, rz }, ts); })
        .def("Rotate", [](Node* self, const Quaternion& rotation) { self->Rotate(rotation); })
        .def("Rotate", [](Node* self, const Quaternion& rotation, TransformSpace ts) { self->Rotate(rotation, ts); })
        .def("RotateAround", [](Node* self, const Vector3& point, float rx, float ry, float rz) { self->RotateAround(point, { rx, ry, rz }); })
        .def("RotateAround", [](Node* self, const Vector3& point, float rx, float ry, float rz, TransformSpace ts) { self->RotateAround(point, { rx, ry, rz }, ts); })
        .def("RotateAround", [](Node* self, const Vector3& point, const Quaternion& rotation) { self->RotateAround(point, rotation); })
        .def("RotateAround", [](Node* self, const Vector3& point, const Quaternion& rotation, TransformSpace ts) { self->RotateAround(point, rotation, ts); })
        .def("Translate", [](Node* self, float x, float y, float z) { self->Translate({ x, y, z }); })
        .def("Translate", [](Node* self, float x, float y, float z, TransformSpace ts) { self->Translate({ x, y, z }, ts); })
        .def("Translate", [](Node* self, const Vector3& translate) { self->Translate(translate); })
        .def("Translate", [](Node* self, const Vector3& translate, TransformSpace ts) { self->Translate(translate, ts); })
        .def("Pitch", [](Node* self, float angle) { self->Pitch(angle); })
        .def("Pitch", [](Node* self, float angle, TransformSpace ts) { self->Pitch(angle, ts); })
        .def("Yaw", [](Node* self, float angle) { self->Yaw(angle); })
        .def("Yaw", [](Node* self, float angle, TransformSpace ts) { self->Yaw(angle, ts); })
        .def("Roll", [](Node* self, float angle) { self->Roll(angle); })
        .def("Roll", [](Node* self, float angle, TransformSpace ts) { self->Roll(angle, ts); })
        .def("LookAt", [](Node* self, const Vector3& target) { return self->LookAt(target); })
        .def("LookAt", [](Node* self, const Vector3& target, const Vector3& up) { return self->LookAt(target, up); })
        .def("LookAt", [](Node* self, const Vector3& target, const Vector3& up, TransformSpace ts) { return self->LookAt(target, up, ts); })
        .def("Scale", [](Node* self, float scale) { self->Scale(scale); })
        .def("Scale", [](Node* self, const Vector3& scale) { self->Scale(scale); })
        .def("ScaleAround", [](Node* self, const Vector3& point, const Vector3& scale) { self->ScaleAround(point, scale); })
        .def("ScaleAround", [](Node* self, const Vector3& point, const Vector3& scale, TransformSpace space) { self->ScaleAround(point, scale, space); })
        .def("GetParent", &Node::GetParent)
        .def("SetParent", &Node::SetParent)
        .def("GetScene", &Node::GetScene)
        .def("CreateChild", [](Node* self) { return self->CreateChild(); }, nb::rv_policy::reference)
        .def("CreateChild", [](Node* self, const ea::string& name) { return self->CreateChild(name); }, nb::rv_policy::reference)
        .def("CreateChild", [](Node* self, const ea::string& name, unsigned id) { return self->CreateChild(name, id); }, nb::rv_policy::reference)
        .def("CreateChild", [](Node* self, const ea::string& name, unsigned id, bool temporary) { return self->CreateChild(name, id, temporary); }, nb::rv_policy::reference)
        .def("Clone", [](Node* self) { return self->Clone(); })
        .def("Clone", [](Node* self, Node* parent) { return self->Clone(parent); })
        .def("CreateComponent", [](Node* self, StringHash type) { return self->CreateComponent(type); }, nb::rv_policy::reference)
        .def("GetComponent", [](Node* self, StringHash type) { return self->GetComponent(type); }, nb::rv_policy::reference)
        .def("GetComponent", [](Node* self, StringHash type, bool recursive) { return self->GetComponent(type); }, nb::rv_policy::reference)
        .def("GetComponents", [](Node* self, StringHash type) {
                ea::vector<Component*> dest;
                self->GetComponents(dest, type);
                return std::vector<Component*>(dest.begin(), dest.end());
            })
        .def("GetComponents", [](Node* self, StringHash type, bool recursive) {
                ea::vector<Component*> dest;
                self->GetComponents(dest, type);
                return std::vector<Component*>(dest.begin(), dest.end());
            })
        .def("RemoveComponent", [](Node* self, StringHash type) { self->RemoveComponent(type); })
        .def("RemoveComponent", [](Node* self, Component* component) { self->RemoveComponent(component); })
        .def("RemoveComponents", [](Node* self, StringHash type) { self->RemoveComponents(type); })
        .def("RemoveAllComponents", &Node::RemoveAllComponents)
        .def("CreateScriptObject", [](Node* self, const ea::string& name) {
//         auto instance = self->CreateComponent<LuaScriptInstance>();
//         instance->CreateObject(name);
//         return instance->GetScriptObject();
        })
        .def("GetScriptObject", [](Node* self) {
//         auto comp = self->GetComponent<LuaScriptInstance>();
//         return comp->GetScriptObject();
        })
        .def("SetEnabled", nb::overload_cast<bool>(&Node::SetEnabled))
        .def("SetEnabledRecursive", &Node::SetEnabledRecursive)
        .def("AddChild", [](Node* self, Node* obj) { self->AddChild(obj); })
        .def("RemoveChild", nb::overload_cast<Node*>(&Node::RemoveChild))
        .def("Remove", &Node::Remove)
        .def("RemoveChildren", &Node::RemoveChildren)
        .def("GetChildren", [](Node* self, bool recursive) {
            auto children = self->GetChildren(recursive);
            std::vector<Node*> ret;
            for (auto& child : children) {
                ret.push_back(child);
            }
            return ret;
        })
        .def("GetChild", [](Node* self, unsigned index) { return self->GetChild(index); })
        .def("GetChild", [](Node* self, const ea::string& name) { return self->GetChild(name); })
        .def("GetChild", [](Node* self, const ea::string& name, bool recursive) { return self->GetChild(name, recursive); })
        .def("GetNumChildren", [](Node* self) { return self->GetNumChildren(); })
        .def("GetNumChildren", [](Node* self, bool recursive) { return self->GetNumChildren(recursive); })
        .def("SendEvent", [](Node* self, StringHash name) { self->SendEvent(name); })
        .def("SendEvent", [](Node* self, StringHash name, VariantMap& param) { self->SendEvent(name, param); })
        .def("SetVar", &Node::SetVar)
        .def("SetVarByHash", &Node::SetVarByHash)
        .def("GetVar", &Node::GetVar)
        .def("GetVarByHash", &Node::GetVarByHash)
        // 2D
        .def("Scale2D", &Node::Scale2D)
        .def("Rotate2D", [](Node* self, float delta) { self->Rotate2D(delta); })
        .def("Rotate2D", [](Node* self, float delta, TransformSpace space) { self->Rotate2D(delta, space); })
        .def("RotateAround2D", [](Node* self, const Vector2& point, float delta) { self->RotateAround2D(point, delta); })
        .def("RotateAround2D", [](Node* self, const Vector2& point, float delta, TransformSpace space) { self->RotateAround2D(point, delta, space); })
        .def("Translate2D", [](Node* self, const Vector2& delta) { self->Translate2D(delta); })
        .def("Translate2D", [](Node* self, const Vector2& delta, TransformSpace space) { self->Translate2D(delta, space); })
        .def("SetScale2D", [](Node* self, const Vector2& scale) { self->SetScale2D(scale); })
        .def("SetScale2D", [](Node* self, float x, float y) { self->SetScale2D(x, y); })
        .def("SetRotation2D", &Node::SetRotation2D)
        .def("SetPosition2D", [](Node* self, const Vector2& position) { self->SetPosition2D(position); })
        .def("SetPosition2D", [](Node* self, float x, float y) { self->SetPosition2D(x, y); })
        .def("GetScale2D", &Node::GetScale2D)
        .def("GetRotation2D", &Node::GetRotation2D)
        .def("GetPosition2D", &Node::GetPosition2D)
        .def("SetTransform2D", [](Node* self, const Vector2& position, float rotation) { self->SetTransform2D(position, rotation); })
        .def("SetTransform2D", [](Node* self, const Vector2& position, float rotation, float scale) { self->SetTransform2D(position, rotation, scale); })
        .def("SetTransform2D", [](Node* self, const Vector2& position, float rotation, const Vector2& scale) { self->SetTransform2D(position, rotation, scale); })
        .def("SetWorldTransform2D", [](Node* self, const Vector2& position, float rotation) { self->SetWorldTransform2D(position, rotation); })
        .def("SetWorldTransform2D", [](Node* self, const Vector2& position, float rotation, float scale) { self->SetWorldTransform2D(position, rotation, scale); })
        .def("SetWorldTransform2D", [](Node* self, const Vector2& position, float rotation, const Vector2& scale) { self->SetWorldTransform2D(position, rotation, scale); });

    nb::class_<Scene, Node>(m, "Scene")
        .def(nb::new_([]() { return new Scene(Context::GetInstance()); }))
        .def("LoadXML", [](Scene* self, XMLFile* file) { return self->LoadXML(file->GetRoot()); }) // sol::resolve<bool(Deserializer&)>(&Scene::LoadXML),
        .def("GetNode", &Scene::GetNode)
        .def("GetNodesWithTag", [](Scene* self, const ea::string& tag) {
            std::vector<Node*> ret;
            ea::vector<Node*> dest;
            if (self->GetNodesWithTag(dest, tag)) {
                ret.reserve(dest.size());
                for (auto nd : dest) {
                    ret.push_back(nd);
                }
            }
            return ret;
        })
        .def("Clear", &Scene::Clear)
        .def("SetUpdateEnabled", &Scene::SetUpdateEnabled)
        .def("IsUpdateEnabled", &Scene::IsUpdateEnabled)
        .def("SetTimeScale", &Scene::SetTimeScale)
        .def("SetElapsedTime", &Scene::SetElapsedTime);
        //"CreateAction", [](ActionType actionType, sol::variadic_args va) { return CreateAction(actionType, va); },

    nb::class_<ValueAnimation>(m, "ValueAnimation")
        .def(nb::init<Context*>())
        //         sol::call_constructor, sol::factories([context]() {
        //             // lua does not hold object, c++ manage object's life
        //             return new ValueAnimation(context);
        //             }));
        .def("SetSplineTension", &ValueAnimation::SetSplineTension)
        .def("SetInterpolationMethod", &ValueAnimation::SetInterpolationMethod)
        .def("SetKeyFrame", &ValueAnimation::SetKeyFrame)
        .def("SetEventFrame", [](ValueAnimation* self, float time, const StringHash& eventType) { self->SetEventFrame(time, eventType); })
        .def("SetEventFrame", [](ValueAnimation* self, float time, const StringHash& eventType, const VariantMap& eventData) { self->SetEventFrame(time, eventType, eventData); });
 
    nb::class_<ObjectAnimation>(m, "ObjectAnimation")
        .def(nb::init<Context*>())
        //sol::call_constructor, sol::factories([context]() { return new ObjectAnimation(context); }));
        .def("AddAttributeAnimation", [](ObjectAnimation* self, const ea::string& name, ValueAnimation* attributeAnimation) { self->AddAttributeAnimation(name, attributeAnimation); })
        .def("AddAttributeAnimation", [](ObjectAnimation* self, const ea::string& name, ValueAnimation* attributeAnimation, WrapMode wrapMode, float speed) { self->AddAttributeAnimation(name, attributeAnimation, wrapMode, speed); })
        .def("RemoveAttributeAnimation", [](ObjectAnimation* self, const ea::string& name) { self->RemoveAttributeAnimation(name); })
        .def("RemoveAttributeAnimation", [](ObjectAnimation* self, ValueAnimation* attributeAnimation) { self->RemoveAttributeAnimation(attributeAnimation); });
    
    nb::class_<PrefabReference, Component>(m, "PrefabReference")
        .def_ro_static("TypeId", &PrefabReference::TypeId)
        .def("SetPrefab", [](PrefabReference* self, PrefabResource* prefab) { self->SetPrefab(prefab); })
        .def("SetPrefab", [](PrefabReference* self, PrefabResource* prefab, const ea::string& path) { self->SetPrefab(prefab, path); })
        .def("SetPrefab", [](PrefabReference* self, PrefabResource* prefab, const ea::string& path, bool createInstance) { self->SetPrefab(prefab, path, createInstance); })
        .def("SetPrefab", [](PrefabReference* self, PrefabResource* prefab, const ea::string& path, bool createInstance, PrefabInstanceFlags instanceFlags) { self->SetPrefab(prefab, path, createInstance, instanceFlags); })
        .def("Inline", &PrefabReference::Inline)
        .def("InlineConservative", &PrefabReference::InlineConservative)
        .def("InlineAggressive", &PrefabReference::InlineAggressive);

    nb::class_<SplinePath, Component>(m, "SplinePath")
        .def_ro_static("TypeId", &SplinePath::TypeId)
        .def("SetSpeed", &SplinePath::SetSpeed)
        .def("SetPosition", &SplinePath::SetPosition)
        .def("GetPoint", &SplinePath::GetPoint)
        .def("GetControlledNode", &SplinePath::GetControlledNode)
        .def("SetControlledNode", &SplinePath::SetControlledNode)
        .def("GetInterpolationMode", &SplinePath::GetInterpolationMode)
        .def("SetInterpolationMode", &SplinePath::SetInterpolationMode)
        .def("Move", &SplinePath::Move)
        .def("Reset", &SplinePath::Reset)
        .def("IsFinished", &SplinePath::IsFinished)
        .def("SetControlPointIdsAttr", &SplinePath::SetControlPointIdsAttr)
        .def("ClearControlPoints", &SplinePath::ClearControlPoints)
        .def("RemoveControlPoint", &SplinePath::RemoveControlPoint)
        .def("AddControlPoint", [](SplinePath* self, Node* point) { self->AddControlPoint(point); })
        .def("AddControlPoint", [](SplinePath* self, Node* point, unsigned index) { self->AddControlPoint(point, index); });

    nb::class_<RmlUIComponent, LogicComponent>(m, "RmlUIComponent")
        .def_ro_static("TypeId", &RmlUIComponent::TypeId)
        .def_prop_rw("position", &RmlUIComponent::GetPosition, &RmlUIComponent::SetPosition)
        .def_prop_rw("size", &RmlUIComponent::GetSize, &RmlUIComponent::SetSize)
        .def_prop_rw("auto_size", &RmlUIComponent::GetAutoSize, &RmlUIComponent::SetAutoSize)
        //.def("SetResource", [&lua](RmlUIComponent* self, const ea::string& res, sol::table t) { lua["rmlui"]["uicontext"][res.c_str()] = t; self->SetResource(res); };//sol::resolve<void(const ea::string&)>(&RmlUIComponent::SetResource),//
        .def("GetResource", [](RmlUIComponent* self) { auto& res = self->GetResource(); return res.name_; })
        .def("GetDocument", &RmlUIComponent::GetDocument)
        //.def("AddUpdateListener", [](RmlUIComponent* self, sol::function func) { self->AddUpdateListener([func](float timeStep, Rml::ElementDocument* doc) { CALL_LUA(func, timeStep, doc) }); };
        .def("RemoveUpdateListener", [](RmlUIComponent* self) { self->AddUpdateListener(nullptr); })
        .def("GetRenderTexture", &RmlUIComponent::GetRenderTexture);

    nb::enum_<PrefabInstanceFlag>(m, "PrefabInstanceFlag", nb::is_flag())
        .value("None", PrefabInstanceFlag::None)
        .value("UpdateName", PrefabInstanceFlag::UpdateName)
        .value("UpdateTags", PrefabInstanceFlag::UpdateTags)
        .value("UpdatePosition", PrefabInstanceFlag::UpdatePosition)
        .value("UpdateRotation", PrefabInstanceFlag::UpdateRotation)
        .value("UpdateScale", PrefabInstanceFlag::UpdateScale)
        .value("UpdateVariables", PrefabInstanceFlag::UpdateVariables)
        .value("UpdateAll", PrefabInstanceFlag::UpdateAll);

   RegisterSceneConst(m);
}
