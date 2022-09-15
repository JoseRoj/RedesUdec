cmake_minimum_required(VERSION 3.14)
project(lab1 C)


include(FetchContent)

FetchContent_Declare(Sodium
    GIT_REPOSITORY https://github.com/robinlinden/libsodium-cmake.git
    GIT_TAG 99f14233eab1d4f7f49c2af4ec836f2e701c445e # HEAD as of 2022-05-28
)

set(SODIUM_DISABLE_TESTS ON)
FetchContent_MakeAvailable(Sodium)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(server server.c tcp.c util.c crypto.c)
add_executable(client client.c tcp.c util.c crypto.c)

include_directories(includes)
target_link_libraries(server PRIVATE sodium)
target_link_libraries(client PRIVATE sodium)


