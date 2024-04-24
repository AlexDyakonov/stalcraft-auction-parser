#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <unistd.h> 
#include <cstring>
#include <filesystem> 
#include <ctime>
#include <stdexcept>
#include "utils.hpp"
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

    std::string getToken(const std::string& server) {
        static const std::map<std::string, std::string> token_map = {
            {"ru", "EXBO_TOKEN_RU"},
            {"sea", "EXBO_TOKEN_SEA"},
            {"na", "EXBO_TOKEN_NA"},
            {"eu", "EXBO_TOKEN_EU"}
        };

        auto it = token_map.find(server);
        if (it != token_map.end()) {
            const char* env_var = std::getenv(it->second.c_str());
            if (env_var) {
                return std::string(env_var);
            } else {
                const char* fallback_var = std::getenv("EXBO_TOKEN");
                if (fallback_var) {
                    return std::string(fallback_var);
                } else {
                    throw std::runtime_error("No valid environment variables found for tokens");
                }
            }
        } else {
            throw std::runtime_error("Invalid server: " + server);
        }
    }

    std::vector<AuctionItem> parseJsonToAuctionItems(const std::string &jsonString, const std::string &server, const std::string &itemId) {
        auto j = json::parse(jsonString);
        std::vector<AuctionItem> items;

        if (j.contains("prices") && j["prices"].is_array()) {
            items.reserve(j["prices"].size());

            for (auto &item : j["prices"]) {
                item["server"] = server;
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

    std::string getAuctionUrl(const std::string& server, const std::string& itemId) {
        std::string url = "https://eapi.stalcraft.net/%server%/auction/%itemId%/history";

        size_t pos = url.find("%server%");
        if (pos != std::string::npos) {
            url.replace(pos, 8, server);
        }

        pos = url.find("%itemId%");
        if (pos != std::string::npos) {
            url.replace(pos, 8, itemId);
        }

        return url;
    }

    std::string getCurrentDate() {
        time_t now = time(0);
        struct tm *timeinfo = localtime(&now);
        char buffer[80];
        strftime(buffer, 80, "%Y-%m-%d", timeinfo);
        return std::string(buffer);
    }

    void writeToSummaryTable(const std::vector<std::string>& lines, bool clearFile = false) {
        const std::string currentDate = getCurrentDate();
        const std::string fileName = "summary_table_" + currentDate + ".csv";

        std::ofstream file(fileName, clearFile ? std::ios::out : (std::ios::out | std::ios::app));
        
        if (!file.is_open()) {
            std::cerr << "Failed to open or create the file: " << fileName << std::endl;
            return;
        }

        for (size_t i = 0; i < lines.size(); ++i) {
            file << lines[i];
            if (i < lines.size() - 1) {
                file << ", ";
            }
        }

        file << std::endl;

        file.close();
    }

}
