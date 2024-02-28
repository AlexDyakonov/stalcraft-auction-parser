/* 
  * PURPOSE     : StalcraftLAB project.
  *               Main startup module.
  * PROGRAMMER  : Dyakonov Alexandr
  * NOTE        : None.
  */

#include <iostream>
#include <clickhouse/client.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "utils/utils.hpp"
#include "database/database_manager.hpp"
#include "utils/logger_macros.hpp" 
#include "database/auction_item_repository.hpp"

std::shared_ptr<spdlog::logger> logger;

int main(int argc, char* argv[])
{    
    logger = spdlog::basic_logger_mt("logger", "debug.log");
    spdlog::set_default_logger(logger);

    DatabaseManager dbManager; 

    try {
        AuctionItemRepository itemRepository(dbManager);

        AuctionItem item;
        item.itemId = "exampleItem123";
        item.amount = 10;
        item.price = 99.99;
        item.time = "2023-01-01 00:00:00";
        item.additional = "{\"key\": \"value\"}"; 

        itemRepository.AddItem(item);

        std::cout << "Item successfully added to the database." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }    


    if (argc > 1 && (std::strcmp(argv[1], "--build-tables") == 0 || std::strcmp(argv[1], "--bt") == 0)) {
        dbManager.initializeTables();
    }

    return 0;
}