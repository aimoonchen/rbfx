#pragma once
//#include <EASTL/string.h>
#include <sol/sol.hpp>
// #include "../../Math/StringHash.h"
// #include "../../IO/Log.h"
/*
template <typename Handler>
bool sol_lua_check(
    sol::types<const ea::string&>, lua_State* L, int index, Handler&& handler, sol::stack::record& tracking)
{
    int absolute_index = lua_absindex(L, index);
    bool success = sol::stack::check<const char*>(L, absolute_index, handler);
    tracking.use(1);

    return success;
}
template <typename Handler>
bool sol_lua_check(sol::types<ea::string>, lua_State* L, int index, Handler&& handler, sol::stack::record& tracking)
{
    int absolute_index = lua_absindex(L, index);
    bool success = sol::stack::check<const char*>(L, absolute_index, handler);
    tracking.use(1);

    return success;
}
ea::string sol_lua_get(sol::types<const ea::string&>, lua_State* L, int index, sol::stack::record& tracking);
ea::string sol_lua_get(sol::types<ea::string>, lua_State* L, int index, sol::stack::record& tracking);
int sol_lua_push(lua_State* L, const ea::string& str);
int sol_lua_push(lua_State* L, const ea::string* str);
*/
namespace sol
{
template <typename Handler>
inline bool sol_lua_check(
    sol::types<ea::string_view>, lua_State* L, int index, Handler&& handler, sol::stack::record& tracking)
{
    int absoluteIndex = lua_absindex(L, index);
    bool success = sol::stack::check<std::string_view>(L, absoluteIndex, handler);
    tracking.use(1);
    return success;
}

ea::string_view sol_lua_get(sol::types<ea::string_view>, lua_State* L, int index, sol::stack::record& tracking);
int sol_lua_push(sol::types<ea::string_view>, lua_State* L, const ea::string_view& obj);

template <typename Handler>
inline bool sol_lua_check(
    sol::types<ea::string>, lua_State* L, int index, Handler&& handler, sol::stack::record& tracking)
{
    int absoluteIndex = lua_absindex(L, index);
    bool success = sol::stack::check<std::string>(L, absoluteIndex, handler);
    tracking.use(1);
    return success;
}

ea::string sol_lua_get(sol::types<ea::string>, lua_State* L, int index, sol::stack::record& tracking);
int sol_lua_push(sol::types<ea::string>, lua_State* L, const ea::string& obj);

} // namespace sol

#define CALL_LUA(func,...)                                                          \
sol::protected_function_result result = func(__VA_ARGS__);                          \
if (!result.valid()) {                                                              \
    sol::error err = result;                                                        \
    sol::call_status status = result.status();                                      \
    URHO3D_LOGERRORF("%s error\n\t%s", sol::to_string(status).c_str(), err.what()); \
}
