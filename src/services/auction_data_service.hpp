#ifndef AUCTION_DATA_SERVICE_HPP
#define AUCTION_DATA_SERVICE_HPP

#include <string>

namespace services {

    void fetchAndStoreAuctionData(const std::string& server, const std::string& itemId, const std::string& token, std::vector<std::string>& lines);
    void parseDataForSingleItem(const std::string& server, const std::string& itemId, const std::string& token, std::vector<std::string>& lines);
    void parseDataForAllItems(const std::string& server, const std::string& token);
    void parseNewDataForSingleItem(const std::string& server, const std::string& itemId, const std::string& token);
    void parseNewDataForAllItems(const std::string& server, const std::string& token);
} 

#endif
