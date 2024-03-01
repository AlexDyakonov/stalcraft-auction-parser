#include <iostream>
#include <string>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "../utils/utils.hpp"


namespace api_client {
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

    size_t getPos(const std::string &url){
        for (int64_t i = 0; i < url.size() - 1; ++i) {
            if (url[i] == '/' && url[i + 1] == '/' && i > 8) {
                return i + 1;
            }
        }
        return -1;
    }

    std::string getItemPrices(const std::string &server, const std::string &itemId, int limit, int offset, std::string bearer){
        std::string url = utils::getAuctionUrl(server, itemId);     

        cpr::Response r = cpr::Get(cpr::Url{url},
                               cpr::Parameters{{"limit", std::to_string(limit)}, {"offset", std::to_string(offset)}},
                               cpr::Bearer{bearer});
        std::cout << r.header["x-ratelimit-remaining"] << std::endl;
        std::cout << r.header["x-ratelimit-reset"] << std::endl;

        return r.text;
    }

    int64_t getItemTotal(const std::string &server, const std::string &itemId, std::string bearer){
        std::string url = utils::getAuctionUrl(server, itemId);     

        cpr::Response response = cpr::Get(cpr::Url{url},
                               cpr::Parameters{{"limit", std::to_string(1)}, {"offset", std::to_string(0)}},
                               cpr::Bearer{bearer});
        auto json_response = json::parse(response.text);
        int64_t total = json_response["total"];
        
        return total;
    }
    
} 
