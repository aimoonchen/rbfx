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
#include "../../Urho2D/StaticSprite2D.h"
#include "../../Urho2D/AnimatedSprite2D.h"
#include "../../Urho2D/TileMapLayer2D.h"
#include "../../Urho2D/TileMap2D.h"
#include "../../Urho2D/StretchableSprite2D.h"

#include "GetPush.h"
#include "../LuaScriptInstance.h"

using namespace Urho3D;

namespace {
ea::unordered_map<Urho3D::StringHash, ea::function<int(lua_State* L, const Component* obj)>> component_convert_map = {
    {EffekseerEmitter::GetTypeStatic(),         [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const EffekseerEmitter*>(obj)).push(L); }},
    {Octree::GetTypeStatic(),                   [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const Octree*>(obj)).push(L); }},
    {DebugRenderer::GetTypeStatic(),            [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const DebugRenderer*>(obj)).push(L); }},
    {MeshLine::GetTypeStatic(),                 [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const MeshLine*>(obj)).push(L); }},
    {Camera::GetTypeStatic(),                   [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const Camera*>(obj)).push(L); }},
    {Zone::GetTypeStatic(),                     [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const Zone*>(obj)).push(L); }},
    {Light::GetTypeStatic(),                    [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const Light*>(obj)).push(L); }},
    {StaticModel::GetTypeStatic(),              [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const StaticModel*>(obj)).push(L); }},
    {AnimatedModel::GetTypeStatic(),            [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const AnimatedModel*>(obj)).push(L); }},
    {Skybox::GetTypeStatic(),                   [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const Skybox*>(obj)).push(L); }},
    {DecalSet::GetTypeStatic(),                 [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const DecalSet*>(obj)).push(L); }},
    {BillboardSet::GetTypeStatic(),             [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const BillboardSet*>(obj)).push(L); }},
    {Terrain::GetTypeStatic(),                  [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const Terrain*>(obj)).push(L); }},
    {RenderPipeline::GetTypeStatic(),           [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const RenderPipeline*>(obj)).push(L); }},
    {ProceduralSky::GetTypeStatic(),            [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const ProceduralSky*>(obj)).push(L); }},
    {OutlineGroup::GetTypeStatic(),             [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const OutlineGroup*>(obj)).push(L); }},
    {Text3D::GetTypeStatic(),                   [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const Text3D*>(obj)).push(L); }},
    {NavigationMesh::GetTypeStatic(),           [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const NavigationMesh*>(obj)).push(L); }},
    {DynamicNavigationMesh::GetTypeStatic(),    [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const DynamicNavigationMesh*>(obj)).push(L); }},
    {CrowdAgent::GetTypeStatic(),               [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const CrowdAgent*>(obj)).push(L); }},
    {CrowdManager::GetTypeStatic(),             [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const CrowdManager*>(obj)).push(L); }},
    {Navigable::GetTypeStatic(),                [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const Navigable*>(obj)).push(L); }},
    {Obstacle::GetTypeStatic(),                 [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const Obstacle*>(obj)).push(L); }},
    {OffMeshConnection::GetTypeStatic(),        [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const OffMeshConnection*>(obj)).push(L); }},
    {RigidBody::GetTypeStatic(),                [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const RigidBody*>(obj)).push(L); }},
    {CollisionShape::GetTypeStatic(),           [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const CollisionShape*>(obj)).push(L); }},
    {Constraint::GetTypeStatic(),               [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const Constraint*>(obj)).push(L); }},
    {PhysicsWorld::GetTypeStatic(),             [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const PhysicsWorld*>(obj)).push(L); }},
    {StaticSprite2D::GetTypeStatic(),           [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const StaticSprite2D*>(obj)).push(L); }},
    {AnimatedSprite2D::GetTypeStatic(),         [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const AnimatedSprite2D*>(obj)).push(L); }},
    {StretchableSprite2D::GetTypeStatic(),      [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const StretchableSprite2D*>(obj)).push(L); }},
    {PrefabReference::GetTypeStatic(),          [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const PrefabReference*>(obj)).push(L); }},
    {RmlUIComponent::GetTypeStatic(),           [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const RmlUIComponent*>(obj)).push(L); }},
    {TileMapLayer2D::GetTypeStatic(),           [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const TileMapLayer2D*>(obj)).push(L); }},
    {TileMap2D::GetTypeStatic(),                [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const TileMap2D*>(obj)).push(L); }}
};
} // namespace

