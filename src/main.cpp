#include <iostream>
#include <clickhouse/client.h>
#include "utils/utils.hpp"
extern char **environ;


int main()
{    
    utils::loadEnvVariables(".env");

    std::cout << "Hello" << std::endl;

    clickhouse::ClientOptions options;
    
    options.SetHost(std::getenv("CLICKHOUSE_HOST"))
           .SetPort(std::atoi(std::getenv("CLICKHOUSE_PORT")))
           .SetUser(std::getenv("CLICKHOUSE_USER"))
           .SetPassword(std::getenv("CLICKHOUSE_PASSWORD"))
           .SetDefaultDatabase(std::getenv("CLICKHOUSE_DB"));
    clickhouse::Client client(options);


    try {
        client.Ping();
        std::cout << "Successfully connected to ClickHouse." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
    }


    return 0;
}