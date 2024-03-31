#pragma once

#include <atomic>
#include <memory>
#include <chrono>
#include <functional>
#include <string>
#include <string_view>
#include <algorithm>
#include <iterator>
#include <cstring>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>

#include <fmt/core.h>
#include <fmt/format.h>


namespace naiveLogger
{

#if defined(SPDLOG_NO_ATOMIC_LEVELS)
using level_t = details::null_atomic_int;
#else
using level_t = std::atomic<int>;
#endif


using log_clock = std::chrono::system_clock;
using err_handler = std::function<void(const std::string *err_msg)>;

using filename_t = std::string;
using string_view_t = fmt::basic_string_view<char>;
using memory_buf_t = fmt::basic_memory_buffer<char, 500>;

template <typename... Args>
using format_string_t = fmt::format_string<Args...>;


#define LEVEL_TRACE 0
#define LEVEL_DEBUG 1
#define LEVEL_INFO 2
#define LEVEL_WARN 3
#define LEVEL_ERROR 4
#define LEVEL_CRITICAL 5
#define LEVEL_OFF 6

enum level_num : int {
    trace = LEVEL_TRACE,
    debug = LEVEL_DEBUG,
    info = LEVEL_INFO,
    warn = LEVEL_WARN,
    error = LEVEL_ERROR,
    critical = LEVEL_CRITICAL,
    off = LEVEL_OFF
};

static string_view_t level_names[] = {string_view_t("trace", 5), string_view_t("debug", 5), 
string_view_t("info", 4), string_view_t("warning", 7), string_view_t("error", 5), 
string_view_t("critical", 8), string_view_t("off", 3) 
};


// given a string of the level name, return level enum
level_num from_string(const std::string &name) {
    auto iter = std::find(std::begin(level_names), std::end(level_names), name);
    if (iter != std::end(level_names)) {
        return static_cast<level_num>(std::distance(std::begin(level_names), iter));
    }

    if (name == "warn") {
        return level_num::warn;
    } else if (name == "err") {
        return level_num::error;
    }
    return level_num::off;
}


/// 
struct source_location {
    int line{0};
    const char *filename{nullptr};
    const char *funcname{nullptr};

    source_location() = default;
    source_location(const char *filename, int line, const char *funcname): filename(filename), line(line), funcname(funcname) {}

    constexpr bool empty() {
        return line == 0;
    }
};


// to string_view

// return the string_view type of log level
const string_view_t& to_string_view(level_num level) {
    return level_names[level];
}

constexpr string_view_t to_string_view(const memory_buf_t &buf) {
    return string_view_t{buf.data(), buf.size()};
}

constexpr string_view_t to_string_view(string_view_t str) {
    return str;
}

template<typename T, typename... Args>
inline fmt::basic_string_view<T> to_string_view(fmt::basic_format_string<T, Args...> fmat) {
    return fmat;
}





namespace ntm {
    log_clock::time_point getNow() {
        return log_clock::now();
    }

    // get local Time, localtime_r (linux)
    // std::tm getLocalTime(const std::time_t &time_tt) {
    //     std::tm l_tm;
    //     localtime_r(&time_tt, &l_tm);
    //     return l_tm;
    // }

    // get local Time, localtime_r (linux)
    std::tm getLocalTime() {
        std::time_t tm_now = time(nullptr);
        std::tm l_tm;
        localtime_r(&tm_now, &l_tm);
        return l_tm;
    }

    // get gm time, gmtime_r (linux)
    std::tm getgmTime() {
        std::time_t tm_now = time(nullptr);
        std::tm gm_tm;
        gmtime_r(&tm_now, &gm_tm);
        return gm_tm;
    }

}

/// for file io 
namespace fileIO {



}


} // namespace naiveLogger
