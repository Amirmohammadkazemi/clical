#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <chrono>
#include <ostream>

struct Event {
    std::string title;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;

    Event() = default;
    Event(std::string t, 
          std::chrono::system_clock::time_point start,
          std::chrono::system_clock::time_point end)
        : title(std::move(t)), startTime(start), endTime(end) {}

    std::string formatTimePoint(const std::chrono::system_clock::time_point& tp,
                                const std::string& format = "%Y-%m-%d %H:%M") const;

    friend std::ostream& operator<<(std::ostream& os, const Event& event);
};

#endif