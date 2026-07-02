#ifndef RECURRING_EVENT_H
#define RECURRING_EVENT_H

#include "event.h"
#include <string>
#include <vector>

enum class RecurrenceType { Daily, Weekly, Monthly, None };

class RecurringEvent : public Event {
public:
    RecurrenceType recurrence;
    int interval;
    std::chrono::system_clock::time_point recurrenceEnd;

    RecurringEvent() = default;
    RecurringEvent(std::string t,
                   std::chrono::system_clock::time_point start,
                   std::chrono::system_clock::time_point end,
                   RecurrenceType rec = RecurrenceType::None,
                   int inter = 1,
                   std::chrono::system_clock::time_point recEnd = {})
        : Event(std::move(t), start, end), recurrence(rec), interval(inter), recurrenceEnd(recEnd) {}

    std::vector<Event> generateOccurrences(const std::chrono::system_clock::time_point& until) const;

    friend std::ostream& operator<<(std::ostream& os, const RecurringEvent& re);
};

#endif