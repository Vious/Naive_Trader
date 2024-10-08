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



} // namespace naiveTrader
