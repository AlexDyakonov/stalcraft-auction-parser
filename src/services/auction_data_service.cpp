#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
#include <mutex>

#include "../utils/utils.hpp"
#include "../database/database_manager.hpp"
#include "../utils/logger_macros.hpp" 
#include "../utils/utils.hpp" 
#include "../database/auction_item_repository.hpp"
#include "../api/api_client.hpp"

namespace services {

    std::vector<AuctionItem> itemsBuffer;
    std::mutex bufferMutex;
    const size_t batchSize = 1000;

    std::time_t parseDateTime(const std::string& datetime) {
        std::tm tm = {};
        std::istringstream ss(datetime);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        return std::mktime(&tm);
    }

    void flushItemsToDatabase() {
        if (!itemsBuffer.empty()) {
            auto ai_repo = AuctionItemRepository(DatabaseManager::CreateNewClient());
            ai_repo.AddItems(itemsBuffer);
            itemsBuffer.clear();
        }
    }

    void fetchAndStoreAuctionData(const std::string& server, const std::string& itemId, const std::string& token, std::vector<std::string>& lines) {
        api_client::APIClient apiClient(token);
        int64_t total = 0;
        std::string lastItemTime = "";

        try {
            total = apiClient.getItemTotal(server, itemId);
        } catch (const std::exception& e) {
            LOG_ERROR("Error fetching total items for server: {}, itemId: {}. Exception: {}", server, itemId, e.what());
            return;
        }
        
        lines.push_back(itemId);
        lines.push_back(std::to_string(total));

        LOG_INFO("Total items for server: {}, itemId: {}: {}", server, itemId, total);

        size_t numThreads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;

        int limit = 200; 
        int totalRequests = total / limit + (total % limit != 0 ? 1 : 0);

        for (int i = 0; i < totalRequests; ++i) {
            threads.emplace_back([i, limit, server, itemId, &apiClient, total, &lastItemTime]() {
                int offset = i * limit;
                std::vector<AuctionItem> fetchedItems;
                bool success = false;
                
                for (int attempt = 1; attempt <= 5; ++attempt) {
                    try {
                        auto jsonResponse = apiClient.getItemPrices(server, itemId, limit, offset);
                        
                        if (jsonResponse.empty() || jsonResponse == "[]") {
                            LOG_ERROR("Attempt {}: Empty response for server: {}, itemId: {}, offset: {}. Last item time: {}", attempt, server, itemId, offset, lastItemTime);

                            if (attempt == 5) {
                                LOG_ERROR("Final attempt failed. No more retries. Server: {}, itemId: {}, offset: {}", server, itemId, offset);
                                return;
                            }

                            LOG_INFO("Waiting 10 min before new attempt for server: {}, itemId: {}, offset: {}. Last item time: {}", server, itemId, offset, lastItemTime);                            
                            std::this_thread::sleep_for(std::chrono::minutes(3));
                            continue;
                        }
                        
                        fetchedItems = utils::parseJsonToAuctionItems(jsonResponse, server, itemId);
                        if (!fetchedItems.empty()) {
                            lastItemTime = fetchedItems.back().time;
                            success = true;
                            break;
                        }
                    } catch (const nlohmann::json::parse_error& e) {
                        LOG_ERROR("JSON parse error for server: {}, itemId: {}, offset: {}. Exception: {}", server, itemId, offset, e.what());
                        return;
                    } catch (const std::exception& e) {
                        LOG_ERROR("Unexpected error for server: {}, itemId: {}, offset: {}. Exception: {}", server, itemId, offset, e.what());
                        return;
                    }
                }
                
                if (success) {
                    std::lock_guard<std::mutex> guard(bufferMutex);
                    itemsBuffer.insert(itemsBuffer.end(), fetchedItems.begin(), fetchedItems.end());
                    if (itemsBuffer.size() >= batchSize) {
                        flushItemsToDatabase();
                    }
                }
            });
        }

        std::lock_guard<std::mutex> guard(bufferMutex);
        flushItemsToDatabase();
    }

    void parseDataForSingleItem(const std::string& server, const std::string& itemId, const std::string& token, std::vector<std::string>& lines) {
        fetchAndStoreAuctionData(server, itemId, token, lines);

        AuctionItemRepository ai_repo(DatabaseManager::CreateNewClient());
        int64_t totalCount = ai_repo.CountItemsByItemId(itemId, server);

        lines.push_back(std::to_string(totalCount));
        LOG_INFO("Total items for server: {}, itemId: {}: {}", server, itemId, totalCount);
        utils::writeToSummaryTable(lines, false);
    }

