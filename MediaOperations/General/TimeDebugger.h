#pragma once
#include <chrono>
#include <string>
#include <iostream>

class TimeDebugger
{
public:
    enum TimeUnit
    {
        Seconds,
        Milli,
        Micro,
        Nano
    };

    TimeDebugger() noexcept
        : start_(std::chrono::steady_clock::now()),
        last_(start_), auto_print_(false) {
    }

    TimeDebugger(TimeUnit unit) noexcept
        : start_(std::chrono::steady_clock::now()),
        last_(start_), unit_(unit), auto_print_(true)
    {
        std::cout << "[" << "TimeDebugger" << "] " << "\n";
    }

    ~TimeDebugger() noexcept
    {
        if (auto_print_)
        {
            callElapsedSince("~TimeDebugger");
        }
    }

    /* ---------- 累计耗时 ---------- */
    double elapsedSeconds(const std::string& msg = "") const noexcept
    {
        return elapsed<std::ratio<1, 1>>(msg);
    }
    double elapsedMilli(const std::string& msg = "") const noexcept
    {
        return elapsed<std::milli>(msg);
    }
    double elapsedMicro(const std::string& msg = "") const noexcept
    {
        return elapsed<std::micro>(msg);
    }
    double elapsedNano(const std::string& msg = "") const noexcept
    {
        return elapsed<std::nano>(msg);
    }

    /* ---------- 上次调用至今 ---------- */
    double elapsedSinceSeconds(const std::string& msg = "") noexcept
    {
        return elapsedSince<std::ratio<1, 1>>(msg);
    }
    double elapsedSinceMilli(const std::string& msg = "") noexcept
    {
        return elapsedSince<std::milli>(msg);
    }
    double elapsedSinceMicro(const std::string& msg = "") noexcept
    {
        return elapsedSince<std::micro>(msg);
    }
    double elapsedSinceNano(const std::string& msg = "") noexcept
    {
        return elapsedSince<std::nano>(msg);
    }

    /* 手动重置 */
    void reset() noexcept
    {
        start_ = std::chrono::steady_clock::now();
        last_ = start_;
    }

private:
    using time_point = std::chrono::steady_clock::time_point;
    time_point start_;
    mutable time_point last_;

    TimeUnit unit_;
    bool auto_print_;

    void callElapsedSince(const std::string& msg) noexcept
    {
        switch (unit_)
        {
        case TimeUnit::Seconds:
            elapsedSinceSeconds(msg);
            break;
        case TimeUnit::Milli:
            elapsedSinceMilli(msg);
            break;
        case TimeUnit::Micro:
            elapsedSinceMicro(msg);
            break;
        case TimeUnit::Nano:
            elapsedSinceNano(msg);
            break;
        }
    }

    /* 通用累计 */
    template <class Ratio>
    double elapsed(const std::string& msg) const noexcept
    {
        auto now = std::chrono::steady_clock::now();
        double val = std::chrono::duration<double, Ratio>(now - start_).count();
        if (!msg.empty())
            printMsg(msg, val, Ratio{});
        last_ = now;
        return val;
    }

    /* 通用片段 */
    template <class Ratio>
    double elapsedSince(const std::string& msg) noexcept
    {
        auto now = std::chrono::steady_clock::now();
        double val = std::chrono::duration<double, Ratio>(now - last_).count();
        printMsg(msg, val, Ratio{});
        last_ = now;
        return val;
    }

    /* 打印辅助：根据 Ratio 自动拼单位 */
    static void printMsg(const std::string& msg, double val, std::ratio<1, 1>)
    {
        std::cout << "[" << msg << "] " << val << " s\n";
    }
    static void printMsg(const std::string& msg, double val, std::milli)
    {
        std::cout << "[" << msg << "] " << val << " ms\n";
    }
    static void printMsg(const std::string& msg, double val, std::micro)
    {
        std::cout << "[" << msg << "] " << val << " μs\n";
    }
    static void printMsg(const std::string& msg, double val, std::nano)
    {
        std::cout << "[" << msg << "] " << val << " ns\n";
    }
};
