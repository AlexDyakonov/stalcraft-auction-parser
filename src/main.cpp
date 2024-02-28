/* 
  * PURPOSE     : StalcraftLAB project.
  *               Main startup module.
  * PROGRAMMER  : Dyakonov Alexandr
  * NOTE        : None.
  */

#include <iostream>
#include <clickhouse/client.h>
#include "utils/utils.hpp"
#include <spdlog/sinks/basic_file_sink.h>
#include "database/database_manager.hpp"

#include "utils/logger_macros.hpp" 

std::shared_ptr<spdlog::logger> logger;

int main()
{    
    logger = spdlog::basic_logger_mt("logger", "debug.log");
    spdlog::set_default_logger(logger);


    DatabaseManager dbManager;
    auto* client = dbManager.getClient();
    
    try {
        client->Ping();
        std::cout << "Successfully connected to ClickHouse." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
    }


    return 0;
}