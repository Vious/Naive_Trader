#pragma once
#include <chrono>
#include <memory>
#include <thread>

#include "define.hpp"
#include "atomic_pool.hpp"
#include "stream_buffer.hpp"
#include "../utils/mpsc_queue.hpp"

namespace naiveTrader
{

enum class LogLevel : uint8_t {
    LOGLV_TRACE = 0U,
    LOGLV_DEBUG = 1U,
    LOGLV_INFO = 2U,
    LOGLV_WARNING = 3U,
    LOGLV_ERROR = 4U,
    LOGLV_FATAL = 5U
};

#define LOG_BUFFER_SIZE 1024U
#ifndef NDEBUG
#define LOG_QUEUE_SIZE 16384U
#else
#define LOG_QUEUE_SIZE 8192U
#endif

/*  */
struct NanoLogLine {
public:

    NanoLogLine() = default;

    ~NanoLogLine() = default;

    NanoLogLine(NanoLogLine &&) = default;
    NanoLogLine &operator=(NanoLogLine &&) = default;

    unsigned char mBuffer[LOG_BUFFER_SIZE] = {0};

    uint64_t mTimestamp = 0LLU;

    std::thread::id mThreadId = std::this_thread::get_id();

    LogLevel m_logLevel = LogLevel::LOGLV_TRACE;

    std::string m_sourceFile = "";

    std::string mFunc = "";

    uint32_t m_sourceLine = 0U;

    void initialize(LogLevel lv, const char *file, char const *func, uint32_t line) {
        m_logLevel = lv;
        m_sourceFile = file;
        mFunc = func;
        m_sourceLine = line;
        mTimestamp = std::chrono::duration_cast<std::chrono::microseconds> (std::chrono::system_clock::now().time_since_epoch()).count();
        mThreadId = std::this_thread::get_id();
        std::memset(mBuffer, 0, LOG_BUFFER_SIZE);
    }

};

using LogLinePool = AtomicPool<NanoLogLine, LOG_QUEUE_SIZE>;
using LogLineQueue = mpsc::MPSCQueue<NanoLogLine>;

void initLog(const char *path, size_t fileSize);

bool isReady();

NanoLogLine* allocLogLine();

void recycleLogLine(NanoLogLine* line);

void dumpLogLine(NanoLogLine* line);


/* log line */
class LogLine{
private:
    std::unique_ptr<stream_carbureter> m_streamdata;

    NanoLogLine* mLine;

    bool isDump;

public:

    LogLine(LogLevel lv, const char *file, char const *func, uint32_t line) : isDump(false), mLine(nullptr) {
        mLine = allocLogLine();
        mLine->initialize(lv, file, func, line);
        m_streamdata = std::make_unique<stream_carbureter>(mLine->mBuffer, LOG_BUFFER_SIZE);
    }

    ~LogLine() {
        if (!isDump) {
            recycleLogLine(mLine);
        }
    }

    template <typename First, typename... Types>
    typename std::enable_if<!std::is_enum<First>::Value, void>::type print(First firstArg, Types... args) {
        *m_streamdata << static_cast<std::decay_t<First>>(firstArg) << " ";
        print(args...);
    }

    template <typename First, typename... Types>
    typename std::enable_if<std::is_enum<First>::value, void>::type print(First firstArg, Types... args) {
        *m_streamdata << static_cast<uint8_t>(firstArg) << " ";
        print(args...);
    }

    template <typename... Types>
    void print(const std::string &firstArg, Types... args) {
        *m_streamdata << firstArg.c_str() << " ";
        print(args...);
    }

    template <typename... Types>
    void print(char *firstArg, Types... args) {
        *m_streamdata << static_cast<const char*>(firstArg) << " ";
        print(args...);
    }

    // define end function for recursive template func
    void print() {
        isDump = true;
        dumpLogLine(mLine);
    }

};


} // namespace naiveTrader

#ifndef NDEBUG
#define LOG_TRACE(...) if(isReady())LogLine(LogLevel::LOGLV_TRACE,__FILE__,__func__,__LINE__).print(__VA_ARGS__);
#define LOG_DEBUG(...) if(isReady())LogLine(LogLevel::LOGLV_DEBUG,__FILE__,__func__,__LINE__).print(__VA_ARGS__);
#define PROFILE_DEBUG(msg) //log_profile(LogLevel::LOGLV_DEBUG,__FILE__,__func__,__LINE__,msg);
#else
#define LOG_TRACE(...) 
#define LOG_DEBUG(...) 
#define PROFILE_DEBUG(msg) 
#endif
#define LOG_INFO(...) if(isReady())LogLine(LogLevel::LOGLV_INFO,__FILE__,__func__,__LINE__).print(__VA_ARGS__);
#define LOG_WARNING(...) if(isReady())LogLine(LogLevel::LOGLV_WARNING,__FILE__,__func__,__LINE__).print(__VA_ARGS__);
#define LOG_ERROR(...) if(isReady())LogLine(LogLevel::LOGLV_ERROR,__FILE__,__func__,__LINE__).print(__VA_ARGS__);
#define LOG_FATAL(...) if(isReady())LogLine(LogLevel::LOGLV_FATAL,__FILE__,__func__,__LINE__).print(__VA_ARGS__);

#define PROFILE_INFO(msg) //log_profile(LLV_INFO,__FILE__,__func__,__LINE__,msg);
