#include "functions.h"

FunctionContext::FunctionContext(std::string function_key, ContextKinds kind, EContext *ctx)
{
    m_function_key = function_key;
    m_kind = kind;
    m_ctx = ctx;
}

FunctionContext::~FunctionContext()
{
    if (returnRef != LUA_NOREF)
        luaL_unref(m_ctx->GetLuaState(), LUA_REGISTRYINDEX, returnRef);
}

bool FunctionContext::HasResult()
{
    return (returnRef != LUA_NOREF || !JS_IsUndefined(returnVal));
}