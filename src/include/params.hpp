#pragma once

#include <regex>
#include <stdexcept>
#include "basic_declarations.h"
#include "../utils/string_helper.hpp"

namespace naiveTrader
{
 
using Str2StrMap = std::map<std::string, std::string>;

class Params {
private:
    Str2StrMap mParamsMap;

public:
    Params() = default;

    Params(Str2StrMap &params) : mParamsMap(params) {}

    void setData(Str2StrMap &params) {
        this->mParamsMap = params;
    }

    Str2StrMap getData() const {
        return mParamsMap;
    }

    Params(const std::string &param) {
        mParamsMap.clear();
        auto paramPair = StringHelper::split(param, '&');
        for (auto ele : paramPair) {
            auto aPair = StringHelper::split(ele, '=');
            mParamsMap[aPair[0]] = aPair[1];
        }
    }

    template<typename T>
    T get(const char key[]) const {
        return get<T>(std::string(key));
    }

private:
    /* char* Type */
    template <typename T>
    typename std::enable_if<std::is_same<T, const char*>::value, T>::type get(const std::string &key) const {
        auto iter = mParamsMap.find(key);
        if (iter == mParamsMap.end()) {
            throw std::invalid_argument("key not find: " + key);
        }
        return iter->second.c_str();
    }

    /* string type */
    template <typename T>
    typename std::enable_if<std::is_same<T, std::string>::value, T>::type get(const std::string &key) const {
        auto iter = mParamsMap.find(key);
        if (iter == mParamsMap.end()) {
            throw std::invalid_argument("key not find: " + key);
        }
        return iter->second;
    }

    /* create excode type */
    template <typename T>
    typename std::enable_if<std::is_same<T, ExCode>::value, T>::type get(const std::string &key) const {
        auto iter = mParamsMap.find(key);
        if (iter == mParamsMap.end()) {
            throw std::invalid_argument("key not find: " + key);
        }
        return ExCode(iter->second.c_str());
    }

    /* int8_t/uint8_t */
    template <typename T>
    typename std::enable_if<std::is_same<T, int8_t>::value, T>::type get(const std::string &key) const {
        auto iter = mParamsMap.find(key);
        if (iter == mParamsMap.end()) {
            throw std::invalid_argument("key not find: " + key);
        }
        return static_cast<int8_t>(std::atoi(iter->second.c_str() ));
    }

    template <typename T>
    typename std::enable_if<std::is_same<T, uint8_t>::value, T>::type get(const std::string &key) const {
        auto iter = mParamsMap.find(key);
        if (iter == mParamsMap.end()) {
            throw std::invalid_argument("key not find: " + key);
        }
        return static_cast<uint8_t>(std::atoi(iter->second.c_str() ));
    }

    /* int16_t/uint16_t */
    template <typename T>
    typename std::enable_if<std::is_same<T, int16_t>::value, T>::type get(const std::string &key) const {
        auto iter = mParamsMap.find(key);
        if (iter == mParamsMap.end()) {
            throw std::invalid_argument("key not find: " + key);
        }
        return static_cast<int16_t>(std::atoi(iter->second.c_str() ));
    }

    template <typename T>
    typename std::enable_if<std::is_same<T, uint16_t>::value, T>::type get(const std::string &key) const {
        auto iter = mParamsMap.find(key);
        if (iter == mParamsMap.end()) {
            throw std::invalid_argument("key not find: " + key);
        }
        return static_cast<uint16_t>(std::atoi(iter->second.c_str() ));
    }

    /* int32_t/uint32_t */
    template <typename T>
    typename std::enable_if<std::is_same<T, int32_t>::value, T>::type get(const std::string &key) const {
        auto iter = mParamsMap.find(key);
        if (iter == mParamsMap.end()) {
            throw std::invalid_argument("key not find: " + key);
        }
        return std::atoi(iter->second.c_str());
    }

    template <typename T>
    typename std::enable_if<std::is_same<T, uint32_t>::value, T>::type get(const std::string &key) const {
        auto iter = mParamsMap.find(key);
        if (iter == mParamsMap.end()) {
            throw std::invalid_argument("key not find: " + key);
        }
        return static_cast<uint32_t>(std::atoi(iter->second.c_str() ));
    }

    /* int64_t/uint64_t */
    template <typename T>
    typename std::enable_if<std::is_same<T, int64_t>::value, T>::type get(const std::string &key) const {
        auto iter = mParamsMap.find(key);
        if (iter == mParamsMap.end()) {
            throw std::invalid_argument("key not find: " + key);
        }
        return std::atoll(iter->second.c_str());
    }

    template <typename T>
    typename std::enable_if<std::is_same<T, uint64_t>::value, T>::type get(const std::string &key) const {
        auto iter = mParamsMap.find(key);
        if (iter == mParamsMap.end()) {
            throw std::invalid_argument("key not find: " + key);
        }
        return static_cast<uint64_t>(std::atoll(iter->second.c_str() ));
    }

    /* double_t */
    template <typename T>
    typename std::enable_if<std::is_same<T, double_t>::value, T>::type get(const std::string &key) const {
        auto iter = mParamsMap.find(key);
        if (iter == mParamsMap.end()) {
            throw std::invalid_argument("key not find: " + key);
        }
        return std::atof(iter->second.c_str());
    }

    /* bool */
    template <typename T>
    typename std::enable_if<std::is_same<T, bool>::value, T>::type get(const std::string &key) const {
        auto iter = mParamsMap.find(key);
        if (iter == mParamsMap.end()) {
            throw std::invalid_argument("key not find: " + key);
        }
        return iter->second == "True" || iter->second == "true" || iter->second == "TRUE" || std::atoll(iter->second.c_str()) > 0;
    }


};


} // namespace naiveTrader

