/* 
  * PURPOSE     : StalcraftLAB project.
  *               Main startup module.
  * PROGRAMMER  : Dyakonov Alexandr
  * NOTE        : None.
  */

#include <iostream> 
#include <spdlog/sinks/basic_file_sink.h>
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
    logger = spdlog::basic_logger_mt("logger", "debug.log");
    spdlog::set_default_logger(logger);

    utils::loadEnvVariables(".env");

    cli::CommandLineParser parser;
    parser.parseCommandLineArgs(argc, argv);

    return 0;
}