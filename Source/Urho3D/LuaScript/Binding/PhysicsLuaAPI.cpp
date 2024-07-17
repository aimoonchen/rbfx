#include "../../Core/Context.h"
#include "GetPush.h"
#include "../../Math/Ray.h"
#include "../../Physics/PhysicsWorld.h"
#include "../../Physics/RigidBody.h"
#include "../../Physics/CollisionShape.h"
#include "../../Physics/Constraint.h"
#include "../../Physics/PhysicsEvents.h"
#include "../../Physics/KinematicCharacterController.h"
using namespace Urho3D;

Urho3D::Context* GetContext(lua_State* L);

int sol2_PhysicsLuaAPI_open(sol::state& lua)
{
    auto eventType = lua["EventType"].get_or_create<sol::table>();
    eventType["PhysicsPreUpdate"] = E_PHYSICSPREUPDATE;
    eventType["PhysicsPostUpdate"] = E_PHYSICSPOSTUPDATE;
    eventType["PhysicsPreStep"] = E_PHYSICSPRESTEP;
    eventType["PhysicsPostStep"] = E_PHYSICSPOSTSTEP;
    eventType["PhysicsCollisionStart"] = E_PHYSICSCOLLISIONSTART;
    eventType["PhysicsCollision"] = E_PHYSICSCOLLISION;
    eventType["PhysicsCollisionEnd"] = E_PHYSICSCOLLISIONEND;
    eventType["NodeCollisionStart"] = E_NODECOLLISIONSTART;
    eventType["NodeCollision"] = E_NODECOLLISION;
    eventType["NodeCollisionEnd"] = E_NODECOLLISIONEND;

    auto paramType = lua["ParamType"].get_or_create<sol::table>();

    auto physicsPreUpdate = paramType["PhysicsPreUpdate"].get_or_create<sol::table>();
    physicsPreUpdate["World"]       = PhysicsPreUpdate::P_WORLD;
    physicsPreUpdate["TimeStep"]    = PhysicsPreUpdate::P_TIMESTEP;

    auto physicsPostUpdate = paramType["PhysicsPostUpdate"].get_or_create<sol::table>();
    physicsPostUpdate["World"]      = PhysicsPostUpdate::P_WORLD;
    physicsPostUpdate["TimeStep"]   = PhysicsPostUpdate::P_TIMESTEP;
    physicsPostUpdate["Overtime"]   = PhysicsPostUpdate::P_OVERTIME;

    auto physicsPreStep = paramType["PhysicsPreStep"].get_or_create<sol::table>();
    physicsPreStep["World"]         = PhysicsPreStep::P_WORLD;
    physicsPreStep["TimeStep"]      = PhysicsPreStep::P_TIMESTEP;
    physicsPreStep["NetworkFrame"]  = PhysicsPreStep::P_NETWORKFRAME;

    auto physicsPostStep = paramType["PhysicsPostStep"].get_or_create<sol::table>();
    physicsPostStep["World"]        = PhysicsPostStep::P_WORLD;
    physicsPostStep["TimeStep"]     = PhysicsPostStep::P_TIMESTEP;

    auto physicsCollisionStart = paramType["PhysicsCollisionStart"].get_or_create<sol::table>();
    physicsCollisionStart["World"]      = PhysicsCollisionStart::P_WORLD;
    physicsCollisionStart["NodeA"]      = PhysicsCollisionStart::P_NODEA;
    physicsCollisionStart["NodeB"]      = PhysicsCollisionStart::P_NODEB;
    physicsCollisionStart["BodyA"]      = PhysicsCollisionStart::P_BODYA;
    physicsCollisionStart["BodyB"]      = PhysicsCollisionStart::P_BODYB;
    physicsCollisionStart["Trigger"]    = PhysicsCollisionStart::P_TRIGGER;
    physicsCollisionStart["Contacts"]   = PhysicsCollisionStart::P_CONTACTS;

    auto physicsCollision = paramType["PhysicsCollision"].get_or_create<sol::table>();
    physicsCollision["World"]       = PhysicsCollision::P_WORLD;
    physicsCollision["NodeA"]       = PhysicsCollision::P_NODEA;
    physicsCollision["NodeB"]       = PhysicsCollision::P_NODEB;
    physicsCollision["BodyA"]       = PhysicsCollision::P_BODYA;
    physicsCollision["BodyB"]       = PhysicsCollision::P_BODYB;
    physicsCollision["Trigger"]     = PhysicsCollision::P_TRIGGER;
    physicsCollision["Contacts"]    = PhysicsCollision::P_CONTACTS;

    auto physicsCollisionEnd = paramType["PhysicsCollisionEnd"].get_or_create<sol::table>();
    physicsCollisionEnd["World"]      = PhysicsCollisionEnd::P_WORLD;
    physicsCollisionEnd["NodeA"]      = PhysicsCollisionEnd::P_NODEA;
    physicsCollisionEnd["NodeB"]      = PhysicsCollisionEnd::P_NODEB;
    physicsCollisionEnd["BodyA"]      = PhysicsCollisionEnd::P_BODYA;
    physicsCollisionEnd["BodyB"]      = PhysicsCollisionEnd::P_BODYB;
    physicsCollisionEnd["Trigger"]    = PhysicsCollisionEnd::P_TRIGGER;

    auto nodeCollisionStart = paramType["NodeCollisionStart"].get_or_create<sol::table>();
    nodeCollisionStart["Body"]      = NodeCollisionStart::P_BODY;
    nodeCollisionStart["OtherNode"] = NodeCollisionStart::P_OTHERNODE;
    nodeCollisionStart["OtherBody"] = NodeCollisionStart::P_OTHERBODY;
    nodeCollisionStart["Trigger"]   = NodeCollisionStart::P_TRIGGER;
    nodeCollisionStart["Contacts"]  = NodeCollisionStart::P_CONTACTS;

    auto nodeCollision = paramType["NodeCollision"].get_or_create<sol::table>();
    nodeCollision["Body"]      = NodeCollision::P_BODY;
    nodeCollision["OtherNode"] = NodeCollision::P_OTHERNODE;
    nodeCollision["OtherBody"] = NodeCollision::P_OTHERBODY;
    nodeCollision["Trigger"]   = NodeCollision::P_TRIGGER;
    nodeCollision["Contacts"]  = NodeCollision::P_CONTACTS;

    auto nodeCollisionEnd = paramType["NodeCollisionEnd"].get_or_create<sol::table>();
    nodeCollisionEnd["Body"]      = NodeCollisionEnd::P_BODY;
    nodeCollisionEnd["OtherNode"] = NodeCollisionEnd::P_OTHERNODE;
    nodeCollisionEnd["OtherBody"] = NodeCollisionEnd::P_OTHERBODY;
    nodeCollisionEnd["Trigger"]   = NodeCollisionEnd::P_TRIGGER;

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
        
    auto bindRigidBody = lua.new_usertype<RigidBody>("RigidBody", sol::base_classes, sol::bases<Component>());
    bindRigidBody["id"]                       = sol::var(StringHash("RigidBody"));
    bindRigidBody["position"]                 = sol::property(&RigidBody::GetPosition, &RigidBody::SetPosition);
    bindRigidBody["rotation"]                 = sol::property(&RigidBody::GetRotation, &RigidBody::SetRotation);
    bindRigidBody["mass"]                     = sol::property(&RigidBody::GetMass, &RigidBody::SetMass);
    bindRigidBody["friction"]                 = sol::property(&RigidBody::GetFriction, &RigidBody::SetFriction);
    bindRigidBody["rolling_friction"]         = sol::property(&RigidBody::GetRollingFriction, &RigidBody::SetRollingFriction);
    bindRigidBody["restitution"]              = sol::property(&RigidBody::GetRestitution, &RigidBody::SetRestitution);
    bindRigidBody["contact_threshold"]        = sol::property(&RigidBody::GetContactProcessingThreshold, &RigidBody::SetContactProcessingThreshold);
    bindRigidBody["angular_velocity"]         = sol::property(&RigidBody::GetAngularVelocity, &RigidBody::SetAngularVelocity);
    bindRigidBody["angular_factor"]           = sol::property(&RigidBody::GetAngularFactor, &RigidBody::SetAngularFactor);
    bindRigidBody["angular_rest_threshold"]   = sol::property(&RigidBody::GetAngularRestThreshold, &RigidBody::SetAngularRestThreshold);
    bindRigidBody["angular_damping"]          = sol::property(&RigidBody::GetAngularDamping, &RigidBody::SetAngularDamping);
    bindRigidBody["linear_velocity"]          = sol::property(&RigidBody::GetLinearVelocity, &RigidBody::SetLinearVelocity);
    bindRigidBody["linear_factor"]            = sol::property(&RigidBody::GetLinearFactor, &RigidBody::SetLinearFactor);
    bindRigidBody["linear_rest_threshold"]    = sol::property(&RigidBody::GetLinearRestThreshold, &RigidBody::SetLinearRestThreshold);
    bindRigidBody["linear_damping"]           = sol::property(&RigidBody::GetLinearDamping, &RigidBody::SetLinearDamping);
    bindRigidBody["collision_layer"]          = sol::property(&RigidBody::GetCollisionLayer, &RigidBody::SetCollisionLayer);
    bindRigidBody["collision_mask"]           = sol::property(&RigidBody::GetCollisionMask, &RigidBody::SetCollisionMask);
    bindRigidBody["SetCollisionLayerAndMask"] = &RigidBody::SetCollisionLayerAndMask;
    bindRigidBody["SetCcdRadius"]             = &RigidBody::SetCcdRadius;
    bindRigidBody["SetCcdMotionThreshold"]    = &RigidBody::SetCcdMotionThreshold;
    bindRigidBody["SetUseGravity"]            = &RigidBody::SetUseGravity;
    bindRigidBody["SetGravityOverride"]       = &RigidBody::SetGravityOverride;
    bindRigidBody["SetKinematic"]             = &RigidBody::SetKinematic;
    bindRigidBody["SetTrigger"]               = &RigidBody::SetTrigger;
    bindRigidBody["ApplyTorque"]              = &RigidBody::ApplyTorque;
    bindRigidBody["SetTransform"]             = &RigidBody::SetTransform;
    bindRigidBody["ApplyTorque"]              = &RigidBody::ApplyTorque;
    bindRigidBody["ApplyForce"]               = sol::overload(
        [](RigidBody* self, const Vector3& force) { self->ApplyForce(force); },
        [](RigidBody* self, const Vector3& force, const Vector3& position) { self->ApplyForce(force, position); });
    bindRigidBody["ApplyImpulse"]             = sol::overload(
        [](RigidBody* self, const Vector3& impulse) { self->ApplyImpulse(impulse); },
        [](RigidBody* self, const Vector3& impulse, const Vector3& position) { self->ApplyImpulse(impulse, position); });
    bindRigidBody["ApplyTorqueImpulse"]       = &RigidBody::ApplyTorqueImpulse;
    bindRigidBody["ResetForces"]              = &RigidBody::ResetForces;
    bindRigidBody["Activate"]                 = &RigidBody::Activate;
    bindRigidBody["ReAddBodyToWorld"]         = &RigidBody::ReAddBodyToWorld;
    bindRigidBody["DisableMassUpdate"]        = &RigidBody::DisableMassUpdate;
    bindRigidBody["EnableMassUpdate"]         = &RigidBody::EnableMassUpdate;
    bindRigidBody["SetCollisionEventMode"]    = &RigidBody::SetCollisionEventMode;

    auto bindCollisionShape = lua.new_usertype<CollisionShape>("CollisionShape", sol::base_classes, sol::bases<Component>());
    bindCollisionShape["id"]                       = sol::var(StringHash("CollisionShape"));
    bindCollisionShape["size"]                     = sol::property(&CollisionShape::GetSize, &CollisionShape::SetSize);
    bindCollisionShape["margin"]                   = sol::property(&CollisionShape::GetMargin, &CollisionShape::SetMargin);
    bindCollisionShape["position"]                 = sol::property(&CollisionShape::GetPosition, &CollisionShape::SetPosition);
    bindCollisionShape["rotation"]                 = sol::property(&CollisionShape::GetRotation, &CollisionShape::SetRotation);
    bindCollisionShape["SetBox"]                   = sol::overload(
        [](CollisionShape* self, const Vector3& size) { self->SetBox(size); },
        [](CollisionShape* self, const Vector3& size, const Vector3& position, const Quaternion& rotation) { self->SetBox(size, position, rotation); });
    bindCollisionShape["SetSphere"]                = sol::overload(
        [](CollisionShape* self, float diameter) { self->SetSphere(diameter); },
        [](CollisionShape* self, float diameter, const Vector3& position, const Quaternion& rotation) { self->SetSphere(diameter, position, rotation); });
    bindCollisionShape["SetStaticPlane"]           = sol::overload(
        [](CollisionShape* self) { self->SetStaticPlane(); },
        [](CollisionShape* self, const Vector3& position, const Quaternion& rotation) { self->SetStaticPlane(position, rotation); });
    bindCollisionShape["SetCylinder"]              = sol::overload(
        [](CollisionShape* self, float diameter, float height) { self->SetCylinder(diameter, height); },
        [](CollisionShape* self, float diameter, float height, const Vector3& position, const Quaternion& rotation) { self->SetCylinder(diameter, height, position, rotation); });
    bindCollisionShape["SetCapsule"]               = sol::overload(
        [](CollisionShape* self, float diameter, float height) { self->SetCapsule(diameter, height); },
        [](CollisionShape* self, float diameter, float height, const Vector3& position, const Quaternion& rotation) { self->SetCapsule(diameter, height, position, rotation); });
    bindCollisionShape["SetCone"]                  = sol::overload(
        [](CollisionShape* self, float diameter, float height) { self->SetCone(diameter, height); },
        [](CollisionShape* self, float diameter, float height, const Vector3& position, const Quaternion& rotation) { self->SetCone(diameter, height, position, rotation); });
    bindCollisionShape["SetTriangleMesh"]          = sol::overload(
        [](CollisionShape* self, Model* model) { self->SetTriangleMesh(model); },
        [](CollisionShape* self, Model* model, unsigned lodLevel, const Vector3& scale, const Vector3& position, const Quaternion& rotation) { self->SetTriangleMesh(model, lodLevel, scale, position, rotation); });
    bindCollisionShape["SetConvexHull"]            = sol::overload(
        [](CollisionShape* self, Model* model) { self->SetConvexHull(model); },
        [](CollisionShape* self, Model* model, unsigned lodLevel, const Vector3& scale, const Vector3& position, const Quaternion& rotation) { self->SetConvexHull(model, lodLevel, scale, position, rotation); });
    bindCollisionShape["SetCustomTriangleMesh"]    = sol::overload(
        [](CollisionShape* self, CustomGeometry* custom) { self->SetCustomTriangleMesh(custom); },
        [](CollisionShape* self, CustomGeometry* custom, const Vector3& scale, const Vector3& position, const Quaternion& rotation) { self->SetCustomTriangleMesh(custom, scale, position, rotation); });
    bindCollisionShape["SetCustomConvexHull"]      = sol::overload(
        [](CollisionShape* self, CustomGeometry* custom) { self->SetCustomConvexHull(custom); },
        [](CollisionShape* self, CustomGeometry* custom, const Vector3& scale, const Vector3& position, const Quaternion& rotation) { self->SetCustomConvexHull(custom, scale, position, rotation); });
    bindCollisionShape["SetTerrain"]               = &CollisionShape::SetTerrain;
    bindCollisionShape["SetShapeType"]             = &CollisionShape::SetShapeType;
    bindCollisionShape["SetModel"]                 = &CollisionShape::SetModel;
    bindCollisionShape["SetLodLevel"]              = &CollisionShape::SetLodLevel;

    auto bindConstraint = lua.new_usertype<Constraint>("Constraint", sol::base_classes, sol::bases<Component>());
    bindConstraint["id"]                   = sol::var(StringHash("Constraint"));
    bindConstraint["constraint_type"]      = sol::property(&Constraint::GetConstraintType, &Constraint::SetConstraintType);
    bindConstraint["position"]             = sol::property(&Constraint::GetPosition, &Constraint::SetPosition);
    bindConstraint["rotation"]             = sol::property(&Constraint::GetRotation, &Constraint::SetRotation);
    bindConstraint["other_position"]       = sol::property(&Constraint::GetOtherPosition, &Constraint::SetOtherPosition);
    bindConstraint["other_rotation"]       = sol::property(&Constraint::GetRotation, &Constraint::SetOtherRotation);
    bindConstraint["high_limit"]           = sol::property(&Constraint::GetHighLimit, &Constraint::SetHighLimit);
    bindConstraint["low_limit"]            = sol::property(&Constraint::GetLowLimit, &Constraint::SetLowLimit);
    bindConstraint["disable_collision"]    = sol::property(&Constraint::GetDisableCollision, &Constraint::SetDisableCollision);
    bindConstraint["erp"]                  = sol::property(&Constraint::GetERP, &Constraint::SetERP);
    bindConstraint["cfm"]                  = sol::property(&Constraint::GetCFM, &Constraint::SetCFM);
    bindConstraint["SetAxis"]              = &Constraint::SetAxis;
    bindConstraint["SetOtherAxis"]         = &Constraint::SetOtherAxis;
    bindConstraint["GetWorldPosition"]     = &Constraint::GetWorldPosition;
    bindConstraint["SetWorldPosition"]     = &Constraint::SetWorldPosition;
    bindConstraint["SetOtherBody"]         = &Constraint::SetOtherBody;
    bindConstraint["GetOtherBody"]         = &Constraint::GetOtherBody;
    bindConstraint["GetOwnBody"]           = &Constraint::GetOwnBody;
    bindConstraint["ReleaseConstraint"]    = &Constraint::ReleaseConstraint;
    bindConstraint["ApplyFrames"]          = &Constraint::ApplyFrames;

    auto bindPhysicsRaycastResult = lua.new_usertype<PhysicsRaycastResult>("PhysicsRaycastResult");
    bindPhysicsRaycastResult["position"] = &PhysicsRaycastResult::position_;
    bindPhysicsRaycastResult["normal"] = &PhysicsRaycastResult::normal_;
    bindPhysicsRaycastResult["distance"] = &PhysicsRaycastResult::distance_;
    bindPhysicsRaycastResult["hit_fraction"] = &PhysicsRaycastResult::hitFraction_;
    bindPhysicsRaycastResult["rigid_body"] = &PhysicsRaycastResult::body_;
    
    auto bindPhysicsWorld = lua.new_usertype<PhysicsWorld>("PhysicsWorld", sol::base_classes, sol::bases<Component>());
    bindPhysicsWorld["id"]               = sol::var(StringHash("PhysicsWorld"));
    bindPhysicsWorld["fps"]              = sol::property(&PhysicsWorld::GetFps, &PhysicsWorld::SetFps);
    bindPhysicsWorld["gravity"]          = sol::property(&PhysicsWorld::GetGravity, &PhysicsWorld::SetGravity);
    bindPhysicsWorld["max_sub_steps"]    = sol::property(&PhysicsWorld::GetMaxSubSteps, &PhysicsWorld::SetMaxSubSteps);
    bindPhysicsWorld["num_iterations"]   = sol::property(&PhysicsWorld::GetNumIterations, &PhysicsWorld::SetNumIterations);
    bindPhysicsWorld["update_enabled"]   = sol::property(&PhysicsWorld::IsUpdateEnabled, &PhysicsWorld::SetUpdateEnabled);
    bindPhysicsWorld["interpolation"]    = sol::property(&PhysicsWorld::GetInterpolation, &PhysicsWorld::SetInterpolation);
    bindPhysicsWorld["internal_edge"]    = sol::property(&PhysicsWorld::GetInternalEdge, &PhysicsWorld::SetInternalEdge);
    bindPhysicsWorld["split_impulse"]    = sol::property(&PhysicsWorld::GetSplitImpulse, &PhysicsWorld::SetSplitImpulse);
    bindPhysicsWorld["Raycast"]          = sol::overload(
        [](PhysicsWorld* self, const Ray& ray, float maxDistance) {
            ea::vector<PhysicsRaycastResult> result;
            self->Raycast(result, ray, maxDistance);
            return std::vector<PhysicsRaycastResult>(result.begin(), result.end());
        },
        [](PhysicsWorld* self, const Ray& ray, float maxDistance, unsigned collisionMask) {
            ea::vector<PhysicsRaycastResult> result;
            self->Raycast(result, ray, maxDistance, collisionMask);
            return std::vector<PhysicsRaycastResult>(result.begin(), result.end());
        });
    bindPhysicsWorld["RaycastSingle"]   = sol::overload(
        [](PhysicsWorld* self, const Ray& ray, float maxDistance) {
            PhysicsRaycastResult result;
            self->RaycastSingle(result, ray, maxDistance);
            return result;
        },
        [](PhysicsWorld* self, const Ray& ray, float maxDistance, unsigned collisionMask) {
            PhysicsRaycastResult result;
            self->RaycastSingle(result, ray, maxDistance, collisionMask);
            return result;
        });
    bindPhysicsWorld["RaycastSingleSegmented"]  = sol::overload(
        [](PhysicsWorld* self, const Ray& ray, float maxDistance, float segmentDistance) {
            PhysicsRaycastResult result;
            self->RaycastSingleSegmented(result, ray, maxDistance, segmentDistance);
            return result;
        },
        [](PhysicsWorld* self, const Ray& ray, float maxDistance, float segmentDistance, unsigned collisionMask) {
            PhysicsRaycastResult result;
            self->RaycastSingleSegmented(result, ray, maxDistance, segmentDistance, collisionMask);
            return result;
        });
    bindPhysicsWorld["SphereCast"] = sol::overload(
        [](PhysicsWorld* self, const Ray& ray, float radius, float maxDistance) {
            PhysicsRaycastResult result;
            self->SphereCast(result, ray, radius, maxDistance);
            return result;
        },
        [](PhysicsWorld* self, const Ray& ray, float radius, float maxDistance, unsigned collisionMask) {
            PhysicsRaycastResult result;
            self->SphereCast(result, ray, radius, maxDistance, collisionMask);
            return result;
        });
    bindPhysicsWorld["ConvexCast"] = sol::overload(
        [](PhysicsWorld* self, CollisionShape* shape, const Vector3& startPos, const Quaternion& startRot, const Vector3& endPos, const Quaternion& endRot) {
            PhysicsRaycastResult result;
            self->ConvexCast(result, shape, startPos, startRot, endPos, endRot);
            return result;
        },
        [](PhysicsWorld* self, CollisionShape* shape, const Vector3& startPos, const Quaternion& startRot, const Vector3& endPos, const Quaternion& endRot, unsigned collisionMask) {
            PhysicsRaycastResult result;
            self->ConvexCast(result, shape, startPos, startRot, endPos, endRot, collisionMask);
            return result;
        });
    bindPhysicsWorld["RemoveCachedGeometry"]    = &PhysicsWorld::RemoveCachedGeometry;
    bindPhysicsWorld["DrawDebugGeometry"]       = sol::resolve<void(bool)>(&PhysicsWorld::DrawDebugGeometry);
    bindPhysicsWorld["GetRigidBodies"]          = sol::overload(
        [](PhysicsWorld* self, const Sphere& sphere) {
            ea::vector<RigidBody*> out;
            self->GetRigidBodies(out, sphere);
            return std::vector<RigidBody*>(out.begin(), out.end());
        },
        [](PhysicsWorld* self, const BoundingBox& box) {
            ea::vector<RigidBody*> out;
            self->GetRigidBodies(out, box);
            return std::vector<RigidBody*>(out.begin(), out.end());
        },
        [](PhysicsWorld* self, const Sphere& sphere, unsigned collisionMask) {
            ea::vector<RigidBody*> out;
            self->GetRigidBodies(out, sphere, collisionMask);
            return std::vector<RigidBody*>(out.begin(), out.end());
        },
        [](PhysicsWorld* self, const BoundingBox& box, unsigned collisionMask) {
            ea::vector<RigidBody*> out;
            self->GetRigidBodies(out, box, collisionMask);
            return std::vector<RigidBody*>(out.begin(), out.end());
        },
        [](PhysicsWorld* self, const RigidBody* body) {
            ea::vector<RigidBody*> out;
            self->GetRigidBodies(out, body);
            return std::vector<RigidBody*>(out.begin(), out.end());
        });
    bindPhysicsWorld["GetCollidingBodies"] = [](PhysicsWorld* self, const RigidBody* body) {
            ea::vector<RigidBody*> out;
            self->GetCollidingBodies(out, body);
            return std::vector<RigidBody*>(out.begin(), out.end());
        };

    auto bindKinematicCharacterController = lua.new_usertype<KinematicCharacterController>("KinematicCharacterController", sol::base_classes, sol::bases<Component>());
    bindKinematicCharacterController["id"]                       = sol::var(StringHash("KinematicCharacterController"));
    bindKinematicCharacterController["gravity"]                  = sol::property(&KinematicCharacterController::GetGravity, &KinematicCharacterController::SetGravity);
    bindKinematicCharacterController["linear_damping"]           = sol::property(&KinematicCharacterController::GetLinearDamping, &KinematicCharacterController::SetLinearDamping);
    bindKinematicCharacterController["angular_damping"]          = sol::property(&KinematicCharacterController::GetAngularDamping, &KinematicCharacterController::SetAngularDamping);
    bindKinematicCharacterController["height"]                   = sol::property(&KinematicCharacterController::GetHeight, &KinematicCharacterController::SetHeight);
    bindKinematicCharacterController["diameter"]                 = sol::property(&KinematicCharacterController::GetDiameter, &KinematicCharacterController::SetDiameter);
    bindKinematicCharacterController["offset"]                   = sol::property(&KinematicCharacterController::GetOffset, &KinematicCharacterController::SetOffset);
    bindKinematicCharacterController["step_height"]              = sol::property(&KinematicCharacterController::GetStepHeight, &KinematicCharacterController::SetStepHeight);
    bindKinematicCharacterController["max_jump_height"]          = sol::property(&KinematicCharacterController::GetMaxJumpHeight, &KinematicCharacterController::SetMaxJumpHeight);
    bindKinematicCharacterController["fall_speed"]               = sol::property(&KinematicCharacterController::GetFallSpeed, &KinematicCharacterController::SetFallSpeed);
    bindKinematicCharacterController["jump_speed"]               = sol::property(&KinematicCharacterController::GetJumpSpeed, &KinematicCharacterController::SetJumpSpeed);
    bindKinematicCharacterController["max_slope"]                = sol::property(&KinematicCharacterController::GetMaxSlope, &KinematicCharacterController::SetMaxSlope);
    bindKinematicCharacterController["linear_velocity"]          = sol::property(&KinematicCharacterController::GetLinearVelocity, &KinematicCharacterController::SetLinearVelocity);
    bindKinematicCharacterController["angular_velocity"]         = sol::property(&KinematicCharacterController::GetAngularVelocity, &KinematicCharacterController::SetAngularVelocity);
    bindKinematicCharacterController["collision_layer"]          = sol::property(&KinematicCharacterController::GetCollisionLayer, &KinematicCharacterController::SetCollisionLayer);
    bindKinematicCharacterController["collision_mask"]           = sol::property(&KinematicCharacterController::GetCollisionMask, &KinematicCharacterController::SetCollisionMask);
    bindKinematicCharacterController["SetCollisionLayerAndMask"] = &KinematicCharacterController::SetCollisionLayerAndMask;
    bindKinematicCharacterController["SetWalkIncrement"]         = &KinematicCharacterController::SetWalkIncrement;
    bindKinematicCharacterController["OnGround"]                 = &KinematicCharacterController::OnGround;
    bindKinematicCharacterController["Jump"]                     = sol::overload(
        [](KinematicCharacterController* self) { self->Jump(); },
        [](KinematicCharacterController* self, const Vector3& jump) { self->Jump(jump); });
    bindKinematicCharacterController["ApplyImpulse"]             = &KinematicCharacterController::ApplyImpulse;
    bindKinematicCharacterController["CanJump"]                  = &KinematicCharacterController::CanJump;
        
	return 0;
}
