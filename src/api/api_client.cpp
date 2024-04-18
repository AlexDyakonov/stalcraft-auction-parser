#include "api_client.hpp"
#include "../utils/utils.hpp"
#include "../utils/logger_macros.hpp" 
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

    int64_t APIClient::getItemTotal(const std::string &server, const std::string &itemId) {
        std::string url = utils::getAuctionUrl(server, itemId);     

        cpr::Response response = cpr::Get(cpr::Url{url},
                                        cpr::Parameters{{"limit", "1"}, {"offset", "0"}, {"additional", "true"}},
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
            xRatelimitReset = std::stoll(response.header.at("x-ratelimit-reset")) / 1000;
        }
    }

    void waitForDate(long long timestamp) {
        auto now = std::chrono::system_clock::now();
        auto resetTime = std::chrono::system_clock::from_time_t(timestamp);
        if (resetTime > now) {
            std::this_thread::sleep_until(resetTime);
        }
    }

    std::string APIClient::getItemPrices(const std::string &server, const std::string &itemId, int limit, int offset) {
        cpr::Response response;
        bool requestCompleted = false;

        while (!requestCompleted) {
            {
                std::unique_lock<std::mutex> lock(mutex);
                if (xRatelimitRemaining <= 0) {
                    auto now = std::chrono::system_clock::now();
                    auto resetTimePoint = std::chrono::system_clock::from_time_t(xRatelimitReset / 1000 + 10); 
                    if (now < resetTimePoint) {
                        lock.unlock(); 
                        std::this_thread::sleep_until(resetTimePoint);
                    }
                }
            }

            std::string url = utils::getAuctionUrl(server, itemId);
            response = cpr::Get(cpr::Url{url},
                                cpr::Parameters{{"limit", std::to_string(limit)}, {"offset", std::to_string(offset)}, {"additional", "true"}},
                                cpr::Bearer{bearerToken});

            updateRateLimits(response);

            if (response.status_code == 200) {
                requestCompleted = true;
            } else if (response.status_code == 429) { 
                LOG_ERROR("Rate limit exceeded for server: " + server + ", itemId: " + itemId + ", offset: " + std::to_string(offset) + ". Waiting for reset.");
                std::this_thread::sleep_for(std::chrono::seconds(1));
            } else if (response.status_code == 500 || response.status_code == 525) {
                LOG_ERROR("Server error encountered. Status code: " + std::to_string(response.status_code) + ". Retrying in 10 minutes. server: " + server + ", itemId: " + itemId + ", offset: " + std::to_string(offset));
                std::this_thread::sleep_for(std::chrono::minutes(10));
            } else {
                LOG_ERROR("Unhandled response status code: " + std::to_string(response.status_code) + ". Aborting request for server: " + server + ", itemId: " + itemId + ", offset: " + std::to_string(offset));
                return ""; 
            }
        }

        return response.text; 
    }

}