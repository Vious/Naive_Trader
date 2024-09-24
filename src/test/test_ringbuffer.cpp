#include <iostream>
#include <chrono>
#include <thread>
#include <random>

#include "../utils/ringbuffer.hpp"
#include "../include/market_api.h"
struct MarketData
{
    /* data */
    int64_t id;
    double lastPrice;
    double presettlementPrice;
    double curPrice;
    int64_t sendTime;
    int bidVolume;
};

jnk0le::Ringbuffer<MarketData, 256> mdBuffer;

void writeThread() {
    // int i = 0;
    while(true) {
        MarketData md;
        md.curPrice = (float)(rand()) / (float)(rand());
        md.sendTime = std::chrono::steady_clock::now().time_since_epoch().count();
        mdBuffer.insert(md);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

}

void readThread() {
    MarketData mm;
    MarketData *mdPtr = &mm;
    while(true) {
        if (mdBuffer.remove(mdPtr)) {
            auto rdTime = std::chrono::steady_clock::now();
            std::chrono::steady_clock::time_point readTime{std::chrono::steady_clock::time_point(std::chrono::nanoseconds(mdPtr->sendTime))};
            std::cout << "Have read : " << mdPtr->curPrice << "\t" << std::chrono::duration_cast<std::chrono::nanoseconds>(rdTime - readTime).count() << "[ns]" << std::endl;
        }
    }
}

int main()
{
    std::thread sendData(writeThread);
    std::thread receiver(readThread);

    sendData.join();
    receiver.join();


    return 0;
}
