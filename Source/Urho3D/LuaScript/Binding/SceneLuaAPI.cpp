#include <RmlUi/Core/ElementDocument.h>
#include "../../Core/Context.h"
#include "../../Scene/Node.h"
#include "../../Scene/Scene.h"
#include "../../Scene/Component.h"
#include "../../Scene/LogicComponent.h"
#include "../../RmlUI/RmlUIComponent.h"
#include "../../Scene/ValueAnimation.h"
#include "../../Scene/ObjectAnimation.h"
#include "../../Scene/PrefabReference.h"
#include "../../Scene/SplinePath.h"
#include "../../IO/File.h"
#include "../../Graphics/Light.h"
#include "../../Graphics/StaticModel.h"
#include "../../Graphics/AnimatedModel.h"
#include "../../Graphics/AnimationController.h"
#include "../../Graphics/Skybox.h"
#include "../../Graphics/Octree.h"
#include "../../Graphics/Camera.h"
#include "../../Graphics/Zone.h"
#include "../../Graphics/DebugRenderer.h"
#include "../../Graphics/MeshLine.h"
#include "../../Graphics/DecalSet.h"
#include "../../Graphics/BillboardSet.h"
#include "../../Graphics/Terrain.h"
#include "../../Graphics/ProceduralSky.h"
#include "../../Graphics/OutlineGroup.h"
#include "../../Graphics/Texture2D.h"
#include "../../UI/Text3D.h"
#include "../../RenderPipeline/RenderPipeline.h"

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

#include "GetPush.h"
#include "../LuaScriptInstance.h"

using namespace Urho3D;

namespace sol {
    int sol_lua_push(sol::types<Component*>, lua_State* L, const Component* obj)
    {
        if (obj) {
            const auto& objType = obj->GetType();
            if (objType == StaticModel::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const StaticModel*>(obj)).push(L);
            } else if (objType == AnimatedModel::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const AnimatedModel*>(obj)).push(L);
            } else if (objType == AnimationController::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const AnimationController*>(obj)).push(L);
            } else if (objType == Light::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const Light*>(obj)).push(L);
            } else if (objType == Octree::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const Octree*>(obj)).push(L);
            } else if (objType == Camera::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const Camera*>(obj)).push(L);
            } else if (objType == DebugRenderer::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const DebugRenderer*>(obj)).push(L);
            } else if (objType == Zone::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const Zone*>(obj)).push(L);
            } else if (objType == Skybox::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const Skybox*>(obj)).push(L);
#if defined(__linux__) && !defined(__ANDROID__)
#else
            } else if (objType == EffekseerEmitter::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const EffekseerEmitter*>(obj)).push(L);
#endif
            } else if (objType == RmlUIComponent::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const RmlUIComponent*>(obj)).push(L);
            } else if (objType == NavigationMesh::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const NavigationMesh*>(obj)).push(L);
            } else if (objType == DynamicNavigationMesh::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const DynamicNavigationMesh*>(obj)).push(L);
            } else if (objType == CrowdAgent::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const CrowdAgent*>(obj)).push(L);
            } else if (objType == CrowdManager::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const CrowdManager*>(obj)).push(L);
            } else if (objType == RigidBody::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const RigidBody*>(obj)).push(L);
            } else if (objType == CollisionShape::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const CollisionShape*>(obj)).push(L);
            } else if (objType == Constraint::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const Constraint*>(obj)).push(L);
            } else if (objType == PhysicsWorld::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const PhysicsWorld*>(obj)).push(L);
            } else if (objType == Terrain::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const Terrain*>(obj)).push(L);
            } else if (objType == DecalSet::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const DecalSet*>(obj)).push(L);
            } else if (objType == BillboardSet::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const BillboardSet*>(obj)).push(L);
            } else if (objType == ProceduralSky::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const ProceduralSky*>(obj)).push(L);
            } else if (objType == RenderPipeline::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const RenderPipeline*>(obj)).push(L);
            } else if (objType == Navigable::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const Navigable*>(obj)).push(L);
            } else if (objType == Obstacle::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const Obstacle*>(obj)).push(L);
            } else if (objType == OffMeshConnection::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const OffMeshConnection*>(obj)).push(L);
            } else if (objType == OutlineGroup::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const OutlineGroup*>(obj)).push(L);
            } else if (objType == Text3D::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const Text3D*>(obj)).push(L);
            } else if (objType == PrefabReference::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const PrefabReference*>(obj)).push(L);
            } else if (objType == MeshLine::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const MeshLine*>(obj)).push(L);
            }
        }
        return sol::make_object(L, obj).push(L);
    }
}

