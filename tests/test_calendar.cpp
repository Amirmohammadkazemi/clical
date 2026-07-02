#include <gtest/gtest.h>
#include "../src/calendar.h"
#include "../src/utils.h"
#include <chrono>
#include <fstream>
#include <cstdlib>

class CalendarTest : public ::testing::Test {
protected:
    Calendar* calendar;
    const std::string testFile = "test_calendar_data.json";

    void SetUp() override {
        std::remove(testFile.c_str());
        calendar = new Calendar(testFile);
    }

    void TearDown() override {
        delete calendar;
        std::remove(testFile.c_str());
    }

    std::optional<std::string> addEventHelper(const std::string& title,
                                              const std::string& startDate,
                                              const std::string& startTime,
                                              const std::string& endDate,
                                              const std::string& endTime,
                                              bool& conflict,
                                              std::string& conflictTitle) {
        return calendar->addEvent(title, startDate, startTime, endDate, endTime, conflict, conflictTitle);
    }
};

TEST_F(CalendarTest, AddValidEvent) {
    bool conflict = false;
    std::string conflictTitle;
    auto error = addEventHelper("Meeting", "2025-12-25", "10:00", "2025-12-25", "11:00", conflict, conflictTitle);
    EXPECT_FALSE(error.has_value());
    EXPECT_EQ(calendar->eventCount(), 1);
    EXPECT_FALSE(conflict);
}

TEST_F(CalendarTest, AddEventInvalidDate) {
    bool conflict = false;
    std::string conflictTitle;
    auto error = addEventHelper("Meeting", "invalid", "10:00", "2025-12-25", "11:00", conflict, conflictTitle);
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(calendar->eventCount(), 0);
}

TEST_F(CalendarTest, AddEventEndBeforeStart) {
    bool conflict = false;
    std::string conflictTitle;
    auto error = addEventHelper("Meeting", "2025-12-25", "11:00", "2025-12-25", "10:00", conflict, conflictTitle);
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(calendar->eventCount(), 0);
}

TEST_F(CalendarTest, AddEventZeroDuration) {
    bool conflict = false;
    std::string conflictTitle;
    auto error = addEventHelper("Meeting", "2025-12-25", "10:00", "2025-12-25", "10:00", conflict, conflictTitle);
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(calendar->eventCount(), 0);
}

TEST_F(CalendarTest, ExactOverlapWarning) {
    bool conflict1 = false, conflict2 = false;
    std::string conflictTitle1, conflictTitle2;
    addEventHelper("First", "2025-12-25", "10:00", "2025-12-25", "11:00", conflict1, conflictTitle1);
    
    auto error = addEventHelper("Second", "2025-12-25", "10:00", "2025-12-25", "11:00", conflict2, conflictTitle2);
    
    EXPECT_FALSE(error.has_value());
    EXPECT_TRUE(conflict2);
    EXPECT_EQ(conflictTitle2, "First");
    EXPECT_EQ(calendar->eventCount(), 2);
}

TEST_F(CalendarTest, NoOverlapForDifferentTimes) {
    bool conflict1 = false, conflict2 = false;
    std::string conflictTitle1, conflictTitle2;
    addEventHelper("First", "2025-12-25", "10:00", "2025-12-25", "11:00", conflict1, conflictTitle1);
    auto error = addEventHelper("Second", "2025-12-25", "11:00", "2025-12-25", "12:00", conflict2, conflictTitle2);
    EXPECT_FALSE(error.has_value());
    EXPECT_FALSE(conflict2);
    EXPECT_EQ(calendar->eventCount(), 2);
}

TEST_F(CalendarTest, AddRecurringEvent) {
    auto error = calendar->addRecurringEvent("Daily Meeting", "2025-12-25", "10:00", "2025-12-25", "11:00", "daily", 1, "2026-01-01");
    EXPECT_FALSE(error.has_value());
    EXPECT_EQ(calendar->recurringEventCount(), 1);
}

TEST_F(CalendarTest, AddRecurringEventInvalidType) {
    auto error = calendar->addRecurringEvent("Daily Meeting", "2025-12-25", "10:00", "2025-12-25", "11:00", "invalid", 1);
    EXPECT_TRUE(error.has_value());
}

TEST_F(CalendarTest, EditEvent) {
    bool conflict = false;
    std::string conflictTitle;
    addEventHelper("Original", "2025-12-25", "10:00", "2025-12-25", "11:00", conflict, conflictTitle);
    auto error = calendar->editEvent(1, "Edited", "2025-12-26", "12:00", "2025-12-26", "13:00");
    EXPECT_FALSE(error.has_value());
    EXPECT_EQ(calendar->eventCount(), 1);
}

TEST_F(CalendarTest, EditEventInvalidIndex) {
    auto error = calendar->editEvent(999, "New Title");
    EXPECT_TRUE(error.has_value());
}

TEST_F(CalendarTest, EditEventInvalidDate) {
    bool conflict = false;
    std::string conflictTitle;
    addEventHelper("Original", "2025-12-25", "10:00", "2025-12-25", "11:00", conflict, conflictTitle);
    auto error = calendar->editEvent(1, "", "invalid", "12:00", "2025-12-26", "13:00");
    EXPECT_TRUE(error.has_value());
}

TEST_F(CalendarTest, RemoveEventByIndex) {
    bool conflict = false;
    std::string conflictTitle;
    addEventHelper("Event1", "2025-12-25", "10:00", "2025-12-25", "11:00", conflict, conflictTitle);
    addEventHelper("Event2", "2025-12-25", "11:00", "2025-12-25", "12:00", conflict, conflictTitle);
    EXPECT_EQ(calendar->eventCount(), 2);
    auto error = calendar->removeEventByIndex(1);
    EXPECT_FALSE(error.has_value());
    EXPECT_EQ(calendar->eventCount(), 1);
}

