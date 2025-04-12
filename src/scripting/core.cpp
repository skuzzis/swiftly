#include "core.h"

#include <utils/utils.h>
#include <core/entrypoint.h>

#include <tools/crashreporter/callstack.h>

std::string FetchPluginName(EContext* state);
std::map<std::string, EContext*> pluginNamesMap = {};
std::map<EContext*, std::string> pluginNamesMap2 = {};
std::vector<ClassLoader*> loaderClasses;

std::string FetchPluginName(EContext* state)
{
    return pluginNamesMap2[state];
}

extern "C" {
    LUALIB_API int luaopen_rapidjson(lua_State* L);
}

void SetupScriptingEnvironment(PluginObject plugin, EContext* ctx)
{
    if (pluginNamesMap.find(plugin.GetName()) == pluginNamesMap.end()) {
        pluginNamesMap.insert({ plugin.GetName(), ctx });
        pluginNamesMap2.insert({ ctx, plugin.GetName() });
    }
    else {
        EContext* st = pluginNamesMap[plugin.GetName()];
        pluginNamesMap2.erase(st);

        pluginNamesMap2.insert({ ctx, plugin.GetName() });
        pluginNamesMap[plugin.GetName()] = ctx;
    }

    if (ctx->GetKind() == ContextKinds::Lua) ctx->RegisterLuaLib("json", luaopen_rapidjson);

    ADD_FUNCTION_NS(ctx->GetKind() == ContextKinds::Lua ? "_G" : "console", ctx->GetKind() == ContextKinds::Lua ? "print" : "log", [](FunctionContext* context) -> void {
        std::string prefix = TerminalProcessColor(string_format("[Swiftly] %s[%s]{DEFAULT} ", GetTerminalStringColor(FetchPluginName(context->GetPluginContext())).c_str(), ("plugin:" + FetchPluginName(context->GetPluginContext())).c_str()));

        std::vector<std::string> outputArr;

        for (int i = 0; i < context->GetArgumentsCount(); i++) {
            if (i > 0) outputArr.push_back("\t");
            outputArr.push_back(TerminalProcessColor(context->GetArgumentAsString(i)));
        }

        std::vector<std::string> processingNewLines = explode(implode(outputArr, ""), "\n");
        for (const std::string str : processingNewLines)
        {
            if (str.size() == 0)
                continue;

            std::string final_string = TerminalProcessColor(string_format("%s%s{DEFAULT}{BGDEFAULT}\n", prefix.c_str(), str.c_str()));
            META_CONPRINT(final_string.c_str());
        }
    });

    ADD_FUNCTION("GetCurrentPluginName", [](FunctionContext* context) -> void {
        int* someptr = nullptr;
        *someptr = 4;
        context->SetReturn(FetchPluginName(context->GetPluginContext()));
    });

    for (auto classLoader : loaderClasses)
        classLoader->ExecuteLoad(plugin, ctx);

    ADD_FUNCTION("OnFunctionContextRegister", [](FunctionContext* context) -> void {
        std::string function_call = replace(context->GetFunctionKey(), " ", "::");
        std::vector<std::string> arguments;
        for (int i = 0; i < context->GetArgumentsCount(); i++)
            arguments.push_back(context->GetArgumentAsString(i));
        function_call += "(" + implode(arguments, ", ") + ")";

        context->temporaryData.push_back(g_callStack.RegisterPluginCallstack(FetchPluginName(context->GetPluginContext()), function_call));
    });

    ADD_FUNCTION("OnFunctionContextUnregister", [](FunctionContext* context) -> void {
        g_callStack.UnregisterPluginCallstack(FetchPluginName(context->GetPluginContext()), context->temporaryData[0]);
    });
}