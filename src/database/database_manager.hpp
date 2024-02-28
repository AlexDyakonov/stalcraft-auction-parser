#ifndef DATABASE_MANAGER_HPP
#define DATABASE_MANAGER_HPP

#include "clickhouse/client.h"
#include <memory>
#include "../utils/utils.hpp"

class DatabaseManager {
public:
    DatabaseManager(); 

    clickhouse::Client* getClient();

private:
    std::unique_ptr<clickhouse::Client> client;
};

#endif