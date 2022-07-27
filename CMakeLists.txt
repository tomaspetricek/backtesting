cmake_minimum_required(VERSION 3.17)
project(EMAStrategy)

set(CMAKE_CXX_COMPILER /usr/bin/g++)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(EMAStrategy src/main.cpp)

# add libraries
find_package(Boost REQUIRED COMPONENTS date_time)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(EMAStrategy PUBLIC ${Boost_LIBRARIES})
