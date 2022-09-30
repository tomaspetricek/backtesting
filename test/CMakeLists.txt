find_package (Boost COMPONENTS system filesystem unit_test_framework date_time REQUIRED)
find_package(fmt)
include_directories (
        ../include
        ${TEST_SOURCE_DIR}/src
        ${Boost_INCLUDE_DIRS}
)
add_definitions (-DBOOST_TEST_DYN_LINK)
add_executable(test_ test.cpp)
target_link_libraries(test_ PUBLIC ${Boost_LIBRARIES} fmt::fmt)
add_test (NAME MyTest COMMAND test_)
