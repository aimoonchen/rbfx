#include "../../Core/Context.h"
#include "GetPush.h"
#include "../../Math/Ray.h"
#include "../../Physics/PhysicsWorld.h"
#include "../../Physics/RigidBody.h"
#include "../../Physics/CollisionShape.h"
#include "../../Physics/Constraint.h"
#include "../../Physics/KinematicCharacterController.h"
using namespace Urho3D;

Urho3D::Context* GetContext(lua_State* L);

int sol2_PhysicsLuaAPI_open(sol::state& lua)
{
    lua["COLLISION_NEVER"]      = COLLISION_NEVER;
    lua["COLLISION_ACTIVE"]     = COLLISION_ACTIVE;
    lua["COLLISION_ALWAYS"]     = COLLISION_ALWAYS;
    lua["SHAPE_BOX"]            = SHAPE_BOX;
    lua["SHAPE_SPHERE"]         = SHAPE_SPHERE;
    lua["SHAPE_STATICPLANE"]    = SHAPE_STATICPLANE;
    lua["SHAPE_CYLINDER"]       = SHAPE_CYLINDER;
    lua["SHAPE_CAPSULE"]        = SHAPE_CAPSULE;
    lua["SHAPE_CONE"]           = SHAPE_CONE;
    lua["SHAPE_TRIANGLEMESH"]   = SHAPE_TRIANGLEMESH;
    lua["SHAPE_CONVEXHULL"]     = SHAPE_CONVEXHULL;
    lua["SHAPE_TERRAIN"]        = SHAPE_TERRAIN;
    lua["SHAPE_GIMPACTMESH"]    = SHAPE_GIMPACTMESH;
    lua["CONSTRAINT_POINT"]     = CONSTRAINT_POINT;
    lua["CONSTRAINT_HINGE"]     = CONSTRAINT_HINGE;
    lua["CONSTRAINT_SLIDER"]    = CONSTRAINT_SLIDER;
    lua["CONSTRAINT_CONETWIST"] = CONSTRAINT_CONETWIST;
        
    lua.new_usertype<RigidBody>("RigidBody",
        "id",                       sol::var(StringHash("RigidBody")),
        "position",                 sol::property(&RigidBody::GetPosition, &RigidBody::SetPosition),
        "rotation",                 sol::property(&RigidBody::GetRotation, &RigidBody::SetRotation),
        "mass",                     sol::property(&RigidBody::GetMass, &RigidBody::SetMass),
        "friction",                 sol::property(&RigidBody::GetFriction, &RigidBody::SetFriction),
        "rolling_friction",         sol::property(&RigidBody::GetRollingFriction, &RigidBody::SetRollingFriction),
        "restitution",              sol::property(&RigidBody::GetRestitution, &RigidBody::SetRestitution),
        "contact_threshold",        sol::property(&RigidBody::GetContactProcessingThreshold, &RigidBody::SetContactProcessingThreshold),
        "angular_velocity",         sol::property(&RigidBody::GetAngularVelocity, &RigidBody::SetAngularVelocity),
        "angular_factor",           sol::property(&RigidBody::GetAngularFactor, &RigidBody::SetAngularFactor),
        "angular_rest_threshold",   sol::property(&RigidBody::GetAngularRestThreshold, &RigidBody::SetAngularRestThreshold),
        "angular_damping",          sol::property(&RigidBody::GetAngularDamping, &RigidBody::SetAngularDamping),
        "linear_velocity",          sol::property(&RigidBody::GetLinearVelocity, &RigidBody::SetLinearVelocity),
        "linear_factor",            sol::property(&RigidBody::GetLinearFactor, &RigidBody::SetLinearFactor),
        "linear_rest_threshold",    sol::property(&RigidBody::GetLinearRestThreshold, &RigidBody::SetLinearRestThreshold),
        "linear_damping",           sol::property(&RigidBody::GetLinearDamping, &RigidBody::SetLinearDamping),
        "collision_layer",          sol::property(&RigidBody::GetCollisionLayer, &RigidBody::SetCollisionLayer),
        "collision_mask",           sol::property(&RigidBody::GetCollisionMask, &RigidBody::SetCollisionMask),
        "SetCollisionLayerAndMask", &RigidBody::SetCollisionLayerAndMask,
        "SetCcdRadius",             &RigidBody::SetCcdRadius,
        "SetCcdMotionThreshold",    &RigidBody::SetCcdMotionThreshold,
        "SetUseGravity",            &RigidBody::SetUseGravity,
        "SetGravityOverride",       &RigidBody::SetGravityOverride,
        "SetKinematic",             &RigidBody::SetKinematic,
        "SetTrigger",               &RigidBody::SetTrigger,
        "ApplyTorque",              &RigidBody::ApplyTorque,
        "SetTransform",             &RigidBody::SetTransform,
        "ApplyTorque",              &RigidBody::ApplyTorque,
        "ApplyForce",               sol::overload(
            [](RigidBody* self, const Vector3& force) { self->ApplyForce(force); },
            [](RigidBody* self, const Vector3& force, const Vector3& position) { self->ApplyForce(force, position); }),
        "ApplyImpulse",             sol::overload(
            [](RigidBody* self, const Vector3& impulse) { self->ApplyImpulse(impulse); },
            [](RigidBody* self, const Vector3& impulse, const Vector3& position) { self->ApplyImpulse(impulse, position); }),
        "ApplyTorqueImpulse",       &RigidBody::ApplyTorqueImpulse,
        "ResetForces",              &RigidBody::ResetForces,
        "Activate",                 &RigidBody::Activate,
        "ReAddBodyToWorld",         &RigidBody::ReAddBodyToWorld,
        "DisableMassUpdate",        &RigidBody::DisableMassUpdate,
        "EnableMassUpdate",         &RigidBody::EnableMassUpdate,
        "SetCollisionEventMode",    &RigidBody::SetCollisionEventMode,
        sol::base_classes, sol::bases<Component>()
        );
    lua.new_usertype<CollisionShape>("CollisionShape",
        "id",                       sol::var(StringHash("CollisionShape")),
        "size",                     sol::property(&CollisionShape::GetSize, &CollisionShape::SetSize),
        "margin",                   sol::property(&CollisionShape::GetMargin, &CollisionShape::SetMargin),
        "position",                 sol::property(&CollisionShape::GetPosition, &CollisionShape::SetPosition),
        "rotation",                 sol::property(&CollisionShape::GetRotation, &CollisionShape::SetRotation),
        "SetBox",                   sol::overload(
            [](CollisionShape* self, const Vector3& size) { self->SetBox(size); },
            [](CollisionShape* self, const Vector3& size, const Vector3& position, const Quaternion& rotation) { self->SetBox(size, position, rotation); }),
        "SetSphere",                sol::overload(
            [](CollisionShape* self, float diameter) { self->SetSphere(diameter); },
            [](CollisionShape* self, float diameter, const Vector3& position, const Quaternion& rotation) { self->SetSphere(diameter, position, rotation); }),
        "SetStaticPlane",           sol::overload(
            [](CollisionShape* self) { self->SetStaticPlane(); },
            [](CollisionShape* self, const Vector3& position, const Quaternion& rotation) { self->SetStaticPlane(position, rotation); }),
        "SetCylinder",              sol::overload(
            [](CollisionShape* self, float diameter, float height) { self->SetCylinder(diameter, height); },
            [](CollisionShape* self, float diameter, float height, const Vector3& position, const Quaternion& rotation) { self->SetCylinder(diameter, height, position, rotation); }),
        "SetCapsule",               sol::overload(
            [](CollisionShape* self, float diameter, float height) { self->SetCapsule(diameter, height); },
            [](CollisionShape* self, float diameter, float height, const Vector3& position, const Quaternion& rotation) { self->SetCapsule(diameter, height, position, rotation); }),
        "SetCone",                  sol::overload(
            [](CollisionShape* self, float diameter, float height) { self->SetCone(diameter, height); },
            [](CollisionShape* self, float diameter, float height, const Vector3& position, const Quaternion& rotation) { self->SetCone(diameter, height, position, rotation); }),
        "SetTriangleMesh",          sol::overload(
            [](CollisionShape* self, Model* model) { self->SetTriangleMesh(model); },
            [](CollisionShape* self, Model* model, unsigned lodLevel, const Vector3& scale, const Vector3& position, const Quaternion& rotation) { self->SetTriangleMesh(model, lodLevel, scale, position, rotation); }),
        "SetConvexHull",            sol::overload(
            [](CollisionShape* self, Model* model) { self->SetConvexHull(model); },
            [](CollisionShape* self, Model* model, unsigned lodLevel, const Vector3& scale, const Vector3& position, const Quaternion& rotation) { self->SetConvexHull(model, lodLevel, scale, position, rotation); }),
        "SetCustomTriangleMesh",    sol::overload(
            [](CollisionShape* self, CustomGeometry* custom) { self->SetCustomTriangleMesh(custom); },
            [](CollisionShape* self, CustomGeometry* custom, const Vector3& scale, const Vector3& position, const Quaternion& rotation) { self->SetCustomTriangleMesh(custom, scale, position, rotation); }),
        "SetCustomConvexHull",      sol::overload(
            [](CollisionShape* self, CustomGeometry* custom) { self->SetCustomConvexHull(custom); },
            [](CollisionShape* self, CustomGeometry* custom, const Vector3& scale, const Vector3& position, const Quaternion& rotation) { self->SetCustomConvexHull(custom, scale, position, rotation); }),
        "SetTerrain",               &CollisionShape::SetTerrain,
        "SetShapeType",             &CollisionShape::SetShapeType,
        "SetModel",                 &CollisionShape::SetModel,
        "SetLodLevel",              &CollisionShape::SetLodLevel,
        sol::base_classes,          sol::bases<Component>()
        );
    lua.new_usertype<Constraint>("Constraint",
        "id",                   sol::var(StringHash("Constraint")),
        "constraint_type",      sol::property(&Constraint::GetConstraintType, &Constraint::SetConstraintType),
        "position",             sol::property(&Constraint::GetPosition, &Constraint::SetPosition),
        "rotation",             sol::property(&Constraint::GetRotation, &Constraint::SetRotation),
        "other_position",       sol::property(&Constraint::GetOtherPosition, &Constraint::SetOtherPosition),
        "other_rotation",       sol::property(&Constraint::GetRotation, &Constraint::SetOtherRotation),
        "high_limit",           sol::property(&Constraint::GetHighLimit, &Constraint::SetHighLimit),
        "low_limit",            sol::property(&Constraint::GetLowLimit, &Constraint::SetLowLimit),
        "disable_collision",    sol::property(&Constraint::GetDisableCollision, &Constraint::SetDisableCollision),
        "erp",                  sol::property(&Constraint::GetERP, &Constraint::SetERP),
        "cfm",                  sol::property(&Constraint::GetCFM, &Constraint::SetCFM),
        "SetAxis",              &Constraint::SetAxis,
        "SetOtherAxis",         &Constraint::SetOtherAxis,
        "GetWorldPosition",     &Constraint::GetWorldPosition,
        "SetWorldPosition",     &Constraint::SetWorldPosition,
        "SetOtherBody",         &Constraint::SetOtherBody,
        "GetOtherBody",         &Constraint::GetOtherBody,
        "GetOwnBody",           &Constraint::GetOwnBody,
        "ReleaseConstraint",    &Constraint::ReleaseConstraint,
        "ApplyFrames",          &Constraint::ApplyFrames,
        sol::base_classes,      sol::bases<Component>()
        );
    lua.new_usertype<PhysicsRaycastResult>("PhysicsRaycastResult",
        "position",     &PhysicsRaycastResult::position_,
        "normal",       &PhysicsRaycastResult::normal_,
        "distance",     &PhysicsRaycastResult::distance_,
        "hit_fraction", &PhysicsRaycastResult::hitFraction_,
        "rigid_body",   &PhysicsRaycastResult::body_
    );
    lua.new_usertype<PhysicsWorld>("PhysicsWorld",
        "id",               sol::var(StringHash("PhysicsWorld")),
        "fps",              sol::property(&PhysicsWorld::GetFps, &PhysicsWorld::SetFps),
        "gravity",          sol::property(&PhysicsWorld::GetGravity, &PhysicsWorld::SetGravity),
        "max_sub_steps",    sol::property(&PhysicsWorld::GetMaxSubSteps, &PhysicsWorld::SetMaxSubSteps),
        "num_iterations",   sol::property(&PhysicsWorld::GetNumIterations, &PhysicsWorld::SetNumIterations),
        "update_enabled",   sol::property(&PhysicsWorld::IsUpdateEnabled, &PhysicsWorld::SetUpdateEnabled),
        "interpolation",    sol::property(&PhysicsWorld::GetInterpolation, &PhysicsWorld::SetInterpolation),
        "internal_edge",    sol::property(&PhysicsWorld::GetInternalEdge, &PhysicsWorld::SetInternalEdge),
        "split_impulse",    sol::property(&PhysicsWorld::GetSplitImpulse, &PhysicsWorld::SetSplitImpulse),
        "Raycast", sol::overload(
            [](PhysicsWorld* self, const Ray& ray, float maxDistance) {
                ea::vector<PhysicsRaycastResult> result;
                self->Raycast(result, ray, maxDistance);
                std::vector<PhysicsRaycastResult> ret;
                if (!result.empty()) {
                    ret.reserve(result.size());
                    for (const auto& prr : result) {
                        ret.emplace_back(prr);
                    }
                }
                return ret;
            },
            [](PhysicsWorld* self, const Ray& ray, float maxDistance, unsigned collisionMask) {
                ea::vector<PhysicsRaycastResult> result;
                self->Raycast(result, ray, maxDistance, collisionMask);
                std::vector<PhysicsRaycastResult> ret;
                if (!result.empty()) {
                    ret.reserve(result.size());
                    for (const auto& prr : result) {
                        ret.emplace_back(prr);
                    }
                }
                return ret;
            }),
        "RaycastSingle", sol::overload(
            [](PhysicsWorld* self, const Ray& ray, float maxDistance) {
                PhysicsRaycastResult result;
                self->RaycastSingle(result, ray, maxDistance);
                return result;
            },
            [](PhysicsWorld* self, const Ray& ray, float maxDistance, unsigned collisionMask) {
                PhysicsRaycastResult result;
                self->RaycastSingle(result, ray, maxDistance, collisionMask);
                return result;
            }),
        "RaycastSingleSegmented", sol::overload(
            [](PhysicsWorld* self, const Ray& ray, float maxDistance, float segmentDistance) {
                PhysicsRaycastResult result;
                self->RaycastSingleSegmented(result, ray, maxDistance, segmentDistance);
                return result;
            },
            [](PhysicsWorld* self, const Ray& ray, float maxDistance, float segmentDistance, unsigned collisionMask) {
                PhysicsRaycastResult result;
                self->RaycastSingleSegmented(result, ray, maxDistance, segmentDistance, collisionMask);
                return result;
            }),
        "DrawDebugGeometry", sol::resolve<void(bool)>(&PhysicsWorld::DrawDebugGeometry),
        sol::base_classes,  sol::bases<Component>()
        );
    lua.new_usertype<KinematicCharacterController>("KinematicCharacterController",
        "id",                       sol::var(StringHash("KinematicCharacterController")),
        "gravity",                  sol::property(&KinematicCharacterController::GetGravity, &KinematicCharacterController::SetGravity),
        "linear_damping",           sol::property(&KinematicCharacterController::GetLinearDamping, &KinematicCharacterController::SetLinearDamping),
        "angular_damping",          sol::property(&KinematicCharacterController::GetAngularDamping, &KinematicCharacterController::SetAngularDamping),
        "height",                   sol::property(&KinematicCharacterController::GetHeight, &KinematicCharacterController::SetHeight),
        "diameter",                 sol::property(&KinematicCharacterController::GetDiameter, &KinematicCharacterController::SetDiameter),
        "offset",                   sol::property(&KinematicCharacterController::GetOffset, &KinematicCharacterController::SetOffset),
        "step_height",              sol::property(&KinematicCharacterController::GetStepHeight, &KinematicCharacterController::SetStepHeight),
        "max_jump_height",          sol::property(&KinematicCharacterController::GetMaxJumpHeight, &KinematicCharacterController::SetMaxJumpHeight),
        "fall_speed",               sol::property(&KinematicCharacterController::GetFallSpeed, &KinematicCharacterController::SetFallSpeed),
        "jump_speed",               sol::property(&KinematicCharacterController::GetJumpSpeed, &KinematicCharacterController::SetJumpSpeed),
        "max_slope",                sol::property(&KinematicCharacterController::GetMaxSlope, &KinematicCharacterController::SetMaxSlope),
        "linear_velocity",          sol::property(&KinematicCharacterController::GetLinearVelocity, &KinematicCharacterController::SetLinearVelocity),
        "angular_velocity",         sol::property(&KinematicCharacterController::GetAngularVelocity, &KinematicCharacterController::SetAngularVelocity),
        "collision_layer",          sol::property(&KinematicCharacterController::GetCollisionLayer, &KinematicCharacterController::SetCollisionLayer),
        "collision_mask",           sol::property(&KinematicCharacterController::GetCollisionMask, &KinematicCharacterController::SetCollisionMask),
        "SetCollisionLayerAndMask", &KinematicCharacterController::SetCollisionLayerAndMask,
        "SetWalkIncrement",         &KinematicCharacterController::SetWalkIncrement,
        "OnGround",                 &KinematicCharacterController::OnGround,
        "Jump",                     sol::overload(
            [](KinematicCharacterController* self) { self->Jump(); },
            [](KinematicCharacterController* self, const Vector3& jump) { self->Jump(jump); }),
        "ApplyImpulse",             &KinematicCharacterController::ApplyImpulse,
        "CanJump",                  &KinematicCharacterController::CanJump,
        sol::base_classes,          sol::bases<Component>()
        );
	return 0;
}
