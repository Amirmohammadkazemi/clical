#include <gtest/gtest.h>
#include "../src/recurring_event.h"
#include "../src/utils.h"
#include <chrono>

TEST(RecurringEventTest, GenerateDailyOccurrences) {
    std::chrono::system_clock::time_point start, end;
    std::string err;
    Utils::parseDateTime("2025-12-25", "10:00", start, err);
    Utils::parseDateTime("2025-12-25", "11:00", end, err);
    RecurringEvent rev("Daily Meeting", start, end, RecurrenceType::Daily, 1);
    
    std::chrono::system_clock::time_point until;
    Utils::parseDateTime("2025-12-29", "00:00", until, err);  // ← اصلاح شده
    auto occs = rev.generateOccurrences(until);
    EXPECT_EQ(occs.size(), 4); // 25, 26, 27, 28
    EXPECT_EQ(Utils::tpToString(occs[0].startTime), "2025-12-25 10:00");
    EXPECT_EQ(Utils::tpToString(occs[3].startTime), "2025-12-28 10:00");
}

TEST(RecurringEventTest, GenerateWeeklyOccurrences) {
    std::chrono::system_clock::time_point start, end;
    std::string err;
    Utils::parseDateTime("2025-12-25", "10:00", start, err);
    Utils::parseDateTime("2025-12-25", "11:00", end, err);
    RecurringEvent rev("Weekly Meeting", start, end, RecurrenceType::Weekly, 1);
    
    std::chrono::system_clock::time_point until;
    Utils::parseDateTime("2026-01-10", "00:00", until, err);
    auto occs = rev.generateOccurrences(until);
    EXPECT_EQ(occs.size(), 3); // 25 Dec, 1 Jan, 8 Jan
    EXPECT_EQ(Utils::tpToString(occs[0].startTime), "2025-12-25 10:00");
    EXPECT_EQ(Utils::tpToString(occs[1].startTime), "2026-01-01 10:00");
}

TEST(RecurringEventTest, GenerateMonthlyOccurrences) {
    std::chrono::system_clock::time_point start, end;
    std::string err;
    Utils::parseDateTime("2025-12-25", "10:00", start, err);
    Utils::parseDateTime("2025-12-25", "11:00", end, err);
    RecurringEvent rev("Monthly Meeting", start, end, RecurrenceType::Monthly, 1);
    
    std::chrono::system_clock::time_point until;
    Utils::parseDateTime("2026-03-01", "00:00", until, err);
    auto occs = rev.generateOccurrences(until);
    EXPECT_EQ(occs.size(), 3); // Dec, Jan, Feb
}

TEST(RecurringEventTest, GenerateWithEndDate) {
    std::chrono::system_clock::time_point start, end, recEnd;
    std::string err;
    Utils::parseDateTime("2025-12-25", "10:00", start, err);
    Utils::parseDateTime("2025-12-25", "11:00", end, err);
    // ↓ اصلاح: تاریخ پایان را یک روز جلوتر ببرید
    Utils::parseDateTime("2026-01-02", "00:00", recEnd, err);
    RecurringEvent rev("Daily Meeting", start, end, RecurrenceType::Daily, 1, recEnd);
    
    std::chrono::system_clock::time_point until;
    Utils::parseDateTime("2026-01-02", "00:00", until, err);  // همین مقدار کافی است
    auto occs = rev.generateOccurrences(until);
    EXPECT_EQ(occs.size(), 8);  // ۲۵ تا ۱ ژانویه = ۸ روز
}

TEST(RecurringEventTest, NoRecurrence) {
    std::chrono::system_clock::time_point start, end;
    std::string err;
    Utils::parseDateTime("2025-12-25", "10:00", start, err);
    Utils::parseDateTime("2025-12-25", "11:00", end, err);
    RecurringEvent rev("One Time", start, end, RecurrenceType::None);
    
    std::chrono::system_clock::time_point until;
    Utils::parseDateTime("2025-12-26", "00:00", until, err);
    auto occs = rev.generateOccurrences(until);
    EXPECT_EQ(occs.size(), 1);
}

TEST(RecurringEventTest, OutputOperator) {
    std::chrono::system_clock::time_point start, end;
    std::string err;
    Utils::parseDateTime("2025-12-25", "10:00", start, err);
    Utils::parseDateTime("2025-12-25", "11:00", end, err);
    RecurringEvent rev("Daily Meeting", start, end, RecurrenceType::Daily, 2);
    std::stringstream ss;
    ss << rev;
    std::string output = ss.str();
    EXPECT_TRUE(output.find("Daily Meeting") != std::string::npos);
    EXPECT_TRUE(output.find("Daily") != std::string::npos);
    EXPECT_TRUE(output.find("every 2") != std::string::npos);
}