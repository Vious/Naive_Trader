#pragma once

#include "define.hpp"
#include "event_helper.hpp"

namespace naiveTrader
{

enum class TraderEventType : uint8_t {
    TET_Invalid,
    TET_OrderCancel,
    TET_OrderPlace,
    TET_OrderDeal,
    TET_OrderTrade,
    TET_OrderError
};

struct PositionSeed {
    ExCode id;
    uint32_t todayLong;
    uint32_t todayShort;

    uint32_t historyLong;
    uint32_t historyShort;

    PositionSeed(): todayLong(0U), todayShort(0U), historyLong(0U), historyShort(0U) {}
};

struct TraderData {
    std::vector<OrderData> orders;

    std::vector<PositionSeed> positions;
};


typedef std::map<ExCode, PositionData> PositionMap;
typedef std::map<estid_t, OrderData> EntrustMap;

struct AccountInfo {
    double numOfMoney;

    double frozenMoney;

    AccountInfo() : numOfMoney(.0F), frozenMoney(.0F) {}
};

class TradeAPI {
public:

    virtual ~TradeAPI() {}

    virtual bool isUsable() const = 0;

    virtual void update() = 0;

    /* place order */
    virtual estid_t placeOrder(PosOffsetType offset, DirectionType direction, const ExCode &code, uint32_t count, double_t price, OrderType flag) = 0 ;

    virtual bool cancelOrder(estid_t estid) = 0;

    virtual uint32_t getTradingDay() const = 0;

    virtual std::shared_ptr<TraderData> getTraderData() = 0;

    virtual void bindEvent(TraderEventType type, std::function<void(const std::vector<std::any>&)> handler) = 0;

    virtual void clearEvent() = 0;

};

class ActualTrader : public TradeAPI {
public:

    virtual ~ActualTrader() {}

    virtual bool login() = 0;

    virtual void logout() = 0;

    /* check done or not */
    virtual bool isIdle() const = 0;


protected:
    std::shared_ptr<Str2StrMap> excgIdMap;

    ActualTrader(const std::shared_ptr<Str2StrMap> &excgIdMap) : excgIdMap(excgIdMap) {}

    bool isSubscribed(const std::string &codeId) const {
        return excgIdMap && excgIdMap->find(codeId) != excgIdMap->end();
    }

};

class SyncActualTrader : public ActualTrader, public DirectEvent<TraderEventType> {
protected:
    SyncActualTrader(const std::shared_ptr<Str2StrMap> &excgIdMap) : ActualTrader(excgIdMap) {

    }

    virtual bool isIdle() const override {
        return true;
    }

    virtual void bindEvent(TraderEventType type, std::function<void(const std::vector<std::any>&)> handler) override {
        this->addHandler(type, handler);
    }

    virtual void clearEvent() override {
        this->clearHandler();
    }

};

class AsyncActualTrader : public ActualTrader, public EventSource<TraderEventType, 128> {
protected:
    AsyncActualTrader(const std::shared_ptr<Str2StrMap> &excgIdMap) : ActualTrader(excgIdMap) {}

    virtual void update() override {
        this->process();
    }

    virtual bool isIdle() const override {
        return this->isEmpty();
    }

    virtual void bindEvent(TraderEventType type, std::function<void(const std::vector<std::any>&)> handler) override {
        this->addHandler(type, handler);
    }

    virtual void clearEvent() override {
        this->clearHandler();
    }

};

class DummyTrader : public TradeAPI, public DirectEvent<TraderEventType> {
public:

    virtual ~DummyTrader() {}

public:
    
    virtual void pushTick(const TickData &tick) = 0;

    virtual void crossDay(uint32_t tradingDay) = 0;

    virtual const AccountInfo& getAccount() = 0;

    virtual void bindEvent(TraderEventType type, std::function<void(const std::vector<std::any>&)> handler) override {
        this->addHandler(type, handler);
    }

    virtual void clearEvent() override {
        this->clearHandler();
    }

};

} // namespace naiveTrader
