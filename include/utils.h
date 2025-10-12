#pragma once

#include <chrono>

double get_time_in_ms()
{
    using namespace std::chrono;
    using double_ms = duration<double, std::chrono::milliseconds::period>;

    auto now = system_clock::now().time_since_epoch();
    double now_ms = duration_cast<double_ms>(now).count();

    return now_ms;
}
