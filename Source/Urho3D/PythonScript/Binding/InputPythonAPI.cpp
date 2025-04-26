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
    nb::enum_<Key>(m, "Key")
        .value("KEY_F1", KEY_F1)
        .value("KEY_F2", KEY_F2)
        .value("KEY_F3", KEY_F3)
        .value("KEY_F4", KEY_F4)
        .value("KEY_F5", KEY_F5)
        .value("KEY_F6", KEY_F6)
        .value("KEY_F7", KEY_F7)
        .value("KEY_F8", KEY_F8)
        .value("KEY_F9", KEY_F9)
        .value("KEY_F10", KEY_F10)
        .value("KEY_F11", KEY_F11)
        .value("KEY_F12", KEY_F12)
        .value("KEY_PAGEDOWN", KEY_PAGEDOWN)
        .value("KEY_PAGEUP", KEY_PAGEUP)
        .value("KEY_LEFT", KEY_LEFT)
        .value("KEY_UP", KEY_UP)
        .value("KEY_DOWN", KEY_DOWN)
        .value("KEY_RIGHT", KEY_RIGHT)
        .value("KEY_SELECT", KEY_SELECT)
        .value("KEY_ESCAPE", KEY_ESCAPE)
        .value("KEY_SPACE", KEY_SPACE)
        .value("KEY_SHIFT", KEY_SHIFT)
        .value("KEY_TAB", KEY_TAB)
        .value("KEY_1", KEY_1)
        .value("KEY_2", KEY_2)
        .value("KEY_3", KEY_3)
        .value("KEY_4", KEY_4)
        .value("KEY_5", KEY_5)
        .value("KEY_6", KEY_6)
        .value("KEY_7", KEY_7)
        .value("KEY_8", KEY_8)
        .value("KEY_9", KEY_9)
        .value("KEY_W", KEY_W)
        .value("KEY_S", KEY_S)
        .value("KEY_A", KEY_A)
        .value("KEY_D", KEY_D)
        .value("KEY_F", KEY_F)
        .value("KEY_O", KEY_O);

    nb::enum_<Qualifier>(m, "Qualifier")
        .value("QUAL_SHIFT", QUAL_SHIFT)
        .value("QUAL_CTRL", QUAL_CTRL)
        .value("QUAL_ALT", QUAL_ALT)
        .value("QUAL_ANY", QUAL_ANY);

    nb::enum_<MouseButton>(m, "MouseButton")
        .value("MOUSEB_LEFT", MOUSEB_LEFT)
        .value("MOUSEB_MIDDLE", MOUSEB_MIDDLE)
        .value("MOUSEB_RIGHT", MOUSEB_RIGHT);
}
static void RegisterInputConst(nb::module_ m)
{
    auto eventType = m.def_submodule("EventType");
    eventType.attr("E_TOUCHBEGIN") = E_TOUCHBEGIN;
    eventType.attr("E_TOUCHEND") = E_TOUCHEND;
    eventType.attr("E_TOUCHMOVE") = E_TOUCHMOVE;
    eventType.attr("E_MOUSEBUTTONDOWN") = E_MOUSEBUTTONDOWN;
    eventType.attr("E_MOUSEBUTTONUP") = E_MOUSEBUTTONUP;
    eventType.attr("E_MOUSEMOVE") = E_MOUSEMOVE;
    eventType.attr("E_MOUSEWHEEL") = E_MOUSEWHEEL;
    eventType.attr("E_KEYDOWN") = E_KEYDOWN;
    eventType.attr("E_KEYUP") = E_KEYUP;

    auto paramType = m.def_submodule("ParamType");
    paramType.attr("P_TOUCHID") = TouchBegin::P_TOUCHID;
    paramType.attr("P_PRESSURE") = TouchBegin::P_PRESSURE;
    paramType.attr("P_MODE") = MouseModeChanged::P_MODE;
    paramType.attr("P_MOUSELOCKED") = MouseModeChanged::P_MOUSELOCKED;
    paramType.attr("P_BUTTON") = MouseButtonDown::P_BUTTON;
    paramType.attr("P_BUTTONS") = MouseButtonDown::P_BUTTONS;
    paramType.attr("P_QUALIFIERS") = MouseButtonDown::P_QUALIFIERS;
    paramType.attr("P_CLICKS") = MouseButtonDown::P_CLICKS;
    paramType.attr("P_WHEEL") = MouseWheel::P_WHEEL;
    paramType.attr("P_X") = MouseMove::P_X;
    paramType.attr("P_Y") = MouseMove::P_Y;
    paramType.attr("P_DX") = MouseMove::P_DX;
    paramType.attr("P_DY") = MouseMove::P_DY;
    paramType.attr("P_KEY") = KeyDown::P_KEY;
}
void init_cmodule_input(nb::module_& pm)
{
    auto m = pm.def_submodule("input");

    nb::enum_<MouseMode>(m, "MouseMode")
        .value("MM_ABSOLUTE", MM_ABSOLUTE)
        .value("MM_RELATIVE", MM_RELATIVE)
        .value("MM_WRAP", MM_WRAP)
        .value("MM_FREE", MM_FREE)
        .value("MM_INVALID", MM_INVALID);

    nb::class_<TouchState>(m, "TouchState")
        .def_rw("position", &TouchState::position_)
        .def_rw("delta", &TouchState::delta_)
        .def_prop_ro("touchedElement", [](TouchState* self) { return self->touchedElement_.Get(); });

    auto gui = Context::GetInstance()->GetSubsystem<GUI>();
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
    
    RegisterKeyboard(m);
    RegisterInputConst(m);
     
    m.attr("input_system") = Context::GetInstance()->GetSubsystem<Input>();
}
