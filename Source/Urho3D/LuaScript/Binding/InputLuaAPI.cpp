#include <sol/sol.hpp>
#include "../../Core/Context.h"
#include "../../Input/Input.h"
#include "../../Input/InputConstants.h"
#include "../../GUI/Gui.h"

using namespace Urho3D;
Urho3D::Context* GetContext(lua_State* L);

static void RegisterKeyboard(sol::table& lua)
{
    lua["KEY_F1"] = KEY_F1;
    lua["KEY_F2"] = KEY_F2;
    lua["KEY_F3"] = KEY_F3;
    lua["KEY_F4"] = KEY_F4;
    lua["KEY_F5"] = KEY_F5;
    lua["KEY_F6"] = KEY_F6;
    lua["KEY_F7"] = KEY_F7;
    lua["KEY_F8"] = KEY_F8;
    lua["KEY_F9"] = KEY_F9;
    lua["KEY_F10"] = KEY_F10;
    lua["KEY_F11"] = KEY_F11;
    lua["KEY_F12"] = KEY_F12;
    lua["KEY_SELECT"] = KEY_SELECT;
    lua["KEY_ESCAPE"] = KEY_ESCAPE;
    lua["KEY_SPACE"] = KEY_SPACE;
    lua["KEY_SHIFT"] = KEY_SHIFT;
    lua["KEY_TAB"] = KEY_TAB;
    lua["KEY_1"] = KEY_1;
    lua["KEY_2"] = KEY_2;
    lua["KEY_3"] = KEY_3;
    lua["KEY_4"] = KEY_4;
    lua["KEY_5"] = KEY_5;
    lua["KEY_6"] = KEY_6;
    lua["KEY_7"] = KEY_7;
    lua["KEY_8"] = KEY_8;
    lua["KEY_9"] = KEY_9;
    lua["KEY_W"] = KEY_W;
    lua["KEY_S"] = KEY_S;
    lua["KEY_A"] = KEY_A;
    lua["KEY_D"] = KEY_D;
    lua["KEY_F"] = KEY_F;
    lua["KEY_O"] = KEY_O;
    lua["QUAL_SHIFT"]   = QUAL_SHIFT;
    lua["QUAL_CTRL"]    = QUAL_CTRL;
    lua["QUAL_ALT"]     = QUAL_ALT;
    lua["QUAL_ANY"]     = QUAL_ANY;
    //
    lua["MOUSEB_LEFT"]      = MOUSEB_LEFT;
    lua["MOUSEB_MIDDLE"]    = MOUSEB_MIDDLE;
    lua["MOUSEB_RIGHT"]     = MOUSEB_RIGHT;
}
static void RegisterInputConst(sol::state& lua)
{
    auto eventType = lua["EventType"].get_or_create<sol::table>();
    eventType["E_TOUCHBEGIN"] = E_TOUCHBEGIN;
    eventType["E_TOUCHEND"] = E_TOUCHEND;
    eventType["E_TOUCHMOVE"] = E_TOUCHMOVE;
    eventType["E_MOUSEBUTTONDOWN"] = E_MOUSEBUTTONDOWN;
    eventType["E_MOUSEBUTTONUP"] = E_MOUSEBUTTONUP;
    eventType["E_MOUSEMOVE"] = E_MOUSEMOVE;
    eventType["E_MOUSEWHEEL"] = E_MOUSEWHEEL;
    eventType["E_KEYDOWN"] = E_KEYDOWN;
    eventType["E_KEYUP"] = E_KEYUP;

    auto paramType = lua["ParamType"].get_or_create<sol::table>();
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
int sol2_InputLuaAPI_open(sol::state& lua)
{
    auto input = lua["input"].get_or_create<sol::table>();
//     input.new_usertype<Controls>("Controls",
// 		sol::call_constructor, sol::factories([]() { return Controls(); }),
// 		"buttons",&Controls::buttons_,
// 		"yaw", &Controls::yaw_,
// 		"pitch", &Controls::pitch_,
//         "extraData", &Controls::extraData_,
//         "Set", &Controls::Set,
//         "IsDown", &Controls::IsDown,
//         "IsPressed", &Controls::IsPressed);

    input.new_usertype<TouchState>("TouchState",
        "position", &TouchState::position_,
        "delta", &TouchState::delta_,
        "touchedElement", sol::property([](TouchState* obj) { return obj->touchedElement_.Get(); })
    );

    auto context = GetContext(lua.lua_state());
    auto gui = context->GetSubsystem<GUI>();
    input.new_usertype<Input>(
        "Input",
        "GetNumJoysticks", &Input::GetNumJoysticks,
        "AddScreenJoystick", &Input::AddScreenJoystick,
        "SetScreenJoystickVisible", &Input::SetScreenJoystickVisible,
        "GetNumJoysticks", &Input::GetNumJoysticks,
        "SetMouseMode", &Input::SetMouseMode,
        "GetNumTouches", &Input::GetNumTouches,
        "GetTouch", &Input::GetTouch,
        "GetKeyDown", &Input::GetKeyDown,
        "GetKeyPress", &Input::GetKeyPress,
        "GetMouseMove", &Input::GetMouseMove,
        "GetMouseMoveWheel", &Input::GetMouseMoveWheel,
        "GetMouseButtonDown", [](Input* self, MouseButton button) { return self->GetMouseButtonDown(button); },
        "GetMouseButtonPress", [](Input* self, MouseButton button) { return self->GetMouseButtonPress(button); },
        "touchEmulation", sol::property(&Input::GetTouchEmulation, &Input::SetTouchEmulation),
        "mouseVisible", sol::property(&Input::IsMouseVisible, [](Input* self, bool enable) { self->SetMouseVisible(enable); }),
        "mouseMode", sol::property(&Input::GetMouseMode, [](Input* self, MouseMode mode) { self->SetMouseMode(mode); }),
        "mouseMove", sol::property(&Input::GetMouseMove),
        "GetMousePosition", &Input::GetMousePosition,
        "GetQualifierDown", [](Input* self, int qualifier) { return self->GetQualifierDown((Qualifier)qualifier); },
        "GetQualifierPress", [](Input* self, int qualifier) { return self->GetQualifierPress((Qualifier)qualifier); },
        // TODO: rework joystick
        "CreateJoystick", [gui](const IntVector2& area, float scale) { gui->CreateJoystick(area, scale); },
        "EnableJoystick", [gui](bool enabled) { return gui->EnableJoystick(enabled); },
        "GetJoystickDegree", [gui]() { return gui->GetJoystickDegree(); },
        "GetJoystickTouchID", [gui]() { return gui->GetJoystickTouchID(); },
        "IsJoystickCapture", [gui]() { return gui->GetJoystickTouchID() != -1; },
        sol::base_classes, sol::bases<Object>());
    //
    input["MM_ABSOLUTE"] = MM_ABSOLUTE;
    input["MM_RELATIVE"] = MM_RELATIVE;
    input["MM_WRAP"] = MM_WRAP;
    input["MM_FREE"] = MM_FREE;
    input["MM_INVALID"] = MM_INVALID;
    
    RegisterKeyboard(input);
    RegisterInputConst(lua);
    
    lua["input_system"] = context->GetSubsystem<Input>();
    return 0;
}
