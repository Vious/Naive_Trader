#pragma once

/* for some saving, just using only ostream */

#include "basic_declarations.h"

namespace naiveTrader
{

constexpr size_t CODE_LEN = 24;

// codeId for constract
#define CODEID_START    0
// exchange id
#define EXCGID_START      8
// commodity type id
#define COMDID_START    14
// commodity No.
#define COMDNUM_START   18

struct ExCode {
private:
    char mData[CODE_LEN];

public:
    ExCode(const char *codeID) {
        size_t exId, comdNumIdx;
        exId = comdNumIdx = 0;
        std::memset(&mData, 0, sizeof(mData));
        for (int i = 0; i < CODE_LEN && codeID[i] != '\0'; i++) {
            if (codeID[i] == '.') {
                exId = i;
                continue;
            }
            if (exId == 0) {
                mData[i + EXCGID_START] = codeID[i];
            } else {
                mData[i - exId - 1] = codeID[i];
                if (!comdNumIdx && codeID[i] >= '0' && codeID[i] <= '9') {
                    comdNumIdx = i - exId - 1;
                } else {
                    mData[i - exId - 1 + COMDID_START] = codeID[i];
                }
            }
        }
        *reinterpret_cast<uint16_t*> (mData + COMDNUM_START) = static_cast<uint16_t> (std::atoi(mData + comdNumIdx));
    }

    ExCode() {
        std::memset(&mData, 0, sizeof(mData));
    }

    ExCode(const ExCode &other) {
        std::memcpy(mData, other.mData, CODE_LEN);
    }

    ExCode(const char *id, const char *excgID) {
        std::memset(&mData, 0, sizeof(mData));
        size_t comdNumIdx;
        comdNumIdx = 0;
        for (int i = 0; i < EXCGID_START && id[i] != '\0'; i++) {
            mData[i] = id[i];
            if (!comdNumIdx && id[i] >= '0' && id[i] <= '9') {
                comdNumIdx = i;
            } else {
                mData[i + COMDID_START] = id[i];
            }
        }
        *reinterpret_cast<uint16_t*> (mData + COMDNUM_START) = static_cast<uint16_t>(std::atoi(mData + comdNumIdx));
        if (std::strlen(excgID) < COMDID_START - EXCGID_START) {
            std::strcpy(mData + EXCGID_START, excgID);
        }
    }

    ExCode(const char *excgId, const char *comdId, const char *comdNum) {
        std::memset(&mData, 0, sizeof(mData));
        if (EXCGID_START > (std::strlen(comdId) + std::strlen(comdNum))) {
            std::strcpy(mData, comdId);
            size_t comdNumIdx = std::strlen(mData);
            std::strcpy(mData + comdNumIdx, comdNum);
            std::strcpy(mData + COMDID_START, comdId);
            *reinterpret_cast<uint16_t*> (mData + COMDNUM_START) = static_cast<uint16_t>(std::atoi(mData + comdNumIdx));
        }
        if ((COMDID_START - EXCGID_START) > std::strlen(excgId)) {
            std::strcpy(mData + EXCGID_START, excgId);
        }
    }

    bool operator == (const ExCode &other) const {
        return std::memcmp(mData, other.mData, CODE_LEN) == 0; 
    }

    bool operator < (const ExCode &other) const {
        return std::memcmp(mData, other.mData, CODE_LEN) < 0;
    }

    bool operator != (const ExCode &other) const {
        return std::memcmp(mData, other.mData, CODE_LEN) != 0;
    }

    const char* getCode() const {
		return mData;
	}

    const char* getExcgId() const {
        return mData + EXCGID_START;
    }

    const char* getComdId() const {
        return mData + COMDID_START;
    }

    uint16_t getComdNum() const {
        return *reinterpret_cast<const uint16_t*>(mData + COMDNUM_START);
    }

    std::ostream &operator << (std::ostream &os) {
        return os << getCode();
    }

