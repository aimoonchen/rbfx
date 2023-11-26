#include "GetPush.h"

using namespace Urho3D;

ea::string sol_lua_get(sol::types<ea::string>, lua_State* L, int index, sol::stack::record& tracking)
{
    tracking.use(1);
    size_t len;
    const char* p = lua_tolstring(L, index, &len);
    return ea::string(p, (unsigned int)len);
}

ea::string sol_lua_get(sol::types<const ea::string&>, lua_State* L, int index, sol::stack::record& tracking)
{
    tracking.use(1);
    size_t len;
    const char* p = lua_tolstring(L, index, &len);
    return ea::string(p, (unsigned int)len);
}

int sol_lua_push(lua_State* L, const ea::string& str)
{
    lua_pushlstring(L, str.c_str(), str.length());
    return 1;
}

int sol_lua_push(lua_State* L, const ea::string* str)
{
    if (!str) {
        lua_pushnil(L);
    } else {
        lua_pushlstring(L, str->c_str(), str->length());
    }
    return 1;
}

// StringHash sol_lua_get(sol::types<StringHash>, lua_State* L, int index, sol::stack::record& tracking)
// {
//     tracking.use(1);
//     size_t len;
//     const char* p = lua_tolstring(L, index, &len);
//     return StringHash(p);
// }
// StringHash sol_lua_get(sol::types<const StringHash&>, lua_State* L, int index, sol::stack::record& tracking)
// {
//     tracking.use(1);
//     size_t len;
//     const char* p = lua_tolstring(L, index, &len);
//     return StringHash(p);
// }
// 
// int sol_lua_push(lua_State* L, StringHash str)
// {
//      const auto& cstr = str.ToString();
//      return sol::stack::push(L, cstr.CString());
// }
