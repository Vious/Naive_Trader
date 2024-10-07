#include "../include/log_wrapper.hpp"
#include "../loger/nanolog.hpp"
#include "nanolog.hpp"

#include <chrono>
#include <filesystem>
#include "../utils/string_helper.hpp"
#include "../utils/process_helper.hpp"

namespace naiveTrader
{
using namespace nanolog;

bool isLogReady = false;

std::unique_ptr<NanoLogger> nanologger;

std::atomic<NanoLogger*> atomic_nanologger;

std::atomic<bool> m_isReady = false;

bool isReady() {
    return m_isReady.load();
}

NanoLogLine* allocLogLine() {
    return atomic_nanologger.load(std::memory_order_acquire)->alloc();
}

void recycleLogLine(NanoLogLine* line) {
    atomic_nanologger.load(std::memory_order_acquire)->recycle(line);
}

void dumpLogLine(NanoLogLine* line) {
    atomic_nanologger.load(std::memory_order_acquire)->dump(line);
}

void init_log(const char* path,size_t file_size)
{
	if (!std::filesystem::exists(path))
	{
		std::filesystem::create_directories(path);
	}
	auto time_string = (std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()),"%Y-%m-%d_%H%M%S");
	auto file_name = StringHelper::format("lt_{0}.{1}", time_string, process_helper::get_pid());

#ifndef NDEBUG
	nanologger.reset(new NanoLogger(path, file_name, file_size, 10240));
	uint8_t field = static_cast<uint8_t>(LogField::TIME_SPAMP) | static_cast<uint8_t>(LogField::THREAD_ID) | static_cast<uint8_t>(LogField::LOG_LEVEL) | static_cast<uint8_t>(LogField::SOURCE_FILE);
	uint8_t print = static_cast<uint8_t>(LogPrint::LOG_FILE) | static_cast<uint8_t>(LogPrint::CONSOLE);
	nanologger->set_option(LogLevel::LOGLV_TRACE, field, print);
#else
	nanologger.reset(new NanoLogger(path, file_name, file_size, 1024));
	uint8_t field = static_cast<uint8_t>(LogField::TIME_SPAMP) | static_cast<uint8_t>(LogField::THREAD_ID) | static_cast<uint8_t>(LogField::LOG_LEVEL) ;
	uint8_t print = static_cast<uint8_t>(LogPrint::LOG_FILE) ;
	nanologger->set_option(LogLevel::LLV_INFO, field, print);
#endif
	atomic_nanologger.store(nanologger.get(), std::memory_order_seq_cst);
	m_isReady.store(true, std::memory_order_release);
}

}