    bool isDistinct() const {
        if (std::strcmp(EXCHANGE_SHFE, getExcgId()) == 0) {
            return true;
        } else if (std::strcmp(EXCHANGE_CFFEX, getExcgId()) == 0) {
            return true;
        } else if (std::strcmp(EXCHANGE_DCE, getExcgId()) == 0) {
            return true;
        } else if (std::strcmp(EXCHANGE_GFEX, getExcgId()) == 0) {
            return true;
        } else if (std::strcmp(EXCHANGE_INE, getExcgId()) == 0) {
            return true;
        } else if (std::strcmp(EXCHANGE_CZCE, getExcgId()) == 0) {
            return true;
        } 
        return false;
    }

};

const ExCode emptyCode;

using dayTime_t = uint32_t;
using order_t = std::array<std::pair<double_t, uint32_t>, 5>;

struct TickData {
    /* data */
    ExCode codeId;

    dayTime_t dtime; // millis

    double_t price;

    double_t openPrice;

    double_t closePrice;

    double_t high;

    double_t low;

    double_t standard;

    double_t openInterest;

    uint64_t volume;

    uint32_t tradingDay;

    order_t buyOrder;

    order_t sellOrder;

    double_t getBuyPrice() const {
        if (buyOrder.empty()) {
            return 0;
        }
        return buyOrder[0].first;
    }

    double_t getSellPrice() const {
        if (sellOrder.empty()) {
            return 0;
        }
        return sellOrder[0].first;
    }
    
    TickData(): dtime(0), price(0), openPrice(0), closePrice(0), high(0), low(0), \
        standard(0), volume(0llU), tradingDay(0), openInterest(.0F) {}

    TickData(const ExCode &code, dayTime_t dtm, double_t price, double_t openPrice, double_t clsPrice, double_t high, double_t low, double_t std, double_t interests, uint64_t vol, uint32_t traday, order_t && buyOrder, order_t && sellOrder): \
        codeId(code), dtime(dtm), price(price), openPrice(openPrice), closePrice(clsPrice), high(high), low(low), standard(std), openInterest(interests), volume(vol), tradingDay(traday), buyOrder(buyOrder), sellOrder(sellOrder) {}
    
    uint32_t totalBuyVolume() const {
        uint32_t sum = 0;
        for (auto &iter : buyOrder) {
            sum += iter.second;
        }
        return sum;
    }

    uint32_t totalSellVoluem() const {
        uint32_t sum = 0;
        for (auto &iter : sellOrder) {
            sum += iter.second;
        }
        return sum;
    }

    bool isValidTick() const {
        return !(codeId == emptyCode);
    }

};

const TickData emptyTick;

struct PositionCell {
    uint32_t position;

    uint32_t frozen;

    PositionCell(): position(0), frozen(0) {}

    uint32_t usable() const {
        return position - frozen;
    }

    bool isEmpty() const {
        return position == 0;
    }

    void clear() {
        position = 0;
        frozen = 0;
    }

};

struct PositionData {
    ExCode codeId;

    // today's position
    PositionCell todayLong;
    PositionCell todayShort;

    // position of yesterday
    PositionCell historyLong;
    PositionCell historyShort;

    // pending 
    uint32_t longPending;
    uint32_t shortPending;

    PositionData(const ExCode &code): codeId(code), longPending(0), shortPending(0) {}

    PositionData(): longPending(0), shortPending(0) {}

    uint32_t getLongPosition() const {
        return todayLong.position + historyLong.position;
    }

    uint32_t getShortPosition() const {
        return todayShort.position + historyShort.position;
    }

    uint32_t getLongFrozen() const {
        return todayLong.frozen + historyLong.frozen;
    }

    uint32_t getShortFrozen() const {
        return todayShort.frozen + historyShort.frozen;
    }

    uint32_t getTotalNum() const {
        return todayLong.position + todayShort.position + historyLong.position + historyShort.position;
    }

    int32_t getLongShortDiff() const {
        return todayLong.position + historyLong.position - (todayShort.position + historyShort.position); 
    }

