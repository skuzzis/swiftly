#include "core.h"

#include <utils/utils.h>
#include <core/entrypoint.h>

std::string FetchPluginName(EContext* state);
std::map<std::string, EContext*> pluginNamesMap = {};
std::map<EContext*, std::string> pluginNamesMap2 = {};
std::vector<ClassLoader*> loaderClasses;

std::string FetchPluginName(EContext* state)
{
    return pluginNamesMap2[state];
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

    ADD_FUNCTION_NS(ctx->GetKind() == ContextKinds::Lua ? "_G" : "console", ctx->GetKind() == ContextKinds::Lua ? "print" : "log", [](FunctionContext* context) -> void {
        std::string prefix = TerminalProcessColor(string_format("[Swiftly] %s[%s]{DEFAULT} ", GetTerminalStringColor(FetchPluginName(context->GetPluginContext())).c_str(), ("plugin:" + FetchPluginName(context->GetPluginContext())).c_str()));
        
        std::vector<std::string> outputArr;

        for(int i = 0; i < context->GetArgumentsCount(); i++) {
            if(i > 0) outputArr.push_back("\t");
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
        context->SetReturn(FetchPluginName(context->GetPluginContext()));
    });

    for(auto classLoader : loaderClasses)
        classLoader->ExecuteLoad(plugin, ctx);
}