#include "event.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

std::string Event::formatTimePoint(const std::chrono::system_clock::time_point& tp,
                                   const std::string& format) const {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    char buffer[80];
    if (std::strftime(buffer, sizeof(buffer), format.c_str(), std::localtime(&t)))
        return std::string(buffer);
    return "Invalid Date";
}

std::ostream& operator<<(std::ostream& os, const Event& event) {
    os << "Title: " << event.title << "\n"
       << "Start: " << event.formatTimePoint(event.startTime) << "\n"
       << "End:   " << event.formatTimePoint(event.endTime);
    return os;
}