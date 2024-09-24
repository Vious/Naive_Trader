#pragma once

#include "basic_declarations.h"
#include "define.hpp"
#include "event_helper.hpp"


namespace naiveTrader 
{

enum class MarketEventType {
    MET_Invalid,
    MET_TickRecv
};

/* Api for market  */
class MarketAPI {
public:
    /* subscribe contracts */
    virtual void subscribe(const std::set<ExCode> &codes) = 0;

    virtual void unsubscribe(const std::set<ExCode> &codes) = 0;

    virtual void update() = 0;

    /* event binding */
    virtual void bindEvent(MarketEventType type, std::function<void(const std::vector<std::any> &)> handler) = 0;

    /* clear event */
    virtual void clearEvent() = 0;

    virtual ~MarketAPI() {};
};

class ActualMarket : public MarketAPI {
public:

    virtual bool login() = 0;

    virtual bool logout() = 0;

    virtual ~ActualMarket() {}

protected:
    std::shared_ptr<std::unordered_map<std::string, std::string>> excgIdMap;

    ActualMarket(const std::shared_ptr<std::unordered_map<std::string, std::string>> &aExcgIdMap) : excgIdMap(aExcgIdMap) {}
};

class SyncActualMarket : public ActualMarket, public EventDispatcher<MarketEventType> {
protected:
    SyncActualMarket(const std::shared_ptr<std::unordered_map<std::string, std::string>> &aExcgIdMap) : ActualMarket(aExcgIdMap) {}

    virtual void bindEvent(MarketEventType type, std::function<void(const std::vector<std::any>&)> handler) override {
        this->addHandler(type, handler);
    }

    virtual void clearEvent() override {
        this->clearHandler();
    }

};

}