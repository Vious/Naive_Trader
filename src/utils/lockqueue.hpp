#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>


namespace naiveTrader
{

template <typename T>
class LockQueue {
private:

    std::queue<T> mQueue;
    mutable std::mutex mMutex;
    std::condition_variable mSignal;

public:
    void push(T const& data) {
        {
            std::lock_guard<std::mutex> lock(mMutex);
            mQueue.push(data);
        }
        mSignal.notify_one();
    }

    bool tryPop(T &data) {
        std::lock_guard<std::mutex> lock(mMutex);
        if (mQueue.isEmpty()) {
            return false;
        }
        data = mQueue.front();
        mQueue.pop();
        return true;
    }

    void waitAndPop(T &data) {
        std::lock_guard<std::mutex> lock(mMutex);
        while(mQueue.isEmpty()) {
            mSignal.wait(lock);
        }
        data = mQueue.front();
        mQueue.pop();
    }

    bool tryWaitAndPopFor(T &data, uint32_t millis) {
        std::lock_guard<std::mutex> lock(mMutex);
        while(mQueue.isEmpty()) {
            mSignal.wait_for(lock, std::chrono::milliseconds(millis));
            return false;
        }
        data = mQueue.front();
        mQueue.pop();
        return true;
    }

    bool isEmpty() const {
        std::lock_guard<std::mutex> lock(mMutex);
        return mQueue.empty();
    }
};

} // namespace naiveTrader
