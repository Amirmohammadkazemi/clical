#include "utils.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <limits>
#include <readline/readline.h>

namespace Utils {

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (std::string::npos == first) return str;
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(first, (last - first + 1));
}

std::string stripQuotes(const std::string& str) {
    if (str.size() >= 2 && str.front() == '"' && str.back() == '"')
        return str.substr(1, str.size() - 2);
    return str;
}

bool askYesNo(const std::string& prompt) {
    std::string answer;
    std::cout << prompt << " (y/n): ";
    char* line = readline(nullptr);
    if (line) {
        answer = trim(line);
        free(line);
    } else {
        return false;
    }
    return (!answer.empty() && (answer[0] == 'y' || answer[0] == 'Y'));
}

const std::string RESET = "\033[0m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string CYAN = "\033[36m";

std::string colorize(const std::string& text, const std::string& colorCode) {
    return colorCode + text + RESET;
}

void printError(const std::string& msg) {
    std::cerr << colorize("Error: ", RED) << msg << std::endl;
}

void printSuccess(const std::string& msg) {
    std::cout << colorize("Success: ", GREEN) << msg << std::endl;
}

void printWarning(const std::string& msg) {
    std::cout << colorize("Warning: ", YELLOW) << msg << std::endl;
}

void printInfo(const std::string& msg) {
    std::cout << colorize("Info: ", BLUE) << msg << std::endl;
}

bool parseDateTime(const std::string& dateStr, const std::string& timeStr,
                   std::chrono::system_clock::time_point& tp,
                   std::string& errorMsg) {
    std::tm t{};
    std::stringstream ss(dateStr + " " + timeStr);
    ss >> std::get_time(&t, "%Y-%m-%d %H:%M");
    if (ss.fail()) {
        errorMsg = "Invalid date/time format. Expected 'YYYY-MM-DD HH:MM' but got '" +
                   dateStr + " " + timeStr + "'.";
        return false;
    }
    if (t.tm_year < 70 || t.tm_year > 137) {
        errorMsg = "Year out of supported range (1970-2037).";
        return false;
    }
    if (t.tm_mon < 0 || t.tm_mon > 11 || t.tm_mday < 1 || t.tm_mday > 31 ||
        t.tm_hour < 0 || t.tm_hour > 23 || t.tm_min < 0 || t.tm_min > 59) {
        errorMsg = "Invalid date/time components.";
        return false;
    }
    t.tm_sec = 0;
    std::time_t tt = std::mktime(&t);
    if (tt == -1) {
        errorMsg = "Cannot convert date/time to system time.";
        return false;
    }
    tp = std::chrono::system_clock::from_time_t(tt);
    return true;
}

std::string tpToString(const std::chrono::system_clock::time_point& tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    char buffer[80];
    if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", std::localtime(&t)))
        return std::string(buffer);
    return "";
}

std::chrono::system_clock::time_point stringToTp(const std::string& str) {
    std::tm t{};
    std::stringstream ss(str);
    ss >> std::get_time(&t, "%Y-%m-%d %H:%M");
    if (ss.fail()) {
        return std::chrono::system_clock::time_point{};
    }
    std::time_t tt = std::mktime(&t);
    if (tt == -1) return std::chrono::system_clock::time_point{};
    return std::chrono::system_clock::from_time_t(tt);
}

bool parseDateOnly(const std::string& dateStr,
                   std::chrono::system_clock::time_point& tp,
                   std::string& errorMsg) {
    std::tm t{};
    std::stringstream ss(dateStr);
    ss >> std::get_time(&t, "%Y-%m-%d");
    if (ss.fail()) {
        errorMsg = "Invalid date format. Expected 'YYYY-MM-DD'.";
        return false;
    }
    t.tm_sec = 0;
    std::time_t tt = std::mktime(&t);
    if (tt == -1) {
        errorMsg = "Cannot convert date to system time.";
        return false;
    }
    tp = std::chrono::system_clock::from_time_t(tt);
    return true;
}

} // namespace Utils