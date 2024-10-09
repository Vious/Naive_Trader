#include "ctp_trader_api.h"
#include "../../utils/time_utils.hpp"
#include <filesystem>
#include <iostream>

namespace naiveTrader
{

CTPTraderAPI::CTPTraderAPI(const std::shared_ptr<Str2StrMap> &excgIdMap, const Params &config) noexcept
: AsyncActualTrader(excgIdMap), traderApi(nullptr), mTraderHandler(nullptr), mReqId(0), mFrontId(0), mSessionId(0), mOrderRef(0), mProccessMtx(mMutex), mIsRunning(false), mIsInited(false), mIsInquery(false), mIsSyncWait(false) {
    try {
        mFrontAddr = config.get<std::string>("front");
        mBrokerId = config.get<std::string>("broker");
        mUserId = config.get<std::string>("userid");
        mPasswd = config.get<std::string>("passwd");
        mAppid = config.get<std::string>("appid");
        mAuthcode = config.get<std::string>("authcode");
        mProductInfo = config.get<std::string>("product");
    } catch(...) {
        std::cout << "CTP API config error!!!" << std::endl;
        //LOG_ERROR("");
    }
    mTraderHandler = DLLHelper::loadLibrary("thosttraderapi_se");
    if (mTraderHandler) {
        const char* creatorName = "_ZN19CThostFtdcTraderApi19CreateFtdcTraderApiEPKc";
        mCtpCreatore = (traderCreator)DLLHelper::getSymbol(mTraderHandler, creatorName);
    } else {
        //LOG_ERROR("")
        std::cout << "CTP Trader API thosttraderapi_se load error";
    }
}

CTPTraderAPI::~CTPTraderAPI() noexcept {
	DLLHelper::freeLibrary(mTraderHandler);
	mTraderHandler = nullptr;
}

bool CTPTraderAPI::login() noexcept {
    mIsRunning = true;
    char pathBuffer[64];
    sprintf(pathBuffer, "trade_data/%s/%s/", mBrokerId.c_str(), mUserId.c_str());
    if (!std::filesystem::exists(pathBuffer)) {
        std::filesystem::create_directories(pathBuffer);
    }
    traderApi = mCtpCreatore(pathBuffer);
    traderApi->RegisterSpi(this);

    traderApi->SubscribePrivateTopic(THOST_TERT_QUICK);
    traderApi->SubscribePublicTopic(THOST_TERT_QUICK);

    traderApi->RegisterFront(const_cast<char*>(mFrontAddr.c_str()));
    traderApi->Init();
    // LOG_INFO
    mProccessSignal.wait(mProccessMtx);
    mIsInited = true;
    submitSettlement();
    // LOG_INFO
    return true;
}

bool CTPTraderAPI::doLogin() noexcept {
    if (traderApi == nullptr) {
        return false;
    }
    CThostFtdcReqUserLoginField req;
    std::memset(&req, 0, sizeof(req));
    std::strcpy(req.BrokerID, mBrokerId.c_str());
    std::strcpy(req.UserID, mUserId.c_str());
    std::strcpy(req.Password, mPasswd.c_str());
    std::strcpy(req.UserProductInfo, mProductInfo.c_str());
    int reqResult = traderApi->ReqUserLogin(&req, genReqId());
    if (reqResult != 0) {
        std::cout << "CTP Trader API do login failed!!!" << ", Error id: " << reqResult << std::endl;
        return false;
    }
    return true;
}

void CTPTraderAPI::logout() noexcept {
    mIsRunning = false;
    doLogout();

    mReqId = 0;
    mFrontId = 0;
    mSessionId = 0;
    mOrderRef.exchange(0);

    mPosInfo.clear();
    mOrderInfo.clear();

    mIsInquery.exchange(false);
    mIsInited = false;
    mIsConnected = false;
    mIsSyncWait.exchange(false);
    if (traderApi) {
        traderApi->RegisterSpi(nullptr);
        traderApi->Release();
        traderApi = nullptr;
    }
    //LOG_INFO("Logout")
}

bool CTPTraderAPI::doLogout() noexcept {
    if (traderApi == nullptr) {
        return false;
    }
    CThostFtdcUserLogoutField req;
    std::memset(&req, 0, sizeof(req));
    std::strcpy(req.BrokerID, mBrokerId.c_str());
    std::strcpy(req.UserID, mUserId.c_str());
    int reqRet = traderApi->ReqUserLogout(&req, genReqId());
    if (reqRet != 0) {
        std::cout << "CTP Trader API do logout failed!!!" << ", Error id: " << reqRet << std::endl;
        return false;
    }
    return true;
}

bool CTPTraderAPI::queryPosition(bool isSync) noexcept {
    if (traderApi == nullptr) {
        return false;
    }
    bool expected = false;
    if (!mIsInquery.compare_exchange_weak(expected, true)) {
        // LOG_ERROR("");
        std::cout << "CTP Trader isInquery not returned!!!" <<std::endl;
        return false;
    }
    CThostFtdcQryInvestorPositionField req;
    std::memset(&req, 0, sizeof(req));
    std::strcpy(req.BrokerID, mBrokerId.c_str());
    std::strcpy(req.InvestorID, mUserId.c_str());
    int reqRet = traderApi->ReqQryInvestorPosition(&req, genReqId());
    if (reqRet != 0) {
        // LOG_ERROR("");
        std::cout << "ReqQryInvestorPosition Failed!!!" << std::endl;
        while(!mIsInquery.exchange(false));
        return false;
    }
    if (isSync) {
        while(!mIsSyncWait.exchange(true));
        mProccessSignal.wait(mProccessMtx);
    }
    return true;
}

bool CTPTraderAPI::queryOrders(bool isSync) noexcept {
    if (traderApi == nullptr) {
        return false;
    }
    bool expected = false;
    if (!mIsInquery.compare_exchange_weak(expected, true)) {
        // LOG_ERROR("");
        std::cout << "CTP Trader isInquery not returned!!!" <<std::endl;
        return false;
    }
    CThostFtdcQryOrderField req;
    std::memset(&req, 0, sizeof(req));
    std::strcpy(req.BrokerID, mBrokerId.c_str());
    std::strcpy(req.InvestorID, mUserId.c_str());
    int reqRet = traderApi->ReqQryOrder(&req, genReqId());
    if (reqRet != 0) {
        // LOG_ERROR()
        std::cout << "ReqQryOrder Failed!!!" << std::endl;
        while(!mIsInquery.exchange(false));
        return false;
    }
    if (isSync) {
        while(!mIsSyncWait.exchange(true));
        mProccessSignal.wait(mProccessMtx);
    }
    return true;
}

void CTPTraderAPI::OnFrontConnected() noexcept {
    // LOG_INFO("Connected.");
    mIsConnected = true;
    if (mIsRunning) {
        doAuthentication();
    }
}

void CTPTraderAPI::OnFrontDisconnected(int nReason) noexcept {
    // LOG_INFO("DisConnected.");
    mIsConnected = false;
}

void CTPTraderAPI::OnRspAuthenticate(CThostFtdcRspAuthenticateField* pRspAuthenticateField, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept {
    if (pRspInfo && pRspInfo->ErrorID) {
        std::cout << "CTPTraderAPI OnRspAuthenticate Error : " << pRspInfo->ErrorID << ", Error Msg: " << pRspInfo->ErrorMsg << std::endl;
        return;
    }
    doLogin();
}

void CTPTraderAPI::OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept {
    if (pRspInfo && pRspInfo->ErrorID) {
        std::cout << "CTPTraderAPI OnRspUserLogin Error : " << pRspInfo->ErrorID << ", Error Msg: " << pRspInfo->ErrorMsg << std::endl;
        return;
    }
    if (pRspUserLogin) {
        // should LOG_INFO
        // 
        mFrontId = pRspUserLogin->FrontID;
        mSessionId = pRspUserLogin->SessionID;
        mOrderRef = std::atoi(pRspUserLogin->MaxOrderRef);
    }
    if (bIsLast && !mIsInited) {
        mProccessSignal.notify_all();
    }
}

void CTPTraderAPI::OnRspUserLogout(CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept {
    if (pRspInfo) {
        std::cout << "OnRspUserLogout : " << pRspInfo->ErrorID << " " << pRspInfo->ErrorMsg << std::endl;
    }
}

void CTPTraderAPI::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept {
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        std::cout << "CTPTraderAPI OnRspSettlementInfoConfirm Error : " << pRspInfo->ErrorID << ", Error Msg: " << pRspInfo->ErrorMsg << std::endl;
    }
    if(bIsLast) {
        mProccessSignal.notify_all();
    }
}

void CTPTraderAPI::OnRspOrderInsert(CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept {
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        std::cout << "CTPTraderAPI OnRspOrderInsert Error : " << pRspInfo->ErrorID << ", Error Msg: " << pRspInfo->ErrorMsg << std::endl;
    }
    if (pInputOrder && pRspInfo) {
        estid_t estid = generateEstId(mFrontId, mSessionId, std::strtol(pInputOrder->OrderRef, NULL, 10));
        this->triggerEvent(TraderEventType::TET_OrderError, ErrorType::ERR_PLACE_ORDER, estid, (uint8_t)pRspInfo->ErrorID);
    }
}

void CTPTraderAPI::OnRspOrderAction(CThostFtdcInputOrderActionField* pInputOrderAction, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept {
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        std::cout << "CTPTraderAPI OnRspOrderAction Error : " << pRspInfo->ErrorID << ", Error Msg: " << pRspInfo->ErrorMsg << std::endl;
    }
    if (pInputOrderAction && pRspInfo) {
        estid_t estid = generateEstId(pInputOrderAction->FrontID, pInputOrderAction->SessionID, std::strtol(pInputOrderAction->OrderRef, NULL, 10));
        this->triggerEvent(TraderEventType::TET_OrderError, ErrorType::ERR_CANCEL_ORDER, estid, (uint8_t)pRspInfo->ErrorID);
    }
}

void CTPTraderAPI::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField* pInvestorPosition, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept {
    if (mIsInquery) {
        while(!mIsInquery.exchange(false));
        mPosInfo.clear();
    }
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        std::cout << "CTPTraderAPI OnRspQryInvestorPosition Error : " << pRspInfo->ErrorID << ", Error Msg: " << pRspInfo->ErrorMsg << std::endl;
    }
    if(pInvestorPosition) {
        // LOG_INFO("OnRspQryInvestorPosition", pInvestorPosition->InstrumentID, pInvestorPosition->TodayPosition, pInvestorPosition->Position, pInvestorPosition->YdPosition);
        ExCode code(pInvestorPosition->InstrumentID, pInvestorPosition->ExchangeID);
        PositionSeed pos;
        auto iter = mPosInfo.find(code);
        if (iter != mPosInfo.end()) {
            pos = iter->second;
        }
        pos.id = code;
        if (pInvestorPosition->PosiDirection == THOST_FTDC_PD_Long) {
            if (pInvestorPosition->PositionDate == THOST_FTDC_PSD_Today) {
                if (code.isDistinct()) {
                    pos.todayLong += pInvestorPosition->TodayPosition;
                } else {
                    pos.todayLong += pInvestorPosition->Position;
                }
            } else {
                pos.historyLong += (pInvestorPosition->Position - pInvestorPosition->TodayPosition);
            }
        } else if (pInvestorPosition->PosiDirection == THOST_FTDC_PD_Short) {
            if (pInvestorPosition->PositionDate == THOST_FTDC_PSD_Today) {
                if (code.isDistinct()) {
                    pos.todayShort += pInvestorPosition->TodayPosition;
                }  else {
                    pos.todayShort += pInvestorPosition->Position;
                }
            } else {
                pos.historyShort += (pInvestorPosition->Position - pInvestorPosition->TodayPosition);
            }
        }
        mPosInfo[code] = pos;
    }
    if (bIsLast && mIsSyncWait) {
        while(!mIsSyncWait.exchange(false));
        mProccessSignal.notify_all();
    }
}

void CTPTraderAPI::OnRspQryOrder(CThostFtdcOrderField* pOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept {
    if (mIsInquery) {
        while(!mIsInquery.exchange(false));
        mOrderInfo.clear();
    }
    if (pOrder && pOrder->VolumeTotal > 0 && pOrder->OrderStatus != THOST_FTDC_OST_Canceled && pOrder->OrderStatus != THOST_FTDC_OST_AllTraded) {
        estid_t estid = generateEstId(pOrder->FrontID, pOrder->SessionID, std::strtol(pOrder->OrderRef, NULL, 10));
        auto order = mOrderInfo[estid];
        order.code = ExCode(pOrder->InstrumentID, pOrder->ExchangeID);
        order.creatTime = TimeUtils::makeDayTm(pOrder->InsertTime, 0U);
        order.estId = estid;
        order.direction = wrapDirectionOffset(pOrder->Direction, pOrder->CombOffsetFlag[0]);
        order.offset = wrapOffsetType(pOrder->CombOffsetFlag[0]);
        order.lastVolume = pOrder->VolumeTotal;
        order.totalVolume = pOrder->VolumeTotal + pOrder->VolumeTraded;
        order.price = pOrder->LimitPrice;
        mOrderInfo[estid] = order;
        // LOG_INFO("OnRspQryOrder", pOrder->InstrumentID, estid, pOrder->FrontID, pOrder->SessionID, pOrder->OrderRef, pOrder->LimitPrice);
    }
    if (bIsLast && mIsSyncWait) {
        while(!mIsSyncWait.exchange(false));
        mProccessSignal.notify_all();
    }
}

void CTPTraderAPI::OnRspError(CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept {
    if (!mIsInited) return;
    if (pRspInfo) {
        std::cout << "CTPTraderAPI OnRspError Error : " << pRspInfo->ErrorID << ", Error Msg: " << pRspInfo->ErrorMsg << std::endl;
        this->triggerEvent(TraderEventType::TET_OrderError, ErrorType::ERR_OTHER, INVALID_ESTID, (uint8_t) pRspInfo->ErrorID);
    }
}

void CTPTraderAPI::OnRtnOrder(CThostFtdcOrderField* pOrder) noexcept {
    if (pOrder == nullptr || !mIsInited) return;
    auto estid = generateEstId(pOrder->FrontID, pOrder->SessionID, std::strtol(pOrder->OrderRef, NULL, 10));
    auto code = ExCode(pOrder->InstrumentID, pOrder->ExchangeID);
    auto direction = wrapDirectionOffset(pOrder->Direction, pOrder->CombOffsetFlag[0]);
    auto offset = wrapOffsetType(pOrder->CombOffsetFlag[0]);
    auto isToday = (THOST_FTDC_OF_CloseToday == pOrder->CombOffsetFlag[0]);
    // LOG_INGO("OnRtnOrder", estid, pOrder->InstrumentID, pOrder->FrontID);
    if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled || pOrder->OrderStatus == THOST_FTDC_OST_AllTraded) {
        auto iter = mOrderInfo.find(estid);
        if (iter != mOrderInfo.end()) {
            auto order = iter->second;
            if (order.lastVolume > static_cast<uint32_t>(pOrder->VolumeTotal)) {
                uint32_t dealVolume = order.lastVolume - pOrder->VolumeTotal;
                order.lastVolume = pOrder->VolumeTotal;
                this->triggerEvent(TraderEventType::TET_OrderDeal, estid, dealVolume, (uint32_t)(pOrder->VolumeTotal));
            }
            if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled) {
                // LOG_INFO("OnRtnOrder", estid, code.getCode(), direction, offset);
                this->triggerEvent(TraderEventType::TET_OrderCancel, estid, code, offset, direction, pOrder->LimitPrice, (uint32_t)(pOrder->VolumeTraded + pOrder->VolumeTotal));
            }
            mOrderInfo.erase(iter);
        }
    } else {
        auto iter = mOrderInfo.find(estid);
        if (iter == mOrderInfo.end()) {
            OrderData entrust;
            entrust.code = code;
            entrust.creatTime = TimeUtils::makeDayTm(pOrder->InsertTime, 0U);
            entrust.estId = estid;
            entrust.direction = direction;
            entrust.lastVolume = pOrder->VolumeTotal;
            entrust.totalVolume = pOrder->VolumeTotal + pOrder->VolumeTraded;
            entrust.offset = offset;
            entrust.price = pOrder->LimitPrice;
            mOrderInfo.insert(std::make_pair(estid, entrust));
            this->triggerEvent(TraderEventType::TET_OrderPlace, entrust);
            if (pOrder->VolumeTraded > 0) {
                this->triggerEvent(TraderEventType::TET_OrderDeal, estid, (uint32_t) pOrder->VolumeTotal, (uint32_t)(pOrder->VolumeTotal));
            }
        } else {
            auto entrust = iter->second;
            if (entrust.lastVolume > static_cast<uint32_t>(pOrder->VolumeTotal)) {
                uint32_t dealVolume = entrust.lastVolume - pOrder->VolumeTotal;
                entrust.lastVolume = pOrder->VolumeTotal;
                this->triggerEvent(TraderEventType::TET_OrderDeal, estid, dealVolume, (uint32_t)(pOrder->VolumeTotal));
            } else {
                if (entrust.lastVolume < static_cast<uint32_t>(pOrder->VolumeTotal)) {
                    std::cout << "OnRtnOrder Error : " << std::endl;
                    // should call LOG_ERROR()
                }
            }
            mOrderInfo[estid] = entrust;
        }
    }
}

void CTPTraderAPI::OnErrRtnOrderInsert(CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo) noexcept {
    if (!mIsInited) return;
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        std::cout << "CTPTraderAPI OnErrRtnOrderInsert Error : " << pRspInfo->ErrorID << ", Error Msg: " << pRspInfo->ErrorMsg << std::endl;
    }
    if (pInputOrder && pRspInfo) {
        // LOG_ERROR("OnErrRtnOrderInsert", pInputOrder->InstrumentID, pInputOrder->VolumeTotalOriginal, pRspInfo->ErrorID);
        estid_t estid = generateEstId(mFrontId, mSessionId, std::strtol(pInputOrder->OrderRef, NULL, 10));
        auto iter = mOrderInfo.find(estid);
        if (iter != mOrderInfo.end()) {
            mOrderInfo.erase(iter);
        }
        this->triggerEvent(TraderEventType::TET_OrderError, ErrorType::ERR_PLACE_ORDER, estid, (uint8_t)pRspInfo->ErrorID);
    }
}

