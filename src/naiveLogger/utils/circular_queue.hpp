#pragma once

#include <vector>
#include "common.hpp"

namespace naiveLogger {

template <typename T>
class CircularQueue {
public:
    CircularQueue() = default;
    explicit CircularQueue(size_t max_size) : capacity(max_size + 1), que_vec(capacity) {} 


private:
    size_t capacity = 0;
    size_t size_counter = 0;
    std::vector<T> que_vec;
    typename std::vector<T>::size_type front = 0;
    typename std::vector<T>::size_type rear = 0;

    



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