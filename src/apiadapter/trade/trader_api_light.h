#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <functional>
#include <cmath>
#include <numeric>
#include <thread>

#include "../../../api/CTP_V6.6.9_20220920/ThostFtdcTraderApi.h"
#include "../../utils/lockqueue.hpp"

namespace naiveTrader
{

class SimpleOrder{
private:
    double_t mPrice;
    uint32_t mAmount;
    /* simple use string to denote status */
    std::string mStatus;
public:
    SimpleOrder() : mPrice(0), mAmount(0) {}
    void setPrice(double_t price) {
        mPrice = price;
    }
    
    void setAmount(uint32_t amount) {
        mAmount = amount;
    }

    void setOrderStatus(std::string status) {
        mStatus = status;
    }

    double_t getPrice() const {
        return mPrice;
    }
};

enum class TradeTaskName {
    OnRspQryInstrument,
    OnRtnTrade,
    OnRtnOrder,
    OnRspOrderInsert,
    OnErrRtnOrderInsert,
    OnRspSettlementInfoConfirm,
    OnRspQryInvestorPosition
};

static std::vector<std::string> contracts;
static int reqID = 0;

inline std::vector<std::string> getContracts() {
    return contracts;
}

class TraderTask {
private:
    TradeTaskName mName;
    void *message;

public:
    void *getMessage() const {return message;}

    TradeTaskName getTaskName() const {return mName; }

    void setMessage(void* const message) {this->message = message; }
    void setName(const TradeTaskName name) {mName = name; }
};

/* singletion pattern */
class LightTraderAPI final : public CThostFtdcTraderSpi {
private:
    LightTraderAPI();

    static std::shared_ptr<LightTraderAPI> m_traderInstance;
    static std::mutex m_tradeMutex;
    CThostFtdcTraderApi *m_traderApi;
    
    std::shared_ptr<std::thread> m_workThread;
    LockQueue<std::shared_ptr<TraderTask>> msgQueue;
    std::mutex m_msgMutex;

    void processTaskThread();
    void processOrder(const std::shared_ptr<TraderTask>& orderInfo) const;
    
public:
    static std::shared_ptr<LightTraderAPI> getInstance();

    ~LightTraderAPI() = default;

    virtual void OnFrontConnected() noexcept override;

    virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField* pRspAuthenticateField, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept override;

    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept override;

    virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept override;

    virtual void OnRtnOrder(CThostFtdcOrderField* pOrder) noexcept override;

    virtual void OnRspOrderInsert(CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept override;

    virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo) noexcept override;

    virtual void OnRtnTrade(CThostFtdcTradeField* pTrade) noexcept override;

    void reqQryInstrument(const std::string& exchangeId, const std::string &instrumentId = "") const;

    virtual void OnRspQryInstrument(CThostFtdcInstrumentField* pInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept override;

    void sendFakOrder(std::string instrumentId, char side, double price, int size);

    void sendFokOrder(std::string instrumentId, char side, double price, int size);

    void start();


};


} // namespace naiveTrader
