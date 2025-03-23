#include "object.h"
#include "manager.h"

#include <utils/common.h>
#include <utils/utils.h>
#include <filesystem/files/files.h>

PluginObject::PluginObject(std::string m_name, ContextKinds m_kind)
{
    name = m_name;
    kind = m_kind;
}

PluginObject::~PluginObject() {
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
    state = state;
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
    if (GetPluginState() == PluginState_t::Stopped) return 0;
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

    std::vector<std::string> scriptingFiles = Files::FetchFileNames("addons/swiftly/bin/scripting");
    for (std::string file : scriptingFiles)
    {
        if (ends_with(file, fileExt))
        {
            try {
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
            catch (EException& e) {
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
            try {
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
            catch (EException& e) {
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

    // PluginEvent* event = new PluginEvent("core", nullptr, nullptr);
    // TriggerEvent("core", "OnPluginStart", encoders::msgpack::SerializeToString({}), event);
    // delete event;

    if (GetLoadError() != "") return false;

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

    // PluginEvent* event = new PluginEvent("core", nullptr, nullptr);
    // TriggerEvent("core", "OnPluginStop", encoders::msgpack::SerializeToString({}), event);
    // delete event;

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