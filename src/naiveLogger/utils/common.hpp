#pragma once

#include <atomic>
#include <memory>
#include <chrono>
#include <thread>
#include <functional>
#include <string>
#include <string_view>
#include <algorithm>
#include <iterator>
#include <cstring>
#include <ctime>
#include <cstdio>
#include <cstdlib>
// system call functions
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

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
// or using memory_buf_t = std::string 
using memory_buf_t = fmt::basic_memory_buffer<char, 250>;

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
struct SourceLoc {
    int line{0};
    const char *filename{nullptr};
    const char *funcname{nullptr};

    constexpr SourceLoc() = default;
    constexpr SourceLoc(const char *filename, int line, const char *funcname): filename(filename), line(line), funcname(funcname) {}

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



// get system environment variables
// source https://github.com/gabime/spdlog
std::string getEnv(const char *field) {
    char *buf = getenv(field);
    return buf? buf : std::string{};
}



// namespace ntm (for time)
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

    //calculate the minutes offsets of local time to utc time
    int uts_offsets_minutes(const std::tm &tm) {
        // const std::tm &localtm = getLocalTime();
        // const std::tm &gmtm = getgmTime();

        // int localt_year = localtm.tm_year + (1900 - 1);
        // int gmt_year = gmtm.tm_year + (1900 - 1);

        // // 
        // long int days_diff = (localtm.tm_yday - gmtm.tm_yday) + 
        //     ((localt_year >> 2) - (gmt_year >> 2)) - (localt_year / 100 - gmt_year / 100) +
        //     ((localt_year / 100 >> 2) - (gmt_year / 100) >> 2);

        // long int hours_diff = 24 * days_diff + (localtm.tm_hour - gmtm.tm_hour);
        // long int minutes_diff = 60 * hours_diff + (localtm.tm_min - gmtm.tm_min);
        // long int secs_diff = 60 * minutes_diff + (localtm.tm_sec - gmtm.tm_sec);

        // return static_cast<int> (secs_diff / 60);
        return static_cast<int>(tm.tm_gmtoff / 60);
    }

} // namespace ntm (for time)


/// for file io 
namespace fileIO {
    // follow the folder sep for unix file system
    // static const std::string::value_type folder_seps[] = "/";
    
    bool fopen_s(FILE **fp, const filename_t &filename, const filename_t &mode) {
        *fp = fopen((filename.c_str()), mode.c_str());
        return *fp == nullptr;      
    }

    // get file size (for linux)
    size_t getFileSize(FILE *file) {
        if (file == nullptr) {
            std::cout << "Failed to get the file, nullptr!\n";
            return 111111;
        } else {
            // currently only work for x64 system
            int fd = fileno(file);
            struct stat64 st;
            if (fstat64(fd, &st) == 0) {
                return static_cast<size_t> (st.st_size);
            }
        }
        std::cout << "Failed to get the size from fd.\n";
        return 0;
    }

    int rename(const filename_t &ori_name, filename_t &tar_name) {
        return std::rename(ori_name.c_str(), tar_name.c_str());
    }

    bool pathExists(const filename_t &filename) {
        struct stat buffer;
        return stat(filename.c_str(), &buffer) == 0;
    }

    int remove(const filename_t &filename) {
        return std::remove(filename.c_str());
    }

    int removeIfExist(const filename_t &filename) {
        return pathExists(filename) ? remove(filename) : 0;
    }

    // get directory name for a file
    filename_t getDirName(const filename_t &path) {
        size_t pos = path.find_last_of("/");
        return pos != filename_t::npos ? path.substr(0, pos) : filename_t{};
    }

    // mkdir funtion
    bool createDir(const filename_t &path) {
        if(pathExists(path)) {
            return true;
        } else if(path.empty()){
            return false;
        } else {
            size_t moving_pos = 0;
            do {
                auto cur_pos = path.find_first_of("/", moving_pos);
                if (cur_pos == filename_t::npos) {
                    cur_pos = path.size() - 1;
                }
                auto sub_dir = path.substr(0, cur_pos);
                // calling mkdir function
                if (!sub_dir.empty() && !pathExists(sub_dir) && mkdir(sub_dir.c_str(), 0777) == -1) {
                    return false;
                }
                moving_pos = cur_pos + 1;
            } while(moving_pos < path.size());
        }

        return true;
    }

    // file sync (linux)
    bool fileSync(FILE *fp) {
        return fsync(fileno(fp)) == 0;
    }


} //namespace fileIO



// namespace nthread, for thread related functions
namespace nthread{
    size_t getThreadId() {
        return static_cast<size_t>(syscall(__NR_gettid));
    }

    void sleepForMillis(unsigned int millis) {
        std::this_thread::sleep_for(std::chrono::milliseconds(millis));
    }

    int getPid() {
        return static_cast<int>(getpid());
    }

} // namespace nthread






} // namespace naiveLogger
