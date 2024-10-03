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
        std::shared_ptr<T> value;

        // next node in the queue
        std::atomic<std::shared_ptr<Node<T>>> next;

        Node(std::shared_ptr<T> val) : value(val) {
            this->next.exchange(std::atomic<std::shared_ptr<Node<T>>>(nullptr));
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
        std::atomic<std::shared_ptr<Node<T>>> head;

        // queue tail pointer
        std::shared_ptr<Node<T>> tail;

        // initialize the queue
        MPSCQueue() {
            auto nodePtr = std::make_shared<Node<T>>(nullptr);
            this->head.exchange(std::atomic<std::shared_ptr<Node<T>>>(nodePtr));
            this->tail = nodePtr;
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
        void push(std::shared_ptr<T> val) {
            auto aNode = std::make_shared<Node<T>>(val);
            auto prev = this->head.exchange(aNode, std::memory_order_acq_rel);
            prev->next.store(aNode, std::memory_order_release);
        }

        // pop a node
        std::shared_ptr<T> pop() {
            std::shared_ptr<T> ret = nullptr;
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
