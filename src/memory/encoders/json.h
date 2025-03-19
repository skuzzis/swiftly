#ifndef _encoders_json_h
#define _encoders_json_h

#include <nlohmann/json.hpp>

namespace encoders {
    namespace json {
        std::string ToString(nlohmann::json& j);
        nlohmann::json FromString(std::string str, std::string path = "");
    }
}

#endif