#include <nanobind/nanobind.h>
#include "../../Core/Context.h"
#include "../../Input/Input.h"
#include "../../Input/InputConstants.h"
#include "../../GUI/Gui.h"

using namespace Urho3D;
namespace nb = nanobind;
using namespace nb::literals;

static void RegisterKeyboard(nb::module_ m)
{
    m["KEY_F1"] = KEY_F1;
    m["KEY_F2"] = KEY_F2;
    m["KEY_F3"] = KEY_F3;
    m["KEY_F4"] = KEY_F4;
    m["KEY_F5"] = KEY_F5;
    m["KEY_F6"] = KEY_F6;
    m["KEY_F7"] = KEY_F7;
    m["KEY_F8"] = KEY_F8;
    m["KEY_F9"] = KEY_F9;
    m["KEY_F10"] = KEY_F10;
    m["KEY_F11"] = KEY_F11;
    m["KEY_F12"] = KEY_F12;
    m["KEY_PAGEDOWN"] = KEY_PAGEDOWN;
    m["KEY_PAGEUP"] = KEY_PAGEUP;
    m["KEY_LEFT"] = KEY_LEFT;
    m["KEY_UP"] = KEY_UP;
    m["KEY_DOWN"] = KEY_DOWN;
    m["KEY_RIGHT"] = KEY_RIGHT;
    m["KEY_SELECT"] = KEY_SELECT;
    m["KEY_ESCAPE"] = KEY_ESCAPE;
    m["KEY_SPACE"] = KEY_SPACE;
    m["KEY_SHIFT"] = KEY_SHIFT;
    m["KEY_TAB"] = KEY_TAB;
    m["KEY_1"] = KEY_1;
    m["KEY_2"] = KEY_2;
    m["KEY_3"] = KEY_3;
    m["KEY_4"] = KEY_4;
    m["KEY_5"] = KEY_5;
    m["KEY_6"] = KEY_6;
    m["KEY_7"] = KEY_7;
    m["KEY_8"] = KEY_8;
    m["KEY_9"] = KEY_9;
    m["KEY_W"] = KEY_W;
    m["KEY_S"] = KEY_S;
    m["KEY_A"] = KEY_A;
    m["KEY_D"] = KEY_D;
    m["KEY_F"] = KEY_F;
    m["KEY_O"] = KEY_O;
    m["QUAL_SHIFT"] = QUAL_SHIFT;
    m["QUAL_CTRL"] = QUAL_CTRL;
    m["QUAL_ALT"] = QUAL_ALT;
    m["QUAL_ANY"] = QUAL_ANY;
    //
    m["MOUSEB_LEFT"]      = MOUSEB_LEFT;
    m["MOUSEB_MIDDLE"]    = MOUSEB_MIDDLE;
    m["MOUSEB_RIGHT"]     = MOUSEB_RIGHT;
}
static void RegisterInputConst(nb::module_ m)
{
    auto eventType = m.def_submodule("EventType");
    eventType["E_TOUCHBEGIN"] = E_TOUCHBEGIN;
    eventType["E_TOUCHEND"] = E_TOUCHEND;
    eventType["E_TOUCHMOVE"] = E_TOUCHMOVE;
    eventType["E_MOUSEBUTTONDOWN"] = E_MOUSEBUTTONDOWN;
    eventType["E_MOUSEBUTTONUP"] = E_MOUSEBUTTONUP;
    eventType["E_MOUSEMOVE"] = E_MOUSEMOVE;
    eventType["E_MOUSEWHEEL"] = E_MOUSEWHEEL;
    eventType["E_KEYDOWN"] = E_KEYDOWN;
    eventType["E_KEYUP"] = E_KEYUP;

    auto paramType = m.def_submodule("ParamType");
    paramType["P_TOUCHID"] = TouchBegin::P_TOUCHID;
    paramType["P_PRESSURE"] = TouchBegin::P_PRESSURE;
    paramType["P_MODE"] = MouseModeChanged::P_MODE;
    paramType["P_MOUSELOCKED"] = MouseModeChanged::P_MOUSELOCKED;
    paramType["P_BUTTON"] = MouseButtonDown::P_BUTTON;
    paramType["P_BUTTONS"] = MouseButtonDown::P_BUTTONS;
    paramType["P_QUALIFIERS"] = MouseButtonDown::P_QUALIFIERS;
    paramType["P_CLICKS"] = MouseButtonDown::P_CLICKS;
    paramType["P_WHEEL"] = MouseWheel::P_WHEEL;
    paramType["P_X"] = MouseMove::P_X;
    paramType["P_Y"] = MouseMove::P_Y;
    paramType["P_DX"] = MouseMove::P_DX;
    paramType["P_DY"] = MouseMove::P_DY;
    paramType["P_KEY"] = KeyDown::P_KEY;
}

