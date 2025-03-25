#include "../Engine.h"
#include "classes.h"
#include "../CHelpers.h"

#include <regex>

int LuaClassFunctionCall(lua_State *L)
{
    std::string str_key = lua_tostring(L, lua_upvalueindex(1));
    auto ctx = GetContextByState(L);

    void *func = ctx->GetClassFunctionCall(str_key);
    if (!func)
        return 0;

    ScriptingClassFunctionCallback cb = reinterpret_cast<ScriptingClassFunctionCallback>(func);
    FunctionContext fctx(str_key, ctx->GetKind(), ctx, true);
    FunctionContext *fptr = &fctx;

    ClassData *data = nullptr;
    bool ignoreCustomReturn = false;

    auto functionPreCalls = ctx->GetClassFunctionPreCalls();
    auto functionPostCalls = ctx->GetClassFunctionPostCalls();
    bool stopExecution = false;

    auto splits = str_split(str_key, " ");
    if (splits[0] == splits[1])
    {
        data = new ClassData({});
        MarkDeleteOnGC(data);

        ClassData **udata = (ClassData **)lua_newuserdata(L, sizeof(ClassData *));
        *udata = data;

        luaL_getmetatable(L, splits[0].c_str());
        lua_setmetatable(L, -2);

        ignoreCustomReturn = true;
    }
    else
    {
        data = *(ClassData **)luaL_checkudata(L, 1, splits[0].c_str());
    }

    // @todo smarter approach, maybe at first function execution try to see if everything is valid, and if it is, cache it in a map the list and just iterate through it
    for (auto it = functionPreCalls.begin(); it != functionPreCalls.end(); ++it)
        if (std::regex_search(str_key, std::regex(it->first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::optimize | std::regex_constants::nosubs)))
        {
            for (void *func : it->second)
            {
                reinterpret_cast<ScriptingClassFunctionCallback>(func)(fptr, data);
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
        goto classfunctioncbend;

    cb(fptr, data);

    // @todo smarter approach, maybe at first function execution try to see if everything is valid, and if it is, cache it in a map the list and just iterate through it
    for (auto it = functionPostCalls.begin(); it != functionPostCalls.end(); ++it)
        if (std::regex_search(str_key, std::regex(it->first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::optimize | std::regex_constants::nosubs)))
        {
            for (void *func : it->second)
            {
                reinterpret_cast<ScriptingClassFunctionCallback>(func)(fptr, data);
                if (fctx.ShouldStopExecution())
                {
                    stopExecution = true;
                    break;
                }
            }
            if (stopExecution)
                break;
        }

classfunctioncbend:
    int hasResult = (int)fctx.HasResult();

    if (ignoreCustomReturn)
        return 1;
    else if (hasResult != 0)
    {
        fctx.pushLuaResult();
    }
    return hasResult;
}

JSValue JSClassCallback(JSContext *L, JSValue this_val, int argc, JSValue *argv, int magic, JSValue *func_data)
{
    auto ctx = GetContextByState(L);
    std::string str_key = Stack<std::string>::getJS(ctx, func_data[0]);

    void *func = ctx->GetClassFunctionCall(str_key);
    if (!func)
        return JS_ThrowSyntaxError(L, "Unknown function: '%s'.", str_split(str_key, " ").back());

    ScriptingClassFunctionCallback cb = reinterpret_cast<ScriptingClassFunctionCallback>(func);
    FunctionContext fctx(str_key, ctx->GetKind(), ctx, argv, argc);
    FunctionContext *fptr = &fctx;
    ClassData *data = nullptr;
    bool ignoreCustomReturn = false;

    auto functionPreCalls = ctx->GetClassFunctionPreCalls();
    auto functionPostCalls = ctx->GetClassFunctionPostCalls();
    bool stopExecution = false;
    JSValue ret = JS_UNDEFINED;

    auto splits = str_split(str_key, " ");
    if (splits[0] == splits[1])
    {
        data = new ClassData({});
        MarkDeleteOnGC(data);

        JSClassID &id = *(ctx->GetClassID(splits[0]));
        auto proto = JS_GetClassProto(L, id);
        ret = JS_NewObjectProtoClass(L, proto, id);
        JS_FreeValue(L, proto);

        if (JS_IsException(ret))
        {
            JS_FreeValue(L, ret);
            return JS_EXCEPTION;
        }
        else
        {
            JS_SetOpaque(ret, (void *)data);
        }

        ignoreCustomReturn = true;
    }
    else
    {
        data = (ClassData *)JS_GetOpaque(this_val, *ctx->GetClassID(splits[0]));
    }

    // @todo smarter approach, maybe at first function execution try to see if everything is valid, and if it is, cache it in a map the list and just iterate through it
    for (auto it = functionPreCalls.begin(); it != functionPreCalls.end(); ++it)
        if (std::regex_search(str_key, std::regex(it->first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::optimize | std::regex_constants::nosubs)))
        {
            for (void *func : it->second)
            {
                reinterpret_cast<ScriptingClassFunctionCallback>(func)(fptr, data);
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
        goto functionclasscbendjs;

    cb(fptr, data);

    // @todo smarter approach, maybe at first function execution try to see if everything is valid, and if it is, cache it in a map the list and just iterate through it
    for (auto it = functionPostCalls.begin(); it != functionPostCalls.end(); ++it)
        if (std::regex_search(str_key, std::regex(it->first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::optimize | std::regex_constants::nosubs)))
        {
            for (void *func : it->second)
            {
                reinterpret_cast<ScriptingClassFunctionCallback>(func)(fptr, data);
                if (fctx.ShouldStopExecution())
                {
                    stopExecution = true;
                    break;
                }
            }
            if (stopExecution)
                break;
        }

functionclasscbendjs:
    if (ignoreCustomReturn)
        return ret;
    else if (fctx.HasResult())
        return fctx.pushJSResult();
    else
        return JS_UNDEFINED;
}

void AddScriptingClass(EContext *ctx, std::string class_name)
{
    if (ctx->GetKind() == ContextKinds::Lua)
    {
        auto L = ctx->GetLuaState();

        luaL_newmetatable(L, class_name.c_str());

        lua_pushcfunction(L, CHelpers::LuaGCFunction);
        rawsetfield(L, -2, "__gc");

        lua_newtable(L);
        rawsetfield(L, -2, "__index");

        lua_pop(L, 1);
    }
    else if (ctx->GetKind() == ContextKinds::JavaScript)
    {
        auto L = ctx->GetJSState();
        auto &classID = *ctx->GetClassID(class_name);
        if (classID != 0)
            return;

        JSRuntime *rt = JS_GetRuntime(L);
        JS_NewClassID(rt, &classID);

        JSClassDef class_def = {
            class_name.c_str()};
        class_def.finalizer = CHelpers::JSGCFunction;

        JS_NewClass(rt, classID, &class_def);
    }
}

void AddScriptingClassFunction(EContext *ctx, std::string class_name, std::string function_name, ScriptingClassFunctionCallback callback)
{
    if (ctx->GetKind() == ContextKinds::Lua)
    {
        auto L = ctx->GetLuaState();

        std::string func_key = class_name + " " + function_name;
        ctx->AddClassFunctionCalls(func_key, reinterpret_cast<void *>(callback));

        if (function_name == class_name)
        {
            lua_pushstring(L, func_key.c_str());
            lua_pushcclosure(L, LuaClassFunctionCall, 1);
            lua_setglobal(L, class_name.c_str());
        }
        else
        {
            luaL_getmetatable(L, class_name.c_str());

            rawgetfield(L, -1, "__index");

            lua_pushstring(L, func_key.c_str());
            lua_pushcclosure(L, LuaClassFunctionCall, 1);
            rawsetfield(L, -2, function_name.c_str());

            lua_pop(L, 2);
        }
    }
    else if (ctx->GetKind() == ContextKinds::JavaScript)
    {
        auto L = ctx->GetJSState();

        std::string func_key = class_name + " " + function_name;
        ctx->AddClassFunctionCalls(func_key, reinterpret_cast<void *>(callback));

        if (function_name == class_name)
        {
            auto ns = JS_GetGlobalObject(L);
            std::vector<JSValue> vals = {Stack<std::string>::pushJS(ctx, func_key)};
            JS_SetPropertyStr(L, ns, class_name.c_str(), JS_NewCFunctionData(L, JSClassCallback, 0, 1, 1, vals.data()));
            JS_FreeValue(L, ns);
        }
        else
        {
            auto proto = JS_GetClassProto(L, *ctx->GetClassID(class_name));

            std::vector<JSValue> vals = {Stack<std::string>::pushJS(ctx, func_key)};
            JS_SetPropertyStr(L, proto, function_name.c_str(), JS_NewCFunctionData(L, JSClassCallback, 0, 1, 1, vals.data()));

            JS_FreeValue(L, proto);
        }
    }
}

void AddScriptingClassFunctionPre(EContext *ctx, std::string class_name, std::string function_name, ScriptingClassFunctionCallback callback)
{
    auto func_key = class_name + " " + function_name;
    ctx->AddClassFunctionPreCalls(func_key, reinterpret_cast<void *>(callback));
}

void AddScriptingClassFunctionPost(EContext *ctx, std::string class_name, std::string function_name, ScriptingClassFunctionCallback callback)
{
    auto func_key = class_name + " " + function_name;
    ctx->AddClassFunctionPostCalls(func_key, reinterpret_cast<void *>(callback));
}

EValue CreateScriptingClassInstance(FunctionContext *context, std::string class_name, std::map<std::string, std::any> classdata)
{
    return CreateScriptingClassInstance(context->GetPluginContext(), class_name, classdata);
}

EValue CreateScriptingClassInstance(EContext *context, std::string class_name, std::map<std::string, std::any> classdata)
{
    if (context->GetKind() == ContextKinds::Lua)
    {
        auto L = context->GetLuaState();
        ClassData *data = new ClassData(classdata);
        MarkDeleteOnGC(data);

        ClassData **udata = (ClassData **)lua_newuserdata(L, sizeof(ClassData *));
        *udata = data;

        luaL_getmetatable(L, class_name.c_str());
        lua_setmetatable(L, -2);

        return EValue(context, 0, false);
    }
    else if (context->GetKind() == ContextKinds::JavaScript)
    {
        auto L = context->GetJSState();
        auto data = new ClassData(classdata);
        MarkDeleteOnGC(data);

        JSClassID &id = *(context->GetClassID(class_name));
        auto proto = JS_GetClassProto(L, id);
        auto ret = JS_NewObjectProtoClass(L, proto, id);
        JS_FreeValue(L, proto);

        if (JS_IsException(ret))
        {
            JS_FreeValue(L, ret);
            return EValue(context);
        }
        else
        {
            JS_SetOpaque(ret, (void *)data);
        }
        EValue v(context, ret);
        JS_FreeValue(L, ret);
        return v;
    }
    else
        return EValue(context);
}