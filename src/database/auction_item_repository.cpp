#include "auction_item_repository.hpp"
#include <sstream>
#include <iostream>
#include "../utils/logger_macros.hpp"

AuctionItemRepository::AuctionItemRepository(std::unique_ptr<clickhouse::Client> client)
    : client(std::move(client)) {}
    
void AuctionItemRepository::AddItem(const AuctionItem& item) {
    if (!client){
        LOG_ERROR("Client is not initialized");
        return;
    }

    std::ostringstream queryStream;
    queryStream << "INSERT INTO item_auction_info (itemId, amount, price, time, additional, server) VALUES ";
    queryStream << "('" << item.itemId << "', " << item.amount << ", " << item.price << ", '"
                << item.time << "', '" << item.additional.dump() << "', '" << item.server << "')";

    std::string query = queryStream.str();
    try {
        client->Execute(query);
    } catch (const std::exception& e) {
        LOG_ERROR("Exception occurred during AddItem execution: {}", e.what());
    }
}

void AuctionItemRepository::AddItems(const std::vector<AuctionItem>& items) {
    if (items.empty() || !client) {
        LOG_ERROR("Items are empty or client is not initialized");
        return;
    }
    std::ostringstream queryStream;
    queryStream << "INSERT INTO item_auction_info (itemId, amount, price, time, additional, server) VALUES ";

    for (size_t i = 0; i < items.size(); ++i) {
        const auto& item = items[i];
        queryStream << "('" << item.itemId << "', " << item.amount << ", " << item.price << ", '"
                    << item.time << "', '" << item.additional.dump() << "', '" << item.server << "')";
        if (i < items.size() - 1) {
            queryStream << ", ";
        }
    }

    std::string query = queryStream.str();

    try {
        client->Execute(query);
    } catch (const std::exception& e) {
        LOG_ERROR("Exception occurred during AddItems execution: {}", e.what());
    }
}


int64_t AuctionItemRepository::CountItemsByItemId(const std::string& itemId, const std::string& server) {
    if (!client) {
        LOG_ERROR("Client is not initialized");
        return 0;
    }

    std::ostringstream queryStream;
    queryStream << "SELECT COUNT() FROM item_auction_info WHERE itemId = '" << itemId << "' AND server = '" << server << "'";

    std::string query = queryStream.str();

    int64_t count = 0;
    try {
        client->Select(query, [&count](const clickhouse::Block& block) {
            if (block.GetRowCount() > 0) {
                count = block[0]->As<clickhouse::ColumnUInt64>()->At(0);
            }
        });
    } catch (const std::exception& e) {
        LOG_ERROR("Exception occurred during CountItemsByItemId execution: {}", e.what());
    }
    return count;
}

