include_directories(../include)
add_executable(backtesting main.cpp)
find_package(Boost REQUIRED COMPONENTS unit_test_framework)
find_package(Boost REQUIRED COMPONENTS date_time)
find_package(fmt REQUIRED)
find_package(etl 20.35.11 REQUIRED)
find_package(OpenMP REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(backtesting PUBLIC ${Boost_LIBRARIES} fmt::fmt OpenMP::OpenMP_CXX PRIVATE etl::etl)

# optimizations
add_definitions(-DNDEBUG) # disables asserts
set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra -ffast-math")