void CTPTraderAPI::OnErrRtnOrderAction(CThostFtdcOrderActionField* pOrderAction, CThostFtdcRspInfoField* pRspInfo) noexcept {
    if (!mIsInited) return;
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        std::cout << "CTPTraderAPI OnErrRtnOrderAction Error : " << pRspInfo->ErrorID << ", Error Msg: " << pRspInfo->ErrorMsg << std::endl;
        if (pOrderAction) {
            // LOG_ERROR("OnErrRtnOrderAction", pOrderAction->OrderRef, pOrderAction->RequestID, pOrderAction->SessionID, pOrderAction->FrontID);
            estid_t estid = generateEstId(pOrderAction->FrontID, pOrderAction->SessionID, std::strtol(pOrderAction->OrderRef, NULL, 10));
            this->triggerEvent(TraderEventType::TET_OrderError, ErrorType::ERR_CANCEL_ORDER, estid, (uint8_t)pRspInfo->ErrorID);
        }
    }
}

void CTPTraderAPI::OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField* pInstrumentStatus) noexcept {

}

bool CTPTraderAPI::doAuthentication() noexcept {
    if (traderApi == nullptr) return false;
    CThostFtdcReqAuthenticateField req;
    std::memset(&req, 0, sizeof(req));
    std::strcpy(req.BrokerID, mBrokerId.c_str());
    std::strcpy(req.UserID, mUserId.c_str());

    std::strcpy(req.AuthCode, mAuthcode.c_str());
    std::strcpy(req.AppID, mAppid.c_str());
    int reqRet = traderApi->ReqAuthenticate(&req, genReqId());
    if (reqRet != 0) {
        std::cout << "CTPTraderAPI doAuthentication Error : " << reqRet << std::endl;
        return false;
    }
    return true;
}

