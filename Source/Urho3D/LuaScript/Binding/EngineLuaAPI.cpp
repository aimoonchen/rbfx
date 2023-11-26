#include <sol/sol.hpp>
#include "../../Core/Context.h"
#include "../../Engine/Engine.h"
//#include "../../SystemUI/Console.h"
//#include "../../SystemUI/DebugHud.h"
#include "../../Resource/XMLFile.h"
#include "../../UI/BorderImage.h"

Urho3D::Context* GetContext(lua_State* L);

using namespace Urho3D;


static void RegisterEngineConst(sol::state& lua)
{
    auto eventType = lua["EventType"].get_or_create<sol::table>();
    auto paramType = lua["ParamType"].get_or_create<sol::table>();
}

int sol2_EngineLuaAPI_open(sol::state& lua)
{
    auto context = GetContext(lua.lua_state());
//     lua.new_usertype<Console>("Console",// sol::constructors<Console(Context*)>(),
//         "IsVisible", &Console::IsVisible,
//         "SetVisible", &Console::SetVisible,
//         "Toggle", &Console::Toggle,
//         "visible", sol::property(&Console::IsVisible, &Console::SetVisible)
// //         "defaultStyle", sol::property(&Console::GetDefaultStyle, &Console::SetDefaultStyle),
// //         "background", sol::property(&Console::GetBackground)
//         );
//     lua.new_usertype<DebugHud>("Console",// sol::constructors<DebugHud(Context*)>(),
//         "ToggleAll", &DebugHud::ToggleAll,
//         "defaultStyle", sol::property(&DebugHud::GetDefaultStyle, &DebugHud::SetDefaultStyle)
//         );
    lua.new_usertype<Engine>("Engine",// sol::constructors<Engine(Context*)>(),
//         "CreateConsole", [&lua, context](Engine* obj) {
//             obj->CreateConsole();
//             lua["console"] = context->GetSubsystem<Console>();
//         },
//         "CreateDebugHud", [&lua, context](Engine* obj) {
//             obj->CreateDebugHud();
//             lua["debugHud"] = context->GetSubsystem<DebugHud>();
//         },
        "Exit", &Engine::Exit);
    lua["engine"] = context->GetSubsystem<Engine>();
    RegisterEngineConst(lua);
    
    return 0;
}
