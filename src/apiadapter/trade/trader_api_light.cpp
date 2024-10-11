#include "trader_api_light.h"
#include "../../config/config.h"

#include <filesystem>
#include <iostream>

namespace naiveTrader
{


std::mutex LightTraderAPI::m_tradeMutex;
std::shared_ptr<LightTraderAPI> LightTraderAPI::m_traderInstance;

LightTraderAPI::LightTraderAPI() {
    auto currentPath = std::filesystem::current_path();
    currentPath += userconfig::TraderFlow;
    if (!std::filesystem::exists(currentPath)) {
        auto flowPath = std::filesystem::create_directories(currentPath);
    }
    m_traderApi = CThostFtdcTraderApi::CreateFtdcTraderApi(currentPath.string().c_str());
    m_traderApi->RegisterSpi(this);
    m_traderApi->RegisterFront(const_cast<char*>(userconfig::TradeFrontAddr.c_str()));
    m_traderApi->SubscribePrivateTopic(THOST_TERT_RESTART);
    m_traderApi->SubscribePublicTopic(THOST_TERT_RESTART);
    m_traderApi->Init();
}

std::shared_ptr<LightTraderAPI> LightTraderAPI::getInstance() {
    std::lock_guard<std::mutex> lock(m_tradeMutex);
    if (!m_traderInstance) {
        m_traderInstance.reset(new LightTraderAPI());
    }
    return m_traderInstance;
}

void LightTraderAPI::start() {
    std::lock_guard<std::mutex> lock(m_msgMutex);
    if (!m_workThread) {
        m_workThread = std::make_shared<std::thread>(std::bind(&LightTraderAPI::processTaskThread, this));
    }
}

void LightTraderAPI::OnFrontConnected() noexcept {
    CThostFtdcReqAuthenticateField req = {0};
    std::memset(&req, 0, sizeof(req));
    std::strcpy(req.BrokerID, userconfig::BrokerID.c_str());
    std::strcpy(req.UserID, userconfig::UserID.c_str());
    std::strcpy(req.UserProductInfo, userconfig::ProductInfo.c_str());
    std::strcpy(req.AuthCode, userconfig::AuthCode.c_str());
    std::strcpy(req.AppID, userconfig::AppID.c_str());

    int reqRet = m_traderApi->ReqAuthenticate(&req, ++reqID);
    if (reqRet != 0) {
        std::cout << "OnFrontConnected ReqAuthenticate Failed:  " << reqRet << std::endl;
    }
}

void LightTraderAPI::OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept {
    if (pRspInfo != nullptr && pRspInfo->ErrorID) {
        std::cout << "OnRspUserLogin failed : " << pRspInfo->ErrorID << " ErrorMsg:" << pRspInfo->ErrorMsg << std::endl;
    }
    std::cout << "|Login success, LoginTime: " << pRspUserLogin->LoginTime << "|BrokerID: " << pRspUserLogin->BrokerID << "|UserID: " << pRspUserLogin->UserID << std::endl;

    CThostFtdcSettlementInfoConfirmField req;
    memset(&req, 0 , sizeof(req));
    std::strcpy(req.BrokerID, userconfig::BrokerID.c_str());
    std::strcpy(req.InvestorID, userconfig::UserID.c_str());

    int reqRet = m_traderApi->ReqSettlementInfoConfirm(&req, ++reqID);
    if (reqRet != 0) {
        std::cout << "OnRspUserLogin ReqSettlementInfoConfirm Failed:  " << reqRet << std::endl;
    }
}

void LightTraderAPI::OnRspAuthenticate(CThostFtdcRspAuthenticateField* pRspAuthenticateField, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept {
    if (pRspInfo != nullptr && pRspInfo->ErrorID) {
        std::cout << "OnRspAuthenticate failed : " << pRspInfo->ErrorID << " ErrorMsg:" << pRspInfo->ErrorMsg << std::endl;
    }
    if (pRspAuthenticateField != nullptr) {
        std::cout << "AppID: " << pRspAuthenticateField->AppID << "|AppType: " << pRspAuthenticateField->AppType << "|BrokerID: " << pRspAuthenticateField->BrokerID << "|UserProductInfo: " << pRspAuthenticateField->UserProductInfo;
        CThostFtdcReqUserLoginField req;
        std::memset(&req, 0, sizeof(req));
        std::strcpy(req.BrokerID, userconfig::BrokerID.c_str());
        std::strcpy(req.UserID, userconfig::UserID.c_str());
        std::strcpy(req.Password, userconfig::Password.c_str());
        int reqRet = m_traderApi->ReqUserLogin(&req, ++reqID);
        if (reqRet != 0) {
            std::cout << "OnRspAuthenticate ReqUserLogin Failed:  " << reqRet << std::endl;
        }
    } else {
        std::cout << "pRspAuthenticateField nullptr " << std::endl;
    }
}

void LightTraderAPI::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept {
    if (pRspInfo != nullptr && pRspInfo->ErrorID) {
        std::cout << "OnRspSettlementInfoConfirm failed : " << pRspInfo->ErrorID << " ErrorMsg:" << pRspInfo->ErrorMsg << std::endl;
    }
    if (pSettlementInfoConfirm != nullptr) {
        std::cout << "Success to confirm settlement information, |BrokerID: " << pSettlementInfoConfirm->BrokerID << "|ConfirmDate: " << pSettlementInfoConfirm->ConfirmDate << "|ConfirmTime: " << pSettlementInfoConfirm->ConfirmTime << "|InvestorID: " << pSettlementInfoConfirm->InvestorID << "|SettlementID: " << pSettlementInfoConfirm->SettlementID;
    } else {
        std::cout << "OnRspSettlementInfoConfirm nullptr " << std::endl;
    }

}

void LightTraderAPI::OnRtnOrder(CThostFtdcOrderField* pOrder) noexcept {
    const std::shared_ptr<TraderTask> msgS = std::make_shared<TraderTask>();
    msgS->setName(TradeTaskName::OnRtnOrder);
    const auto data = new CThostFtdcOrderField();
    *data = *pOrder;
    msgS->setMessage(data);
    msgQueue.push(msgS);
}

void LightTraderAPI::OnRspOrderInsert(CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept {

}

void LightTraderAPI::OnErrRtnOrderInsert(CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo) noexcept {

}

void LightTraderAPI::OnRtnTrade(CThostFtdcTradeField* pTrade) noexcept {
    const std::shared_ptr<TraderTask> msgS = std::make_shared<TraderTask>();
    msgS->setName(TradeTaskName::OnRtnTrade);
    const auto data = new CThostFtdcTradeField();
    *data = *pTrade;
    msgS->setMessage(data);
    msgQueue.push(msgS);
}

void LightTraderAPI::reqQryInstrument(const std::string& exchangeId, const std::string &instrumentId) const {
    CThostFtdcQryInstrumentField req;
    std::memset(&req, 0, sizeof(req));
    std::strcpy(req.ExchangeID, exchangeId.c_str());
    int reqRet = m_traderApi->ReqQryInstrument(&req, reqID);
    if (reqRet != 0) {
        std::cout << "reqQryInstrument ReqQryInstrument Failed:  " << reqRet << std::endl;
    }
}

void LightTraderAPI::OnRspQryInstrument(CThostFtdcInstrumentField* pInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept {
    if (pRspInfo != nullptr && pRspInfo->ErrorID) {
        std::cout << "OnRspQryInstrument failed : " << pRspInfo->ErrorID << " ErrorMsg:" << pRspInfo->ErrorMsg << std::endl;
    }
    if (pInstrument != nullptr) {
        std::cout << "|InstrumentID: " << pInstrument->InstrumentID << "|UnderlyingInstrID: " << pInstrument->UnderlyingInstrID << "|ExchangeID: " << pInstrument->ExchangeID << std::endl;
        contracts.emplace_back(pInstrument->InstrumentID);
    } else {
        std::cout << "OnRspQryInstrument nullptr " << std::endl;
    }
}

void LightTraderAPI::sendFakOrder(std::string instrumentId, char side, double price, int size) {
    CThostFtdcInputOrderField order;
    std::memset(&order, 0, sizeof(order));
    std::strcpy(order.BrokerID, userconfig::BrokerID.c_str());
    std::strcpy(order.InvestorID, userconfig::UserID.c_str());
    std::strcpy(order.InstrumentID, instrumentId.c_str());
    std::strcpy(order.UserID, userconfig::UserID.c_str());

    order.Direction = side;
    order.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
    order.TimeCondition = THOST_FTDC_TC_IOC; // 立即完成，否则撤销
    order.VolumeCondition = THOST_FTDC_VC_AV; ///任何数量
    order.MinVolume = 1;
    order.ContingentCondition = THOST_FTDC_CC_Immediately; //立即
    order.StopPrice = 0;
    order.ForceCloseReason = THOST_FTDC_FCC_NotForceClose; //非强平
    order.IsAutoSuspend = 0;
    order.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
    order.LimitPrice = price;
    order.VolumeTotalOriginal = size;
    order.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
    int reqRet = m_traderApi->ReqOrderInsert(&order, reqID);
    if (reqRet != 0) {
        std::cout << "sendFakOrder ReqOrderInsert Failed:  " << reqRet << std::endl;
    }
}

void LightTraderAPI::sendFokOrder(std::string instrumentId, char side, double price, int size) {
    CThostFtdcInputOrderField order;
    std::memset(&order, 0, sizeof(order));
    std::strcpy(order.BrokerID, userconfig::BrokerID.c_str());
    std::strcpy(order.InvestorID, userconfig::UserID.c_str());
    std::strcpy(order.InstrumentID, instrumentId.c_str());
    std::strcpy(order.UserID, userconfig::UserID.c_str());

    order.Direction = side;
    order.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
    order.TimeCondition = THOST_FTDC_TC_IOC; // 立即完成，否则撤销
    order.VolumeCondition = THOST_FTDC_VC_AV; ///任何数量
    order.MinVolume = 1;
    order.ContingentCondition = THOST_FTDC_CC_Immediately; //立即
    order.StopPrice = 0;
    order.ForceCloseReason = THOST_FTDC_FCC_NotForceClose; //非强平
    order.IsAutoSuspend = 0;
    order.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
    order.LimitPrice = price;
    order.VolumeTotalOriginal = size;
    order.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
    int reqRet = m_traderApi->ReqOrderInsert(&order, reqID);
    if (reqRet != 0) {
        std::cout << "sendFakOrder ReqOrderInsert Failed:  " << reqRet << std::endl;
    }
}

void LightTraderAPI::processTaskThread() {
    std::shared_ptr<TraderTask> task = nullptr;
    while(true) {
        msgQueue.waitAndPop(task);
        try {
            if (!task) {
                continue;
            }
            TradeTaskName taskName = task->getTaskName();
            switch (taskName)
            {
            case TradeTaskName::OnRspQryInstrument:
                /* code */
                // do some loging
                std::cout << "Querying instrument" << std::endl;
                break;
            case TradeTaskName::OnRtnOrder:
                processOrder(task);
                break;
            case TradeTaskName::OnRtnTrade:
                std::cout << "OnRtnTrade called." << std::endl;
                break;
            default:
                break;
            }
        } catch(...) {

        }
    }
}
void convert2Order(const CThostFtdcOrderField* pOrder, std::shared_ptr<SimpleOrder>& order) {
    order->setPrice(pOrder->LimitPrice);
    order->setAmount(pOrder->VolumeTotal);
}

void LightTraderAPI::processOrder(const std::shared_ptr<TraderTask>& orderInfo) const {
    const auto pOrder = static_cast<CThostFtdcOrderField*>(orderInfo->getMessage());
    std::shared_ptr<SimpleOrder> order = std::make_shared<SimpleOrder>();
    convert2Order(pOrder, order);
    std::cout << order->getPrice() << std::endl;
}


} // namespace naiveTrader
