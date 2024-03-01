#ifndef UTILS_HPP
#define UTILS_HPP
#include "../database/auction_item_repository.hpp"

#include <string>


namespace utils {
    void loadEnvVariables(const std::string& relativePath);
    std::vector<AuctionItem> parseJsonToAuctionItems(const std::string &jsonString, const std::string &server, const std::string &itemId) ;
    std::vector<std::string> readIdListFromFile(const std::string& filename);
    std::string getAuctionUrl(const std::string& server, const std::string& itemId);
}

#endif
