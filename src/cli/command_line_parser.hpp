#ifndef COMMAND_LINE_PARSER_HPP
#define COMMAND_LINE_PARSER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <functional>

struct Command {
    std::string name;
    std::string shortName;
    std::string description;
    std::function<void()> action;

    Command(std::string n, std::string sN, std::string desc, std::function<void()> act)
        : name(n), shortName(sN), description(desc), action(act) {}
};

namespace cli {
    class CommandLineParser {
    private:
        std::vector<Command> commands;

    public:
        CommandLineParser();
        void parseCommandLineArgs(int argc, char* argv[]);
        void addCommand(const Command& cmd);
        void printHelp() const;
    };
}

#endif
