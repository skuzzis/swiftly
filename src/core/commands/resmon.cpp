#include "commands.h"

#include <tools/resourcemonitor/monitor.h>
#include <plugins/manager.h>

#include <sstream>

void SwiftlyResourceMonitorManagerHelp(CPlayerSlot slot)
{
    PrintToClientOrConsole(slot, "Commands", "Swiftly Resource Monitor Menu\n");
    PrintToClientOrConsole(slot, "Commands", "Usage: swiftly resmon <command>\n");
    PrintToClientOrConsole(slot, "Commands", " enable          - Enabled the usage monitoring.\n");
    PrintToClientOrConsole(slot, "Commands", " disable         - Disables the usage monitoring.\n");
    PrintToClientOrConsole(slot, "Commands", " view            - Shows the usage monitored.\n");
    PrintToClientOrConsole(slot, "Commands", " viewplugin <ID> - Shows the usage monitored for a specific plugin.\n");
}

void SwiftlyResourceMonitorManagerEnable(CPlayerSlot slot)
{
    if (g_ResourceMonitor.IsEnabled())
        return PrintToClientOrConsole(slot, "Resource Monitor", "Resource monitoring is already enabled.\n");

    g_ResourceMonitor.Enable();
    PrintToClientOrConsole(slot, "Resource Monitor", "Resource monitoring has been enabled.\n");
}

void SwiftlyResourceMonitorManagerDisable(CPlayerSlot slot)
{
    if (!g_ResourceMonitor.IsEnabled())
        return PrintToClientOrConsole(slot, "Resource Monitor", "Resource monitoring is already disabled.\n");

    g_ResourceMonitor.Disable();
    PrintToClientOrConsole(slot, "Resource Monitor", "Resource monitoring has been disabled.\n");
}

void SwiftlyResourceMonitorManagerViewPlugin(CPlayerSlot slot, std::string plugin_id)
{
    if (!g_ResourceMonitor.IsEnabled())
        return PrintToClientOrConsole(slot, "Resource Monitor", "Resource monitoring is not enabled.\n");

    if (!g_pluginManager.PluginExists(plugin_id) && plugin_id != "core")
        return PrintToClientOrConsole(slot, "Resource Monitor", "Invalid plugin ID.\n");

    auto PrintTable = [](TextTable tbl) -> void
        {
            std::stringstream outputTable;
            outputTable << tbl;
            std::vector<std::string> rows = explode(outputTable.str(), "\n");
            for (int i = 0; i < rows.size(); i++)
                PLUGIN_PRINTF("Resource Monitor", "%s\n", rows[i].c_str());
        };

    PrintToClientOrConsole(slot, "Resource Monitor", "Resource Monitor View Plugin\n");
    PrintToClientOrConsole(slot, "Resource Monitor", "ID: %s\n", plugin_id.c_str());
    PrintToClientOrConsole(slot, "Resource Monitor", " \n", plugin_id.c_str());

    PrintToClientOrConsole(slot, "Resource Monitor", "Plugin Usage View\n");

    TextTable usagesTable('-', '|', '+');

    usagesTable.add(" ID ");
    usagesTable.add(" Name ");
    usagesTable.add(" Calls ");
    usagesTable.add(" min/avg/max ");
    usagesTable.endOfRow();

    std::map<std::string, std::map<std::string, std::vector<float>>> data = g_ResourceMonitor.GetResmonTimeTables();
    if (data.count(plugin_id) > 0)
    {
        std::map<std::string, std::vector<float>> pluginData = data.at(plugin_id);
        uint64_t idx = 0;
        for (std::map<std::string, std::vector<float>>::iterator it = pluginData.begin(); it != pluginData.end(); ++it)
        {
            ++idx;
            usagesTable.add(string_format(" %02d. ", idx));
            usagesTable.add(string_format(" %s ", it->first.c_str()));
            usagesTable.add(string_format(" %llu ", it->second.size()));

            if (it->second.size() == 0)
                usagesTable.add(" 0.00000ms / 0.00000ms / 0.00000ms ");
            else
            {
                auto [min, max] = std::minmax_element(it->second.begin(), it->second.end());

                float avg = 0;
                uint64_t avgCount = 0;
                for (std::vector<float>::iterator ii = it->second.begin(); ii != it->second.end(); ++ii)
                {
                    avg += *(ii);
                    ++avgCount;
                }

                usagesTable.add(string_format(" %.5fms / %.5fms / %.5fms ", *min, (avg / avgCount), *max));
            }
            usagesTable.endOfRow();
        }
    }

    PrintTable(usagesTable);
}

