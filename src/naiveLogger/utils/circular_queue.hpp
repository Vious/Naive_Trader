#pragma once

#include <vector>
#include <cassert>
#include "common.hpp"

namespace naiveLogger {

template <typename T>
class CircularQueue {
public:
    CircularQueue() = default;
    explicit CircularQueue(size_t max_size) : capacity(max_size + 1), que_vec(capacity) {}
    CircularQueue(const CircularQueue &circ_q) = default;
    CircularQueue &operator=(const CircularQueue &circ_q) = default;

    // move copy
    CircularQueue(CircularQueue &&other) {
        copyMoveable(std::move(other));
    }
    CircularQueue &operator=(CircularQueue &&other) {
        copyMoveable(std::move(other));
        return *this;
    }

    void enQueue(T &&item) {
        if (capacity > 0) {
            que_vec[rear] = std::move(item);
            rear = (rear + 1) % capacity;

            // check if it reaches the front
            if (rear == front) {
                front = (front + 1) % capacity;
                oversize_count++;
            }
        }
    }

    void deQueue() {
        front = (front + 1) % capacity;
    }

    bool isEmpty() const {
        return front == rear;
    }

    bool isFull() const {
        if (capacity > 0) {
            return ((rear + 1) % capacity) == front;
        }
        return false;
    }

    size_t getSize() const {
        if (rear >= front) {
            return rear - front;
        } else {
            return capacity - (front - rear);
        }
    }

    const T &at(size_t idx) const {
        assert(idx < getSize());
        return que_vec[(front + 1) % capacity];
    }

    const T &getFront() const {
        return que_vec[front];
    }
    T &getFront() {
        return que_vec[front];
    }


private:
    size_t capacity = 0;
    size_t oversize_count = 0;
    std::vector<T> que_vec;
    typename std::vector<T>::size_type front = 0;
    typename std::vector<T>::size_type rear = 0;

    void copyMoveable(CircularQueue &&other) {
        capacity = other.capacity;
        oversize_count = other.oversize_count;
        front = other.front;
        rear = other.rear;
        que_vec = std::move(other.que_vec);

        // disable the data in &other
        other.capacity = 0;
        other.front = 0;
        other.rear = 0;
        other.oversize_count = 0;
    }

};


// below are one solution code for leetcode problem: design a circular queue
// class CircularQueue {
// private:
//     int front;
//     size_t size;
//     size_t capacity;
//     std::vector<int> q;

// public:
//     CircularQueue(size_t k) {
//         capacity = k;
//         q = std::vector<int>(capacity);
//         front = 0;
//         size = 0;
//     }

//     bool isEmpty() {
//         return size == 0;
//     }

//     bool isFull() {
//         return size == capacity;
//     }

//     int Front() {
//         if (isEmpty()) {
//             return -1;
//         } else {
//             return q[front];
//         }
//     }

//     int Rear() {
//         if (isEmpty()) {
//             return -1;
//         } else {
//             size_t tmp = (front + size - 1) % capacity;
//             return q[tmp];
//         }
//     }

//     bool enQueue(int value) {
//         if (isFull()) {
//             return false;
//         } else {
//             size_t idx = (front + size) % capacity;
//             q[idx] = value;
//             size++;
//             return true;
//         }
//         return true;
//     }

//     bool deQueue() {
//         if (isEmpty()) {
//             return false;
//         } else {
//             front = (front + 1) % capacity;
//             size--;
//             return true;
//         }
//         return true;
//     }

// };


} // namespace naiveLogger