/* reference: https://github.com/gabime/spdlog/blob/v1.x/include/spdlog/details/null_mutex.h
 */

#pragma once

#include <atomic>
// #include <utility>

namespace naiveLogger {

// should allow none mutex version
struct null_mutex {
    void lock() const {}
    void unlock() const {}
};

// none atomic int
struct null_atomic_int {
    int value;
    null_atomic_int() = default;

    explicit null_atomic_int(int new_value) : value(new_value) {}

    int load(std::memory_order = std::memory_order_relaxed) const {return value;}

    void store(int new_value, std::memory_order = std::memory_order_relaxed) {
        value = new_value;
    }

    int exchange(int new_value, std::memory_order = std::memory_order_relaxed) {
        std::swap(new_value, value);
        return new_value;
    }
};



}