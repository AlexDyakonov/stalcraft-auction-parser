cmake_minimum_required(VERSION 3.12)
project(stalcraft-auction-parser VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)


include(CTest)
enable_testing()

configure_file(${CMAKE_SOURCE_DIR}/.env ${CMAKE_BINARY_DIR}/.env COPYONLY)

add_executable(stalcraft-auction-parser 
    src/main.cpp
    src/utils/utils.cpp
    )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG 1ad1cd2daf138aefdcdb1c35d99608387d230ec1) # Replace with your desired git commit from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

target_link_libraries(stalcraft-auction-parser PRIVATE cpr::cpr)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(stalcraft-auction-parser PRIVATE nlohmann_json::nlohmann_json)

add_subdirectory(contribs/clickhouse-cpp)

target_include_directories(${PROJECT_NAME} PRIVATE contribs/clickhouse-cpp/ contribs/clickhouse-cpp/contrib/absl)

target_link_libraries(${PROJECT_NAME} PRIVATE clickhouse-cpp-lib)