#include "GetPush.h"
#include "../../LuaScript/ToluaUtils.h"
#include "../../LuaScript/LuaScript.h"
#include "../../LuaScript/LuaScriptInstance.h"
#include "../../Scene/Node.h"

using namespace Urho3D;
#pragma warning(disable : 4800)

static void RegisterEventName(const ea::string eventName) { GetEventNameRegister().RegisterString(eventName.c_str()); }

static LuaScript* GetLuaScript(lua_State* L) { return GetContext(L)->GetSubsystem<LuaScript>(); }

#define LuaScriptAddEventHandler GetLuaScript(lua)->AddEventHandler
#define LuaScriptRemoveEventHandler GetLuaScript(lua)->RemoveEventHandler
#define LuaScriptRemoveEventHandlers GetLuaScript(lua)->RemoveEventHandlers
#define LuaScriptRemoveAllEventHandlers GetLuaScript(lua)->RemoveAllEventHandlers
#define LuaScriptRemoveEventHandlersExcept GetLuaScript(lua)->RemoveEventHandlersExcept
#define LuaScriptHasSubscribedToEvent GetLuaScript(lua)->HasEventHandler

#define LuaScriptSendEvent GetLuaScript(lua)->SendEvent
#define LuaScriptSetExecuteConsoleCommands GetLuaScript(lua)->SetExecuteConsoleCommands
#define LuaScriptGetExecuteConsoleCommands GetLuaScript(lua)->GetExecuteConsoleCommands

#define LuaScriptSetGlobalVar GetLuaScript(lua)->SetGlobalVar
#define LuaScriptGetGlobalVar GetLuaScript(lua)->GetGlobalVar
#define LuaScriptGetGlobalVars GetLuaScript(lua)->GetGlobalVars

int sol2_LuaScriptLuaAPI_open(sol::state& lua)
{
    lua.new_usertype<LuaScriptInstance>("LuaScriptInstance",
        "SubscribeToEvent", sol::overload(
            sol::resolve<void(const ea::string&, const ea::string&)>(&LuaScriptInstance::AddEventHandler),
            sol::resolve<void(Object*, const ea::string&, const ea::string&)>(&LuaScriptInstance::AddEventHandler)),
        "GetNode", &LuaScriptInstance::GetNode,
        "object", sol::property(&LuaScriptInstance::GetScriptObject, &LuaScriptInstance::SetScriptObject),
        sol::base_classes, sol::bases<Component>());

    lua["SubscribeToEvent"] = sol::overload(
        [&lua](const ea::string& eventName, const ea::string& functionName) {
            LuaScriptAddEventHandler(eventName, functionName);
        },
        [&lua](const ea::string& eventName, sol::function function) {
            LuaScriptAddEventHandler(eventName, &function);
        },
        [&lua](Object* sender, const ea::string& eventName, const ea::string& functionName) {
            LuaScriptAddEventHandler(sender, eventName, functionName);
        },
        [&lua](Object* sender, const ea::string& eventName, sol::function function) {
            LuaScriptAddEventHandler(sender, eventName, &function);
        });
    lua["UnSubscribeToEvent"] = sol::overload(
        [&lua](const ea::string& eventName) { LuaScriptRemoveEventHandler(eventName); },
        [&lua](Object* sender, const ea::string& eventName) { LuaScriptRemoveEventHandler(sender, eventName); });
    lua["UnSubscribeToAllEvent"] =
        sol::overload([&lua]() { LuaScriptRemoveAllEventHandlers(); });
    //     sol_S["SubscribeToEvent"] = [&sol_S](sol::variadic_args va) {
//         if (va.size() == 2)
//         {
//             // SubscribeToEvent(const String eventName, void* functionOrFunctionName);
//             const String eventName = va.get<std::string>(0).c_str();// ((const String)tolua_tourho3dstring(sol_S, 1, 0));
//             if (sol::stack::check<sol::function>(sol_S, 2)/*lua_isfunction(tolua_S, 2)*/)
//                 LuaScriptAddEventHandler(eventName, 2);
//             else
//             {
//                 const String functionName = va.get<std::string>(1).c_str();// (const String)tolua_tourho3dstring(sol_S, 2, 0);
//                 LuaScriptAddEventHandler(eventName, functionName);
//             }
//         }
//         else if (va.size() == 3)
//         {
//             // SubscribeToEvent(Object* sender, const String eventName, void* functionOrFunctionName);
//             Object* sender = (Object*)lua_touserdata(sol_S, 1);// (Object*)tolua_touserdata(tolua_S, 1, 0));
//             const String eventName = va.get<std::string>(1).c_str();// ((const String)tolua_tourho3dstring(sol_S, 2, 0));
//             if (sol::stack::check<sol::function>(sol_S, 3)/*lua_isfunction(sol_S, 3)*/)
//                 LuaScriptAddEventHandler(sender, eventName, 3);
//             else
//             {
//                 const String functionName = va.get<std::string>(2).c_str();// (const String)tolua_tourho3dstring(sol_S, 3, 0);
//                 LuaScriptAddEventHandler(sender, eventName, functionName);
//             }
//         }
// 	};
    lua.script(R"(
LuaScriptObject = {}

function LuaScriptObject:Start()
end

function LuaScriptObject:Stop()
end

function LuaScriptObject:GetNode()
    return self.node
end

function LuaScriptObject:SubscribeToEvent(param1, param2, param3)
    local instance = self.instance
    if instance == nil then
        return
    end

    if param3 == nil then
        instance:SubscribeToEvent(param1, param2)
    else
        instance:SubscribeToEvent(param1, param2, param3)
    end
end

function LuaScriptObject:UnsubscribeFromEvent(param1, param2)
    local instance = self.instance
    if instance == nil then
        return
    end

    if param2 == nil then
        instance:UnsubscribeFromEvent(param1)
    else
        instance:UnsubscribeFromEvent(param1, param2)
    end
end

function LuaScriptObject:UnsubscribeFromEvents(sender)
    local instance = self.instance
    if instance == nil then
        return
    end
    instance:UnsubscribeFromEvents(sender)
end

function LuaScriptObject:UnsubscribeFromAllEvents()
    local instance = self.instance
    if instance == nil then
        return
    end
    instance:UnsubscribeFromAllEvents()
end

function LuaScriptObject:UnsubscribeFromAllEventsExcept()
    local instance = self.instance
    if instance == nil then
        return
    end
    instance:UnsubscribeFromAllEventsExcept()
end

function LuaScriptObject:HasSubscribedToEvent(param1, param2)
    local instance = self.instance
    if instance == nil then
        return
    end

    if param2 == nil then
        return instance:HasSubscribedToEvent(param1)
    else
        return instance:HasSubscribedToEvent(param1, param2)
    end
end

function ScriptObject()
    local o = {}
    setmetatable(o, LuaScriptObject)
    LuaScriptObject.__index = LuaScriptObject
    return o
end

function CreateScriptObjectInstance(object, instance)
    local o = {}
    setmetatable(o, object)
    object.__index = object

    instance.object = o
    o.instance = instance
    o.node = instance:GetNode()

    local keys = {}
    for k, v in pairs(o) do
        keys[k] = true
    end

    -- Call start function
    o:Start()

    local attrNames = {}
    for k, v in pairs(o) do
        if keys[k] == nil then
            table.insert(attrNames, k)
        end
    end

    return o, attrNames
end

function DestroyScriptObjectInstance(instance)
    local object = instance.object
    if object == nil then
        return
    end

    -- Call stop function
    object:Stop()

    object.node = nil
    object.instance = nil
    instance.object = nil
end
        )"
    );
    return 0;
}
