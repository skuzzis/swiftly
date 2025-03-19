#include "configuration.h"

#include <filesystem/files/files.h>
#include <core/entrypoint.h>
#include <utils/utils.h>
#include <utils/common.h>
#include <memory/encoders/json.h>
#include <sdk/game.h>
#include <cstddef>

bool followServerGuidelines = true;

bool ConfigurationError(std::string configuration_file, std::string error)
{
    if (!g_SMAPI)
        return false;

    PLUGIN_PRINTF("Configurations", "Error: %s: %s\n", configuration_file.c_str(), error.c_str());
    return false;
}

void WriteJSONFile(std::string path, nlohmann::json& j)
{
    Files::Write(path, j.dump(4), false);
}

nlohmann::json& GetJSONDoc(nlohmann::json& doc, std::string& key, nlohmann::json& value, bool& wasCreated)
{
    auto keys = explode(key, ".");
    nlohmann::json* currentDoc = &doc;

    while(keys.size()) {
        std::string currentKey = keys.front();
        keys.erase(keys.begin());

        if (!currentDoc->contains(currentKey) || !(*currentDoc)[currentKey].is_object()) {
            (*currentDoc)[currentKey] = nlohmann::json::object();
        }
        currentDoc = &((*currentDoc)[currentKey]);
    }

    std::string lastKey = keys.front();
    if (!currentDoc->contains(lastKey)) {
        (*currentDoc)[lastKey] = value;
        wasCreated = true;
    }
    return (*currentDoc)[lastKey];
}

template <class T>
void RegisterConfiguration(bool& wasCreated, nlohmann::json& document, std::string configFilePath, std::string config_prefix, std::string key, T default_value)
{
    nlohmann::json defaultValue = default_value;
    nlohmann::json& jsonDoc = GetJSONDoc(document, key, defaultValue, wasCreated);

    if constexpr (std::is_same<T, std::string>::value ||
                  std::is_same<T, const char*>::value)
    {
        if (!jsonDoc.is_string()) {
            jsonDoc = defaultValue;
            wasCreated = true;
        }
        g_Config.SetValue(config_prefix + "." + key, jsonDoc.get<std::string>());
    }
    else if constexpr (std::is_same<T, bool>::value)
    {
        if (!jsonDoc.is_boolean()) {
            jsonDoc = defaultValue;
            wasCreated = true;
        }
        g_Config.SetValue(config_prefix + "." + key, jsonDoc.get<bool>());
    }
    else if constexpr (std::is_same<T, uint64_t>::value)
    {
        if (!jsonDoc.is_number_unsigned()) {
            jsonDoc = defaultValue;
            wasCreated = true;
        }
        g_Config.SetValue(config_prefix + "." + key, jsonDoc.get<uint64_t>());
    }
    else if constexpr (std::is_same<T, uint32_t>::value ||
                       std::is_same<T, uint16_t>::value ||
                       std::is_same<T, uint8_t>::value)
    {
        if (!jsonDoc.is_number_unsigned()) {
            jsonDoc = defaultValue;
            wasCreated = true;
        }
        g_Config.SetValue(config_prefix + "." + key, jsonDoc.get<T>());
    }
    else if constexpr (std::is_same<T, int64_t>::value)
    {
        if (!jsonDoc.is_number_integer()) {
            jsonDoc = defaultValue;
            wasCreated = true;
        }
        g_Config.SetValue(config_prefix + "." + key, jsonDoc.get<int64_t>());
    }
    else if constexpr (std::is_same<T, int32_t>::value ||
                       std::is_same<T, int16_t>::value ||
                       std::is_same<T, int8_t>::value)
    {
        if (!jsonDoc.is_number_integer()) {
            jsonDoc = defaultValue;
            wasCreated = true;
        }
        g_Config.SetValue(config_prefix + "." + key, jsonDoc.get<T>());
    }
    else if constexpr (std::is_same<T, float>::value)
    {
        if (!jsonDoc.is_number_float()) {
            jsonDoc = defaultValue;
            wasCreated = true;
        }
        g_Config.SetValue(config_prefix + "." + key, jsonDoc.get<float>());
    }
    else if constexpr (std::is_same<T, double>::value)
    {
        if (!jsonDoc.is_number_float()) {
            jsonDoc = defaultValue;
            wasCreated = true;
        }
        g_Config.SetValue(config_prefix + "." + key, jsonDoc.get<double>());
    }
}

