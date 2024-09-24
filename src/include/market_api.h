#include "basic_declarations.h"
#include "define.hpp"
#include "event_helper.hpp"


namespace naiveTrader 
{

enum class MarketEventType : int {
    MET_Invalid = -1,
    MET_TickRecv = 1
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
    ActualMarket(const std::shared_ptr<std::unordered_map<std::string, std::string>> &aExcgIdMap) : excgIdMap(aExcgIdMap) {}

    std::shared_ptr<std::unordered_map<std::string, std::string>> excgIdMap;

};

class SyncActualMarket : public ActualMarket, public DirectEvent<MarketEventType> {
protected:
    SyncActualMarket(const std::shared_ptr<std::unordered_map<std::string, std::string>> &aExcgIdMap) : ActualMarket(aExcgIdMap) {}

    virtual void bindEvent(MarketEventType type, std::function<void(const std::vector<std::any>&)> handler) override {
        this->addHandler(type, handler);
    }

    virtual void clearEvent() override {
        this->clearHandler();
    }

};

class Async_ActualMarket : public ActualMarket, public EventSource<MarketEventType, 1024> {
protected:
    Async_ActualMarket(const std::shared_ptr<std::unordered_map<std::string, std::string>> &aExcgIdMap) : ActualMarket(aExcgIdMap) {}

    virtual void update() override {
        this->process();
    }

    virtual void bindEvent(MarketEventType type, std::function<void(const std::vector<std::any>&)> handler) override {
        this->addHandler(type, handler);
    }

    virtual void clearEvent() {
        this->clearHandler();
    }

};

class DummyMarket : public MarketAPI, public DirectEvent<MarketEventType> {
public:
    virtual ~DummyMarket() {}

    virtual void bindEvent(MarketEventType type, std::function<void(const std::vector<std::any>&)> handler) override {
        this->addHandler(type, handler);
    }

    virtual void clearEvent() {
        this->clearHandler();
    }

    virtual void play(uint32_t tradingDay, std::function<void(const TickData&)> publishCallback) = 0;

    virtual bool isFinished() const = 0;

};

}