#include <cstring>
#include <cstdlib>
#include <functional>
#include <iomanip> 
#include <iostream>
#include "command_line_parser.hpp"
#include "../utils/logger_macros.hpp" 
#include "../utils/utils.hpp" 
#include "../database/database_manager.hpp"
#include "../services/auction_data_service.hpp"
#include "../database/auction_item_repository.hpp"

cli::CommandLineParser::CommandLineParser() {

    addCommand(Command("--build-tables", "--bt", "Initialize database tables", [](const std::vector<std::string>& args) {
        std::cout << DatabaseManager::initializeTables() << std::endl;
    }));

    addCommand(Command("parse", "", "Parse items with given parameters", [](const std::vector<std::string>& args) {
        std::vector<std::string> lines = {};
        std::string token = std::getenv("EXBO_TOKEN");

        bool help = false;
        std::string server;
        std::string itemId;
        bool allFirst = false;
        bool itemFirst = false;
        bool allReparse = false;
        bool itemReparse = false;

        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == "--help" || args[i] == "-h") {
                help = true;
            } else if (args[i] == "--all-first") {
                allFirst = true;
            } else if (args[i] == "--item-first" && i + 1 < args.size()) {
                itemFirst = true;
                itemId = args[++i];
            } else if (args[i] == "--all-reparse") {
                allReparse = true;
            } else if (args[i] == "--item-reparse" && i + 1 < args.size()) {
                itemReparse = true;
                itemId = args[++i];
            } else if ((args[i] == "--server" || args[i] == "-s") && i + 1 < args.size()) {
                server = args[++i];
            }
        }

        if (help) {
            std::cout << "Usage of 'parse' command:\n";
            std::cout << "--server, -s <serverName>: Specify the server to parse.\n";
            std::cout << "--item-first <itemId>: Parse a single item from the server for the first time.\n";
            std::cout << "--all-first: Parse all items from the server for the first time.\n";
            std::cout << "--item-reparse <itemId>: Reparse a single item from the server.\n";
            std::cout << "--all-reparse: Reparse all items from the server.\n";
            std::cout << "--help, -h: Display this help message.\n";
            return;
        }

        if (allFirst) {
            std::cout << "Parsing all items from server: " << server << " for the first time." << std::endl;
            services::parseDataForAllItems(server, utils::getToken(server));
        } else if (itemFirst && !itemId.empty()) {
            std::cout << "Parsing item " << itemId << " from server: " << server << " for the first time." << std::endl;
            services::parseDataForSingleItem(server, itemId, utils::getToken(server), lines);
        } else if (allReparse) {
            std::cout << "Reparsing new data for all items from server: " << server << "." << std::endl;
            services::parseNewDataForAllItems(server, utils::getToken(server));
        } else if (itemReparse && !itemId.empty()) {
            std::cout << "Reparsing new data for item " << itemId << " from server: " << server << "." << std::endl;
            AuctionItemRepository ai_repo(DatabaseManager::CreateNewClient());
            services::parseNewDataForSingleItem(server, itemId, utils::getToken(server));
        } else {
            std::cout << "Missing or incorrect arguments for 'parse' command.\n";
        }
    }));

    addCommand(Command("--help", "-h", "Display this help and exit", [this](const std::vector<std::string>& args) {
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