    bool isEmpty() const {
        return todayLong.isEmpty() && todayShort.isEmpty() && historyLong.isEmpty() && historyShort.isEmpty() && longPending == 0U && shortPending == 0U;
    }

};

const PositionData emptyPosition;


enum class OrderType {
    OT_NORMAL = '0', // normal order
    OT_FOK, // /* an order to buy or sell a contract that must be executed immediately in its entirety; otherwise, the entire order will be canceled (i.e., no partial execution of the order is allowed). */
    OT_FAK, // /* immediately executed against resting orders. If the order cannot be fully filled, the remaining balance is canceled.  */

};

enum class PosOffsetType {
    POFFT_OPEN = '0', // /* open position */
    POFFT_CLOSE,      // /* close position (yesterday's position for SHFE) */
    POFFT_CLSTD       // /* close today's position */  
};

enum class DirectionType {
    DT_LONG = '0', // LONG
    DT_SHORT       // SHORT
};

struct OrderData {
    estid_t estId;
    ExCode code;
    std::string unitId;
    
    uint32_t totalVolume;
    uint32_t lastVolume;
    
    dayTime_t creatTime;

    PosOffsetType offset;
    DirectionType direction;
    
    double_t price;

    OrderData() : estId(INVALID_ESTID), offset(PosOffsetType::POFFT_OPEN), direction(DirectionType::DT_LONG), totalVolume(0), lastVolume(0), creatTime(0), price(.0f) {}

    bool isValid() const {
        return estId == INVALID_ESTID;
    }

    bool isBuy() const {
        if (direction == DirectionType::DT_LONG && offset == PosOffsetType::POFFT_OPEN) {
            return true;
        }
        if (direction == DirectionType::DT_SHORT && (offset == PosOffsetType::POFFT_CLSTD || offset == PosOffsetType::POFFT_CLOSE)) {
            return true;
        }
        return false;
    }

    bool isSell() const {
        if (direction == DirectionType::DT_SHORT && offset == PosOffsetType::POFFT_OPEN) {
            return true;
        }
        if (direction == DirectionType::DT_LONG && (offset == PosOffsetType::POFFT_CLOSE || offset == PosOffsetType::POFFT_CLSTD)) {
            return true;
        }
        return false;
    }

};

const OrderData emptyOrder;

struct OrderStatistics {
    // Place order amount
    uint32_t placeOrdAmount;

    // delegated order amount
    uint32_t delegatedAmount;

    // Executed order amount
    uint32_t executAmount;

    // cancelled order amount
    uint32_t cancelAmount;

    // Error order amount
    uint32_t errorAmount;

    OrderStatistics(): placeOrdAmount(0), delegatedAmount(0), executAmount(0), \
        cancelAmount(0), errorAmount(0) {}

public:
    std::ostream &operator >> (std::ostream &os) {
        os << placeOrdAmount << delegatedAmount << executAmount << cancelAmount << errorAmount;
        return os;
    }

};

enum class ErrorType {
    ERR_PLACE_ORDER,
    ERR_CANCEL_ORDER,
    ERR_OTHER,
};

enum class ErrorCode : uint8_t{
    EC_SUCCESS,
    EC_FAILURE,
    EC_ORDER_FIELD_ERROR = 23U, // Order field error
    EC_POSITION_NOT_ENOUGH = 30U, // Insufficient position
    EC_MARGIN_NOT_ENOUGH = 31U, // Insufficient margin
    EC_STATE_NOT_READY = 32, // Incorrect state
};

struct DailyMarketData {
    ExCode code;

    std::map<double_t, uint32_t> volumeDistribution;

    TickData lastTickInfo;

    double_t getControlPrice() const {
        double_t ctrlPrice = lastTickInfo.price;
        uint32_t maxVolume = 0;
        for (const auto &iter : volumeDistribution) {
            if (iter.second > maxVolume) {
                maxVolume = iter.second;
                ctrlPrice = iter.first;
            }
        }
        return ctrlPrice;
    }
    
    void clear() {
        volumeDistribution.clear();
    }

};

const DailyMarketData emptyDailyMarket;

}