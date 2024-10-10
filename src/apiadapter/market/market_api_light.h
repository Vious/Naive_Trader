/*  This light version market/trader apis are only used to 
    test the connection and basic function calls to simnow platform
*/
#pragma once

#include <string>
#include <memory>
#include "../../../api/CTP_V6.6.9_20220920/ThostFtdcMdApi.h"

namespace naiveTrader
{

class LightMarketAPI final : public CThostFtdcMdSpi {
private:
    
    CThostFtdcMdApi *m_marketApi;
    uint32_t reqID;

public:
    LightMarketAPI();
    virtual ~LightMarketAPI() = default;

    void OnFrontConnected() noexcept override;

    void OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept override;

    void OnRspSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) noexcept override;

    void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData) noexcept override;

public:

    void subscribe(const std::string &instrumentID) noexcept;
};


} // namespace naiveTrader
