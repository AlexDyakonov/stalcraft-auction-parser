#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <unistd.h> 
#include <cstring>
#include "logger_macros.hpp"

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
}