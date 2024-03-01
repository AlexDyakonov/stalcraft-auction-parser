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

std::shared_ptr<spdlog::logger> logger;

int main(int argc, char* argv[])
{    
    logger = spdlog::basic_logger_mt("logger", "debug.log");
    spdlog::set_default_logger(logger);

    utils::loadEnvVariables(".env");

    if (argc > 1 && (std::strcmp(argv[1], "--build-tables") == 0 || std::strcmp(argv[1], "--bt") == 0)) {
        DatabaseManager dbManager; 
        dbManager.initializeTables();
        return 0;
    }

    // std::vector<std::string> idVector = utils::readIdListFromFile("data/items_id_list");
    // for (const auto& id : idVector) {
    //     std::cout << "Read id: " << id << std::endl;
    // }

    const size_t numThreads = 10;
    std::vector<std::thread> threads;

    int64_t total = api_client::getItemTotal("eu", "4q7pl", std::getenv("EXBO_TOKEN"));
    std::cout << total << std::endl;

    std::string itemId = "4q7pl";
    std::string server = "eu";

    for (int i = 10; i < 20; i++) {
        threads.emplace_back([i, total, server, itemId]() {
            DatabaseManager dbManager; 
            AuctionItemRepository ai_repo(dbManager);

            int offset = total - i * 200;
            std::vector<AuctionItem> items = utils::parseJsonToAuctionItems(api_client::getItemPrices(server, itemId, 200, offset, std::getenv("EXBO_TOKEN")), server, itemId);
            ai_repo.AddItems(items);
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }    

    return 0;
}