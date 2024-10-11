/* some functions for instruments, used for simple testing */
#pragma once
#include <string>

namespace naiveTrader
{

class SimpleInstrumet {
private:
	std::string m_exchangeID = "";
	std::string m_instrumentName = "";
	std::string m_productType = "";
	int m_deliveryYear  = 0;
	int m_deliveryMonth = 0;
	int m_maxMarketOrderVolume = 0;
	int m_minMarketOrderVolume = 0;
	int m_maxLimitOrderVolume = 0;
	int m_minLimitOrderVolume = 0;
	int m_volumeMultiple = 0;
	double m_priceTick = 0.0;
	std::string m_expireDate = "";
	std::string m_startDelivDate = "";
	std::string m_endDelivDate = "";
	double m_longMarginRatio = 0.0;
	double m_shortMarginRatio = 0.0;
	double m_strikePrice = 0.0;
	std::string m_optionsType = "";
	double m_underlyingMultiple = 0.0;
	std::string m_instrumentId = "";
	std::string m_exchangeInstId= "";
	std::string m_productID = "";
	std::string m_underlyingInstrID = "";

public:
    // setter
	void setExchangeId(const std::string exchangeId) { m_exchangeID = exchangeId; }

	void setInstrumentName(const std::string instrumentName) { m_instrumentName = instrumentName; }

	void setProductType(const std::string productClass) { m_productType = productClass; }

	void setDeliveryYear(const int deliveryYear) { m_deliveryYear = deliveryYear; }

	void setDeliveryMonth(const int deliveryMonth) { m_deliveryMonth = deliveryMonth; }

	void setMaxMarketOrderVolume(const int maxMarketOrderVolume) { m_maxMarketOrderVolume = maxMarketOrderVolume; }

	void setMinMarketOrderVolume(const int minMarketOrderVolume) { m_minMarketOrderVolume = minMarketOrderVolume; }

	void setMaxLimitOrderVolume(const int maxLimitOrderVolume) { m_maxLimitOrderVolume = maxLimitOrderVolume; }

	void setMinLimitOrderVolume(const int minLimitOrderVolume) { m_minLimitOrderVolume = minLimitOrderVolume; }

	void setVolumeMultiple(const int volumeMultiple) { m_volumeMultiple = volumeMultiple; }

	void setPriceTick(const double priceTick) { m_priceTick = priceTick; }

	void setExpireDate(const std::string& expireDate) { m_expireDate = expireDate; }

	void setStartDelivDate(const std::string& startDelivDate) { m_startDelivDate = startDelivDate; }

	void setEndDelivDate(const std::string& endDelivDate) { m_endDelivDate = endDelivDate; }

	void setLongMarginRatio(const double longMarginRatio) { m_longMarginRatio = longMarginRatio; }

	void setShortMarginRatio(const double shortMarginRatio) { m_shortMarginRatio = shortMarginRatio; }

	void setStrikePrice(const double strikePrice) { m_strikePrice = strikePrice; }

	void setOptionsType(const std::string optionsType) { m_optionsType = optionsType; }

	void setUnderlyingMultiple(const double underlyingMultiple) { m_underlyingMultiple = underlyingMultiple; }

	void setInstrumentId(const std::string& instrumentID) { m_instrumentId = instrumentID; }

	void setExchangeInstId(const std::string& exchangeInstId) { m_exchangeInstId = exchangeInstId; }

	void setProductId(const std::string& productId) { m_productID = productId; }

	void setUnderlyingInstrId(const std::string& underlyingInstrId) { m_underlyingInstrID = underlyingInstrId; }



    // getter
    const std::string getExchangeId() const { return m_exchangeID; }

	const std::string getInstrumentName() const { return m_instrumentName; }

	const std::string getProductType() const { return m_productType; }

	const int getDeliveryYear() const { return m_deliveryYear; }

	const int getDeliveryMonth() const { return m_deliveryMonth; }

	const int getMaxMarketOrderVolume() const { return m_maxMarketOrderVolume; }

	const int getMinMarketOrderVolume() const { return m_minMarketOrderVolume; }

	const int getMaxLimitOrderVolume() const { return m_maxLimitOrderVolume; }

	const int getMinLimitOrderVolume() const { return m_minLimitOrderVolume; }

	const int getVolumeMultiple() const { return m_volumeMultiple; }

	const double getPriceTick() const { return m_priceTick; }

	const std::string getExpireDate() const { return m_expireDate; }

	const std::string getStartDelivDate() const { return m_startDelivDate; }

	const std::string getEndDelivDate() const { return m_endDelivDate; }

	const double getLongMarginRatio() const { return m_longMarginRatio; }

	const double getShortMarginRatio() const { return m_shortMarginRatio; }

	const double getStrikePrice() const { return m_strikePrice; }

	const std::string getOptionsType() const { return m_optionsType; }

	const double getUnderlyingMultiple() const { return m_underlyingMultiple; }

	const std::string getInstrumentId() const { return m_instrumentId; }

	const std::string getExchangeInstId() const { return m_exchangeInstId; }

	const std::string getProductId() const { return m_productID; }

	const std::string getUnderlyingInstrId() const { return m_underlyingInstrID; }

};


} // namespace naiveTrader
