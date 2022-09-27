find_package (Boost COMPONENTS system filesystem unit_test_framework REQUIRED)
include_directories (
        ../include
        ${TEST_SOURCE_DIR}/src
        ${Boost_INCLUDE_DIRS}
)
add_definitions (-DBOOST_TEST_DYN_LINK)
add_executable(Test test.cpp)
target_link_libraries(Test PUBLIC ${Boost_LIBRARIES})
add_test (NAME MyTest COMMAND Test)
