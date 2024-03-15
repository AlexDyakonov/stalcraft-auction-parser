#include "api_client.hpp"
#include "../utils/utils.hpp"
#include <iostream>
#include <sstream>

namespace api_client {

APIClient::APIClient(std::string token) : bearerToken(std::move(token)) {}

    std::string getBearerToken(std::string client_id, std::string client_secret){
    std::string url = "https://exbo.net/oauth/token";

    cpr::Response response = cpr::Post(cpr::Url{url},
                                           cpr::Header{{"Content-Type", "application/x-www-form-urlencoded"}},
                                           cpr::Payload{{"client_id", client_id},
                                                        {"client_secret", client_secret},
                                                        {"grant_type", "client_credentials"}});

    auto json_response = json::parse(response.text);

    std::string token;
    try {
            auto json_response = nlohmann::json::parse(response.text);
            if (json_response.find("access_token") != json_response.end()) {
                token = json_response["access_token"];
            }
        } catch (const nlohmann::json::exception& e) {
            std::cerr << "JSON parse error: " << e.what() << '\n';
        }

    return token;
    }

    std::string APIClient::getItemPrices(const std::string &server, const std::string &itemId, int limit, int offset) {
        std::string url = utils::getAuctionUrl(server, itemId);     

        cpr::Response response = cpr::Get(cpr::Url{url},
                                        cpr::Parameters{{"limit", std::to_string(limit)}, {"offset", std::to_string(offset)}},
                                        cpr::Bearer{bearerToken});

        updateRateLimits(response);

        return response.text;
    }

    int64_t APIClient::getItemTotal(const std::string &server, const std::string &itemId) {
        std::string url = utils::getAuctionUrl(server, itemId);     

        cpr::Response response = cpr::Get(cpr::Url{url},
                                        cpr::Parameters{{"limit", "1"}, {"offset", "0"}},
                                        cpr::Bearer{bearerToken});

        auto json_response = json::parse(response.text);
        int64_t total = json_response["total"];

        updateRateLimits(response);

        return total;
    }

    void APIClient::updateRateLimits(const cpr::Response& response) {
        std::lock_guard<std::mutex> lock(mutex);
        if (xRatelimitRemaining < 10) {
        std::cout << xRatelimitRemaining << std::endl;
        std::cout << xRatelimitReset << std::endl;
        }
        
        if (response.header.find("x-ratelimit-remaining") != response.header.end()) {
            xRatelimitRemaining = std::stoi(response.header.at("x-ratelimit-remaining"));
        }
        if (response.header.find("x-ratelimit-reset") != response.header.end()) {
            xRatelimitReset = std::stoll(response.header.at("x-ratelimit-reset"));
        }
    }

}