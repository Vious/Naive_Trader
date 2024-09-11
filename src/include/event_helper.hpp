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

template <typename T, size_t N>
class EventQueue : public EventDispatcher<T> {
private:
    jnk0le::Ringbuffer<EventData<T>, N> mEventQ;

    void triggerEvent(EventData<T> &data) {
        while(!mEventQ.insert(data)) {}
    }

    template<typename Arg, typename... Types>
    void triggerEvent(EventData<T> &data, const Arg &firstArg, const Types&... args) {
        data.params.emplace_back(firstArg);
        triggerEvent(data, args...);
    }

public:
    template<typename... Types>
    void triggerEvent(T type, const Types &... args) {
        EventData<T> data;
        data.type = type;
        triggerEvent(data, args...);
    }

    void process() {
        EventData<T> data;
        while(mEventQ.remove(data)) {
            this->trigger(data.type, data.params);
        }
    }

    bool isEmpty() const {
        return mEventQ.isEmpty();
    }

    bool isFull() const {
        return mEventQ.isFull();
    }

};


template<typename T>
class DirectEvent : public EventDispatcher<T> {
private:
    void triggerEvent(EventData<T> &data) {
        this->trigger(data.type, data.params);
    }

    template<typename Arg, typename... Types>
    void triggerEvent(EventData<T> &data, const Arg &firstArg, const Types&... args) {
        data.params.emplace_back(firstArg);
        triggerEvent(data, args...);
    }

public:
    template<typename... Types>
    void triggerEvent(T type, const Types&... args) {
        EventData<T> data;
        data.type = type;
        triggerEvent(data, args...);
    }

};

}