#ifndef LOGGER_MACROS_HPP
#define LOGGER_MACROS_HPP

#include <spdlog/spdlog.h>

#define LOG_INFO(fmt, ...) spdlog::info("{}: " fmt, __FILE__, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) spdlog::warn("{}: " fmt, __FILE__, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) spdlog::error("{}: " fmt, __FILE__, ##__VA_ARGS__)

#endif
