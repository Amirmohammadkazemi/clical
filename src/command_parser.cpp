#include "command_parser.h"
#include "utils.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cctype>

CommandParser::CommandParser(Calendar& cal) : calendar(cal) {
    commands["add"] = [this](const auto& args) { handleAdd(args); };
    commands["list"] = [this](const auto& args) { handleList(args); };
    commands["remove"] = [this](const auto& args) { handleRemove(args); };
    commands["edit"] = [this](const auto& args) { handleEdit(args); };
    commands["search"] = [this](const auto& args) { handleSearch(args); };
    commands["show"] = [this](const auto& args) { handleShow(args); };
    commands["help"] = [this](const auto& args) { handleHelp(args); };
}

std::vector<std::string> CommandParser::tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    std::string current;
    bool inQuotes = false;
    for (char ch : line) {
        if (ch == '"') {
            if (inQuotes) {
                tokens.push_back(current);
                current.clear();
            }
            inQuotes = !inQuotes;
            continue;
        }
        if (ch == ' ' && !inQuotes) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current += ch;
        }
    }
    if (!current.empty()) tokens.push_back(current);
    return tokens;
}

std::map<std::string, std::string> CommandParser::parseKeyValueArgs(const std::vector<std::string>& args) {
    std::map<std::string, std::string> result;
    for (size_t i = 1; i < args.size(); ++i) {
        const std::string& token = args[i];
        if (token.rfind("--", 0) == 0) {
            if (i + 1 < args.size()) {
                result[token] = args[i + 1];
                i++;
            } else {
                result[token] = "";
            }
        }
    }
    return result;
}

void CommandParser::processCommand(const std::string& line) {
    auto tokens = tokenize(line);
    if (tokens.empty()) return;

    std::string command = tokens[0];
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);

    auto it = commands.find(command);
    if (it != commands.end()) {
        it->second(tokens);
    } else {
        Utils::printError("Unknown command. Type 'help' for available commands.");
    }
}

void CommandParser::handleAdd(const std::vector<std::string>& args) {
    auto params = parseKeyValueArgs(args);
    auto titleIt = params.find("--title");
    auto startDateIt = params.find("--start-date");
    auto startTimeIt = params.find("--start-time");
    auto endDateIt = params.find("--end-date");
    auto endTimeIt = params.find("--end-time");
    auto recIt = params.find("--recurrence");
    auto intervalIt = params.find("--interval");
    auto recEndIt = params.find("--recurrence-end");

    if (titleIt == params.end() || startDateIt == params.end() || startTimeIt == params.end() ||
        endDateIt == params.end() || endTimeIt == params.end()) {
        Utils::printError("Missing required arguments. Usage: ADD --title \"Title\" --start-date YYYY-MM-DD --start-time HH:MM --end-date YYYY-MM-DD --end-time HH:MM [--recurrence daily/weekly/monthly --interval N --recurrence-end YYYY-MM-DD]");
        return;
    }

    std::string title = Utils::stripQuotes(titleIt->second);
    std::string startDate = startDateIt->second;
    std::string startTime = startTimeIt->second;
    std::string endDate = endDateIt->second;
    std::string endTime = endTimeIt->second;

    bool conflictDetected = false;
    std::string conflictTitle;
    std::optional<std::string> error;

    if (recIt != params.end()) {
        std::string recType = recIt->second;
        int interval = 1;
        if (intervalIt != params.end()) {
            try { interval = std::stoi(intervalIt->second); } catch (...) {}
        }
        std::string recEnd = (recEndIt != params.end()) ? recEndIt->second : "";
        error = calendar.addRecurringEvent(title, startDate, startTime, endDate, endTime, recType, interval, recEnd);
    } else {
        error = calendar.addEvent(title, startDate, startTime, endDate, endTime, conflictDetected, conflictTitle);
    }

    if (error) {
        Utils::printError(*error);
        return;
    }

    if (conflictDetected) {
        Utils::printWarning("An event with exactly the same start and end time already exists ('" + conflictTitle + "').");
        if (!Utils::askYesNo("Add anyway?")) {
            calendar.removeEventByIndex(calendar.eventCount());
            return;
        }
    }
    Utils::printSuccess("Event added.");
}

void CommandParser::handleList(const std::vector<std::string>& args) {
    if (args.size() == 1) {
        calendar.listEvents();
        return;
    }
    auto params = parseKeyValueArgs(args);
    auto dateIt = params.find("--date");
    auto rangeIt = params.find("--range");
    if (dateIt != params.end()) {
        calendar.listEventsOnDate(dateIt->second);
    } else if (rangeIt != params.end()) {
        // فرمت --range "YYYY-MM-DD YYYY-MM-DD"
        std::string range = rangeIt->second;
        size_t space = range.find(' ');
        if (space == std::string::npos) {
            Utils::printError("Invalid range format. Use --range \"YYYY-MM-DD YYYY-MM-DD\"");
            return;
        }
        std::string start = range.substr(0, space);
        std::string end = range.substr(space + 1);
        calendar.listEventsInRange(start, end);
    } else {
        Utils::printError("Invalid LIST arguments. Use LIST, LIST --date YYYY-MM-DD, or LIST --range \"YYYY-MM-DD YYYY-MM-DD\"");
    }
}

