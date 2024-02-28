#include "auction_item_repository.hpp"
#include <sstream>
#include <iostream>

AuctionItemRepository::AuctionItemRepository(DatabaseManager& dbManager)
    : dbManager(dbManager) {}

void AuctionItemRepository::AddItem(const AuctionItem& item) {
    auto* client = dbManager.getClient();

    std::ostringstream queryStream;
    queryStream << "INSERT INTO item_auction_info (itemId, amount, price, time, additional) VALUES ";

    queryStream << "('" << item.itemId << "', " << item.amount << ", " << item.price << ", '"
                << item.time << "', '" << item.additional << "')";

    std::string query = queryStream.str();
    try {
        auto* client = dbManager.getClient();
        if (client) {
            client->Execute(query);
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception occurred during AddItem execution: " << e.what() << std::endl;
    }
}