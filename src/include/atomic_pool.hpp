#pragma once
#include <iostream>
#include <vector>
#include <atomic>

#include "define.hpp"

namespace naiveTrader
{

template <typename T, size_t N>
class AtomicPool {
private:
    std::atomic<bool> bLock;

    std::vector<std::shared_ptr<T>> mData;
    std::array<std::atomic<bool>, N> mIsUsed;

public:
    AtomicPool(size_t initSize) : bLock(false) {
        static_assert(std::is_default_constructible<T>::value, "AtomicPool needs a default constructor");
        for (size_t i = 0; i < N; i++) {
            if (i < initSize) {
                mData.emplace_back(std::make_shared<T>());
            }
            mIsUsed[i].store(false, std::memory_order_release);
        }
    }

    ~AtomicPool() {
        size_t size = mData.size();
        for (size_t i = 0; i < size; i++) {
            delete mData[i];
        }
        mData.clear();
    }

    std::shared_ptr<T> resize(bool isUsed) {
        while(bLock.exchange(true, std::memory_order_acquire));
        std::shared_ptr<T> ret;
        if (mData.size() < N) {
            size_t len = mData.size();
            mData.emplace_back(std::make_shared<T>());
            mIsUsed[len].store(isUsed, std::memory_order_release);
            ret = mData.back();
        }
        bLock.store(false, std::memory_order_release);
        return ret;
    }

    std::shared_ptr<T> alloc(bool force) {
        std::shared_ptr<T> ret = nullptr;
        size_t size = mData.size();
        for (size_t i = 0; i < size; i++) {
            auto &isUsed = mIsUsed[i];
            bool expected = false;
            if (isUsed.compare_exchange_weak(expected, true, std::memory_order_relaxed)) {
                ret = mData[i];
            }
        }
        if (ret == nullptr) {
            ret = resize(true);
            if (force && ret == nullptr) {
                for (size_t i = 0; ; i = (i + 1) % mData.size()) {
                    auto &isUsed = mIsUsed[i];
                    bool expected = false;
                    if (isUsed.compare_exchange_weak(expected, true, std::memory_order_relaxed)) {
                        ret = mData[i];
                        break;
                    }
                }
            }
        }
        return ret;
    }

    void recycle(std::shared_ptr<T> data) {
        size_t size = mData.size();
        for (size_t i = 0; i < size; i++) {
            if (mData[i] == data) {
                mIsUsed[i].store(false, std::memory_order_release);
                break;
            }
        }
    }

};
    
} // namespace naiveTrader
