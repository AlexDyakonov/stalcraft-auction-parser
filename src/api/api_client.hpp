#ifndef API_CLIENT_HPP
#define API_CLIENT_HPP

#include <string>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <mutex>

namespace api_client {

using json = nlohmann::json;

class APIClient {
public:
    APIClient(std::string token);
    std::string getBearerToken(std::string client_id, std::string client_secret);
    std::string getItemPrices(const std::string &server, const std::string &itemId, int limit, int offset);
    int64_t getItemTotal(const std::string &server, const std::string &itemId);

private:
    std::string bearerToken;
    int xRatelimitRemaining = 400;
    int xRatelimitLimit = 400;
    long long xRatelimitReset = 0; 
    std::mutex mutex;
    void updateRateLimits(const cpr::Response& response);
};

} 

#endif