template <class T>
void RegisterConfigurationVector(bool& wasCreated, nlohmann::json& document, std::string configFilePath, std::string config_prefix, std::string key, std::vector<T> default_value, bool shouldImplode, std::string delimiter)
{
    nlohmann::json defaultValue = nlohmann::json::array();
    for (const T& val : default_value) {
        defaultValue.push_back(val);
    }
    
    nlohmann::json& jsonDoc = GetJSONDoc(document, key, defaultValue, wasCreated);
    
    if (!jsonDoc.is_array()) {
        jsonDoc = defaultValue;
        wasCreated = true;
    }
    
    std::vector<T> result;
    
    for (std::size_t i = 0; i < jsonDoc.size(); i++) {
        auto& element = jsonDoc[i];
        if constexpr (std::is_same<T, std::string>::value ||
                      std::is_same<T, const char*>::value)
        {
            if (!element.is_string()) {
                ConfigurationError(configFilePath + ".json",
                    string_format("The field \"%s[%zu]\" is not a string.\n", key.c_str(), i));
                continue;
            }
            result.push_back(element.get<std::string>());
        }
        else if constexpr (std::is_same<T, bool>::value)
        {
            if (!element.is_boolean()) {
                ConfigurationError(configFilePath + ".json",
                    string_format("The field \"%s[%zu]\" is not a boolean.\n", key.c_str(), i));
                continue;
            }
            result.push_back(element.get<bool>());
        }
        else if constexpr (std::is_same<T, uint64_t>::value)
        {
            if (!element.is_number_unsigned()) {
                ConfigurationError(configFilePath + ".json",
                    string_format("The field \"%s[%zu]\" is not an unsigned integer (64-bit).\n", key.c_str(), i));
                continue;
            }
            result.push_back(element.get<uint64_t>());
        }
        else if constexpr (std::is_same<T, uint32_t>::value)
        {
            if (!element.is_number_unsigned()) {
                ConfigurationError(configFilePath + ".json",
                    string_format("The field \"%s[%zu]\" is not an unsigned integer (32-bit).\n", key.c_str(), i));
                continue;
            }
            result.push_back(element.get<uint32_t>());
        }
        else if constexpr (std::is_same<T, uint16_t>::value)
        {
            if (!element.is_number_unsigned()) {
                ConfigurationError(configFilePath + ".json",
                    string_format("The field \"%s[%zu]\" is not an unsigned integer (16-bit).\n", key.c_str(), i));
                continue;
            }
            result.push_back(static_cast<uint16_t>(element.get<uint32_t>()));
        }
        else if constexpr (std::is_same<T, uint8_t>::value)
        {
            if (!element.is_number_unsigned()) {
                ConfigurationError(configFilePath + ".json",
                    string_format("The field \"%s[%zu]\" is not an unsigned integer (8-bit).\n", key.c_str(), i));
                continue;
            }
            result.push_back(static_cast<uint8_t>(element.get<uint32_t>()));
        }
        else if constexpr (std::is_same<T, int64_t>::value)
        {
            if (!element.is_number_integer()) {
                ConfigurationError(configFilePath + ".json",
                    string_format("The field \"%s[%zu]\" is not an integer (64-bit).\n", key.c_str(), i));
                continue;
            }
            result.push_back(element.get<int64_t>());
        }
        else if constexpr (std::is_same<T, int32_t>::value)
        {
            if (!element.is_number_integer()) {
                ConfigurationError(configFilePath + ".json",
                    string_format("The field \"%s[%zu]\" is not an integer (32-bit).\n", key.c_str(), i));
                continue;
            }
            result.push_back(element.get<int32_t>());
        }
        else if constexpr (std::is_same<T, int16_t>::value)
        {
            if (!element.is_number_integer()) {
                ConfigurationError(configFilePath + ".json",
                    string_format("The field \"%s[%zu]\" is not an integer (16-bit).\n", key.c_str(), i));
                continue;
            }
            result.push_back(static_cast<int16_t>(element.get<int32_t>()));
        }
        else if constexpr (std::is_same<T, int8_t>::value)
        {
            if (!element.is_number_integer()) {
                ConfigurationError(configFilePath + ".json",
                    string_format("The field \"%s[%zu]\" is not an integer (8-bit).\n", key.c_str(), i));
                continue;
            }
            result.push_back(static_cast<int8_t>(element.get<int32_t>()));
        }
        else if constexpr (std::is_same<T, float>::value)
        {
            if (!element.is_number_float()) {
                ConfigurationError(configFilePath + ".json",
                    string_format("The field \"%s[%zu]\" is not a float.\n", key.c_str(), i));
                continue;
            }
            result.push_back(element.get<float>());
        }
        else if constexpr (std::is_same<T, double>::value)
        {
            if (!element.is_number_float()) {
                ConfigurationError(configFilePath + ".json",
                    string_format("The field \"%s[%zu]\" is not a double.\n", key.c_str(), i));
                continue;
            }
            result.push_back(element.get<double>());
        }
        else {
            ConfigurationError(configFilePath + ".json",
                string_format("Invalid data type: %s.\n", typeid(T).name()));
            return;
        }
    }
    
    if (shouldImplode) {
        std::vector<std::string> implodeArr;
        for (const T& val : result) {
            if constexpr (std::is_same<T, std::string>::value ||
                          std::is_same<T, const char*>::value)
                implodeArr.push_back(val);
            else
                implodeArr.push_back(std::to_string(val));
        }
        g_Config.SetValue(config_prefix + "." + key, implode(implodeArr, delimiter));
    }
    else {
        g_Config.SetValue(config_prefix + "." + key, result);
    }
}

