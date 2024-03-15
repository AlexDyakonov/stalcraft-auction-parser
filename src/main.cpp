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
        std::cout << DatabaseManager::initializeTables() << std::endl;
        return 0;
    }

    // std::vector<std::string> idVector = utils::readIdListFromFile("data/items_id_list");
    // for (const auto& id : idVector) {
    //     std::cout << "Read id: " << id << std::endl;
    // }


    std::string token = std::getenv("EXBO_TOKEN");
    api_client::APIClient apiClient(token);


       std::string itemId = "4q7pl";
    std::string server = "eu";
    int64_t total = apiClient.getItemTotal(server, itemId);
    std::cout << "Total items: " << total << std::endl;

    size_t numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    int limit = 200; // Максимальное количество элементов за запрос
    int totalRequests = total / limit + (total % limit != 0 ? 1 : 0); // Общее количество запросов

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < totalRequests; ++i) {
        threads.emplace_back([i, limit, server, itemId, &apiClient]() {
            AuctionItemRepository ai_repo(DatabaseManager::CreateNewClient());
            int offset = i * limit; // Смещение для текущего запроса
            std::vector<AuctionItem> items = utils::parseJsonToAuctionItems(apiClient.getItemPrices(server, itemId, limit, offset), server, itemId);
            ai_repo.AddItems(items);
        });

        if (threads.size() == numThreads || i == totalRequests - 1) {
            for (auto& thread : threads) {
                thread.join();
            }
            threads.clear();
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Execution time: " << elapsed.count() << " seconds" << std::endl;


    return 0;
}