#include "command.h"

#include <utils/common.h>
#include <plugins/manager.h>

Command::Command(std::string plugin_name, EValue *functionPtr, std::string name)
{
    m_funcPtr = functionPtr;
    m_pluginName = plugin_name;
    m_commandName = name;
}

Command::~Command()
{
    auto plugin = g_pluginManager.FetchPlugin(m_pluginName);
    if(!plugin) return;

    delete m_funcPtr;
}

void Command::Execute(int slot, std::vector<std::string> args, bool silent, std::string prefix)
{
    if(!m_funcPtr) return;

    if(!m_funcPtr->isFunction()) return;

    try {
        EValue command = *m_funcPtr;
        int argc = args.size();
        command(slot, args, argc, silent, prefix);
    } catch(EException& e) {
        PRINTF("An error has occured while executing command '%s'.\n", m_commandName.c_str());
        PRINTF("%s\n", e.what());
    }
}

std::string Command::GetPluginName()
{
    return m_pluginName;
}