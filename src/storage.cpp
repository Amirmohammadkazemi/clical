#include "storage.h"
#include "utils.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

Storage::Storage(const std::string& filePath) : filePath(filePath) {}

bool Storage::save(const std::vector<Event>& events,
                   const std::vector<RecurringEvent>& recurring,
                   std::string& errorMsg) {
    try {
        json j;
        for (const auto& ev : events) {
            json obj;
            obj["title"] = ev.title;
            obj["start"] = Utils::tpToString(ev.startTime);
            obj["end"] = Utils::tpToString(ev.endTime);
            obj["recurrence"] = "none";
            j.push_back(obj);
        }
        for (const auto& rev : recurring) {
            json obj;
            obj["title"] = rev.title;
            obj["start"] = Utils::tpToString(rev.startTime);
            obj["end"] = Utils::tpToString(rev.endTime);
            std::string recType;
            switch (rev.recurrence) {
                case RecurrenceType::Daily: recType = "daily"; break;
                case RecurrenceType::Weekly: recType = "weekly"; break;
                case RecurrenceType::Monthly: recType = "monthly"; break;
                default: recType = "none"; break;
            }
            obj["recurrence"] = recType;
            obj["interval"] = rev.interval;
            if (rev.recurrenceEnd != std::chrono::system_clock::time_point{}) {
                obj["recurrenceEnd"] = Utils::tpToString(rev.recurrenceEnd);
            }
            j.push_back(obj);
        }

        std::ofstream file(filePath);
        if (!file.is_open()) {
            errorMsg = "Cannot open file for writing: " + filePath;
            return false;
        }
        file << j.dump(4);
        return true;
    } catch (const std::exception& e) {
        errorMsg = "JSON save error: " + std::string(e.what());
        return false;
    }
}

bool Storage::load(std::vector<Event>& events,
                   std::vector<RecurringEvent>& recurring,
                   std::string& errorMsg) {
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            errorMsg = "";
            return true;
        }
        json j;
        file >> j;
        if (!j.is_array()) {
            errorMsg = "Storage file is not a valid JSON array.";
            return false;
        }
        events.clear();
        recurring.clear();
        for (const auto& item : j) {
            std::string title = item.value("title", "");
            std::string startStr = item.value("start", "");
            std::string endStr = item.value("end", "");
            std::string recType = item.value("recurrence", "none");
            int interval = item.value("interval", 1);
            std::string recEndStr = item.value("recurrenceEnd", "");

            if (title.empty() || startStr.empty() || endStr.empty()) {
                errorMsg = "Missing fields in stored event.";
                return false;
            }
            auto start = Utils::stringToTp(startStr);
            auto end = Utils::stringToTp(endStr);
            if (start == std::chrono::system_clock::time_point{} ||
                end == std::chrono::system_clock::time_point{}) {
                errorMsg = "Invalid date/time in stored data.";
                return false;
            }

            if (recType == "none") {
                events.emplace_back(title, start, end);
            } else {
                RecurrenceType rt = RecurrenceType::None;
                if (recType == "daily") rt = RecurrenceType::Daily;
                else if (recType == "weekly") rt = RecurrenceType::Weekly;
                else if (recType == "monthly") rt = RecurrenceType::Monthly;
                auto recEnd = std::chrono::system_clock::time_point{};
                if (!recEndStr.empty()) {
                    recEnd = Utils::stringToTp(recEndStr);
                }
                recurring.emplace_back(title, start, end, rt, interval, recEnd);
            }
        }
        return true;
    } catch (const std::exception& e) {
        errorMsg = "JSON load error: " + std::string(e.what());
        return false;
    }
}