NB_MODULE(input, m)
{
    //auto input = lua["input"].get_or_create<sol::table>();

    nb::class_<TouchState>(m, "TouchState")
        .def_rw("position", &TouchState::position_)
        .def_rw("delta", &TouchState::delta_)
        .def_prop_ro("touchedElement", [](TouchState* self) { return self->touchedElement_.Get(); });

    //auto context = GetContext(lua.lua_state());
    Context* context = nullptr;
    auto gui = context->GetSubsystem<GUI>();
    nb::class_<Input, Object>(m, "Input")
        .def("GetNumJoysticks", &Input::GetNumJoysticks)
        .def("AddScreenJoystick", &Input::AddScreenJoystick)
        .def("SetScreenJoystickVisible", &Input::SetScreenJoystickVisible)
        .def("GetNumJoysticks", &Input::GetNumJoysticks)
        .def("SetMouseMode", &Input::SetMouseMode)
        .def("GetNumTouches", &Input::GetNumTouches)
        .def("GetTouch", &Input::GetTouch)
        .def("GetKeyDown", &Input::GetKeyDown)
        .def("GetKeyPress", &Input::GetKeyPress)
        .def("GetMouseMove", &Input::GetMouseMove)
        .def("GetMouseMoveWheel", &Input::GetMouseMoveWheel)
        .def("GetMouseButtonDown", [](Input* self, MouseButton button) { return self->GetMouseButtonDown(button); })
        .def("GetMouseButtonPress", [](Input* self, MouseButton button) { return self->GetMouseButtonPress(button); })
        .def_prop_rw("touchEmulation", &Input::GetTouchEmulation, &Input::SetTouchEmulation)
        .def_prop_rw("mouseVisible", &Input::IsMouseVisible, [](Input* self, bool enable) { self->SetMouseVisible(enable); })
        .def_prop_rw("mouseMode", &Input::GetMouseMode, [](Input* self, MouseMode mode) { self->SetMouseMode(mode); })
        .def_prop_ro("mouseMove", &Input::GetMouseMove)
        .def("GetMousePosition", &Input::GetMousePosition)
        .def("GetQualifierDown", [](Input* self, int qualifier) { return self->GetQualifierDown((Qualifier)qualifier); })
        .def("GetQualifierPress", [](Input* self, int qualifier) { return self->GetQualifierPress((Qualifier)qualifier); })
        // TODO: rework joystick
        .def("CreateJoystick", [gui](const IntVector2& area, float scale) { gui->CreateJoystick(area, scale); })
        .def("EnableJoystick", [gui](bool enabled) { return gui->EnableJoystick(enabled); })
        .def("GetJoystickDegree", [gui]() { return gui->GetJoystickDegree(); })
        .def("GetJoystickTouchID", [gui]() { return gui->GetJoystickTouchID(); })
        .def("IsJoystickCapture", [gui]() { return gui->GetJoystickTouchID() != -1; });
    //
    m.attr("MM_ABSOLUTE")    = MM_ABSOLUTE;
    m.attr("MM_RELATIVE")    = MM_RELATIVE;
    m.attr("MM_WRAP")        = MM_WRAP;
    m.attr("MM_FREE")        = MM_FREE;
    m.attr("MM_INVALID")     = MM_INVALID;
    
    RegisterKeyboard(m);
    RegisterInputConst(m);
     
    m.attr("input_system") = context->GetSubsystem<Input>();
}
