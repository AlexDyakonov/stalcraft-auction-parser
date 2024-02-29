#ifndef AUCTION_ITEM_REPOSITORY_HPP
#define AUCTION_ITEM_REPOSITORY_HPP

#include "../database/database_manager.hpp"
#include <string>
#include <nlohmann/json.hpp> 

using json = nlohmann::json;


struct AuctionItem
{
    std::string itemId;
    int amount;
    double price;
    std::string time;
    json additional;

    AuctionItem() : amount(0), price(0.0) {}

    AuctionItem(const json& j)
        : itemId(j.at("itemId").get<std::string>()),
          amount(j.at("amount").get<int>()),
          price(j.at("price").get<double>()),
          time(j.at("time").get<std::string>().replace(10, 1, " ").replace(19, 1, "")),
          additional(j.at("additional")) {}
};

class AuctionItemRepository {
public:
    AuctionItemRepository(DatabaseManager& dbManager);
    void AddItem(const AuctionItem& item);
    void AddItems(const std::vector<AuctionItem>& items);

private:
    DatabaseManager& dbManager;
};


#endif