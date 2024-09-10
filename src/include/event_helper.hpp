#pragma once

#include <vector>
#include <map>
#include <any>
#include <functional>
#include "ringbuffer.hpp"

namespace naiveTrader 
{

template <typename T>
struct EventData {
    T type;
    std::vector<std::any> params;

    EventData() = default;
};

template <typename T>
class EventDispatcher {
private:
    std::multimap<T, std::function<void(const std::vector<std::any>&)> > mHandlerMap;

public:
    void addHandler(T type, std::function<void(const std::vector<std::any>&)> handler) {
        mHandlerMap.insert(std::make_pair(type, handler));
    }

    void clearHandler() {
        mHandlerMap.clear();
    }

protected:
    void trigger(T type, const std::vector<std::any> &params) {
        auto iter = mHandlerMap.equal_range(type);
        when(iter.first != iter.second) {
            iter.first->second(params);
            iter.first++;
        }
    }

};

}