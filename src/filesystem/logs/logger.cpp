#include "logger.h"
#include <filesystem/files/files.h>

std::optional<Log> Logger::FetchLogger(std::string key)
{
    if (m_Loggers.find(key) == m_Loggers.end())
        return {};

    return m_Loggers[key];
}

void Logger::AddLogger(std::string key, bool plugin)
{
    if (g_Config.FetchValue<bool>("core.logging.enabled") == false)
        return;
    if (FetchLogger(key).has_value())
        return;

    if (!Files::ExistsPath("addons/swiftly/logs"))
        Files::CreateDirectory("addons/swiftly/logs");
    if (!Files::ExistsPath("addons/swiftly/logs/plugins"))
        Files::CreateDirectory("addons/swiftly/logs/plugins");

    m_Loggers.insert({key, Log((plugin ? "plugins/" : "") + key)});
}

void Logger::RemoveLogger(std::string key)
{
    if (g_Config.FetchValue<bool>("core.logging.enabled") == false)
        return;
    if (!FetchLogger(key).has_value())
        return;

    m_Loggers.erase(key);
}