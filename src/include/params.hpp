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

    


};


} // namespace naiveTrader