Urho3D::Context* GetContext(lua_State* L);

static void RegisterSceneConst(sol::state& lua)
{
//     auto& lua = *solLua;
//     auto eventType = lua["ComponentType"].get_or_create<sol::table>();
}

int sol2_SceneLuaAPI_open(sol::state& lua)
{
    auto context = GetContext(lua.lua_state());
    lua.new_usertype<Component>("Component",
        "GetID", &Component::GetID,
        "SetEnabled", &Component::SetEnabled,
        "IsEnabled", &Component::IsEnabled,
        "GetNode", &Component::GetNode,
        "GetScene", &Component::GetScene,
        "Remove", &Component::Remove,
        "DrawDebugGeometry", &Component::DrawDebugGeometry,
        "SetAttribute", [](Component* self, const ea::string& name, const Variant& value) { self->SetAttribute(name, value); },
        "SendEvent", sol::overload(
            [](Component* self, StringHash name) { self->SendEvent(name); },
            [](Component* self, StringHash name, VariantMap& param) { self->SendEvent(name, param); })
        //         "SetAnimationEnabled", &Animatable::SetAnimationEnabled,
        //         "SetAnimationTime", &Animatable::SetAnimationTime,
        //         "SetObjectAnimation", &Animatable::SetObjectAnimation,
        //         "SetAttributeAnimationWrapMode", &Animatable::SetAttributeAnimationWrapMode,
        //         "SetAttributeAnimationSpeed", &Animatable::SetAttributeAnimationSpeed,
        //         "SetAttributeAnimationTime", &Animatable::SetAttributeAnimationTime,
        //         "RemoveObjectAnimation", &Animatable::RemoveObjectAnimation,
        //         "RemoveAttributeAnimation", &Animatable::RemoveAttributeAnimation
    );

    lua.new_usertype<LogicComponent>("LogicComponent",
        "GetID", &LogicComponent::GetID,
        "SetUpdateEventMask", &LogicComponent::SetUpdateEventMask,
        "GetUpdateEventMask", &LogicComponent::GetUpdateEventMask,
        sol::base_classes, sol::bases<Component>()
    );

    lua.new_usertype<Node>("Node",
        sol::call_constructor, sol::factories([context]() { return std::make_unique<Node>(context); }),
        "id", sol::property(&Node::GetID, &Node::SetID),
        "name", sol::property(&Node::GetName, &Node::SetName),
        "parent", sol::property(&Node::GetParent, &Node::SetParent),
        "scale", sol::property(&Node::GetScale, sol::resolve<void(const Vector3&)>(&Node::SetScale)),
        "rotation", sol::property(&Node::GetRotation, &Node::SetRotation),
        "position", sol::property(&Node::GetPosition, &Node::SetPosition),
        "direction", sol::property(&Node::GetDirection, &Node::SetDirection),
        "local_matrix", sol::property(&Node::GetTransformMatrix, &Node::SetTransformMatrix),
        "world_scale", sol::property(&Node::GetWorldScale, sol::resolve<void(const Vector3&)>(&Node::SetWorldScale)),
        "world_rotation", sol::property(&Node::GetWorldRotation, &Node::SetWorldRotation),
        "world_position", sol::property(&Node::GetWorldPosition, &Node::SetWorldPosition),
        "world_direction", sol::property(&Node::GetWorldDirection, &Node::SetWorldDirection),
        "world_up", sol::property(&Node::GetWorldUp),
        "world_right", sol::property(&Node::GetWorldRight),
        "world_matrix", sol::property(&Node::GetWorldTransform, sol::resolve<void(const Matrix3x4&)>(&Node::SetWorldTransform)),
        "SetTransform", sol::overload(
            [](Node* self, const Vector3& position, const Quaternion& rotation) { self->SetTransform(position, rotation); },
            [](Node* self, const Vector3& position, const Quaternion& rotation, float scale) { self->SetTransform(position, rotation, scale); },
            [](Node* self, const Vector3& position, const Quaternion& rotation, const Vector3& scale) { self->SetTransform(position, rotation, scale); }),
        "AddTag", &Node::AddTag,
        "SetTags", [](Node* self, const ea::string& tags) {
            self->RemoveAllTags();
            self->AddTags(tags);
        },
        "AddTags", sol::overload(
            [](Node* self, const ea::string& tags) { self->AddTags(tags); },
            [](Node* self, const ea::string& tags, char separator) { self->AddTags(tags, separator); }),
        "GetTags", [](Node* self) {
            const auto& rawtags = self->GetTags();
            // TODO:
            std::vector<std::string> tags;
            for (const auto& tag : rawtags) {
                tags.emplace_back(tag.c_str());
            }
            return tags;
        },
        "HasTag", &Node::HasTag,
        "RemoveTag", &Node::RemoveTag,
        "RemoveAllTags", &Node::RemoveAllTags,
        "SetScale", sol::overload(sol::resolve<void(float)>(&Node::SetScale), [](Node* self, float sx, float sy, float sz) { self->SetScale({ sx, sy, sz }); }, sol::resolve<void(const Vector3&)>(&Node::SetScale)),
        "SetWorldScale", sol::overload(sol::resolve<void(float)>(&Node::SetWorldScale), [](Node* self, float sx, float sy, float sz) { self->SetWorldScale({ sx, sy, sz }); }, sol::resolve<void(const Vector3&)>(&Node::SetWorldScale)),
        "SetRotation", sol::overload([](Node* self, const Quaternion& rot) { self->SetRotation(rot); }, [](Node* self, float rx, float ry, float rz) { self->SetRotation({ rx, ry, rz }); }),
        "SetPosition", sol::overload([](Node* self, const Vector3& pos) { self->SetPosition(pos); }, [](Node* self, float x, float y, float z) { self->SetPosition({ x, y, z }); }),
        "SetDirection", sol::overload([](Node* self, const Vector3& dir) { self->SetDirection(dir); }, [](Node* self, float dx, float dy, float dz) { self->SetDirection({ dx, dy, dz }); }),
        "Rotate", sol::overload(
            [](Node* self, float rx, float ry, float rz) { self->Rotate({ rx, ry, rz }); },
            [](Node* self, float rx, float ry, float rz, TransformSpace ts) { self->Rotate({ rx, ry, rz }, ts); },
            [](Node* self, const Quaternion& rotation) { self->Rotate(rotation); },
            [](Node* self, const Quaternion& rotation, TransformSpace ts) { self->Rotate(rotation, ts); }),
        "RotateAround", sol::overload(
            [](Node* self, const Vector3& point, float rx, float ry, float rz) { self->RotateAround(point, { rx, ry, rz }); },
            [](Node* self, const Vector3& point, float rx, float ry, float rz, TransformSpace ts) { self->RotateAround(point, { rx, ry, rz }, ts); },
            [](Node* self, const Vector3& point, const Quaternion& rotation) { self->RotateAround(point, rotation); },
            [](Node* self, const Vector3& point, const Quaternion& rotation, TransformSpace ts) { self->RotateAround(point, rotation, ts); }),
        "Translate", sol::overload(
            [](Node* self, float x, float y, float z) { self->Translate({ x, y, z }); },
            [](Node* self, float x, float y, float z, TransformSpace ts) { self->Translate({ x, y, z }, ts); },
            [](Node* self, const Vector3& translate) { self->Translate(translate); },
            [](Node* self, const Vector3& translate, TransformSpace ts) { self->Translate(translate, ts); }),
        "Pitch", sol::overload(
            [](Node* self, float angle) { self->Pitch(angle); },
            [](Node* self, float angle, TransformSpace ts) { self->Pitch(angle, ts); }),
        "Yaw", sol::overload(
            [](Node* self, float angle) { self->Yaw(angle); },
            [](Node* self, float angle, TransformSpace ts) { self->Yaw(angle, ts); }),
        "Roll", sol::overload(
            [](Node* self, float angle) { self->Roll(angle); },
            [](Node* self, float angle, TransformSpace ts) { self->Roll(angle, ts); }),
        "LookAt", sol::overload(
            [](Node* self, const Vector3& target) { return self->LookAt(target); },
            [](Node* self, const Vector3& target, const Vector3& up) { return self->LookAt(target, up); },
            [](Node* self, const Vector3& target, const Vector3& up, TransformSpace ts) { return self->LookAt(target, up, ts); }),
        "GetParent", &Node::GetParent,
        "SetParent", &Node::SetParent,
        "GetScene", &Node::GetScene,
        "CreateChild", sol::overload(
            [](Node* self) { return self->CreateChild(); },
            [](Node* self, const ea::string& name) { return self->CreateChild(name); },
            [](Node* self, const ea::string& name, unsigned id) { return self->CreateChild(name, id); },
            [](Node* self, const ea::string& name, unsigned id, bool temporary) { return self->CreateChild(name, id, temporary); }),
        "Clone", sol::overload(
            [](Node* self) { return self->Clone(); },
            [](Node* self, Node* parent) { return self->Clone(parent); }),
        "CreateComponent", [](Node* self, StringHash type) { return self->CreateComponent(type); },
        "GetComponent", sol::overload([](Node* self, StringHash type) { return self->GetComponent(type); }, [](Node* self, StringHash type, bool recursive) { return self->GetComponent(type, recursive); }),
        "GetComponents", sol::overload(
            [](Node* self, StringHash type) {
                ea::vector<Component*> dest;
                self->GetComponents(dest, type);
                std::vector<Component*> stddest;
                stddest.reserve(dest.size());
                for (auto it : dest) {
                    stddest.emplace_back(it);
                }
                return stddest;
            },
            [](Node* self, StringHash type, bool recursive) {
                ea::vector<Component*> dest;
                self->GetComponents(dest, type, recursive);
                std::vector<Component*> stddest;
                stddest.reserve(dest.size());
                for (auto it : dest) {
                    stddest.emplace_back(it);
                }
                return stddest;
            }),
        "RemoveComponent", sol::overload([](Node* self, StringHash type) { self->RemoveComponent(type); }, [](Node* self, Component* component) { self->RemoveComponent(component); }),
        "RemoveComponents", [](Node* self, StringHash type) { self->RemoveComponents(type); },
        "RemoveAllComponents", &Node::RemoveAllComponents,
        "CreateScriptObject", [](Node* self, const ea::string& name) {
            auto instance = self->CreateComponent<LuaScriptInstance>();
            instance->CreateObject(name);
            return instance->GetScriptObject();
        },
        "SetEnabled", sol::resolve<void(bool)>(&Node::SetEnabled),
        "SetEnabledRecursive", &Node::SetEnabledRecursive,
        "AddChild", [](Node* self, Node* obj) { self->AddChild(obj); },
        "RemoveChild", sol::resolve<void(Node*)>(&Node::RemoveChild),
        "Remove", &Node::Remove,
        "RemoveChildren", &Node::RemoveChildren,
        "GetChildren", [](Node* self, bool recursive) {
                auto children = self->GetChildren(recursive);
                std::vector<Node*> ret;
                for (auto& child : children) {
                    ret.push_back(child);
                }
                return ret;
            },
        "GetChild", sol::overload(
            [](Node* self, unsigned index) { return self->GetChild(index); },
            [](Node* self, const ea::string& name) { return self->GetChild(name); },
            [](Node* self, const ea::string& name, bool recursive) { return self->GetChild(name, recursive); }),
        "GetNumChildren", sol::overload(
            [](Node* self) { return self->GetNumChildren(); },
            [](Node* self, bool recursive) { return self->GetNumChildren(recursive); }),
        "SendEvent", sol::overload(
            [](Node* self, StringHash name) { self->SendEvent(name); },
            [](Node* self, StringHash name, VariantMap& param) { self->SendEvent(name, param); })
        //         "SetAnimationEnabled", &Animatable::SetAnimationEnabled,
        //         "SetAnimationTime", &Animatable::SetAnimationTime,
        //         "SetObjectAnimation", &Animatable::SetObjectAnimation,
        //         "SetAttributeAnimationWrapMode", &Animatable::SetAttributeAnimationWrapMode,
        //         "SetAttributeAnimationSpeed", &Animatable::SetAttributeAnimationSpeed,
        //         "SetAttributeAnimationTime", &Animatable::SetAttributeAnimationTime,
        //         "RemoveObjectAnimation", &Animatable::RemoveObjectAnimation,
        //         "RemoveAttributeAnimation", &Animatable::RemoveAttributeAnimation
    );
    lua.new_usertype<Scene>("Scene",
        sol::call_constructor, sol::factories([context]() { return std::make_unique<Scene>(context); }),
        "LoadXML", [](Scene* self, XMLFile* file) { return self->LoadXML(file->GetRoot()); }, // sol::resolve<bool(Deserializer&)>(&Scene::LoadXML),
        "GetNode", &Scene::GetNode,
        "GetNodesWithTag", [](Scene* self, const ea::string& tag) {
            std::vector<Node*> ret;
            ea::vector<Node*> dest;
            if (self->GetNodesWithTag(dest, tag)) {
                ret.reserve(dest.size());
                for (auto nd : dest) {
                    ret.push_back(nd);
                }
            }
            return ret;
        },
        "Clear", &Scene::Clear,
        "SetUpdateEnabled", &Scene::SetUpdateEnabled,
        "IsUpdateEnabled", &Scene::IsUpdateEnabled,
        "SetTimeScale", &Scene::SetTimeScale,
        "SetElapsedTime", &Scene::SetElapsedTime,
        //"CreateAction", [](ActionType actionType, sol::variadic_args va) { return CreateAction(actionType, va); },
        sol::base_classes, sol::bases<Node>());

    lua.new_usertype<ValueAnimation>("ValueAnimation",
        sol::call_constructor, sol::factories([context]() {
            // lua does not hold object, c++ manage object's life
            return new ValueAnimation(context);
            }),
        "SetSplineTension", &ValueAnimation::SetSplineTension,
        "SetInterpolationMethod", &ValueAnimation::SetInterpolationMethod,
        "SetKeyFrame", &ValueAnimation::SetKeyFrame,
        "SetEventFrame", sol::overload(
            [](ValueAnimation* self, float time, const StringHash& eventType) { self->SetEventFrame(time, eventType); },
            [](ValueAnimation* self, float time, const StringHash& eventType, const VariantMap& eventData) { self->SetEventFrame(time, eventType, eventData); })
    );
    lua.new_usertype<ObjectAnimation>("ObjectAnimation",
        sol::call_constructor, sol::factories([context]() { return new ObjectAnimation(context); }),
        "AddAttributeAnimation", sol::overload(
            [](ObjectAnimation* self, const ea::string& name, ValueAnimation* attributeAnimation) { self->AddAttributeAnimation(name, attributeAnimation); },
            [](ObjectAnimation* self, const ea::string& name, ValueAnimation* attributeAnimation, WrapMode wrapMode, float speed) { self->AddAttributeAnimation(name, attributeAnimation, wrapMode, speed); }),
        "RemoveAttributeAnimation", sol::overload(
            [](ObjectAnimation* self, const ea::string& name) { self->RemoveAttributeAnimation(name); },
            [](ObjectAnimation* self, ValueAnimation* attributeAnimation) { self->RemoveAttributeAnimation(attributeAnimation); })
    );
    lua.new_usertype<PrefabReference>("PrefabReference",
        "id", sol::var(StringHash("PrefabReference")),
        "SetPrefab", sol::overload(
            [](PrefabReference* self, PrefabResource* prefab) { self->SetPrefab(prefab); },
            [](PrefabReference* self, PrefabResource* prefab, const ea::string& path) { self->SetPrefab(prefab, path); },
            [](PrefabReference* self, PrefabResource* prefab, const ea::string& path, bool createInstance) { self->SetPrefab(prefab, path, createInstance); },
            [](PrefabReference* self, PrefabResource* prefab, const ea::string& path, bool createInstance, PrefabInstanceFlags instanceFlags) { self->SetPrefab(prefab, path, createInstance, instanceFlags); }),
        "Inline", &PrefabReference::Inline,
        "InlineConservative", &PrefabReference::InlineConservative,
        "InlineAggressive", &PrefabReference::InlineAggressive,
        sol::base_classes, sol::bases<Component>());

    lua.new_usertype<SplinePath>("SplinePath",
        "id", sol::var(StringHash("SplinePath")),
        "SetSpeed", &SplinePath::SetSpeed,
        "SetPosition", &SplinePath::SetPosition,
        "GetPoint", &SplinePath::GetPoint,
        "GetControlledNode", &SplinePath::GetControlledNode,
        "SetControlledNode", &SplinePath::SetControlledNode,
        "GetInterpolationMode", &SplinePath::GetInterpolationMode,
        "SetInterpolationMode", &SplinePath::SetInterpolationMode,
        "Move", &SplinePath::Move,
        "Reset", &SplinePath::Reset,
        "IsFinished", &SplinePath::IsFinished,
        "SetControlPointIdsAttr", &SplinePath::SetControlPointIdsAttr,
        "ClearControlPoints", &SplinePath::ClearControlPoints,
        "RemoveControlPoint", &SplinePath::RemoveControlPoint,
        "AddControlPoint", sol::overload(
            [](SplinePath* self, Node* point) { self->AddControlPoint(point); },
            [](SplinePath* self, Node* point, unsigned index) { self->AddControlPoint(point, index); }),
        sol::base_classes, sol::bases<Component>());

    lua.new_usertype<RmlUIComponent>("RmlUIComponent",
        "id", sol::var(StringHash("RmlUIComponent")),
        "position", sol::property(&RmlUIComponent::GetPosition, &RmlUIComponent::SetPosition),
        "size", sol::property(&RmlUIComponent::GetSize, &RmlUIComponent::SetSize),
        "auto_size", sol::property(&RmlUIComponent::GetAutoSize, &RmlUIComponent::SetAutoSize),
        "SetResource", [&lua](RmlUIComponent* self, const ea::string& res, sol::table t) { lua["rmlui"]["uicontext"][res.c_str()] = t; self->SetResource(res); },//sol::resolve<void(const ea::string&)>(&RmlUIComponent::SetResource),//
        "GetResource", [](RmlUIComponent* self) { auto& res = self->GetResource(); return res.name_; },
        "GetDocument", &RmlUIComponent::GetDocument,
        "AddUpdateListener", [](RmlUIComponent* self, sol::function func) { self->AddUpdateListener([func](float timeStep, Rml::ElementDocument* doc) { CALL_LUA(func, timeStep, doc) }); },
        "RemoveUpdateListener", [](RmlUIComponent* self) { self->AddUpdateListener(nullptr); },
        "GetRenderTexture", &RmlUIComponent::GetRenderTexture,
        sol::base_classes, sol::bases<LogicComponent, Component>());

    lua.new_enum<PrefabInstanceFlag, true>("PrefabInstanceFlag", {
        { "None", PrefabInstanceFlag::None },
        { "UpdateName", PrefabInstanceFlag::UpdateName },
        { "UpdateTags", PrefabInstanceFlag::UpdateTags },
        { "UpdatePosition", PrefabInstanceFlag::UpdatePosition },
        { "UpdateRotation", PrefabInstanceFlag::UpdateRotation },
        { "UpdateScale", PrefabInstanceFlag::UpdateScale },
        { "UpdateVariables", PrefabInstanceFlag::UpdateVariables },
        { "UpdateAll", PrefabInstanceFlag::UpdateAll }});

    lua["WM_LOOP"]      = WM_LOOP;
    lua["WM_ONCE"]      = WM_ONCE;
    lua["WM_CLAMP"]     = WM_CLAMP;
    lua["IM_NONE"]      = IM_NONE;
    lua["IM_LINEAR"]    = IM_LINEAR;
    lua["IM_SPLINE"]    = IM_SPLINE;
    lua["TS_LOCAL"]     = TS_LOCAL;
    lua["TS_PARENT"]    = TS_PARENT;
    lua["TS_WORLD"]     = TS_WORLD;
//    lua["REPLICATED"]   = REPLICATED;
//    lua["LOCAL"]        = LOCAL;
    RegisterSceneConst(lua);
    return 0;
}
