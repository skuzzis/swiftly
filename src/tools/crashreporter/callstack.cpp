#include "callstack.h"

uint64_t stackID = 0;

uint64_t CallStackManager::RegisterPluginCallstack(std::string plugin_name, std::string stackMsg)
{
    ++stackID;
    if (stackID % 2000000000 == 0)
        stackID = 1;

    callStacks[plugin_name].insert_or_assign(stackID, stackMsg);
    return stackID;
}

void CallStackManager::UnregisterPluginCallstack(std::string plugin_name, uint64_t stackID)
{
    if (callStacks[plugin_name].find(stackID) == callStacks[plugin_name].end()) return;

    callStacks[plugin_name].erase(stackID);
}

std::unordered_map<uint64_t, std::string> CallStackManager::GetPluginCallstack(std::string plugin_name)
{
    return callStacks[plugin_name];
}
