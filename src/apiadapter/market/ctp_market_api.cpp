#include "ctp_market_api.h"
#include "../../include/params.hpp"
#include "../../utils/time_utils.hpp"
#include "../../include/log_wrapper.hpp"
#include <filesystem>
#include <iostream>

namespace naiveTrader
{

CTPMarketAPI::CTPMarketAPI(const std::shared_ptr<Str2StrMap> &excgIdMap, const Params &config) noexcept : AsyncActualMarket(excgIdMap), mdApi(nullptr), reqId(0), processMutex(mMutex), m_isInited(false) {
    try {
        mFrontAddr = config.get<std::string>("front");
        mBrokerId = config.get<std::string>("broker");
        mUserId = config.get<std::string>("userid");
        mPassWord = config.get<std::string>("passwd");
    } catch(...) {
        // LOG_ERROR("CTP Market API config Error!!! ");
        std::cout << "CTP Market API config Error!!!" << std::endl;
    }
    m_marketHandler = DLLHelper::loadLibrary("thostmduserapi_se");
    if (m_marketHandler) {
        const char *creatorName = "_ZN15CThostFtdcMdApi15CreateFtdcMdApiEPKcbb";
        m_ctpCreator = (marketCreator)DLLHelper::getSymbol(m_marketHandler, creatorName);
    } else {
        std::cout << "CTP API trader thosttraderapi_se load Error!!! " << std::endl;
    }
}

CTPMarketAPI::~CTPMarketAPI() noexcept {
    DLLHelper::freeLibrary(m_marketHandler);
    m_marketHandler = nullptr;
}

bool CTPMarketAPI::login() noexcept {
    char pathBuffer[64] = {0};
    sprintf(pathBuffer, "market_data/%s/%s/", mBrokerId.c_str(), mUserId.c_str());
    if (!std::filesystem::exists(pathBuffer)) {
        std::filesystem::create_directories(pathBuffer);
    }
    mdApi = m_ctpCreator(pathBuffer, false, false);
    mdApi->RegisterSpi(this);
    mdApi->RegisterFront((char*) mFrontAddr.c_str());
    mdApi->Init();
    processSignal.wait(processMutex);
    m_isInited = true;
    return true;
}

void CTPMarketAPI::logout() noexcept {
    reqId = 0;
    excgIdMap->clear();
    if(mdApi) {
        mdApi->RegisterSpi(nullptr);
        mdApi->Release();
        mdApi = nullptr;
    }
    m_isInited = false;
}

void CTPMarketAPI::doUserLogin() noexcept {
    if (mdApi == nullptr) return;
    CThostFtdcReqUserLoginField req;
    std::memset(&req, 0, sizeof(req));
    std::strcpy(req.BrokerID, mBrokerId.c_str());
    std::strcpy(req.UserID, mUserId.c_str());
    std::strcpy(req.Password, mPassWord.c_str());
    int logResult = mdApi->ReqUserLogin(&req, ++reqId);
    if (logResult != 0) {
        std::cout << "Log error: " << logResult << std::endl;
    }
}

void CTPMarketAPI::doSubscribe() noexcept {
    char *idList[500];
    int num = 0;
    for (auto &iter : *excgIdMap) {
        idList[num] = const_cast<char*>(iter.first.c_str());
        num++;
        if (num == 500) {
            mdApi->SubscribeMarketData(idList, num);
            return;
        }
    }
    mdApi->SubscribeMarketData(idList, num);
}

void CTPMarketAPI::doUnSubscribe(const std::vector<ExCode> &codeList) noexcept {
    char *idList[500];
    int num = 0;
    size_t size = codeList.size();
    for (size_t i = 0; i < size; i++) {
        idList[num] = const_cast<char*>(codeList[i].getCode());
        num++;
        if (num == 500) {
            mdApi->UnSubscribeMarketData(idList, num);
            return;
        }
    }
    mdApi->UnSubscribeMarketData(idList, num);
}

void CTPMarketAPI::subscribe(const std::set<ExCode> &codes) noexcept {
    for (auto &iter : codes) {
        (*excgIdMap)[iter.getCode()] = iter.getExcgId();
    }
    doSubscribe();
}

void CTPMarketAPI::unsubscribe(const std::set<ExCode> &codes) noexcept {
    std::vector<ExCode> rmList;
    for (auto &iter: codes) {
        auto id = excgIdMap->find(iter.getCode());
        if (id != excgIdMap->end()) {
            rmList.emplace_back(iter);
            excgIdMap->erase(id);
        }
    }
    doUnSubscribe(rmList);
}

void CTPMarketAPI::OnFrontConnected() noexcept {
    // LOG_INFO("Connected: %s", mFrontAddr.c_str());
    std::cout << "Connected: " << mFrontAddr << std::endl;
    doUserLogin();
}

void CTPMarketAPI::OnFrontDisconnected(int nReason) noexcept {
    // LOG_INFO("DisConnected, Reason is: %d", nReason);
    std::cout << "DisConnected, Reason is : " << nReason << std::endl;
}

void CTPMarketAPI::OnRspError(CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept {
    if(pRspInfo) {
        std::cout << "ErrorId: " << pRspInfo->ErrorID << " , ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
    }
}

void CTPMarketAPI::OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept {
    if (pRspInfo) {
        std::cout << "Login Error : " << pRspInfo->ErrorID << " , ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
    }
    if (bIsLast) {
        std::cout << "User Login, trading day: " << pRspUserLogin->TradingDay << " , User ID: " << pRspUserLogin->UserID << std::endl;
        doSubscribe();
        if (!m_isInited) {
            processSignal.notify_all();
        }
    }
}

void CTPMarketAPI::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) noexcept {
    if (pRspInfo) {
        std::cout << "Logout: " << pRspInfo->ErrorID << " , Msg: " << pRspInfo->ErrorMsg << std::endl;
    }
}

void CTPMarketAPI::OnRspSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept {
    if (pRspInfo) {
        std::cout << "Subscribe Market Data : " << pRspInfo->ErrorID << " , Msg: " << pRspInfo->ErrorMsg << std::endl;
    }
}

void CTPMarketAPI::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)  noexcept {
    if (pRspInfo) {
        std::cout << "UnSubscribe Market Data : " << pRspInfo->ErrorID << " , Msg: " << pRspInfo->ErrorMsg << std::endl;
    }
}

