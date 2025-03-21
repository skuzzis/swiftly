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

void WriteJSONFile(std::string path, rapidjson::Value& j)
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    j.Accept(writer);
    std::string content = buffer.GetString();

    Files::Write(path, content, false);
}

rapidjson::Value& GetJSONDoc(rapidjson::Document& doc, std::string key, rapidjson::Value& defaultValue, bool& wasCreated)
{
    rapidjson::Value* currentDoc = &doc;
    auto keys = explode(key, ".");

    while (keys.size() > 1)
    {
        std::string key = keys[0];
        keys.erase(keys.begin());

        if (!currentDoc->HasMember(key.c_str()))
            currentDoc->AddMember(rapidjson::Value().SetString(key.c_str(), doc.GetAllocator()), rapidjson::Value(rapidjson::kObjectType), doc.GetAllocator());
        else if (!currentDoc->operator[](key.c_str()).IsObject())
            currentDoc->operator[](key.c_str()).SetObject();

        currentDoc = &currentDoc->operator[](key.c_str());
    }

    if (!currentDoc->HasMember(keys[0].c_str()))
    {
        currentDoc->AddMember(rapidjson::Value().SetString(keys[0].c_str(), doc.GetAllocator()), defaultValue, doc.GetAllocator());
        wasCreated = true;
    }

    return currentDoc->operator[](keys[0].c_str());
}

template <class T>
void RegisterConfiguration(bool& wasCreated, rapidjson::Document& document, std::string configFilePath, std::string config_prefix, std::string key, T default_value)
{
    rapidjson::Value defaultValue;

    if constexpr (std::is_same<T, std::string>::value)
        defaultValue.SetString(default_value.c_str(), document.GetAllocator());
    else if constexpr (std::is_same<T, const char*>::value)
        defaultValue.SetString(default_value, document.GetAllocator());
    else if constexpr (std::is_same<T, bool>::value)
        defaultValue.SetBool(default_value);
    else if constexpr (std::is_same<T, uint64_t>::value)
        defaultValue.SetUint64(default_value);
    else if constexpr (std::is_same<T, uint32_t>::value)
        defaultValue.SetUint(default_value);
    else if constexpr (std::is_same<T, uint16_t>::value)
        defaultValue.SetUint(default_value);
    else if constexpr (std::is_same<T, uint8_t>::value)
        defaultValue.SetUint(default_value);
    else if constexpr (std::is_same<T, int64_t>::value)
        defaultValue.SetInt64(default_value);
    else if constexpr (std::is_same<T, int32_t>::value)
        defaultValue.SetInt(default_value);
    else if constexpr (std::is_same<T, int16_t>::value)
        defaultValue.SetInt(default_value);
    else if constexpr (std::is_same<T, int8_t>::value)
        defaultValue.SetInt(default_value);
    else if constexpr (std::is_same<T, float>::value)
        defaultValue.SetFloat(default_value);
    else if constexpr (std::is_same<T, double>::value)
        defaultValue.SetDouble(default_value);

    rapidjson::Value& jsonDoc = GetJSONDoc(document, key, defaultValue, wasCreated);

    if constexpr (std::is_same<T, std::string>::value)
    {
        if (!jsonDoc.IsString())
        {
            jsonDoc = defaultValue;
            wasCreated = true;
        }

        g_Config.SetValue(config_prefix + "." + key, std::string(jsonDoc.GetString()));
    }
    else if constexpr (std::is_same<T, const char*>::value)
    {
        if (!jsonDoc.IsString())
        {
            jsonDoc = defaultValue;
            wasCreated = true;
        }

        g_Config.SetValue(config_prefix + "." + key, std::string(jsonDoc.GetString()));
    }
    else if constexpr (std::is_same<T, bool>::value)
    {
        if (!jsonDoc.IsBool())
        {
            jsonDoc = defaultValue;
            wasCreated = true;
        }

        g_Config.SetValue(config_prefix + "." + key, jsonDoc.GetBool());
    }
    else if constexpr (std::is_same<T, uint64_t>::value)
    {
        if (!jsonDoc.IsUint64())
        {
            jsonDoc = defaultValue;
            wasCreated = true;
        }
        g_Config.SetValue(config_prefix + "." + key, jsonDoc.GetUint64());
    }
    else if constexpr (std::is_same<T, uint32_t>::value)
    {
        if (!jsonDoc.IsUint())
        {
            jsonDoc = defaultValue;
            wasCreated = true;
        }

        g_Config.SetValue(config_prefix + "." + key, jsonDoc.GetUint());
    }
    else if constexpr (std::is_same<T, uint16_t>::value)
    {
        if (!jsonDoc.IsUint())
        {
            jsonDoc = defaultValue;
            wasCreated = true;
        }

        g_Config.SetValue(config_prefix + "." + key, jsonDoc.GetUint());
    }
    else if constexpr (std::is_same<T, uint8_t>::value)
    {
        if (!jsonDoc.IsUint())
        {
            jsonDoc = defaultValue;
            wasCreated = true;
        }

        g_Config.SetValue(config_prefix + "." + key, jsonDoc.GetUint());
    }
    else if constexpr (std::is_same<T, int64_t>::value)
    {
        if (!jsonDoc.IsInt64())
        {
            jsonDoc = defaultValue;
            wasCreated = true;
        }

        g_Config.SetValue(config_prefix + "." + key, jsonDoc.GetInt64());
    }
    else if constexpr (std::is_same<T, int32_t>::value)
    {
        if (!jsonDoc.IsInt())
        {
            jsonDoc = defaultValue;
            wasCreated = true;
        }

        g_Config.SetValue(config_prefix + "." + key, jsonDoc.GetInt());
    }
    else if constexpr (std::is_same<T, int16_t>::value)
    {
        if (!jsonDoc.IsInt())
        {
            jsonDoc = defaultValue;
            wasCreated = true;
        }

        g_Config.SetValue(config_prefix + "." + key, jsonDoc.GetInt());
    }
    else if constexpr (std::is_same<T, int8_t>::value)
    {
        if (!jsonDoc.IsInt())
        {
            jsonDoc = defaultValue;
            wasCreated = true;
        }

        g_Config.SetValue(config_prefix + "." + key, jsonDoc.GetInt());
    }
    else if constexpr (std::is_same<T, float>::value)
    {
        if (!jsonDoc.IsFloat())
        {
            jsonDoc = defaultValue;
            wasCreated = true;
        }

        g_Config.SetValue(config_prefix + "." + key, jsonDoc.GetFloat());
    }
    else if constexpr (std::is_same<T, double>::value)
    {
        if (!jsonDoc.IsDouble())
        {
            jsonDoc = defaultValue;
            wasCreated = true;
        }

        g_Config.SetValue(config_prefix + "." + key, jsonDoc.GetDouble());
    }
}

