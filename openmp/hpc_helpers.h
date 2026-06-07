#pragma once

#include <chrono>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

// Usage:
//   TIMERSTART(label)
//   ... code to measure ...
//   TIMERSTOP(label)
//
// Prints elapsed time in milliseconds to stderr.

// Thread-safe logging to stderr.
//
// Usage (variadic, works with any << -streamable types):
//   LOG("thread", omp_get_thread_num(), "processed", n, "items");
//
// Each call is a single atomic write — no interleaving between threads.

namespace hpc {

inline std::mutex& log_mutex() {
    static std::mutex m;
    return m;
}

template <typename... Args>
inline void log(Args&&... args) {
    std::ostringstream oss;
    (oss << ... << args);
    oss << '\n';
    std::lock_guard<std::mutex> lock(log_mutex());
    std::cerr << oss.str();
}

} // namespace hpc

#define LOG(...) hpc::log(__VA_ARGS__)

// ---------------------------------------------------------------------------

#define TIMERSTART(label)                                                      \
    auto _timer_##label##_start = std::chrono::high_resolution_clock::now();

#define TIMERSTOP(label)                                                       \
    do {                                                                       \
        auto _timer_##label##_stop = std::chrono::high_resolution_clock::now(); \
        double _timer_##label##_ms =                                           \
            std::chrono::duration<double, std::milli>(                         \
                _timer_##label##_stop - _timer_##label##_start).count();       \
        std::cerr << #label << ": " << _timer_##label##_ms << " ms\n";        \
    } while (0)