bool CTPTraderAPI::isUsable() const noexcept {
    if (traderApi == nullptr) {
        return false;
    } 
    if (!mIsInited) {
        return false;
    }
    return mIsInited;
}

estid_t CTPTraderAPI::placeOrder(PosOffsetType offset, DirectionType direction, const ExCode &code, uint32_t volume, double_t price, OrderType flag) noexcept {
    if (traderApi == nullptr) {
        return INVALID_ESTID;
    }
    estid_t estid = generateEstId();
    CThostFtdcInputOrderField req;
    std::memset(&req, 0, sizeof(req));
    std::strcpy(req.BrokerID, mBrokerId.c_str());
    std::strcpy(req.InvestorID, mUserId.c_str());
    std::strcpy(req.InstrumentID, code.getCode());
    std::strcpy(req.ExchangeID, code.getExcgId());

    uint32_t orderRef = 0, sessionId = 0, frontId = 0;
    extractEstId(estid, frontId, sessionId, orderRef);

    sprintf(req.OrderRef, "%u", orderRef);
    if (price != .0F) {
        req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
    } else {
        req.OrderPriceType = THOST_FTDC_OPT_BestPrice;
    }

    req.Direction = convertDirectionOffset(direction, offset);
    req.CombOffsetFlag[0] = convertOffsetType(code, volume, offset, direction);
    req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
    req.LimitPrice = price;
    req.VolumeTotalOriginal = volume;

    if (flag == OrderType::OT_NORMAL) {
        req.TimeCondition = THOST_FTDC_TC_GFD;
        req.VolumeCondition = THOST_FTDC_VC_AV;
        req.MinVolume = 1;
    } else if (flag == OrderType::OT_FAK) {
        req.TimeCondition = THOST_FTDC_TC_IOC;
        req.VolumeCondition = THOST_FTDC_VC_AV;
        req.MinVolume = 1;
    } else if (flag == OrderType::OT_FOK) {
        req.TimeCondition = THOST_FTDC_TC_IOC;
        req.VolumeCondition = THOST_FTDC_VC_CV;
        req.MinVolume = volume;
    }
    
    // Immediately order
    req.ContingentCondition = THOST_FTDC_CC_Immediately;
    // not force close
    req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
    // not auto suspend
    req.IsAutoSuspend = 0;
    // not user force close
    req.UserForceClose = 0;
    //
    int reqRet = traderApi->ReqOrderInsert(&req, genReqId());
    if (reqRet != 0) {
        std::cout << "CTPTraderAPI ReqOrderInsert FAILED : " << reqRet << std::endl;
        return INVALID_ESTID;
    }
    return estid;
}

