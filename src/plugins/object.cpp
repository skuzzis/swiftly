#include "object.h"
#include "manager.h"

#include <utils/common.h>
#include <utils/utils.h>
#include <filesystem/files/files.h>
#include <scripting/core.h>
#include <server/commands/manager.h>
#include <tools/crashreporter/callstack.h>

PluginObject::PluginObject(std::string m_name, ContextKinds m_kind)
{
    name = m_name;
    kind = m_kind;
}

PluginObject::~PluginObject()
{
    if (eventFunctionPtr)
    {
        delete eventFunctionPtr;
    }
}

void PluginObject::RegisterEventHandler(EValue* functionPtr)
{
    if (!functionPtr->isFunction())
        return;

    eventFunctionPtr = functionPtr;
}

void PluginObject::RegisterEventHandling(std::string eventName)
{
    if (eventHandlers.find(eventName) == eventHandlers.end()) {
        eventHandlers.insert(eventName);
    }
}

void PluginObject::UnregisterEventHandling(std::string eventName)
{
    auto it = eventHandlers.find(eventName);
    if (it != eventHandlers.end())
        eventHandlers.erase(it);
}

EventResult PluginObject::TriggerEvent(std::string invokedBy, std::string eventName, std::vector<std::any> eventPayload, ClassData* eventObject)
{
    if (GetPluginState() == PluginState_t::Stopped && eventName != "OnPluginStart" && eventName != "OnAllPluginsLoaded")
        return EventResult::Continue;

    if (!eventFunctionPtr)
        return EventResult::Continue;

    if (eventHandlers.find(eventName) == eventHandlers.end())
        return EventResult::Continue;

    uint64_t stackid = g_callStack.RegisterPluginCallstack(GetName(), string_format("Event: %s(invokedBy=\"%s\",payloadSize=%d,event=%p)", eventName.c_str(), invokedBy.c_str(), eventPayload.size(), (void*)eventObject));

    EventResult response = EventResult::Continue;
    try
    {
        if (!eventObject) eventObject = new ClassData({ { "plugin_name", invokedBy } }, "Event", ctx);
        auto value = (*eventFunctionPtr)(eventObject, eventName, eventPayload);
        if (value.isNumber())
        {
            int result = value.cast<int>();
            if (result < (int)EventResult::Continue || result >(int)EventResult::Stop)
                response = EventResult::Continue;
            else
                response = (EventResult)result;
        }
    }
    catch (EException& e)
    {
        PRINTF("An error has occured while trying to execute event '%s' in plugin '%s'.\nError: %s", eventName.c_str(), GetName().c_str(), e.what());
        response = EventResult::Continue;
    }

    g_callStack.UnregisterPluginCallstack(GetName(), stackid);

    return response;
}

std::string PluginObject::GetName()
{
    return name;
}

PluginState_t PluginObject::GetPluginState()
{
    return state;
}

void PluginObject::SetPluginState(PluginState_t state)
{
    this->state = state;
}

std::string PluginObject::GetLoadError()
{
    return loadError;
}

void PluginObject::SetLoadError(std::string error)
{
    loadError = error;
}

int64_t PluginObject::GetMemoryUsage()
{
    if (GetPluginState() == PluginState_t::Stopped)
        return 0;
    return ctx->GetMemoryUsage();
}

std::string PluginObject::GetAuthor()
{
    if (GetPluginState() == PluginState_t::Stopped)
        return "";

    auto func = EValue::getGlobal(ctx, "GetPluginAuthor");
    return func().cast<std::string>();
}

std::string PluginObject::GetWebsite()
{
    if (GetPluginState() == PluginState_t::Stopped)
        return "";

    auto func = EValue::getGlobal(ctx, "GetPluginWebsite");
    return func().cast<std::string>();
}

std::string PluginObject::GetVersion()
{
    if (GetPluginState() == PluginState_t::Stopped)
        return "";

    auto func = EValue::getGlobal(ctx, "GetPluginVersion");
    return func().cast<std::string>();
}

std::string PluginObject::GetPlName()
{
    if (GetPluginState() == PluginState_t::Stopped)
        return "";

    auto func = EValue::getGlobal(ctx, "GetPluginName");
    return func().cast<std::string>();
}

