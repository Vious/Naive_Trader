#pragma once

#include <string>
#include <vector>

namespace naiveTrader
{
namespace StringHelper
{

inline std::vector<std::string> split(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    size_t startPos = 0, endPos;
    std::string aToken;
    while((endPos = str.find(delimiter, startPos)) != std::string::npos) {
        aToken = str.substr(startPos, endPos - startPos);
        startPos = endPos + 1;
        tokens.emplace_back(aToken);
    }
    tokens.emplace_back(str.substr(startPos));
    return tokens;
}

inline std::vector<std::string> split(const std::string &str, std::string delimiter) {
    std::vector<std::string> tokens;
    size_t startPos = 0, endPos, delimLength = delimiter.length();
    std::string aToken;
    while((endPos = str.find(delimiter, startPos)) != std::string::npos) {
        aToken = str.substr(startPos, endPos - startPos);
        startPos = endPos + delimLength;
        tokens.emplace_back(aToken);
    }
    tokens.emplace_back(str.substr(startPos));
    return tokens;
}

inline std::string toString(const char *value) {
    return std::string(value);
}

inline std::string toString(const std::string &value) {
    return value;
}

/* only support primitives !!! */
template <typename T>
inline std::string toString(const T &value) {
    return std::to_string(value);
}

template <typename T>
inline std::string toString(const std::vector<T> &values) {
    std::string res;
    for (auto val : values) {
        res.append(val);
    }
    return res;
}

    
} // namespace StringHelper
    
} // namespace naiveTrader

