#include <nanobind/nanobind.h>
#include "../../Actions/ActionState.h"
#include "../../Actions/ActionBuilder.h"
#include "../../Actions/ActionManager.h"
#include "../../Scene/Component.h"
#include "../../Scene/Node.h"
#include "../../Graphics/Material.h"

using namespace Urho3D;
namespace nb = nanobind;
using namespace nb::literals;
#undef NB_EXPORT
#define NB_EXPORT
NB_MODULE(action, m)
{
    using namespace Actions;

    nb::class_<BaseAction>(m, "BaseAction");

    nb::class_<FiniteTimeAction, BaseAction>(m, "FiniteTimeAction");

    nb::class_<ActionState>(m, "ActionState")
        .def("GetTarget", &ActionState::GetTarget)
        .def("GetOriginalTarget", &ActionState::GetOriginalTarget)
        .def("GetAction", &ActionState::GetAction)
        .def("IsDone", &ActionState::IsDone);

    nb::class_<ActionBuilder>(m, "ActionBuilder")
        .def(nb::init<Context*, bool>())
        //sol::call_constructor, sol::factories([context]() { return ActionBuilder(context, true); }),
        .def("Then", [](ActionBuilder* self, FiniteTimeAction* nextAction) { return self->Then(SharedPtr<Actions::FiniteTimeAction>(nextAction)); })
        .def("Also", [](ActionBuilder* self, FiniteTimeAction* nextAction) { return self->Also(SharedPtr<Actions::FiniteTimeAction>(nextAction)); })
        .def("MoveBy", [](ActionBuilder* self, float duration, const Vector3& offset) { return self->MoveBy(duration, offset); })
        .def("MoveBy", [](ActionBuilder* self, float duration, const Vector3& offset, const char* attributeName) { return self->MoveBy(duration, offset, attributeName); })
        .def("MoveBy", [](ActionBuilder* self, float duration, const Vector2& offset) { return self->MoveBy(duration, offset); })
        .def("MoveBy", [](ActionBuilder* self, float duration, const Vector2& offset, const char* attributeName) { return self->MoveBy(duration, offset, attributeName); })
        .def("MoveByQuadratic", [](ActionBuilder* self, float duration, const Vector3& controlOffset, const Vector3& targetOffset) { return self->MoveByQuadratic(duration, controlOffset, targetOffset); })
        .def("MoveByQuadratic", [](ActionBuilder* self, float duration, const Vector3& controlOffset, const Vector3& targetOffset, const char* attributeName) { return self->MoveByQuadratic(duration, controlOffset, targetOffset, attributeName); })
        .def("MoveByQuadratic", [](ActionBuilder* self, float duration, const Vector2& controlOffset, const Vector2& targetOffset) { return self->MoveByQuadratic(duration, controlOffset, targetOffset); })
        .def("MoveByQuadratic", [](ActionBuilder* self, float duration, const Vector2& controlOffset, const Vector2& targetOffset, const char* attributeName) { return self->MoveByQuadratic(duration, controlOffset, targetOffset, attributeName); })
        .def("JumpBy", [](ActionBuilder* self, const Vector3& offset) { return self->JumpBy(offset); })
        .def("JumpBy", [](ActionBuilder* self, const Vector3& offset, const char* attributeName) { return self->JumpBy(offset, attributeName); })
        .def("JumpBy", [](ActionBuilder* self, const Vector2& offset) { return self->JumpBy(offset); })
        .def("JumpBy", [](ActionBuilder* self, const Vector2& offset, const char* attributeName) { return self->JumpBy(offset, attributeName); })
        .def("ScaleBy", [](ActionBuilder* self, float duration, const Vector3& delta) { return self->ScaleBy(duration, delta); })
        .def("ScaleBy", [](ActionBuilder* self, float duration, const Vector3& delta, const char* attributeName) { return self->ScaleBy(duration, delta, attributeName); })
        .def("ScaleBy", [](ActionBuilder* self, float duration, const Vector2& delta) { return self->ScaleBy(duration, delta); })
        .def("ScaleBy", [](ActionBuilder* self, float duration, const Vector2& delta, const char* attributeName) { return self->ScaleBy(duration, delta, attributeName); })
        .def("RotateBy", [](ActionBuilder* self, float duration, const Quaternion& delta) { return self->RotateBy(duration, delta); })
        .def("RotateBy", [](ActionBuilder* self, float duration, const Quaternion& delta, const char* attributeName) { return self->RotateBy(duration, delta, attributeName); })
        .def("RotateAround", &ActionBuilder::RotateAround)
        .def("Hide", [](ActionBuilder* self) { return self->Hide(); })
        .def("Hide", [](ActionBuilder* self, const char* attributeName) { return self->Hide(attributeName); })
        .def("Show", [](ActionBuilder* self) { return self->Show(); })
        .def("Show", [](ActionBuilder* self, const char* attributeName) { return self->Show(attributeName); })
        .def("Enable", [](ActionBuilder* self) { return self->Enable(); })
        .def("Enable", [](ActionBuilder* self, const char* attributeName) { return self->Enable(attributeName); })
        .def("Disable", [](ActionBuilder* self) { return self->Disable(); })
        .def("Disable", [](ActionBuilder* self, const char* attributeName) { return self->Disable(attributeName); })
        .def("Blink", [](ActionBuilder* self, float duration, unsigned numOfBlinks) { return self->Blink(duration, numOfBlinks); })
        .def("Blink", [](ActionBuilder* self, float duration, unsigned numOfBlinks, const char* attributeName) { return self->Blink(duration, numOfBlinks, attributeName); })
        .def("AttributeTo", [](ActionBuilder* self, float duration, const char* attributeName, const Variant& to) { return self->AttributeTo(duration, attributeName, to); })
        .def("AttributeFromTo", [](ActionBuilder* self, float duration, const char* attributeName, const Variant& from, const Variant& to) { return self->AttributeFromTo(duration, attributeName, from, to); })
        .def("ShaderParameterTo", [](ActionBuilder* self, float duration, const char* attributeName, const Variant& to) { return self->ShaderParameterTo(duration, attributeName, to); })
        .def("ShaderParameterFromTo", [](ActionBuilder* self, float duration, const char* attributeName, const Variant& from, const Variant& to) { return self->ShaderParameterFromTo(duration, attributeName, from, to); })
        .def("BackIn", &ActionBuilder::BackIn)
        .def("BackOut", &ActionBuilder::BackOut)
        .def("BackInOut", &ActionBuilder::BackInOut)
        .def("BounceOut", &ActionBuilder::BounceOut)
        .def("BounceIn", &ActionBuilder::BounceIn)
        .def("BounceInOut", &ActionBuilder::BounceInOut)
        .def("SineOut", &ActionBuilder::SineOut)
        .def("SineIn", &ActionBuilder::SineIn)
        .def("SineInOut", &ActionBuilder::SineInOut)
        .def("ExponentialOut", &ActionBuilder::ExponentialOut)
        .def("ExponentialIn", &ActionBuilder::ExponentialIn)
        .def("ExponentialInOut", &ActionBuilder::ExponentialInOut)
        .def("ElasticIn", [](ActionBuilder* self) { return self->ElasticIn(); })
        .def("ElasticIn", [](ActionBuilder* self, float period) { return self->ElasticIn(period); })
        .def("ElasticOut", [](ActionBuilder* self) { return self->ElasticOut(); })
        .def("ElasticOut", [](ActionBuilder* self, float period) { return self->ElasticOut(period); })
        .def("ElasticInOut", [](ActionBuilder* self) { return self->ElasticInOut(); })
        .def("ElasticInOut", [](ActionBuilder* self, float period) { return self->ElasticInOut(period); })
        .def("RemoveSelf", &ActionBuilder::RemoveSelf)
        .def("DelayTime", &ActionBuilder::DelayTime)
        .def("Repeat", &ActionBuilder::Repeat)
        .def("RepeatForever", &ActionBuilder::RepeatForever)
        .def("Build", [](ActionBuilder* self) { return (FiniteTimeAction*)self->Build().Detach(); })
        .def("Run", [](ActionBuilder* self, Object* target) { return self->Run(target); })
        .def("Run", [](ActionBuilder* self, ActionManager* actionManager, Object* target) { return self->Run(actionManager, target); });

    nb::class_<ActionManager, Object>(m, "ActionManager")
        .def("CompleteAllActions", &ActionManager::CompleteAllActions)
        .def("CancelAllActions", &ActionManager::CancelAllActions)
        .def("CompleteAllActionsOnTarget", [](ActionManager* self, Material* target) { self->CompleteAllActionsOnTarget(target); })
        .def("CompleteAllActionsOnTarget", [](ActionManager* self, Node* target) { self->CompleteAllActionsOnTarget(target); })
        .def("CompleteAllActionsOnTarget", [](ActionManager* self, Component* target) { self->CompleteAllActionsOnTarget(target); })
        .def("CancelAllActionsFromTarget", [](ActionManager* self, Material* target) { self->CancelAllActionsFromTarget(target); })
        .def("CancelAllActionsFromTarget", [](ActionManager* self, Node* target) { self->CancelAllActionsFromTarget(target); })
        .def("CancelAllActionsFromTarget", [](ActionManager* self, Component* target) { self->CancelAllActionsFromTarget(target); })
        .def("CancelAction", &ActionManager::CancelAction)
        .def("GetNumActions", [](ActionManager* self, Material* target) { return self->GetNumActions(target); })
        .def("GetNumActions", [](ActionManager* self, Node* target) { return self->GetNumActions(target); })
        .def("GetNumActions", [](ActionManager* self, Component* target) { return self->GetNumActions(target); })
        .def("AddAction", [](ActionManager* self, Actions::BaseAction* action, Material* target) { return self->AddAction(action, target); })
        .def("AddAction", [](ActionManager* self, Actions::BaseAction* action, Material* target, bool paused) { return self->AddAction(action, target, paused); })
        .def("AddAction", [](ActionManager* self, Actions::BaseAction* action, Node* target) { return self->AddAction(action, target); })
        .def("AddAction", [](ActionManager* self, Actions::BaseAction* action, Node* target, bool paused) { return self->AddAction(action, target, paused); })
        .def("AddAction", [](ActionManager* self, Actions::BaseAction* action, Component* target) { return self->AddAction(action, target); })
        .def("AddAction", [](ActionManager* self, Actions::BaseAction* action, Component* target, bool paused) { return self->AddAction(action, target, paused); })
        .def("GetEmptyAction", &ActionManager::GetEmptyAction);

    m.attr("action_manager") = Context::GetInstance()->GetSubsystem<ActionManager>();
}