void SwiftlyResourceMonitorManagerView(CPlayerSlot slot)
{
    if (!g_ResourceMonitor.IsEnabled())
        return PrintToClientOrConsole(slot, "Resource Monitor", "Resource monitoring is not enabled.\n");

    TextTable pluginsTable('-', '|', '+');

    pluginsTable.add(" ID ");
    pluginsTable.add(" Status ");
    pluginsTable.add(" Type ");
    pluginsTable.add(" Memory ");
    pluginsTable.add(" min/avg/max ");
    pluginsTable.endOfRow();

    auto PrintTable = [](TextTable tbl) -> void
        {
            std::stringstream outputTable;
            outputTable << tbl;
            std::vector<std::string> rows = explode(outputTable.str(), "\n");
            for (int i = 0; i < rows.size(); i++)
                PLUGIN_PRINTF("Resource Monitor", "%s\n", rows[i].c_str());
        };

    PLUGIN_PRINTF("Resource Monitor", "Plugin Resource Viewer\n");

    std::map<std::string, std::map<std::string, std::vector<float>>> data = g_ResourceMonitor.GetResmonTimeTables();

    pluginsTable.add(" core ");
    pluginsTable.add(" Loaded ");
    pluginsTable.add(" - ");
    pluginsTable.add(" - ");

    if (data.find("core") != data.end())
    {
        float max = 0;
        uint64_t maxCount = 0;
        float avg = 0;
        uint64_t avgCount = 0;
        float min = 0;
        uint64_t minCount = 0;

        std::map<std::string, std::vector<float>> pluginData = data.at("core");
        for (std::map<std::string, std::vector<float>>::iterator it = pluginData.begin(); it != pluginData.end(); ++it)
        {
            if (it->second.size() == 0)
                continue;

            auto [mi, mx] = std::minmax_element(it->second.begin(), it->second.end());
            max += *mx;
            min += *mi;
            ++maxCount;
            ++minCount;

            for (std::vector<float>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
            {
                avg += *(it2);
                ++avgCount;
            }
        }

        pluginsTable.add(string_format(" %.5fms / %.5fms / %.5fms ", (min / minCount), (avg / avgCount), (max / maxCount)));
    }
    else
        pluginsTable.add(" 0.00000ms / 0.00000ms / 0.00000ms ");

    pluginsTable.endOfRow();

    for (auto plugin : g_pluginManager.GetPluginsList())
    {
        std::string plugin_id = plugin->GetName();

        pluginsTable.add(" " + plugin_id + " ");
        pluginsTable.add(std::string(" ") + (plugin->GetPluginState() == PluginState_t::Started ? "Loaded" : "Unloaded") + " ");
        pluginsTable.add(std::string(" ") + (plugin->GetKind() == ContextKinds::Lua ? "Lua" : "JavaScript") + " ");
        if (plugin->GetPluginState() == PluginState_t::Started)
            pluginsTable.add(string_format(" %.4f MB ", (double(plugin->GetMemoryUsage()) / 1024.0f / 1024.0f)));
        else
            pluginsTable.add(" - ");

        if (plugin->GetPluginState() == PluginState_t::Started && data.find(plugin->GetName()) != data.end())
        {
            float max = 0;
            uint64_t maxCount = 0;
            float avg = 0;
            uint64_t avgCount = 0;
            float min = 0;
            uint64_t minCount = 0;

            std::map<std::string, std::vector<float>> pluginData = data.at(plugin->GetName());
            for (std::map<std::string, std::vector<float>>::iterator it = pluginData.begin(); it != pluginData.end(); ++it)
            {
                if (it->second.size() == 0)
                    continue;

                auto [mi, mx] = std::minmax_element(it->second.begin(), it->second.end());
                max += *mx;
                min += *mi;
                ++maxCount;
                ++minCount;

                for (std::vector<float>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
                {
                    avg += *(it2);
                    ++avgCount;
                }
            }

            pluginsTable.add(string_format(" %.5fms / %.5fms / %.5fms ", (min / minCount), (avg / avgCount), (max / maxCount)));
        }
        else
            pluginsTable.add(" 0.00000ms / 0.00000ms / 0.00000ms ");

        pluginsTable.endOfRow();
    }

    PrintTable(pluginsTable);
    PrintToClientOrConsole(slot, "Resource Monitor", "To view more detailed informations for each plugin, use: sw resmon viewplugin <ID>\n");
}

void SwiftlyResourceMonitorManager(CPlayerSlot slot, const char* subcmd, const char* subcmd2)
{
    if (slot.Get() != -1)
        return;

    std::string sbcmd = subcmd;
    if (sbcmd.size() == 0)
    {
        SwiftlyResourceMonitorManagerHelp(slot);
        return;
    }

    if (sbcmd == "enable")
        SwiftlyResourceMonitorManagerEnable(slot);
    else if (sbcmd == "disable")
        SwiftlyResourceMonitorManagerDisable(slot);
    else if (sbcmd == "view")
        SwiftlyResourceMonitorManagerView(slot);
    else if (sbcmd == "viewplugin")
        SwiftlyResourceMonitorManagerViewPlugin(slot, (subcmd2 == nullptr ? "" : subcmd2));
    else
        SwiftlyResourceMonitorManagerHelp(slot);
}