    void parseDataForAllItems(const std::string& server, const std::string& token) {
        std::vector<std::string> idVector = utils::readIdListFromFile("data/items_id_list");
        for (const auto& itemId : idVector) {
            std::vector<std::string> lines = {};
            services::parseDataForSingleItem(server, itemId, token, lines);
        }
    }

    void fetchAndStoreNewAuctionData(const std::string& server, const std::string& itemId, const std::string& token, std::vector<std::string>& lines) {
        api_client::APIClient apiClient(token);
        AuctionItemRepository ai_repo(DatabaseManager::CreateNewClient());
        std::string lastKnownTimeString = ai_repo.GetLatestItemDate(itemId, server);
        std::time_t lastKnownTime = parseDateTime(lastKnownTimeString);

        int64_t total = apiClient.getItemTotal(server, itemId); 
        if (total <= 0) {
            LOG_INFO("No items to fetch for server: {}, itemId: {}", server, itemId);
            return;
        }

        size_t numThreads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;
        int limit = 200; 
        int totalRequests = total / limit + (total % limit != 0 ? 1 : 0);
        std::atomic<bool> stopFetching = false;

        for (int i = 0; i < totalRequests; ++i) {
            threads.emplace_back([&, i, limit, server, itemId]() {
                int offset = i * limit;
                std::vector<AuctionItem> fetchedItems;
                bool success = false;

                for (int attempt = 1; attempt <= 5; ++attempt) {
                    auto jsonResponse = apiClient.getItemPrices(server, itemId, limit, offset);

                    if (jsonResponse.empty() || jsonResponse == "[]") {
                        LOG_ERROR("Attempt {}: Empty response for server: {}, itemId: {}, offset: {}", attempt, server, itemId, offset);
                        
                        if (attempt == 5) {
                            LOG_ERROR("Final attempt failed. No more retries. Server: {}, itemId: {}, offset: {}", server, itemId, offset);
                            return;
                        }
                        
                        std::this_thread::sleep_for(std::chrono::minutes(3));
                        continue;
                    }

                    fetchedItems = utils::parseJsonToAuctionItems(jsonResponse, server, itemId);
                    if (!fetchedItems.empty()) {
                        success = true;
                        break;
                    }
                }

                if (success) {
                    for (const auto& item : fetchedItems) {
                        std::time_t itemTime = parseDateTime(item.time);
                        
                        if (itemTime <= lastKnownTime) {
                            stopFetching.store(true);
                            break;
                        }
                    }

                    if (!stopFetching.load()) {
                        std::lock_guard<std::mutex> guard(bufferMutex);
                        itemsBuffer.insert(itemsBuffer.end(), fetchedItems.begin(), fetchedItems.end());
                        if (itemsBuffer.size() >= batchSize) {
                            flushItemsToDatabase();
                        }
                    }
                }
            });

            if (threads.size() == numThreads || i == totalRequests - 1) {
                for (auto& thread : threads) {
                    if (thread.joinable()) {
                        thread.join();
                    }
                }
                threads.clear();
            }
        }

        if (!itemsBuffer.empty()) {
            std::lock_guard<std::mutex> guard(bufferMutex);
            flushItemsToDatabase();
        }
    }

    void parseNewDataForSingleItem(const std::string& server, const std::string& itemId, const std::string& token, std::vector<std::string>& lines) {
        fetchAndStoreNewAuctionData(server, itemId, token, lines);

        AuctionItemRepository ai_repo(DatabaseManager::CreateNewClient());
        int64_t newCount = ai_repo.CountItemsByItemId(itemId, server);

        lines.push_back(std::to_string(newCount));
        LOG_INFO("New items parsed for server: {}, itemId: {}: {}", server, itemId, newCount);
        utils::writeToSummaryTable(lines, true);
    }

    void parseNewDataForAllItems(const std::string& server, const std::string& token) {
        std::vector<std::string> idVector = utils::readIdListFromFile("data/items_id_list"); 

        for (const auto& itemId : idVector) {
            std::vector<std::string> lines;
            parseNewDataForSingleItem(server, itemId, token, lines);
            LOG_INFO("Completed parsing new data for item: {}", itemId);
        }
    }
    
}
