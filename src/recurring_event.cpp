#include "recurring_event.h"
#include <vector>
#include <chrono>

std::vector<Event> RecurringEvent::generateOccurrences(const std::chrono::system_clock::time_point& until) const {
    std::vector<Event> result;
    if (recurrence == RecurrenceType::None) {
        if (startTime <= until) result.push_back(*this);
        return result;
    }

    auto currentStart = startTime;
    auto currentEnd = endTime;
    const auto duration = endTime - startTime;

    while (currentStart <= until && (recurrenceEnd == std::chrono::system_clock::time_point{} || currentStart <= recurrenceEnd)) {
        result.emplace_back(title, currentStart, currentEnd);
        switch (recurrence) {
            case RecurrenceType::Daily:
                currentStart += std::chrono::hours(24 * interval);
                break;
            case RecurrenceType::Weekly:
                currentStart += std::chrono::hours(24 * 7 * interval);
                break;
            case RecurrenceType::Monthly:
                currentStart += std::chrono::hours(24 * 30 * interval);
                break;
            default: break;
        }
        currentEnd = currentStart + duration;
    }
    return result;
}

std::ostream& operator<<(std::ostream& os, const RecurringEvent& re) {
    os << static_cast<const Event&>(re);
    if (re.recurrence != RecurrenceType::None) {
        os << "\nRecurrence: ";
        switch (re.recurrence) {
            case RecurrenceType::Daily: os << "Daily"; break;
            case RecurrenceType::Weekly: os << "Weekly"; break;
            case RecurrenceType::Monthly: os << "Monthly"; break;
            default: break;
        }
        os << " (every " << re.interval << ")";
        if (re.recurrenceEnd != std::chrono::system_clock::time_point{}) {
            os << " until " << re.formatTimePoint(re.recurrenceEnd);
        }
    }
    return os;
}