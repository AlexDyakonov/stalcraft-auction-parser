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

#include "utils/utils.hpp"
#include "database/database_manager.hpp"
#include "utils/logger_macros.hpp" 
#include "database/auction_item_repository.hpp"
#include "api/api_client.hpp"

std::shared_ptr<spdlog::logger> logger;

int main(int argc, char* argv[])
{    
    logger = spdlog::basic_logger_mt("logger", "debug.log");
    spdlog::set_default_logger(logger);

    utils::loadEnvVariables(".env");

    int64_t total = api_client::getItemTotal("4q7pl", std::getenv("EXBO_TOKEN"));

    DatabaseManager dbManager; 


    // DatabaseManager dbManager; 
    // AuctionItemRepository ai_repo(dbManager);

    // auto start1 = std::chrono::steady_clock::now();
    // for(int i = 1; i <= 10; i++) {
    //     int offset = total - i * 200;
    //     std::vector<AuctionItem> items =  utils::parseJsonToAuctionItems(api_client::getItemPrices("4q7pl", 200, offset, std::getenv("EXBO_TOKEN")), "4q7pl");
    //     ai_repo.AddItems(items);
    // }
    // auto end1 = std::chrono::steady_clock::now(); 
    // auto delta1 = end1 - start1;

    // std::cout << "Время выполнения: " << std::chrono::duration <double, std::milli> (delta1).count() << " миллисекунд" << std::endl;


    const size_t numThreads = 10;
    std::vector<std::thread> threads;

    auto start1 = std::chrono::steady_clock::now();
    for (int i = 10; i <= 20; i++) {
        threads.emplace_back([i, total]() {
            DatabaseManager dbManager; 
            AuctionItemRepository ai_repo(dbManager);

            int offset = total - i * 200;
            std::vector<AuctionItem> items = utils::parseJsonToAuctionItems(api_client::getItemPrices("4q7pl", 200, offset, std::getenv("EXBO_TOKEN")), "4q7pl");
            ai_repo.AddItems(items);
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }
    auto end1 = std::chrono::steady_clock::now(); 
    auto delta1 = end1 - start1;

    std::cout << "Время выполнения: " << std::chrono::duration <double, std::milli> (delta1).count() << " миллисекунд" << std::endl;    

    


    if (argc > 1 && (std::strcmp(argv[1], "--build-tables") == 0 || std::strcmp(argv[1], "--bt") == 0)) {
        dbManager.initializeTables();
    }

    return 0;
}