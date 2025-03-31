#include "GetPush.h"
namespace sol
{
ea::string_view sol_lua_get(sol::types<ea::string_view>, lua_State* L, int index, sol::stack::record& tracking)
{
    int absolute_index = lua_absindex(L, index);
    std::string_view a = sol::stack::get<std::string_view>(L, absolute_index);
    tracking.use(1);
    return ea::string_view(a.data());
}

int sol_lua_push(sol::types<ea::string_view>, lua_State* L, const ea::string_view& obj)
{
    return sol::stack::push(L, obj.data(), obj.length());
}

ea::string sol_lua_get(sol::types<ea::string>, lua_State* L, int index, sol::stack::record& tracking)
{
    int absolute_index = lua_absindex(L, index);
    std::string a = sol::stack::get<std::string>(L, absolute_index);
    tracking.use(1);
    return ea::string(a.c_str());
}

int sol_lua_push(sol::types<ea::string>, lua_State* L, const ea::string& obj)
{
    return sol::stack::push(L, obj.c_str(), obj.length());
}

}
/*
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
    if (!str)
    {
        lua_pushnil(L);
    }
    else
    {
        lua_pushlstring(L, str->c_str(), str->length());
    }
    return 1;
}
*/
