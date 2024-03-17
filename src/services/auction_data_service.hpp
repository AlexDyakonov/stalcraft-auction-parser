#ifndef AUCTION_DATA_SERVICE_HPP
#define AUCTION_DATA_SERVICE_HPP

#include <string>

namespace services {

    void fetchAndStoreAuctionData(const std::string& server, const std::string& itemId, const std::string& token, std::vector<std::string>& lines);
    void performDataUpdateAndCount(const std::string& server, const std::string& itemId, const std::string& token, std::vector<std::string>& lines);
} 

#endif
