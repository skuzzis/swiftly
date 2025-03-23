#include "object.h"

PluginObject::PluginObject(std::string m_name, ContextKinds m_kind)
{
    this->name = m_name;
    this->kind = m_kind;
}

PluginObject::~PluginObject() {
}

std::string PluginObject::GetName()
{
    return this->name;
}

PluginState_t PluginObject::GetPluginState()
{
    return this->state;
}

void PluginObject::SetPluginState(PluginState_t state)
{
    this->state = state;
}

std::string PluginObject::GetLoadError()
{
    return this->loadError;
}

void PluginObject::SetLoadError(std::string error)
{
    this->loadError = error;
}

int64_t PluginObject::GetMemoryUsage()
{
    if (this->GetPluginState() == PluginState_t::Stopped) return 0;
    return ctx->GetMemoryUsage();
}

std::string PluginObject::GetAuthor()
{
    if (this->GetPluginState() == PluginState_t::Stopped)
        return "";

    // auto func = EValue::getGlobal(ctx, "GetPluginAuthor");
    // return func().cast<std::string>();
    return "";
}

std::string PluginObject::GetWebsite()
{
    if (this->GetPluginState() == PluginState_t::Stopped)
        return "";

    // auto func = EValue::getGlobal(ctx, "GetPluginWebsite");
    // return func().cast<std::string>();
    return "";
}

std::string PluginObject::GetVersion()
{
    if (this->GetPluginState() == PluginState_t::Stopped)
        return "";

    // auto func = EValue::getGlobal(ctx, "GetPluginVersion");
    // return func().cast<std::string>();
    return "";
}

std::string PluginObject::GetPlName()
{
    if (this->GetPluginState() == PluginState_t::Stopped)
        return "";

    // auto func = EValue::getGlobal(ctx, "GetPluginName");
    // return func().cast<std::string>();
    return "";
}

bool PluginObject::LoadScriptingEnvironment()
{
    this->SetLoadError("");
    ctx = new EContext(kind);

    return true;
}

void PluginObject::DestroyScriptingEnvironment()
{
    this->eventHandlers.clear();
    delete this->ctx;
}

bool PluginObject::ExecuteStart()
{
    // auto PluginAuthor = EValue::getGlobal(ctx, "GetPluginAuthor");
    // if (!PluginAuthor.isFunction())
    // {
    //     PRINTF("Failed to load plugin '%s'.\n", this->GetName().c_str());
    //     PRINT("Error: Mandatory function 'GetPluginAuthor' is not defined.\n");
    //     this->SetLoadError("Mandatory function 'GetPluginAuthor' is not defined.");
    //     return false;
    // }

    // auto PluginWebsite = EValue::getGlobal(ctx, "GetPluginWebsite");
    // if (!PluginWebsite.isFunction())
    // {
    //     PRINTF("Failed to load plugin '%s'.\n", this->GetName().c_str());
    //     PRINT("Error: Mandatory function 'GetPluginWebsite' is not defined.\n");
    //     this->SetLoadError("Mandatory function 'GetPluginWebsite' is not defined.");
    //     return false;
    // }

    // auto PluginVersion = EValue::getGlobal(ctx, "GetPluginVersion");
    // if (!PluginVersion.isFunction())
    // {
    //     PRINTF("Failed to load plugin '%s'.\n", this->GetName().c_str());
    //     PRINT("Error: Mandatory function 'GetPluginVersion' is not defined.\n");
    //     this->SetLoadError("Mandatory function 'GetPluginVersion' is not defined.");
    //     return false;
    // }

    // auto PluginName = EValue::getGlobal(ctx, "GetPluginName");
    // if (!PluginName.isFunction())
    // {
    //     PRINTF("Failed to load plugin '%s'.\n", this->GetName().c_str());
    //     PRINT("Error: Mandatory function 'GetPluginName' is not defined.\n");
    //     this->SetLoadError("Mandatory function 'GetPluginName' is not defined.");
    //     return false;
    // }

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
    //         if (!ext->GetAPI()->OnPluginUnload(this->GetName(), this->ctx, this->GetKind(), error)) {
    //             PRINTF("Failed to unload plugin '%s'.\n", this->GetName().c_str());
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