#include <iostream>
#include "../utils/threadPool.hpp"


int time_delay_add(int a, int b) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return a + b;
}

int main() 
{
    SimpleThreadPool stpool(10);
    int taskNum = 30;
    std::vector<std::future<int>> results(taskNum);

    for (size_t i = 0; i < taskNum; i++) {
        results[i] = stpool.submitTask(time_delay_add, i, i + 5);
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "Main thread continue...\n" ;

    std::cout << "Running tasks from thread pool..\n";

    for (size_t i = 0; i < taskNum; i++) {
        std::cout << "result[" << i << "]:" << results[i].get() << std::endl; 
    }

    std::cout << "Ending everthing here..\n";

    return 0;
}