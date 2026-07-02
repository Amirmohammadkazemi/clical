#include "calendar.h"
#include "storage.h"
#include "utils.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <set>

Calendar::Calendar() : storageFile("calendar_data.json") {
    std::string err;
    if (!loadFromFile(err) && !err.empty()) {
        Utils::printWarning("Could not load storage: " + err);
    }
}

Calendar::Calendar(const std::string& storageFilePath) : storageFile(storageFilePath) {
    std::string err;
    if (!loadFromFile(err) && !err.empty()) {
        Utils::printWarning("Could not load storage: " + err);
    }
}

Calendar::~Calendar() {
    std::string err;
    if (!saveToFile(err)) {
        Utils::printWarning("Failed to save calendar: " + err);
    }
}

bool Calendar::hasExactOverlap(const std::chrono::system_clock::time_point& start,
                               const std::chrono::system_clock::time_point& end,
                               std::string& conflictingTitle) const {
    for (const auto& ev : events) {
        if (ev.startTime == start && ev.endTime == end) {
            conflictingTitle = ev.title;
            return true;
        }
    }
    return false;
}

std::optional<std::string> Calendar::addEvent(const std::string& title,
                                              const std::string& startDateStr,
                                              const std::string& startTimeStr,
                                              const std::string& endDateStr,
                                              const std::string& endTimeStr,
                                              bool& conflictDetected,
                                              std::string& conflictTitle) {
    std::chrono::system_clock::time_point start, end;
    std::string errorMsg;
    if (!Utils::parseDateTime(startDateStr, startTimeStr, start, errorMsg))
        return errorMsg;
    if (!Utils::parseDateTime(endDateStr, endTimeStr, end, errorMsg))
        return errorMsg;

    if (start > end) {
        return "Start time must be before end time.";
    }
    if (start == end) {
        return "Event duration cannot be zero.";
    }

    conflictDetected = false;
    if (hasExactOverlap(start, end, conflictTitle)) {
        conflictDetected = true;
    }

    events.emplace_back(title, start, end);
    sortEvents();
    return std::nullopt;
}

std::optional<std::string> Calendar::addRecurringEvent(const std::string& title,
                                                       const std::string& startDateStr,
                                                       const std::string& startTimeStr,
                                                       const std::string& endDateStr,
                                                       const std::string& endTimeStr,
                                                       const std::string& recurrenceType,
                                                       int interval,
                                                       const std::string& recurrenceEndDateStr) {
    std::chrono::system_clock::time_point start, end, recEnd;
    std::string errorMsg;
    if (!Utils::parseDateTime(startDateStr, startTimeStr, start, errorMsg))
        return errorMsg;
    if (!Utils::parseDateTime(endDateStr, endTimeStr, end, errorMsg))
        return errorMsg;
    if (!recurrenceEndDateStr.empty()) {
        std::string dummyTime = "00:00";
        if (!Utils::parseDateTime(recurrenceEndDateStr, dummyTime, recEnd, errorMsg))
            return errorMsg;
    }

    if (start > end) {
        return "Start time must be before end time.";
    }
    if (start == end) {
        return "Event duration cannot be zero.";
    }

    RecurrenceType rt = RecurrenceType::None;
    if (recurrenceType == "daily") rt = RecurrenceType::Daily;
    else if (recurrenceType == "weekly") rt = RecurrenceType::Weekly;
    else if (recurrenceType == "monthly") rt = RecurrenceType::Monthly;
    else return "Invalid recurrence type. Use 'daily', 'weekly', or 'monthly'.";

    recurringEvents.emplace_back(title, start, end, rt, interval, recEnd);
    sortEvents();
    return std::nullopt;
}

std::optional<std::string> Calendar::editEvent(size_t index,
                                               const std::string& newTitle,
                                               const std::string& newStartDate,
                                               const std::string& newStartTime,
                                               const std::string& newEndDate,
                                               const std::string& newEndTime) {
    if (index < 1 || index > events.size()) {
        return "Invalid event number. Valid range is 1 to " + std::to_string(events.size()) + ".";
    }
    auto& ev = events[index - 1];
    if (!newTitle.empty()) ev.title = newTitle;
    if (!newStartDate.empty() && !newStartTime.empty()) {
        std::chrono::system_clock::time_point tp;
        std::string err;
        if (!Utils::parseDateTime(newStartDate, newStartTime, tp, err))
            return err;
        ev.startTime = tp;
    }
    if (!newEndDate.empty() && !newEndTime.empty()) {
        std::chrono::system_clock::time_point tp;
        std::string err;
        if (!Utils::parseDateTime(newEndDate, newEndTime, tp, err))
            return err;
        ev.endTime = tp;
    }
    if (ev.startTime > ev.endTime) {
        return "Start time must be before end time.";
    }
    if (ev.startTime == ev.endTime) {
        return "Event duration cannot be zero.";
    }
    sortEvents();
    return std::nullopt;
}