bool CTPTraderAPI::cancelOrder(estid_t estid) noexcept {
    if (traderApi == nullptr) {
        std::cout << "CTPTraderAPI cancelOrder traderAPI nullptr : " << estid << std::endl;
        return false;
    }
    auto iter = mOrderInfo.find(estid);
    if (iter == mOrderInfo.end()) {
        std::cout << "CTPTraderAPI cancelOrder order invalid : " << estid << std::endl;
        return false;
    }
    auto &order = iter->second;
    uint32_t frontId = 0, sessionId = 0, orderRef = 0;
    extractEstId(estid, frontId, sessionId, orderRef);
    CThostFtdcInputOrderActionField req;

    std::memset(&req, 0, sizeof(req));
    std::strcpy(req.BrokerID, mBrokerId.c_str());
    std::strcpy(req.InvestorID, mUserId.c_str());
    std::strcpy(req.UserID, mUserId.c_str());

    req.FrontID = mFrontId;
    req.SessionID = sessionId;
    sprintf(req.OrderRef, "%u", orderRef);
    req.ActionFlag = convertActionFlag(ActionFlag::AF_CANCEL);
    std::strcpy(req.InstrumentID, order.code.getCode());

    std::strcpy(req.ExchangeID, order.code.getExcgId());
    int reqRet = traderApi->ReqOrderAction(&req, genReqId());
    if (reqRet != 0) {
        std::cout << "CTPTraderAPI cancelOrder request failed : " << reqRet << std::endl;
        return false;
    }
    return true;
}

