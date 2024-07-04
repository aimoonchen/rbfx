#include "../../Core/Context.h"
#include "GetPush.h"

#include "../../Physics2D/CollisionShape2D.h"
#include "../../Physics2D/RigidBody2D.h"
#include "../../Physics2D/PhysicsWorld2D.h"

using namespace Urho3D;

Urho3D::Context* GetContext(lua_State* L);

int sol2_Physics2DLuaAPI_open(sol::state& lua)
{
    auto context = GetContext(lua.lua_state());
    lua.new_enum("BodyType2D",
        "STATIC",       BT_STATIC,
        "KINEMATIC",    BT_KINEMATIC,
        "DYNAMIC",      BT_DYNAMIC
    );
    auto bindCollisionShape2D = lua.new_usertype<CollisionShape2D>("CollisionShape2D",
        sol::base_classes, sol::bases<Component>());
    bindCollisionShape2D["id"]              = sol::var(StringHash("CollisionShape2D"));
    bindCollisionShape2D["SetTrigger"]      = &CollisionShape2D::SetTrigger;
    bindCollisionShape2D["SetCategoryBits"] = &CollisionShape2D::SetCategoryBits;
    bindCollisionShape2D["SetMaskBits"]     = &CollisionShape2D::SetMaskBits;
    bindCollisionShape2D["SetGroupIndex"]   = &CollisionShape2D::SetGroupIndex;
    bindCollisionShape2D["SetDensity"]      = &CollisionShape2D::SetDensity;
    bindCollisionShape2D["SetFriction"]     = &CollisionShape2D::SetFriction;
    bindCollisionShape2D["SetRestitution"]  = &CollisionShape2D::SetRestitution;
    bindCollisionShape2D["CreateFixture"]   = &CollisionShape2D::CreateFixture;
    bindCollisionShape2D["ReleaseFixture"]  = &CollisionShape2D::ReleaseFixture;
    bindCollisionShape2D["IsTrigger"]       = &CollisionShape2D::IsTrigger;
    bindCollisionShape2D["GetCategoryBits"] = &CollisionShape2D::GetCategoryBits;
    bindCollisionShape2D["GetMaskBits"]     = &CollisionShape2D::GetMaskBits;
    bindCollisionShape2D["GetGroupIndex"]   = &CollisionShape2D::GetGroupIndex;
    bindCollisionShape2D["GetDensity"]      = &CollisionShape2D::GetDensity;
    bindCollisionShape2D["GetFriction"]     = &CollisionShape2D::GetFriction;
    bindCollisionShape2D["GetRestitution"]  = &CollisionShape2D::GetRestitution;
    bindCollisionShape2D["GetMass"]         = &CollisionShape2D::GetMass;
    bindCollisionShape2D["GetInertia"]      = &CollisionShape2D::GetInertia;
    bindCollisionShape2D["GetMassCenter"]   = &CollisionShape2D::GetMassCenter;

    auto bindRigidBody2D = lua.new_usertype<RigidBody2D>("RigidBody2D",
        sol::base_classes, sol::bases<Component>());
    bindRigidBody2D["id"]                   = sol::var(StringHash("RigidBody2D"));
    bindRigidBody2D["SetBodyType"]          = &RigidBody2D::SetBodyType;
    bindRigidBody2D["SetMass"]              = &RigidBody2D::SetMass;
    bindRigidBody2D["SetInertia"]           = &RigidBody2D::SetInertia;
    bindRigidBody2D["SetMassCenter"]        = &RigidBody2D::SetMassCenter;
    bindRigidBody2D["SetUseFixtureMass"]    = &RigidBody2D::SetUseFixtureMass;
    bindRigidBody2D["SetLinearDamping"]     = &RigidBody2D::SetLinearDamping;
    bindRigidBody2D["SetAngularDamping"]    = &RigidBody2D::SetAngularDamping;
    bindRigidBody2D["SetAllowSleep"]        = &RigidBody2D::SetAllowSleep;
    bindRigidBody2D["SetFixedRotation"]     = &RigidBody2D::SetFixedRotation;
    bindRigidBody2D["SetBullet"]            = &RigidBody2D::SetBullet;
    bindRigidBody2D["SetGravityScale"]      = &RigidBody2D::SetGravityScale;
    bindRigidBody2D["SetAwake"]             = &RigidBody2D::SetAwake;
    bindRigidBody2D["SetLinearVelocity"]    = &RigidBody2D::SetLinearVelocity;
    bindRigidBody2D["SetAngularVelocity"]   = &RigidBody2D::SetAngularVelocity;
    bindRigidBody2D["ApplyForce"]           = &RigidBody2D::ApplyForce;
    bindRigidBody2D["ApplyForceToCenter"]   = &RigidBody2D::ApplyForceToCenter;
    bindRigidBody2D["ApplyTorque"]          = &RigidBody2D::ApplyTorque;
    bindRigidBody2D["ApplyLinearImpulse"]   = &RigidBody2D::ApplyLinearImpulse;
    bindRigidBody2D["ApplyLinearImpulseToCenter"] = &RigidBody2D::ApplyLinearImpulseToCenter;
    bindRigidBody2D["ApplyAngularImpulse"]  = &RigidBody2D::ApplyAngularImpulse;
    bindRigidBody2D["CreateBody"]           = &RigidBody2D::CreateBody;
    bindRigidBody2D["ReleaseBody"]          = &RigidBody2D::ReleaseBody;
    bindRigidBody2D["ApplyWorldTransform"]  = sol::overload(
        [](RigidBody2D* self) { self->ApplyWorldTransform(); },
        [](RigidBody2D* self, const Vector3& newWorldPosition, const Quaternion& newWorldRotation) { self->ApplyWorldTransform(newWorldPosition, newWorldRotation); });
    bindRigidBody2D["AddCollisionShape2D"]  = &RigidBody2D::AddCollisionShape2D;
    bindRigidBody2D["RemoveCollisionShape2D"] = &RigidBody2D::RemoveCollisionShape2D;
    bindRigidBody2D["AddConstraint2D"]      = &RigidBody2D::AddConstraint2D;
    bindRigidBody2D["RemoveConstraint2D"]   = &RigidBody2D::RemoveConstraint2D;
    bindRigidBody2D["GetBodyType"]          = &RigidBody2D::GetBodyType;
    bindRigidBody2D["GetMass"]              = &RigidBody2D::GetMass;
    bindRigidBody2D["GetInertia"]           = &RigidBody2D::GetInertia;
    bindRigidBody2D["GetMassCenter"]        = &RigidBody2D::GetMassCenter;
    bindRigidBody2D["GetUseFixtureMass"]    = &RigidBody2D::GetUseFixtureMass;
    bindRigidBody2D["GetLinearDamping"]     = &RigidBody2D::GetLinearDamping;
    bindRigidBody2D["GetAngularDamping"]    = &RigidBody2D::GetAngularDamping;
    bindRigidBody2D["IsAllowSleep"]         = &RigidBody2D::IsAllowSleep;
    bindRigidBody2D["IsFixedRotation"]      = &RigidBody2D::IsFixedRotation;
    bindRigidBody2D["IsBullet"]             = &RigidBody2D::IsBullet;
    bindRigidBody2D["GetGravityScale"]      = &RigidBody2D::GetGravityScale;
    bindRigidBody2D["IsAwake"]              = &RigidBody2D::IsAwake;
    bindRigidBody2D["GetLinearVelocity"]    = &RigidBody2D::GetLinearVelocity;
    bindRigidBody2D["GetAngularVelocity"]   = &RigidBody2D:: GetAngularVelocity;

    auto bindPhysicsWorld2D = lua.new_usertype<PhysicsWorld2D>("PhysicsWorld2D",
        sol::base_classes, sol::bases<Component>());
    bindPhysicsWorld2D["id"]                = sol::var(StringHash("PhysicsWorld2D"));
    bindPhysicsWorld2D["SetGravity"]        = &PhysicsWorld2D::SetGravity;
    bindPhysicsWorld2D["SetAutoClearForces"] = &PhysicsWorld2D::SetAutoClearForces;
    bindPhysicsWorld2D["SetVelocityIterations"] = &PhysicsWorld2D::SetVelocityIterations;
    bindPhysicsWorld2D["SetPositionIterations"] = &PhysicsWorld2D::SetPositionIterations;
    bindPhysicsWorld2D["AddRigidBody"] = &PhysicsWorld2D::AddRigidBody;
    bindPhysicsWorld2D["RemoveRigidBody"] = &PhysicsWorld2D::RemoveRigidBody;
    bindPhysicsWorld2D["AddDelayedWorldTransform"] = &PhysicsWorld2D::AddDelayedWorldTransform;
    bindPhysicsWorld2D["AddDelayedWorldTransform"] = &PhysicsWorld2D::AddDelayedWorldTransform;
    bindPhysicsWorld2D["AddDelayedWorldTransform"] = &PhysicsWorld2D::AddDelayedWorldTransform;
    bindPhysicsWorld2D["GetRigidBody"] = sol::overload(
        [](PhysicsWorld2D* self, const Vector2& point) { return self->GetRigidBody(point); },
        [](PhysicsWorld2D* self, const Vector2& point, unsigned collisionMask) { return self->GetRigidBody(point, collisionMask); },
        [](PhysicsWorld2D* self, int screenX, int screenY) { return self->GetRigidBody(screenX, screenY); },
        [](PhysicsWorld2D* self, int screenX, int screenY, unsigned collisionMask) { return self->GetRigidBody(screenX, screenY, collisionMask); });
    bindPhysicsWorld2D["DrawDebugGeometry"] = sol::resolve<void()>(&PhysicsWorld2D::DrawDebugGeometry);

    return 0;
}
