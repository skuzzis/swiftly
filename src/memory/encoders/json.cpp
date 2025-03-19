#include "json.h"
#include <utils/common.h>
#include <utils/utils.h>

std::string encoders::json::ToString(nlohmann::json& j)
{
    return j.dump();
}

nlohmann::json encoders::json::FromString(std::string str, std::string path)
{
    try {
        return nlohmann::json::parse(str);
    } catch(const nlohmann::json::parse_error& e) {
        PLUGIN_PRINTF("json::FromString", string_format("An error as occured while trying to parse %s:\nError: %s.", path == "" ? "json string" : path, e.what()));
        return nlohmann::json();
    } 
}