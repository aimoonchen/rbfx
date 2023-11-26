#pragma once
#include <EASTL/string.h>
#include <sol/sol.hpp>
#include "../../Math/StringHash.h"
#include "../../IO/Log.h"
template <typename Handler>
bool sol_lua_check(sol::types<const ea::string&>, lua_State* L, int index, Handler&& handler, sol::stack::record& tracking)
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

// template <typename Handler>
// bool sol_lua_check(sol::types<const Urho3D::StringHash&>, lua_State* L, int index, Handler&& handler, sol::stack::record& tracking)
// {
//     int absolute_index = lua_absindex(L, index);
//     bool success = sol::stack::check<const char*>(L, absolute_index, handler);
//     tracking.use(1);
// 
//     return success;
// }
// template <typename Handler>
// bool sol_lua_check(sol::types<Urho3D::StringHash>, lua_State* L, int index, Handler&& handler, sol::stack::record& tracking)
// {
//     int absolute_index = lua_absindex(L, index);
//     bool success = sol::stack::check<const char*>(L, absolute_index, handler);
//     tracking.use(1);
// 
//     return success;
// }
// Urho3D::StringHash sol_lua_get(sol::types<const Urho3D::StringHash&>, lua_State* L, int index, sol::stack::record& tracking);
// Urho3D::StringHash sol_lua_get(sol::types<Urho3D::StringHash>, lua_State* L, int index, sol::stack::record& tracking);
//int sol_lua_push(lua_State* L, StringHash str);

// namespace sol
// {
// // template <> struct lua_size<ea::string> : std::integral_constant<int, 1>
// // {
// // };
// 
// namespace stack
// {
// 
// template <> struct checker<ea::string>
// {
//     template <typename Handler> static bool check(lua_State* L, int index, Handler&& handler, record& tracking)
//     {
//         int absolute_index = lua_absindex(L, index);
//         bool success = stack::check<const char*>(L, absolute_index, handler);
//         tracking.use(1);
// 
//         return success;
//     }
// };
// 
// template <> struct getter<ea::string>
// {
//     static ea::string get(lua_State* L, int index, record& tracking)
//     {
//         tracking.use(1);
//         std::size_t len;
//         auto str = lua_tolstring(L, index, &len);
//         return ea::string(str, len);
//     }
// };
// 
// template <> struct pusher<ea::string>
// {
//     static int push(lua_State* L, const ea::string& str)
//     {
//         lua_pushlstring(L, str.data(), str.size());
//         return 1;
//     }
// 
//     static int push(lua_State* L, const ea::string& str, std::size_t sz)
//     {
//         lua_pushlstring(L, str.data(), sz);
//         return 1;
//     }
// };
// 
// } // namespace stack
// } // namespace sol
// namespace sol
// {
// 
// namespace stack
// {
// 
// template <> struct checker<ea::string>
// {
//     template <typename Handler> static bool check(lua_State* L, int index, Handler&& handler, record& tracking)
//     {
//         int absolute_index = lua_absindex(L, index);
//         bool success = stack::check<const char*>(L, absolute_index, handler);
//         tracking.use(1);
// 
//         return success;
//     }
// };
// 
// template <> struct getter<ea::string>
// {
//     ea::string get(lua_State* L, int index, record& tracking)
//     {
//         tracking.use(1);
//         size_t len;
//         const char* p = lua_tolstring(L, index, &len);
//         return ea::string(p, (ea::string::size_type)len);
//     }
// };
// 
// template <> struct pusher<ea::string>
// {
//     static int push(lua_State* L, const ea::string& str)
//     {
//         lua_pushlstring(L, str.c_str(), str.length());
//         return 1;
//     }
// };
// 
// } // namespace stack
// } // namespace sol

#define CALL_LUA(func,...)                                                          \
sol::protected_function_result result = func(__VA_ARGS__);                          \
if (!result.valid()) {                                                              \
    sol::error err = result;                                                        \
    sol::call_status status = result.status();                                      \
    URHO3D_LOGERRORF("%s error\n\t%s", sol::to_string(status).c_str(), err.what()); \
}
