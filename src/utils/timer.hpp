/* reference to https://github.com/eglimi/cpptime
and https://github.com/li199603/cpp_toys/timer/Timer.hpp
 */
#pragma once
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <functional>
#include <vector>


namespace NaiveTimer 
{ // namespace NaiveTimer

using FuncType = std::function<void()>;
using Duration = std::chrono::microseconds;
using StdyClock = std::chrono::steady_clock;
using Timestamp = std::chrono::time_point<StdyClock>;

struct Task {
    std::size_t task_id;
    Duration period;

    FuncType func;

};


class Timer {
public:


private:
    std::thread worker_thread;
    std::mutex tm_mutex;
    std::condition_variable tm_condVar;
    std::atomic<bool> running_status;

    
};


} // namespace NaiveTimer