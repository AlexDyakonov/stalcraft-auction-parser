#ifndef API_CLIENT_HPP
#define API_CLIENT_HPP

#include <string>
#include <sstream>


namespace api_client {
    std::string getBearerToken(std::string client_id, std::string client_secret);
    std::string getItemPrices(const std::string &server, const std::string &itemId, int limit, int offset, std::string bearer);
    int64_t getItemTotal(const std::string &server, const std::string &itemId, std::string bearer);
}


#endif