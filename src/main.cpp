/* 
  * PURPOSE     : StalcraftLAB project.
  *               Main startup module.
  * PROGRAMMER  : Dyakonov Alexandr
  * NOTE        : None.
  */

#include <iostream>
#include <clickhouse/client.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <chrono> 
#include <string>
#include <fstream>
#include <sstream>
#include <cstdint> 
#include <ctime> 

#include "utils/utils.hpp"
#include "database/database_manager.hpp"
#include "utils/logger_macros.hpp" 
#include "database/auction_item_repository.hpp"
#include "api/api_client.hpp"
#include "services/auction_data_service.hpp"
#include "cli/command_line_parser.hpp"

std::shared_ptr<spdlog::logger> logger;

int main(int argc, char* argv[])
{    
    logger = spdlog::basic_logger_mt("logger", "debug.log");
    spdlog::set_default_logger(logger);

    utils::loadEnvVariables(".env");

    cli::CommandLineParser parser;
    parser.parseCommandLineArgs(argc, argv);

    // if (argc > 1 && (std::strcmp(argv[1], "--build-tables") == 0 || std::strcmp(argv[1], "--bt") == 0)) {
    //     std::cout << DatabaseManager::initializeTables() << std::endl;
    //     return 0;
    // }

    // std::vector<std::string> idVector = utils::readIdListFromFile("data/items_id_list");
    // for (const auto& id : idVector) {
    //     std::cout << "Read id: " << id << std::endl;
    // }

    // std::string server = "eu";
    // std::string itemId = "5r5g";
    // std::string token = std::getenv("EXBO_TOKEN");

    // auto start = std::chrono::high_resolution_clock::now();

    // std::vector<std::string> lines = {};
    // services::performDataUpdateAndCount(server, itemId, token, lines);


    // auto end = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> elapsed = end - start;
    // std::cout << "Execution time: " << elapsed.count() << " seconds" << std::endl;


    return 0;
}