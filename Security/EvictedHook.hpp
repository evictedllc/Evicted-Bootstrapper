#include <iostream>
#include <chrono>
#include <ctime>

int main()
{
    auto now = std::chrono::system_clock::now();
    std::time_t time_now = std::chrono::system_clock::to_time_t(now);

    std::cout << "Injection time: " << std::ctime(&time_now) << std::endl;

    // Your hook code here

    return 0;
}
