#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <unistd.h> 
#include <cstring>

namespace utils {
    void loadEnvVariables(const std::string& relativePath) {
        char cwd[1024]; 
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            std::string fullPath = std::string(cwd) + "/" + relativePath; 

            std::ifstream file(fullPath);
            std::string line;

            if (!file.is_open()) {
                std::cerr << "Ошибка при открытии файла " << fullPath << std::endl;
                return;
            }

            while (getline(file, line)) {
                std::string::size_type pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);

                    setenv(key.c_str(), value.c_str(), 1);

                    // Для Windows:
                    // _putenv_s(key.c_str(), value.c_str());
                }
            }

            file.close();
        } else {
            std::cerr << "Ошибка при получении текущего рабочего каталога" << std::endl;
        }
    }
}