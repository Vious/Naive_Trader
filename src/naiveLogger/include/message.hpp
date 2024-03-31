#pragma once

#include <string>
#include "utils/common.hpp"

namespace naiveLogger 
{

// log message
struct log_message {
    log_message() = default;
    log_message(log_clock::time_point lg_time, source_location loc, string_view_t log_name, level_num lvl, string_view_t msg);
    log_message(const log_message &other) = default;
    log_message &operator=(const log_message &other) = default;

    source_location src_loc;
    string_view_t log_name;
    level_num level{level_num::off};
    log_clock::time_point time;

    size_t thread{0};

/*     
    string_view_t payload;

 */    // wrapping the formatted text with color (updated by pattern_formatter).
    // mutable size_t color_range_start{0};
    // mutable size_t color_range_end{0};


};



} // namespace naiveLogger