bool Configuration::LoadConfiguration()
{
    auto coreConfigFile = encoders::json::FromString(Files::Read("addons/swiftly/configs/core.json"), "addons/swiftly/configs/core.json");
    if(!coreConfigFile.is_object() || coreConfigFile.empty()) {
        return ConfigurationError("core.json", "Core configuration file needs to be an object.");
    }

    bool wasEdited = false;

    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "logging.enabled", true);
    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "logging.mode", std::string("daily"));
    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "logging.save_core_messages", false);

    std::string loggingMode = coreConfigFile["logging"]["mode"].get<std::string>();
    if (loggingMode != "daily" && loggingMode != "map" && loggingMode != "permanent") {
        return ConfigurationError("core.json", "The field \"logging.mode\" needs to be: \"daily\" or \"map\".");
    }

    RegisterConfigurationVector<std::string>(wasEdited, coreConfigFile, "core", "core", "commandPrefixes", { "!" }, true, " ");
    RegisterConfigurationVector<std::string>(wasEdited, coreConfigFile, "core", "core", "commandSilentPrefixes", { "/" }, true, " ");
    RegisterConfigurationVector<std::string>(wasEdited, coreConfigFile, "core", "core", "patches_to_perform", {}, true, " ");

    if(GetGameName() == "cs2") RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "CS2ServerGuidelines", "https://blog.counter-strike.net/index.php/server_guidelines/");
    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", string_format("Follow%sServerGuidelines", str_toupper(GetGameName())), true);

    followServerGuidelines = FetchValue<bool>(string_format("core.Follow%sServerGuidelines", str_toupper(GetGameName())));

    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "console_filtering", true);

    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "language", "en");
    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "use_player_language", true);

    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "menu.navigation_prefix", "➤");

    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "menu.sound.use.name", "Vote.Cast.Yes");
    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "menu.sound.use.volume", 0.75);

    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "menu.sound.scroll.name", "UI.ContractType");
    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "menu.sound.scroll.volume", 0.75);

    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "menu.sound.exit.name", "Vote.Failed");
    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "menu.sound.exit.volume", 0.75);

    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "menu.buttons.use", "e");
    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "menu.buttons.scroll", "shift");
    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "menu.buttons.exit.option", false);
    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "menu.buttons.exit.button", "tab");

    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "menu.kind", "screen");
    RegisterConfigurationVector<std::string>(wasEdited, coreConfigFile, "core", "core", "menu.available_kinds", {"screen", "center"}, true, " ");

    RegisterConfigurationVector<std::string>(wasEdited, coreConfigFile, "core", "core", "menu.available_inputmodes", {"chat", "button"}, true, " ");
    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "menu.inputMode", "button");
    
    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "menu.kind_settings.center.itemsPerPage", 7);
    
    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "menu.kind_settings.screen.mode", "compatibility");

    RegisterConfiguration<float>(wasEdited, coreConfigFile, "core", "core", "menu.kind_settings.screen.modes.compatibility.x", 0.14);
    RegisterConfiguration<float>(wasEdited, coreConfigFile, "core", "core", "menu.kind_settings.screen.modes.compatibility.y", 0.68);
    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "menu.kind_settings.screen.modes.compatibility.fontSize", 35);
    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "menu.kind_settings.screen.modes.compatibility.font", "Sans Serif");

    RegisterConfiguration<float>(wasEdited, coreConfigFile, "core", "core", "menu.kind_settings.screen.modes.normal.x", 0.0);
    RegisterConfiguration<float>(wasEdited, coreConfigFile, "core", "core", "menu.kind_settings.screen.modes.normal.y", 0.68);
    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "menu.kind_settings.screen.modes.normal.fontSize", 35);
    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "menu.kind_settings.screen.modes.normal.font", "Sans Serif");
    
    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "menu.kind_settings.screen.drawBackground", true);
    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "menu.kind_settings.screen.itemsPerPage", 9);

    RegisterConfiguration<float>(wasEdited, coreConfigFile, "core", "core", "vgui.textBackground.paddingX", 0.1);
    RegisterConfiguration<float>(wasEdited, coreConfigFile, "core", "core", "vgui.textBackground.paddingY", 0.1);

    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "vgui.textBackground.textSize", 35);

    if(wasEdited) {
        WriteJSONFile("addons/swiftly/configs/core.json", coreConfigFile);
    }

    loaded = true;

    return true;
}

