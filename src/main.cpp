#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>
#include "calendar.h"
#include "command_parser.h"
#include "utils.h"

int main() {
    std::cout << Utils::colorize("------ Welcome to Advanced Calendar (CC) ------", "\033[1;34m") << std::endl;
    Calendar calendar;
    CommandParser parser(calendar);

    using_history();
    char* line = nullptr;
    while (true) {
        line = readline(Utils::colorize("> ", "\033[1;32m").c_str());
        if (!line) break;
        std::string inputLine = Utils::trim(line);
        free(line);
        if (inputLine.empty()) continue;

        add_history(inputLine.c_str());

        if (inputLine == "exit" || inputLine == "quit" || inputLine == "EXIT") break;

        parser.processCommand(inputLine);
    }

    std::cout << Utils::colorize("Goodbye!", "\033[1;34m") << std::endl;
    return 0;
}