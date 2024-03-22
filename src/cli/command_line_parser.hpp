#ifndef COMMAND_LINE_PARSER_HPP
#define COMMAND_LINE_PARSER_HPP

#include <string>
#include <vector>
#include <functional> 

struct Command {
    std::string name; 
    std::string shortName; 
    std::string description; 
    std::function<void(const std::vector<std::string>& args)> action; 

    Command(std::string n, std::string sN, std::string desc, std::function<void(const std::vector<std::string>& args)> act)
        : name(std::move(n)), shortName(std::move(sN)), description(std::move(desc)), action(std::move(act)) {}
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