void Configuration::SetArraySize(std::string key, unsigned int size)
{
    configArraySizes.insert({key, size});
}

template <typename T>
void Configuration::SetValue(std::string key, T value)
{
    if (config.find(key) == config.end())
        config.insert({key, value});
    else
        config[key] = value;
}

template <typename T>
void Configuration::SetPluginValue(std::string key, T value)
{
    pluginConfig.insert_or_assign(key, value);
}

void Configuration::ClearPluginConfig()
{
    pluginConfig.clear();
}

bool Configuration::IsConfigurationLoaded()
{
    return loaded;
}

std::map<std::string, std::any>& Configuration::FetchPluginConfiguration() { 
    return pluginConfig; 
}

std::map<std::string, std::any>& Configuration::FetchConfiguration() { 
    return config; 
}

std::map<std::string, unsigned int> Configuration::FetchConfigArraySizes() { 
    return configArraySizes; 
}

bool Configuration::HasKey(std::string key) {
    return config.find(key) != config.end();
}

void LoadConfigPart(std::string& key, nlohmann::json& document);

void LoadValue(const char* key, const char* keyname, nlohmann::json& value, std::string separator = ".")
{
    std::string k = std::string(key) + separator + keyname;
    
    if (value.is_boolean())
    {
        g_Config.SetPluginValue(k, value.get<bool>());
    }
    else if (value.is_string())
    {
        g_Config.SetPluginValue(k, value.get<std::string>());
    }
    else if (value.is_number_float())
    {
        // nlohmann::json uses double for floating-point numbers.
        g_Config.SetPluginValue(k, value.get<double>());
    }
    else if (value.is_number_integer())
    {
        // For integers, you can use int64_t (or choose a different integer type as needed).
        g_Config.SetPluginValue(k, value.get<int64_t>());
    }
    else if (value.is_null())
    {
        g_Config.SetPluginValue(k, nullptr);
    }
    else if (value.is_object())
    {
        g_Config.SetPluginValue(k, string_format("JSON⇚%s⇛", encoders::json::ToString(value).c_str()));
        LoadConfigPart(k, value);
    }
    else if (value.is_array())
    {
        g_Config.SetPluginValue(k, string_format("JSON⇚%s⇛", encoders::json::ToString(value).c_str()));
        g_Config.SetArraySize(k, value.size());
        for (int i = 0; i < value.size(); i++)
        {
            g_Config.SetPluginValue(string_format("%s[%d]", k.c_str(), i).c_str(), nullptr);
            LoadValue(k.c_str(), string_format("[%d]", i).c_str(), value[i], "");
        }
    }
}

