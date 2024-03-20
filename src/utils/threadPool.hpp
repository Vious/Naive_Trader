#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <thread>
#include <future>
#include <type_traits>

template <typename T>
class SimpleQueue {
public:
    SimpleQueue() = default;
    
    ~SimpleQueue() = default;

    SimpleQueue(const SimpleQueue& sq) = delete;
    SimpleQueue(const SimpleQueue&& sq) = delete;
    SimpleQueue(SimpleQueue&& sq) = delete;
    SimpleQueue& operator=(const SimpleQueue& sq) = delete;
    SimpleQueue& operator=(const SimpleQueue&& sq) = delete;
    
    void push(T& data) {
        std::unique_lock<std::mutex> locker(sq_mutex);
        sq_mQueue.emplace(data);
    }

    void push(T&& data) {
        std::unique_lock<std::mutex> locker(sq_mutex);
        sq_mQueue.emplace(std::move(data));
    }

    bool pop() {
        std::unique_lock<std::mutex> locker(sq_mutex);
        if (sq_mQueue.empty()) {
            return false;
        } else {
            sq_mQueue.pop();
            return true;
        }
    }

    bool pop(T& data) {
        std::unique_lock<std::mutex> locker(sq_mutex);
        if (sq_mQueue.empty()) {
            return false;
        } else {
            data = std::move(sq_mQueue.front());
            sq_mQueue.pop();
            return true;
        }

    }

    int size() {
        std::unique_lock<std::mutex> locker(sq_mutex);
        return sq_mQueue.size();
    }

    bool empty() {
        std::unique_lock<std::mutex> locker(sq_mutex);
        return sq_mQueue.empty();
    }



private:
    std::queue<T> sq_mQueue;
    std::mutex sq_mutex;

};


// define a simple thread pool class, dealing with tasks, functions, etc.

class SimpleThreadPool {
public:
    SimpleThreadPool(const SimpleThreadPool&) = delete;
    SimpleThreadPool(const SimpleThreadPool&&) = delete;

    SimpleThreadPool& operator=(const SimpleThreadPool&) = delete;
    SimpleThreadPool& operator=(const SimpleThreadPool&&) = delete;

    SimpleThreadPool(int num_of_threads): stp_threads(num_of_threads), stp_runStatus(true) {
        initialize();
     }

    SimpleThreadPool(): stp_threads(std::thread::hardware_concurrency()), stp_runStatus(true) {
        initialize();
    }

    ~SimpleThreadPool() {
        stp_runStatus = false;
        stp_condVar.notify_all();

        for (auto& thread: stp_threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

    }

    //template functions for task submission
    template <typename Func, typename... Args>
    auto submitTask(Func&& func, Args... args) {
        using returnType = typename std::invoke_result<Func, Args...>::type;
        std::function<returnType()> taskWrapper1 = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);

        auto taskWrapper2 = std::make_shared<std::packaged_task<returnType()>>(taskWrapper1);
        TaskType wrapperFunction = [taskWrapper2]() {
            (*taskWrapper2)();
        };

        stp_queue.push(wrapperFunction);
        stp_condVar.notify_one();

        return taskWrapper2->get_future();
    }


private:
    std::mutex stp_mutex;
    std::condition_variable stp_condVar;
    std::atomic<bool> stp_runStatus;
    std::vector<std::thread> stp_threads;

    using TaskType = std::function<void()>;
    SimpleQueue<TaskType> stp_queue;

    void initialize() {
        for (int i = 0; i < stp_threads.size(); i++) {
            auto worker = [this, i] {
                while (this->stp_runStatus) {
                    TaskType task;
                    bool shouldRun = false;
                    {
                        std::unique_lock<std::mutex> locker(stp_mutex);
                        if (this->stp_queue.empty()) {
                            this->stp_condVar.wait(locker);
                        }
                        shouldRun = this->stp_queue.pop(task);
                    }
                    if (shouldRun) {
                        std::cout << "start running task : " << i << std::endl;
                        task();
                        std::cout << "task " << i << " finished." << std::endl; 
                    }
                }
            };


        }
    }

};