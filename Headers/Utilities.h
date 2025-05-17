 // it tracks when an event happens to log it 
#pragma once
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <ctime>

inline std::string getCurrentTimestamp() {
    // gets the current time from real-world
    auto now = std::chrono::system_clock::now();
    // type conversion
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm tm_now;
    // splitting into year, month, date, hour, min, sec
#ifdef _WIN32
    localtime_s(&tm_now, &time_t_now);
#else
    localtime_r(&time_t_now, &tm_now);  // thread-safe
#endif

    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(&tm_now, "%Y-%m-%dT%H:%M:%S");
    oss << '.' << std::setw(3) << std::setfill('0') << milliseconds.count();

    return oss.str();
}
