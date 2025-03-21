#include "json.h"
#include <utils/common.h>
#include <utils/utils.h>

std::string encoders::json::ToString(rapidjson::Value& j)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    j.Accept(writer);
    return buffer.GetString();
}

rapidjson::Document encoders::json::FromString(std::string str, std::string path)
{
    rapidjson::Document doc;
    if (doc.Parse(str.c_str()).HasParseError()) {
        PLUGIN_PRINTF("json::FromString", string_format("An error as occured while trying to parse %s:\nError: %s.", path == "" ? "json string" : path, doc.GetParseError()));
        doc.SetObject();
    }
    return doc;
}