void CTPMarketAPI::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData) noexcept {
    if (pDepthMarketData == nullptr) return;
    //LOG_DEBUG("MarketData =", pDepthMarketData->InstrumentID, pDepthMarketData->LastPrice, pDepthMarketData->UpdateTime, pDepthMarketData->UpdateMillisec);
    std::cout << "InstrumentID: " << pDepthMarketData->InstrumentID << " , LastPrice : " << pDepthMarketData->LastPrice << " , UpdateTime: " << pDepthMarketData->UpdateTime << " , " << pDepthMarketData->UpdateMillisec << std::endl;
    const char *excgId = pDepthMarketData->ExchangeID;
    auto excgIter = excgIdMap->find(pDepthMarketData->InstrumentID);
    if (excgIter != excgIdMap->end()) {
        excgId = excgIter->second.c_str();
    }
    // Profile/LOG instrumentID
    TickData tick(
        ExCode(pDepthMarketData->InstrumentID, excgId),
        TimeUtils::makeDayTm(pDepthMarketData->UpdateTime,static_cast<uint32_t>(pDepthMarketData->UpdateMillisec)),
        pDepthMarketData->LastPrice,
        pDepthMarketData->OpenPrice,
        pDepthMarketData->ClosePrice,
        pDepthMarketData->HighestPrice,
        pDepthMarketData->LowestPrice,
        pDepthMarketData->PreSettlementPrice,
        pDepthMarketData->OpenInterest,
        pDepthMarketData->Volume,
        std::atoi(pDepthMarketData->TradingDay),
        {
            std::make_pair(pDepthMarketData->BidPrice1, pDepthMarketData->BidVolume1),
            std::make_pair(pDepthMarketData->BidPrice2, pDepthMarketData->BidVolume2),
            std::make_pair(pDepthMarketData->BidPrice3, pDepthMarketData->BidVolume3),
            std::make_pair(pDepthMarketData->BidPrice4, pDepthMarketData->BidVolume4),
            std::make_pair(pDepthMarketData->BidPrice5, pDepthMarketData->BidVolume5),
        },
        {
            std::make_pair(pDepthMarketData->AskPrice1, pDepthMarketData->AskVolume1),
            std::make_pair(pDepthMarketData->AskPrice2, pDepthMarketData->AskVolume2),
            std::make_pair(pDepthMarketData->AskPrice3, pDepthMarketData->AskVolume3),
            std::make_pair(pDepthMarketData->AskPrice4, pDepthMarketData->AskVolume4),
            std::make_pair(pDepthMarketData->AskPrice5, pDepthMarketData->AskVolume5),
        }
    );

    this->triggerEvent(MarketEventType::MET_TickRecv, tick);
    // profile loging
}


} // namespace naiveTrader
