enable_testing()

add_executable(derivative-test derivative-test.cpp)
target_link_libraries(derivative-test GTest::gtest_main hugegrad)

add_executable(scalar-test scalar-test.cpp)
target_link_libraries(scalar-test GTest::gtest_main hugegrad)

add_executable(graph-vis-test graph-vis.cpp)
target_link_libraries(graph-vis-test hugegrad)

include(GoogleTest)
gtest_discover_tests(derivative-test)
gtest_discover_tests(scalar-test)
