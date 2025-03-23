#ifndef _embedder_helpers_h
#define _embedder_helpers_h

#include <lua.hpp>
#include <quickjs.h>
#include <cassert>
#include <string>

#include "Context.h"

inline int get_length(lua_State* L, int idx)
{
    lua_len(L, idx);
    int len = int(luaL_checknumber(L, -1));
    lua_pop(L, 1);
    return len;
}

inline void rawgetfield(lua_State* L, int index, char const* key)
{
    assert(lua_istable(L, index));
    index = lua_absindex(L, index);
    lua_pushstring(L, key);
    lua_rawget(L, index);
}

inline void rawsetfield(lua_State* L, int index, char const* key)
{
    assert(lua_istable(L, index));
    index = lua_absindex(L, index);
    lua_pushstring(L, key);
    lua_insert(L, -2);
    lua_rawset(L, index);
}

inline const void* getPropgetKey()
{
#ifdef _NDEBUG
    static char value;
    return &value;
#else
    return reinterpret_cast<void*>(0x6e7);
#endif
}

inline const void* getParentKey()
{
#ifdef _NDEBUG
    static char value;
    return &value;
#else
    return reinterpret_cast<void*>(0xdad);
#endif
}

inline const void* getPropsetKey()
{
#ifdef _NDEBUG
    static char value;
    return &value;
#else
    return reinterpret_cast<void*>(0x5e7);
#endif
}

inline const void* getContextKey()
{
#ifdef _NDEBUG
    static char value;
    return &value;
#else
    return reinterpret_cast<void*>(0xbee);
#endif
}

inline uint32_t JSGetArrayLength(JSContext* ctx, JSValue val)
{
    uint32_t len;
    auto prop = JS_GetPropertyStr(ctx, val, "length");
    JS_ToUint32(ctx, &len, prop);
    JS_FreeValue(ctx, prop);
    return len;
}

inline std::string getClassName(JSContext *ctx, JSValue obj) {
    if (!JS_IsObject(obj)) {
        return "";
    }

    return GetContextByState(ctx)->GetClsName(JS_GetClassID(obj));
}

#endif