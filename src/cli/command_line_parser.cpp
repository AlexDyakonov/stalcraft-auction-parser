#include "command_line_parser.hpp"
#include "../utils/logger_macros.hpp" 
#include "../database/database_manager.hpp"
#include "../services/auction_data_service.hpp"
#include <cstring>
#include <cstdlib>
#include <functional>
#include <iomanip> 
#include <iostream>


cli::CommandLineParser::CommandLineParser() {
    addCommand(Command("--build-tables", "--bt", "Initialize database tables", [](const std::vector<std::string>& args) {
        std::cout << DatabaseManager::initializeTables() << std::endl;
    }));

    addCommand(Command("--fetch-data", "", "Fetch and store auction data", [](const std::vector<std::string>& args) {
        std::cout << "Fetching and storing auction data..." << std::endl;
    }));

    addCommand(Command("--help", "-h", "Display this help and exit", [this](const std::vector<std::string>& args) {
        this->printHelp();
    }));

    addCommand(Command("parse", "", "Parse items with given parameters", [](const std::vector<std::string>& args) {
        std::string itemId;
        std::string server;
        bool all = false;

        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == "--item" && i + 1 < args.size()) {
                itemId = args[++i];
            } else if (args[i] == "--server" && i + 1 < args.size()) {
                server = args[++i];
            } else if (args[i] == "--new") {
                all = true;
            }
        }

        if (itemId.empty() || server.empty()) {
            std::cout << "Missing '--item' or '--server' argument.\n";
            return;
        }

        if (all) {
            std::cout << "Parsing all items for server: " << server << " and item ID: " << itemId << "\n";
        } else {
            std::cout << "Parsing items for server: " << server << " and item ID: " << itemId << "\n";
        }
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

    bool commandFound = false;

    for (int i = 1; i < argc; ++i) {
        for (const auto& cmd : commands) {
            if (std::strcmp(argv[i], cmd.name.c_str()) == 0 || (!cmd.shortName.empty() && std::strcmp(argv[i], cmd.shortName.c_str()) == 0)) {
                std::vector<std::string> args;
                int nextArg = i + 1;
                while (nextArg < argc && argv[nextArg][0] == '-') {
                    if (nextArg + 1 < argc && argv[nextArg + 1][0] != '-') {
                        args.push_back(argv[nextArg++]); 
                        args.push_back(argv[nextArg++]); 
                    } else {
                        args.push_back(argv[nextArg++]);
                    }
                }
                cmd.action(args);
                commandFound = true;
                break; 
            }
        }
        if (commandFound) break; 
    }

    if (!commandFound) {
        std::cout << "Unknown command. Use --help or -h for a list of available commands." << std::endl;
    }
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