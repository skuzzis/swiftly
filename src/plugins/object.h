#ifndef _plugins_object_h
#define _plugins_object_h

#include <set>
#include <string>
#include <any>
#include <vector>
#include <optional>

#include <embedder/src/embedder.h>
#include <types/PluginState.h>
#include <types/EventResult.h>

class PluginObject
{
private:
    std::string name;
    PluginState_t state = PluginState_t::Stopped;
    std::string loadError = "";

    EContext* ctx = nullptr;
    ContextKinds kind;

    std::set<std::string> eventHandlers;
    EValue* eventFunctionPtr = nullptr;

public:
    PluginObject(std::string m_name, ContextKinds m_kind);
    PluginObject() = default;
    ~PluginObject();

    std::string GetName();

    ContextKinds GetKind();

    PluginState_t GetPluginState();
    void SetPluginState(PluginState_t state);

    std::string GetLoadError();
    void SetLoadError(std::string error);

    bool LoadScriptingEnvironment();
    void DestroyScriptingEnvironment();
    bool ExecuteStart();
    bool ExecuteStop();

    int64_t GetMemoryUsage();

    std::string GetAuthor();
    std::string GetWebsite();
    std::string GetVersion();
    std::string GetPlName();

    EventResult TriggerEvent(std::string invokedBy, std::string eventName, std::vector<std::any> eventPayload, ClassData* eventObject);
    void RegisterEventHandler(EValue* functionPtr);
    void RegisterEventHandling(std::string eventName);
    void UnregisterEventHandling(std::string eventName);

    EContext* GetContext();
};

#endif