#include <nanobind/nanobind.h>
#include "Urho3D/Core/Context.h"
#include "Urho3D/Physics2D/CollisionShape2D.h"
#include "Urho3D/Physics2D/RigidBody2D.h"
#include "Urho3D/Physics2D/PhysicsWorld2D.h"
#include "Urho3D/Physics2D/PhysicsEvents2D.h"
#include "Urho3D/Physics2D/CollisionBox2D.h"
#include "Urho3D/Physics2D/CollisionCircle2D.h"
#include "Urho3D/Physics2D/CollisionPolygon2D.h"
#include "Urho3D/Physics2D/CollisionChain2D.h"
#include "Urho3D/Physics2D/Constraint2D.h"

using namespace Urho3D;
namespace nb = nanobind;
using namespace nb::literals;

NB_MODULE(physics2d, m)
{
    auto eventType = m.def_submodule("EventType");
    eventType.attr("PhysicsBeginContact2D") = E_PHYSICSBEGINCONTACT2D;

    auto paramType = m.def_submodule("ParamType");
    auto physicsBeginContact2D = paramType.def_submodule("PhysicsBeginContact2D");
    physicsBeginContact2D.attr("World") = PhysicsBeginContact2D::P_WORLD;
    physicsBeginContact2D.attr("BodyA") = PhysicsBeginContact2D::P_BODYA;
    physicsBeginContact2D.attr("BodyB") = PhysicsBeginContact2D::P_BODYB;
    physicsBeginContact2D.attr("NodeA") = PhysicsBeginContact2D::P_NODEA;
    physicsBeginContact2D.attr("NodeB") = PhysicsBeginContact2D::P_NODEB;
    physicsBeginContact2D.attr("Contacts") = PhysicsBeginContact2D::P_CONTACTS;
    physicsBeginContact2D.attr("ShapeA") = PhysicsBeginContact2D::P_SHAPEA;
    physicsBeginContact2D.attr("ShapeB") = PhysicsBeginContact2D::P_SHAPEB;

    auto physicsUpdateContact2D = paramType.def_submodule("PhysicsUpdateContact2D");
    physicsUpdateContact2D.attr("World") = PhysicsUpdateContact2D::P_WORLD;
    physicsUpdateContact2D.attr("BodyA") = PhysicsUpdateContact2D::P_BODYA;
    physicsUpdateContact2D.attr("BodyB") = PhysicsUpdateContact2D::P_BODYB;
    physicsUpdateContact2D.attr("NodeA") = PhysicsUpdateContact2D::P_NODEA;
    physicsUpdateContact2D.attr("NodeB") = PhysicsUpdateContact2D::P_NODEB;
    physicsUpdateContact2D.attr("Contacts") = PhysicsUpdateContact2D::P_CONTACTS;
    physicsUpdateContact2D.attr("ShapeA") = PhysicsUpdateContact2D::P_SHAPEA;
    physicsUpdateContact2D.attr("ShapeB") = PhysicsUpdateContact2D::P_SHAPEB;
    physicsUpdateContact2D.attr("Enabled") = PhysicsUpdateContact2D::P_ENABLED;

    auto physicsEndContact2D = paramType.def_submodule("PhysicsEndContact2D");
    physicsEndContact2D.attr("World") = PhysicsEndContact2D::P_WORLD;
    physicsEndContact2D.attr("BodyA") = PhysicsEndContact2D::P_BODYA;
    physicsEndContact2D.attr("BodyB") = PhysicsEndContact2D::P_BODYB;
    physicsEndContact2D.attr("NodeA") = PhysicsEndContact2D::P_NODEA;
    physicsEndContact2D.attr("NodeB") = PhysicsEndContact2D::P_NODEB;
    physicsEndContact2D.attr("Contacts") = PhysicsEndContact2D::P_CONTACTS;
    physicsEndContact2D.attr("ShapeA") = PhysicsEndContact2D::P_SHAPEA;
    physicsEndContact2D.attr("ShapeB") = PhysicsEndContact2D::P_SHAPEB;

    auto nodeUpdateContact2D = paramType.def_submodule("NodeUpdateContact2D");
    nodeUpdateContact2D.attr("Body") = NodeUpdateContact2D::P_BODY;
    nodeUpdateContact2D.attr("OtherNode") = NodeUpdateContact2D::P_OTHERNODE;
    nodeUpdateContact2D.attr("OtherBody") = NodeUpdateContact2D::P_OTHERBODY;
    nodeUpdateContact2D.attr("Contacts") = NodeUpdateContact2D::P_CONTACTS;
    nodeUpdateContact2D.attr("Shape") = NodeUpdateContact2D::P_SHAPE;
    nodeUpdateContact2D.attr("OtherShape") = NodeUpdateContact2D::P_OTHERSHAPE;
    nodeUpdateContact2D.attr("Enabled") = NodeUpdateContact2D::P_ENABLED;

    auto nodeBeginContact2D = paramType.def_submodule("NodeBeginContact2D");
    nodeBeginContact2D.attr("Body") = NodeBeginContact2D::P_BODY;
    nodeBeginContact2D.attr("OtherNode") = NodeBeginContact2D::P_OTHERNODE;
    nodeBeginContact2D.attr("OtherBody") = NodeBeginContact2D::P_OTHERBODY;
    nodeBeginContact2D.attr("Contacts") = NodeBeginContact2D::P_CONTACTS;
    nodeBeginContact2D.attr("Shape") = NodeBeginContact2D::P_SHAPE;
    nodeBeginContact2D.attr("OtherShape") = NodeBeginContact2D::P_OTHERSHAPE;

    auto nodeEndContact2D = paramType.def_submodule("NodeEndContact2D");
    nodeEndContact2D.attr("Body") = NodeEndContact2D::P_BODY;
    nodeEndContact2D.attr("OtherNode") = NodeEndContact2D::P_OTHERNODE;
    nodeEndContact2D.attr("OtherBody") = NodeEndContact2D::P_OTHERBODY;
    nodeEndContact2D.attr("Contacts") = NodeEndContact2D::P_CONTACTS;
    nodeEndContact2D.attr("Shape") = NodeEndContact2D::P_SHAPE;
    nodeEndContact2D.attr("OtherShape")  = NodeEndContact2D::P_OTHERSHAPE;

    nb::enum_<BodyType2D>(m, "BodyType2D")
        .value("STATIC",       BT_STATIC)
        .value("KINEMATIC",    BT_KINEMATIC)
        .value("DYNAMIC",      BT_DYNAMIC);

    nb::class_<CollisionShape2D, Component>(m, "CollisionShape2D")
    //bindCollisionShape2D.def("id"]              = sol::var(StringHash("CollisionShape2D"));
        .def("SetTrigger", &CollisionShape2D::SetTrigger)
        .def("SetCategoryBits", &CollisionShape2D::SetCategoryBits)
        .def("SetMaskBits", &CollisionShape2D::SetMaskBits)
        .def("SetGroupIndex", &CollisionShape2D::SetGroupIndex)
        .def("SetDensity", &CollisionShape2D::SetDensity)
        .def("SetFriction", &CollisionShape2D::SetFriction)
        .def("SetRestitution", &CollisionShape2D::SetRestitution)
        .def("CreateFixture", &CollisionShape2D::CreateFixture)
        .def("ReleaseFixture", &CollisionShape2D::ReleaseFixture)
        .def("IsTrigger", &CollisionShape2D::IsTrigger)
        .def("GetCategoryBits", &CollisionShape2D::GetCategoryBits)
        .def("GetMaskBits", &CollisionShape2D::GetMaskBits)
        .def("GetGroupIndex", &CollisionShape2D::GetGroupIndex)
        .def("GetDensity", &CollisionShape2D::GetDensity)
        .def("GetFriction", &CollisionShape2D::GetFriction)
        .def("GetRestitution", &CollisionShape2D::GetRestitution)
        .def("GetMass", &CollisionShape2D::GetMass)
        .def("GetInertia", &CollisionShape2D::GetInertia)
        .def("GetMassCenter", &CollisionShape2D::GetMassCenter);

    nb::class_<CollisionBox2D, CollisionShape2D>(m, "CollisionBox2D")
        //bindCollisionBox2D["id"]        = sol::var(StringHash("CollisionBox2D"));
        .def("SetSize", [](CollisionBox2D* self, const Vector2& size) { self->SetSize(size); })
        .def("SetSize", [](CollisionBox2D* self, float width, float height) { self->SetSize(width, height); })
        .def("SetCenter", [](CollisionBox2D* self, const Vector2& size) { self->SetCenter(size); })
        .def("SetCenter", [](CollisionBox2D* self, float width, float height) { self->SetCenter(width, height); })
        .def("SetAngle", &CollisionBox2D::SetAngle)
        .def("GetSize", &CollisionBox2D::GetSize)
        .def("GetCenter", &CollisionBox2D::GetCenter)
        .def("GetAngle", &CollisionBox2D::GetAngle);

    nb::class_<CollisionCircle2D, CollisionShape2D>(m, "CollisionCircle2D")
        //bindCollisionCircle2D["id"]         = sol::var(StringHash("CollisionCircle2D"));
        .def("SetRadius", &CollisionCircle2D::SetRadius)
        .def("SetCenter", [](CollisionCircle2D* self, const Vector2& size) { self->SetCenter(size); })
        .def("SetCenter", [](CollisionCircle2D* self, float width, float height) { self->SetCenter(width, height); })
        .def("GetRadius", &CollisionCircle2D::GetRadius)
        .def("GetCenter", &CollisionCircle2D::GetCenter);

    nb::class_<CollisionChain2D, CollisionShape2D>(m, "CollisionChain2D")
    //bindCollisionChain2D["id"]              = sol::var(StringHash("CollisionChain2D"));
    .def("SetLoop", &CollisionChain2D::SetLoop)
    .def("SetVertexCount", &CollisionChain2D::SetVertexCount)
    .def("SetVertex", &CollisionChain2D::SetVertex)
    .def("SetVertices", [](CollisionChain2D* self, const std::vector<Vector2>& vertices) {
        self->SetVertices(ea::vector<Vector2>(vertices.begin(), vertices.end()));
    })
    .def("GetLoop", &CollisionChain2D::GetLoop)
    .def("GetVertexCount", &CollisionChain2D::GetVertexCount)
    .def("GetVertex", &CollisionChain2D::GetVertex)
    .def("GetVertices", [](CollisionChain2D* self) {
        auto& eav = self->GetVertices();
        return std::vector<Vector2>(eav.begin(), eav.end());
    });

    nb::class_<CollisionPolygon2D, CollisionShape2D>(m, "CollisionPolygon2D")
    //bindCollisionPolygon2D["id"]                = sol::var(StringHash("CollisionPolygon2D"));
    .def("SetVertexCount", &CollisionPolygon2D::SetVertexCount)
    .def("SetVertex", &CollisionPolygon2D::SetVertex)
    .def("GetVertexCount", &CollisionPolygon2D::GetVertexCount)
    .def("GetVertex", &CollisionPolygon2D::GetVertex)
    .def("SetVertices", [](CollisionPolygon2D* self, const std::vector<Vector2>& vertices) {
        self->SetVertices(ea::vector<Vector2>(vertices.begin(), vertices.end()));
    })
    .def("GetVertices", [](CollisionPolygon2D* self) {
        auto& eav = self->GetVertices();
        return std::vector<Vector2>(eav.begin(), eav.end());
    });

    nb::class_<RigidBody2D, Component>(m, "RigidBody2D")
    //bindRigidBody2D["id"]                   = sol::var(StringHash("RigidBody2D"));
        .def("SetBodyType", &RigidBody2D::SetBodyType)
        .def("SetMass", &RigidBody2D::SetMass)
        .def("SetInertia", &RigidBody2D::SetInertia)
        .def("SetMassCenter", &RigidBody2D::SetMassCenter)
        .def("SetUseFixtureMass", &RigidBody2D::SetUseFixtureMass)
        .def("SetLinearDamping", &RigidBody2D::SetLinearDamping)
        .def("SetAngularDamping", &RigidBody2D::SetAngularDamping)
        .def("SetAllowSleep", &RigidBody2D::SetAllowSleep)
        .def("SetFixedRotation", &RigidBody2D::SetFixedRotation)
        .def("SetBullet", &RigidBody2D::SetBullet)
        .def("SetGravityScale", &RigidBody2D::SetGravityScale)
        .def("SetAwake", &RigidBody2D::SetAwake)
        .def("SetLinearVelocity", &RigidBody2D::SetLinearVelocity)
        .def("SetAngularVelocity", &RigidBody2D::SetAngularVelocity)
        .def("ApplyForce", &RigidBody2D::ApplyForce)
        .def("ApplyForceToCenter", &RigidBody2D::ApplyForceToCenter)
        .def("ApplyTorque", &RigidBody2D::ApplyTorque)
        .def("ApplyLinearImpulse", &RigidBody2D::ApplyLinearImpulse)
        .def("ApplyLinearImpulseToCenter", &RigidBody2D::ApplyLinearImpulseToCenter)
        .def("ApplyAngularImpulse", &RigidBody2D::ApplyAngularImpulse)
        .def("CreateBody", &RigidBody2D::CreateBody)
        .def("ReleaseBody" , &RigidBody2D::ReleaseBody)
        .def("ApplyWorldTransform", [](RigidBody2D* self) { self->ApplyWorldTransform(); })
        .def("ApplyWorldTransform", [](RigidBody2D* self, const Vector3& newWorldPosition, const Quaternion& newWorldRotation) { self->ApplyWorldTransform(newWorldPosition, newWorldRotation); })
        .def("AddCollisionShape2D", &RigidBody2D::AddCollisionShape2D)
        .def("RemoveCollisionShape2D", &RigidBody2D::RemoveCollisionShape2D)
        .def("AddConstraint2D", &RigidBody2D::AddConstraint2D)
        .def("RemoveConstraint2D", &RigidBody2D::RemoveConstraint2D)
        .def("GetBodyType", &RigidBody2D::GetBodyType)
        .def("GetMass", &RigidBody2D::GetMass)
        .def("GetInertia", &RigidBody2D::GetInertia)
        .def("GetMassCenter" , &RigidBody2D::GetMassCenter)
        .def("GetUseFixtureMass", &RigidBody2D::GetUseFixtureMass)
        .def("GetLinearDamping", &RigidBody2D::GetLinearDamping)
        .def("GetAngularDamping", &RigidBody2D::GetAngularDamping)
        .def("IsAllowSleep", &RigidBody2D::IsAllowSleep)
        .def("IsFixedRotation", &RigidBody2D::IsFixedRotation)
        .def("IsBullet", &RigidBody2D::IsBullet)
        .def("GetGravityScale", &RigidBody2D::GetGravityScale)
        .def("IsAwake", &RigidBody2D::IsAwake)
        .def("GetLinearVelocity", &RigidBody2D::GetLinearVelocity)
        .def("GetAngularVelocity", &RigidBody2D:: GetAngularVelocity);

    nb::class_<PhysicsWorld2D, Component>(m, "PhysicsWorld2D")
    //bindPhysicsWorld2D["id"]                        = sol::var(StringHash("PhysicsWorld2D"));
        .def("SetGravity", &PhysicsWorld2D::SetGravity)
        .def("SetAutoClearForces", &PhysicsWorld2D::SetAutoClearForces)
        .def("SetVelocityIterations", &PhysicsWorld2D::SetVelocityIterations)
        .def("SetPositionIterations", &PhysicsWorld2D::SetPositionIterations)
        .def("AddRigidBody", &PhysicsWorld2D::AddRigidBody)
        .def("RemoveRigidBody", &PhysicsWorld2D::RemoveRigidBody)
        .def("AddDelayedWorldTransform", &PhysicsWorld2D::AddDelayedWorldTransform)
        .def("AddDelayedWorldTransform", &PhysicsWorld2D::AddDelayedWorldTransform)
        .def("AddDelayedWorldTransform", &PhysicsWorld2D::AddDelayedWorldTransform)
        .def("GetRigidBody", [](PhysicsWorld2D* self, const Vector2& point) { return self->GetRigidBody(point); })
        .def("GetRigidBody", [](PhysicsWorld2D* self, const Vector2& point, unsigned collisionMask) { return self->GetRigidBody(point, collisionMask); })
        .def("GetRigidBody", [](PhysicsWorld2D* self, int screenX, int screenY) { return self->GetRigidBody(screenX, screenY); })
        .def("GetRigidBody", [](PhysicsWorld2D* self, int screenX, int screenY, unsigned collisionMask) { return self->GetRigidBody(screenX, screenY, collisionMask); })
        .def("DrawDebugGeometry", nb::overload_cast<>(&PhysicsWorld2D::DrawDebugGeometry))
        .def("Raycast", [](PhysicsWorld2D* self, const Vector2& startPoint, const Vector2& endPoint) {
            ea::vector<PhysicsRaycastResult2D> results;
            self->Raycast(results, startPoint, endPoint);
            return std::vector<PhysicsRaycastResult2D>(results.begin(), results.end());
        })
        .def("Raycast", [](PhysicsWorld2D* self, const Vector2& startPoint, const Vector2& endPoint, unsigned collisionMask) {
            ea::vector<PhysicsRaycastResult2D> results;
            self->Raycast(results, startPoint, endPoint, collisionMask);
            return std::vector<PhysicsRaycastResult2D>(results.begin(), results.end());
        })
        .def("RaycastSingle", [](PhysicsWorld2D* self, const Vector2& startPoint, const Vector2& endPoint) {
            PhysicsRaycastResult2D result;
            self->RaycastSingle(result, startPoint, endPoint);
            return result;
        })
        .def("RaycastSingle", [](PhysicsWorld2D* self, const Vector2& startPoint, const Vector2& endPoint, unsigned collisionMask) {
            PhysicsRaycastResult2D result;
            self->RaycastSingle(result, startPoint, endPoint, collisionMask);
            return result;
        });

    nb::class_<PhysicsRaycastResult2D>(m, "PhysicsRaycastResult2D")
        .def_rw("position", &PhysicsRaycastResult2D::position_)
        .def_rw("normal", &PhysicsRaycastResult2D::normal_)
        .def_rw("distance", &PhysicsRaycastResult2D::distance_)
        .def_rw("rigid_body", &PhysicsRaycastResult2D::body_);
}
