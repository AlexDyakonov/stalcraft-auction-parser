#include <iostream>
#include <clickhouse/client.h>
#include "utils/utils.hpp"
extern char **environ;
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

int main()
{    
    auto logger = spdlog::basic_logger_mt("logger", "logfile.log");
    spdlog::set_default_logger(logger);

    spdlog::info("Это информационное сообщение");
    spdlog::warn("Это предупреждение");
    spdlog::error("Это сообщение об ошибке");


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