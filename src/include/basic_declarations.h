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
#include <map>
#include <set>

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
using Str2StrMap = std::map<std::string, std::string>;
typedef uint64_t estid_t;

constexpr estid_t INVALID_ESTID = 0x0LLU;


enum class OrderType;

enum class PositionOffsetType;

enum class DirectionType;

enum class EventType;

enum class ErrorType;

enum class ErrorCode : uint8_t;

struct ExCode;

struct TickData;

struct PositionCell;

struct PositionData;

struct OrderData;

struct OrderStatistics;

struct DailyMarketData;

} // namespace naiveTrader