template <class T>
void RegisterConfigurationVector(bool& wasCreated, rapidjson::Document& document, std::string configFilePath, std::string config_prefix, std::string key, std::vector<T> default_value, bool shouldImplode, std::string delimiter)
{
    rapidjson::Value defaultValue(rapidjson::kArrayType);

    for (const T& val : default_value)
    {
        rapidjson::Value defVal;

        if constexpr (std::is_same<T, std::string>::value)
            defVal.SetString(val.c_str(), document.GetAllocator());
        else if constexpr (std::is_same<T, const char*>::value)
            defVal.SetString(val, document.GetAllocator());
        else if constexpr (std::is_same<T, bool>::value)
            defVal.SetBool(val);
        else if constexpr (std::is_same<T, uint64_t>::value)
            defVal.SetUint64(val);
        else if constexpr (std::is_same<T, uint32_t>::value)
            defVal.SetUint(val);
        else if constexpr (std::is_same<T, uint16_t>::value)
            defVal.SetUint(val);
        else if constexpr (std::is_same<T, uint8_t>::value)
            defVal.SetUint(val);
        else if constexpr (std::is_same<T, int64_t>::value)
            defVal.SetInt64(val);
        else if constexpr (std::is_same<T, int32_t>::value)
            defVal.SetInt(val);
        else if constexpr (std::is_same<T, int16_t>::value)
            defVal.SetInt(val);
        else if constexpr (std::is_same<T, int8_t>::value)
            defVal.SetInt(val);
        else if constexpr (std::is_same<T, float>::value)
            defVal.SetFloat(val);
        else if constexpr (std::is_same<T, double>::value)
            defVal.SetDouble(val);

        defaultValue.PushBack(defVal, document.GetAllocator());
    }

    rapidjson::Value& jsonDoc = GetJSONDoc(document, key, defaultValue, wasCreated);

    if (!jsonDoc.IsArray())
    {
        jsonDoc = defaultValue;
        wasCreated = true;
    }

    std::vector<T> result;

    auto arr = jsonDoc.GetArray();

    for (rapidjson::SizeType i = 0; i < arr.Size(); i++)
    {
        if constexpr (std::is_same<T, std::string>::value)
        {
            if (!arr[i].IsString())
            {
                ConfigurationError(configFilePath + ".json", string_format("The field \"%s[%d]\" is not a string.\n", key.c_str(), i));
                continue;
            }

            result.push_back(std::string(arr[i].GetString()));
        }
        else if constexpr (std::is_same<T, const char*>::value)
        {
            if (!arr[i].IsString())
            {
                ConfigurationError(configFilePath + ".json", string_format("The field \"%s[%d]\" is not a string.\n", key.c_str(), i));
                continue;
            }

            result.push_back(arr[i].GetString());
        }
        else if constexpr (std::is_same<T, bool>::value)
        {
            if (!arr[i].IsBool())
            {
                ConfigurationError(configFilePath + ".json", string_format("The field \"%s[%d]\" is not a boolean.\n", key.c_str(), i));
                continue;
            }
            result.push_back(arr[i].GetBool());
        }
        else if constexpr (std::is_same<T, uint64_t>::value)
        {
            if (!arr[i].IsInt64())
            {
                ConfigurationError(configFilePath + ".json", string_format("The field \"%s[%d]\" is not an unsigned integer (64-bit).\n", key.c_str(), i));
                continue;
            }
            result.push_back(arr[i].GetUint64());
        }
        else if constexpr (std::is_same<T, uint32_t>::value)
        {
            if (!arr[i].IsInt())
            {
                ConfigurationError(configFilePath + ".json", string_format("The field \"%s[%d]\" is not an unsigned integer (32-bit).\n", key.c_str(), i));
                continue;
            }
            result.push_back(arr[i].GetUint());
        }
        else if constexpr (std::is_same<T, uint16_t>::value)
        {
            if (!arr[i].IsInt())
            {
                ConfigurationError(configFilePath + ".json", string_format("The field \"%s[%d]\" is not an unsigned integer (16-bit).\n", key.c_str(), i));
                continue;
            }
            result.push_back((uint16_t)arr[i].GetUint());
        }
        else if constexpr (std::is_same<T, uint8_t>::value)
        {
            if (!arr[i].IsInt())
            {
                ConfigurationError(configFilePath + ".json", string_format("The field \"%s[%d]\" is not an unsigned integer (8-bit).\n", key.c_str(), i));
                continue;
            }
            result.push_back((uint8_t)arr[i].GetUint());
        }
        else if constexpr (std::is_same<T, int64_t>::value)
        {
            if (!arr[i].IsInt64())
            {
                ConfigurationError(configFilePath + ".json", string_format("The field \"%s[%d]\" is not an integer (64-bit).\n", key.c_str(), i));
                continue;
            }
            result.push_back(arr[i].GetInt64());
        }
        else if constexpr (std::is_same<T, int32_t>::value)
        {
            if (!arr[i].IsInt())
            {
                ConfigurationError(configFilePath + ".json", string_format("The field \"%s[%d]\" is not an integer (32-bit).\n", key.c_str(), i));
                continue;
            }
            result.push_back(arr[i].GetInt());
        }
        else if constexpr (std::is_same<T, int16_t>::value)
        {
            if (!arr[i].IsInt())
            {
                ConfigurationError(configFilePath + ".json", string_format("The field \"%s[%d]\" is not an integer (16-bit).\n", key.c_str(), i));
                continue;
            }
            result.push_back((int16_t)arr[i].GetInt());
        }
        else if constexpr (std::is_same<T, int8_t>::value)
        {
            if (!arr[i].IsInt())
            {
                ConfigurationError(configFilePath + ".json", string_format("The field \"%s[%d]\" is not an integer (8-bit).\n", key.c_str(), i));
                continue;
            }
            result.push_back((int8_t)arr[i].GetInt());
        }
        else if constexpr (std::is_same<T, float>::value)
        {
            if (!arr[i].IsFloat())
            {
                ConfigurationError(configFilePath + ".json", string_format("The field \"%s[%d]\" is not a float.\n", key.c_str(), i));
                continue;
            }
            result.push_back(arr[i].GetFloat());
        }
        else if constexpr (std::is_same<T, double>::value)
        {
            if (!arr[i].IsFloat())
            {
                ConfigurationError(configFilePath + ".json", string_format("The field \"%s[%d]\" is not a double.\n", key.c_str(), i));
                continue;
            }
            result.push_back(arr[i].GetDouble());
        }
        else
        {
            ConfigurationError(configFilePath + ".json", string_format("Invalid data type: %s.\n", typeid(T).name()));
            return;
        }
    }

    if (shouldImplode)
    {
        std::vector<std::string> implodeArr;
        for (const T& val : result)
        {
            if constexpr (std::is_same<T, std::string>::value)
                implodeArr.push_back(val);
            else if constexpr (std::is_same<T, const char*>::value)
                implodeArr.push_back(std::string(val));
            else
                implodeArr.push_back(std::to_string(val));
        }

        g_Config.SetValue(config_prefix + "." + key, implode(implodeArr, delimiter));
    }
    else
        g_Config.SetValue(config_prefix + "." + key, result);
}

