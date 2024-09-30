#pragma once

#include <mutex>
#include <condition_variable>

#include "../../api/CTP_V6.6.9_20220920/ThostFtdcMdApi.h"
#include "../../include/basic_declarations.h"
#include "../../include/market_api.h"
#include "../../include/event_helper.hpp"
#include "../../include/params.hpp"


namespace naiveTrader
{

class CTPMarketAPI: public AsyncActualMarket, public CThostFtdcMdSpi {
public:

    CTPMarketAPI(const std::shared_ptr<Str2StrMap> &excgIdMap, const Params &config) noexcept;

    virtual ~CTPMarketAPI() noexcept;


// market api interface
public:
    virtual bool login() noexcept override;

    virtual void logout() noexcept override;

    virtual void subscribe(const std::set<ExCode> &codes) noexcept override;

    virtual void unsubscribe(const std::set<ExCode> &codes) noexcept override;


//CThostFtdcMdSpi interface
public:
    /// Called when the client establishes a communication connection with the transaction backend (before logging in).
    virtual void OnFrontConnected() noexcept;

    /// Called when disconected with server
    /// @param nReason The error reason
    /// 0x1001 Network read failure
    /// 0x1002 Network write failure
    /// 0x2001 Heartbeat receive timeout
    /// 0x2002 Heartbeat send failure
    /// 0x2003 Received error message
    virtual void OnFrontDisconnected(int nReason) noexcept;

    /// login response
    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept;

    /// logout response
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) noexcept;

    /// error response
    virtual void OnRspError(CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept;

    /// subscribe market data response
    virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept;

    /// depth market data
    virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData) noexcept;

    /// unsubscribe response
    virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept;


private:
    /* login request */
    void doUserLogin() noexcept;

    /* subscribe */ 
    void doSubscribe() noexcept;

    /* unsubscribe */
    void doUnSubscribe(const std::vector<ExCode> &codeList) noexcept;


private:
    CThostFtdcMdApi *mdApi;

    std::string mFrontAddr;
    std::string mBrokerId;
    std::string mUserId;
    std::string mPassWord;

    int reqId;

    std::mutex mMutex;
    std::unique_lock<std::mutex> processMutex;
    std::condition_variable processSignal;

    bool isInit;

};

    
} // namespace naiveTrader
