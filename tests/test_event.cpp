#include <gtest/gtest.h>
#include "../src/event.h"
#include "../src/utils.h"
#include <chrono>

TEST(EventTest, Constructor) {
    std::chrono::system_clock::time_point start, end;
    std::string err;
    Utils::parseDateTime("2025-12-25", "10:00", start, err);
    Utils::parseDateTime("2025-12-25", "11:00", end, err);
    Event ev("Meeting", start, end);
    EXPECT_EQ(ev.title, "Meeting");
    EXPECT_EQ(ev.startTime, start);
    EXPECT_EQ(ev.endTime, end);
}

TEST(EventTest, FormatTimePoint) {
    std::chrono::system_clock::time_point tp;
    std::string err;
    Utils::parseDateTime("2025-12-25", "10:30", tp, err);
    Event ev("Test", tp, tp);
    EXPECT_EQ(ev.formatTimePoint(tp), "2025-12-25 10:30");
    EXPECT_EQ(ev.formatTimePoint(tp, "%H:%M"), "10:30");
}

TEST(EventTest, OutputOperator) {
    std::chrono::system_clock::time_point start, end;
    std::string err;
    Utils::parseDateTime("2025-12-25", "10:00", start, err);
    Utils::parseDateTime("2025-12-25", "11:00", end, err);
    Event ev("Meeting", start, end);
    std::stringstream ss;
    ss << ev;
    std::string output = ss.str();
    EXPECT_TRUE(output.find("Meeting") != std::string::npos);
    EXPECT_TRUE(output.find("2025-12-25 10:00") != std::string::npos);
    EXPECT_TRUE(output.find("2025-12-25 11:00") != std::string::npos);
}