bool Configuration::LoadConfiguration()
{
    auto coreConfigFile = encoders::json::FromString(Files::Read("addons/swiftly/configs/core.json"), "addons/swiftly/configs/core.json");
    if(!coreConfigFile.IsObject()) {
        return ConfigurationError("core.json", "Core configuration file needs to be an object.");
    }

    bool wasEdited = false;

    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "logging.enabled", true);
    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "logging.mode", std::string("daily"));
    RegisterConfiguration(wasEdited, coreConfigFile, "core", "core", "logging.save_core_messages", false);

    std::string loggingMode = coreConfigFile["logging"]["mode"].GetString();
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

void LoadConfigPart(std::string key, rapidjson::Value& document);

void LoadValue(const char* key, const char* keyname, rapidjson::Value& value, std::string separator = ".")
{
    std::string k = key + separator + keyname;
    if (value.IsBool())
        g_Config.SetPluginValue(k, value.GetBool());
    else if (value.IsString())
        g_Config.SetPluginValue(k, std::string(value.GetString()));
    else if (value.IsDouble())
        g_Config.SetPluginValue(k, value.GetDouble());
    else if (value.IsFloat())
        g_Config.SetPluginValue(k, value.GetFloat());
    else if (value.IsInt64())
        g_Config.SetPluginValue(k, value.GetInt64());
    else if (value.IsInt())
        g_Config.SetPluginValue(k, value.GetInt());
    else if (value.IsUint64())
        g_Config.SetPluginValue(k, value.GetUint64());
    else if (value.IsUint())
        g_Config.SetPluginValue(k, value.GetUint());
    else if (value.IsNull())
        g_Config.SetPluginValue(k, nullptr);
    else if (value.IsObject()) {
        g_Config.SetPluginValue(k, string_format("JSON⇚%s⇛", encoders::json::ToString(value).c_str()));
        LoadConfigPart(k, value);
    }
    else if (value.IsArray())
    {
        g_Config.SetPluginValue(k, string_format("JSON⇚%s⇛", encoders::json::ToString(value).c_str()));
        g_Config.SetArraySize(k, value.Size());
        for (size_t i = 0; i < value.Size(); i++)
        {
            g_Config.SetPluginValue(string_format("%s[%d]", k.c_str(), i).c_str(), nullptr);
            LoadValue(k.c_str(), string_format("[%d]", i).c_str(), value[i], "");
        }
    }
}

void LoadConfigPart(std::string key, rapidjson::Value& document)
{
    for (auto it = document.MemberBegin(); it != document.MemberEnd(); ++it)
    {
        std::string keyname = it->name.GetString();
        LoadValue(key.c_str(), keyname.c_str(), it->value);
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
        if (!configurationFile.IsObject())
        {
            ConfigurationError(configFileName, "Configuration file needs to be an object.");
            continue;
        }

        std::string main_key = explode(configFileName, ".json")[0];
        rapidjson::Value& root = configurationFile;

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
    if (!configurationFile.IsObject())
    {
        ConfigurationError(configFileName, "Configuration file needs to be an object.");
        return;
    }

    configFileName = replace(configFileName, "/", ".");
    configFileName = replace(configFileName, "\\", ".");
    std::string main_key = explode(configFileName, ".json")[0];

    rapidjson::Value& root = configurationFile;

    g_Config.SetPluginValue(main_key, encoders::json::ToString(root));
    LoadConfigPart(main_key, root);
}