#pragma once

#include <any>
#include <embedder/src/embedder.h>

std::any DeserializeData(EValue ref, EContext* state);
EValue SerializeData(std::any data, EContext* state);

template<class T>
struct Stack;

template<>
struct Stack<std::any>
{
    static void pushLua(EContext* ctx, std::any value)
    {
        SerializeData(value, ctx).pushLua();
    }

    static JSValue pushJS(EContext* ctx, std::any value)
    {
        return JS_DupValue(ctx->GetJSState(), SerializeData(value, ctx).pushJS());
    }

    static std::any getLua(EContext* ctx, int ref)
    {
        return DeserializeData(EValue(ctx, ref, false), ctx);
    }

    static std::any getJS(EContext* ctx, JSValue value)
    {
        return DeserializeData(EValue(ctx, value), ctx);
    }

    static bool isLuaInstance(EContext* ctx, int ref)
    {
        return true;
    }

    static bool isJSInstance(EContext* ctx, JSValue value)
    {
        return true;
    }
};