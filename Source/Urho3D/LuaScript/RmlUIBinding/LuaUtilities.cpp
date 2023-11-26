/*
 * This source file is part of RmlUi, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://github.com/mikke89/RmlUi
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 * Copyright (c) 2019 The RmlUi Team, and contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#include <RmlUi/Core/Variant.h>
#include "LuaUtilities.h"
#include <sol/sol.hpp>

namespace Rml {
namespace Lua {

void PushVariant(lua_State* L, const Variant* var)
{
	if (!var)
	{
		lua_pushnil(L);
		return;
	}

	switch (var->GetType())
	{
	case Variant::BOOL:
		lua_pushboolean(L, var->Get<bool>());
		break;
	case Variant::BYTE:
	case Variant::CHAR:
	case Variant::INT:
		lua_pushinteger(L, var->Get<int>());
		break;
	case Variant::INT64:
		lua_pushinteger(L, var->Get<int64_t>());
		break;
	case Variant::UINT:
		lua_pushinteger(L, var->Get<unsigned int>());
		break;
	case Variant::UINT64:
		lua_pushinteger(L, var->Get<uint64_t>());
		break;
	case Variant::FLOAT:
	case Variant::DOUBLE:
		lua_pushnumber(L, var->Get<double>());
		break;
	case Variant::COLOURB:
		//LuaType<Colourb>::push(L, new Colourb(var->Get<Colourb>()), true);
		break;
	case Variant::COLOURF:
		//LuaType<Colourf>::push(L, new Colourf(var->Get<Colourf>()), true);
		break;
	case Variant::STRING:
	{
		const String& s = var->GetReference<Rml::String>();
		lua_pushlstring(L, s.c_str(), s.length());
	}
	break;
	case Variant::VECTOR2:
		//according to Variant.inl, it is going to be a Vector2f
		//LuaType<Vector2f>::push(L, new Vector2f(var->Get<Vector2f>()), true);
		break;
	case Variant::VOIDPTR:
		lua_pushlightuserdata(L, var->Get<void*>());
		break;
	default:
		lua_pushnil(L);
		break;
	}
}

void GetVariant(lua_State* L, int index, Variant* variant)
{
	if (!variant)
		return;

	switch (lua_type(L, index))
	{
	case LUA_TBOOLEAN:
		*variant = (bool)lua_toboolean(L, index);
		break;
	case LUA_TNUMBER:
		*variant = lua_tonumber(L, index);
		break;
	case LUA_TSTRING:
		*variant = Rml::String(lua_tostring(L, index));
		break;
	case LUA_TLIGHTUSERDATA:
		*variant = lua_touserdata(L, index);
		break;
	case LUA_TNIL:
	default:	// todo: support other types
		*variant = Variant();
		break;
	}
}

void PushIndex(lua_State* L, int index)
{
	lua_pushinteger(L, index + 1);
}

int GetIndex(lua_State* L, int index)
{
	return (int)luaL_checkinteger(L, index) - 1;
}

static int ErrorHandler(lua_State* L)
{
    const char* msg = lua_tostring(L, 1);
    if (msg == NULL)
    {
        if (luaL_callmeta(L, 1, "__tostring") && lua_type(L, -1) == LUA_TSTRING)
            return 1;
        else
            msg = lua_pushfstring(L, "(error object is a %s value)", luaL_typename(L, 1));
    }
    luaL_traceback(L, L, msg, 1);
    return 1;
}

static bool LuaCall(lua_State* L, int nargs, int nresults)
{
    int errfunc = -2 - nargs;
    lua_pushcfunction(L, ErrorHandler);
    lua_insert(L, errfunc);
    if (lua_pcall(L, nargs, nresults, errfunc) != LUA_OK)
    {
        Log::Message(Log::LT_WARNING, "%s", lua_tostring(L, -1));
        lua_pop(L, 2);
        return false;
    }
    lua_remove(L, -1 - nresults);
    return true;
}

void BeginCall(lua_State* L, int funRef)
{
    lua_settop(L, 0); // empty stack
    // lua_getref(g_L,funRef);
    lua_rawgeti(L, LUA_REGISTRYINDEX, (int)funRef);
}

bool ExecuteCall(lua_State* L, int params, int res)
{
    return LuaCall(L, params, res);
}

void EndCall(lua_State* L, int res)
{
    lua_pop(L, res);
}

} // namespace Lua
} // namespace Rml
