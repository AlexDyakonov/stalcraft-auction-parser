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



void daemonize() {
    pid_t pid = fork();

    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS); 
    }

    if (setsid() < 0) { 
        exit(EXIT_FAILURE);
    }

    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    pid = fork(); 

    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS); 
    }

    umask(0); 

    chdir("/"); 

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main(int argc, char* argv[]) {    
    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        daemonize();
    }

    logger = spdlog::basic_logger_mt("logger", "debug.log");
    spdlog::set_default_logger(logger);

    utils::loadEnvVariables(".env");

    cli::CommandLineParser parser;
    parser.parseCommandLineArgs(argc, argv);

    return 0;
}