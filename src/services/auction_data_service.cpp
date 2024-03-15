#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
#include <mutex>

#include "../utils/utils.hpp"
#include "../database/database_manager.hpp"
#include "../utils/logger_macros.hpp" 
#include "../database/auction_item_repository.hpp"
#include "../api/api_client.hpp"

namespace services {

    std::vector<AuctionItem> itemsBuffer;
    std::mutex bufferMutex;
    const size_t batchSize = 1000;

    void flushItemsToDatabase() {
        if (!itemsBuffer.empty()) {
            auto ai_repo = AuctionItemRepository(DatabaseManager::CreateNewClient());
            ai_repo.AddItems(itemsBuffer);
            itemsBuffer.clear();
        }
    }

    void fetchAndStoreAuctionData(const std::string& server, const std::string& itemId, const std::string& token) {
        api_client::APIClient apiClient(token);
        int64_t total = apiClient.getItemTotal(server, itemId);
        std::cout << "Total items: " << total << std::endl;

        size_t numThreads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;

        int limit = 200; 
        int totalRequests = total / limit + (total % limit != 0 ? 1 : 0);

        for (int i = 0; i < totalRequests; ++i) {
            threads.emplace_back([i, limit, server, itemId, &apiClient]() {
                int offset = i * limit;
                auto fetchedItems = utils::parseJsonToAuctionItems(apiClient.getItemPrices(server, itemId, limit, offset), server, itemId);
                
                {
                    std::lock_guard<std::mutex> guard(bufferMutex);
                    itemsBuffer.insert(itemsBuffer.end(), fetchedItems.begin(), fetchedItems.end());
                    if (itemsBuffer.size() >= batchSize) {
                        flushItemsToDatabase();
                    }
                }
            });

            if (threads.size() == numThreads || i == totalRequests - 1) {
                for (auto& thread : threads) thread.join();
                threads.clear();
            }
        }

        std::lock_guard<std::mutex> guard(bufferMutex);
        flushItemsToDatabase();
    }
}
