#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include "calendar.h"

class CommandParser {
public:
    CommandParser(Calendar& calendar);

    void processCommand(const std::string& line);
    void showHelp() const;

private:
    Calendar& calendar;
    std::map<std::string, std::function<void(const std::vector<std::string>&)>> commands;

    void handleAdd(const std::vector<std::string>& args);
    void handleList(const std::vector<std::string>& args);
    void handleRemove(const std::vector<std::string>& args);
    void handleEdit(const std::vector<std::string>& args);
    void handleSearch(const std::vector<std::string>& args);
    void handleShow(const std::vector<std::string>& args);
    void handleHelp(const std::vector<std::string>& args);

    std::map<std::string, std::string> parseKeyValueArgs(const std::vector<std::string>& args);
    std::vector<std::string> tokenize(const std::string& line);
};

#endif