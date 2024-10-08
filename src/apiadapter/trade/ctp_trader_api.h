#pragma once

#include <queue>
#include <stdint.h>
#include <thread>
#include <condition_variable>

#include "../../include/trade_api.h"
#include "../../../api/CTP_V6.6.9_20220920/ThostFtdcTraderApi.h"
#include "../../utils/dll_helper.hpp"
#include "../../include/params.hpp"

namespace naiveTrader
{

class CTPTraderAPI : public AsyncActualTrader, public CThostFtdcTraderSpi {
    
    enum class ActionFlag {
        AF_CANCEL = '0',
        AF_MODIFY = '3'
    };

public:

    CTPTraderAPI(const std::shared_ptr<Str2StrMap> &excgIdMap, const Params &config) noexcept;

    virtual ~CTPTraderAPI() noexcept;

public:

    virtual bool login() noexcept override;

    virtual void logout() noexcept override;

    virtual bool isUsable() const noexcept override;

    virtual estid_t placeOrder(PosOffsetType offset, DirectionType direction, const ExCode &code, uint32_t count, double_t price, OrderType flag) noexcept override;

    virtual bool cancelOrder(estid_t estid) noexcept override;

    virtual uint32_t getTradingDay() const noexcept override;

    virtual std::shared_ptr<TraderData> getTraderData() noexcept override;

public:

    /// 当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
    virtual void OnFrontConnected() noexcept override;

    /// 当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
    ///@param nReason 错误原因
    ///         0x1001 网络读失败
    ///         0x1002 网络写失败
    ///         0x2001 接收心跳超时
    ///         0x2002 发送心跳失败
    ///         0x2003 收到错误报文
    virtual void OnFrontDisconnected(int nReason) noexcept override;

    /// 客户端认证响应
    virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField* pRspAuthenticateField, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept override;

    /// 登录请求响应
    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept override;

