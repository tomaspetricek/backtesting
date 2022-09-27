cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 20)

# choose compiler
# set(CMAKE_CXX_COMPILER /usr/local/opt/llvm/bin/clang++)
# set(CMAKE_CXX_COMPILER /usr/local/bin/g++-12)

# use optimization
# set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")

# https://stackoverflow.com/questions/14446495/cmake-project-structure-with-unit-tests
project (BACKTESTING)
add_subdirectory (src)

enable_testing ()
add_subdirectory (test)