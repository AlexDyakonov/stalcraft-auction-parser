#include <cstdlib> 
#include <stdexcept> 
#include <iostream>

#include "database_manager.hpp"
#include "../utils/logger_macros.hpp"

extern std::shared_ptr<spdlog::logger> logger;

DatabaseManager::DatabaseManager() {
    try {
        clickhouse::ClientOptions options;
        options.SetHost(std::getenv("CLICKHOUSE_HOST"))
               .SetPort(std::atoi(std::getenv("CLICKHOUSE_PORT")))
               .SetUser(std::getenv("CLICKHOUSE_USER"))
               .SetPassword(std::getenv("CLICKHOUSE_PASSWORD"))
               .SetDefaultDatabase(std::getenv("CLICKHOUSE_DB"));

        client = std::make_unique<clickhouse::Client>(options);
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to connect to ClickHouse database: " + std::string(e.what()));
        throw std::runtime_error("Failed to connect to ClickHouse database: " + std::string(e.what()));
    }
}

clickhouse::Client* DatabaseManager::getClient() {
    return client.get();
}

DatabaseManager::~DatabaseManager() {
    closeConnection();
}

void DatabaseManager::closeConnection() {
    client.reset();
}

std::unique_ptr<clickhouse::Client> DatabaseManager::CreateNewClient() {
    clickhouse::ClientOptions options;
    options.SetHost(std::getenv("CLICKHOUSE_HOST"))
           .SetPort(std::atoi(std::getenv("CLICKHOUSE_PORT")))
           .SetUser(std::getenv("CLICKHOUSE_USER"))
           .SetPassword(std::getenv("CLICKHOUSE_PASSWORD"))
           .SetDefaultDatabase(std::getenv("CLICKHOUSE_DB"));

    return std::make_unique<clickhouse::Client>(options);
}

int DatabaseManager::initializeTables(){
    try {
        auto client = CreateNewClient(); 
        client->Execute("CREATE TABLE IF NOT EXISTS item_auction_info (uuid UUID DEFAULT generateUUIDv4(), itemId String, amount Int32, price Float64, time DateTime, additional String, server String) ENGINE = MergeTree() ORDER BY (itemId, time, uuid);");
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to initialize tables: " + std::string(e.what()));
        return -1;
    }
    return 0;
}
