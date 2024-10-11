#include <thread>
#include <iostream>
#include <filesystem>

#include "../config/config.h"
#include "../apiadapter/market/market_api_light.h"
#include "../apiadapter/trade/trader_api_light.h"


using namespace naiveTrader;

int main(int argc, char *argv[])
{
    auto traderInstance = LightTraderAPI::getInstance();
    traderInstance->start();
    traderInstance->reqQryInstrument(userconfig::SHFE);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    auto marketApi = new naiveTrader::LightMarketAPI();
    // marketApi->OnRspSubMarketData();
    std::vector<std::string> allConstracts = getContracts();

    uint32_t size = allConstracts.size();
    for (uint32_t i = 0; i < size; i++) {
        marketApi->subscribe(allConstracts[i]);
    }

    std::string aInstrument = "MO2401-C-4800";

    std::string myIput;
    char side = '0';
	double price = 9.99;
	int size = 100;
	traderInstance->sendFakOrder(aInstrument, side, price, size);
	traderInstance->sendFokOrder(aInstrument, side, price, size);

    return 0;
}