uint32_t CTPTraderAPI::getTradingDay() const noexcept {
    if (traderApi) {
        return static_cast<uint32_t>(std::atoi(traderApi->GetTradingDay()));
    }
    return 0x0U;
}

std::shared_ptr<TraderData> CTPTraderAPI::getTraderData() noexcept {
    auto result = std::make_shared<TraderData>();
    if (!queryPosition(true)) {
        // LOG_ERROR("queryPosition Error");
        std::cout << "queryPosition Error!!!" << std::endl;
        return result;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    if (!queryOrders(true)) {
        // LOG_ERROR("queryOrders Error");
        std::cout << "queryOrders Error!!!" << std::endl;
        return result;
    }
    for (auto ele : mOrderInfo) {
        result->orders.emplace_back(ele.second);
    }
    for (auto ele : mPosInfo) {
        result->positions.emplace_back(ele.second);
    }
    return result;
}

void CTPTraderAPI::submitSettlement() noexcept {
    if (traderApi == nullptr) {
        return;
    }
    CThostFtdcSettlementInfoConfirmField req;
    std::memset(&req, 0, sizeof(req));
    std::strcpy(req.BrokerID, mBrokerId.c_str());
    std::strcpy(req.InvestorID, mUserId.c_str());

    int reqRet = traderApi->ReqSettlementInfoConfirm(&req, genReqId());
    if (reqRet != 0) {
        // LOG_ERROR("queryOrders Error");
        std::cout << "ReqSettlementInfoConfirm Error: " << reqRet << std::endl;
    }
    while(!mIsSyncWait.exchange(true));
    mProccessSignal.wait(mProccessMtx);
}

} // namespace naiveTrader
