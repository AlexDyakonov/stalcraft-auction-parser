#include "auction_item_repository.hpp"
#include <sstream>
#include <iostream>

AuctionItemRepository::AuctionItemRepository(std::unique_ptr<clickhouse::Client> client)
    : client(std::move(client)) {}
    
void AuctionItemRepository::AddItem(const AuctionItem& item) {
    if (!client) return;

    std::ostringstream queryStream;
    queryStream << "INSERT INTO item_auction_info (itemId, amount, price, time, additional, server) VALUES ";
    queryStream << "('" << item.itemId << "', " << item.amount << ", " << item.price << ", '"
                << item.time << "', '" << item.additional.dump() << "', '" << item.server << "')";

    std::string query = queryStream.str();
    try {
        client->Execute(query);
    } catch (const std::exception& e) {
        std::cerr << "Exception occurred during AddItem execution: " << e.what() << std::endl;
    }
}

void AuctionItemRepository::AddItems(const std::vector<AuctionItem>& items) {
    if (items.empty() || !client) return;

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
        std::cerr << "Exception occurred during AddItems execution: " << e.what() << std::endl;
    }
}
