#pragma once
#include <string>

namespace Config {


	static std::string TRADE_FLOW = "/trade_data_flow";
	static std::string TRADE_FROND_ADDRESS = "tcp://180.168.146.187:10130";
	static std::string BROKER_ID = "9999";
	static std::string APP_ID = "simnow_client_test";
    static std::string AUTH_CODE = "0000000000000000";
    static std::string USER_ID = "~~~";
	static std::string USER_PRODUCT_INFO = "~~~";
	static std::string PASSWORD = "~~~";
    
    static std::string CFFEX = "CFFEX";     //中金所
	static std::string CZCE = "CZCE";       //郑商所
	static std::string DCE = "DCE";         //大商所
	static std::string INE = "INE";         //能源中心
	static std::string SHFE = "SHFE";       //上期所
    static std::string SGE = "SGE";         //广期所



	// market config
	static std::string MARKET_FROND_ADDRESS = "tcp://180.168.146.187:10131";

}
