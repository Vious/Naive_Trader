#include "market_api_light.h"
#include <iostream>
#include <filesystem>
#include <cstring>
#include "../../config/config.h"
#include "../../include/instrument.hpp"

namespace naiveTrader
{

LightMarketAPI::LightMarketAPI() {
    // char pathBuffer[64] = {0};
    // sprintf(pathBuffer, "%s/market_data/%s/%s/", userconfig::TraderFlow.c_str(), userconfig::BrokerID.c_str(), userconfig::UserID.c_str());
    auto currentPath = std::filesystem::current_path();
    currentPath += userconfig::TraderFlow;
    // auto flowPath = std::filesystem::create_directories(currentPath);

    if (!std::filesystem::exists(currentPath)) {
        auto flowPath = std::filesystem::create_directories(currentPath);
    }
    m_marketApi = CThostFtdcMdApi::CreateFtdcMdApi(currentPath.string().c_str());
    m_marketApi->RegisterSpi(this);
    m_marketApi->RegisterFront(const_cast<char*>(userconfig::MarketFrontAddr.c_str()));
    m_marketApi->Init();
}

void LightMarketAPI::OnFrontConnected() noexcept {
    CThostFtdcReqUserLoginField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, userconfig::BrokerID.c_str());
    strcpy(req.UserID, userconfig::UserID.c_str());
    strcpy(req.Password, userconfig::Password.c_str());
    int reqRet = m_marketApi->ReqUserLogin(&req, reqID++);
    if (reqRet != 0) {
        std::cout << "OnFrontConnected ReqUserLogin Failed:  " << reqRet << std::endl;
    }
}

void LightMarketAPI::OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept {
    if (pRspInfo != nullptr && pRspInfo->ErrorID != 0) {
        std::cout << "OnRspUserLogin Error ID : " << pRspInfo->ErrorID << " , ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
    } else {
        std::cout << "User Login succeed:, user ID:  " << pRspUserLogin->UserID << ", Login time: " << pRspUserLogin->LoginTime << std::endl;
    }
}

void LightMarketAPI::OnRspSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept {

}

void LightMarketAPI::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData) noexcept {
    std::cout << pDepthMarketData->InstrumentID << " Bid: " << pDepthMarketData->BidPrice1
			<< " " << pDepthMarketData->BidVolume1 << " Ask: " << pDepthMarketData->AskPrice1
			<< " Bid: " << pDepthMarketData->AskVolume1 << std::endl;

    // do something like
    // instrument = std::make_shared<SimpleInstrumet>();
    // instrument->setExchangeId(pDepthMarketData->ExchangeID);
    // instrument->setInstrumentId(pDepthMarketData->InstrumentID);
    // instrument->setProductType(pDepthMarketData-)
}

void LightMarketAPI::subscribe(const std::string &instrumentID) noexcept {
    auto instrument = (char*)instrumentID.c_str();
    char *buffer[1] = {instrument};
    int reqRet = m_marketApi->SubscribeMarketData(buffer, 1);
    if (reqRet != 0 ) {
        std::cout << "subscribe response : " << reqRet << std::endl;
    }
}

} // namespace naiveTrader
