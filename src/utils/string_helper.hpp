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

/* end function!!! */
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

template<typename T>
inline std::string extractToString(const T& value) {
    return toString(value);
}

template<typename A, typename... Types>
inline void extractToString(std::vector<std::string>& data, const A& firstArg, const Types&... args) {
    data.emplace_back(toString(firstArg));
    extractToString(data, args...);
}

template<typename... T>
inline std::string format(const std::string& s, const T &... values) {
    std::vector<std::string> args;
    extractToString(args, values...);
    std::string result = s;
    char open = '{';
    char close = '}';
    bool is_open = false;
    size_t index = 0;
    size_t index_length = 0;
    size_t start = 0;

    for (size_t i = 0; i < result.length(); ++i) {
        char c = result.at(i);

        if (c == '\\') {
            i += 1;
        }
        else if (c == open) {
            is_open = true;
            start = i;
        }
        else if (c == close) {
            size_t one = result.length();
            result.erase(start, i + 1 - start);

            if (index_length > 0) {
                result.insert(start, args.at(index));
            }

            size_t two = result.length();

            i -= one - two;
            is_open = false;
            index = 0;
            index_length = 0;
        }
        else if (is_open) {
            int n = c - '0';

            if (n < 0 || n > 9) {
                continue;
            }

            index = (index * 10) + n;
            ++index_length;
        }
    }

    return result;
}

template<typename T>
inline bool contains(const std::vector<T>& values, const T value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}

template<typename... T>
inline std::string join(const std::string& separator, const std::vector<T> &... values) {
    std::vector<std::string> args;
    extractToString(args, values...);
    std::string result;
    for (const auto& s : args) {
        result.append(s);
        result.append(separator);
    }

    return result;
}


    
} // namespace StringHelper
    
} // namespace naiveTrader

