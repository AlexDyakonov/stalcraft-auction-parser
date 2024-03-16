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
        int64_t total = 0;
        try {
            total = apiClient.getItemTotal(server, itemId);
        } catch (const std::exception& e) {
            LOG_ERROR("Error fetching total items for server: {}, itemId: {}. Exception: {}", server, itemId, e.what());
            return;
        }
        
        LOG_INFO("Total items for server: {}, itemId: {}: {}", server, itemId, total);

        size_t numThreads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;

        int limit = 200; 
        int totalRequests = total / limit + (total % limit != 0 ? 1 : 0);

        for (int i = 0; i < totalRequests; ++i) {
            threads.emplace_back([i, limit, server, itemId, &apiClient, total]() {
                int offset = i * limit;
                std::vector<AuctionItem> fetchedItems;
                try {
                    auto jsonResponse = apiClient.getItemPrices(server, itemId, limit, offset);
                    fetchedItems = utils::parseJsonToAuctionItems(jsonResponse, server, itemId);
                } catch (const nlohmann::json::parse_error& e) {
                    LOG_ERROR("JSON parse error for server: {}, itemId: {}, offset: {}. Exception: {}", server, itemId, offset, e.what());
                    return;
                } catch (const std::exception& e) {
                    LOG_ERROR("Unexpected error for server: {}, itemId: {}, offset: {}. Exception: {}", server, itemId, offset, e.what());
                    return;
                }
                
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
