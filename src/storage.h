#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <vector>
#include "event.h"
#include "recurring_event.h"

class Storage {
public:
    Storage(const std::string& filePath = "calendar_data.json");

    bool save(const std::vector<Event>& events, 
              const std::vector<RecurringEvent>& recurring,
              std::string& errorMsg);
    
    bool load(std::vector<Event>& events,
              std::vector<RecurringEvent>& recurring,
              std::string& errorMsg);

private:
    std::string filePath;
};

#endif