void Calendar::listEvents() const {
    if (events.empty() && recurringEvents.empty()) {
        Utils::printInfo("No events found.");
        return;
    }
    int counter = 1;
    for (const auto& ev : events) {
        std::cout << Utils::colorize(std::to_string(counter) + ".", "\033[1;33m") << "\n";
        std::cout << ev << "\n\n";
        counter++;
    }
    for (const auto& rev : recurringEvents) {
        std::cout << Utils::colorize(std::to_string(counter) + ".", "\033[1;33m") << "\n";
        std::cout << rev << "\n\n";
        counter++;
    }
}

void Calendar::listEventsOnDate(const std::string& dateStr) const {
    std::chrono::system_clock::time_point date;
    std::string err;
    if (!Utils::parseDateOnly(dateStr, date, err)) {
        Utils::printError(err);
        return;
    }
    // بازه یک روز: از 00:00 تا 23:59
    auto startOfDay = date;
    auto endOfDay = date + std::chrono::hours(24) - std::chrono::seconds(1);
    listEventsInRange(Utils::tpToString(startOfDay), Utils::tpToString(endOfDay));
}

void Calendar::listEventsInRange(const std::string& startDateStr, const std::string& endDateStr) const {
    std::chrono::system_clock::time_point start, end;
    std::string err;
    // برای سادگی، زمان را 00:00 و 23:59 در نظر می‌گیریم
    std::string startFull = startDateStr + " 00:00";
    std::string endFull = endDateStr + " 23:59";
    if (!Utils::parseDateTime(startDateStr, "00:00", start, err) ||
        !Utils::parseDateTime(endDateStr, "23:59", end, err)) {
        Utils::printError(err);
        return;
    }

    auto occurrences = generateAllOccurrences(start, end);
    if (occurrences.empty()) {
        Utils::printInfo("No events in this range.");
        return;
    }
    int counter = 1;
    for (const auto& ev : occurrences) {
        std::cout << Utils::colorize(std::to_string(counter) + ".", "\033[1;33m") << "\n";
        std::cout << ev << "\n\n";
        counter++;
    }
}

std::optional<std::string> Calendar::removeEventByIndex(size_t index) {
    if (index < 1 || index > events.size()) {
        return "Invalid event number. Valid range is 1 to " + std::to_string(events.size()) + ".";
    }
    std::string removedTitle = events[index - 1].title;
    events.erase(events.begin() + (index - 1));
    Utils::printSuccess("Event '" + removedTitle + "' removed.");
    return std::nullopt;
}

std::optional<std::string> Calendar::removeEventsByTitle(const std::string& title) {
    auto it = std::remove_if(events.begin(), events.end(),
                             [&title](const Event& e) { return e.title == title; });
    if (it == events.end()) {
        return "No event with title '" + title + "' found.";
    }
    events.erase(it, events.end());
    Utils::printSuccess("All events with title '" + title + "' removed.");
    return std::nullopt;
}

std::optional<std::string> Calendar::removeEventsByDateRange(const std::string& startDateStr,
                                                             const std::string& endDateStr) {
    std::chrono::system_clock::time_point start, end;
    std::string err;
    if (!Utils::parseDateTime(startDateStr, "00:00", start, err) ||
        !Utils::parseDateTime(endDateStr, "23:59", end, err)) {
        return err;
    }
    auto it = std::remove_if(events.begin(), events.end(),
                             [&](const Event& e) {
                                 return e.startTime >= start && e.endTime <= end;
                             });
    if (it == events.end()) {
        return "No events found in this range.";
    }
    events.erase(it, events.end());
    Utils::printSuccess("Events in range removed.");
    return std::nullopt;
}

std::vector<size_t> Calendar::findEventsByTitle(const std::string& title) const {
    std::vector<size_t> indices;
    for (size_t i = 0; i < events.size(); ++i) {
        if (events[i].title == title)
            indices.push_back(i + 1);
    }
    return indices;
}

void Calendar::sortEvents() {
    std::sort(events.begin(), events.end(),
              [](const Event& a, const Event& b) {
                  return a.startTime < b.startTime;
              });
    std::sort(recurringEvents.begin(), recurringEvents.end(),
              [](const RecurringEvent& a, const RecurringEvent& b) {
                  return a.startTime < b.startTime;
              });
}

bool Calendar::saveToFile(std::string& errorMsg) {
    Storage storage(storageFile);
    return storage.save(events, recurringEvents, errorMsg);
}

bool Calendar::loadFromFile(std::string& errorMsg) {
    Storage storage(storageFile);
    return storage.load(events, recurringEvents, errorMsg);
}

size_t Calendar::eventCount() const {
    return events.size();
}

size_t Calendar::recurringEventCount() const {
    return recurringEvents.size();
}

std::vector<Event> Calendar::generateAllOccurrences(const std::chrono::system_clock::time_point& start,
                                                    const std::chrono::system_clock::time_point& end) const {
    std::vector<Event> result;

    for (const auto& ev : events) {
        if (ev.startTime >= start && ev.endTime <= end) {
            result.push_back(ev);
        }
    }

    for (const auto& rev : recurringEvents) {
        auto occs = rev.generateOccurrences(end);
        for (const auto& occ : occs) {
            if (occ.startTime >= start && occ.endTime <= end) {
                result.push_back(occ);
            }
        }
    }
    std::sort(result.begin(), result.end(),
              [](const Event& a, const Event& b) { return a.startTime < b.startTime; });
    return result;
}