#include <cstdlib> 
#include <stdexcept> 
#include "database_manager.hpp"
#include <iostream>

DatabaseManager::DatabaseManager() {
    try {
        utils::loadEnvVariables(".envasd");

        clickhouse::ClientOptions options;
        options.SetHost(std::getenv("CLICKHOUSE_HOST"))
               .SetPort(std::atoi(std::getenv("CLICKHOUSE_PORT")))
               .SetUser(std::getenv("CLICKHOUSE_USER"))
               .SetPassword(std::getenv("CLICKHOUSE_PASSWORD"))
               .SetDefaultDatabase(std::getenv("CLICKHOUSE_DB"));

        client = std::make_unique<clickhouse::Client>(options);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to connect to ClickHouse database: " + std::string(e.what()));
    }
}

clickhouse::Client* DatabaseManager::getClient() {
    return client.get();
}