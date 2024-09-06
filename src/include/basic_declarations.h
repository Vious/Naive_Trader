/* include defitions of some needed variables */

#pragma once
#include <string>
#include <memory>
#include <utility>
#include <cstring>
#include <vector>
#include <ostream>
#include <cmath>
#include <numeric>

// #ifndef EXPORT_FLAG
// #define EXPORT_FLAG __attribute__((__visibility__("default")))
// #endif

// define Exchange ID
#define EXCHANGE_SHFE    "SHFE"  // Shanghai Futures Exchange
#define EXCHANGE_CFFEX   "CFFEX" // China Financial Futures Exchange
#define EXCHANGE_GFEX    "GFEX"  // Guangzhou Futures Exchange

#define EXCHANGE_INE     "INE"   // Shanghai International Energy Exchange

#define EXCHANGE_DCE     "DCE"   // Dalian Commodity Exchange
#define EXCHANGE_CZCE    "CZCE"  // Zhengzhou Commodity Exchange

namespace naiveTrader
{

struct ExCode;

struct TickData;

struct PositionCell;

struct PositionData;

struct OrderData;

struct DailyMarketData;


enum class OrderStatus;

enum class PositionOffsetType;

enum class TradeDirection;

enum class EventType;

enum class ErrorType;

} // namespace naiveTrader
