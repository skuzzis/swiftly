#include "../Engine.h"
#include "../CHelpers.h"
#include "../Helpers.h"

#include <regex>

int LuaFunctionCallback(lua_State *L)
{
    std::string str_key = lua_tostring(L, lua_upvalueindex(1));
    auto ctx = GetContextByState(L);

    void *func = ctx->GetFunctionCall(str_key);
    if (!func)
        return 0;

    ScriptingFunctionCallback cb = reinterpret_cast<ScriptingFunctionCallback>(func);
    FunctionContext fctx(str_key, ctx->GetKind(), ctx);
    FunctionContext *fptr = &fctx;

    auto functionPreCalls = ctx->GetFunctionPreCalls();
    bool stopExecution = false;

    for (auto it = functionPreCalls.begin(); it != functionPreCalls.end(); ++it)
        if (std::regex_search(str_key, std::regex(it->first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::optimize | std::regex_constants::nosubs)))
        {
            for (void *func : it->second)
            {
                reinterpret_cast<ScriptingFunctionCallback>(func)(fptr);
                if (fctx.ShouldStopExecution())
                {
                    stopExecution = true;
                    break;
                }
            }
            if (stopExecution)
                break;
        }

    if (stopExecution)
        goto functioncbend;

    cb(fptr);

functioncbend:
    int hasResult = (int)fctx.HasResult();

    if (hasResult != 0)
        fctx.pushLuaResult();

    return hasResult;
}

void AddScriptingFunction(EContext *ctx, std::string namespace_path, std::string function_name, ScriptingFunctionCallback callback)
{
    if (ctx->GetKind() == ContextKinds::Lua)
    {
        int pop_values = 0;
        pop_values++;

        auto L = ctx->GetLuaState();
        lua_pushglobaltable(L);

        if (namespace_path != "_G")
        {
            auto paths = str_split(namespace_path, ".");
            for (auto path : paths)
            {
                rawgetfield(L, -1, path.c_str());

                if (lua_isnil(L, -1))
                {
                    lua_pop(L, 1);

                    lua_newtable(L); // parent ns, ns

                    lua_pushvalue(L, -1); // parent ns, ns, ns

                    // ns.__metatable = ns
                    lua_setmetatable(L, -2); // parent ns, ns

                    // ns.__index = indexMetaMethod
                    lua_pushcfunction(L, &CHelpers::indexMetaMethod);
                    rawsetfield(L, -2, "__index"); // parent ns, ns

                    // ns.__newindex = newindexMetaMethod
                    lua_pushcfunction(L, &CHelpers::newindexStaticMetaMethod);
                    rawsetfield(L, -2, "__newindex"); // pns, ns

                    lua_newtable(L);                     // pns, ns, propget table (pg)
                    lua_rawsetp(L, -2, getPropgetKey()); // ns [propgetKey] = pg. pns, ns

                    lua_newtable(L);                     // pns, ns, propset table (ps)
                    lua_rawsetp(L, -2, getPropsetKey()); // ns [propsetKey] = ps. pns, ns

                    // pns [name] = ns
                    lua_pushvalue(L, -1);             // pns, ns, ns
                    rawsetfield(L, -3, path.c_str()); // pns, ns
                }

                pop_values++;
            }
        }

        auto func_key = namespace_path + " " + function_name;
        ctx->AddFunctionCall(func_key, reinterpret_cast<void *>(callback));

        lua_pushstring(L, func_key.c_str());
        lua_pushcclosure(L, LuaFunctionCallback, 1);
        rawsetfield(L, -2, function_name.c_str());

        lua_pop(L, pop_values);
    }
    else if (ctx->GetKind() == ContextKinds::JavaScript)
    {
    }
}

void AddScriptingFunctionPre(EContext *ctx, std::string namespace_path, std::string function_name, ScriptingFunctionCallback callback)
{
    if (ctx->GetKind() == ContextKinds::Lua)
    {
        auto func_key = namespace_path + " " + function_name;
        ctx->AddFunctionPreCall(func_key, reinterpret_cast<void *>(callback));
    }
    else if (ctx->GetKind() == ContextKinds::JavaScript)
    {
    }
}