void CommandParser::handleRemove(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        Utils::printError("REMOVE requires arguments. Use REMOVE <number> or REMOVE --title \"Title\" or REMOVE --range \"YYYY-MM-DD YYYY-MM-DD\"");
        return;
    }
    auto params = parseKeyValueArgs(args);
    auto titleIt = params.find("--title");
    auto rangeIt = params.find("--range");
    if (titleIt != params.end()) {
        std::string title = Utils::stripQuotes(titleIt->second);
        auto error = calendar.removeEventsByTitle(title);
        if (error) Utils::printError(*error);
    } else if (rangeIt != params.end()) {
        std::string range = rangeIt->second;
        size_t space = range.find(' ');
        if (space == std::string::npos) {
            Utils::printError("Invalid range format. Use --range \"YYYY-MM-DD YYYY-MM-DD\"");
            return;
        }
        std::string start = range.substr(0, space);
        std::string end = range.substr(space + 1);
        auto error = calendar.removeEventsByDateRange(start, end);
        if (error) Utils::printError(*error);
    } else {
        // عدد
        try {
            size_t idx = std::stoul(args[1]);
            auto error = calendar.removeEventByIndex(idx);
            if (error) Utils::printError(*error);
        } catch (...) {
            Utils::printError("Invalid argument. Use a number, --title, or --range.");
        }
    }
}

void CommandParser::handleEdit(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        Utils::printError("EDIT requires an index and at least one field to change. Usage: EDIT <index> --title \"New Title\" --start-date YYYY-MM-DD --start-time HH:MM --end-date YYYY-MM-DD --end-time HH:MM");
        return;
    }
    size_t index;
    try {
        index = std::stoul(args[1]);
    } catch (...) {
        Utils::printError("Invalid index.");
        return;
    }
    auto params = parseKeyValueArgs(args);
    std::string newTitle = (params.find("--title") != params.end()) ? Utils::stripQuotes(params["--title"]) : "";
    std::string newStartDate = (params.find("--start-date") != params.end()) ? params["--start-date"] : "";
    std::string newStartTime = (params.find("--start-time") != params.end()) ? params["--start-time"] : "";
    std::string newEndDate = (params.find("--end-date") != params.end()) ? params["--end-date"] : "";
    std::string newEndTime = (params.find("--end-time") != params.end()) ? params["--end-time"] : "";

    auto error = calendar.editEvent(index, newTitle, newStartDate, newStartTime, newEndDate, newEndTime);
    if (error) Utils::printError(*error);
    else Utils::printSuccess("Event edited.");
}

void CommandParser::handleSearch(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        Utils::printError("SEARCH requires a title. Usage: SEARCH \"Title\"");
        return;
    }
    std::string title = Utils::stripQuotes(args[1]);
    auto indices = calendar.findEventsByTitle(title);
    if (indices.empty()) {
        Utils::printInfo("No events with title '" + title + "' found.");
    } else {
        std::cout << "Events with title '" << title << "' found at indices: ";
        for (size_t i : indices) std::cout << i << " ";
        std::cout << std::endl;
    }
}

void CommandParser::handleShow(const std::vector<std::string>& args) {
    handleList(args);
}

void CommandParser::handleHelp(const std::vector<std::string>&) {
    showHelp();
}

void CommandParser::showHelp() const {
    std::cout << Utils::colorize("\nAvailable commands:", "\033[1;36m") << "\n"
              << "  ADD --title \"Title\" --start-date YYYY-MM-DD --start-time HH:MM --end-date YYYY-MM-DD --end-time HH:MM [--recurrence daily/weekly/monthly --interval N --recurrence-end YYYY-MM-DD]\n"
              << "  LIST                          (lists all events)\n"
              << "  LIST --date YYYY-MM-DD        (lists events on a specific date)\n"
              << "  LIST --range \"YYYY-MM-DD YYYY-MM-DD\"  (lists events in range)\n"
              << "  REMOVE <number>               (removes event by list number)\n"
              << "  REMOVE --title \"Title\"      (removes all events with the title)\n"
              << "  REMOVE --range \"YYYY-MM-DD YYYY-MM-DD\"  (removes events in range)\n"
              << "  EDIT <index> --title \"New Title\" --start-date YYYY-MM-DD ...\n"
              << "  SEARCH \"Title\"              (finds event indices by title)\n"
              << "  HELP                         (shows this help)\n"
              << "  EXIT                         (exits the program)\n\n";
}