#include <nanobind/nanobind.h>
#include "Urho3D/Core/Context.h"
#include "Urho3D/Math/Ray.h"
#include "Urho3D/Physics/PhysicsWorld.h"
#include "Urho3D/Physics/RigidBody.h"
#include "Urho3D/Physics/CollisionShape.h"
#include "Urho3D/Physics/Constraint.h"
#include "Urho3D/Physics/PhysicsEvents.h"
#include "Urho3D/Physics/KinematicCharacterController.h"
#include "Urho3D/Graphics/Model.h"
#include "Urho3D/Graphics/CustomGeometry.h"

using namespace Urho3D;
namespace nb = nanobind;
using namespace nb::literals;

NB_MODULE(physics, m)
{
    auto subm = m.def_submodule("EventType");
    subm.attr("PhysicsPreUpdate") = E_PHYSICSPREUPDATE;
    subm.attr("PhysicsPostUpdate") = E_PHYSICSPOSTUPDATE;
    subm.attr("PhysicsPreStep") = E_PHYSICSPRESTEP;
    subm.attr("PhysicsPostStep") = E_PHYSICSPOSTSTEP;
    subm.attr("PhysicsCollisionStart") = E_PHYSICSCOLLISIONSTART;
    subm.attr("PhysicsCollision") = E_PHYSICSCOLLISION;
    subm.attr("PhysicsCollisionEnd") = E_PHYSICSCOLLISIONEND;
    subm.attr("NodeCollisionStart") = E_NODECOLLISIONSTART;
    subm.attr("NodeCollision") = E_NODECOLLISION;
    subm.attr("NodeCollisionEnd") = E_NODECOLLISIONEND;

    auto paramType = m.def_submodule("ParamType");
    subm = paramType.def_submodule("PhysicsPreUpdate");
    subm.attr("World")       = PhysicsPreUpdate::P_WORLD;
    subm.attr("TimeStep")    = PhysicsPreUpdate::P_TIMESTEP;

    subm = paramType.def_submodule("PhysicsPostUpdate");
    subm.attr("World") = PhysicsPostUpdate::P_WORLD;
    subm.attr("TimeStep") = PhysicsPostUpdate::P_TIMESTEP;
    subm.attr("Overtime") = PhysicsPostUpdate::P_OVERTIME;

    subm = paramType.def_submodule("PhysicsPreStep");
    subm.attr("World") = PhysicsPreStep::P_WORLD;
    subm.attr("TimeStep") = PhysicsPreStep::P_TIMESTEP;
    subm.attr("NetworkFrame") = PhysicsPreStep::P_NETWORKFRAME;

    subm = paramType.def_submodule("PhysicsPostStep");
    subm.attr("World") = PhysicsPostStep::P_WORLD;
    subm.attr("TimeStep") = PhysicsPostStep::P_TIMESTEP;

    subm = paramType.def_submodule("PhysicsCollisionStart");
    subm.attr("World")      = PhysicsCollisionStart::P_WORLD;
    subm.attr("NodeA")      = PhysicsCollisionStart::P_NODEA;
    subm.attr("NodeB")      = PhysicsCollisionStart::P_NODEB;
    subm.attr("BodyA")      = PhysicsCollisionStart::P_BODYA;
    subm.attr("BodyB")      = PhysicsCollisionStart::P_BODYB;
    subm.attr("Trigger") = PhysicsCollisionStart::P_TRIGGER;
    subm.attr("Contacts") = PhysicsCollisionStart::P_CONTACTS;

    subm = paramType.def_submodule("PhysicsCollision");
    subm.attr("World") = PhysicsCollision::P_WORLD;
    subm.attr("NodeA") = PhysicsCollision::P_NODEA;
    subm.attr("NodeB") = PhysicsCollision::P_NODEB;
    subm.attr("BodyA") = PhysicsCollision::P_BODYA;
    subm.attr("BodyB") = PhysicsCollision::P_BODYB;
    subm.attr("Trigger") = PhysicsCollision::P_TRIGGER;
    subm.attr("Contacts")    = PhysicsCollision::P_CONTACTS;

    subm = paramType.def_submodule("PhysicsCollisionEnd");
    subm.attr("World")      = PhysicsCollisionEnd::P_WORLD;
    subm.attr("NodeA")      = PhysicsCollisionEnd::P_NODEA;
    subm.attr("NodeB")      = PhysicsCollisionEnd::P_NODEB;
    subm.attr("BodyA")      = PhysicsCollisionEnd::P_BODYA;
    subm.attr("BodyB")      = PhysicsCollisionEnd::P_BODYB;
    subm.attr("Trigger") = PhysicsCollisionEnd::P_TRIGGER;

    subm = paramType.def_submodule("NodeCollisionStart");
    subm.attr("Body") = NodeCollisionStart::P_BODY;
    subm.attr("OtherNode") = NodeCollisionStart::P_OTHERNODE;
    subm.attr("OtherBody") = NodeCollisionStart::P_OTHERBODY;
    subm.attr("Trigger") = NodeCollisionStart::P_TRIGGER;
    subm.attr("Contacts") = NodeCollisionStart::P_CONTACTS;

    subm = paramType.def_submodule("NodeCollision");
    subm.attr("Body") = NodeCollision::P_BODY;
    subm.attr("OtherNode") = NodeCollision::P_OTHERNODE;
    subm.attr("OtherBody") = NodeCollision::P_OTHERBODY;
    subm.attr("Trigger") = NodeCollision::P_TRIGGER;
    subm.attr("Contacts") = NodeCollision::P_CONTACTS;

    subm = paramType.def_submodule("NodeCollisionEnd");
    subm.attr("Body") = NodeCollisionEnd::P_BODY;
    subm.attr("OtherNode") = NodeCollisionEnd::P_OTHERNODE;
    subm.attr("OtherBody") = NodeCollisionEnd::P_OTHERBODY;
    subm.attr("Trigger") = NodeCollisionEnd::P_TRIGGER;

    m.attr("COLLISION_NEVER")      = COLLISION_NEVER;
    m.attr("COLLISION_ACTIVE")     = COLLISION_ACTIVE;
    m.attr("COLLISION_ALWAYS") = COLLISION_ALWAYS;
    m.attr("SHAPE_BOX") = SHAPE_BOX;
    m.attr("SHAPE_SPHERE") = SHAPE_SPHERE;
    m.attr("SHAPE_STATICPLANE") = SHAPE_STATICPLANE;
    m.attr("SHAPE_CYLINDER") = SHAPE_CYLINDER;
    m.attr("SHAPE_CAPSULE") = SHAPE_CAPSULE;
    m.attr("SHAPE_CONE") = SHAPE_CONE;
    m.attr("SHAPE_TRIANGLEMESH") = SHAPE_TRIANGLEMESH;
    m.attr("SHAPE_CONVEXHULL") = SHAPE_CONVEXHULL;
    m.attr("SHAPE_TERRAIN") = SHAPE_TERRAIN;
    m.attr("SHAPE_GIMPACTMESH") = SHAPE_GIMPACTMESH;
    m.attr("CONSTRAINT_POINT") = CONSTRAINT_POINT;
    m.attr("CONSTRAINT_HINGE") = CONSTRAINT_HINGE;
    m.attr("CONSTRAINT_SLIDER") = CONSTRAINT_SLIDER;
    m.attr("CONSTRAINT_CONETWIST") = CONSTRAINT_CONETWIST;
        
    nb::class_<RigidBody, Component>(m, "RigidBody")
    //bindRigidBody["id"]                       = sol::var(StringHash("RigidBody"));
        .def_prop_rw("position", &RigidBody::GetPosition, &RigidBody::SetPosition)
        .def_prop_rw("rotation", &RigidBody::GetRotation, &RigidBody::SetRotation)
        .def_prop_rw("mass", &RigidBody::GetMass, &RigidBody::SetMass)
        .def_prop_rw("friction", &RigidBody::GetFriction, &RigidBody::SetFriction)
        .def_prop_rw("rolling_friction", &RigidBody::GetRollingFriction, &RigidBody::SetRollingFriction)
        .def_prop_rw("restitution", &RigidBody::GetRestitution, &RigidBody::SetRestitution)
        .def_prop_rw("contact_threshold", &RigidBody::GetContactProcessingThreshold, &RigidBody::SetContactProcessingThreshold)
        .def_prop_rw("angular_velocity", &RigidBody::GetAngularVelocity, &RigidBody::SetAngularVelocity)
        .def_prop_rw("angular_factor", &RigidBody::GetAngularFactor, &RigidBody::SetAngularFactor)
        .def_prop_rw("angular_rest_threshold", &RigidBody::GetAngularRestThreshold, &RigidBody::SetAngularRestThreshold)
        .def_prop_rw("angular_damping", &RigidBody::GetAngularDamping, &RigidBody::SetAngularDamping)
        .def_prop_rw("linear_velocity", &RigidBody::GetLinearVelocity, &RigidBody::SetLinearVelocity)
        .def_prop_rw("linear_factor", &RigidBody::GetLinearFactor, &RigidBody::SetLinearFactor)
        .def_prop_rw("linear_rest_threshold", &RigidBody::GetLinearRestThreshold, &RigidBody::SetLinearRestThreshold)
        .def_prop_rw("linear_damping", &RigidBody::GetLinearDamping, &RigidBody::SetLinearDamping)
        .def_prop_rw("collision_layer", &RigidBody::GetCollisionLayer, &RigidBody::SetCollisionLayer)
        .def_prop_rw("collision_mask", &RigidBody::GetCollisionMask, &RigidBody::SetCollisionMask)
        .def("SetCollisionLayerAndMask", &RigidBody::SetCollisionLayerAndMask)
        .def("SetCcdRadius", &RigidBody::SetCcdRadius)
        .def("SetCcdMotionThreshold", &RigidBody::SetCcdMotionThreshold)
        .def("SetUseGravity", &RigidBody::SetUseGravity)
        .def("SetGravityOverride", &RigidBody::SetGravityOverride)
        .def("SetKinematic", &RigidBody::SetKinematic)
        .def("SetTrigger", &RigidBody::SetTrigger)
        .def("ApplyTorque", &RigidBody::ApplyTorque)
        .def("SetTransform", &RigidBody::SetTransform)
        .def("ApplyTorque", &RigidBody::ApplyTorque)
        .def("ApplyForce", [](RigidBody* self, const Vector3& force) { self->ApplyForce(force); })
        .def("ApplyForce", [](RigidBody* self, const Vector3& force, const Vector3& position) { self->ApplyForce(force, position); })
        .def("ApplyImpulse", [](RigidBody* self, const Vector3& impulse) { self->ApplyImpulse(impulse); })
        .def("ApplyImpulse", [](RigidBody* self, const Vector3& impulse, const Vector3& position) { self->ApplyImpulse(impulse, position); })
        .def("ApplyTorqueImpulse", &RigidBody::ApplyTorqueImpulse)
        .def("ResetForces", &RigidBody::ResetForces)
        .def("Activate", &RigidBody::Activate)
        .def("ReAddBodyToWorld", &RigidBody::ReAddBodyToWorld)
        .def("DisableMassUpdate", &RigidBody::DisableMassUpdate)
        .def("EnableMassUpdate", &RigidBody::EnableMassUpdate)
        .def("SetCollisionEventMode", &RigidBody::SetCollisionEventMode);

    nb::class_<CollisionShape, Component>(m, "CollisionShape")
    //bindCollisionShape["id"]                       = sol::var(StringHash("CollisionShape"));
        .def_prop_rw("size", &CollisionShape::GetSize, &CollisionShape::SetSize)
        .def_prop_rw("margin", &CollisionShape::GetMargin, &CollisionShape::SetMargin)
        .def_prop_rw("position", &CollisionShape::GetPosition, &CollisionShape::SetPosition)
        .def_prop_rw("rotation", &CollisionShape::GetRotation, &CollisionShape::SetRotation)
        .def("SetBox", [](CollisionShape* self, const Vector3& size) { self->SetBox(size); })
        .def("SetBox", [](CollisionShape* self, const Vector3& size, const Vector3& position, const Quaternion& rotation) { self->SetBox(size, position, rotation); })
        .def("SetSphere", [](CollisionShape* self, float diameter) { self->SetSphere(diameter); })
        .def("SetSphere", [](CollisionShape* self, float diameter, const Vector3& position, const Quaternion& rotation) { self->SetSphere(diameter, position, rotation); })
        .def("SetStaticPlane", [](CollisionShape* self) { self->SetStaticPlane(); })
        .def("SetStaticPlane", [](CollisionShape* self, const Vector3& position, const Quaternion& rotation) { self->SetStaticPlane(position, rotation); })
        .def("SetCylinder", [](CollisionShape* self, float diameter, float height) { self->SetCylinder(diameter, height); })
        .def("SetCylinder" , [](CollisionShape* self, float diameter, float height, const Vector3& position, const Quaternion& rotation) { self->SetCylinder(diameter, height, position, rotation); })
        .def("SetCapsule", [](CollisionShape* self, float diameter, float height) { self->SetCapsule(diameter, height); })
        .def("SetCapsule", [](CollisionShape* self, float diameter, float height, const Vector3& position, const Quaternion& rotation) { self->SetCapsule(diameter, height, position, rotation); })
        .def("SetCone", [](CollisionShape* self, float diameter, float height) { self->SetCone(diameter, height); })
        .def("SetCone", [](CollisionShape* self, float diameter, float height, const Vector3& position, const Quaternion& rotation) { self->SetCone(diameter, height, position, rotation); })
        .def("SetTriangleMesh", [](CollisionShape* self, Model* model) { self->SetTriangleMesh(model); })
        .def("SetTriangleMesh", [](CollisionShape* self, Model* model, unsigned lodLevel, const Vector3& scale, const Vector3& position, const Quaternion& rotation) { self->SetTriangleMesh(model, lodLevel, scale, position, rotation); })
        .def("SetConvexHull", [](CollisionShape* self, Model* model) { self->SetConvexHull(model); })
        .def("SetConvexHull", [](CollisionShape* self, Model* model, unsigned lodLevel, const Vector3& scale, const Vector3& position, const Quaternion& rotation) { self->SetConvexHull(model, lodLevel, scale, position, rotation); })
        .def("SetCustomTriangleMesh", [](CollisionShape* self, CustomGeometry* custom) { self->SetCustomTriangleMesh(custom); })
        .def("SetCustomTriangleMesh", [](CollisionShape* self, CustomGeometry* custom, const Vector3& scale, const Vector3& position, const Quaternion& rotation) { self->SetCustomTriangleMesh(custom, scale, position, rotation); })
        .def("SetCustomConvexHull", [](CollisionShape* self, CustomGeometry* custom) { self->SetCustomConvexHull(custom); })
        .def("SetCustomConvexHull", [](CollisionShape* self, CustomGeometry* custom, const Vector3& scale, const Vector3& position, const Quaternion& rotation) { self->SetCustomConvexHull(custom, scale, position, rotation); })
        .def("SetTerrain", &CollisionShape::SetTerrain)
        .def("SetShapeType", &CollisionShape::SetShapeType)
        .def("SetModel", &CollisionShape::SetModel)
        .def("SetLodLevel", &CollisionShape::SetLodLevel);

    nb::class_<Constraint, Component>(m, "Constraint")
    //bindConstraint["id"]                   = sol::var(StringHash("Constraint"));
        .def_prop_rw("constraint_type", &Constraint::GetConstraintType, &Constraint::SetConstraintType)
        .def_prop_rw("position", &Constraint::GetPosition, &Constraint::SetPosition)
        .def_prop_rw("rotation", &Constraint::GetRotation, &Constraint::SetRotation)
        .def_prop_rw("other_position", &Constraint::GetOtherPosition, &Constraint::SetOtherPosition)
        .def_prop_rw("other_rotation", &Constraint::GetRotation, &Constraint::SetOtherRotation)
        .def_prop_rw("high_limit", &Constraint::GetHighLimit, &Constraint::SetHighLimit)
        .def_prop_rw("low_limit", &Constraint::GetLowLimit, &Constraint::SetLowLimit)
        .def_prop_rw("disable_collision", &Constraint::GetDisableCollision, &Constraint::SetDisableCollision)
        .def_prop_rw("erp", &Constraint::GetERP, &Constraint::SetERP)
        .def_prop_rw("cfm", &Constraint::GetCFM, &Constraint::SetCFM)
        .def("SetAxis", &Constraint::SetAxis)
        .def("SetOtherAxis", &Constraint::SetOtherAxis)
        .def("GetWorldPosition", &Constraint::GetWorldPosition)
        .def("SetWorldPosition", &Constraint::SetWorldPosition)
        .def("SetOtherBody", &Constraint::SetOtherBody)
        .def("GetOtherBody", &Constraint::GetOtherBody)
        .def("GetOwnBody", &Constraint::GetOwnBody)
        .def("ReleaseConstraint", &Constraint::ReleaseConstraint)
        .def("ApplyFrames", &Constraint::ApplyFrames);

    nb::class_<PhysicsRaycastResult>(m, "PhysicsRaycastResult")
        .def_rw("position", &PhysicsRaycastResult::position_)
        .def_rw("normal", &PhysicsRaycastResult::normal_)
        .def_rw("distance", &PhysicsRaycastResult::distance_)
        .def_rw("hit_fraction", &PhysicsRaycastResult::hitFraction_)
        .def_rw("rigid_body", &PhysicsRaycastResult::body_);
    
    nb::class_<PhysicsWorld, Component>(m, "PhysicsWorld")
    //bindPhysicsWorld["id"]               = sol::var(StringHash("PhysicsWorld"));
        .def_prop_rw("fps", &PhysicsWorld::GetFps, &PhysicsWorld::SetFps)
        .def_prop_rw("gravity", &PhysicsWorld::GetGravity, &PhysicsWorld::SetGravity)
        .def_prop_rw("max_sub_steps", &PhysicsWorld::GetMaxSubSteps, &PhysicsWorld::SetMaxSubSteps)
        .def_prop_rw("num_iterations", &PhysicsWorld::GetNumIterations, &PhysicsWorld::SetNumIterations)
        .def_prop_rw("update_enabled", &PhysicsWorld::IsUpdateEnabled, &PhysicsWorld::SetUpdateEnabled)
        .def_prop_rw("interpolation", &PhysicsWorld::GetInterpolation, &PhysicsWorld::SetInterpolation)
        .def_prop_rw("internal_edge", &PhysicsWorld::GetInternalEdge, &PhysicsWorld::SetInternalEdge)
        .def_prop_rw("split_impulse", &PhysicsWorld::GetSplitImpulse, &PhysicsWorld::SetSplitImpulse)
        .def("Raycast", [](PhysicsWorld* self, const Ray& ray, float maxDistance) {
            ea::vector<PhysicsRaycastResult> result;
            self->Raycast(result, ray, maxDistance);
            return std::vector<PhysicsRaycastResult>(result.begin(), result.end());
        })
        .def("Raycast", [](PhysicsWorld* self, const Ray& ray, float maxDistance, unsigned collisionMask) {
            ea::vector<PhysicsRaycastResult> result;
            self->Raycast(result, ray, maxDistance, collisionMask);
            return std::vector<PhysicsRaycastResult>(result.begin(), result.end());
        })
        .def("RaycastSingle", [](PhysicsWorld* self, const Ray& ray, float maxDistance) {
            PhysicsRaycastResult result;
            self->RaycastSingle(result, ray, maxDistance);
            return result;
        })
        .def("RaycastSingle", [](PhysicsWorld* self, const Ray& ray, float maxDistance, unsigned collisionMask) {
            PhysicsRaycastResult result;
            self->RaycastSingle(result, ray, maxDistance, collisionMask);
            return result;
        })
        .def("RaycastSingleSegmented", [](PhysicsWorld* self, const Ray& ray, float maxDistance, float segmentDistance) {
            PhysicsRaycastResult result;
            self->RaycastSingleSegmented(result, ray, maxDistance, segmentDistance);
            return result;
        })
        .def("RaycastSingleSegmented", [](PhysicsWorld* self, const Ray& ray, float maxDistance, float segmentDistance, unsigned collisionMask) {
            PhysicsRaycastResult result;
            self->RaycastSingleSegmented(result, ray, maxDistance, segmentDistance, collisionMask);
            return result;
        })
        .def("SphereCast", [](PhysicsWorld* self, const Ray& ray, float radius, float maxDistance) {
            PhysicsRaycastResult result;
            self->SphereCast(result, ray, radius, maxDistance);
            return result;
        })
        .def("SphereCast", [](PhysicsWorld* self, const Ray& ray, float radius, float maxDistance, unsigned collisionMask) {
            PhysicsRaycastResult result;
            self->SphereCast(result, ray, radius, maxDistance, collisionMask);
            return result;
        })
        .def("ConvexCast", [](PhysicsWorld* self, CollisionShape* shape, const Vector3& startPos, const Quaternion& startRot, const Vector3& endPos, const Quaternion& endRot) {
            PhysicsRaycastResult result;
            self->ConvexCast(result, shape, startPos, startRot, endPos, endRot);
            return result;
        })
        .def("ConvexCast", [](PhysicsWorld* self, CollisionShape* shape, const Vector3& startPos, const Quaternion& startRot, const Vector3& endPos, const Quaternion& endRot, unsigned collisionMask) {
            PhysicsRaycastResult result;
            self->ConvexCast(result, shape, startPos, startRot, endPos, endRot, collisionMask);
            return result;
        })
        .def("RemoveCachedGeometry", &PhysicsWorld::RemoveCachedGeometry)
        .def("DrawDebugGeometry", nb::overload_cast<bool>(&PhysicsWorld::DrawDebugGeometry))
        .def("GetRigidBodies", [](PhysicsWorld* self, const Sphere& sphere) {
            ea::vector<RigidBody*> out;
            self->GetRigidBodies(out, sphere);
            return std::vector<RigidBody*>(out.begin(), out.end());
        })
        .def("GetRigidBodies", [](PhysicsWorld* self, const BoundingBox& box) {
            ea::vector<RigidBody*> out;
            self->GetRigidBodies(out, box);
            return std::vector<RigidBody*>(out.begin(), out.end());
        })
        .def("GetRigidBodies", [](PhysicsWorld* self, const Sphere& sphere, unsigned collisionMask) {
            ea::vector<RigidBody*> out;
            self->GetRigidBodies(out, sphere, collisionMask);
            return std::vector<RigidBody*>(out.begin(), out.end());
        })
        .def("GetRigidBodies", [](PhysicsWorld* self, const BoundingBox& box, unsigned collisionMask) {
            ea::vector<RigidBody*> out;
            self->GetRigidBodies(out, box, collisionMask);
            return std::vector<RigidBody*>(out.begin(), out.end());
        })
        .def("GetRigidBodies", [](PhysicsWorld* self, const RigidBody* body) {
            ea::vector<RigidBody*> out;
            self->GetRigidBodies(out, body);
            return std::vector<RigidBody*>(out.begin(), out.end());
        })
        .def("GetCollidingBodies", [](PhysicsWorld* self, const RigidBody* body) {
            ea::vector<RigidBody*> out;
            self->GetCollidingBodies(out, body);
            return std::vector<RigidBody*>(out.begin(), out.end());
        });

    nb::class_<KinematicCharacterController, Component>(m, "KinematicCharacterController")
    //bindKinematicCharacterController["id"]                       = sol::var(StringHash("KinematicCharacterController"));
        .def_prop_rw("gravity", &KinematicCharacterController::GetGravity, &KinematicCharacterController::SetGravity)
        .def_prop_rw("linear_damping", &KinematicCharacterController::GetLinearDamping, &KinematicCharacterController::SetLinearDamping)
        .def_prop_rw("angular_damping", &KinematicCharacterController::GetAngularDamping, &KinematicCharacterController::SetAngularDamping)
        .def_prop_rw("height", &KinematicCharacterController::GetHeight, &KinematicCharacterController::SetHeight)
        .def_prop_rw("diameter", &KinematicCharacterController::GetDiameter, &KinematicCharacterController::SetDiameter)
        .def_prop_rw("offset", &KinematicCharacterController::GetOffset, &KinematicCharacterController::SetOffset)
        .def_prop_rw("step_height", &KinematicCharacterController::GetStepHeight, &KinematicCharacterController::SetStepHeight)
        .def_prop_rw("max_jump_height", &KinematicCharacterController::GetMaxJumpHeight, &KinematicCharacterController::SetMaxJumpHeight)
        .def_prop_rw("fall_speed", &KinematicCharacterController::GetFallSpeed, &KinematicCharacterController::SetFallSpeed)
        .def_prop_rw("jump_speed", &KinematicCharacterController::GetJumpSpeed, &KinematicCharacterController::SetJumpSpeed)
        .def_prop_rw("max_slope", &KinematicCharacterController::GetMaxSlope, &KinematicCharacterController::SetMaxSlope)
        .def_prop_rw("linear_velocity", &KinematicCharacterController::GetLinearVelocity, &KinematicCharacterController::SetLinearVelocity)
        .def_prop_rw("angular_velocity", &KinematicCharacterController::GetAngularVelocity, &KinematicCharacterController::SetAngularVelocity)
        .def_prop_rw("collision_layer", &KinematicCharacterController::GetCollisionLayer, &KinematicCharacterController::SetCollisionLayer)
        .def_prop_rw("collision_mask", &KinematicCharacterController::GetCollisionMask, &KinematicCharacterController::SetCollisionMask)
        .def("SetCollisionLayerAndMask", &KinematicCharacterController::SetCollisionLayerAndMask)
        .def("SetWalkIncrement", &KinematicCharacterController::SetWalkIncrement)
        .def("OnGround", &KinematicCharacterController::OnGround)
        .def("Jump", [](KinematicCharacterController* self) { self->Jump(); })
        .def("Jump", [](KinematicCharacterController* self, const Vector3& jump) { self->Jump(jump); })
        .def("ApplyImpulse", &KinematicCharacterController::ApplyImpulse)
        .def("CanJump", &KinematicCharacterController::CanJump);
}
