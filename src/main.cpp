/* 
  * PURPOSE     : StalcraftLAB project.
  *               Main startup module.
  * PROGRAMMER  : Dyakonov Alexandr
  * NOTE        : None.
  */

#include <iostream> 
#include <spdlog/sinks/basic_file_sink.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <csignal>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>

#include "utils/utils.hpp"
#include "utils/logger_macros.hpp" 
#include "cli/command_line_parser.hpp"

std::shared_ptr<spdlog::logger> logger;


int main(int argc, char* argv[]) {    
    auto console = spdlog::stdout_color_mt("console");    
    auto err_logger = spdlog::stderr_color_mt("stderr");    
    spdlog::get("console")->info("loggers can be retrieved from a global registry using the spdlog::get(logger_name)");

    utils::loadEnvVariables(".env");

    cli::CommandLineParser parser;
    try {
          parser.parseCommandLineArgs(argc, argv);
    } catch(const std::exception& e) {
      SPDLOG_LOGGER_ERROR(err_logger, "Caught exception: {}", e.what());
    }

    return 0;
}