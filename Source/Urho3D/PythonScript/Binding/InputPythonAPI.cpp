#include <nanobind/nanobind.h>
#include "../../Core/Context.h"
#include "../../Input/Input.h"
#include "../../Input/InputConstants.h"
#include "../../GUI/Gui.h"

using namespace Urho3D;
namespace nb = nanobind;
using namespace nb::literals;
//Urho3D::Context* GetContext(lua_State* L);

// static void RegisterKeyboard(sol::table& lua)
// {
//     lua["KEY_F1"] = KEY_F1;
//     lua["KEY_F2"] = KEY_F2;
//     lua["KEY_F3"] = KEY_F3;
//     lua["KEY_F4"] = KEY_F4;
//     lua["KEY_F5"] = KEY_F5;
//     lua["KEY_F6"] = KEY_F6;
//     lua["KEY_F7"] = KEY_F7;
//     lua["KEY_F8"] = KEY_F8;
//     lua["KEY_F9"] = KEY_F9;
//     lua["KEY_F10"] = KEY_F10;
//     lua["KEY_F11"] = KEY_F11;
//     lua["KEY_F12"] = KEY_F12;
//     lua["KEY_PAGEDOWN"] = KEY_PAGEDOWN;
//     lua["KEY_PAGEUP"] = KEY_PAGEUP;
//     lua["KEY_LEFT"] = KEY_LEFT;
//     lua["KEY_UP"] = KEY_UP;
//     lua["KEY_DOWN"] = KEY_DOWN;
//     lua["KEY_RIGHT"] = KEY_RIGHT;
//     lua["KEY_SELECT"] = KEY_SELECT;
//     lua["KEY_ESCAPE"] = KEY_ESCAPE;
//     lua["KEY_SPACE"] = KEY_SPACE;
//     lua["KEY_SHIFT"] = KEY_SHIFT;
//     lua["KEY_TAB"] = KEY_TAB;
//     lua["KEY_1"] = KEY_1;
//     lua["KEY_2"] = KEY_2;
//     lua["KEY_3"] = KEY_3;
//     lua["KEY_4"] = KEY_4;
//     lua["KEY_5"] = KEY_5;
//     lua["KEY_6"] = KEY_6;
//     lua["KEY_7"] = KEY_7;
//     lua["KEY_8"] = KEY_8;
//     lua["KEY_9"] = KEY_9;
//     lua["KEY_W"] = KEY_W;
//     lua["KEY_S"] = KEY_S;
//     lua["KEY_A"] = KEY_A;
//     lua["KEY_D"] = KEY_D;
//     lua["KEY_F"] = KEY_F;
//     lua["KEY_O"] = KEY_O;
//     lua["QUAL_SHIFT"]   = QUAL_SHIFT;
//     lua["QUAL_CTRL"]    = QUAL_CTRL;
//     lua["QUAL_ALT"]     = QUAL_ALT;
//     lua["QUAL_ANY"]     = QUAL_ANY;
//     //
//     lua["MOUSEB_LEFT"]      = MOUSEB_LEFT;
//     lua["MOUSEB_MIDDLE"]    = MOUSEB_MIDDLE;
//     lua["MOUSEB_RIGHT"]     = MOUSEB_RIGHT;
// }
// static void RegisterInputConst(sol::state& lua)
// {
//     auto eventType = lua["EventType"].get_or_create<sol::table>();
//     eventType["E_TOUCHBEGIN"] = E_TOUCHBEGIN;
//     eventType["E_TOUCHEND"] = E_TOUCHEND;
//     eventType["E_TOUCHMOVE"] = E_TOUCHMOVE;
//     eventType["E_MOUSEBUTTONDOWN"] = E_MOUSEBUTTONDOWN;
//     eventType["E_MOUSEBUTTONUP"] = E_MOUSEBUTTONUP;
//     eventType["E_MOUSEMOVE"] = E_MOUSEMOVE;
//     eventType["E_MOUSEWHEEL"] = E_MOUSEWHEEL;
//     eventType["E_KEYDOWN"] = E_KEYDOWN;
//     eventType["E_KEYUP"] = E_KEYUP;
// 
//     auto paramType = lua["ParamType"].get_or_create<sol::table>();
//     paramType["P_TOUCHID"] = TouchBegin::P_TOUCHID;
//     paramType["P_PRESSURE"] = TouchBegin::P_PRESSURE;
//     paramType["P_MODE"] = MouseModeChanged::P_MODE;
//     paramType["P_MOUSELOCKED"] = MouseModeChanged::P_MOUSELOCKED;
//     paramType["P_BUTTON"] = MouseButtonDown::P_BUTTON;
//     paramType["P_BUTTONS"] = MouseButtonDown::P_BUTTONS;
//     paramType["P_QUALIFIERS"] = MouseButtonDown::P_QUALIFIERS;
//     paramType["P_CLICKS"] = MouseButtonDown::P_CLICKS;
//     paramType["P_WHEEL"] = MouseWheel::P_WHEEL;
//     paramType["P_X"] = MouseMove::P_X;
//     paramType["P_Y"] = MouseMove::P_Y;
//     paramType["P_DX"] = MouseMove::P_DX;
//     paramType["P_DY"] = MouseMove::P_DY;
//     paramType["P_KEY"] = KeyDown::P_KEY;
// }
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
    
//     RegisterKeyboard(input);
//     RegisterInputConst(lua);
//     
//     lua["input_system"] = context->GetSubsystem<Input>();
}
