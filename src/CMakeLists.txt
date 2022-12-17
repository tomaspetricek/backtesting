include_directories(../include)
add_executable(backtesting main.cpp)
find_package(Boost REQUIRED COMPONENTS date_time)
find_package(fmt)
find_package(OpenMP REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(backtesting PUBLIC ${Boost_LIBRARIES} fmt::fmt OpenMP::OpenMP_CXX)
# add_definitions(-DNDEBUG) # disables asserts
#set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")