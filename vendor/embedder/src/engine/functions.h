#ifndef _embedder_engine_functions_h
#define _embedder_engine_functions_h

#include "../Value.h"

class FunctionContext
{
private:
    std::string m_function_key;
    ContextKinds m_kind;
    EContext *m_ctx;

    int returnRef = LUA_NOREF;
    JSValue returnVal = JS_UNDEFINED;
    bool stopExecution = false;

    JSValue* m_vals;
    int m_argc;

public:
    FunctionContext(std::string function_key, ContextKinds kind, EContext *ctx);
    FunctionContext(std::string function_key, ContextKinds kind, EContext *ctx, JSValue* vals, int argc);
    ~FunctionContext();

    bool HasResult();

    void StopExecution();
    bool ShouldStopExecution();

    void pushLuaResult()
    {
        if (returnRef == LUA_NOREF)
            return;
        lua_rawgeti(m_ctx->GetLuaState(), LUA_REGISTRYINDEX, returnRef);
    }

    JSValue pushJSResult()
    {
        return returnVal;
    }

    template <class T>
    T GetArgument(int index)
    {
        if (m_kind == ContextKinds::Lua)
        {
            if (index < 0 || index + 1 > lua_gettop(m_ctx->GetLuaState()))
                return *(T *)0;

            return Stack<T>::getLua(m_ctx, index + 1);
        }
        else if (m_kind == ContextKinds::JavaScript)
        {
            if(index < 0 || index >= m_argc) return *(T*)0;
            return Stack<T>::getJS(m_ctx, m_vals[index]);
        }
    }

    template <class T>
    T GetArgumentOr(int index, T defaultVal)
    {
        if (m_kind == ContextKinds::Lua)
        {
            if (index < 0 || index + 1 > lua_gettop(m_ctx->GetLuaState()))
                return defaultVal;

            return Stack<T>::getLua(m_ctx, index + 1);
        }
        else if (m_kind == ContextKinds::JavaScript)
        {
            if(index < 0 || index >= m_argc) return defaultVal;
            return Stack<T>::getJS(m_ctx, m_vals[index]);
        }
    }

    template <class T>
    void SetReturn(T value)
    {
        if (m_kind == ContextKinds::Lua)
        {
            if (returnRef != LUA_NOREF)
                luaL_unref(m_ctx->GetLuaState(), LUA_REGISTRYINDEX, returnRef);

            Stack<T>::pushLua(m_ctx, value);
            returnRef = luaL_ref(m_ctx->GetLuaState(), LUA_REGISTRYINDEX);
        }
        else if (m_kind == ContextKinds::JavaScript)
        {
            if (!JS_IsUndefined(returnVal))
                JS_FreeValue(m_ctx->GetJSState(), returnVal);

            returnVal = Stack<T>::pushJS(m_ctx, value);
        }
    }

    template <class T>
    T GetReturn()
    {
        if (m_kind == ContextKinds::Lua)
        {
            if (returnRef == LUA_NOREF)
                return *(T *)0;
            return Stack<T>::getLua(m_ctx, returnRef);
        }
        else if (m_kind == ContextKinds::JavaScript)
        {
            if (JS_IsUndefined(returnVal))
                return *(T *)0;

            return Stack<T>::getJS(m_ctx, returnVal);
        }
    }
};

#endif