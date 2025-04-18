#include "Urho3D/Precompiled.h"
#include <nanobind/nanobind.h>
#include "Urho3D/IO/Log.h"
#include "Urho3D/PythonScript/PythonScriptEventInvoker.h"
#include "Urho3D/PythonScript/PythonScriptInstance.h"
#include "Urho3D/PythonScript/PythonScript.h"
#include "Urho3D/DebugNew.h"

namespace nb = nanobind;

namespace Urho3D
{
PythonScriptEventInvoker::PythonScriptEventInvoker(Context* context) :
    Object(context)
{
}

PythonScriptEventInvoker::PythonScriptEventInvoker(PythonScriptInstance* instance) :
    Object(instance->GetContext()),
    instance_(instance)
{
}

PythonScriptEventInvoker::~PythonScriptEventInvoker() = default;

void PythonScriptEventInvoker::AddEventHandler(Object* sender, const StringHash& eventType, nb::callable function)
{
    if (!function)
        return;

    if (sender)
        SubscribeToEvent(sender, eventType, [function](StringHash eventType, VariantMap& eventData) { CallPythonFuntion(function, eventType, eventData); });
    else
        SubscribeToEvent(eventType, [function](StringHash eventType, VariantMap& eventData) { CallPythonFuntion(function, eventType, eventData); });
}

void PythonScriptEventInvoker::HandlePythonScriptEvent(StringHash eventType, VariantMap& eventData)
{

}

}
