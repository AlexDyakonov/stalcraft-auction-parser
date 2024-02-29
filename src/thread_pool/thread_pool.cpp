#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>

#include "../api/api_client.hpp"

void processItems(int offset, int limit, const std::string& token, DatabaseManager& dbManager) {
    auto jsonResult = api_client::getItemPrices(offset, limit, token); 
    auto items = utils::parseJsonToAuctionItems(jsonResult);
    AuctionItemRepository ai_repo(dbManager);
    ai_repo.AddItems(items);
}

void fetchDataAndStore(int total, const std::string& token) {
    DatabaseManager dbManager; 
    const int limit = 200;
    int availableThreads = std::thread::hardware_concurrency();
    int numberOfThreads = static_cast<int>(availableThreads * 0.75);
    if (numberOfThreads < 1) numberOfThreads = 1;

    std::vector<std::thread> threads;

    for (int offset = 0; offset < total; offset += limit * numberOfThreads) {
        for (int i = 0; i < numberOfThreads && (offset + i * limit) < total; ++i) {
            int currentOffset = offset + i * limit;
            threads.emplace_back(processItems, currentOffset, limit, token, std::ref(dbManager));
        }

        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        threads.clear(); 
    }
}
