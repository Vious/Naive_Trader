/* reference: http://www.1024cores.net/home/lock-free-algorithms/queues/non-intrusive-mpsc-node-based-queue */

#ifndef MPSC_QUEUE
#define MPSC_QUEUE

#include <atomic>
#include <iostream>
#include <memory>

namespace naiveTrader
{

namespace mpsc
{

    // queue node
    template <typename T>
    struct Node {
        T* value;

        // next node in the queue
        std::atomic<Node<T>*> next;

        Node(T* val) : value(val) {
            this->next.exchange(std::atomic<Node<T>*>(nullptr));
        }

        // move constructor
        Node(Node &&other) : value(other.value) {
            this->next.exchange(other.next);
        }

    };

    // mspc queue
    template <typename T>
    struct MPSCQueue {
        // head of the queue, atomic pointer
        std::atomic<Node<T>*> head;

        // queue tail pointer
        Node<T>* tail;

        // initialize the queue
        MPSCQueue() {
            auto stub = new Node<T>(nullptr);
            this->head.exchange(std::atomic<Node<T>*>(stub));
            this->tail = stub;
        }

        // move constructor
        MPSCQueue(MPSCQueue &&other) {
            this->tail = std::move(other.tail);
            this->head.exchange(other.head);
            other.tail = nullptr;
        }

        // deconstructor
        ~MPSCQueue() {
            if (tail != nullptr) {
                while(this->pop());
                delete this->tail;
            }
        }

        // clone
        MPSCQueue clone() {
            auto newQ = MPSCQueue<T>();
            newQ.head = this->head;
            newQ.tail = this->tail;
            return std::move(newQ);
        }

        // push function
        void push(T* val) {
            auto aNode = new Node<T>(val);
            auto prev = this->head.exchange(aNode, std::memory_order_acq_rel);
            prev->next.store(aNode, std::memory_order_release);
        }

        // pop a node
        T* pop() {
            T* ret = nullptr;
            auto tail = this->tail;
            auto next = tail->next.load(std::memory_order_acquire);
            if (next != nullptr) {
                delete this->tail;
                this->tail = next;
                ret = next->value;
            }
            return ret;
        }


        // check empty or not
        bool isEmpty() const {
            return this->head.load(std::memory_order_acquire) == tail;
        }


    };


} // namespace mpsc

} // namespace naiveTrader

#endif
