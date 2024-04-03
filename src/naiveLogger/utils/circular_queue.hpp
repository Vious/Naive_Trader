#pragma once

#include <vector>
#include <cassert>
#include "common.hpp"

namespace naiveLogger {

template <typename T>
class CircularQueue {
public:
    CircularQueue() = default;
    explicit CircularQueue(size_t max_size) : capacity(max_size), que_vec(capacity) {}
    CircularQueue(const CircularQueue &circ_q) = default;
    CircularQueue &operator=(const CircularQueue &circ_q) = default;

    // move copy
    CircularQueue(CircularQueue &&other_q) {
        copyMoveable(std::move(other_q));
    }

    CircularQueue &operator=(CircularQueue &&other_q) {
        copyMoveable(std::move(other_q));
        return *this;
    }

    bool enQueue(T &&item) {
        if (isFull()) {
            std::cout << "Failed to enqueue, FULL!\n";
            return false;
        } else {
            size_t idx = (front + size_counter) % capacity;
            que_vec[idx] = std::move(item);
            size_counter++;
            return true;
            // if (capacity > 0) {
            //     size_t idx = (front + size_counter) % capacity;
            //     que_vec[idx] = std::move(item);
            //     size_counter++;
            //     return true;
            // } else {
            //     std::cout << "Wrong setting, capacity less than 0!\n";
            //     return false;
            // }
        }
    }

    bool deQueue() {
        if(isEmpty()) {
            std::cout << "Empty queue\n";
            return false;
        } else {
            front = (front + 1) % capacity;
            size_counter--;
            return true;
        }
    }

    T &getFront() {
        return que_vec[front]; 
    }

    T &getRear() {
        return que_vec[(front + size_counter - 1) % capacity];
    }

    const T &at(size_t idx) {
        assert(idx < getSize());
        return que_vec[(front + idx) % capacity];
    }

    size_t getSize() const {
        return size_counter;
    }

    // void resetCounter() {
    //     size_counter = 0;
    // }
    void clearQueue() {
        front = 0;
        size_counter = 0;
    }

    bool isEmpty() const {
        return size_counter == 0;
        // return front == rear;
    }

    bool isFull() const {
        return size_counter == capacity;
    }




private:
    size_t capacity = 0;
    size_t size_counter = 0;
    std::vector<T> que_vec;
    typename std::vector<T>::size_type front = 0;
    // typename std::vector<T>::size_type rear = 0;

    void copyMoveable(CircularQueue &&other) {
        capacity = other.capacity;
        size_counter = other.size_counter;
        front = other.front;
        // rear = other.rear;
        que_vec = std::move(other.que_vec);

        // disable the data in &other
        other.capacity = 0;
        other.front = 0;
        // other.rear = 0;
        other.size_counter = 0;
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