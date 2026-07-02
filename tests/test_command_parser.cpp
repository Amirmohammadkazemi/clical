#include <gtest/gtest.h>
#include "../src/command_parser.h"
#include "../src/calendar.h"
#include <sstream>

class CommandParserTest : public ::testing::Test {
protected:
    Calendar* calendar;
    CommandParser* parser;
    const std::string testFile = "test_calendar_data.json";

    void SetUp() override {
        std::remove(testFile.c_str());
        calendar = new Calendar(testFile);
        parser = new CommandParser(*calendar);
    }

    void TearDown() override {
        delete parser;
        delete calendar;
        std::remove(testFile.c_str());
    }

    void addTestEvent(const std::string& title, const std::string& startDate,
                      const std::string& startTime, const std::string& endDate,
                      const std::string& endTime) {
        bool conflict = false;
        std::string conflictTitle;
        calendar->addEvent(title, startDate, startTime, endDate, endTime, conflict, conflictTitle);
    }
};

TEST_F(CommandParserTest, ParseAddCommand) {
    std::string cmd = "ADD --title \"Meeting\" --start-date 2025-12-25 --start-time 10:00 --end-date 2025-12-25 --end-time 11:00";
    parser->processCommand(cmd);
    EXPECT_EQ(calendar->eventCount(), 1);
}

TEST_F(CommandParserTest, ParseAddCommandMissingArgs) {
    std::string cmd = "ADD --title \"Meeting\" --start-date 2025-12-25";
    parser->processCommand(cmd);
    EXPECT_EQ(calendar->eventCount(), 0);
}

TEST_F(CommandParserTest, ParseAddRecurringCommand) {
    std::string cmd = "ADD --title \"Daily\" --start-date 2025-12-25 --start-time 10:00 --end-date 2025-12-25 --end-time 11:00 --recurrence daily --interval 2 --recurrence-end 2026-01-01";
    parser->processCommand(cmd);
    EXPECT_EQ(calendar->recurringEventCount(), 1);
}

TEST_F(CommandParserTest, ParseListCommand) {
    addTestEvent("Event1", "2025-12-25", "10:00", "2025-12-25", "11:00");
    EXPECT_NO_FATAL_FAILURE(parser->processCommand("LIST"));
}

TEST_F(CommandParserTest, ParseListCommandWithDate) {
    addTestEvent("Event1", "2025-12-25", "10:00", "2025-12-25", "11:00");
    EXPECT_NO_FATAL_FAILURE(parser->processCommand("LIST --date 2025-12-25"));
}

TEST_F(CommandParserTest, ParseRemoveCommandByIndex) {
    addTestEvent("Event1", "2025-12-25", "10:00", "2025-12-25", "11:00");
    parser->processCommand("REMOVE 1");
    EXPECT_EQ(calendar->eventCount(), 0);
}

TEST_F(CommandParserTest, ParseRemoveCommandByTitle) {
    addTestEvent("Meeting", "2025-12-25", "10:00", "2025-12-25", "11:00");
    parser->processCommand("REMOVE --title \"Meeting\"");
    EXPECT_EQ(calendar->eventCount(), 0);
}

TEST_F(CommandParserTest, ParseEditCommand) {
    addTestEvent("Original", "2025-12-25", "10:00", "2025-12-25", "11:00");
    parser->processCommand("EDIT 1 --title \"Edited\" --start-date 2025-12-26 --start-time 12:00 --end-date 2025-12-26 --end-time 13:00");
    EXPECT_EQ(calendar->eventCount(), 1);
}

TEST_F(CommandParserTest, ParseSearchCommand) {
    addTestEvent("Meeting", "2025-12-25", "10:00", "2025-12-25", "11:00");
    EXPECT_NO_FATAL_FAILURE(parser->processCommand("SEARCH \"Meeting\""));
}

TEST_F(CommandParserTest, ParseHelpCommand) {
    EXPECT_NO_FATAL_FAILURE(parser->processCommand("HELP"));
}

TEST_F(CommandParserTest, UnknownCommand) {
    EXPECT_NO_FATAL_FAILURE(parser->processCommand("UNKNOWN"));
}