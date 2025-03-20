#include "access.h"
#include "game.h"

#include <filesystem/files/files.h>
#include <memory/encoders/json.h>

#include <nlohmann/json.hpp>

extern std::set<uint32_t> structCache;
void PopulateClassData(const char* className, uint32_t classOffset);

void SDKAccess::LoadSDKData()
{
    std::string game_name = GetGameName();
    if(game_name == "unknown") {
        PRINTF("Unknown game detected, not loading any SDK data.\n");
        return;
    }

    std::string gamedata_path = string_format("addons/swiftly/gamedata/%s/", game_name.c_str());
    auto j = encoders::json::FromString(Files::Read(gamedata_path + "sdk.json"), gamedata_path + "sdk.json");
    if(!j.is_object()) return;

    for(auto it = j.begin(); it != j.end(); ++it)
    {
        std::string className = it.key();
        uint32_t classOffset = hash_32_fnv1a_const(className.c_str());
        PopulateClassData(className.c_str(), classOffset);
        classes.insert(className);

        if (it.value().is_object()) {
            for (auto it2 = it.value().begin(); it2 != it.value().end(); ++it2)
            {
                std::string fieldName = it2.key();

                if (it2.value().is_object()) {
                    if (!it2.value().contains("field") || !it2.value().contains("type")) continue;
                    if (!it2.value()["field"].is_string() || !it2.value()["type"].is_number_unsigned()) continue;

                    uint64_t key = ((uint64_t) hash_32_fnv1a_const(className.c_str()) << 32 | hash_32_fnv1a_const(fieldName.c_str()));

                    fieldNames.insert({ key, it2.value()["field"].get<std::string>() });
                    fieldTypes.insert({ key, (SDKFieldType_t)it2.value()["type"].get<uint32_t>() });
                    if (it2.value().contains("size") && it2.value()["size"].is_number_unsigned()) fieldSizes.insert({ key, it2.value()["size"].get<uint32_t>() });
                    if (it2.value().contains("classname") && it2.value()["classname"].is_string()) fieldClass.insert({ key, it2.value()["classname"].get<std::string>() });
                }
            }
        }
    }

    PRINTF("Succesfully loaded %lld SDK fields.\n", fieldNames.size());

    j = encoders::json::FromString(Files::Read(gamedata_path + "sdk_types.json"), gamedata_path + "sdk_types.json");
    if(!j.is_object()) return;

    for (auto it = j.begin(); it != j.end(); ++it)
    {
        std::string typeName = it.key();
        if(sdktypes.find(typeName) == sdktypes.end()) sdktypes.insert({typeName, {}});

        if (it.value().is_object()) {
            for (auto it2 = it.value().begin(); it2 != it.value().end(); ++it2)
            {
                std::string fieldName = it2.key();
                int64_t value = it2.value().get<uint64_t>();
                sdktypes[typeName].insert({fieldName, value});
            }
        }
    }

    PRINTF("Succesfully loaded %lld SDK types.\n", this->sdktypes.size());
}

std::set<std::string> SDKAccess::GetClasses()
{
    return classes;
}

std::map<std::string, std::map<std::string, int64_t>> SDKAccess::GetSDKTypes()
{
    return sdktypes;
}

std::string SDKAccess::GetFieldName(uint64_t path)
{
    return fieldNames[path];
}

SDKFieldType_t SDKAccess::GetFieldType(uint64_t path)
{
    return fieldTypes[path];
}

std::string SDKAccess::GetFieldClass(uint64_t path)
{
    return fieldClass[path];
}

uint32_t SDKAccess::GetFieldSize(uint64_t path)
{
    return fieldSizes[path];
}

bool SDKAccess::ExistsField(uint64_t path)
{
    return (fieldNames.find(path) != fieldNames.end());
}