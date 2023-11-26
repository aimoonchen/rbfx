#include "GetPush.h"
#include "../../Actions/ActionState.h"
#include "../../Actions/ActionBuilder.h"
#include "../../Actions/ActionManager.h"
#include "../../Scene/Component.h"
#include "../../Scene/Node.h"

using namespace Urho3D;
Urho3D::Context* GetContext(lua_State* L);

int sol2_ActionLuaAPI_open(sol::state& solLua)
{
    using namespace Actions;
    auto context = GetContext(solLua.lua_state());

    solLua.new_usertype<BaseAction>("BaseAction");

    solLua.new_usertype<FiniteTimeAction>("FiniteTimeAction",
        sol::base_classes, sol::bases<BaseAction>());

    solLua.new_usertype<ActionState>("ActionState",
        "GetTarget", &ActionState::GetTarget,
        "GetOriginalTarget", &ActionState::GetOriginalTarget,
        "GetAction", &ActionState::GetAction,
        "IsDone", &ActionState::IsDone);

    solLua.new_usertype<ActionBuilder>("ActionBuilder",
        sol::call_constructor, sol::factories([context]() { return ActionBuilder(context); }),
        "Then", [](ActionBuilder* self, FiniteTimeAction* nextAction) { return self->Then(SharedPtr<Actions::FiniteTimeAction>(nextAction)); },
        "Also", [](ActionBuilder* self, FiniteTimeAction* nextAction) { return self->Also(SharedPtr<Actions::FiniteTimeAction>(nextAction)); },
        "MoveBy", sol::overload(
            [](ActionBuilder* self, float duration, const Vector3& offset) { return self->MoveBy(duration, offset); },
            [](ActionBuilder* self, float duration, const Vector3& offset, const char* attributeName) { return self->MoveBy(duration, offset, attributeName); },
            [](ActionBuilder* self, float duration, const Vector2& offset) { return self->MoveBy(duration, offset); },
            [](ActionBuilder* self, float duration, const Vector2& offset, const char* attributeName) { return self->MoveBy(duration, offset, attributeName); }),
        "MoveByQuadratic", sol::overload(
            [](ActionBuilder* self, float duration, const Vector3& controlOffset, const Vector3& targetOffset) { return self->MoveByQuadratic(duration, controlOffset, targetOffset); },
            [](ActionBuilder* self, float duration, const Vector3& controlOffset, const Vector3& targetOffset, const char* attributeName) { return self->MoveByQuadratic(duration, controlOffset, targetOffset, attributeName); },
            [](ActionBuilder* self, float duration, const Vector2& controlOffset, const Vector2& targetOffset) { return self->MoveByQuadratic(duration, controlOffset, targetOffset); },
            [](ActionBuilder* self, float duration, const Vector2& controlOffset, const Vector2& targetOffset, const char* attributeName) { return self->MoveByQuadratic(duration, controlOffset, targetOffset, attributeName); }),
        "JumpBy", sol::overload(
            [](ActionBuilder* self, const Vector3& offset) { return self->JumpBy(offset); },
            [](ActionBuilder* self, const Vector3& offset, const char* attributeName) { return self->JumpBy(offset, attributeName); },
            [](ActionBuilder* self, const Vector2& offset) { return self->JumpBy(offset); },
            [](ActionBuilder* self, const Vector2& offset, const char* attributeName) { return self->JumpBy(offset, attributeName); }),
        "ScaleBy", sol::overload(
            [](ActionBuilder* self, float duration, const Vector3& delta) { return self->ScaleBy(duration, delta); },
            [](ActionBuilder* self, float duration, const Vector3& delta, const char* attributeName) { return self->ScaleBy(duration, delta, attributeName); },
            [](ActionBuilder* self, float duration, const Vector2& delta) { return self->ScaleBy(duration, delta); },
            [](ActionBuilder* self, float duration, const Vector2& delta, const char* attributeName) { return self->ScaleBy(duration, delta, attributeName); }),
        "RotateBy", sol::overload(
            [](ActionBuilder* self, float duration, const Quaternion& delta) { return self->RotateBy(duration, delta); },
            [](ActionBuilder* self, float duration, const Quaternion& delta, const char* attributeName) { return self->RotateBy(duration, delta, attributeName); }),
        "RotateAround", &ActionBuilder::RotateAround,
        "Hide", sol::overload(
            [](ActionBuilder* self) { return self->Hide(); },
            [](ActionBuilder* self, const char* attributeName) { return self->Hide(attributeName); }),
        "Show", sol::overload(
            [](ActionBuilder* self) { return self->Show(); },
            [](ActionBuilder* self, const char* attributeName) { return self->Show(attributeName); }),
        "Enable", sol::overload(
            [](ActionBuilder* self) { return self->Enable(); },
            [](ActionBuilder* self, const char* attributeName) { return self->Enable(attributeName); }),
        "Disable", sol::overload(
            [](ActionBuilder* self) { return self->Disable(); },
            [](ActionBuilder* self, const char* attributeName) { return self->Disable(attributeName); }),
        "Blink", sol::overload(
            [](ActionBuilder* self, float duration, unsigned numOfBlinks) { return self->Blink(duration, numOfBlinks); },
            [](ActionBuilder* self, float duration, unsigned numOfBlinks, const char* attributeName) { return self->Blink(duration, numOfBlinks, attributeName); }),
        "AttributeTo", [](ActionBuilder* self, float duration, const char* attributeName, const Variant& to) { return self->AttributeTo(duration, attributeName, to); },
        "AttributeFromTo", [](ActionBuilder* self, float duration, const char* attributeName, const Variant& from, const Variant& to) { return self->AttributeFromTo(duration, attributeName, from, to); },
        "ShaderParameterTo", [](ActionBuilder* self, float duration, const char* attributeName, const Variant& to) { return self->ShaderParameterTo(duration, attributeName, to); },
        "ShaderParameterFromTo", [](ActionBuilder* self, float duration, const char* attributeName, const Variant& from, const Variant& to) { return self->ShaderParameterFromTo(duration, attributeName, from, to); },
        "BackIn", &ActionBuilder::BackIn,
        "BackOut", &ActionBuilder::BackOut,
        "BackInOut", &ActionBuilder::BackInOut,
        "BounceOut", &ActionBuilder::BounceOut,
        "BounceIn", &ActionBuilder::BounceIn,
        "BounceInOut", &ActionBuilder::BounceInOut,
        "SineOut", &ActionBuilder::SineOut,
        "SineIn", &ActionBuilder::SineIn,
        "SineInOut", &ActionBuilder::SineInOut,
        "ExponentialOut", &ActionBuilder::ExponentialOut,
        "ExponentialIn", &ActionBuilder::ExponentialIn,
        "ExponentialInOut", &ActionBuilder::ExponentialInOut,
        "ElasticIn", sol::overload([](ActionBuilder* self) { return self->ElasticIn(); }, [](ActionBuilder* self, float period) { return self->ElasticIn(period); }),
        "ElasticOut", sol::overload([](ActionBuilder* self) { return self->ElasticOut(); }, [](ActionBuilder* self, float period) { return self->ElasticOut(period); }),
        "ElasticInOut", sol::overload([](ActionBuilder* self) { return self->ElasticInOut(); }, [](ActionBuilder* self, float period) { return self->ElasticInOut(period); }),
        "RemoveSelf", &ActionBuilder::RemoveSelf,
        "DelayTime", &ActionBuilder::DelayTime,
        "Repeat", &ActionBuilder::Repeat,
        "RepeatForever", &ActionBuilder::RepeatForever,
        "Build", [](ActionBuilder* self) { return (FiniteTimeAction*)self->Build().Detach(); },
        "Run", sol::overload([](ActionBuilder* self, Object* target) { return self->Run(target); }, [](ActionBuilder* self, ActionManager* actionManager, Object* target) { return self->Run(actionManager, target); }));

    solLua.new_usertype<ActionManager>("ActionManager",
        "CompleteAllActions", &ActionManager::CompleteAllActions,
        "CancelAllActions", &ActionManager::CancelAllActions,
        "CompleteAllActionsOnTarget", &ActionManager::CompleteAllActionsOnTarget,
        "CancelAllActionsFromTarget", &ActionManager::CancelAllActionsFromTarget,
        "CancelAction", &ActionManager::CancelAction,
        "GetNumActions", &ActionManager::GetNumActions,
        "AddAction", sol::overload(
            [](ActionManager* self, Actions::BaseAction* action, Node* target) { return self->AddAction(action, target); },
            [](ActionManager* self, Actions::BaseAction* action, Node* target, bool paused) { self->AddAction(action, target, paused); },
            [](ActionManager* self, Actions::BaseAction* action, Component* target) { return self->AddAction(action, target); },
            [](ActionManager* self, Actions::BaseAction* action, Component* target, bool paused) { self->AddAction(action, target, paused); }),
        "GetEmptyAction", &ActionManager::GetEmptyAction,
        sol::base_classes, sol::bases<Object>());

    solLua["action_manager"] = context->GetSubsystem<ActionManager>();
    return 0;
}
