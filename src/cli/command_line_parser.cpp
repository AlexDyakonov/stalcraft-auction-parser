#include "command_line_parser.hpp"
#include "../utils/logger_macros.hpp" 
#include "../database/database_manager.hpp"
#include "../services/auction_data_service.hpp"
#include <cstring>
#include <cstdlib>
#include <functional>
#include <iomanip> 


cli::CommandLineParser::CommandLineParser() {
    addCommand(Command("--build-tables", "--bt", "Initialize database tables", []() {
        std::cout << DatabaseManager::initializeTables() << std::endl;
    }));

    addCommand(Command("--fetch-data", "", "Fetch and store auction data", []() {
        std::cout << "test" << std::endl;
    }));

    addCommand(Command("--help", "-h", "Display this help and exit", [this]() {
        this->printHelp();
    }));
}

void cli::CommandLineParser::addCommand(const Command& cmd) {
    commands.push_back(cmd);
}

void cli::CommandLineParser::parseCommandLineArgs(int argc, char* argv[]) {
    if (argc < 2) {
        printHelp();
        return;
    }

    for (int i = 1; i < argc; ++i) {
        for (const auto& cmd : commands) {
            if (std::strcmp(argv[i], cmd.name.c_str()) == 0 || (!cmd.shortName.empty() && std::strcmp(argv[i], cmd.shortName.c_str()) == 0)) {
                cmd.action();
                return;
            }
        }
    }

    std::cout << "Unknown command. Use --help or -h for a list of available commands." << std::endl;
}

void cli::CommandLineParser::printHelp() const {
        std::cout << "Global Options:\n";
        for (const auto& cmd : commands) {
            std::cout << "  ";
            if (!cmd.shortName.empty()) {
                std::cout << std::left << std::setw(2) << cmd.shortName << ", ";
            } else {
                std::cout << "    ";
            }
            std::cout << std::left << std::setw(20) << cmd.name << cmd.description << "\n";
        }
        // std::cout << "\nRun '<program> COMMAND --help' for more information on a command.\n";
}