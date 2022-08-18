cmake_minimum_required(VERSION 3.17)
project(backtesting)

# choose compiler
# set(CMAKE_CXX_COMPILER /usr/local/opt/llvm/bin/clang++)
# set(CMAKE_CXX_COMPILER /usr/local/bin/g++-12)

set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(backtesting src/main.cpp)

# add libraries
find_package(Boost REQUIRED COMPONENTS date_time)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(backtesting PUBLIC ${Boost_LIBRARIES})
