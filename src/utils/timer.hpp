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
#include <atomic>
#include <map>


namespace NaiveTimer 
{ // namespace NaiveTimer

// using TimerId = std::size_t;
using FuncType = std::function<void()>;
using DurationType = std::chrono::milliseconds;
using SysClock = std::chrono::system_clock;

struct Task {
    uint64_t task_id;
    // Timestamp start_time;
    uint64_t period;
    
    FuncType func;
    bool isValid;
    bool repeatable;

    // Task(): task_id(0), start_time(DurationType::zero()), period(DurationType::zero()), func(nullptr), repeatable(false), isValid(false) {}

    // Task(TimerId t_id, Timestamp start, DurationType period, FuncType&& func, bool repeatable)
    //     : task_id(t_id), start_time(start), period(period), func(func), repeatable(repeatable), isValid(true)
    // {}
    // Task(): task_id(0), period(DurationType::zero()), func(nullptr), repeatable(false), isValid(false) {}

    Task(uint64_t t_id, uint64_t period, FuncType&& func, bool repeatable)
        : task_id(t_id), period(period), func(func), repeatable(repeatable), isValid(true)
    {}

    // Task(Task&& task) = default;
    // Task& operator=(Task&& task) = default;
    // Task(const Task& task) = delete;
    // Task& operator=(const Task& task) = delete;

};


class Timer {
public:
    Timer();
    ~Timer();
    uint64_t add(uint64_t period_ms, FuncType&& func, bool repeatable);
    bool remove(uint64_t id);
    // void stop();

private:
    std::thread worker_thread;
    std::mutex tm_mutex;
    std::condition_variable tm_condVar;
    std::atomic<bool> isDone;

    uint64_t tm_curId;
    std::multimap<uint64_t, Task> tm_tasks;

    void run();
    
};


Timer::Timer() {
    isDone = false;
    worker_thread = std::thread([this] {run(); } );
}

Timer::~Timer() {
    isDone = true;
    tm_condVar.notify_all();
    worker_thread.join();
    tm_tasks.clear();
}

void Timer::run() {
    
    while(!isDone) {
        std::unique_lock<std::mutex> locker(tm_mutex);
        tm_condVar.wait(locker, [this]() -> bool {return !isDone || !tm_tasks.empty();});
        if (isDone) {
            break;
        }

        uint64_t cur_time = std::chrono::duration_cast<DurationType>(SysClock::now().time_since_epoch()).count();
        std::multimap<uint64_t, Task>::iterator iter = tm_tasks.begin();
        uint64_t task_time = iter->first;
        if (cur_time >= task_time) {
            Task& cur_task = iter->second;
            if (cur_task.isValid) {
                locker.unlock();
                // std::cout << "running function...\n"; 
                cur_task.func();
                locker.lock();
                
                if (cur_task.repeatable && cur_task.isValid) {
                    uint64_t when = cur_time + cur_task.period;
                    Task new_task(cur_task.task_id, cur_task.period, std::move(cur_task.func), cur_task.repeatable);
                    tm_tasks.insert({when, new_task});
                } 

            }
            tm_tasks.erase(iter);

        } else {
            // std::cout << "waiting for running time...\n"; 
            tm_condVar.wait_for(locker, DurationType(task_time - cur_time));
        }

    }
}


uint64_t Timer::add(uint64_t period_ms, FuncType&& func, bool repeatable) {
    uint64_t cur_time = std::chrono::duration_cast<DurationType>(SysClock::now().time_since_epoch()).count();
    uint64_t when = cur_time + period_ms;
    Task new_task(tm_curId, period_ms, std::move(func), repeatable);
    {
        std::lock_guard<std::mutex> locker(tm_mutex);
        tm_tasks.insert({when, new_task});
    }
    tm_curId++;
    tm_condVar.notify_all();
    return tm_curId;
}


bool Timer::remove(uint64_t id) {
    bool rmSuccess = false;
    std::lock_guard<std::mutex> locker(tm_mutex);
    std::multimap<uint64_t, Task>::iterator iter = 
        std::find_if(tm_tasks.begin(), tm_tasks.end(), [id](const std::pair<uint64_t, Task>& item) -> bool {return item.second.task_id == id;} );
    
    if (iter != tm_tasks.end()) {
        iter->second.isValid = false;
        tm_tasks.erase(iter);
        rmSuccess = true; 
    }

    return rmSuccess;
}



} // namespace NaiveTimer