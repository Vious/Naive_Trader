#pragma once

#include <atomic>
#include <functional>
#include <mutex>
#include "../utils/common.hpp"
#include "../utils/circular_queue.hpp"
#include "message.hpp"

namespace naiveLogger {

namespace msgFunc{
    using FuncType = std::function<void(const LogMessage &log_msg)>;
}

class Backtracer {
public:
    Backtracer() = default;
    Backtracer(const Backtracer &other);

    Backtracer(Backtracer &&other);
    Backtracer &operator=(Backtracer other);

    // functions
    bool isEmpty() const;
    bool isEnabled() const;

    void enable(size_t size);
    void disable();

    void push_back(const LogMessage &log_msg);
    void foreach_pop(msgFunc::FuncType func);

    size_t getSize();

private:
    mutable std::mutex btr_mtx; // locker might always change its state
    std::atomic<bool> enabled_ = false;
    CircularQueue<MessageBuffer> msg_q;


}; // class Backtracer


size_t Backtracer::getSize() {
    return msg_q.getSize();
}

Backtracer::Backtracer(const Backtracer &other) {
    std::lock_guard<std::mutex> locker(other.btr_mtx);
    enabled_ = other.isEnabled();
    msg_q = other.msg_q;
}

Backtracer::Backtracer(Backtracer &&other) {
    std::lock_guard<std::mutex> locker(other.btr_mtx);
    enabled_ = other.isEnabled();
    msg_q = std::move(other.msg_q);
}

Backtracer &Backtracer::operator=(Backtracer other) {
    std::lock_guard<std::mutex> locker(this->btr_mtx);
    enabled_ = other.isEnabled();
    msg_q = std::move(other.msg_q);
    return *this;
}

bool Backtracer::isEmpty() const{
    std::lock_guard<std::mutex> lock(btr_mtx);
    return msg_q.isEmpty();
}

bool Backtracer::isEnabled() const {
    return enabled_.load(std::memory_order_relaxed);
}

void Backtracer::enable(size_t size) {
    std::lock_guard<std::mutex> lock(btr_mtx);
    enabled_.store(true, std::memory_order_relaxed);
    msg_q = CircularQueue<MessageBuffer>{size};
}

void Backtracer::disable() {
    std::lock_guard<std::mutex> lock(btr_mtx);
    enabled_.store(false, std::memory_order_relaxed);
}

void Backtracer::push_back(const LogMessage &a_msg) {
    std::lock_guard<std::mutex> lock(btr_mtx);
    msg_q.enQueue(MessageBuffer{a_msg});
}

void Backtracer::foreach_pop(msgFunc::FuncType func) {
    std::lock_guard<std::mutex> lock(btr_mtx);
    while(!msg_q.isEmpty()) {
        auto &msg = msg_q.getFront();
        func(msg);
        msg_q.deQueue();
    }
}


     
} // namespace naiveLogger