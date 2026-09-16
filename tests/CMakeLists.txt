add_executable(log_entry_test log_entry_test.cpp)
target_link_libraries(log_entry_test gtest_main)

add_executable(node_test node_test.cpp)
target_link_libraries(node_test raft_core gtest_main)

include(GoogleTest)
gtest_discover_tests(log_entry_test)
gtest_discover_tests(node_test)
