#include <gtest/gtest.h>
#include "../src/storage.h"
#include "../src/utils.h"
#include <fstream>
#include <cstdlib>

TEST(StorageTest, SaveAndLoadEvents) {
    std::string testFile = "test_storage.json";
    std::remove(testFile.c_str());
    
    Storage storage(testFile);
    std::vector<Event> events;
    std::vector<RecurringEvent> recurring;
    std::string err;
    
    std::chrono::system_clock::time_point start, end;
    Utils::parseDateTime("2025-12-25", "10:00", start, err);
    Utils::parseDateTime("2025-12-25", "11:00", end, err);
    events.emplace_back("Meeting", start, end);
    
    EXPECT_TRUE(storage.save(events, recurring, err));
    EXPECT_TRUE(err.empty());
    
    std::vector<Event> loadedEvents;
    std::vector<RecurringEvent> loadedRecurring;
    EXPECT_TRUE(storage.load(loadedEvents, loadedRecurring, err));
    EXPECT_TRUE(err.empty());
    EXPECT_EQ(loadedEvents.size(), 1);
    EXPECT_EQ(loadedEvents[0].title, "Meeting");
    
    std::remove(testFile.c_str());
}

TEST(StorageTest, SaveAndLoadRecurring) {
    std::string testFile = "test_storage.json";
    std::remove(testFile.c_str());
    
    Storage storage(testFile);
    std::vector<Event> events;
    std::vector<RecurringEvent> recurring;
    std::string err;
    
    std::chrono::system_clock::time_point start, end, recEnd;
    Utils::parseDateTime("2025-12-25", "10:00", start, err);
    Utils::parseDateTime("2025-12-25", "11:00", end, err);
    Utils::parseDateTime("2026-01-01", "00:00", recEnd, err);
    recurring.emplace_back("Daily", start, end, RecurrenceType::Daily, 2, recEnd);
    
    EXPECT_TRUE(storage.save(events, recurring, err));
    EXPECT_TRUE(err.empty());
    
    std::vector<Event> loadedEvents;
    std::vector<RecurringEvent> loadedRecurring;
    EXPECT_TRUE(storage.load(loadedEvents, loadedRecurring, err));
    EXPECT_TRUE(err.empty());
    EXPECT_EQ(loadedRecurring.size(), 1);
    EXPECT_EQ(loadedRecurring[0].title, "Daily");
    EXPECT_EQ(loadedRecurring[0].recurrence, RecurrenceType::Daily);
    EXPECT_EQ(loadedRecurring[0].interval, 2);
    
    std::remove(testFile.c_str());
}

TEST(StorageTest, LoadNonExistentFile) {
    std::string testFile = "nonexistent.json";
    std::remove(testFile.c_str());
    
    Storage storage(testFile);
    std::vector<Event> events;
    std::vector<RecurringEvent> recurring;
    std::string err;
    EXPECT_TRUE(storage.load(events, recurring, err));
    EXPECT_TRUE(err.empty());
}

TEST(StorageTest, LoadInvalidJson) {
    std::string testFile = "invalid.json";
    std::ofstream file(testFile);
    file << "this is not json";
    file.close();
    
    Storage storage(testFile);
    std::vector<Event> events;
    std::vector<RecurringEvent> recurring;
    std::string err;
    EXPECT_FALSE(storage.load(events, recurring, err));
    EXPECT_FALSE(err.empty());
    
    std::remove(testFile.c_str());
}