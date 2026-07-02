#include <gtest/gtest.h>
#include "../src/utils.h"
#include <chrono>

TEST(UtilsTest, Trim) {
    EXPECT_EQ(Utils::trim("  hello  "), "hello");
    EXPECT_EQ(Utils::trim("\t\n hello \n\t"), "hello");
    EXPECT_EQ(Utils::trim(""), "");
    EXPECT_EQ(Utils::trim("no spaces"), "no spaces");
}

TEST(UtilsTest, StripQuotes) {
    EXPECT_EQ(Utils::stripQuotes("\"hello\""), "hello");
    EXPECT_EQ(Utils::stripQuotes("hello"), "hello");
    EXPECT_EQ(Utils::stripQuotes("\"hello"), "\"hello");
    EXPECT_EQ(Utils::stripQuotes(""), "");
}

TEST(UtilsTest, ParseDateTimeValid) {
    std::chrono::system_clock::time_point tp;
    std::string err;
    EXPECT_TRUE(Utils::parseDateTime("2025-12-25", "10:30", tp, err));
    EXPECT_TRUE(err.empty());
    
    auto tpStr = Utils::tpToString(tp);
    EXPECT_EQ(tpStr, "2025-12-25 10:30");
}

TEST(UtilsTest, ParseDateTimeInvalidFormat) {
    std::chrono::system_clock::time_point tp;
    std::string err;
    EXPECT_FALSE(Utils::parseDateTime("2025/12/25", "10:30", tp, err));
    EXPECT_FALSE(err.empty());
}

TEST(UtilsTest, ParseDateTimeInvalidYear) {
    std::chrono::system_clock::time_point tp;
    std::string err;
    EXPECT_FALSE(Utils::parseDateTime("2050-12-25", "10:30", tp, err));
    EXPECT_FALSE(err.empty());
}

TEST(UtilsTest, ParseDateTimeInvalidHour) {
    std::chrono::system_clock::time_point tp;
    std::string err;
    EXPECT_FALSE(Utils::parseDateTime("2025-12-25", "25:30", tp, err));
    EXPECT_FALSE(err.empty());
}

TEST(UtilsTest, ParseDateOnlyValid) {
    std::chrono::system_clock::time_point tp;
    std::string err;
    EXPECT_TRUE(Utils::parseDateOnly("2025-12-25", tp, err));
    EXPECT_TRUE(err.empty());
    auto tpStr = Utils::tpToString(tp);
    EXPECT_EQ(tpStr.substr(0, 10), "2025-12-25");
}

TEST(UtilsTest, ParseDateOnlyInvalid) {
    std::chrono::system_clock::time_point tp;
    std::string err;
    EXPECT_FALSE(Utils::parseDateOnly("2025/12/25", tp, err));
    EXPECT_FALSE(err.empty());
}

TEST(UtilsTest, TpToString) {
    std::chrono::system_clock::time_point tp;
    std::string err;
    Utils::parseDateTime("2025-12-25", "10:30", tp, err);
    EXPECT_EQ(Utils::tpToString(tp), "2025-12-25 10:30");
}

TEST(UtilsTest, StringToTp) {
    auto tp = Utils::stringToTp("2025-12-25 10:30");
    EXPECT_NE(tp, std::chrono::system_clock::time_point{});
    EXPECT_EQ(Utils::tpToString(tp), "2025-12-25 10:30");
}

TEST(UtilsTest, StringToTpInvalid) {
    auto tp = Utils::stringToTp("invalid");
    EXPECT_EQ(tp, std::chrono::system_clock::time_point{});
}