    /// 登出请求响应
    virtual void OnRspUserLogout(CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept override;

    /// 投资者结算结果确认响应
    virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept override;

    /// 报单录入请求响应
    virtual void OnRspOrderInsert(CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept override;

    /// 报单操作请求响应
    virtual void OnRspOrderAction(CThostFtdcInputOrderActionField* pInputOrderAction, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept override;

    /// 请求查询投资者持仓响应
    virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField* pInvestorPosition, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept override;

    /// 请求查询报单响应
    virtual void OnRspQryOrder(CThostFtdcOrderField* pOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept override;

    /// 错误应答
    virtual void OnRspError(CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept override;

    /// 报单通知
    virtual void OnRtnOrder(CThostFtdcOrderField* pOrder) noexcept override;

    /// 报单录入错误回报
    virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo) noexcept override;

    /// 报单操作错误回报
    virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField* pOrderAction, CThostFtdcRspInfoField* pRspInfo) noexcept override;

    /// 合约交易状态通知
    virtual void OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField* pInstrumentStatus) noexcept override;

private:
    
    bool doAuthentication() noexcept;

    bool doLogin() noexcept;

    bool doLogout() noexcept;

    bool queryPosition(bool isSync) noexcept;

    bool queryOrders(bool isSync) noexcept;

    void submitSettlement() noexcept;

private:
    
    int convertDirectionOffset(DirectionType dirType, PosOffsetType offsetType) noexcept {
        if (dirType == DirectionType::DT_LONG) {
            if(offsetType == PosOffsetType::POFFT_OPEN) {
                return THOST_FTDC_D_Buy;
            } else {
                return THOST_FTDC_D_Sell;
            }
        } else {
            if (offsetType == PosOffsetType::POFFT_OPEN) {
                return THOST_FTDC_D_Sell;
            } else {
                return THOST_FTDC_D_Buy;
            }
        }
    }

    DirectionType wrapDirectionOffset(TThostFtdcDirectionType dirType, TThostFtdcOffsetFlagType offsetType) noexcept {
        if (dirType == THOST_FTDC_D_Buy) {
            if (offsetType == THOST_FTDC_OF_Open) {
                return DirectionType::DT_LONG;
            } else {
                return DirectionType::DT_SHORT;
            }
        } else {
            if (offsetType == THOST_FTDC_OF_Open) {
                return DirectionType::DT_SHORT;
            } else {
                return DirectionType::DT_LONG;
            }
        }
    }

    int convertOffsetType(const ExCode &code, uint32_t volume, PosOffsetType offset, DirectionType direction) noexcept {
        if (offset == PosOffsetType::POFFT_OPEN) {
            return THOST_FTDC_OF_Open;
        } else if (offset == PosOffsetType::POFFT_CLOSE) {
            return THOST_FTDC_OF_CloseYesterday;
        }
        return THOST_FTDC_OF_CloseToday;
    }

    PosOffsetType wrapOffsetType(TThostFtdcOffsetFlagType offset) noexcept {
        if (offset == THOST_FTDC_OF_Open) {
            return PosOffsetType::POFFT_OPEN;
        } else if (offset == THOST_FTDC_OF_CloseToday) {
            return PosOffsetType::POFFT_CLSTD;
        } else {
            return PosOffsetType::POFFT_CLOSE;
        }
    }

    int convertActionFlag(ActionFlag flag) noexcept {
        if (flag == ActionFlag::AF_CANCEL) {
            return THOST_FTDC_AF_Delete;
        } else {
            return THOST_FTDC_AF_Modify;
        }
    }

    estid_t generateEstId(uint32_t frontId, uint32_t sessionId, uint32_t orderRef) noexcept {
        uint64_t part1 = ((uint64_t) sessionId) << 32;
        part1 &= 0xFFFFFFFF00000000LLU;
        uint64_t part2 = ((uint64_t) frontId) << 16;
        part2 &= 0x00000000FFFF0000LLU;
        uint64_t part3 = ((uint64_t) orderRef);
        part3 &= 0x000000000000FFFFLLU;
        return part1 + part2 + part3;
    }

    estid_t generateEstId() noexcept {
        mOrderRef.fetch_add(1);
        return generateEstId(mFrontId, mSessionId, mOrderRef);
    }

    void extractEstId(estid_t estid, uint32_t &frontId, uint32_t &sessionId, uint32_t &orderRef) noexcept {
        uint64_t v1 = estid & 0xFFFFFFFF00000000LLU;
        uint64_t v2 = estid & 0x00000000FFFF0000LLU;
        uint64_t v3 = estid & 0x000000000000FFFFLLU;

        sessionId = static_cast<uint32_t>(v1 >> 32);
        frontId = static_cast<uint32_t>(v2 >> 16);
        orderRef = static_cast<uint32_t>(v3);
    }

    uint32_t genReqId() noexcept {
        return mReqId.fetch_add(1);
    }

protected:
    
    CThostFtdcTraderApi *traderApi;
    std::atomic<uint32_t> mReqId;

    std::string     mFrontAddr;
    std::string     mBrokerId;
    std::string     mUserId;
    std::string     mPasswd;
    std::string     mAppid;
    std::string     mAuthcode;
    std::string     mProductInfo;
    
    uint32_t        mFrontId;
    uint32_t        mSessionId;
    std::atomic<uint32_t> mOrderRef;

    std::map<ExCode, PositionSeed> mPosInfo;
    EntrustMap mOrderInfo;

    bool mIsRunning;

    std::mutex mMutex;
    std::unique_lock<std::mutex> mProccessMtx;
    std::condition_variable mProccessSignal;
    std::atomic<bool> mIsInquery;
    std::atomic<bool> mIsSyncWait;
    bool mIsInited;
    bool mIsConnected;

    typedef CThostFtdcTraderApi* (*traderCreator)(const char*);
    traderCreator mCtpCreatore;
    DLLHandler mTraderHandler;
};



    
} // namespace naiveTrader