template <typename T> void RegisterComponentConverter()
{
    component_convert_map.insert({T::GetTypeStatic(), [](lua_State* L, const Component* obj) { return sol::make_object(L, static_cast<const T*>(obj)).push(L); }});
}

namespace sol {
    int sol_lua_push(sol::types<Component*>, lua_State* L, const Component* obj)
    {
        if (obj) {
            if (auto it = component_convert_map.find(obj->GetType()); it != component_convert_map.end()) {
                auto& [typeKey, converter] = *it;
                return converter(L, obj);
            } else {
                URHO3D_LOGERRORF("sol_lua_push error, can't find convert function for %s.", obj->GetTypeName());
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
    auto bindComponent = lua.new_usertype<Component>("Component");
    bindComponent["GetID"]              = &Component::GetID;
    bindComponent["SetEnabled"]         = &Component::SetEnabled;
    bindComponent["IsEnabled"]          = &Component::IsEnabled;
    bindComponent["GetNode"]            = &Component::GetNode;
    bindComponent["GetScene"]           = &Component::GetScene;
    bindComponent["Remove"]             = &Component::Remove;
    bindComponent["DrawDebugGeometry"]  = &Component::DrawDebugGeometry;
    bindComponent["SetAttribute"]       = [](Component* self, const ea::string& name, const Variant& value) { self->SetAttribute(name, value); };
    bindComponent["SendEvent"]          = sol::overload(
        [](Component* self, StringHash name) { self->SendEvent(name); },
        [](Component* self, StringHash name, VariantMap& param) { self->SendEvent(name, param); });
        //         "SetAnimationEnabled", &Animatable::SetAnimationEnabled,
        //         "SetAnimationTime", &Animatable::SetAnimationTime,
        //         "SetObjectAnimation", &Animatable::SetObjectAnimation,
        //         "SetAttributeAnimationWrapMode", &Animatable::SetAttributeAnimationWrapMode,
        //         "SetAttributeAnimationSpeed", &Animatable::SetAttributeAnimationSpeed,
        //         "SetAttributeAnimationTime", &Animatable::SetAttributeAnimationTime,
        //         "RemoveObjectAnimation", &Animatable::RemoveObjectAnimation,
        //         "RemoveAttributeAnimation", &Animatable::RemoveAttributeAnimation

    auto bindLogicComponent = lua.new_usertype<LogicComponent>("LogicComponent", sol::base_classes, sol::bases<Component>());
    bindLogicComponent["GetID"]                 = &LogicComponent::GetID;
    bindLogicComponent["SetUpdateEventMask"]    = &LogicComponent::SetUpdateEventMask;
    bindLogicComponent["GetUpdateEventMask"]    = &LogicComponent::GetUpdateEventMask;
    
    lua.new_enum("TransformSpace",
        "LOCAL",    TS_LOCAL,
        "PARENT",   TS_PARENT,
        "WORLD",    TS_WORLD);

    auto bindNode = lua.new_usertype<Node>("Node", sol::call_constructor, sol::factories([context]() { return std::make_unique<Node>(context); }));
    bindNode["id"]                  = sol::property(&Node::GetID, &Node::SetID);
    bindNode["name"]                = sol::property(&Node::GetName, &Node::SetName);
    bindNode["parent"]              = sol::property(&Node::GetParent, &Node::SetParent);
    bindNode["scale"]               = sol::property(&Node::GetScale, sol::resolve<void(const Vector3&)>(&Node::SetScale));
    bindNode["rotation"]            = sol::property(&Node::GetRotation, &Node::SetRotation);
    bindNode["position"]            = sol::property(&Node::GetPosition, &Node::SetPosition);
    bindNode["direction"]           = sol::property(&Node::GetDirection, &Node::SetDirection);
    bindNode["local_matrix"]        = sol::property(&Node::GetTransformMatrix, &Node::SetTransformMatrix);
    bindNode["world_scale"]         = sol::property(&Node::GetWorldScale, sol::resolve<void(const Vector3&)>(&Node::SetWorldScale));
    bindNode["world_rotation"]      = sol::property(&Node::GetWorldRotation, &Node::SetWorldRotation);
    bindNode["world_position"]      = sol::property(&Node::GetWorldPosition, &Node::SetWorldPosition);
    bindNode["world_direction"]     = sol::property(&Node::GetWorldDirection, &Node::SetWorldDirection);
    bindNode["world_up"]            = sol::readonly_property(&Node::GetWorldUp);
    bindNode["world_right"]         = sol::readonly_property(&Node::GetWorldRight);
    bindNode["world_matrix"]        = sol::property(&Node::GetWorldTransform, sol::resolve<void(const Matrix3x4&)>(&Node::SetWorldTransform));
    bindNode["SetScale2D"]          = sol::overload(
        [](Node* self, const Vector2& scale) { self->SetScale2D(scale); },
        [](Node* self, float x, float y) { self->SetScale2D(x, y); });
    bindNode["SetRotation2D"]       = &Node::SetRotation2D;
    bindNode["SetPosition2D"]       = sol::overload(
        [](Node* self, const Vector2& position) { self->SetPosition2D(position); },
        [](Node* self, float x, float y) { self->SetPosition2D(x, y); });
    bindNode["SetTransform"]        = sol::overload(
        [](Node* self, const Vector3& position, const Quaternion& rotation) { self->SetTransform(position, rotation); },
        [](Node* self, const Vector3& position, const Quaternion& rotation, float scale) { self->SetTransform(position, rotation, scale); },
        [](Node* self, const Vector3& position, const Quaternion& rotation, const Vector3& scale) { self->SetTransform(position, rotation, scale); });
    bindNode["SetWorldTransform"]        = sol::overload(
        [](Node* self, const Vector3& position, const Quaternion& rotation) { self->SetWorldTransform(position, rotation); },
        [](Node* self, const Vector3& position, const Quaternion& rotation, float scale) { self->SetWorldTransform(position, rotation, scale); },
        [](Node* self, const Vector3& position, const Quaternion& rotation, const Vector3& scale) { self->SetWorldTransform(position, rotation, scale); });
    bindNode["SetTransform2D"]        = sol::overload(
        [](Node* self, const Vector2& position, float rotation) { self->SetTransform2D(position, rotation); },
        [](Node* self, const Vector2& position, float rotation, float scale) { self->SetTransform2D(position, rotation, scale); },
        [](Node* self, const Vector2& position, float rotation, const Vector2& scale) { self->SetTransform2D(position, rotation, scale); });
    bindNode["SetWorldTransform2D"]        = sol::overload(
        [](Node* self, const Vector2& position, float rotation) { self->SetWorldTransform2D(position, rotation); },
        [](Node* self, const Vector2& position, float rotation, float scale) { self->SetWorldTransform2D(position, rotation, scale); },
        [](Node* self, const Vector2& position, float rotation, const Vector2& scale) { self->SetWorldTransform2D(position, rotation, scale); });
    bindNode["AddTag"]              = &Node::AddTag;
    bindNode["SetTags"]             = [](Node* self, const ea::string& tags) {
        self->RemoveAllTags();
        self->AddTags(tags); };
    bindNode["AddTags"]             = sol::overload(
        [](Node* self, const ea::string& tags) { self->AddTags(tags); },
        [](Node* self, const ea::string& tags, char separator) { self->AddTags(tags, separator); });
    bindNode["GetTags"]             = [](Node* self) {
        const auto& rawtags = self->GetTags();
        // TODO:
        std::vector<std::string> tags;
        for (const auto& tag : rawtags) {
            tags.emplace_back(tag.c_str());
        }
        return tags;
    };
    bindNode["HasTag"]              = &Node::HasTag;
    bindNode["RemoveTag"]           = &Node::RemoveTag;
    bindNode["RemoveAllTags"]       = &Node::RemoveAllTags;
    bindNode["SetScale"]            = sol::overload(sol::resolve<void(float)>(&Node::SetScale), [](Node* self, float sx, float sy, float sz) { self->SetScale({ sx, sy, sz }); }, sol::resolve<void(const Vector3&)>(&Node::SetScale));
    bindNode["SetWorldScale"]       = sol::overload(sol::resolve<void(float)>(&Node::SetWorldScale), [](Node* self, float sx, float sy, float sz) { self->SetWorldScale({ sx, sy, sz }); }, sol::resolve<void(const Vector3&)>(&Node::SetWorldScale));
    bindNode["SetRotation"]         = sol::overload([](Node* self, const Quaternion& rot) { self->SetRotation(rot); }, [](Node* self, float rx, float ry, float rz) { self->SetRotation({ rx, ry, rz }); });
    bindNode["SetPosition"]         = sol::overload([](Node* self, const Vector3& pos) { self->SetPosition(pos); }, [](Node* self, float x, float y, float z) { self->SetPosition({ x, y, z }); });
    bindNode["SetDirection"]        = sol::overload([](Node* self, const Vector3& dir) { self->SetDirection(dir); }, [](Node* self, float dx, float dy, float dz) { self->SetDirection({ dx, dy, dz }); });
    bindNode["Rotate"]              = sol::overload(
        [](Node* self, float rx, float ry, float rz) { self->Rotate({ rx, ry, rz }); },
        [](Node* self, float rx, float ry, float rz, TransformSpace ts) { self->Rotate({ rx, ry, rz }, ts); },
        [](Node* self, const Quaternion& rotation) { self->Rotate(rotation); },
        [](Node* self, const Quaternion& rotation, TransformSpace ts) { self->Rotate(rotation, ts); });
    bindNode["Rotate2D"]            = sol::overload(
        [](Node* self, float delta) { self->Rotate2D(delta); },
        [](Node* self, float delta, TransformSpace space) { self->Rotate2D(delta, space); });
    bindNode["RotateAround"]        = sol::overload(
        [](Node* self, const Vector3& point, float rx, float ry, float rz) { self->RotateAround(point, { rx, ry, rz }); },
        [](Node* self, const Vector3& point, float rx, float ry, float rz, TransformSpace ts) { self->RotateAround(point, { rx, ry, rz }, ts); },
        [](Node* self, const Vector3& point, const Quaternion& rotation) { self->RotateAround(point, rotation); },
        [](Node* self, const Vector3& point, const Quaternion& rotation, TransformSpace ts) { self->RotateAround(point, rotation, ts); });
    bindNode["RotateAround2D"] = sol::overload(
        [](Node* self, const Vector2& point, float delta) { self->RotateAround2D(point, delta); },
        [](Node* self, const Vector2& point, float delta, TransformSpace space) { self->RotateAround2D(point, delta, space); });
    bindNode["Translate"]           = sol::overload(
        [](Node* self, float x, float y, float z) { self->Translate({ x, y, z }); },
        [](Node* self, float x, float y, float z, TransformSpace ts) { self->Translate({ x, y, z }, ts); },
        [](Node* self, const Vector3& translate) { self->Translate(translate); },
        [](Node* self, const Vector3& translate, TransformSpace ts) { self->Translate(translate, ts); });
    bindNode["Translate2D"]         = sol::overload(
        [](Node* self, const Vector2& delta) { self->Translate2D(delta); },
        [](Node* self, const Vector2& delta, TransformSpace space) { self->Translate2D(delta, space); });
    bindNode["Pitch"]               = sol::overload(
        [](Node* self, float angle) { self->Pitch(angle); },
        [](Node* self, float angle, TransformSpace ts) { self->Pitch(angle, ts); });
    bindNode["Yaw"]                 = sol::overload(
        [](Node* self, float angle) { self->Yaw(angle); },
        [](Node* self, float angle, TransformSpace ts) { self->Yaw(angle, ts); });
    bindNode["Roll"]                = sol::overload(
        [](Node* self, float angle) { self->Roll(angle); },
        [](Node* self, float angle, TransformSpace ts) { self->Roll(angle, ts); });
    bindNode["LookAt"]              = sol::overload(
        [](Node* self, const Vector3& target) { return self->LookAt(target); },
        [](Node* self, const Vector3& target, const Vector3& up) { return self->LookAt(target, up); },
        [](Node* self, const Vector3& target, const Vector3& up, TransformSpace ts) { return self->LookAt(target, up, ts); });
    bindNode["Scale"]                = sol::overload(
        [](Node* self, float scale) { self->Scale(scale); },
        [](Node* self, const Vector3& scale) { self->Scale(scale); });
    bindNode["ScaleAround"]                = sol::overload(
        [](Node* self, const Vector3& point, const Vector3& scale) { self->ScaleAround(point, scale); },
        [](Node* self, const Vector3& point, const Vector3& scale, TransformSpace space) { self->ScaleAround(point, scale, space); });
    bindNode["Scale2D"]             = &Node::Scale2D;
    bindNode["GetParent"]           = &Node::GetParent;
    bindNode["SetParent"]           = &Node::SetParent;
    bindNode["GetScene"]            = &Node::GetScene;
    bindNode["CreateChild"]         = sol::overload(
        [](Node* self) { return self->CreateChild(); },
        [](Node* self, const ea::string& name) { return self->CreateChild(name); },
        [](Node* self, const ea::string& name, unsigned id) { return self->CreateChild(name, id); },
        [](Node* self, const ea::string& name, unsigned id, bool temporary) { return self->CreateChild(name, id, temporary); });
    bindNode["Clone"]               = sol::overload(
        [](Node* self) { return self->Clone(); },
        [](Node* self, Node* parent) { return self->Clone(parent); });
    bindNode["CreateComponent"]     = [](Node* self, StringHash type) { return self->CreateComponent(type); };
    bindNode["GetComponent"]        = sol::overload([](Node* self, StringHash type) { return self->GetComponent(type); }, [](Node* self, StringHash type, bool recursive) { return self->GetComponent(type, recursive); });
    bindNode["GetComponents"]       = sol::overload(
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
            return stddest; });
    bindNode["RemoveComponent"]     = sol::overload([](Node* self, StringHash type) { self->RemoveComponent(type); }, [](Node* self, Component* component) { self->RemoveComponent(component); });
    bindNode["RemoveComponents"]    = [](Node* self, StringHash type) { self->RemoveComponents(type); };
    bindNode["RemoveAllComponents"] = &Node::RemoveAllComponents;
    bindNode["CreateScriptObject"]  = [](Node* self, const ea::string& name) {
        auto instance = self->CreateComponent<LuaScriptInstance>();
        instance->CreateObject(name);
        return instance->GetScriptObject();
    };
    bindNode["SetEnabled"]          = sol::resolve<void(bool)>(&Node::SetEnabled);
    bindNode["SetEnabledRecursive"] = &Node::SetEnabledRecursive;
    bindNode["AddChild"]            = [](Node* self, Node* obj) { self->AddChild(obj); };
    bindNode["RemoveChild"]         = sol::resolve<void(Node*)>(&Node::RemoveChild);
    bindNode["Remove"]              = &Node::Remove;
    bindNode["RemoveChildren"]      = &Node::RemoveChildren;
    bindNode["GetChildren"]         = [](Node* self, bool recursive) {
        auto children = self->GetChildren(recursive);
        std::vector<Node*> ret;
        for (auto& child : children) {
            ret.push_back(child);
        }
        return ret;
    };
    bindNode["GetChild"]                = sol::overload(
        [](Node* self, unsigned index) { return self->GetChild(index); },
        [](Node* self, const ea::string& name) { return self->GetChild(name); },
        [](Node* self, const ea::string& name, bool recursive) { return self->GetChild(name, recursive); });
    bindNode["GetNumChildren"]          = sol::overload(
        [](Node* self) { return self->GetNumChildren(); },
        [](Node* self, bool recursive) { return self->GetNumChildren(recursive); });
    bindNode["SendEvent"]               = sol::overload(
        [](Node* self, StringHash name) { self->SendEvent(name); },
        [](Node* self, StringHash name, VariantMap& param) { self->SendEvent(name, param); });
        //         "SetAnimationEnabled", &Animatable::SetAnimationEnabled,
        //         "SetAnimationTime", &Animatable::SetAnimationTime,
        //         "SetObjectAnimation", &Animatable::SetObjectAnimation,
        //         "SetAttributeAnimationWrapMode", &Animatable::SetAttributeAnimationWrapMode,
        //         "SetAttributeAnimationSpeed", &Animatable::SetAttributeAnimationSpeed,
        //         "SetAttributeAnimationTime", &Animatable::SetAttributeAnimationTime,
        //         "RemoveObjectAnimation", &Animatable::RemoveObjectAnimation,
        //         "RemoveAttributeAnimation", &Animatable::RemoveAttributeAnimation
    bindNode["SetVar"]                  = &Node::SetVar;
    bindNode["SetVarByHash"]            = &Node::SetVarByHash;
    bindNode["GetVar"]                  = &Node::GetVar;
    bindNode["GetVarByHash"]            = &Node::GetVarByHash;

    auto bindScene = lua.new_usertype<Scene>("Scene",
        sol::call_constructor, sol::factories([context]() { return std::make_unique<Scene>(context); }),
        sol::base_classes, sol::bases<Node>());
    bindScene["LoadXML"]            = [](Scene* self, XMLFile* file) { return self->LoadXML(file->GetRoot()); }; // sol::resolve<bool(Deserializer&)>(&Scene::LoadXML),
    bindScene["GetNode"]            = &Scene::GetNode;
    bindScene["GetNodesWithTag"]    = [](Scene* self, const ea::string& tag) {
        std::vector<Node*> ret;
        ea::vector<Node*> dest;
        if (self->GetNodesWithTag(dest, tag)) {
            ret.reserve(dest.size());
            for (auto nd : dest) {
                ret.push_back(nd);
            }
        }
        return ret;
    };
    bindScene["Clear"]              = &Scene::Clear;
    bindScene["SetUpdateEnabled"]   = &Scene::SetUpdateEnabled;
    bindScene["IsUpdateEnabled"]    = &Scene::IsUpdateEnabled;
    bindScene["SetTimeScale"]       = &Scene::SetTimeScale;
    bindScene["SetElapsedTime"]     = &Scene::SetElapsedTime;
        //"CreateAction", [](ActionType actionType, sol::variadic_args va) { return CreateAction(actionType, va); },

    auto bindValueAnimation = lua.new_usertype<ValueAnimation>("ValueAnimation",
        sol::call_constructor, sol::factories([context]() {
            // lua does not hold object, c++ manage object's life
            return new ValueAnimation(context);
            }));
    bindValueAnimation["SetSplineTension"]          = &ValueAnimation::SetSplineTension;
    bindValueAnimation["SetInterpolationMethod"]    = &ValueAnimation::SetInterpolationMethod;
    bindValueAnimation["SetKeyFrame"]               = &ValueAnimation::SetKeyFrame;
    bindValueAnimation["SetEventFrame"]             = sol::overload(
        [](ValueAnimation* self, float time, const StringHash& eventType) { self->SetEventFrame(time, eventType); },
        [](ValueAnimation* self, float time, const StringHash& eventType, const VariantMap& eventData) { self->SetEventFrame(time, eventType, eventData); });
 
    auto bindObjectAnimation = lua.new_usertype<ObjectAnimation>("ObjectAnimation",
        sol::call_constructor, sol::factories([context]() { return new ObjectAnimation(context); }));
    bindObjectAnimation["AddAttributeAnimation"] = sol::overload(
        [](ObjectAnimation* self, const ea::string& name, ValueAnimation* attributeAnimation) { self->AddAttributeAnimation(name, attributeAnimation); },
        [](ObjectAnimation* self, const ea::string& name, ValueAnimation* attributeAnimation, WrapMode wrapMode, float speed) { self->AddAttributeAnimation(name, attributeAnimation, wrapMode, speed); });
    bindObjectAnimation["RemoveAttributeAnimation"] = sol::overload(
        [](ObjectAnimation* self, const ea::string& name) { self->RemoveAttributeAnimation(name); },
        [](ObjectAnimation* self, ValueAnimation* attributeAnimation) { self->RemoveAttributeAnimation(attributeAnimation); });
    
    auto bindPrefabReference = lua.new_usertype<PrefabReference>("PrefabReference", sol::base_classes, sol::bases<Component>());
    bindPrefabReference["id"]                   = sol::var(StringHash("PrefabReference"));
    bindPrefabReference["SetPrefab"]            = sol::overload(
        [](PrefabReference* self, PrefabResource* prefab) { self->SetPrefab(prefab); },
        [](PrefabReference* self, PrefabResource* prefab, const ea::string& path) { self->SetPrefab(prefab, path); },
        [](PrefabReference* self, PrefabResource* prefab, const ea::string& path, bool createInstance) { self->SetPrefab(prefab, path, createInstance); },
        [](PrefabReference* self, PrefabResource* prefab, const ea::string& path, bool createInstance, PrefabInstanceFlags instanceFlags) { self->SetPrefab(prefab, path, createInstance, instanceFlags); });
    bindPrefabReference["Inline"]               = &PrefabReference::Inline;
    bindPrefabReference["InlineConservative"]   = &PrefabReference::InlineConservative;
    bindPrefabReference["InlineAggressive"]     = &PrefabReference::InlineAggressive;

    auto bindSplinePath = lua.new_usertype<SplinePath>("SplinePath", sol::base_classes, sol::bases<Component>());
    bindSplinePath["id"]                        = sol::var(StringHash("SplinePath"));
    bindSplinePath["SetSpeed"]                  = &SplinePath::SetSpeed;
    bindSplinePath["SetPosition"]               = &SplinePath::SetPosition;
    bindSplinePath["GetPoint"]                  = &SplinePath::GetPoint;
    bindSplinePath["GetControlledNode"]         = &SplinePath::GetControlledNode;
    bindSplinePath["SetControlledNode"]         = &SplinePath::SetControlledNode;
    bindSplinePath["GetInterpolationMode"]      = &SplinePath::GetInterpolationMode;
    bindSplinePath["SetInterpolationMode"]      = &SplinePath::SetInterpolationMode;
    bindSplinePath["Move"]                      = &SplinePath::Move;
    bindSplinePath["Reset"]                     = &SplinePath::Reset;
    bindSplinePath["IsFinished"]                = &SplinePath::IsFinished;
    bindSplinePath["SetControlPointIdsAttr"]    = &SplinePath::SetControlPointIdsAttr;
    bindSplinePath["ClearControlPoints"]        = &SplinePath::ClearControlPoints;
    bindSplinePath["RemoveControlPoint"]        = &SplinePath::RemoveControlPoint;
    bindSplinePath["AddControlPoint"]           = sol::overload(
        [](SplinePath* self, Node* point) { self->AddControlPoint(point); },
        [](SplinePath* self, Node* point, unsigned index) { self->AddControlPoint(point, index); });

    auto bindRmlUIComponent = lua.new_usertype<RmlUIComponent>("RmlUIComponent", sol::base_classes, sol::bases<LogicComponent, Component>());
    bindRmlUIComponent["id"]                    = sol::var(StringHash("RmlUIComponent"));
    bindRmlUIComponent["position"]              = sol::property(&RmlUIComponent::GetPosition, &RmlUIComponent::SetPosition);
    bindRmlUIComponent["size"]                  = sol::property(&RmlUIComponent::GetSize, &RmlUIComponent::SetSize);
    bindRmlUIComponent["auto_size"]             = sol::property(&RmlUIComponent::GetAutoSize, &RmlUIComponent::SetAutoSize);
    bindRmlUIComponent["SetResource"]           = [&lua](RmlUIComponent* self, const ea::string& res, sol::table t) { lua["rmlui"]["uicontext"][res.c_str()] = t; self->SetResource(res); };//sol::resolve<void(const ea::string&)>(&RmlUIComponent::SetResource),//
    bindRmlUIComponent["GetResource"]           = [](RmlUIComponent* self) { auto& res = self->GetResource(); return res.name_; };
    bindRmlUIComponent["GetDocument"]           = &RmlUIComponent::GetDocument;
    bindRmlUIComponent["AddUpdateListener"]     = [](RmlUIComponent* self, sol::function func) { self->AddUpdateListener([func](float timeStep, Rml::ElementDocument* doc) { CALL_LUA(func, timeStep, doc) }); };
    bindRmlUIComponent["RemoveUpdateListener"]  = [](RmlUIComponent* self) { self->AddUpdateListener(nullptr); };
    bindRmlUIComponent["GetRenderTexture"]      = &RmlUIComponent::GetRenderTexture;

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
