#ifndef CALENDAR_H
#define CALENDAR_H

#include <vector>
#include <optional>
#include <string>
#include "event.h"
#include "recurring_event.h"

class Calendar {
public:
    Calendar();
    explicit Calendar(const std::string& storageFilePath);
    ~Calendar();

    std::optional<std::string> addEvent(const std::string& title,
                                        const std::string& startDateStr,
                                        const std::string& startTimeStr,
                                        const std::string& endDateStr,
                                        const std::string& endTimeStr,
                                        bool& conflictDetected,
                                        std::string& conflictTitle);

    std::optional<std::string> addRecurringEvent(const std::string& title,
                                                 const std::string& startDateStr,
                                                 const std::string& startTimeStr,
                                                 const std::string& endDateStr,
                                                 const std::string& endTimeStr,
                                                 const std::string& recurrenceType,
                                                 int interval,
                                                 const std::string& recurrenceEndDateStr = "");

    std::optional<std::string> editEvent(size_t index,
                                         const std::string& newTitle = "",
                                         const std::string& newStartDate = "",
                                         const std::string& newStartTime = "",
                                         const std::string& newEndDate = "",
                                         const std::string& newEndTime = "");

    void listEvents() const;
    void listEventsOnDate(const std::string& dateStr) const;
    void listEventsInRange(const std::string& startDateStr, const std::string& endDateStr) const;

    std::optional<std::string> removeEventByIndex(size_t index);
    std::optional<std::string> removeEventsByTitle(const std::string& title);
    std::optional<std::string> removeEventsByDateRange(const std::string& startDateStr,
                                                       const std::string& endDateStr);

    std::vector<size_t> findEventsByTitle(const std::string& title) const;

    void sortEvents();

    bool saveToFile(std::string& errorMsg);
    bool loadFromFile(std::string& errorMsg);

    size_t eventCount() const;
    size_t recurringEventCount() const;

private:
    std::vector<Event> events;
    std::vector<RecurringEvent> recurringEvents;
    std::string storageFile;

    bool hasExactOverlap(const std::chrono::system_clock::time_point& start,
                         const std::chrono::system_clock::time_point& end,
                         std::string& conflictingTitle) const;

    std::vector<Event> generateAllOccurrences(const std::chrono::system_clock::time_point& start,
                                              const std::chrono::system_clock::time_point& end) const;
};

#endif