bool PluginObject::LoadScriptingEnvironment()
{
    SetLoadError("");
    ctx = new EContext(kind);

    std::string fileExt = GetKind() == ContextKinds::Lua ? ".lua" : ".js";

    SetupScriptingEnvironment(*this, ctx);

    std::vector<std::string> scriptingFiles = Files::FetchFileNames("addons/swiftly/bin/scripting");
    for (std::string file : scriptingFiles)
    {
        if (ends_with(file, fileExt))
        {
            try
            {
                int loadStatus = ctx->RunFile(GeneratePath(file));

                if (loadStatus != 0)
                {
                    std::string error = EException(ctx->GetState(), ctx->GetKind(), loadStatus).what();
                    PRINTF("Failed to load plugin file '%s'.\n", file.c_str());
                    PRINTF("Error: %s\n", error.c_str());

                    SetLoadError(error);
                    return false;
                }
            }
            catch (EException& e)
            {
                std::string error = e.what();
                PRINTF("Failed to load plugin file '%s'.\n", file.c_str());
                PRINTF("Error: %s\n", error.c_str());

                SetLoadError(error);
                return false;
            }
        }
    }

    std::vector<std::string> files = Files::FetchFileNames(g_pluginManager.GetPluginBasePath(GetName()) + "/" + GetName());
    for (std::string file : files)
    {
        if (ends_with(file, fileExt))
        {
            try
            {
                int loadStatus = ctx->RunFile(GeneratePath(file));

                if (loadStatus != 0)
                {
                    std::string error = EException(ctx->GetState(), ctx->GetKind(), loadStatus).what();
                    PRINTF("Failed to load plugin file '%s'.\n", file.c_str());
                    PRINTF("Error: %s\n", error.c_str());

                    SetLoadError(error);
                    return false;
                }
            }
            catch (EException& e)
            {
                std::string error = e.what();
                PRINTF("Failed to load plugin file '%s'.\n", file.c_str());
                PRINTF("Error: %s\n", error.c_str());

                SetLoadError(error);
                return false;
            }
        }
    }

    return true;
}

void PluginObject::DestroyScriptingEnvironment()
{
    std::vector<std::string> commandNames = g_commandsManager.FetchCommandsByPlugin(GetName());
    for (std::string command : commandNames)
        g_commandsManager.UnregisterCommand(command);

    eventHandlers.clear();
    delete ctx;
}

bool PluginObject::ExecuteStart()
{
    auto PluginAuthor = EValue::getGlobal(ctx, "GetPluginAuthor");
    if (!PluginAuthor.isFunction())
    {
        PRINTF("Failed to load plugin '%s'.\n", GetName().c_str());
        PRINT("Error: Mandatory function 'GetPluginAuthor' is not defined.\n");
        SetLoadError("Mandatory function 'GetPluginAuthor' is not defined.");
        return false;
    }

    auto PluginWebsite = EValue::getGlobal(ctx, "GetPluginWebsite");
    if (!PluginWebsite.isFunction())
    {
        PRINTF("Failed to load plugin '%s'.\n", GetName().c_str());
        PRINT("Error: Mandatory function 'GetPluginWebsite' is not defined.\n");
        SetLoadError("Mandatory function 'GetPluginWebsite' is not defined.");
        return false;
    }

    auto PluginVersion = EValue::getGlobal(ctx, "GetPluginVersion");
    if (!PluginVersion.isFunction())
    {
        PRINTF("Failed to load plugin '%s'.\n", GetName().c_str());
        PRINT("Error: Mandatory function 'GetPluginVersion' is not defined.\n");
        SetLoadError("Mandatory function 'GetPluginVersion' is not defined.");
        return false;
    }

    auto PluginName = EValue::getGlobal(ctx, "GetPluginName");
    if (!PluginName.isFunction())
    {
        PRINTF("Failed to load plugin '%s'.\n", GetName().c_str());
        PRINT("Error: Mandatory function 'GetPluginName' is not defined.\n");
        SetLoadError("Mandatory function 'GetPluginName' is not defined.");
        return false;
    }

    TriggerEvent("core", "OnPluginStart", {}, {});

    if (GetLoadError() != "")
        return false;

    return true;
}

bool PluginObject::ExecuteStop()
{
    // for (Extension* ext : extManager->GetExtensionsList())
    //     if (ext->IsLoaded()) {
    //         std::string error = "";
    //         if (!ext->GetAPI()->OnPluginUnload(GetName(), ctx, GetKind(), error)) {
    //             PRINTF("Failed to unload plugin '%s'.\n", GetName().c_str());
    //             PRINTF("Error: %s.\n", error.c_str());
    //             return false;
    //         }
    //     }

    TriggerEvent("core", "OnPluginStop", {}, {});

    return true;
}

ContextKinds PluginObject::GetKind()
{
    return kind;
}

EContext* PluginObject::GetContext()
{
    return ctx;
}