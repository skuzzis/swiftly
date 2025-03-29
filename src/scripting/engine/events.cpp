#include <scripting/core.h>

LoadScriptingComponent(events, [](PluginObject plugin, EContext* ctx) -> void {
    ADD_FUNCTION("AddGlobalEvents", [](FunctionContext* context) -> void {
        if (context->GetArgumentsCount() != 1) return;

        auto func = context->GetArgument<EValue>(0);
        if (!func.isFunction()) return;

        auto plugin = g_pluginManager.FetchPlugin(FetchPluginName(context->GetPluginContext()));
        if (!plugin) return;

        plugin->RegisterEventHandler(new EValue(func));
    });

    ADD_FUNCTION("RegisterEventHandlerPlugin", [](FunctionContext* context) -> void {
        if (context->GetArgumentsCount() != 1) return;

        auto event_name = context->GetArgumentOr<std::string>(0, "");

        auto plugin = g_pluginManager.FetchPlugin(FetchPluginName(context->GetPluginContext()));
        if (!plugin) return;

        plugin->RegisterEventHandling(event_name);
    });

    ADD_FUNCTION("RemoveEventHandlerPlugin", [](FunctionContext* context) -> void {
        if (context->GetArgumentsCount() != 1) return;

        auto event_name = context->GetArgumentOr<std::string>(0, "");

        auto plugin = g_pluginManager.FetchPlugin(FetchPluginName(context->GetPluginContext()));
        if (!plugin) return;

        plugin->UnregisterEventHandling(event_name);
    });

    ADD_FUNCTION("TriggerEventInternal", [](FunctionContext* context) -> void {
        if (context->GetArgumentsCount() != 2) return;

        auto event_name = context->GetArgumentOr<std::string>(0, "");
        auto event_data = context->GetArgumentOr<std::vector<std::any>>(1, {});

        auto evObject = MAKE_CLASS_INSTANCE("Event", { { "plugin_name", FetchPluginName(context->GetPluginContext()) } }).cast<ClassData*>();

        std::vector<std::any> returnValues;

        returnValues.push_back((int)g_pluginManager.ExecuteEvent(FetchPluginName(context->GetPluginContext()), event_name, event_data, evObject));
        returnValues.push_back(evObject);

        context->SetReturn(returnValues);
    });

    ADD_CLASS("Event");

    ADD_CLASS_FUNCTION("Event", "Event", [](FunctionContext* context, ClassData* data) -> void {
        if (context->GetArgumentOr<std::string>(0, "") == "") return;
        std::string event_name = context->GetArgument<std::string>(0);
    });

    if (ctx->GetKind() == ContextKinds::JavaScript) ADD_VARIABLE("_G", "PEvent", EValue::getGlobal(ctx, "Event"));
})