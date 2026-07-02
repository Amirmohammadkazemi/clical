#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <chrono>
#include <optional>

namespace Utils {
    std::string trim(const std::string& str);
    std::string stripQuotes(const std::string& str);
    bool askYesNo(const std::string& prompt);
    std::string colorize(const std::string& text, const std::string& colorCode);
    void printError(const std::string& msg);
    void printSuccess(const std::string& msg);
    void printWarning(const std::string& msg);
    void printInfo(const std::string& msg);

    bool parseDateTime(const std::string& dateStr, const std::string& timeStr,
                       std::chrono::system_clock::time_point& tp,
                       std::string& errorMsg);

    std::string tpToString(const std::chrono::system_clock::time_point& tp);
    std::chrono::system_clock::time_point stringToTp(const std::string& str);
    bool parseDateOnly(const std::string& dateStr,
                       std::chrono::system_clock::time_point& tp,
                       std::string& errorMsg);
}

#endif