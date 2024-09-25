#include <iostream>
#include <chrono>
#include <thread>
#include <random>

#include "../utils/string_helper.hpp"

int main()
{
    std::string str1 = "abdsasdsa+-dasdasdsadasjkl+-dsadklasjdklasdjsakld+-";
    std::string str2 = "abdsasdsa+-dasdasdsadasjkl+-dsadklasjdklasdjsakld+-fdsjkalfdsjlk";

    auto tokens1 = naiveTrader::split(str1, "-");
    for (auto str : tokens1) {
        std::cout << str << " ";
    }
    std::cout << std::endl;

    auto tokens2 = naiveTrader::split(str2, "+-");
    for (auto str : tokens2) {
        std::cout << str << " ";
    }
    std::cout << std::endl;

    std::cout << naiveTrader::toString(321.31232132132134732892374893) << std::endl;

    return 0;
}