TEST_F(CalendarTest, RemoveEventByIndexInvalid) {
    auto error = calendar->removeEventByIndex(1);
    EXPECT_TRUE(error.has_value());
}

TEST_F(CalendarTest, RemoveEventsByTitle) {
    bool conflict = false;
    std::string conflictTitle;
    addEventHelper("Meeting", "2025-12-25", "10:00", "2025-12-25", "11:00", conflict, conflictTitle);
    addEventHelper("Meeting", "2025-12-26", "10:00", "2025-12-26", "11:00", conflict, conflictTitle);
    addEventHelper("Other", "2025-12-27", "10:00", "2025-12-27", "11:00", conflict, conflictTitle);
    EXPECT_EQ(calendar->eventCount(), 3);
    auto error = calendar->removeEventsByTitle("Meeting");
    EXPECT_FALSE(error.has_value());
    EXPECT_EQ(calendar->eventCount(), 1);
}

TEST_F(CalendarTest, RemoveEventsByTitleNotFound) {
    auto error = calendar->removeEventsByTitle("Nonexistent");
    EXPECT_TRUE(error.has_value());
}

TEST_F(CalendarTest, RemoveEventsByDateRange) {
    bool conflict = false;
    std::string conflictTitle;
    addEventHelper("Event1", "2025-12-25", "10:00", "2025-12-25", "11:00", conflict, conflictTitle);
    addEventHelper("Event2", "2025-12-26", "10:00", "2025-12-26", "11:00", conflict, conflictTitle);
    addEventHelper("Event3", "2025-12-27", "10:00", "2025-12-27", "11:00", conflict, conflictTitle);
    EXPECT_EQ(calendar->eventCount(), 3);
    auto error = calendar->removeEventsByDateRange("2025-12-25", "2025-12-26");
    EXPECT_FALSE(error.has_value());
    EXPECT_EQ(calendar->eventCount(), 1);
}

TEST_F(CalendarTest, RemoveEventsByDateRangeNotFound) {
    auto error = calendar->removeEventsByDateRange("2025-12-25", "2025-12-26");
    EXPECT_TRUE(error.has_value());
}

TEST_F(CalendarTest, FindEventsByTitle) {
    bool conflict = false;
    std::string conflictTitle;
    addEventHelper("Meeting", "2025-12-25", "10:00", "2025-12-25", "11:00", conflict, conflictTitle);
    addEventHelper("Meeting", "2025-12-26", "10:00", "2025-12-26", "11:00", conflict, conflictTitle);
    addEventHelper("Other", "2025-12-27", "10:00", "2025-12-27", "11:00", conflict, conflictTitle);
    auto indices = calendar->findEventsByTitle("Meeting");
    EXPECT_EQ(indices.size(), 2);
    EXPECT_EQ(indices[0], 1);
    EXPECT_EQ(indices[1], 2);
}

TEST_F(CalendarTest, FindEventsByTitleNotFound) {
    auto indices = calendar->findEventsByTitle("Nonexistent");
    EXPECT_TRUE(indices.empty());
}

TEST_F(CalendarTest, SortEvents) {
    bool conflict = false;
    std::string conflictTitle;
    addEventHelper("Event1", "2025-12-26", "10:00", "2025-12-26", "11:00", conflict, conflictTitle);
    addEventHelper("Event2", "2025-12-25", "10:00", "2025-12-25", "11:00", conflict, conflictTitle);
    addEventHelper("Event3", "2025-12-27", "10:00", "2025-12-27", "11:00", conflict, conflictTitle);
    calendar->sortEvents();
    auto error = calendar->removeEventByIndex(1);
    EXPECT_FALSE(error.has_value());
    EXPECT_EQ(calendar->eventCount(), 2);
}

TEST_F(CalendarTest, SaveAndLoad) {
    bool conflict = false;
    std::string conflictTitle;
    addEventHelper("Event1", "2025-12-25", "10:00", "2025-12-25", "11:00", conflict, conflictTitle);
    addEventHelper("Event2", "2025-12-26", "10:00", "2025-12-26", "11:00", conflict, conflictTitle);
    calendar->addRecurringEvent("Daily", "2025-12-25", "12:00", "2025-12-25", "13:00", "daily", 1);
    
    std::string err;
    EXPECT_TRUE(calendar->saveToFile(err));
    EXPECT_TRUE(err.empty());
    
    Calendar newCalendar(testFile);
    EXPECT_TRUE(newCalendar.loadFromFile(err));
    EXPECT_TRUE(err.empty());
    EXPECT_EQ(newCalendar.eventCount(), 2);
    EXPECT_EQ(newCalendar.recurringEventCount(), 1);
}

TEST_F(CalendarTest, ListEventsOnDate) {
    bool conflict = false;
    std::string conflictTitle;
    addEventHelper("Event1", "2025-12-25", "10:00", "2025-12-25", "11:00", conflict, conflictTitle);
    addEventHelper("Event2", "2025-12-26", "10:00", "2025-12-26", "11:00", conflict, conflictTitle);
    EXPECT_NO_FATAL_FAILURE(calendar->listEventsOnDate("2025-12-25"));
}

TEST_F(CalendarTest, ListEventsInRange) {
    bool conflict = false;
    std::string conflictTitle;
    addEventHelper("Event1", "2025-12-25", "10:00", "2025-12-25", "11:00", conflict, conflictTitle);
    addEventHelper("Event2", "2025-12-26", "10:00", "2025-12-26", "11:00", conflict, conflictTitle);
    EXPECT_NO_FATAL_FAILURE(calendar->listEventsInRange("2025-12-25", "2025-12-26"));
}