#ifndef _plugins_object_h
#define _plugins_object_h

#include <set>
#include <string>

#include <embedder/src/embedder.h>
#include <types/PluginState.h>

class PluginObject
{
private:
    std::string name;
    PluginState_t state = PluginState_t::Stopped;
    std::string loadError = "";
    std::set<std::string> eventHandlers;
    ContextKinds kind;
    EContext* ctx = nullptr;

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

    // @todo Events & Commands

    // EventResult TriggerEvent(std::string invokedBy, std::string eventName, std::string eventPayload, PluginEvent* event);
    // void RegisterEventHandler(void* functionPtr);
    // void RegisterEventHandling(std::string eventName);
    // void UnregisterEventHandling(std::string eventName);
    
    // void ExecuteCommand(void* functionPtr, std::string name, int slot, std::vector<std::string> args, bool silent, std::string prefix);

    EContext* GetContext();
};

#endif