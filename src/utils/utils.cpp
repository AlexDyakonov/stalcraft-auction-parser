#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <unistd.h> 
#include <cstring>
#include <filesystem> 
#include "logger_macros.hpp"
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "../database/auction_item_repository.hpp"


extern std::shared_ptr<spdlog::logger> logger;

namespace utils {
    void loadEnvVariables(const std::string& relativePath) {
        char cwd[1024]; 
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            std::string fullPath = std::string(cwd) + "/" + relativePath; 

            std::ifstream file(fullPath);
            std::string line;

            if (!file.is_open()) {
                LOG_ERROR("Env file with path: *" + fullPath + "* not found.");
                throw std::runtime_error("Env file with path: *" + fullPath + "* not found.");
            }

            int cnt = 0;

            while (getline(file, line)) {
                std::string::size_type pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);

                    setenv(key.c_str(), value.c_str(), 1);
                    cnt++;
                    // Для Windows:
                    // _putenv_s(key.c_str(), value.c_str());
                }
            }
            LOG_INFO("Successfully got {} envs.", cnt);
            file.close();
        } else {
            LOG_ERROR("Error while retrieving the current working directory");
        }
    }

    std::vector<AuctionItem> parseJsonToAuctionItems(const std::string &jsonString, const std::string &itemId) {
        auto j = json::parse(jsonString);
        std::vector<AuctionItem> items;

        if (j.contains("prices") && j["prices"].is_array()) {
            items.reserve(j["prices"].size());

            for (auto &item : j["prices"]) {
                item["server"] = "ru";
                item["itemId"] = itemId;
                item["time"] = item["time"].get<std::string>().replace(10, 1, " ").replace(19, 1, "");
                items.emplace_back(item);
            }
        }

        return items;
    }


    std::vector<std::string> readIdListFromFile(const std::string& relativePath) {
        std::string absolutePath = std::filesystem::absolute(relativePath);

        std::ifstream file(absolutePath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + absolutePath);
        }

        size_t linesCount = std::count(std::istreambuf_iterator<char>(file),
                                    std::istreambuf_iterator<char>(), '\n');
        file.clear();
        file.seekg(0);

        std::vector<std::string> idList;
        idList.reserve(linesCount);

        std::string line;
        while (getline(file, line)) {
            idList.push_back(line);
        }

        return idList;
    }

}