void LoadConfigPart(std::string& key, nlohmann::json& document)
{
    for (auto it = document.begin(); it != document.end(); ++it)
    {
        std::string keyname = it.key();
        LoadValue(key.c_str(), keyname.c_str(), it.value());
    }
}

void Configuration::LoadPluginConfigurations()
{
    std::vector<std::string> configFiles = Files::FetchFileNames("addons/swiftly/configs/plugins");
    for (const std::string configFilePath : configFiles)
    {
        std::string configFileName = explode(configFilePath, string_format("addons/swiftly/configs/plugins%s", WIN_LINUX("\\", "/")))[1];
        if (!ends_with(configFileName, ".json"))
            continue;

        configFileName = replace(configFileName, "\\", ".");
        configFileName = replace(configFileName, "/", ".");

        auto configurationFile = encoders::json::FromString(Files::Read(configFilePath), configFilePath);
        if (!configurationFile.is_object() || configurationFile.empty())
        {
            ConfigurationError(configFileName, "Configuration file needs to be an object.");
            continue;
        }

        std::string main_key = explode(configFileName, ".json")[0];
        auto& root = configurationFile;

        g_Config.SetPluginValue(main_key, encoders::json::ToString(root));
        LoadConfigPart(main_key, root);
    }
}

void Configuration::LoadPluginConfig(std::string key)
{
    std::string configFilePath = "addons/swiftly/configs/plugins" + std::string(WIN_LINUX("\\", "/")) + key + ".json";
    std::string configFileName = key + ".json";
    if (Files::Read(configFilePath).size() == 0)
        return;

    key = replace(key, "/", ".");
    key = replace(key, "\\", ".");

    std::vector<std::string> toRemoveKeys;
    for (std::map<std::string, std::any>::iterator it = this->config.begin(); it != this->config.end(); ++it)
        if (starts_with(it->first, key + "."))
            toRemoveKeys.push_back(it->first);
    toRemoveKeys.push_back(key);

    for (std::string k : toRemoveKeys)
        this->config.erase(k);

    auto configurationFile = encoders::json::FromString(Files::Read(configFilePath), configFilePath);
    if (!configurationFile.is_object() || configurationFile.empty())
    {
        ConfigurationError(configFileName, "Configuration file needs to be an object.");
        return;
    }

    configFileName = replace(configFileName, "/", ".");
    configFileName = replace(configFileName, "\\", ".");
    std::string main_key = explode(configFileName, ".json")[0];

    auto& root = configurationFile;

    g_Config.SetPluginValue(main_key, encoders::json::ToString(root));
    LoadConfigPart(main_key, root);
}