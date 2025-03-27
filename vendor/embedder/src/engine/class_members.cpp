#include "classes.h"
#include "../Engine.h"
#include "../CHelpers.h"

#include <regex>

int LuaMemberCallbackIndex(lua_State* L, std::string str_key)
{
    auto ctx = GetContextByState(L);

    void *func = ctx->GetClassMemberCalls(str_key).first;
    if (!func)
        return 0;

    ScriptingClassFunctionCallback cb = reinterpret_cast<ScriptingClassFunctionCallback>(func);
    FunctionContext fctx(str_key, ctx->GetKind(), ctx, true, false, true);
    FunctionContext *fptr = &fctx;

    auto splits = str_split(str_key, " ");
    ClassData *data = *(ClassData **)luaL_checkudata(L, 1, splits[0].c_str());

    auto functionPreCalls = ctx->GetClassMemberPreCalls();
    auto functionPostCalls = ctx->GetClassMemberPostCalls();
    bool stopExecution = false;

    // @todo smarter approach, maybe at first function execution try to see if everything is valid, and if it is, cache it in a map the list and just iterate through it
    for (auto it = functionPreCalls.begin(); it != functionPreCalls.end(); ++it)
        if (std::regex_search(str_key, std::regex(it->first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::optimize | std::regex_constants::nosubs)))
        {
            for (auto func : it->second)
            {
                reinterpret_cast<ScriptingClassFunctionCallback>(func.first)(fptr, data);
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
        goto membergetcbendlua;

    cb(fptr, data);

    // @todo smarter approach, maybe at first function execution try to see if everything is valid, and if it is, cache it in a map the list and just iterate through it
    for (auto it = functionPostCalls.begin(); it != functionPostCalls.end(); ++it)
        if (std::regex_search(str_key, std::regex(it->first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::optimize | std::regex_constants::nosubs)))
        {
            for (auto func : it->second)
            {
                reinterpret_cast<ScriptingClassFunctionCallback>(func.first)(fptr, data);
                if (fctx.ShouldStopExecution())
                {
                    stopExecution = true;
                    break;
                }
            }
            if (stopExecution)
                break;
        }

membergetcbendlua:
    int hasResult = (int)fctx.HasResult();
    if (hasResult != 0)
    {
        fctx.pushLuaResult();
    }
    return hasResult;
}

int LuaMemberCallbackNewIndex(lua_State* L, std::string str_key)
{
    auto ctx = GetContextByState(L);

    void *func = ctx->GetClassMemberCalls(str_key).second;
    if (!func)
        return 0;

    ScriptingClassFunctionCallback cb = reinterpret_cast<ScriptingClassFunctionCallback>(func);
    FunctionContext fctx(str_key, ctx->GetKind(), ctx, true, false, true);
    FunctionContext *fptr = &fctx;

    auto splits = str_split(str_key, " ");
    ClassData *data = *(ClassData **)luaL_checkudata(L, 1, splits[0].c_str());

    auto functionPreCalls = ctx->GetClassMemberPreCalls();
    auto functionPostCalls = ctx->GetClassMemberPostCalls();
    bool stopExecution = false;

    // @todo smarter approach, maybe at first function execution try to see if everything is valid, and if it is, cache it in a map the list and just iterate through it
    for (auto it = functionPreCalls.begin(); it != functionPreCalls.end(); ++it)
        if (std::regex_search(str_key, std::regex(it->first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::optimize | std::regex_constants::nosubs)))
        {
            for (auto func : it->second)
            {
                reinterpret_cast<ScriptingClassFunctionCallback>(func.second)(fptr, data);
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
        goto membersetcbendlua;

    cb(fptr, data);

    // @todo smarter approach, maybe at first function execution try to see if everything is valid, and if it is, cache it in a map the list and just iterate through it
    for (auto it = functionPostCalls.begin(); it != functionPostCalls.end(); ++it)
        if (std::regex_search(str_key, std::regex(it->first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::optimize | std::regex_constants::nosubs)))
        {
            for (auto func : it->second)
            {
                reinterpret_cast<ScriptingClassFunctionCallback>(func.second)(fptr, data);
                if (fctx.ShouldStopExecution())
                {
                    stopExecution = true;
                    break;
                }
            }
            if (stopExecution)
                break;
        }

membersetcbendlua:
    int hasResult = (int)fctx.HasResult();
    if (hasResult != 0)
    {
        fctx.pushLuaResult();
    }
    return hasResult;
}

JSValue JSMemberGetCallback(JSContext *L, JSValue this_val, int argc, JSValue *argv, int magic, JSValue *func_data)
{
    auto ctx = GetContextByState(L);
    std::string str_key = Stack<std::string>::getJS(ctx, func_data[0]);

    void *func = ctx->GetClassMemberCalls(str_key).first;
    if (!func)
        return JS_ThrowSyntaxError(L, "Unknown member: '%s'.", str_split(str_key, " ").back());

    ScriptingClassFunctionCallback cb = reinterpret_cast<ScriptingClassFunctionCallback>(func);
    FunctionContext fctx(str_key, ctx->GetKind(), ctx, argv, argc);
    FunctionContext *fptr = &fctx;

    ClassData *data = (ClassData *)JS_GetOpaque(this_val, *ctx->GetClassID(str_split(str_key, " ")[0]));

    auto functionPreCalls = ctx->GetClassMemberPreCalls();
    auto functionPostCalls = ctx->GetClassMemberPostCalls();
    bool stopExecution = false;

    // @todo smarter approach, maybe at first function execution try to see if everything is valid, and if it is, cache it in a map the list and just iterate through it
    for (auto it = functionPreCalls.begin(); it != functionPreCalls.end(); ++it)
        if (std::regex_search(str_key, std::regex(it->first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::optimize | std::regex_constants::nosubs)))
        {
            for (auto func : it->second)
            {
                reinterpret_cast<ScriptingClassFunctionCallback>(func.first)(fptr, data);
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
        goto membergetcbendjs;

    cb(fptr, data);

    // @todo smarter approach, maybe at first function execution try to see if everything is valid, and if it is, cache it in a map the list and just iterate through it
    for (auto it = functionPostCalls.begin(); it != functionPostCalls.end(); ++it)
        if (std::regex_search(str_key, std::regex(it->first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::optimize | std::regex_constants::nosubs)))
        {
            for (auto func : it->second)
            {
                reinterpret_cast<ScriptingClassFunctionCallback>(func.first)(fptr, data);
                if (fctx.ShouldStopExecution())
                {
                    stopExecution = true;
                    break;
                }
            }
            if (stopExecution)
                break;
        }

membergetcbendjs:
    if (fctx.HasResult())
        return fctx.pushJSResult();
    return JS_UNDEFINED;
}

JSValue JSMemberSetCallback(JSContext *L, JSValue this_val, int argc, JSValue *argv, int magic, JSValue *func_data)
{
    auto ctx = GetContextByState(L);
    std::string str_key = Stack<std::string>::getJS(ctx, func_data[0]);

    void *func = ctx->GetClassMemberCalls(str_key).second;
    if (!func)
        return JS_ThrowSyntaxError(L, "Unknown member: '%s'.", str_split(str_key, " ").back());

    ScriptingClassFunctionCallback cb = reinterpret_cast<ScriptingClassFunctionCallback>(func);
    FunctionContext fctx(str_key, ctx->GetKind(), ctx, argv, argc);
    FunctionContext *fptr = &fctx;

    ClassData *data = (ClassData *)JS_GetOpaque(this_val, *ctx->GetClassID(str_split(str_key, " ")[0]));

    auto functionPreCalls = ctx->GetClassMemberPreCalls();
    auto functionPostCalls = ctx->GetClassMemberPostCalls();
    bool stopExecution = false;

    // @todo smarter approach, maybe at first function execution try to see if everything is valid, and if it is, cache it in a map the list and just iterate through it
    for (auto it = functionPreCalls.begin(); it != functionPreCalls.end(); ++it)
        if (std::regex_search(str_key, std::regex(it->first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::optimize | std::regex_constants::nosubs)))
        {
            for (auto func : it->second)
            {
                reinterpret_cast<ScriptingClassFunctionCallback>(func.second)(fptr, data);
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
        goto membersetcbendjs;

    cb(fptr, data);

    // @todo smarter approach, maybe at first function execution try to see if everything is valid, and if it is, cache it in a map the list and just iterate through it
    for (auto it = functionPostCalls.begin(); it != functionPostCalls.end(); ++it)
        if (std::regex_search(str_key, std::regex(it->first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::optimize | std::regex_constants::nosubs)))
        {
            for (auto func : it->second)
            {
                reinterpret_cast<ScriptingClassFunctionCallback>(func.second)(fptr, data);
                if (fctx.ShouldStopExecution())
                {
                    stopExecution = true;
                    break;
                }
            }
            if (stopExecution)
                break;
        }

membersetcbendjs:
    if (fctx.HasResult())
        return fctx.pushJSResult();
    return JS_UNDEFINED;
}

void AddScriptingClassMember(EContext *ctx, std::string class_name, std::string member_name, ScriptingClassFunctionCallback callback_get, ScriptingClassFunctionCallback callback_set)
{
    if(ctx->GetKind() == ContextKinds::Lua) {
        auto L = ctx->GetLuaState();

        std::string func_key = class_name + " " + member_name;
        ctx->AddClassMemberCalls(func_key, {reinterpret_cast<void *>(callback_get), reinterpret_cast<void *>(callback_set)});
    } else if(ctx->GetKind() == ContextKinds::JavaScript) {
        auto L = ctx->GetJSState();

        std::string func_key = class_name + " " + member_name;
        ctx->AddClassMemberCalls(func_key, {reinterpret_cast<void *>(callback_get), reinterpret_cast<void *>(callback_set)});

        auto &proto = ctx->GetClassPrototype(class_name);
        JSAtom atom = JS_NewAtom(L, member_name.c_str());

        std::vector<JSValue> vals = {Stack<std::string>::pushJS(ctx, func_key)};
        JS_DefinePropertyGetSet(L, proto, atom, 
            JS_NewCFunctionData(L, JSMemberGetCallback, 0, 0, 1, vals.data()),
            JS_NewCFunctionData(L, JSMemberSetCallback, 1, 0, 1, vals.data()),
            0
        );

        JS_FreeAtom(L, atom);
    }
}

void AddScriptingClassMemberPre(EContext *ctx, std::string class_name, std::string member_name, ScriptingClassFunctionCallback callback_get, ScriptingClassFunctionCallback callback_set)
{
    auto func_key = class_name + " " + member_name;
    ctx->AddClassMemberPreCalls(func_key, {reinterpret_cast<void *>(callback_get), reinterpret_cast<void *>(callback_set)});
}

void AddScriptingClassMemberPost(EContext *ctx, std::string class_name, std::string member_name, ScriptingClassFunctionCallback callback_get, ScriptingClassFunctionCallback callback_set)
{
    auto func_key = class_name + " " + member_name;
    ctx->AddClassMemberPostCalls(func_key, {reinterpret_cast<void *>(callback_get), reinterpret_cast<void *>(callback_set)});
}