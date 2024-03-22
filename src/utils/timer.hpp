/* reference: https://github.com/eglimi/cpptime
and https://github.com/li199603/cpp_toys/blob/master/timer/Timer.hpp
 */
#pragma once
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <functional>
#include <vector>
#include <map>
#include <stack>


namespace NaiveTimer 
{ // namespace NaiveTimer

using TimerId = std:;size_t;
using FuncType = std::function<void()>;
using Duration = std::chrono::microseconds;
using StdyClock = std::chrono::steady_clock;
using Timestamp = std::chrono::time_point<StdyClock>;

struct Task {
    TimerId task_id;
    Timestamp start_time;
    Duration period;
    
    FuncType func;
    bool isValid;
    bool repeatable;

    Task(): task_id(0), start_time(Duration::zero()), period(Duration::zero()), func(nullptr), repeatable(false), isValid(false) {}

    Task(TimerId t_id, Timestamp start, Duration period, FuncType&& func, bool repeatable)
        : task_id(t_id), start_time(start), period(period), func(func), repeatable(repeatable), isValid(true)
    {}

    Task(Task&& task) = default;
    Task& operator=(Task&& task) = default;
    Task(const Task& task) = delete;
    Task& operator=(const Task& task) = delete;

};


class Timer {
public:
    Timer();
    ~Timer();
    TimerId add();
    bool remove(TimerId);

private:
    std::thread worker_thread;
    std::mutex tm_mutex;
    std::condition_variable tm_condVar;
    std::atomic<bool> running_status;

    TimerId tm_curId;

    std::vector<Task> tasks;
    // use multimap
    

    void run();
    
};


} // namespace NaiveTimer