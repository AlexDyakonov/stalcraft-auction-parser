#ifndef DATABASE_MANAGER_HPP
#define DATABASE_MANAGER_HPP

#include "clickhouse/client.h"
#include <memory>

class DatabaseManager {
public:
    DatabaseManager(); 
    ~DatabaseManager();

    clickhouse::Client* getClient();
    void closeConnection();
    int initializeTables();

private:
    std::unique_ptr<clickhouse::Client> client;
};

#endif